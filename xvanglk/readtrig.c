
/* Copyright 2016, 2017 Marnix van den Bos */

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


descrInfo *ReadDescr(code)
 int32_t  *code;  /* Contains the description id. */
{
  int32_t       len = 0;
  descrInfo *descr;

  /*  Expected syntax: <description id><len><chars> */

  /* malloc space for descr */
  if ((descr = (descrInfo *) malloc(sizeof(descrInfo))) == NULL) {
    sprintf(outputline, "ReadDescr(): error in malloc.\n");
    Output(outputline);
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
    sprintf(outputline, "ReadDescr(): error in malloc for desc->text.\n");
    Output(outputline);
    return(NULL);
  }
  /* Read the char string. */
  if (fread((void *) descr->text, sizeof(char),len,datafile) != len) {
    sprintf(outputline, "ReadDescr(): error reading character string.\n");
    Output(outputline);
    return(ERROR);
  }

  /* OK, read the next code before returning. */
  GetNextCode32(code);
  return(descr);
}

int32_t ReadAdverbInfo(adverbs)
 adverbInfo *adverbs;
{
  int i=0;

  if (!GetNextCode32(&adverbs->nr_of_adverbs)) {
    sprintf(outputline, "ReadAdverbInfo(): Error reading number of adverbs.\n");
    Output(outputline);
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_ADVERBS; i++) {
    if (!GetNextCode32(&(adverbs->adverb[i]))) {
    sprintf(outputline, "ReadAdverbInfo(): Error reading adverb.\n");
    Output(outputline);
      return(ERROR);
    }
  }

  return(OK);
}

int32_t ReadPreposInfo(prepositions)
 preposInfo *prepositions;
{
  int i=0;

  if (!GetNextCode32(&prepositions->nr_of_prepositions)) {
    sprintf(outputline, "ReadPreposInfo(): Error reading number of prepositions.\n");
    Output(outputline);
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_PREPOS; i++) {
    if (!GetNextCode32(&(prepositions->preposition[i]))) {
    sprintf(outputline, "ReadPreposInfo(): Error reading preposition.\n");
    Output(outputline);
      return(ERROR);
    }
  }

  return(OK);
}

compActionRec *ReadActionRec(code)
 int32_t *code;
{
  compActionRec *action_rec;

  /* Expected syntax: <action_rec> */

  /* Malloc space for action_rec.  */
  if ((action_rec = (compActionRec *)
                            malloc(sizeof(compActionRec))) == NULL) {
    sprintf(outputline, "ReadActionRec(): Error in malloc().\n");
    return(NULL);
  }

  /* Read the action record */
  if (!GetNextCode32(&action_rec->actor)) {
    sprintf(outputline, "ReadActionRec(): Error reading actor.\n");
    Output(outputline);
    return(NULL);
  }
  if (!GetNextCode32(&action_rec->action)) {
    sprintf(outputline, "ReadActionRec(): Error reading action.\n");
    Output(outputline);
    return(NULL);
  }
  if (!ReadAdverbInfo(&action_rec->adverbs))
    return(NULL);

  if (!GetNextCode32(&action_rec->q_word)) {
    sprintf(outputline, "ReadActionRec(): Error reading question word.\n");
    Output(outputline);
    return(NULL);
  }
  if (!GetNextCode32(&action_rec->direction)) {
    sprintf(outputline, "ReadActionRec(): Error reading direction.\n");
    Output(outputline);
    return(NULL);
  }
  if (!GetNextCode32(&action_rec->subject)) {
    sprintf(outputline, "ReadActionRec(): Error reading subject.\n");
    Output(outputline);
    return(NULL);
  }
  if (!GetNextCode32(&action_rec->specifier)) {
    sprintf(outputline, "ReadActionRec(): Error reading specifier.\n");
    Output(outputline);
    return(NULL);
  }
  if (!ReadPreposInfo(&action_rec->prepositions))
    return(NULL);

  if (!GetNextCode32(&action_rec->value)) {
    sprintf(outputline, "ReadActionRec(): Error reading value.\n");
    Output(outputline);
    return(NULL);
  }
  if (!GetNextCode32(&action_rec->ordinal)) {
    sprintf(outputline, "ReadActionRec(): Error reading ordinal value.\n");
    Output(outputline);
    return(NULL);
  }
  if (!GetNextCode32(&(action_rec->execute[0]))) {
    sprintf(outputline, "ReadActionRec(): Error reading trigger.\n");
    Output(outputline);
    return(NULL);
  }
  if (!GetNextCode32(&(action_rec->execute[1]))) {
    sprintf(outputline, "ReadActionRec(): Error reading trigger.\n");
    Output(outputline);
    return(NULL);
  }

  /* Set `next' pointer to NULL. */
  action_rec->next = NULL;

  /* get the next code. */
  if (!GetNextCode32(code))
    return(NULL);

  return(action_rec);
}


int32_t ProcCode(triggercode, pos, code)
 int32_t *triggercode;
 int32_t pos;
 int32_t code;
{
  /* Stores code in array and tests for exceeding array bound. */
  if (pos<MAX_TRIGG_LEN)
    triggercode[pos] = code;
  else {
    sprintf(outputline, "Internal error: triggercode too large.\n");
    Output(outputline);
    return(ERROR);
  }
   return(OK);
}


int32_t *RdTrCode(code)
 int32_t *code;
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
    sprintf(outputline, "RdTrCode(): error in malloc %d.\n", len);
    Output(outputline);
    return(ERROR);
  }

  /* Copy temp_triggercode to triggercode. */
  for (i=0; i<len; i++) {
    triggercode[i] = temp_triggercode[i];
  }

  /* Ready */
  return(triggercode);
}


triggerInfo *ReadTrigger(code)
 int32_t  *code;  /* Contains the trigger id. */
 /* This function reads a trigger that belongs to a location   */
 /* or an object.                                              */
{
  triggerInfo *trigger;

  /* Malloc space for trigger */
  if ((trigger = (triggerInfo *) malloc(sizeof(triggerInfo))) == NULL) {
    sprintf(outputline, "ReadTrigger(): error in malloc for trigger.\n");
    Output(outputline);
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
