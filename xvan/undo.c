
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
/*                                                                      */
/* <marnix@xvan.nl>                                                     */
/*                                                                      */
/* This file is part of XVAN, an Interactive Fiction authoring system.  */
/*                                                                      */
/* XVAN is free software: you can redistribute it and/or modify         */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, version 3.                             */
/*                                                                      */
/* XVAN is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with XVAN.  If not, see <http://www.gnu.org/licenses/>.        */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/* Undo functionality can take up lots of memory, depending on how many */
/* turns must be reverted. XVAN reserves a circular buffer for storing  */
/* undo information. THe size of the buffer is determined based on the  */
/* number of locations and objects ('worst'case, an action with scope   */
/* ALL_LOCS can change all locations and objects).                      */
/* The undo buffer is circular, meaning that it will be overwritten     */
/* when it gets full.                                                   */
/* The undo history is not saved with a save game                       */
/*                                                                      */
/* During the execution of a turn, the following information is stored  */
/* in the undo buffer:                                                  */
/*  - changes to flag values                                            */
/*  - changes to attribute values                                       */
/*  - changes to ownership of objects                                   */
/*  - changes to exits                                                  */
/*  - changes to timers                                                 */
/*  - changes to special id values (there are 10 special ids)           */
/*                                                                      */
/* Following information is not stored in the undo buffer               */
/*  - transcript mode                                                   */
/************************************************************************/


#include <stdio.h>
#include <stdlib.h>         /* malloc(), free() */
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "undo.h"

/***********/
/* globals */
/***********/
extern char *outputbuffer;
int32_t     undo_record_counter = 0;
int32_t     undo_sp             = 0;
int32_t     write_undo          = 0; /* don't write before first move */
int32_t     undo_stack_size     = 0;
undoItem    *undo_stack         = NULL;

/*************************/
/* Function declarations */
/*************************/

void    PrintUndoStack(void);
void    EraseUndoTurn(void);
void    EraseUndoItem(int32_t);
int32_t InitUndoStack();
void    PushUndoItem(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
void    PopUndoItem(int32_t*, int32_t*, int32_t*, int32_t*, int32_t*, int32_t*);
int32_t XeqUndo(int32_t**);

/************************/
/* Function definitions */
/************************/

void EraseUndoItem(int32_t index)
{
  undo_stack[index].item1 = NO_ID;
  undo_stack[index].item2 = NO_ID;
  undo_stack[index].item3 = NO_ID;
  undo_stack[index].item4 = NO_ID;
  undo_stack[index].item5 = NO_ID;
  undo_stack[index].value = 0;
}


void EraseUndoTurn()
{
  int32_t i = undo_sp;

  /* this function erases all undo items from the    */
  /* current undo_sp position back to the next EOU   */

  /* undo_sp points to the last filled item (unless) */
  /* the undo stack is empty)                        */

  while (undo_stack[i].item1 != EOU) {
    EraseUndoItem(i);
    i++;
    if (i == undo_stack_size) {
      /* roll over */
      i = 0;
    }
  }
  /* erase the EOU as well */
  EraseUndoItem(i);
}


int32_t InitUndoStack()
{
  if (undo_stack == NULL) {
    /* determine undo stack size */
    undo_stack_size = (nr_of_locs + nr_of_objs + nr_of_timers) * 15;
    undo_stack_size = (undo_stack_size > MAX_UNDO_STACK_SIZE ? MAX_UNDO_STACK_SIZE : undo_stack_size);

    if ( (undo_stack = (undoItem *) malloc(undo_stack_size*sizeof(undoItem))) == NULL) {
      PrintError(15, NULL, "InitUndoStack()");
      return(ERROR);
    }
  }

  /* we have the undo stack */
  for (undo_sp = 0; undo_sp < undo_stack_size; undo_sp++) {
    undo_stack[undo_sp].item1 = NO_ID;
    undo_stack[undo_sp].item2 = NO_ID;
    undo_stack[undo_sp].item3 = NO_ID;
    undo_stack[undo_sp].item4 = NO_ID;
    undo_stack[undo_sp].item5 = NO_ID;
    undo_stack[undo_sp].value = 0;
  }

  undo_sp             = -1;  /* we increment before the push */
  undo_record_counter = 0;

  return(OK);
}


void PushUndoItem(int32_t item1, int32_t item2, int32_t item3, int32_t item4, int32_t item5, int32_t value)
{
  /* this function writes one undo item  to the undo stack,  */
  /* so we must check if there is room for one item          */

  /* check for an overflow, meaning that there are more undo */
  /* records for one move than can fit in the undo stack     */
  /* record _counter counts all undo items for one turn. If  */
  /* not all items fit on the stack, the stack is cleared    */
  /* and we loose the undo history                           */

  /* the interpreter always writes an EOU after each turn.   */
  /* if a turn has no changes to the undo stack (e.g.        */
  /* 'restore', there will be two consecutive EOU records,   */
  /* which takes up unnecessary space. So, when PushUndoItem */
  /* must push an EOU, it will check if the current position */
  /* is an EOU and if so, it will write nothing to the stack */

  /* undo_sp always points to the last filled position in    */
  /* the undo stack (unless the stack is empty, of course)   */

  /* check if we had an earlier error during this move       */
  if (!write_undo) {
    return;
  }

  /* check for 'empty' turn (no undo items written) */
  /* empty turn means current stack item is EOU or  */
  /* NO_ID (initialized stack)                      */
  if (item1 == EOU) {
    /* C-standard says that in case undo_sp equals -1, the other 2 operands will */
    /* not be evaluated. If not, we have a problem because we index with -1 then */
    if (undo_sp == -1 || undo_stack[undo_sp].item1 == EOU || undo_stack[undo_sp].item1 == NO_ID) {
      /* yes, empty turn, do nothing */
      return;
    }
  }

  /* throw an error if item equals NO_ID */
  /* this should never happen            */
  if (item1 == NO_ID) {
    PrintError(109, NULL, NULL);

    /* wipe the undo stack and continue  */
    InitUndoStack();
    write_undo = 0;
    return;
  }

  if (undo_record_counter == undo_stack_size) {
    /* there will be an undo-buffer overflow */
    PrintError(108, NULL, NULL);

    /* wipe the undo stack and continue  */
    InitUndoStack();
    write_undo = 0;
    return;
  }

  if (++undo_sp == undo_stack_size) {
    /* roll over the stack */
    undo_sp = 0;
  }
  /*else {
    undo_sp++;
  }*/

  /* as long as undo_stack[undo_sp] equals NO_ID we are safe */
  /* if not, we are overwriting an earlier turn and we must  */
  /* first blank out all items of that turn to prevent that  */
  /* we end up with partial undos on the undo stack          */

  if (undo_stack[undo_sp].item1 != NO_ID) {
    EraseUndoTurn();
  }

  undo_stack[undo_sp].item1 = item1;
  undo_stack[undo_sp].item2 = item2;
  undo_stack[undo_sp].item3 = item3;
  undo_stack[undo_sp].item4 = item4;
  undo_stack[undo_sp].item5 = item5;
  undo_stack[undo_sp].value = value;

  undo_record_counter++;

  return;
}


void PopUndoItem(int32_t *item1, int32_t *item2, int32_t *item3, int32_t *item4, int32_t *item5, int32_t *value)
{
  /* undo_sp points to the last written item. Unless */
  /* the undo stack is empty (in which case undo_sp  */
  /* will be -1                                      */
  if (undo_sp == -1) {
    /* tell caller that stack is empty */
    *item1 = NO_ID;
    return;
  }

  *item1 = undo_stack[undo_sp].item1;
  *item2 = undo_stack[undo_sp].item2;
  *item3 = undo_stack[undo_sp].item3;
  *item4 = undo_stack[undo_sp].item4;
  *item5 = undo_stack[undo_sp].item5;
  *value = undo_stack[undo_sp].value;

  /* check if there is data on the undo stack */
  if (*item1 != NO_ID) {
    /* there is data on the undo stack */
    /* clear the current undo item     */
    EraseUndoItem(undo_sp);

    /* adjust the undo stack pointer */
    if (--undo_sp < 0) {
      /* roll over */
     undo_sp = undo_stack_size - 1;
    }
  }
  return;
}

