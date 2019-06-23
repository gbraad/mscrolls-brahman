
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
#include <stdlib.h>         /* malloc(), free() */
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "readtrig.h"

/*************************/
/* function declarations */
/*************************/

descrInfo     *ReadDescr(int32_t*);
int32_t       ReadAdverbinfo(adverbInfo*);
int32_t       ReadPreposinfo(preposInfo*);
compActionRec *ReadActionRec(int32_t*);
int32_t       ProcCode(int32_t*, int32_t, int32_t);
int32_t       *RdTrCode(int32_t*);
triggerInfo   *ReadTrigger(int32_t*);

/************************/
/* function definitions */
/************************/

descrInfo *ReadDescr(int32_t *code)
 /* Code contains the description id. */
{
  int32_t       len = 0;
  descrInfo *descr;

  /*  Expected syntax: <description id><len><chars> */

  /* malloc space for descr */
  if ((descr = (descrInfo *) malloc(sizeof(descrInfo))) == NULL) {
    PrintError(15, NULL, "ReadDescr()");
    return(NULL);
  }

  /* Next code section added December 18th 2003 */
  /* Init the descrInfo struct                  */
  descr->id   = NO_ID;
  descr->text = "";
  descr->next = NULL;

  descr->id = *code;

  /* Read the length of the char string that follows */
  GetNextCode32(&len);

  /* Malloc space for descr->text. */
  if ((descr->text = (char *) malloc(len*sizeof(char))) == NULL) {
    PrintError(15, NULL, "ReadDescr()");
    return(NULL);
  }
  /* Read the char string. */
  if (fread((void *) descr->text, sizeof(char),len,datafile) != (size_t)len) {
    PrintError(54, NULL, "string");
    return(ERROR);
  }

  /* OK, read the next code before returning. */
  GetNextCode32(code);
  return(descr);
}


int32_t ReadAdverbInfo(adverbInfo *adverbs)
{
  int i=0;

  if (!GetNextCode32(&adverbs->nr_of_adverbs)) {
    PrintError(14, NULL, "ReadAdverbInfo()");
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_ADVERBS; i++) {
    if (!GetNextCode32(&(adverbs->adverb[i]))) {
      PrintError(14, NULL, "ReadAdverbInfo()");
      return(ERROR);
    }
  }

  return(OK);
}


int32_t ReadPreposInfo(preposInfo *prepositions)
{
  int i=0;

  if (!GetNextCode32(&prepositions->nr_of_prepositions)) {
    PrintError(14, NULL, " ReadPreposInfo()");
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_PREPOS; i++) {
    if (!GetNextCode32(&(prepositions->preposition[i]))) {
      PrintError(14, NULL, " ReadPreposInfo()");
      return(ERROR);
    }
  }

  return(OK);
}


compActionRec *ReadActionRec(int32_t *code)
{
  compActionRec *action_rec;

  /* Expected syntax: <action_rec> */

  /* Malloc space for action_rec.  */
  if ((action_rec = (compActionRec *)
                            malloc(sizeof(compActionRec))) == NULL) {
    PrintError(15, NULL, "ReadActionRec()");
    return(NULL);
  }

  /* Read the action record */
  if (!GetNextCode32(&action_rec->actor)) {
    PrintError(14, NULL, " ReadActionRec()");
    return(NULL);
  }
  if (!GetNextCode32(&action_rec->action1)) {
    PrintError(14, NULL, " ReadActionRec()");
    return(NULL);
  }
  if (!GetNextCode32(&action_rec->action2)) {
    PrintError(14, NULL, " ReadActionRec()");
    return(NULL);
  }
  if (!ReadAdverbInfo(&action_rec->adverbs))
    return(NULL);

  if (!GetNextCode32(&action_rec->q_word)) {
    PrintError(14, NULL, " ReadActionRec()");
    return(NULL);
  }
  if (!GetNextCode32(&action_rec->direction)) {
    PrintError(14, NULL, " ReadActionRec()");
    return(NULL);
  }
  if (!GetNextCode32(&action_rec->subject)) {
    PrintError(14, NULL, " ReadActionRec()");
    return(NULL);
  }
  if (!GetNextCode32(&action_rec->specifier)) {
    PrintError(14, NULL, " ReadActionRec()");
    return(NULL);
  }
  if (!ReadPreposInfo(&action_rec->prepositions))
    return(NULL);

  if (!GetNextCode32(&action_rec->value)) {
    PrintError(14, NULL, " ReadActionRec()");
    return(NULL);
  }
  if (!GetNextCode32(&action_rec->ordinal)) {
    PrintError(14, NULL, " ReadActionRec()");
    return(NULL);
  }
  if (!GetNextCode32(&(action_rec->execute[0]))) {
    PrintError(14, NULL, " ReadActionRec()");
    return(NULL);
  }
  if (!GetNextCode32(&(action_rec->execute[1]))) {
    PrintError(14, NULL, " ReadActionRec()");
    return(NULL);
  }

  /* Set `next' pointer to NULL. */
  action_rec->next = NULL;

  /* get the next code. */
  if (!GetNextCode32(code))
    return(NULL);

  return(action_rec);
}


int32_t ProcCode(int32_t *triggercode, int32_t pos, int32_t code)
{
  /* Stores code in array and tests for exceeding array bound. */
  if (pos<MAX_TRIGG_LEN)
    triggercode[pos] = code;
  else {
    PrintError(55, NULL, NULL);
    return(ERROR);
  }
   return(OK);
}


int32_t *RdTrCode(int32_t *code)
{
  /* Create temporary space for trigger code.                  */
  /* As soon as we know the exact size, we will malloc space.  */

  int32_t len           = 0;
  int32_t i             = 0;
  int32_t cont          = 1;  /* Tells when to quit while-loop.    */

  int32_t temp_triggercode[MAX_TRIGG_LEN];
  int32_t *triggercode;                      /* Permanent trigger. */

  while (cont) {
    switch (*code) {
      case ERROR:
        return(NULL);
      case IF:
        if (!ProcCode(temp_triggercode, len++, IF))
          return(NULL);

        GetNextCode32(code);
        break;
      case THEN:
        if (!ProcCode(temp_triggercode, len++, THEN))
          return(NULL);

        GetNextCode32(code);
        break;
      case ELSE:
        if (!ProcCode(temp_triggercode, len++, ELSE))
          return(NULL);

        GetNextCode32(code);
        break;
      case AND:
      case OR:
      case NOT:
        if (!ProcCode(temp_triggercode, len++, *code))
          return(NULL);

        if (!GetNextCode32(code))
          return(NULL);
        break;
      case ENDIF:
        if (!ProcCode(temp_triggercode, len++, ENDIF))
          return(NULL);

        GetNextCode32(code);
        break;
      default:
        if (IsTestFun(*code) || IsIntAct(*code) ||
                     IsCAttrId(*code) || IsLAttrId(*code)) {
          if (!ReadFun(temp_triggercode, &len, *code))
            return(NULL);
          GetNextCode32(code);
        }
        else {
          /* Let calling function handle this one. */
          cont = 0;
        }

        break;
    } /* switch */
  } /* while */

  /* copy temp_trig_code to a permanent trig */
  /* Add END_OF_CODE */
  if (!ProcCode(temp_triggercode, len++, END_OF_CODE))
    return(NULL);

  /* malloc space for trigger->code. */
  if ((triggercode = (int32_t *) malloc(len*sizeof(int32_t))) == NULL) {
    PrintError(15, NULL, "RdTrCode()");
    return(ERROR);
  }

  /* Copy temp_triggercode to triggercode. */
  for (i=0; i<len; i++) {
    triggercode[i] = temp_triggercode[i];
  }

  /* Ready */
  return(triggercode);
}


triggerInfo *ReadTrigger(int32_t *code)
 /* Code contains the trigger id. */
{
  /* This function reads a trigger that belongs to a location   */
  /* or an object.                                              */

  triggerInfo *trigger;

  /* Malloc space for trigger */
  if ((trigger = (triggerInfo *) malloc(sizeof(triggerInfo))) == NULL) {
    PrintError(15, NULL, "ReadTrigger()");
    return(NULL);
  }

  trigger->id   = *code;
  trigger->next = NULL;

  GetNextCode32(code);

  if ((trigger->code = RdTrCode(code)) == NULL)
    return(NULL);

  /* Ok, we're ready. */
  return(trigger);
}
