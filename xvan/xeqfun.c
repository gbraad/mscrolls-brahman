
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

/********************************************************************/
/* This module contains the code for the functions that the user    */
/* may use in his sourcefile. This code will use so called system   */
/* functions, which cannot be used by the programmer (Pop(), etc.). */
/********************************************************************/

/********************************************************************/
/* 8 december 2003: all occurrences of external variable            */
/* nr_of_locations have been renamed to nr_of_locs (4 occurrences). */
/********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "json.h"      /* for key length definition */
#include "typedefs.h"
#include "IFI.h"
#include "xeqfun.h"

/*************************/
/* Function declarations */
/*************************/

int32_t XeqCanSee(int32_t**);
int32_t XeqExecute(int32_t**, usrActionRec*, int32_t); /* Is called `trigger' in story source. */
int32_t XeqExit(int32_t**, usrActionRec*, int32_t);
int32_t XeqIsLit(int32_t**);
int32_t XeqIsObject(int32_t**);
int32_t XeqOwns(int32_t**);
int32_t XeqRunVerb(int32_t**, usrActionRec*, int32_t);              /* 06oct2017 */
int32_t XeqRunCommon(int32_t**, usrActionRec*, int32_t, int32_t*);  /* 06oct2017 */
int32_t XeqTstFlag(int32_t**);
int32_t XeqTestFun(int32_t, int32_t**, usrActionRec*, int32_t, int32_t*);
int32_t XeqValDir(int32_t**);
int32_t XeqYesNo(int32_t**);
int32_t XeqTry(int32_t**);

int32_t XeqAddJson(int32_t**);
int32_t XeqAgree(int32_t**);
int32_t XeqBlockExit(int32_t**);
int32_t XeqBackground(int32_t**);
int32_t XeqBold(int32_t**);
int32_t XeqClearJson(int32_t**);
int32_t XeqDebug(int32_t**);
int32_t XeqDest(int32_t**);
int32_t XeqDisagree(int32_t**);
int32_t XeqDistance(int32_t**, int32_t);
int32_t XeqGetSubject(int32_t**);
int32_t XeqGetSpec(int32_t**);
int32_t XeqGetAnswer(int32_t**);
int32_t XeqGoTo(int32_t**);
int32_t XeqCount(int32_t**);
int32_t XeqHitAnyKey(int32_t**);
int32_t XeqItalic(int32_t**);
int32_t XeqOwner(int32_t**);
int32_t XeqNoMatch(int32_t**);
int32_t XeqContents(int32_t**, usrActionRec*, int32_t);
int32_t XeqEntrance(int32_t**, usrActionRec*, int32_t);
int32_t XeqMove(int32_t**);
int32_t XeqNewExit(int32_t**);
int32_t XeqNoTimers(int32_t**);
int32_t XeqIndent(int32_t**);
int32_t XeqPrt(int32_t**);
int32_t XeqPrtcr(int32_t**);
int32_t XeqPrtStat(int32_t**);
int32_t XeqSetCursor(int32_t**);
int32_t XeqClearWindow(int32_t**, int);
int32_t XeqQuit(int32_t**);
int32_t XeqRestart(int32_t**);          /* oct 18 18 */
int32_t XeqScore(int32_t**);
int32_t XeqSendJson(int32_t**);
int32_t XeqSetAttribute(int32_t**);
int32_t XeqFlagVal(int32_t**, int32_t);
int32_t XeqShuffle(int32_t**);
int32_t XeqStartTimer(int32_t**);
int32_t XeqStopTimer(int32_t**);
int32_t XeqSynchro(int32_t**, usrActionRec*, int32_t);
int32_t XeqText(int32_t**);
int32_t XeqUnderline(int32_t**);
int32_t XeqWait(int32_t**, usrActionRec*, int32_t);
int32_t XeqSave(int32_t**);			    /* dec 21 07 */
int32_t XeqRestore(int32_t**);			/* dec 21 07 */
int32_t XeqTestmode(int32_t**);
int32_t XeqTranscript(int32_t**);
int32_t XeqIntAct(int32_t, int32_t**, usrActionRec*, int32_t);

/****************************/
/* Testfunction definitions */
/****************************/

int32_t XeqCanSee(trigger)
 int32_t **trigger;
{
  int32_t owner;  /* dummy */
  char    *str;   /* dummy */
  int32_t par1;
  int32_t par2;   /* CanSee() always has two parameters. */
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Read parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  if (par1 == NONE || par2 == NONE) {
    PrintError(78, NULL, "XeqCanSee()");
    return(QUIT);
  }

  if (CheckPars(CANSEE, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE))
    return(CanSee(par1, par2));
  else
    return(QUIT);
}


int32_t XeqExecute(trigger, action_rec, subject_index)
 int32_t      **trigger;
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  /* The corresponding XVAN-function is called `trigger',   */
  /* because EXECUTE is not an internal action in keyword.h */

  int32_t owner;
  char    *str;  /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;
  int32_t result;

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(TRIGGER, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result = XeqTrigger(owner, par, action_rec, subject_index);

    /* XeqTrigger() returns either AGREE, DISAGREE, NO_MATCH or */
    /* QUIT. However, in this case, the trigger is executed as  */
    /* part of another trigger. Therefore we must handle it as  */
    /* one instruction. If the trigger returns QUIT, this       */
    /* routine will return QUIT. Otherwise it will return       */
    /* CONTINUE.                                                */

    switch (result) {
      case AGREE:
      case NO_MATCH:
        return(OK);
        break;

      case DISAGREE:
        return(ERROR);
        break;

      case QUIT:
        return(QUIT);
        break;

      default:
        PrintError(79, &((resultStruct) {VALUE,result}), "XeqExecute()");
        return(QUIT);
        break;
    }
    /* return(result != QUIT? CONTINUE : result); */
  }
  else
    return(QUIT);
}


int32_t XeqExit(trigger, action_rec, subject_index)
 int32_t **trigger;
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  /* exit(loc/obj) */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par;
  int32_t type  = NO_TYPE;

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(ERROR);

  if (CheckPars(EXIT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (Exit(par, action_rec, subject_index) == AGREE)
      return(OK);
    else
      return(ERROR);
  }
  else
    return(QUIT);
}


int32_t XeqIsLit(trigger)
 int32_t **trigger;
{
  /* IsLit(loc/obj) */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(ISLIT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE))
    return(IsLit(par));
  else
    return(QUIT);
}


int32_t XeqIsObject(trigger)
 int32_t **trigger;
{
  /* IsObject(loc/obj) */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(ISOBJECT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE))
    return(IsObjId(par));
  else
    return(QUIT);
}


int32_t XeqOwns(trigger)
 int32_t **trigger;
{
  int32_t  owner; /* dummy */
  char *str;  /* dummy. */
  int32_t  par1;
  int32_t  par2;  /* Owns() has either 2, 3 or 4 parameters. */
  int32_t  par3;
  int32_t  depth = 1;           /* preposition may be par3 or par4 */
  int32_t  preposition = NO_ID; /* preposition may be par3 or par4 */
  int32_t  nr_of_pars;
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;
  int32_t  type3 = NO_TYPE;
  int32_t  type4 = NO_TYPE;
  int      index = 0;

  /* Read number of parameters. */
  nr_of_pars = NextOpcode(trigger);

  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  if(!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  if (par1 == NONE || par2 == NONE) {
    PrintError(78, NULL, "XeqOwns()");
    return(QUIT);
  }

  if (nr_of_pars == 4) {
    /* a number and a word id */
    if (!GetPar(&owner, &depth, &type3, &str, trigger)) {
      return(QUIT);
    }
    if (!GetPar(&owner, &preposition, &type4, &str, trigger)) {
      return(QUIT);
    }
    /* help checkpars a bit (see also comments in XeqMove() */
    if (IsWordId(preposition)) {
      type4 = WORD_ID;
    }
  }
  else {
    if (nr_of_pars == 3) {
      /* third parameter is a number or a word (preposition)  */
      if (!GetPar(&owner, &par3, &type3, &str, trigger)) {
        return(QUIT);
      }
      /* help checkpars a bit (see also comments in XeqMove() */
      if (IsWordId(par3)) {
        preposition = par3;
        type4       = WORD_ID;
        depth       = 1;
        type3       = NUMBER;
      }
      else {
        depth = par3;
      }
    }
    else {
      /* there are only 2 parameters, so set */
      /* containment level to 1              */
      depth = 1;
      type3 = NUMBER;
    }
  }

  /* We now must test if par1 owns (contains) par2. */
  /* The depth of the containment may be max par3.  */
  if (CheckPars(OWNS, type1, type2, type3, type4, NO_TYPE)) {
    if (Owns(par1, par2, depth)) {
      /* par1 owns par2, now check if the prepositions match */
      /* r_preposotion has id FIRST_COMMON_ATTR_ID.          */
      /* This is set at compile time.                        */
      index = (par2-FIRST_OBJECT_ID)*nr_of_cattrs;
      if (preposition == NO_ID || c_obj_attrs[index].value == preposition) {
        return(OK);
      }
      else {
        return(ERROR);
      }
      return(OK);
    }
    else {
      return(ERROR);
    }
    return(OK);
  }
  else
    return(QUIT);
}


int32_t XeqRunVerb(trigger, action_rec, subject_index)
 int32_t      **trigger;
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  int32_t result;

  /* RunVerb() always has 0 parameters.   */
  /* Syntax in triggercode: RUNVERB 0     */

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  /* XeqVerbDefault() returns either AGREE, DISAGREE, NO_MATCH */
  /* or QUIT. However, in this case, the trigger is executed   */
  /* as part of another trigger. Therefore we must handle it   */
  /* as one instruction. If the trigger returns QUIT, this     */
  /* routine will return QUIT. Otherwise it will return        */
  /* CONTINUE.                                                 */

  result = XeqVerbDefault(action_rec, subject_index);

  switch (result) {
    case AGREE: ;
    case NO_MATCH:
      return(OK);
      break;
    case DISAGREE:
      return(ERROR);
      break;
    case QUIT:
      return(QUIT);
      break;
    default:
      PrintError(79, &((resultStruct) {VALUE,result}), "XeqRunVerb()");
      return(QUIT);
      break;
  }
}


int32_t XeqRunCommon(trigger, action_rec, subject_index, com_trig)
 int32_t      **trigger;
 usrActionRec *action_rec;
 int32_t      subject_index;
 int32_t      *com_trig;
{
  int32_t result;

  /* RunCommon() always has 0 parameters. */
  /* Syntax in triggercode: RUNCOMMON 0   */
  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  /* NULL value for com_trig means there is no common trigger.  */
  /* Possible causes:                                           */
  /* 1. calling runverb() from a local trigger that has no      */
  /*    common trigger                                          */
  /* 2. calling runcommon() from a common trigger               */
  /* 3. calling runcommon() from verb default code              */
  /* wrt. 2 and 3: runverb cannot be called directly from verb  */
  /* or common trigger code (the compiler checks for this), but */
  /* it can be called indirectly when verb code or common       */
  /* trigger code calls another trigger through the trigger()   */
  /* function. We only detect this at runtime.                  */

  if (com_trig == NULL) {
    PrintError(80, NULL, NULL);
    return(QUIT);
  }

  result = Execute(com_trig, action_rec, subject_index, NULL);

  switch (result) {
    case AGREE: ;
    case NO_MATCH:
      return(OK);
      break;
    case DISAGREE:
      return(ERROR);
      break;
    case QUIT:
      return(QUIT);
      break;
    default:
      PrintError(79, &((resultStruct) {VALUE,result}), "XeqRunCommon()");
      return(QUIT);
      break;
  }

  return(result);
}


int32_t XeqTstFlag(trigger)
 int32_t **trigger;
{
  /* Has one parameter. */
  int32_t owner;
  char    *str; /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (owner == NONE) {
    PrintError(78, NULL, "XeqTstFlag()");
    return(ERROR);
  }

  if (CheckPars(TESTFLAG, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (IsCFlagId(par))
      return(TestCFlag(owner, par));
    else
      return(TestLFlag(par));
  }
  else
    return(QUIT);
}


int32_t XeqValDir(trigger)
 int32_t **trigger;
{
  /* Syntax: valdir(loc, direction). */

  int32_t owner; /* dummy */
  char    *str;      /* dummy */
  int32_t par1;
  int32_t par2;
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;

  /* Skip nr of pars. */
  NextOpcode(trigger);

  /* Read first parameter. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  /* Read second parameter. */
  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  if (CheckPars(VALID_DIRECTION, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* Test if dir is a valid direction from loc. */
    if (exit_data[(par1-FIRST_LOCATION_ID)*nr_of_directions+
                                   par2-first_direction_id] == NO_ID)
      /* Not a valid exit. */
      return(ERROR);
    else
      return(OK);
  }
  else
    return(QUIT);
}


int32_t XeqYesNo(trigger)
 int32_t **trigger; /* Caller expects rest of trigger to be returned. */
{

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  switch(story_info.story_language) {
    case NL:
      return(NL_XeqYesNo());
      break;
    case ENG:
      return(ENG_XeqYesNo());
      break;
    default:
      /* unknown language, use English */
      return(ENG_XeqYesNo());
      break;
  }
}


int32_t XeqTry(trigger)
 int32_t **trigger;
{
  /* Syntax: try(loc/obj, [number], action record). */

  int32_t      owner; /* dummy */
  char         *str;  /* dummy */
  int32_t      par1;
  int32_t      par2;
  int32_t      par3;
  int32_t      type1      = NO_TYPE;
  int32_t      type2      = NO_TYPE;
  int32_t      type3      = NO_TYPE;
  int32_t      type4      = NO_TYPE;
  int32_t      dummy_type = NO_ID;
  usrActionRec action_rec;
  int32_t      result;
  int32_t      old_muted;
  int32_t      old_special_ids[8];
  int          i = 0;

  /* Skip nr of pars, which will always be 4. */
  NextOpcode(trigger);

  /* Parameter 4 are all members of a */
  /* compiler action record stored in */
  /* the *trigger int array           */

  /* Read first parameter */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  /* read second parameter */ /* mute or not */
  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  /* read third parameter */ /* fire timers or not */
  if (!GetPar(&owner, &par3, &type3, &str, trigger))
    return(QUIT);

  /* read the last parameter */
  /* we do not use GetPar() here, because it  */
  /* is not suited for reading action records */
  /* If we get more functions with action rec */
  /* parameters we might change it.           */

  type4 = NextOpcode(trigger); /* must be ACTION_REC */

  if (!CheckPars(TRY, type1, type2, type3, type4, NO_TYPE)) {
    return(QUIT);
  }

  InitUsrActionRec(&action_rec);

  if (!GetActionRecPar(&action_rec, trigger)) {
    return(QUIT);
  }

  /* 18march2018: in a 'normal' user action record    */
  /* that was created from user input, there won't be */
  /* wildcards, but in a user action record that was  */
  /* created from the try() function there may be.    */

  /* replace wildcards in the action record */
  ConvSpecId(&(action_rec.actor), &dummy_type);
  ConvSpecId(&(action_rec.action1), &dummy_type);
  ConvSpecId(&(action_rec.direction), &dummy_type);
  for (i=0; i<MAX_SUBJECTS; i++) {
    ConvSpecId(&(action_rec.subject[i]), &dummy_type);
  }
  ConvSpecId(&(action_rec.specifier), &dummy_type);
  for (i=0; i<action_rec.prepositions.nr_of_prepositions; i++) {
    ConvSpecId(&(action_rec.prepositions.preposition[i]), &dummy_type);
  }

  /* save the current special ids */
  old_special_ids[0] = action;
  old_special_ids[1] = actor;
  old_special_ids[2] = subject;
  old_special_ids[3] = specifier;
  old_special_ids[4] = prepos;
  old_special_ids[5] = direction;
  old_special_ids[6] = value;
  old_special_ids[7] = ordinal;

  /* load special ids from action rec parameter */
  action    = action_rec.action1;
  actor     = action_rec.actor;
  subject   = action_rec.subject[0];
  specifier = action_rec.specifier;
  prepos    = action_rec.prepositions.preposition[0];
  direction = action_rec.direction;
  value     = action_rec.value;
  ordinal   = action_rec.ordinal;

  /* now execute the action record */
  old_muted = muted;
  muted     = par2;
  result    = LetsTry(par1, action_rec);
  muted     = old_muted;

  /* restore the special ids */
  action    = old_special_ids[0];
  actor     = old_special_ids[1];
  subject   = old_special_ids[2];
  specifier = old_special_ids[3];
  prepos    = old_special_ids[4];
  direction = old_special_ids[5];
  value     = old_special_ids[6];
  ordinal   = old_special_ids[7];

  /* possible return values:                   */
  /* QUIT, DISAGREE, CONTINUE, NO_MATCHAGREE   */
  /* After executing this action record, there */
  /* there may (will) be more trigger code     */

  /* check if we must fire the timers */
  if (par3 && result != QUIT) {
    if (CheckDoTimers()) {
      if (HandleTimers(&action_rec, 0) == QUIT) {
        result = QUIT;
      }
    }
  }

  switch (result) {
    case AGREE: ;
    case NO_MATCH:
      return(OK);
    case DISAGREE:
      return(ERROR);
    case QUIT:
      return(QUIT);
    default:
      PrintError(79, &((resultStruct) {VALUE,result}), "XeqTry()");
      return(QUIT);
  }
}


int32_t XeqTestFun(opcode, trigger, action_rec, subject_index, com_trig)
 int32_t      opcode;
 int32_t      **trigger;
 usrActionRec *action_rec;
 int32_t      subject_index;
 int32_t      *com_trig;
{
  switch (opcode) {
    /* Testfunctions are listed in alphabetical order. */
    case CANSEE:
      return(XeqCanSee(trigger));
    case EQUAL:
      return(XeqEqual(trigger));
    case LESS_THAN:
      return(XeqLtGt(LESS_THAN, trigger));
    case GREATER_THAN:
      return(XeqLtGt(GREATER_THAN, trigger));
    case EXIT:
      return(XeqExit(trigger, action_rec, subject_index));
    case ISLIT:
      return(XeqIsLit(trigger));
    case ISOBJECT:
      return(XeqIsObject(trigger));
    case OWNS:
      return(XeqOwns(trigger));
    case RUNVERB:
      return(XeqRunVerb(trigger, action_rec, subject_index));
    case RUNCOMMON:
      return(XeqRunCommon(trigger, action_rec, subject_index, com_trig));
    case TESTFLAG:
      return(XeqTstFlag(trigger));
    case TRIGGER:
      return(XeqExecute(trigger, action_rec, subject_index));
    case VALID_DIRECTION:
      return(XeqValDir(trigger));
    case YES_NO:
      return(XeqYesNo(trigger));
    case TRY:
      return(XeqTry(trigger));
    default:
      PrintError(81, &((resultStruct) {VALUE,opcode}), NULL);
      return(ERROR);
   } /* switch */
}

/*******************************/
/* Internal action definitions */
/*******************************/

/************************************************************/
/* Possible return values for internal actions:             */
/* CONTINUE: all went well; go on with rest of trigger.     */
/* AGREE   : stop; give trigger to next loc/obj.            */
/* DISAGREE: stop; don't give trigger to rest of locs/objs. */
/* QUIT    : stop; prepare to return to o.s.                */
/* <id>,   : location or object identifier.                 */
/* <value> : integer value.                                 */
/************************************************************/

int32_t XeqAddJson(trigger)
 int32_t **trigger;
{
  /* addjson(string [, loc/obj]) */

  /* 1 or 2 parameters */
  int32_t owner;
  char    *str;
  int     nr_of_pars;
  int32_t par1;
  int32_t par2;
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;

  nr_of_pars = NextOpcode(trigger);

  /* Read the parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  if (nr_of_pars == 2) {
    if (!GetPar(&owner, &par2, &type2, &str, trigger))
      return(QUIT);
  }
  else
    type2 = OBJ_ID; /* obj_id or loc_id will pass */

  if (!(CheckPars(ADDJSON, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE))) {
    return(QUIT);
  }

  /* 1 means print to json string and not to screen */
  PrintString(str, 1);

  if (nr_of_pars == 2) {
    PrintNumber(par2, 1);
  }

  return(CONTINUE);
}


int32_t XeqAgree(trigger)
 int32_t **trigger; /* Caller expects rest of trigger to be returned. */
{
  /* Agree() always has 0 parameters. */
  /* Syntax in triggercode: AGREE 0   */

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  return(AGREE);
}


int32_t XeqBackground(trigger)
 int32_t **trigger;
{
  /* background(word)     */
  /* word must be a color */

  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par;     /* color */
  int32_t type = NO_TYPE;

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(BACKGROUND, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (par == LookUpId(TranslateKeyword("BLUE"))) {
      /* set background color to blue   */
      /* this is  system dependent code */

      return(CONTINUE);
    }
    if (par == LookUpId(TranslateKeyword("BLACK"))) {
      /* set background color to black */
      /* this is system dependent code */

      return(CONTINUE);
    }
    PrintError(82, NULL, NULL);
    return(CONTINUE); /* not a severe error */
  }
  else
    return(QUIT);
}


int32_t XeqBlockExit(trigger)
 int32_t **trigger;
{
  /* blockexit(loc, dir) */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par1;
  int32_t par2;
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;

  /* Skip nr of parameters (which will be 2). */
  NextOpcode(trigger);

  /* Read the parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  if (CheckPars(BLOCK_EXIT, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
     /* Now block the exit. */
    exit_data[(par1-FIRST_LOCATION_ID)*nr_of_directions +
              par2-first_direction_id] = NO_ID;
    return(CONTINUE);
  }
  else
    return(QUIT);
}


int32_t XeqBold(trigger)
 int32_t **trigger;
{
  /* bold(word)                 */
  /* word must be 'on' or 'off' */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par;   /* color */
  int32_t type = NO_TYPE;


  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(BOLD, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (par == LookUpId(TranslateKeyword("ON"))) {
      /* set bold on */
      return(CONTINUE);
    }
    else if (par == LookUpId(TranslateKeyword("OFF"))) {
      /* set bold off */
      return(CONTINUE);
    }
    else {
      PrintError(83, NULL, "XeqBold()");
      return(CONTINUE); /* not a severe error */
    }
  }
  else
    return(QUIT);
}


int32_t XeqClearJson(trigger)
 int32_t **trigger;
{
  /* 0 parameters */

  /* skip nr_of_pars */
  NextOpcode(trigger);

  if (json_msg_from_story) {
    free(json_msg_from_story);
    json_msg_from_story = NULL;
  }

  return(CONTINUE);
}


int32_t XeqClearWindow(trigger, which_one)
 int32_t **trigger; /* Caller expects rest of trigger to be returned. */
 int     which_one;
{
  /* console version has no status window */

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  switch (which_one) {
    case 0:
      /* it is the status window */
      /* do nothing              */
      break;
    case 1:
      /* it is the main window   */
      /* Following code is for windows console */
      break;
    default:
      /* this should never happen */
      /* not a severe error       */
      break;
  }
  return(CONTINUE);
}


int32_t XeqCount(trigger)
 int32_t **trigger;
{
  /* Count(loc/obj, common flag, 0/1 [,preposition]) */
  int32_t owner;       /* dummy */
  char    *str;        /* dummy */
  int32_t nr_of_pars;
  int32_t par1;
  int32_t par2;
  int32_t par3;
  int32_t par4;
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;
  int32_t type3 = NO_TYPE;
  int32_t type4 = NO_TYPE;

  nr_of_pars = NextOpcode(trigger);

  /* Read the parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, &par3, &type3, &str, trigger))
    return(QUIT);

  if (nr_of_pars == 4) {
    if (!GetPar(&owner, &par4, &type4, &str, trigger))
      return(QUIT);
  }
  else {
    par4  = 1;       /* 0 is default level */
    type4 = NUMBER;
  }

  if (CheckPars(COUNT, type1, type2, type3, type4, NO_TYPE)) {
    /* types are correct, now check whether */
    /* par3 is either 0 or 1.               */
    if (par3 != 0 && par3 != 1) {
      sprintf(outputline, "count(): parameter 3 must be 0 or 1.\n");
      Output(outputline, 0);
      return(QUIT);
    }
    /* level must be greater than 0 */
    if (par4 < 0)
      par4 = 1;   /* set to default level */

    return(CountObjects(par1, par2, par3, par4));
  }
  else
    return(QUIT);
}


int32_t XeqDebug(trigger)
 int32_t **trigger;
{
  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Open the debug output file */
  if ((debugfile = fopen(DEBUGFILE, "w")) == NULL) {
    /* not a severe error */
    printf("Error opening %s.\n", DEBUGFILE);
    debug_mode = 0;
    return(CONTINUE);
  }
  /* write headers to debug file */
  fprintf(debugfile, "%s", "XVAN debug information for: ");
  fprintf(debugfile, "%s\n", story_info.title);
  fprintf(debugfile, "%s", "version: ");
  fprintf(debugfile, "%s\n", story_info.version);

  sprintf(outputline, "Generating debug file...\n");
  Output(outputline, 0);
  debug_mode = 1;

  Debug();

  /* turn off debug mode */
  debug_mode = 0;

  sprintf(outputline, "%s\n", "Done.");
  Output(outputline, 0);

  fclose(debugfile);

  return(CONTINUE);
}

int32_t XeqDest(trigger)
 int32_t **trigger;
{
  /* Syntax: dest(loc/obj, direction) */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par1;
  int32_t par2;
  int32_t type1  = NO_TYPE;
  int32_t type2  = NO_TYPE;
  int32_t result = NONE;

  /* Skip nr_of_pars, which will be 2. */
  NextOpcode(trigger);

  /* Read parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  /* check for NONE parameters */
  /* will not be caught by CheckPars() because type may */
  /* be correct type for NONE-parameter. */
  if (par1 == NONE || par2 == NONE) {
    PrintError(78, NULL, "XeqDest()");
    return(QUIT);
  }

  if (CheckPars(DEST, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* convert par1 to its  containing     */
    /* locations in  case it is an object. */
    while (!IsLocId(par1))
      par1 = obj_dir[par1-FIRST_OBJECT_ID].held_by;

    /* find the destination when moving from par1 */
    /* in direction par2                          */

    result = exit_data[(par1-FIRST_LOCATION_ID)*nr_of_directions +
                       (par2-first_direction_id)];

    return(result == NO_ID ? NONE : result);
  }
  else
    return(QUIT);
}


int32_t XeqDisagree(trigger)
 int32_t **trigger; /* Caller expects rest of trigger to be returned. */
{
  /* Disagree() always has 0 parameters. */
  /* Syntax in triggercode: DISAGREE 0   */

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  return(DISAGREE);
}


int32_t XeqDistance(trigger, return_value)
 int32_t **trigger;    /* Caller expects rest of trigger to be returned. */
 int32_t return_value; /* DISTANCE or FIRSTDIR */
{
  /* distance(loc/obj, loc/obj) always has 2 parameters */
  /* firstdir(loc/obj, loc/obj) alswas has 2 parameters */

  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t from;
  int32_t to;
  int32_t type1    = NO_TYPE;
  int32_t type2    = NO_TYPE;
  char    fun_name[MAX_WORD_LEN];

  int32_t i           = 0;
  int32_t found_route = 0;
  int32_t found_dir   = 0;
  int32_t level       = 1;
  int32_t result;

  int32_t  *route  = NULL;
  spanTree *tree   = NULL;

  /* Initialize fun_name. */
  fun_name[0] = '\0';

  /* Skip nr_of_pars, which will be 2. */
  NextOpcode(trigger);

  /* set function name for error msg */
  switch (return_value) {
    case DISTANCE:
      strncpy(fun_name, "distance()",MAX_WORD_LEN);
      result = -1;
      break;

    case FIRSTDIR:
      strncpy(fun_name, "firstdir()",MAX_WORD_LEN);
      result = NONE;
      break;

    default:
      PrintError(79, NULL, "XeqDistance()");
      return(QUIT);
  }

  /* Read parameters. */
  if (!GetPar(&owner, &from, &type1, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, &to, &type2, &str, trigger))
    return(QUIT);

  /* check for NONE parameters */
  /* will not be caught by CheckPars() because type */
  /* may be correct type for NONE-parameter.        */
  if (from == NONE || to == NONE) {
    PrintError(78, NULL, fun_name);
    return(QUIT);
  }

  if (!CheckPars(return_value, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE))
    return(QUIT);

/* WHAT IF THE NUMBER OF HOPS IS EQUAL TO THE QUIT CODE??? */

  /* convert from and to to their containing */
  /* locations in  case they are objects.    */
  while (!IsLocId(from))
    from = obj_dir[from-FIRST_OBJECT_ID].held_by;

  while (!IsLocId(to))
    to = obj_dir[to-FIRST_OBJECT_ID].held_by;

  /* in case from location equals to location we must stop here */
  /* don't go through the rest of the process because there may */
  /* be a (long) route from a location to itself.               */
  if(to == from) {
    result = return_value == DISTANCE ? 0 : NONE;
    free(tree);
    free(route);
    return(result);
  }

  /* Initialize spanning tree and route. */
  if (!InitSpanTree(&tree, &route))
    return(QUIT);

  /* prepare tree for running ExpandTree */
  tree[from-FIRST_LOCATION_ID].level = 0;

  while (level<nr_of_locs && !found_route) {
    if (ExpandTree(level, tree)) {
      /* changes were made to the tree */
      if (tree[to-FIRST_LOCATION_ID].from_loc != NO_ID) {
        /* a route was found */
        found_route = 1;
        if (!BuildRoute(to, route, tree))
          return(QUIT);
      }
    }
    level++;
  } /* while */

  if (found_route) {
    /* There is a route */
    if (return_value == DISTANCE) {
      result = (level == 0 ? 0 : level-1); /* level-1 is nr of hops to dest */
    }
    else { /* FIRSTDIR */
      /* determine first direction to go to */
      while (i<nr_of_directions && !found_dir) {
         /* route[level-2] is first dir after 'from' location */
        if (exit_data[(from-FIRST_LOCATION_ID)*nr_of_directions+i] == route[level-2]) {
          result = i + first_direction_id;
          found_dir = 1;
        }
        else
          i++;
      }
    }
  }
  else
    /* no route found, so no distance or first direction*/
    result = return_value == DISTANCE ? -1 : NONE;
  /* Free the spanning tree and route. */
  free(tree);
  free(route);
  return(result);
}


int32_t XeqHitAnyKey(trigger)
 int32_t **trigger; /* Caller expects rest of trigger to be returned. */
{
  /* HitAnyKey() always has 0 parameters. */

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  return(CONTINUE);
}


int32_t XeqNoMatch(trigger)
 int32_t **trigger; /* Caller expects rest of trigger to be returned. */
{
  /* NoMatch() always has 0 parameters. */
  /* Syntax in triggercode: NOMATCH 0   */

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  return(NO_MATCH);
}


int32_t XeqEntrance(trigger, action_rec, subject_index)
 int32_t      **trigger;
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  int32_t result;

  /* Entrance(loc/obj) always has one parameter. */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(ENTRANCE, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result = Entrance(par, action_rec, subject_index);
    return(result != QUIT? CONTINUE : result);
  }
  else
    return(QUIT);
}


int32_t XeqContents(trigger, action_rec, subject_index)
 int32_t      **trigger;
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  int32_t result;

  /* Contents(loc/obj) always has one parameter. */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(CONTENTS, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result = Contents(par, action_rec, subject_index);
    return(result != QUIT? CONTINUE : result);
  }
  else
    return(QUIT);
}


int32_t XeqFlagVal(trigger, value)
 int32_t **trigger;
 int32_t value;
{
  int32_t owner;
  char    *str;  /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;

  /* Skip nr of pars (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(SETFLAG, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (IsCFlagId(par))
      /* Set the value of the flag to `value'.   */
      ProcCFlagVal(owner, par, value);
    else
      /* Local flag. */
      /* Set the value of the flag to `value'. */
      ProcLFlagVal(par, value);
    return(CONTINUE);
  }
  else
    return(QUIT);
}


int32_t XeqGetSubject(trigger)
 int32_t **trigger;
{
  /* Syntax: getsubject() */

  /* Skip number of parameters. */
  NextOpcode(trigger);

  return(GET_SUBJECT);
}

int32_t XeqGetSpec(trigger)
 int32_t **trigger;
{
  /* Syntax: getspec() */

  /* Skip number of parameters. */
  NextOpcode(trigger);

  return(GET_SPECIFIER);
}


int32_t XeqGoTo(trigger)
 int32_t **trigger;
{
  int32_t  owner;
  char     *str;           /* dummy */
  int32_t  nr_of_pars;
  int32_t  par1;           /* Object that must be moved. */
  int32_t  par2;           /* Location to move par1 to.  */
  int32_t  par3;           /* Number of moves.           */
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;
  int32_t  type3 = NO_TYPE;

  int32_t  from_loc;
  int32_t  level       = 1; /* first level to expand the tree to */
  int32_t  i           = 0;
  int32_t  found_route = 0;
  int32_t  stop        = 0;
  int32_t  *route      = NULL;
  spanTree *tree       = NULL;

  /* Read number of parameters. */
  nr_of_pars = NextOpcode(trigger);

  /* Read the parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  if (nr_of_pars == 3) {
    if (!GetPar(&owner, &par3, &type3, &str, trigger))
      return(QUIT);
  }
  else {
    /* default is one move at a time. */
    par3 = 1;
    type3 = NUMBER;
  }

  if (CheckPars(GO_TO, type1, type2, type3, NO_TYPE, NO_TYPE)) {
    /* Load from_loc. */
    from_loc = par1;
    do
      from_loc = obj_dir[from_loc-FIRST_OBJECT_ID].held_by;
    while (!IsLocId(from_loc));

    /* Initialize spanning tree and route. */
    if (!InitSpanTree(&tree, &route))
      return(QUIT);

    /* prepare tree for running ExpandTree */
    tree[from_loc-FIRST_LOCATION_ID].level = 0;

    while (level<nr_of_locs && !found_route) {
      if (ExpandTree(level, tree)) {
        /* changes were made to the tree */
        if (tree[par2-FIRST_LOCATION_ID].from_loc != NO_ID) {
          /* a route was found */
          found_route = 1;
          if (!BuildRoute(par2, route, tree))
            return(QUIT);
        }
      }
      level++;
    } /* while */

    if (!found_route) {
      /* no route found */
      PrintError(84, NULL, NULL);
      return(CONTINUE);
    }

    /* There is a route, move par1.             */
    /* par3 denotes max nr of moves, stop if 0. */
    while (i >= 0 && par3 != 0 && !stop) {
      /* printf("%d Moving ", i);PrintId(par1);printf(" to ");PrintId(route[level-2]);printf("\n"); */
      if (!Move(par1, route[level-2-i]))
        return(QUIT);
      if (par1 == PLAYER || Owns(par1, PLAYER, -1))
        /* Update current location. */
        curr_loc = route[level-2-i];
      if (route[level-2-i] == par2)
        stop = 1;
      i--;
      par3--;
    }

    /* Free the spanning tree and route. */
    free(tree);
    free(route);
    return(CONTINUE);
  }
  else {
    /* Free the spanning tree and route. */
    free(tree);
    free(route);
    return(QUIT);
  }
}


int32_t XeqGetAnswer(trigger)
 int32_t **trigger;
{
  /* Syntax: getanswer() */

  /* Skip number of parameters. */
  NextOpcode(trigger);

  return(GET_ANSWER);
}


int32_t XeqItalic(trigger)
 int32_t **trigger;
{
  /* italic(word)               */
  /* word must be 'on' or 'off' */

  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par;     /* color */
  int32_t type = NO_TYPE;

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(ITALIC, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (par == LookUpId(TranslateKeyword("ON"))) {
     /* set italic on */
     return(CONTINUE);
    }
    else if (par == LookUpId(TranslateKeyword("OFF"))) {
      /* set italic off */
      return(CONTINUE);
    }
    else {
      PrintError(83, NULL, "XeqItalic()");
      return(CONTINUE); /* not a severe error */
    }
  }
  else
    return(QUIT);
}


int32_t XeqMove(trigger)
 int32_t **trigger;
{
  /* Syntax: move(obj, loc/obj/direction [, preposition]) */

  int32_t owner;          /* dummy */
  char    *str;           /* dummy */
  int     nr_of_pars = 0;
  int     index      = -1;
  int32_t dest;
  int32_t par1;
  int32_t par2;
  int32_t par3;
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;
  int32_t type3 = NO_TYPE;

  nr_of_pars = NextOpcode(trigger);

  /* Read parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  if (nr_of_pars == 3) {
    if (!GetPar(&owner, &par3, &type3, &str, trigger))
      return(QUIT);
    /* Help CheckPars() a bit. Par3 must be of type     */
    /* prepositions but because words may have multiple */
    /* types, we may get the wrong type here. GetPar()  */
    /* will als0 check for DIRECTIONS and VERB_ID types */
    /* for word-parameters, so preposition 'in' will be */
    /* returned as DIRECTIONS type. As there is no      */
    /* specific id range for prepositions, the best we  */
    /* can do here is word id.                          */
    if (IsWordId(par3))
      type3 = WORD_ID;
  }
  else
    type3 = WORD_ID;  /* or else typecheck will fail */

  /* check for NONE parameters */
  /* will not be caught by CheckPars() because type may */
  /* be correct type for NONE-parameter. */
  if (par1 == NONE || par2 == NONE) {
    sprintf(outputline, "XeqMove(): NONE-parameter.\n");
    Output(outputline, 0);
    return(QUIT);
  }

  if (CheckPars(MOVE, type1, type2, type3, NO_TYPE, NO_TYPE)) {
    /* Test for identical parameters. */
    if (par1 == par2) {
      PrintError(85, NULL, NULL);
      return(DISAGREE);
    }

    if (!(IsLocId(par2) || IsObjId(par2)))
      /* Par2 is a direction; convert it to a location. */
      if ((par2 = exit_data[(curr_loc-FIRST_LOCATION_ID)*nr_of_directions+
                       par2-first_direction_id]) == NO_ID) {
        sprintf(outputline, "XeqMove(): Invalid direction.\n");
        Output(outputline, 0);
        return(QUIT);
      }

    /* Par1 must be moved into par2.     */
    if (!Move(par1, par2))
      return(QUIT);

    /* Check if preposition of par1 must be updated */
    /* r_preposotion has id FIRST_COMMON_ATTR_ID.   */
    /* This is set at compile time.                 */
    if (nr_of_pars == 3) {
      index = (par1-FIRST_OBJECT_ID)*nr_of_cattrs;
      c_obj_attrs[index].value = par3;
    }

    /* if the player is moved, we must update curr_loc */
    /* scenarios are:                                  */
    /* - player is moved to a location                 */
    /* - object with the player in it is moved to a    */
    /*   location                                      */
    /* - player is moved to an object in another       */
    /*   location                                      */
    /* - object with the player in it ids moved to an  */
    /*   object in another location                    */
    dest = par2;
    if (par1 == PLAYER || Owns(par1, PLAYER, -1)) {
      while (!IsLocId(dest)) {
        dest = obj_dir[dest-FIRST_OBJECT_ID].held_by;
      }
      curr_loc = dest;
    }

    return(CONTINUE);
  }
  else
    return(QUIT);
}


int32_t XeqNewExit(trigger)
 int32_t **trigger;
{
  /* newexit(loc, dir, loc) */

  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par[3];
  int32_t type[3];
  int32_t i;

  for (i=0; i<3; i++)
    type[i] = NO_TYPE;

  /* Skip nr of parameters, which will be 3. */
  NextOpcode(trigger);

  /* Read the parameters. */
  for (i=0; i<3; i++) {
    if (!GetPar(&owner, &(par[i]), &(type[i]), &str, trigger))
      return(QUIT);
  }

  if (CheckPars(NEW_EXIT, type[0], type[1], type[2], NO_TYPE, NO_TYPE)) {
    /* Now create the new exit. */
    exit_data[(par[0]-FIRST_LOCATION_ID)*nr_of_directions +
              par[1]-first_direction_id] = par[2];

    return(CONTINUE);
  }
  else
    return(QUIT);
}


int32_t XeqNoTimers(trigger)
 int32_t **trigger;
{
  /* syntax: notimers() */
  /* Always has zero parameters. */

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* disable timers for the next move */
  fire_timers = ERROR;

  return(CONTINUE);
}


int32_t XeqOwner(trigger)
 int32_t **trigger;
{
  /* syntax: owner(obj) */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;

  /* Skip nr of parameters, which will be 1. */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(OWNER, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* Test for none-parameter. */
    if (par == NONE) {
      /* None cannot have an owner, so NONE */
      return(NONE);
    }

    if (!IsObjId(par)) {
      /* only object ids can have an owner, so NONE */
      return(NONE);
    }
    return(obj_dir[par-FIRST_OBJECT_ID].held_by);
  }
  else
    return(QUIT);
}


int32_t XeqIndent(trigger)
 int32_t **trigger;
{
  int32_t owner;  /* dummy */
  char    *str;   /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;
  char    spaces_string[OUTPUT_LINE_LEN-1];

  /* Has either 0 or 1 par. If 0 pars, then we must print the   */
  /* indent; if 1 par, then we must change the value of indent. */
  switch (NextOpcode(trigger)) {
    case 0:
      if (indent > OUTPUT_LINE_LEN-1) {
        PrintError(86, &((resultStruct) {VALUE,indent}), NULL);
        return(ERROR);
      }
      else {
        /* build a string for PrintString(), who will check when */
        /* wrap around the line.                                 */
        for (par = 0; par<indent; par++)
          spaces_string[par] = SPACE;
        spaces_string[par] = '\0';
        PrintString(spaces_string, 0);
      }
      break;
    case 1:
      /* 1 parameter. */
      if (!GetPar(&owner, &par, &type, &str, trigger))
        return(QUIT);
      if (CheckPars(INDENT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
        indent += par;
        /* correct negative value */
        indent = (indent<0? 0 : indent);
      }
      else
        return(QUIT);
      break;
    default:
      sprintf(outputline, "XeqIndent(): error.\n");
      Output(outputline, 0);
      break;
  } /* switch */
  return(CONTINUE);
}


int32_t XeqPrt(trigger)
 int32_t **trigger;
{
  /* Syntax: print(loc/obj, descr) or        */
  /*         print(<addrpart1> ..<addrpartn> */

  int32_t  owner;
  int32_t  par;
  int32_t  type = NO_TYPE;

  char *str = NULL;

  /* Skip the number of parameters, which will be 1.   */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(PRINT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (type == STRING)
      PrintString(str, 0);

    else if (type == NUMBER)
      PrintNumber(par, 0);

    else if (owner == NONE || (type == NO_TYPE && par == NONE))
	  /* print nothing. */
      return(CONTINUE);

    /* now check for a d_sys parameter. We must not use GetDescr()  */
    /* to retrieve a d_sys, because multiple System Descriptions    */
    /* are allowed and GetDescr() will always return the only one   */
    /* that was stored in descrInfo: the first one.                 */

    else if (par == DSYS)
      /* DSYS check must be done before DescrId checks              */
      PrintId(owner, 0);

    else if (par == THIS)
      /* DSYS check must be done before DescrId checks              */
      PrintId(THIS, 0);

    else if (IsCDescrId(par) || IsLDescrId(par)) {
      if ((str = GetDescr(owner, par)) != NULL)
        PrintString(str, 0);
    }
    /* 22 dec 2003: didn´t print attributes that are not NUMBERS.   */
    /* Following code was added to print locations or objects that  */
    /* are contained in an attributes parameter (attribute pars are */
    /* returned with type NO_TYPE or type NUMBER).                  */
    else if (IsLocId(par) || IsObjId(par) || IsTimerId(par)) {
      PrintId(par, 0);
    }
    else if (IsWordId(par)) {
      PrintWord(par, 0);
    }
    return(CONTINUE);
  }
  else
    return(QUIT);
}


int32_t XeqPrtcr(trigger)
 int32_t **trigger;
{
  XeqPrt(trigger);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  return(CONTINUE);
}


int32_t XeqPrtStat(trigger)
 int32_t **trigger;
{
  /* This function is for the Glk version of XVAN, but we */
  /* must handle it to get it off the stack so we can     */
  /* continue */

  int32_t  owner;
  int32_t  par;
  int32_t  type = NO_TYPE;

  char *str = NULL;

  /* Skip the number of parameters, which will be 1.   */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (!CheckPars(PRINT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE))
    return(QUIT);
  else
    return(CONTINUE);
}


int32_t XeqSetCursor(trigger)
 int32_t **trigger;
{
  /* This function is for use with the Glk version of XVAN. */
  /* For compatibility reasons we must process it here,     */
  /* although it will do nothing.                           */

  /* setcursor(x-pos, y-pos) */

  int32_t owner;
  char    *str;  /* dummy */
  int32_t par1;
  int32_t par2;
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;

  int32_t index; /* For fixing bug(?). */

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Read first parameter. */
  /* For attribute parameters, GetPar() returns the */
  /* attribute's type and value.                     */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  /* It may be a timer id */
  if (IsTimerId(par1)) {
    index = par1-FIRST_TIMER_ID;
    par1 = timers[index].value;
    type1 = NUMBER;
  }

  /* Read second parameter. */
  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  /* It may be a timer id */
  if (IsTimerId(par2)) {
    index = par2-FIRST_TIMER_ID;
    par2  = timers[index].value;
    type2 = NUMBER;
  }
  /* Now both pars are numeric. */

  if (CheckPars(SETCURSOR, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* do nothing, just get this function off the stack */
    return(CONTINUE);
  } /* if */
  else
    return(QUIT);
}


int32_t XeqQuit(trigger)
 int32_t **trigger;
{
  /* Always has zero parameters. */

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  return(QUIT);
}


int32_t XeqRestart(trigger)
 int32_t **trigger; /* Caller expects rest of trigger to be returned. */
{
  /* Restart() always has 0 parameters. */
  /* Syntax in triggercode: RESTART 0   */

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  if (!Restart()) {
     return(QUIT);
  }
  else {
    return(CONTINUE);
  }
}


int32_t XeqScore(trigger)
 int32_t **trigger;
{
  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par;     /* score */
  int32_t type = NO_TYPE;

  /* This function is used in the disambiguation rules  */
  /* section of a verb. It is special in the way that   */
  /* it does not return the score value, but adds it to */
  /* a global variable disambig_score. This must be     */
  /* because the disambiguation code can have multiple  */
  /* calls to the score() function for different        */
  /* situations. As the disambig code has no memory (it */
  /* will only deliver the last value), we fall back to */
  /* the global var. But remember to reset the var to 0 */
  /* in the calling function.                           */

  /* Always has one parameter. */

  /* Skip nr of pars. */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(SCORE, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /*return(CONTINUE);*/
    /*return(par);*/
    disambig_score += par;
    return(CONTINUE);
  }
  else
    return(QUIT);
}


int32_t XeqSendJson(trigger)
 int32_t **trigger;
{
  char    *json_key_1 = NULL;
  char    *json_key_2 = NULL;
  int32_t request     = IFI_NO_IFI;;
  int     index       = 1; /* skip the '{' */

  /* 0 parameters */

  NextOpcode(trigger);

  /* check if the frond-end wants to receive this json */
  /* get the message type, which is the first key      */

  json_key_1 = ReadJsonString(json_msg_from_story, &index);

  /* now check the ifi_stats if we may send this message */
  /* WHY DO WE NEED json_key_2 ?? */
  json_key_2 = AddToString(json_key_2, json_key_1);

  request = CheckIFI(json_key_2);

  switch (request) {
    case IFI_REQ_ITEMS:
      if (ifi_stats.items) {
        ifi_emitResponse(json_msg_from_story);
      }
      break;

    case IFI_REQ_LOCATION:
      if (ifi_stats.location) {
        ifi_emitResponse(json_msg_from_story);
      }
      break;

    case IFI_REQ_MAP:
      if (ifi_stats.map) {
        ifi_emitResponse(json_msg_from_story);
      }
    case IFI_REQ_MOVES:
      if (ifi_stats.move) {
        ifi_emitResponse(json_msg_from_story);
      }
      break;

    case IFI_REQ_PICTURE:
      if (ifi_stats.picture) {
        ifi_emitResponse(json_msg_from_story);
      }
      break;


    case IFI_REQ_PEOPLE:
      if (ifi_stats.people) {
        ifi_emitResponse(json_msg_from_story);
      }
      break;

    default:
      /* send the msg */
      ifi_emitResponse(json_msg_from_story);
      break;
  }
  free(json_key_1);
  free(json_key_2);
  return(CONTINUE);
}


int32_t XeqSetAttribute(trigger)
  int32_t **trigger;
{
  int32_t par1;            /* for attribute id                               */
  int32_t owner1;          /* par1 owner                                     */
  int32_t type1 = NO_TYPE; /* par1 type                                      */
  int32_t par2;            /* for value that must be stored in the attribute */
  int32_t owner2;          /* par2 owner                                     */
  int32_t type2=NO_TYPE;   /* par2 type                                      */
  char    *str;            /* dummy                                          */

  attrInfo *attributes;       /* No malloc(), we only need the pointer          */
  int32_t  attribute_index;

  /* Syntax: setattribute([loc/obj].attribute, loc/obj/attribute/direction/value/word) */

  /* Skip number of parameters (which will always be 2). */
  NextOpcode(trigger);

  /* Read first parameter, which will be the attribute id. */
  /* We must use GetLvaluePar() here, because otherwise    */
  /* GetPar() will return the attribute's contents and not */
  /* the attribute itself.                                 */

   if (!GetLvaluePar(&owner1, &par1, &type1, &str, trigger))
     return(QUIT);

  /* Read second parameter.                                     */
  /* In case the second par is an attribute, its value and type */
  /* will be returned by GetPar().                              */
  if (!GetPar(&owner2, &par2, &type2, &str, trigger))
    return(QUIT);

  /* Retrieve the par1 attribute info struct */
  if (!GetAttributeInfo(par1, owner1, &attributes, &attribute_index))
    return(QUIT);

  if (CheckPars(SETATTRIBUTE, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* Store par2 in attribute for par1. */
    /* 10 march2017: also set owner in case it is a description id */
    attributes[attribute_index].value_owner = owner2;
    attributes[attribute_index].type        = type2;
    attributes[attribute_index].value       = par2;
    return(CONTINUE);
  }
  else
    return(QUIT);
}


int32_t XeqShuffle(trigger)
 int32_t **trigger;
{
  /* shuffle(loc/obj) */

  /* This function shuffles the parameter's contained */
  /* object list to create randomness in the order in */
  /* which contained objects respond.                 */

  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(ERROR);

  if (CheckPars(SHUFFLE, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (Shuffle(par))
      return(CONTINUE);
    else
      return(ERROR);
  }
  else
    return(QUIT);
}


int32_t XeqStartTimer(trigger)
 int32_t **trigger;
{
  /* Always has one parameter. */

  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par;     /* counter id. */
  int32_t type = NO_TYPE;
  int32_t index;  /* To correct a compiler bug. */

  /* Skip nr of pars. */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(STARTTIMER, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    index = par-FIRST_TIMER_ID;
    timers[index].state = GO;
    /* compiler chokes on timers[par-FIRST_TIMER_ID]. */

    return(CONTINUE);
  }
  else
    return(QUIT);
}


int32_t XeqStopTimer(trigger)
 int32_t **trigger;
{
  /* Always has one parameter. */

  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par;     /* counter id. */
  int32_t type = NO_TYPE;
  int32_t index;   /* To correct a compiler bug. */

  /* Skip nr of pars. */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(STOPTIMER, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    index = par-FIRST_TIMER_ID;
    timers[index].state = STOP;
    /* compiler chokes on timers[par-FIRST_TIMER_ID]. */

    return(CONTINUE);
  }
  else
    return(QUIT);
}


int32_t XeqSynchro(trigger, action_rec, subject_index)
 int32_t      **trigger;
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  /* Synchronize(loc/obj, trigger, common flag, 0/1 [,level]) */
  int32_t owner;       /* dummy */
  char    *str;        /* dummy */
  int     i = 0;
  int32_t nr_of_pars;
  int32_t par[5];      /* 0 .. 4 */
  int32_t type[5];     /* 0 .. 4 */

  for (i=0; i<4; i++)
    type[i] = NO_TYPE;

  nr_of_pars = NextOpcode(trigger); /* either 4 or 5 */

  /* Read the parameters. */
  if (!GetPar(&owner, par, type, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, par+1, type+1, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, par+2, type+2, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, par+3, type+3, &str, trigger))
    return(QUIT);

  if (nr_of_pars == 5) {
    if (!GetPar(&owner, par+4, type+4, &str, trigger))
      return(QUIT);
  }
  else {
    par[4]  = 1;       /* 0 is default level */
    type[4] = NUMBER;
  }

  if (CheckPars(SYNCHRONIZE, type[0], type[1], type[2], type[3], type[4])) {
    /* types are correct, now check whether */
    /* par[3] is either 0 or 1.               */
    if (par[3] != 0 && par[3] != 1) {
      sprintf(outputline, "synchronize(): parameter 3 must be 0 or 1.\n");
      Output(outputline, 0);
      return(QUIT);
    }
    return(Synchronize(par[0], par[1], par[2], par[3], par[4], action_rec, subject_index));
  }
  else
    return(QUIT);
}


int32_t XeqText(trigger)
 int32_t **trigger;
{
  /* text(word)           */
  /* word must be a color */

  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par;     /* color */
  int32_t type = NO_TYPE;


  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(TEXT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (par == LookUpId(TranslateKeyword("BLUE"))) {
      /* set text color to blue        */
      /* this is system dependent code */
      /* system("COLOR F1"); */
      return(CONTINUE);
    }
    else if (par == LookUpId(TranslateKeyword("BLACK"))) {
      /* set text color to black       */
      /* this is system dependent code */
      /* system("COLOR F0"); */
      return(CONTINUE);
    }
    else {
      PrintError(87, NULL, NULL);
      return(CONTINUE); /* not a severe error */
    }
  }
  else
    return(QUIT);
}


int32_t XeqUnderline(trigger)
 int32_t **trigger;
{
  /* underline(word)            */
  /* word must be 'on' or 'off' */

  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par;     /* color */
  int32_t type = NO_TYPE;


  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (par == LookUpId(TranslateKeyword("ON"))) {
    /* set underline on */
    return(CONTINUE);
  }
  if (par == LookUpId(TranslateKeyword("OFF"))) {
    /* set underline off */
    return(CONTINUE);
  }

  PrintError(83, NULL, "XeqUnderline()");
  return(CONTINUE); /* not a severe error */
}


int32_t XeqWait(trigger, action_rec, subject_index)
 int32_t      **trigger;
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par;     /* number of turns to wait. */
  int32_t type = NO_TYPE;
  int32_t i;
  /* Always has one parameter. */

  /* Skip nr of pars. */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(WAIT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* 15aug2015 - HandleTimers() is executed after each move. as wait() is a move, */
    /* for "wait 1" we don't need to execute HandleTimers() in this function.       */

    if (par>1) {
      /* Execute 1 less HandleTimers() then par indicates because        */
      /* the interpreter will kick off HandleTimers as part of the move. */
      for (i=0; i<par-1; i++)
        if (HandleTimers(action_rec, subject_index) == QUIT)
          return(QUIT);
    }
    return(CONTINUE);
  }
  else
    return(QUIT);
}


int32_t XeqSave(trigger)
 int32_t **trigger;
{
  /* Syntax: save()              */
  /* Always has zero parameters. */

  char *json_save  = NULL;
  char *game_state = NULL;


  /* Skip nr of parameters.      */
  NextOpcode(trigger);

  /* send the following json string format to the front-end: */
  /* {"savedata:{data:"blob"}"} (we do not send a filename)  */


  /* start the json string */
  if ( (json_save = AddToString(json_save, "{\"savedata\":{\"data\":\"")) == NULL)
    return(ERROR);


  /* get the game state (save info) in a base64 encoded string */
  if ( (game_state = Base64Save(game_state)) == NULL) {
    /* something went wrong, error was already printed */
    json_save = ResetString(json_save);
    return(ERROR);
  }

  /* add the game state to the json string */
  if ( (json_save = AddToString(json_save, game_state)) == NULL) {
    json_save = ResetString(game_state);
    return(ERROR);
  }

  /* end the json string */
  if ( (json_save = AddToString(json_save, "\"}}")) == NULL)
    return(ERROR);

  /* send to front-end */
  ifi_emitResponse(json_save);

  free(json_save);

  return(CONTINUE);
}


int32_t XeqRestore(trigger)
 int32_t **trigger;
{
  /* Syntax: restore() */
  /* Always has zero parameters. */

  /* Skip nr of parameters.      */
  NextOpcode(trigger);

  char json_string[] = "{\"loaddata\":\"\"}";

  /* only send a {"loaddata":""} json message to the front-end    */
  /* the front-end's response with the save data will be handled  */
  /* by XeqIFIrequest() in ifi.c                                  */

  ifi_emitResponse(json_string);

  return(CONTINUE);
}


int32_t XeqTestmode(trigger)
 int32_t **trigger;
{
  /* Skip nr of parameters. */
  NextOpcode(trigger);

  if (testmode) {
    /* testmode already active */
    PrintError(90, NULL, NULL);
    return(CONTINUE);
  }

  /* Open the file with testdata */
  if ((testfile = fopen(TESTFILE, "r")) == NULL) {
    /* not a severe error */
    PrintError(40, NULL, TESTFILE);
    return(CONTINUE);
  }
  PrintError(91, NULL, NULL);  /* not an error */
  testmode = 1;
  return(CONTINUE);
}


int32_t XeqTranscript(trigger)
 int32_t **trigger;
{
  /* Skip nr of parameters. */
  NextOpcode(trigger);

  if (transcript) {
    /* transcript mode already activated, turn it off */
    PrintError(93, NULL, NULL);  /* not an error */
    transcript = 0;
    fclose(transcriptfile);
    return(CONTINUE);
  }
  /* Open the transcript output file */
  if ((transcriptfile = fopen(TRANSCRIPTFILE, "w")) == NULL) {
    /* not a severe error */
    PrintError(40, NULL, TRANSCRIPTFILE);
    return(CONTINUE);
  }
  /* write headers to transcript file */
  fprintf(transcriptfile, "%s", "\n=====================================\n");
  fprintf(transcriptfile, "%s", "XVAN transcript for: ");
  fprintf(transcriptfile, "%s\n", story_info.title);
  fprintf(transcriptfile, "%s", "version: ");
  fprintf(transcriptfile, "%s\n", story_info.version);
  fprintf(transcriptfile, "%s", "\n=====================================\n");

  PrintError (92, NULL, NULL);  /* not an error */
  transcript = 1;
  return(CONTINUE);
}


int32_t XeqIntAct(opcode, trigger, action_rec, subject_index)
 int32_t opcode;
 int32_t **trigger; /* Caller expects remainder of *trigger to be */
                    /* returned.                                  */
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  switch (opcode) {
    /* Internal actions are listed in alfabetical order. */
    case AGREE:
      return(XeqAgree(trigger));
    case ADD:
      return(XeqBasicOperator(opcode, trigger));
    case ADDJSON:
      return(XeqAddJson(trigger));
    case ASTERIX:
      return(XeqBasicOperator(opcode, trigger));
    case BACKGROUND:
      return(XeqBackground(trigger));
    case BLOCK_EXIT:
      return(XeqBlockExit(trigger));
    case BOLD:
      return(XeqBold(trigger));
    case CLEARFLAG:
      return(XeqFlagVal(trigger, 0));
    case CLEARJSON:
      return(XeqClearJson(trigger));
    case DEBUG:
      return(XeqDebug(trigger));
    case DEST:
      return(XeqDest(trigger));
    case DISAGREE:
      return(XeqDisagree(trigger));
    case DISTANCE:
      return(XeqDistance(trigger, DISTANCE));
    case NOMATCH:
      /* Do not use keyword NO_MATCH here. */
      /* (not an internal action).         */
      return(XeqNoMatch(trigger));
    case CONTENTS:
      return(XeqContents(trigger, action_rec, subject_index));
    case ENTRANCE:
      return(XeqEntrance(trigger, action_rec, subject_index));
    case FIRSTDIR:
      return(XeqDistance(trigger, FIRSTDIR));
    case COUNT:
      return(XeqCount(trigger));
    case GET_SUBJECT:
      return(XeqGetSubject(trigger));
    case GET_SPECIFIER:
      return(XeqGetSpec(trigger));
    case GET_ANSWER:
      return(XeqGetAnswer(trigger));
    case GO_TO:
      return(XeqGoTo(trigger));
   case INDENT:
      return(XeqIndent(trigger));
    case ITALIC:
      return(XeqItalic(trigger));
    case MOVE:
      return(XeqMove(trigger));
    case NEW_EXIT:
      return(XeqNewExit(trigger));
    case NOTIMERS:
      return(XeqNoTimers(trigger));
    case OWNER:
      return(XeqOwner(trigger));
    case PRINT:
      return(XeqPrt(trigger));
    case PRINTBOLD:
     return(XeqPrt(trigger));
    case PRINTITALIC:
      return(XeqPrt(trigger));
    case PRINTCR:
      return(XeqPrtcr(trigger));
    case PRINTCRBOLD:
      return(XeqPrtcr(trigger));
    case PRINTCRITALIC:
      return(XeqPrtcr(trigger));
    case PRINTSTATUS:        /* For Glk XVAN only */
      return(XeqPrtStat(trigger));
    case PRINTCRSTATUS:      /* For Glk XVAN only */
      return(XeqPrtStat(trigger));
    case SENDJSON:
      return(XeqSendJson(trigger));
    case SETCURSOR:          /* For Glk XVAN only */
      return(XeqSetCursor(trigger));
    case CLEARSTATUS:        /* For Glk XVAN only */
      return(XeqClearWindow(trigger, 0));
    case CLEARSCREEN:
      return(XeqClearWindow(trigger, 1));
    case HITANYKEY:
      return(XeqHitAnyKey(trigger));
    case QUIT:
      return(XeqQuit(trigger));
    case RAND:
      return(XeqRnd(trigger));
    case SETTIMER:
      return(XeqSetTimer(trigger));
    case SETFLAG:
      return(XeqFlagVal(trigger, 1));
    case SETATTRIBUTE:
      return(XeqSetAttribute(trigger));
    case QUOT:
      return(XeqBasicOperator(opcode, trigger));
    case REM:
      return(XeqBasicOperator(opcode, trigger));
    case RESTART:                   /* oct 18 18 */
      return(XeqRestart(trigger));
    case SCORE:
      return(XeqScore(trigger));
    case SHUFFLE:
      return(XeqShuffle(trigger));
    case STARTTIMER:
      return(XeqStartTimer(trigger));
    case STOPTIMER:
      return(XeqStopTimer(trigger));
    case SUB:
      return(XeqBasicOperator(opcode, trigger));
    case SYNCHRONIZE:
      return(XeqSynchro(trigger, action_rec, subject_index));
    case TESTMODE:
      return(XeqTestmode(trigger));
    case TEXT:
      return(XeqText(trigger));
    case TRANSCRIPT:
      return(XeqTranscript(trigger));
    case UNDERLINE:
      return(XeqUnderline(trigger));
    case WAIT:
      return(XeqWait(trigger, action_rec, subject_index));
	case SAVE:						/* dec 21 07 */
	  return(XeqSave(trigger));
	case RESTORE:					/* dec 21 07 */
	  return(XeqRestore(trigger));
    default:
      PrintError(94, &((resultStruct) {VALUE,opcode}), NULL);
      return(ERROR);
  } /* switch */
}
