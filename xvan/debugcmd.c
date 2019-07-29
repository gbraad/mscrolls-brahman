
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018, 2019 Marnix van den Bos.             */
/*                                                                      */
/* <marnix.home@gmail.com>                                              */
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


#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "debugcmd.h"

/* this file contains debug functions for the game author. */
/* debug functions that I use myself when developing the   */
/* code are in file debugcmd.c                             */


/***********/
/* Globals */
/***********/

int debug_indent = 0;

debugInfo *loc_dbug       = NULL;
debugInfo *obj_dbug       = NULL;
debugInfo *com_attr_dbug  = NULL;
debugInfo *loc_attr_dbug  = NULL;
debugInfo *com_flag_dbug  = NULL;
debugInfo *loc_flag_dbug  = NULL;
debugInfo *com_trig_dbug  = NULL;
debugInfo *loc_trig_dbug  = NULL;
debugInfo *timer_dbug     = NULL;

/*************************/
/* Function declarations */
/*************************/

void DebugLevel_1(short, char*, int32_t, int32_t, resultStruct);
void Debuglevel_2_pars(char*, resultStruct*, int);
void DebugLevel_2_result(resultStruct);
void PrintDebugParList(resultStruct*, int);
void PrintDebugResult(resultStruct);
void PrintDebugLocObj(int32_t);
void IncreaseDebugIndent(int);
void PrintDebugIndent(void);

/************************/
/* Function definitions */
/************************/

void DebugLevel_1(short in, char *text, int32_t id1, int32_t id2, resultStruct result)
{
  /* check if we must print debug info */
  if (!(debug_level > 0))
    return;

  /* next, we have a verb id, a trigger id or a timer id */

  PrintString("\n", 0);  /* make sure we start on a new line */
  PrintDebugIndent();
  PrintString(text, 0);

  /* in = 1 means we are entering the trigger  */
  /* in = 2 means we have executed the trigger */

  if (IsVerbId(id1)) {
    PrintId(id1, 0);
    if (in == 0) {
      PrintString(" returned ", 0);
      PrintDebugResult(result);
    }
    PrintString(".\n", 0);
    Output();
    return;
  }

  if (IsCTriggId(id1)) {
    PrintString(com_trig_dbug[id1-FIRST_COMMON_TRIGGER_ID].name, 0);
    PrintString(" for ", 0);
    PrintDebugLocObj(id2);
    if (in == 0) {
      PrintString(" returned ", 0);
      PrintDebugResult(result);
    }
    PrintString(".\n", 0);
    Output();
    return;
  }

  if (IsLTriggId(id1)) {
    PrintString(loc_trig_dbug[id1-FIRST_LOCAL_TRIGGER_ID].name, 0);
    PrintString(" for ", 0);
    PrintDebugLocObj(id2);
    if (in == 0) {
      PrintString(" returned ", 0);
      PrintDebugResult(result);
    }
    PrintString(".\n", 0);
    Output();
    return;
  }

  if (IsTimerId(id1)) {
    PrintString("m_", 0);
    PrintString(timer_dbug[id1 - FIRST_TIMER_ID].name, 0);
    PrintString(" has fired.\n", 0);
    Output();
    return;
  }

  /* unknown id */
  PrintString("DebugLevel_1(): unknown id.\n", 0);
  Output();
  return;
}


void DebugLevel_2_pars(char *function, resultStruct *par_list, int nr_pars)
{
  /* check if we must print debug info */
  if (!(debug_level > 1))
    return;

  PrintDebugIndent();
  PrintString("  **** Function: ", 0);  /* or use text_to_print var */
  PrintString(function, 0);
  PrintString("\n", 0);
  if (nr_pars == 0) {
    PrintDebugIndent();
    PrintString("   (no parameters)\n", 0);
  }
  else {
   PrintDebugParList(par_list, nr_pars);
  }
  Output();
  return;
}


void DebugLevel_2_result(resultStruct result)
{
  /* check if we must print debug info */
  if (!(debug_level > 1))
    return;

  PrintDebugIndent();
  PrintString("   result: ", 0);
  PrintDebugResult(result);
  PrintString("\n\n", 0);
  Output();
  return;
}


void PrintDebugParList(resultStruct *par_list, int nr_pars)
{
  int i = 0;

  for (i=0; i<nr_pars; i++) {
    PrintDebugIndent();
    PrintString("   parameter ", 0);
    PrintNumber(i+1, 0);
    PrintString(": ", 0);
    PrintDebugResult(par_list[i]);
    PrintString("\n", 0);
  }
  return;
}


void PrintDebugResult(resultStruct result)
{
  switch(result.tag) {
    case AGREE:
      PrintString("AGREE", 0);
      break;
    case DISAGREE:
      PrintString("DISAGREE", 0);
      break;
    case NO_MATCH:
      PrintString("NO_MATCH", 0);
      break;
    case QUIT:
      PrintString("QUIT", 0);
      break;
    case CONTINUE:
      PrintString("CONTINUE", 0);
      break;
    case OK:
      PrintString("OK", 0);
      break;
    case ERROR:
      PrintString("ERROR", 0);
      break;
    case LOC_ID: ;
    case OBJ_ID:
      PrintDebugLocObj(result.value);
      break;
    case NUMBER:
      PrintNumber(result.value, 0);
      break;
    case TRIGG_ID:
    case COMMON_TRIGGERS:
      if (IsCTriggId(result.value)) {
        PrintDebugLocObj(result.owner);
        PrintString(".", 0);
        PrintString("t_", 0);
        PrintString(com_trig_dbug[result.value-FIRST_COMMON_TRIGGER_ID].name, 0);
      }
      else {
        if (IsLTriggId(result.value)) {
          PrintDebugLocObj(loc_trig_dbug[result.value-FIRST_LOCAL_TRIGGER_ID].owner);
          PrintString(".", 0);
          PrintString("t_", 0);
          PrintString(loc_trig_dbug[result.value-FIRST_LOCAL_TRIGGER_ID].name, 0);
        }
      }
      break;
    case FLAG_ID: ;
    case COMMON_FLAGS:
      /* print the flag name, not its value */
      if (IsCFlagId(result.value)) {
        PrintDebugLocObj(result.owner);
        PrintString(".", 0);
        PrintString("f_", 0);
        PrintString(com_flag_dbug[result.value-FIRST_COMMON_FLAG_ID].name, 0);
        PrintString(" with value: ", 0);
        if (TestCFlag(result.owner, result.value))
          PrintNumber(1, 0);
        else
          PrintNumber(0, 0);
      }
      else {
        if (IsLFlagId(result.value)) {
          PrintDebugLocObj(loc_flag_dbug[result.value-FIRST_LOCAL_FLAG_ID].owner);
          PrintString(".", 0);
          PrintString("f_", 0);
          PrintString(loc_flag_dbug[result.value-FIRST_LOCAL_FLAG_ID].name, 0);
          PrintString(" with value: ", 0);
          if (TestLFlag(result.value))
            PrintNumber(1, 0);
          else
            PrintNumber(0, 0);
        }
        else {
          PrintString("(unknown flag id)", 0);
        }
      }
      break;
    case DIR: ;
    case WORD_ID:
      PrintWord(result.value, 0);
      break;
    case ATTR_ID:
      /* print the attribute name, not its value */
      if (IsCAttrId(result.value)) {
        PrintDebugLocObj(result.owner);
        PrintString(".", 0);
        PrintString(com_attr_dbug[result.value-FIRST_COMMON_ATTR_ID].name, 0);
      }
      else {
        if (IsLAttrId(result.value)) {
          PrintDebugLocObj(loc_attr_dbug[result.value-FIRST_LOCAL_ATTR_ID].owner);
          PrintString(".", 0);
          PrintString(loc_attr_dbug[result.value-FIRST_LOCAL_ATTR_ID].name, 0);
        }
        else {
          PrintString("(unknown attribute id)", 0);
        }
      }
      PrintString("\n", 0);
      break;

    default:
      PrintString("(unknown type) ", 0);
      PrintNumber(result.value, 0);
      break;
  }
  return;
}

void PrintDebugLocObj(int32_t id)
{
  /* this function prints the location or object that is */
  /* in the id parIt does not print the friendly name    */
  /* (d_sys) but the identifier (l_location or o_object  */

  /* id may be NONE. This happens when we have a wildcard */
  /* that converted to NO_ID. GetPar() will return NONE   */
  /* but with the original type.                          */
  if (id == NONE) {
    PrintString("NONE", 0);
    return;
  }

  if (IsLocId(id)) {
    PrintString("l_", 0);
    PrintString(loc_dbug[id-FIRST_LOCATION_ID].name, 0);
  }
  else {
    if (IsObjId(id)) {
      PrintString("o_", 0);
      PrintString(obj_dbug[id-FIRST_OBJECT_ID].name, 0);
    }
    else {
      PrintString("<invalid id>", 0);
    }
  }
}

void IncreaseDebugIndent(int delta)
{
  if (debug_level < 1 ) {
    return;
  }

  if ( (debug_indent += delta) < 0) {
    PrintError(103, NULL, NULL);
    /* not a severe error */
    debug_indent = 0;
    return;
  }
}


void PrintDebugIndent(void)
{
  /* this function is called increase the indent, so it is */
  /* easier to see by which trigger a function is executed */

  int i = 0;

  /* check if we must print debug info */
  if (debug_level < 1) {
    return;
  }

  for (i=0; i<debug_indent; i++) {
    PrintString(" ", 0);
  }
}
