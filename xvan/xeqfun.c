
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
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
int32_t XeqTry(int32_t**);
int32_t XeqValDir(int32_t**);
int32_t XeqYesNo(int32_t**);

resultStruct XeqAddChoice(int32_t**);
resultStruct XeqAddJson(int32_t**);
resultStruct XeqAgree(int32_t**);
resultStruct XeqBackground(int32_t**);
resultStruct XeqBlockExit(int32_t**);
resultStruct XeqBold(int32_t**);
resultStruct XeqClearJson(int32_t**);
resultStruct XeqClearWindow(int32_t**, int);
resultStruct XeqContents(int32_t**, usrActionRec*, int32_t);
resultStruct XeqCount(int32_t**);
resultStruct XeqDebug(int32_t**);
resultStruct XeqDest(int32_t**);
resultStruct XeqDisagree(int32_t**);
resultStruct XeqDistance(int32_t**, int32_t);
resultStruct XeqEntrance(int32_t**, usrActionRec*, int32_t);
resultStruct XeqFlagVal(int32_t**, int32_t);
resultStruct XeqGetSubjectOrSpec(int32_t**, int32_t);
resultStruct XeqGetAnswer(int32_t**);
resultStruct XeqGoTo(int32_t**);
resultStruct XeqHitAnyKey(int32_t**);
resultStruct XeqIndent(int32_t**);
resultStruct XeqItalic(int32_t**);
resultStruct XeqMove(int32_t**);
resultStruct XeqNewDSys(int32_t**);
resultStruct XeqNewExit(int32_t**);
resultStruct XeqNoMatch(int32_t**);
resultStruct XeqNoTimers(int32_t**);
resultStruct XeqOwner(int32_t**);
resultStruct XeqPickOne(int32_t**);
resultStruct XeqPlayMode(int32_t**);         /* jul 20 19 */
resultStruct XeqPrt(int32_t**);
resultStruct XeqPrtcr(int32_t**);
resultStruct XeqPrtStat(int32_t**);
resultStruct XeqQuit(int32_t**);
resultStruct XeqRestart(int32_t**);          /* oct 18 18 */
resultStruct XeqRestore(int32_t**);          /* dec 21 07 */
resultStruct XeqSave(int32_t**);             /* dec 21 07 */
resultStruct XeqScore(int32_t**);
resultStruct XeqSendJson(int32_t**);
resultStruct XeqSetAttribute(int32_t**);
resultStruct XeqSetCursor(int32_t**);
resultStruct XeqShuffle(int32_t**);
resultStruct XeqStartTimer(int32_t**);
resultStruct XeqStopTimer(int32_t**);
resultStruct XeqSynchro(int32_t**, usrActionRec*, int32_t);
resultStruct XeqTestmode(int32_t**);
resultStruct XeqText(int32_t**);
resultStruct XeqTranscript(int32_t**);
resultStruct XeqUnderline(int32_t**);
resultStruct XeqUndo(int32_t**);  /* @!@ */
resultStruct XeqWait(int32_t**, usrActionRec*, int32_t);
resultStruct XeqIntAct(int32_t, int32_t**, usrActionRec*, int32_t);

/****************************/
/* Testfunction definitions */
/****************************/

int32_t XeqCanSee(int32_t **trigger)
{
  int32_t  owner;  /* dummy */
  char     *str;   /* dummy */
  int32_t  par1;
  int32_t  par2;   /* CanSee() always has two parameters. */
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;
  int32_t  result;

  resultStruct par_list[2];  /* for debugging */

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Read parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  /* for debugging */
  if (debug_level == 2) {
    par_list[0].tag   = type1;
    par_list[0].owner = NO_ID;
    par_list[0].value = par1;
    par_list[1].tag   = type2;
    par_list[1].owner = NO_ID;
    par_list[1].value = par2;
    DebugLevel_2_pars("cansee()", par_list, 2);
  }

  if (par1 == NONE || par2 == NONE) {
    PrintError(78, NULL, "XeqCanSee()");
    return(QUIT);
  }

  if (CheckPars(CANSEE, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result = CanSee(par1, par2);
    DebugLevel_2_result( (resultStruct) {result, NO_ID, 0} );
    return(result);
  }
  else
    return(QUIT);
}


int32_t XeqExecute(int32_t **trigger, usrActionRec *action_rec, int32_t subject_index)
{
  /* The corresponding XVAN-function is called `trigger',   */
  /* because EXECUTE is not an internal action in keyword.h */

  int32_t      owner;
  char         *str;  /* dummy */
  int32_t      par;
  int32_t      type = NO_TYPE;
  resultStruct result;

  resultStruct par_list;   /* for debugging */

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = owner;
    par_list.value = par;
    DebugLevel_2_pars("Trigger()", &(par_list), 1);
  }

  if (CheckPars(TRIGGER, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result = XeqTrigger(owner, par, action_rec, subject_index);

    DebugLevel_2_result(result);

    /* XeqTrigger() returns either AGREE, DISAGREE, NO_MATCH or */
    /* QUIT. However, in this case, the trigger is executed as  */
    /* part of another trigger. Therefore we must handle it as  */
    /* one instruction. If the trigger returns QUIT, this       */
    /* routine will return QUIT. Otherwise it will return       */
    /* CONTINUE.                                                */

    switch (result.tag) {
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
        PrintError(79, &((resultStruct) {VALUE, NONE, result.tag}), "XeqExecute()");
        return(QUIT);
        break;
    }
    /* return(result != QUIT? CONTINUE : result); */
  }
  else
    return(QUIT);
}


int32_t XeqExit(int32_t **trigger, usrActionRec *action_rec, int32_t subject_index)
{
  /* exit(loc/obj) */

  int32_t  owner;       /* dummy */
  char *str;            /* dummy */
  int32_t  par;
  int32_t  type   = NO_TYPE;
  int32_t  result = AGREE;

  resultStruct par_list;   /* for debugging */

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(ERROR);

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = NO_ID;  /* only for common flags */
    par_list.value = par;
    DebugLevel_2_pars("exit()", &par_list, 1);
  }

  if (CheckPars(EXIT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result = Exit(par, action_rec, subject_index);

    DebugLevel_2_result( (resultStruct) {result, NONE, result});

    if (result == AGREE)
      return(OK);
    else
      return(ERROR);
  }
  else
    return(QUIT);
}


int32_t XeqIsLit(int32_t **trigger)
{
  /* IsLit(loc/obj) */
  int32_t  owner; /* dummy */
  char     *str;  /* dummy */
  int32_t  par;
  int32_t  type   = NO_TYPE;
  int32_t  result = OK;

  resultStruct par_list;  /* for debugging */

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = NO_ID;  /* only for common flags */
    par_list.value = par;
    DebugLevel_2_pars("exit()", &par_list, 1);
  }

  if (CheckPars(ISLIT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result = IsLit(par);  /* either OK or ERROR */

    DebugLevel_2_result( (resultStruct) {result, NONE, result});

    return(result);
  }
  else
    return(QUIT);
}


int32_t XeqIsObject(int32_t **trigger)
{
  /* IsObject(loc/obj) */
  int32_t  owner; /* dummy */
  char     *str;  /* dummy */
  int32_t  par;
  int32_t  type   = NO_TYPE;
  int32_t  result = OK;

  resultStruct par_list;  /* for debugging */

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = NO_ID;  /* only for common flags */
    par_list.value = par;
    DebugLevel_2_pars("exit()", &par_list, 1);
  }

  if (CheckPars(ISOBJECT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result = IsObjId(par);

    DebugLevel_2_result( (resultStruct) {result, NONE, result});

    return(result);
  }
  else
    return(QUIT);
}


int32_t XeqOwns(int32_t **trigger)
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
  int32_t  result;

  resultStruct par_list[4];  /* for debugging */

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
        nr_of_pars  = 4;
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
      depth      = 1;
      type3      = NUMBER;
      nr_of_pars = 3;
    }
  }

  if (debug_level == 2) {
    par_list[0].tag   = type1;
    par_list[0].owner = NO_ID;
    par_list[0].value = par1;
    par_list[1].tag   = type2;
    par_list[1].owner = NO_ID;
    par_list[1].value = par2;
    par_list[2].tag   = type3;
    par_list[2].owner = NO_ID;
    par_list[2].value = depth;
    par_list[3].tag   = type4;
    par_list[3].owner = NO_ID;
    par_list[3].value = preposition;
    DebugLevel_2_pars("owns():", par_list, nr_of_pars);
  }

  /* We now must test if par1 owns (contains) par2. */
  /* The depth of the containment may be max par3.  */
  if (CheckPars(OWNS, type1, type2, type3, type4, NO_TYPE)) {
    if (Owns(par1, par2, depth)) {
      /* par1 owns par2, now check if the prepositions match */
      /* r_preposition has id FIRST_COMMON_ATTR_ID.          */
      /* This is set at compile time.                        */
      index = (par2-FIRST_OBJECT_ID)*nr_of_cattrs;
      if (preposition == NO_ID || c_obj_attrs[index].value == preposition) {
        result = OK;
      }
      else {
        result = ERROR;
      }
    }
    else {
      result = ERROR;
    }

    DebugLevel_2_result( (resultStruct) {result, NONE, result} );
    return(result);
  }
  else
    return(QUIT);
}


int32_t XeqRunCommon(int32_t **trigger, usrActionRec *action_rec, int32_t subject_index, int32_t *com_trig)
{
  resultStruct result;
  int32_t      xeq_result;

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
  /* function. We can only detect this at runtime.              */

  if (com_trig == NULL) {
    PrintError(80, NULL, NULL);
    return(QUIT);
  }

  if (debug_level == 2) {
    DebugLevel_2_pars("runcommon():", NULL, 0);
  }

  result = Execute(com_trig, action_rec, subject_index, NULL);

  switch (result.tag) {
    case AGREE: ;
    case NO_MATCH:
      xeq_result = OK;
      break;
    case DISAGREE:
      xeq_result = ERROR;
      break;
    case QUIT:
      xeq_result = QUIT;
      break;
    default:
      PrintError(79, &((resultStruct) {VALUE, NONE, result.tag}), "XeqRunCommon()");
      return(QUIT);
      break;
  }

  DebugLevel_2_result( (resultStruct) {xeq_result, NONE, xeq_result} );
  return(xeq_result);
}


int32_t XeqRunVerb(int32_t **trigger, usrActionRec *action_rec, int32_t subject_index)
{
  resultStruct result;
  int32_t      xeq_result;

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

  if (debug_level == 2) {
    DebugLevel_2_pars("runverb():", NULL, 0);
  }

  result = XeqVerbDefault(action_rec, subject_index);

  switch (result.tag) {
    case AGREE: ;
    case NO_MATCH:
      xeq_result = OK;
      break;
    case DISAGREE:
      xeq_result = ERROR;
      break;
    case QUIT:
      xeq_result = QUIT;
      break;
    default:
      PrintError(79, &((resultStruct) {VALUE, NONE, result.tag}), "XeqRunVerb()");
      return(QUIT);
      break;
  }

  DebugLevel_2_result( (resultStruct) {xeq_result, NONE, xeq_result} );
  return(xeq_result);
}


int32_t XeqTstFlag(int32_t **trigger)
{
  /* Has one parameters. */
  int32_t  owner;
  char     *str; /* dummy */
  int32_t  par;
  int32_t  type = NO_TYPE;

  resultStruct par_list; /* for debugging */
  resultStruct result;   /* for debugging */

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (owner == NONE) {
    PrintError(78, NULL, "XeqTstFlag()");
    return(ERROR);
  }

  if (debug_level == 2 ) {
    par_list.tag   = type;
    par_list.owner = owner;
    par_list.value = par;
    DebugLevel_2_pars("testflag()", &(par_list), 1);
  }


  if (CheckPars(TESTFLAG, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (IsCFlagId(par)) {
      result.value = TestCFlag(owner, par);
    }
    else {
      result.value = TestLFlag(par);
    }
    result.tag   = NUMBER;
    result.owner = NO_ID;
    DebugLevel_2_result(result);

    return(result.value);
  }
  else
    return(QUIT);
}


int32_t XeqTry(int32_t **trigger)
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
      PrintError(79, &((resultStruct) {VALUE, NONE, result}), "XeqTry()");
      return(QUIT);
  }
}


int32_t XeqValDir(int32_t **trigger)
{
  /* Syntax: valdir(loc, direction). */

  int32_t  owner; /* dummy */
  char *str;      /* dummy */
  int32_t  par1;
  int32_t  par2;
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;
  int32_t  result;

  resultStruct par_list[2];  /* for debugging */

  /* Skip nr of pars. */
  NextOpcode(trigger);

  /* Read first parameter. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  /* Read second parameter. */
  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  if (debug_level == 2) {
    par_list[0].tag   = type1;
    par_list[0].owner = NO_ID;
    par_list[0].value = par1;
    par_list[1].tag   = type2;
    par_list[1].owner = NO_ID;
    par_list[1].value = par2;
    DebugLevel_2_pars("valdir():", par_list, 2);
  }

  if (CheckPars(VALID_DIRECTION, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* Test if dir is a valid direction from loc. */
    if (exit_data[(par1-FIRST_LOCATION_ID)*nr_of_directions+ par2-first_direction_id] == NO_ID) {
      /* Not a valid exit. */
      result = ERROR;
    }
    else {
      result = OK;
    }
    DebugLevel_2_result( (resultStruct) {result, NONE, result});
    return(result);
  }
  else
    return(QUIT);
}


int32_t XeqYesNo(int32_t **trigger)
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


int32_t XeqTestFun(int32_t opcode, int32_t **trigger, usrActionRec *action_rec, int32_t subject_index, int32_t *com_trig)
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
      PrintError(81, &((resultStruct) {VALUE, NONE, opcode}), NULL);
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

resultStruct XeqAddChoice(int32_t **trigger)
{
  /* addchoice(string, trigger/string) */

  int32_t owner1;
  int32_t owner2;
  char    *str1;
  char    *str2;
  int32_t par1;
  int32_t par2;
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;

  resultStruct par_list[2];  /* for debugging */

  /* skip nr_of_pars which is always 2 */
  NextOpcode(trigger);

  /* Read the parameters. */
  if (!GetPar(&owner1, &par1, &type1, &str1, trigger)) {
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  if (!GetPar(&owner2, &par2, &type2, &str2, trigger)) {
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  if (debug_level == 2) {
    par_list[0].tag   = type1;
    par_list[0].owner = owner1;
    par_list[0].value = par1;
    par_list[1].tag   = type2;
    par_list[1].owner = owner2;
    par_list[1].value = par2;
    DebugLevel_2_pars("addchoice()", par_list, 2);
  }

  if (!(CheckPars(ADDCHOICE, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE))) {
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  /* now add the parameters to the choice string    */
  /* 1 means print to json string and not to screen */

  /* we must make: "{"text":"string1", "chosen":"string2"}," */
  /* he last comma will be deleted when clsing the json      */
  /* we cannot directly use a trigger here, because the      */
  /* wildcards will be set during parsing the user input     */

  PrintString("{\"text\":\"", 1);
  PrintString(str1, 1);
  PrintString("\",\"chosen\":\"", 1);
  PrintString(str2, 1);
  PrintString("\"},", 1);

  DebugLevel_2_result( (resultStruct) {STRING, NONE, 0} );

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqAddJson(int32_t **trigger)
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

  resultStruct par_list[2];  /* for debugging */

  nr_of_pars = NextOpcode(trigger);

  /* Read the parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (nr_of_pars == 2) {
    if (!GetPar(&owner, &par2, &type2, &str, trigger))
      return( (resultStruct) {QUIT, NONE, 0} );
  }
  else
    type2 = OBJ_ID; /* obj_id or loc_id will pass */

  if (debug_level == 2) {
    par_list[0].tag   = STRING;
    par_list[0].owner = NO_ID;
    par_list[0].value = 0;
    if (nr_of_pars == 2) {
      par_list[1].tag   = type2;
      par_list[1].owner = NO_ID;
      par_list[1].value = par2;
    }
    DebugLevel_2_pars("addjson()", par_list, nr_of_pars);
  }

  if (!(CheckPars(ADDJSON, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE))) {
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  /* 1 means print to json string and not to screen */
  PrintString(str, 1);

  if (nr_of_pars == 2) {
    PrintNumber(par2, 1);
  }

  DebugLevel_2_result( (resultStruct) {STRING, NONE, 0});

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqAgree(int32_t **trigger)
 /* Caller expects rest of trigger to be returned. */
{
  /* Agree() always has 0 parameters. */
  /* Syntax in triggercode: AGREE 0   */

  resultStruct result = {AGREE, NONE, 0};

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  if (debug_level == 2) {
    DebugLevel_2_pars("agree()", NULL, 0);
    DebugLevel_2_result( (resultStruct) {AGREE, NONE, AGREE});
  }

  return(result);
}


resultStruct XeqBackground(int32_t **trigger)
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
    return( (resultStruct) {QUIT, NONE, 0} );

  if (CheckPars(BACKGROUND, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (par == LookUpId(TranslateKeyword("BLUE"))) {
      /* set background color to blue   */
      /* this is  system dependent code */

      return( (resultStruct) {CONTINUE, NONE, 0} );
    }
    if (par == LookUpId(TranslateKeyword("BLACK"))) {
      /* set background color to black */
      /* this is system dependent code */

      return( (resultStruct) {CONTINUE, NONE,0} );
    }
    PrintError(82, NULL, NULL);
    return( (resultStruct) {CONTINUE, NONE, 0} ); /* not a severe error */
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqBlockExit(int32_t **trigger)
{
  /* blockexit(loc, dir) */

  int32_t  owner; /* dummy */
  char *str;  /* dummy */
  int32_t  par1;
  int32_t  par2;
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;
  resultStruct result = {CONTINUE ,NONE, 0};

  resultStruct par_list[2];  /* for debugging */

  /* Skip nr of parameters (which will be 2). */
  NextOpcode(trigger);

  /* Read the parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (debug_level == 2) {
    par_list[0].tag   = type1;
    par_list[0].owner = NO_ID;  /* only for common flags */
    par_list[0].value = par1;
    par_list[1].tag   = type2;
    par_list[1].owner = NO_ID;  /* only for common flags */
    par_list[1].value = par2;
    DebugLevel_2_pars("blockexit()", par_list, 2);
  }

  if (CheckPars(BLOCK_EXIT, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
     /* Now block the exit. */
    exit_data[(par1-FIRST_LOCATION_ID)*nr_of_directions + par2-first_direction_id] = NO_ID;

    DebugLevel_2_result(result);

    return(result);
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqBold(int32_t **trigger)
{
  /* bold(word)                 */
  /* word must be 'on' or 'off' */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (CheckPars(BOLD, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (par == LookUpId(TranslateKeyword("ON"))) {
      /* set bold on */
      return( (resultStruct) {CONTINUE, NONE, 0} );
    }
    else if (par == LookUpId(TranslateKeyword("OFF"))) {
      /* set bold off */
      return( (resultStruct) {CONTINUE, NONE, 0} );
    }
    else {
      PrintError(83, NULL, "XeqBold()");
      return( (resultStruct) {CONTINUE, NONE, 0} ); /* not a severe error */
    }
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqClearJson(int32_t **trigger)
{
  /* 0 parameters */

  /* skip nr_of_pars */
  NextOpcode(trigger);

  /* change to: ResetString(json_msg_from_story); */

  if (debug_level == 2) {
    DebugLevel_2_pars("ClearJson()", NULL, 0);
  }

  if (json_msg_from_story) {
    free(json_msg_from_story);
    json_msg_from_story = NULL;
  }

  DebugLevel_2_result( (resultStruct) {CONTINUE, NONE, 0});

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqClearWindow(int32_t **trigger, int which_one)
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
      /* do nothing              */
      break;
    default:
      /* this should never happen */
      /* not a severe error       */
      break;
  }
  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqContents(int32_t **trigger, usrActionRec *action_rec, int32_t subject_index)
{
  /* Contents(loc/obj) always has one parameter. */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;
  resultStruct result = {CONTINUE, NONE, 0};

  resultStruct par_list;  /* for debugging */

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = NO_ID;  /* only for common flags */
    par_list.value = par;
    DebugLevel_2_pars("contents()", &par_list, 1);
  }

  if (CheckPars(CONTENTS, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result.tag = Contents(par, action_rec, subject_index);

    DebugLevel_2_result(result);

    return(result.tag != QUIT ? (resultStruct) {CONTINUE, NONE, 0} : result);
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqCount(int32_t **trigger)
{
  /* Count(loc/obj, common flag, 0/1 [,preposition]) */
  int32_t  owner;       /* dummy */
  char     *str;        /* dummy */
  int32_t  nr_of_pars;
  int32_t  par1;
  int32_t  par2;
  int32_t  par3;
  int32_t  par4;
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;
  int32_t  type3 = NO_TYPE;
  int32_t  type4 = NO_TYPE;

  int32_t  result;

  resultStruct par_list[4];  /* for debugging */

  nr_of_pars = NextOpcode(trigger);

  /* Read the parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, &par3, &type3, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (nr_of_pars == 4) {
    if (!GetPar(&owner, &par4, &type4, &str, trigger))
      return( (resultStruct) {QUIT, NONE, 0} );
  }
  else {
    par4  = 1;       /* 0 is default level */
    type4 = NUMBER;
  }

  if (debug_level == 2) {
    par_list[0].tag   = type1;
    par_list[0].owner = NO_ID;  /* only for common flags */
    par_list[0].value = par1;
    par_list[1].tag   = type2;
    par_list[1].owner = NO_ID;
    par_list[1].value = par2;
    par_list[2].tag   = type3;
    par_list[2].owner = NO_ID;
    par_list[2].value = par3;
    par_list[3].tag   = type4;
    par_list[3].owner = NO_ID;
    par_list[3].value = par4;
    DebugLevel_2_pars("count()", par_list, 4);
  }

  if (CheckPars(COUNT, type1, type2, type3, type4, NO_TYPE)) {
    /* types are correct, now check whether */
    /* par3 is either 0 or 1.               */
    if (par3 != 0 && par3 != 1) {
      outputline = ResetString(outputline);
      PrintString("count(): parameter 3 must be 0 or 1.\n", 0);
      Output();
      return( (resultStruct) {QUIT, NONE, 0} );
    }
    /* level must be greater than 0 */
    if (par4 < 0) {
      par4 = 1;   /* set to default level */
    }

    result = CountObjects(par1, par2, par3, par4);

    DebugLevel_2_result( (resultStruct) {NUMBER, NONE, result});

    return( (resultStruct) {VALUE, NONE, result} );
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqDebug(int32_t **trigger)
{
  int32_t  owner;       /* dummy */
  char     *str;        /* dummy */
  int32_t  par;
  int32_t  type = NO_TYPE;

  /* Skip nr of pars, debug always has 1 parameter. */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (CheckPars(DEBUG, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (debug_info == 0) {
      PrintError(47, NULL, NULL);
    }
    else {
      switch (par) {
        case 0:
          /* turn off debug */
          debug_level = 0;
          break;
        case 1:
          debug_level = 1;
          break;
        case 2:
          debug_level = 2;
          break;
        default:
          /* do nothing */
          break;
      }
    }
  }

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqDest(int32_t **trigger)
{
  /* Syntax: dest(loc/obj, direction) */

  int32_t  owner;   /* dummy */
  char     *str;    /* dummy */
  int32_t par1;
  int32_t par2;
  int32_t type1  = NO_TYPE;
  int32_t type2  = NO_TYPE;
  resultStruct result = {LOC_ID, NONE, NONE};

  resultStruct par_list[2];   /* for debugging */

  /* Skip nr_of_pars, which will be 2. */
  NextOpcode(trigger);

  /* Read parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  /* check for NONE parameters */
  /* will not be caught by CheckPars() because type may */
  /* be correct type for NONE-parameter. */
  if (par1 == NONE || par2 == NONE) {
    PrintError(78, NULL, "XeqDest()");
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  if (CheckPars(DEST, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* convert par1 to its  containing     */
    /* locations in  case it is an object. */
    while (!IsLocId(par1))
      par1 = obj_dir[par1-FIRST_OBJECT_ID].held_by;

    /* find the destination when moving from par1 */
    /* in direction par2                          */

    result.value = exit_data[(par1-FIRST_LOCATION_ID)*nr_of_directions +
                            (par2-first_direction_id)];

    if (debug_level == 2) {
      par_list[0].tag   = type1;
      par_list[0].owner = NO_ID;  /* only for common flags */
      par_list[0].value = par1;
      par_list[1].tag   = type2;
      par_list[1].owner = NO_ID;  /* only for common flags */
      par_list[1].value = par2;
      DebugLevel_2_pars("dest()", par_list, 2);
    }

    /* return(result); */   /* @!@ */

    /* SHOULD WE USE VALUE OR LOC_ID HERE? */
    return(result.value == NO_ID ? (resultStruct) {LOC_ID, NONE, NONE} : result);
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqDisagree(int32_t **trigger)
{
  /* Disagree() always has 0 parameters. */
  /* Syntax in triggercode: DISAGREE 0   */

  resultStruct result = {DISAGREE, NONE, 0};

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  if (debug_level == 2) {
    DebugLevel_2_pars("disagree()", NULL, 0);
  }
  return(result);
}


resultStruct XeqDistance(int32_t **trigger, int32_t return_value)
 /* return_value is DISTANCE or FIRSTDIR */
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

  int32_t  *route  = NULL;
  spanTree *tree   = NULL;

  resultStruct result;

  resultStruct par_list[2];   /* for debugging */

  /* Initialize fun_name. */
  fun_name[0] = '\0';

  /* Skip nr_of_pars, which will be 2. */
  NextOpcode(trigger);

  /* set function name for error msg */
  switch (return_value) {
    case DISTANCE:
      strncpy(fun_name, "distance()",MAX_WORD_LEN);
      result.tag   = VALUE;
      result.value = -1;
      break;
    case FIRSTDIR:
      strncpy(fun_name, "firstdir()",MAX_WORD_LEN);
      result.tag   = NONE;
      result.value = 0;
      break;
    default:
      PrintError(79, NULL, "XeqDistance()");
      return( (resultStruct) {QUIT, NONE, 0} );
  }

  /* Read parameters. */
  if (!GetPar(&owner, &from, &type1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, &to, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (debug_level == 2) {
    par_list[0].tag   = type1;
    par_list[0].owner = NO_ID;  /* only for common flags */
    par_list[0].value = from;
    par_list[1].tag   = type2;
    par_list[1].owner = NO_ID;  /* only for common flags */
    par_list[1].value = to;
    DebugLevel_2_pars(fun_name, par_list, 2);
  }

  /* check for NONE parameters */
  /* will not be caught by CheckPars() because type */
  /* may be correct type for NONE-parameter.        */
  if (from == NONE || to == NONE) {
    PrintError(78, NULL, fun_name);
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  if (!CheckPars(return_value, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE))
    return( (resultStruct) {QUIT, NONE, 0} );

/* WHAT IF THE NUMBER OF HOPS IS EQUAL TO THE QUIT CODE??? */
/* => fixed that by using resultStruct                     */

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
    result = (return_value == DISTANCE ? (resultStruct) {VALUE, NONE, 0} : (resultStruct) {NONE, NONE, 0});
    free(tree);
    free(route);
    return(result);
  }

  /* Initialize spanning tree and route. */
  if (!InitSpanTree(&tree, &route))
    return( (resultStruct) {QUIT, NONE, 0} );

  /* prepare tree for running ExpandTree */
  tree[from-FIRST_LOCATION_ID].level = 0;

  while (level<nr_of_locs && !found_route) {
    if (ExpandTree(level, tree)) {
      /* changes were made to the tree */
      if (tree[to-FIRST_LOCATION_ID].from_loc != NO_ID) {
        /* a route was found */
        found_route = 1;
        if (!BuildRoute(to, route, tree))
          return( (resultStruct) {QUIT, NONE, 0} );
      }
    }
    level++;
  } /* while */

  if (found_route) {
    /* There is a route */
    if (return_value == DISTANCE) {
      result = (level == 0 ? (resultStruct) {VALUE, NONE, 0} : (resultStruct) {VALUE, NONE, level-1} ); /* level-1 is nr of hops to dest */
    }
    else { /* FIRSTDIR */
      /* determine first direction to go to */
      while (i<nr_of_directions && !found_dir) {
         /* route[level-2] is first dir after 'from' location */
        if (exit_data[(from-FIRST_LOCATION_ID)*nr_of_directions+i] == route[level-2]) {
          result = (resultStruct) {DIRECTION, NONE, i + first_direction_id};
          found_dir = 1;
        }
        else
          i++;
      }
    }
  }
  else
    /* no route found, so no distance or first direction*/
    result = return_value == DISTANCE ? (resultStruct) {VALUE, NONE, -1} : (resultStruct) {NONE, NONE, 0};

  DebugLevel_2_result(result);

  /* Free the spanning tree and route. */
  free(tree);
  free(route);
  return(result);
}


resultStruct XeqEntrance(int32_t **trigger, usrActionRec *action_rec, int32_t subject_index)
{
  /* Entrance(loc/obj) always has one parameter. */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;
  resultStruct result = {CONTINUE, NONE, 0};

  resultStruct par_list;  /* for debugging */

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = NO_ID;  /* only for common flags */
    par_list.value = par;
    DebugLevel_2_pars("contents()", &par_list, 1);
  }

  if (CheckPars(ENTRANCE, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result.tag = Entrance(par, action_rec, subject_index);

    DebugLevel_2_result(result);

    return(result.tag != QUIT ? (resultStruct) {CONTINUE, NONE, 0} : result);
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqFlagVal(int32_t **trigger, int32_t value)
{
  int32_t      owner;
  char         *str;   /* dummy */
  int32_t      par;
  int32_t      type = NO_TYPE;
  resultStruct result;

  resultStruct par_list;  /* for debugging */

  /* Skip nr of pars (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (debug_level == 2) {
    par_list.tag   = FLAG_ID;
    par_list.owner = owner;  /* only for common flags */
    par_list.value = par;
    DebugLevel_2_pars("setflag()", &par_list, 1);
  }

  /* clearflag() has same syntax as setflag */
  if (CheckPars(SETFLAG, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (IsCFlagId(par)) {
      /* write undo info */
      PushUndoItem(FLAGS, par, owner, NO_ID, NO_ID, TestCFlag(owner, par));  /* @!@ */

      /* Set the value of the flag to `value'.   */
      ProcCFlagVal(owner, par, value);
    }
    else {
      /* Local flag. */
      /* write undo info */
      PushUndoItem(FLAGS, par, owner, NO_ID, NO_ID, TestLFlag(par));  /* @!@ */

      /* Set the value of the flag to `value'. */
      ProcLFlagVal(par, value);
    }

    result = (resultStruct) {CONTINUE, NONE, 0};
  }
  else {
    result = (resultStruct) {QUIT, NONE, 0};
  }

  DebugLevel_2_result(result);

  return(result);
}


resultStruct XeqGetAnswer(int32_t **trigger)
{
  /* Syntax: getanswer() */

  /* Skip number of parameters. */
  NextOpcode(trigger);

  return( (resultStruct) {GET_ANSWER, NONE, 0} );
}


resultStruct XeqGetSubjectOrSpec(int32_t **trigger, int32_t subj_spec)
 /* subj_spec is either GET_SUBJECT or GET_SPEC */
{
  /* Syntax: getsubject([word_id]) or */
  /* getspec([word_id])               */

  int32_t      owner;      /* dummy */
  char         *str;       /* dummy */
  char         function[15];
  int32_t      nr_of_pars;
  int32_t      par  = NO_ID;
  int32_t      type = WORD_ID /* make checkpar() succeed with 0 parameters */;
  resultStruct result;

  resultStruct par_list;  /* for debugging */

  if (subj_spec == GET_SUBJECT) {
    strcpy(function, "getsubject()");
  }
  else {
    strcpy(function, "getspec()");
  }

  /* Get number of parameters. */
  nr_of_pars = NextOpcode(trigger);

  if (nr_of_pars == 1) {
    /* Read the parameter. */
    if (!GetPar(&owner, &par, &type, &str, trigger))
      return( (resultStruct) {QUIT, NONE, 0} );
    }

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = owner;  /* only for common flags */
    par_list.value = par;
    DebugLevel_2_pars(function, &par_list, nr_of_pars);
  }

  if (!CheckPars(subj_spec, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result = (resultStruct) {QUIT, NONE, 0};
  }
  else {
    /* if nr_of_pars was 0, par will have value NO_ID */
    result = (resultStruct) {subj_spec, NONE, par};
  }

  DebugLevel_2_result(result);

  return(result);
}


resultStruct XeqGoTo(int32_t **trigger)
{
  int32_t      owner;
  char         *str;           /* dummy */
  int32_t      nr_of_pars;
  int32_t      par1;           /* Object that must be moved. */
  int32_t      par2;           /* Location to move par1 to.  */
  int32_t      par3;           /* Number of moves.           */
  int32_t      type1 = NO_TYPE;
  int32_t      type2 = NO_TYPE;
  int32_t      type3 = NO_TYPE;
  resultStruct result;

  int32_t      from_loc;
  int32_t      level       = 1; /* first level to expand the tree to */
  int32_t      i           = 0;
  int32_t      found_route = 0;
  int32_t      stop        = 0;
  int32_t      *route      = NULL;
  spanTree     *tree       = NULL;

  resultStruct par_list[3];  /* for debugging */

  /* Read number of parameters. */
  nr_of_pars = NextOpcode(trigger);

  /* Read the parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (nr_of_pars == 3) {
    if (!GetPar(&owner, &par3, &type3, &str, trigger))
      return( (resultStruct) {QUIT, NONE, 0} );
  }
  else {
    /* default is one move at a time. */
    par3 = 1;
    type3 = NUMBER;
  }

  if (debug_level == 2) {
    par_list[0].tag   = type1;
    par_list[0].owner = NONE;
    par_list[0].value = par1;
    par_list[1].tag   = type2;
    par_list[1].owner = NONE;
    par_list[1].value = par2;
    par_list[2].tag   = type3;
    par_list[2].owner = NONE;
    par_list[2].value = par3;
    DebugLevel_2_pars("goto()", par_list, 3);
  }

  if (CheckPars(GO_TO, type1, type2, type3, NO_TYPE, NO_TYPE)) {
    /* Load from_loc. */
    from_loc = par1;
    do
      from_loc = obj_dir[from_loc-FIRST_OBJECT_ID].held_by;
    while (!IsLocId(from_loc));

    /* Initialize spanning tree and route. */
    if (!InitSpanTree(&tree, &route))
      return( (resultStruct) {QUIT, NONE, 0} );

    /* prepare tree for running ExpandTree */
    tree[from_loc-FIRST_LOCATION_ID].level = 0;

    while (level<nr_of_locs && !found_route) {
      if (ExpandTree(level, tree)) {
        /* changes were made to the tree */
        if (tree[par2-FIRST_LOCATION_ID].from_loc != NO_ID) {
          /* a route was found */
          found_route = 1;
          if (!BuildRoute(par2, route, tree))
            return( (resultStruct) {QUIT, NONE, 0} );
        }
      }
      level++;
    } /* while */

    if (!found_route) {
      /* no route found */
      PrintError(84, NULL, NULL);
      result = (resultStruct) {CONTINUE, NONE, 0};
    }
    else {
      /* There is a route, move par1.             */
      /* par3 denotes max nr of moves, stop if 0. */

      /* first, write undo info                   */
      PushUndoItem(MOVE, par1, NO_ID, NO_ID, NO_ID, obj_dir[par1-FIRST_OBJECT_ID].held_by);  /* @!@ */
      PushUndoItem(CURR_LOC, NO_ID, NO_ID, NO_ID, NO_ID, curr_loc);  /* @!@ */

      while (i >= 0 && par3 != 0 && !stop) {
        /* printf("%d Moving ", i);PrintId(par1);printf(" to ");PrintId(route[level-2]);printf("\n"); */
        if (!Move(par1, route[level-2-i]))
          return( (resultStruct) {QUIT, NONE, 0} );
        if (par1 == PLAYER || Owns(par1, PLAYER, -1))
          /* Update current location. */
          curr_loc = route[level-2-i];
        if (route[level-2-i] == par2)
          stop = 1;
        i--;
        par3--;
      }
    }

    result = (resultStruct) {CONTINUE, NONE, 0};
  }
  else {  /* CheckPars() failed */
    result = (resultStruct) {QUIT, NONE, 0};
  }

  /* Free the spanning tree and route. */
  free(tree);
  free(route);

  DebugLevel_2_result(result);

  return(result);
}


resultStruct XeqHitAnyKey(int32_t **trigger)
{
  /* HitAnyKey() always has 0 parameters. */

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  if (debug_level == 2) {
    DebugLevel_2_pars("hitanykey():", NULL, 0);
  }

  /* flush any text still in the output buffer */
  Output();

  switch(story_info.story_language) {
    case NL:
      NL_XeqHitAnyKey();
      break;
    case ENG:
      ENG_XeqHitAnyKey();
      break;
    default:
      /* unknown language, use English */
      ENG_XeqHitAnyKey();
      break;
  }

  DebugLevel_2_result( (resultStruct) {CONTINUE, NONE, 0} );

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqIndent(int32_t **trigger)
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
        PrintError(86, &((resultStruct) {VALUE, NONE, indent}), NULL);
        return( (resultStruct) {ERROR, NONE, 0} );
      }
      else {
        /* build a string for PrintString(), who will check when */
        /* to wrap around the line.                              */
        for (par = 0; par<indent; par++)
          spaces_string[par] = SPACE;
        spaces_string[par] = '\0';
        PrintString(spaces_string, 0);
      }
      break;
    case 1:
      /* 1 parameter. */
      if (!GetPar(&owner, &par, &type, &str, trigger))
        return( (resultStruct) {QUIT, NONE, 0} );
      if (CheckPars(INDENT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
        indent += par;
        /* correct negative value */
        indent = (indent<0? 0 : indent);
      }
      else
        return( (resultStruct) {QUIT, NONE, 0} );
      break;
    default:
      PrintString("XeqIndent(): error.\n", 0);
      Output();
      break;
  } /* switch */
  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqItalic(int32_t **trigger)
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
    return( (resultStruct) {QUIT, NONE, 0} );

  if (CheckPars(ITALIC, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (par == LookUpId(TranslateKeyword("ON"))) {
     /* set italic on */
     return( (resultStruct) {CONTINUE, NONE, 0} );
    }
    else if (par == LookUpId(TranslateKeyword("OFF"))) {
      /* set italic off */
      return( (resultStruct) {CONTINUE, NONE, 0} );
    }
    else {
      PrintError(83, NULL, "XeqItalic()");
      return( (resultStruct) {CONTINUE, NONE, 0} ); /* not a severe error */
    }
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqMove(int32_t **trigger)  /* @!@ */
{
  /* Syntax: move(obj, loc/obj/direction [, preposition]) */

  int32_t      owner;          /* dummy */
  char         *str;           /* dummy */
  int          nr_of_pars = 0;
  int          index      = -1;
  int32_t      dest;
  int32_t      par1  = NO_ID;
  int32_t      par2  = NO_ID;
  int32_t      par3  = NO_ID;
  int32_t      type1 = NO_TYPE;
  int32_t      type2 = NO_TYPE;
  int32_t      type3 = NO_TYPE;
  resultStruct result;

  resultStruct par_list[3];  /* for debugging */

  nr_of_pars = NextOpcode(trigger);

  /* Read parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (nr_of_pars == 3) {
    if (!GetPar(&owner, &par3, &type3, &str, trigger))
      return( (resultStruct) {QUIT, NONE, 0} );
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

  if (debug_level == 2) {
    par_list[0].tag   = type1;
    par_list[0].owner = NONE;
    par_list[0].value = par1;
    par_list[1].tag   = type2;
    par_list[1].owner = NONE;
    par_list[1].value = par2;
    par_list[2].tag   = type3;
    par_list[2].owner = NONE;
    par_list[2].value = par3;
    DebugLevel_2_pars("move()", par_list, 3);
  }

  /* check for NONE parameters */
  /* will not be caught by CheckPars() because type may */
  /* be correct type for NONE-parameter. */
  if (par1 == NONE || par2 == NONE) {
    PrintString("XeqMove(): NONE-parameter.\n", 0);
    Output();
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  if (CheckPars(MOVE, type1, type2, type3, NO_TYPE, NO_TYPE)) {
    /* Test for identical parameters. */
    if (par1 == par2) {
      PrintError(85, NULL, NULL);
      DebugLevel_2_result( (resultStruct) {DISAGREE, NONE, 0});
      return( (resultStruct) {DISAGREE, NONE, 0} );
    }

    if (!(IsLocId(par2) || IsObjId(par2)))
      /* Par2 is a direction; convert it to a location. */
      if ((par2 = exit_data[(curr_loc-FIRST_LOCATION_ID)*nr_of_directions+
                       par2-first_direction_id]) == NO_ID) {
        PrintString("XeqMove(): Invalid direction.\n", 0);
        Output();
        return( (resultStruct) {QUIT, NONE, 0} );
      }

     /* first, write undo info */
     /* par1 must be moved back to its original owner */
     PushUndoItem(MOVE, par1, NO_ID, NO_ID, NO_ID, obj_dir[par1-FIRST_OBJECT_ID].held_by);

    /* Par1 (obj) must be moved into par2 (obj or loc). */
    if (!Move(par1, par2))
      return( (resultStruct) {QUIT, NONE, 0} );

    /* Check if preposition of par1 must be updated */
    /* r_preposition has id FIRST_COMMON_ATTR_ID.   */
    /* This is set at compile time.                 */
    if (nr_of_pars == 3) {
      index = (par1-FIRST_OBJECT_ID)*nr_of_cattrs;

      /* first, write undo info */
      /* action type, attr id, owner, value_owver, type, value */
      PushUndoItem(ATTRIBUTES, FIRST_COMMON_ATTR_ID, par1, NO_ID, WORD_ID, c_obj_attrs[index].value);

      c_obj_attrs[index].value = par3;
    }

    /* if the player is moved, we must update curr_loc */
    /* scenarios are:                                  */
    /* - player is moved to a location                 */
    /* - object with the player in it is moved to a    */
    /*   location                                      */
    /* - player is moved to an object in another       */
    /*   location                                      */
    /* - object with the player in it is moved to an   */
    /*   object in another location                    */
    dest = par2;
    if (par1 == PLAYER || Owns(par1, PLAYER, -1)) {
      while (!IsLocId(dest)) {
        dest = obj_dir[dest-FIRST_OBJECT_ID].held_by;
      }
      /* first, write undo info */
      PushUndoItem(CURR_LOC, NO_ID, NO_ID, NO_ID, NO_ID, curr_loc);

      curr_loc = dest;
    }

    result = (resultStruct) {CONTINUE, NONE, 0};
  }
  else {
    result = (resultStruct) {QUIT, NONE, 0};
  }

  DebugLevel_2_result( (resultStruct) {DISAGREE, NONE, 0});  /* @!@ */

  return(result);
}


resultStruct XeqNewDSys(int32_t **trigger)
{
  /* Syntax: newdsys(loc/obj, string) */

  /* THIS FUNCTION IS NOT USED AND NOT YET FINISHED */

  int32_t  owner;
  int32_t  par1;
  int32_t  par2;
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;

  char *str      = NULL;
  int  len;
  char *new_dsys = NULL;

  /* Skip the number of parameters, which will be 2. */
  NextOpcode(trigger);

  /* Read the parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!CheckPars(NEWDSYS, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  /* check if there actually is a string with text in it */
  len = strlen(str);
  if (str == NULL || len == 0) {
    /* empty or no string, just continue */
    return( (resultStruct) {CONTINUE, NONE, 0} );
  }

  /* str contains the string with the new d_sys. We must expand  */
  /* it first, in case there are attributes in there.            */
  /* we expand the string by printing str and copying the result */
  /* from the outputline to the new_dys string.                  */

  /* assign space to new_dsys */
  if ( (new_dsys = (char *) malloc(len*sizeof(char))) == NULL) {
    PrintError(15, NULL, "newdsys()");
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  /* first clear the outputline */
  Output();

  /* now expand str, in case it contains parameters */
  PrintString(str, 0);

  len = strlen(outputline);

  if (len > OUTPUT_LINE_LEN) {
    PrintError(103, &((resultStruct) {NUMBER, NONE, len}), outputline);
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  strncpy(new_dsys, outputline, len);
  new_dsys[len] = '\0';

  outputline = ResetString(outputline);

  /* ok, now we have the text for the new system description */

  return( (resultStruct) {CONTINUE, NONE, 0} );

}


resultStruct XeqNewExit(int32_t **trigger)
{
  /* newexit(loc, dir, loc) */

  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par[3];
  int32_t type[3];
  int32_t i;
  resultStruct result;  /* @!@ */

  resultStruct par_list[3];  /* for debugging */  /* @!@ */

  for (i=0; i<3; i++)
    type[i] = NO_TYPE;

  /* Skip nr of parameters, which will be 3. */
  NextOpcode(trigger);

  /* Read the parameters. */
  for (i=0; i<3; i++) {
    if (!GetPar(&owner, &(par[i]), &(type[i]), &str, trigger))
      return( (resultStruct) {QUIT, NONE, 0} );
  }

  if (debug_level == 2) {  /* @!@ */
    par_list[0].tag   = type[0];
    par_list[0].owner = NO_ID;  /* only for common flags */
    par_list[0].value = par[0];
    par_list[1].tag   = type[1];
    par_list[1].owner = NO_ID;
    par_list[1].value = par[1];
    par_list[2].tag   = type[2];
    par_list[2].owner = NO_ID;
    par_list[2].value = par[2];
    DebugLevel_2_pars("newexit()", par_list, 3);
  }

  if (CheckPars(NEW_EXIT, type[0], type[1], type[2], NO_TYPE, NO_TYPE)) {
    /* Now create the new exit. */
    exit_data[(par[0]-FIRST_LOCATION_ID)*nr_of_directions +
              par[1]-first_direction_id] = par[2];

    result = (resultStruct) {CONTINUE, NONE, 0};  /* @!@ */
  }
  else {
    result = (resultStruct) {QUIT, NONE, 0};  /* @!@ */
  }

  DebugLevel_2_result(result);  /* @!@ */

  return(result);
}


resultStruct XeqNoMatch(int32_t **trigger)
{
  /* NoMatch() always has 0 parameters. */
  /* Syntax in triggercode: NOMATCH 0   */

  resultStruct result = {NO_MATCH, NONE, 0};

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  if (debug_level == 2) {
    DebugLevel_2_pars("nomatch()", NULL, 0);
    DebugLevel_2_result( (resultStruct) {NO_MATCH, NONE, NO_MATCH});
  }
  return(result);
}


resultStruct XeqNoTimers(int32_t **trigger)
{
  /* syntax: notimers() */
  /* Always has zero parameters. */

  resultStruct result = {CONTINUE, NONE, 0};

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* disable timers for the next move */
  fire_timers = ERROR;

  if (debug_level == 2) {
    DebugLevel_2_pars("notimers()", NULL, 0);
  }

  return(result);
}


resultStruct XeqOwner(int32_t **trigger)
{
  /* syntax: owner(obj) */

  int32_t      owner;   /* dummy */
  char         *str;    /* dummy */
  int32_t      par;
  int32_t      type = NO_TYPE;
  resultStruct result;

  resultStruct par_list;  /* for debugging */

  /* Skip nr of parameters, which will be 1. */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (debug_level == 2) {  /* @!@ */
    par_list.tag = type;
    par_list.owner = owner;  /* only for common flags */
    par_list.value = par;
    DebugLevel_2_pars("owner()", &par_list, 1);
  }

  if (CheckPars(OWNER, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* Test for none-parameter. */
    if (par == NONE) {
      /* None cannot have an owner, so NONE */
      result = (resultStruct) {NONE, NONE, 0};
    }
    else {
      if (!IsObjId(par)) {
        /* only object ids can have an owner, so NONE */
        result = (resultStruct) {NONE, NONE, 0};
      }
      else {
        result.value = obj_dir[par-FIRST_OBJECT_ID].held_by;
        result.tag = IsObjId(result.value) ? OBJ_ID : LOC_ID;
      }
    }

    DebugLevel_2_result(result);

    return(result);
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqPickOne(int32_t **trigger)
{
  /* syntax: pickone(par_1, par_2, ....., par_n) */

  int32_t      owner;
  char         *str;    /* dummy */
  int32_t      par;
  int32_t      type;
  int32_t      nr_of_pars;
  int          i;
  int          rnd_par;
  resultStruct result;

  resultStruct *par_list = NULL;  /* for debugging */

  /* the nr of parameters for this function is not predetermined */

  /* Read nr of parameters. */
  nr_of_pars = NextOpcode(trigger);

  if (nr_of_pars == 0) {
    if (debug_level == 2) {
      DebugLevel_2_pars("pickone()", NULL, 0);
      DebugLevel_2_result( (resultStruct) {NONE, NONE, 0});
    }
    return( (resultStruct) {NONE, NONE, 0} );
  }

  /* determine a random parameter between 0 and nr_of_pars-1, borders inclusive */
  rnd_par = rand()%nr_of_pars;

  if ( (par_list = (resultStruct*) malloc(nr_of_pars * sizeof(resultStruct))) == NULL) {
    PrintError(15, NULL, "pickone()");
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  /* Read all parameters and store the one we need. */
  for (i=0; i<nr_of_pars; i++) {
    if (!GetPar(&owner, &par, &type, &str, trigger)) {
      return( (resultStruct) {QUIT, NONE, 0} );
    }
    if (i == rnd_par) {
      result.tag   = type;
      result.owner = owner; /* for descriptions, triggers, etc */
      result.value = par;
    }
    if (debug_level == 2) {
      par_list[i].tag   = type;
      par_list[i].owner = owner;
      par_list[i].value = par;
    }
  }

  if (debug_level == 2) {
    DebugLevel_2_pars("pickone()", par_list, nr_of_pars);
  }

  DebugLevel_2_result(result);

  return(result);
}


resultStruct XeqPlayMode(int32_t **trigger)
{
  /* syntax: playmode(word)                           */
  /* word must be 'interpreter', 'choice' or 'hybrid' */

  int32_t owner; /* dummy */
  char    *str;  /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!CheckPars(PLAYMODE, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    return( (resultStruct) {QUIT, NONE, 0} );
  }

  if (par == LookUpId(TranslateKeyword("INTERPRETER"))) {
    story_info.play_mode = INTERPRETER_MODE;
  }
  else {
    if (par == LookUpId(TranslateKeyword("CHOICE"))) {
      story_info.play_mode = CHOICE_MODE;
    }
    else {
      if (par == LookUpId(TranslateKeyword("HYBRID"))) {
        story_info.play_mode = HYBRID_MODE;
      }
      else {
        PrintError(83, NULL, "XeqPlayMode()");
        return( (resultStruct) {QUIT, NONE, 0} );
      }
    }
  }

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqPrt(int32_t **trigger)
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
    return( (resultStruct) {QUIT, NONE, 0} );

  if (CheckPars(PRINT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (type == STRING)
      PrintString(str, 0);

    else if (type == NUMBER)
      PrintNumber(par, 0);

    else if (owner == NONE || (type == NO_TYPE && par == NONE))
	  /* print nothing. */
      return( (resultStruct) {CONTINUE, NONE, 0} );

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
    return( (resultStruct) {CONTINUE, NONE, 0} );
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqPrtcr(int32_t **trigger)
{
  XeqPrt(trigger);
  PrintString("\n", 0);

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqPrtStat(int32_t **trigger)
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
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!CheckPars(PRINT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE))
    return( (resultStruct) {QUIT, NONE, 0} );
  else
    return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqQuit(int32_t **trigger)
{
  /* Always has zero parameters. */

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  if (debug_level == 2) {
    DebugLevel_2_pars("quit()", NULL, 0);
    DebugLevel_2_result( (resultStruct) {QUIT, NONE, QUIT});
  }

/*  ifi_emitResponse("{\"choice\":[{\"text\":{\"text\":\"Hit a key\",\"color\":\"blue\"},\"chosen\":\"{}\"}]}");*/

  return( (resultStruct) {QUIT, NONE, 0} );

}


resultStruct XeqRestart(int32_t **trigger)
{
  /* Restart() always has 0 parameters. */
  /* Syntax in triggercode: RESTART 0   */

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  if (debug_level == 2) {
    DebugLevel_2_pars("restart()", NULL, 0);
    DebugLevel_2_result( (resultStruct) {CONTINUE, NONE, CONTINUE});
  }

  if (!Restart()) {
     return( (resultStruct) {QUIT, NONE, 0} );
  }
  else {
    return( (resultStruct) {CONTINUE, NONE, 0} );
  }
}


resultStruct XeqRestore(int32_t **trigger)
{
  /* Syntax: restore() */
  /* Always has zero parameters. */

  /* Skip nr of parameters.      */
  NextOpcode(trigger);

  if (debug_level == 2) {
    DebugLevel_2_pars("restore()", NULL, 0);
  }

  char json_string[] = "{\"loaddata\":\"\"}";

  /* only send a {"loaddata":""} json message to the front-end    */
  /* the front-end's response with the save data will be handled  */
  /* by XeqIFIrequest() in ifi.c                                  */

  ifi_emitResponse(json_string);

  DebugLevel_2_result( (resultStruct) {CONTINUE, NONE, 0});

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqSave(int32_t **trigger)
{
  /* Syntax: save()              */
  /* Always has zero parameters. */

  char *json_save  = NULL;
  char *game_state = NULL;

  /* print the save message from the verb */
  Output();

  /* Skip nr of parameters.      */
  NextOpcode(trigger);

  if (debug_level == 2) {
    DebugLevel_2_pars("save()", NULL, 0);
  }

  /* send the following json string format to the front-end: */
  /* {"savedata:{data:"blob"}"} (we do not send a filename)  */

  /* start the json string */
  if ( (json_save = AddToString(json_save, "{\"savedata\":{\"data\":\"")) == NULL)
    return( (resultStruct) {ERROR, NONE, 0} );

  /* get the game state (save info) in a base64 encoded string */
  if ( (game_state = Base64Save(game_state)) == NULL) {
    /* something went wrong, error was already printed */
    json_save = ResetString(json_save);
    return( (resultStruct) {ERROR, NONE, 0} );
  }

  /* add the game state to the json string */
  if ( (json_save = AddToString(json_save, game_state)) == NULL) {
    json_save = ResetString(game_state);
    return( (resultStruct) {ERROR, NONE, 0} );
  }

  /* end the json string */
  if ( (json_save = AddToString(json_save, "\"}}")) == NULL)
    return( (resultStruct) {ERROR, NONE, 0} );

  /* send to front-end */
  ifi_emitResponse(json_save);

  free(json_save);

  DebugLevel_2_result( (resultStruct) {CONTINUE, NONE, 0});

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqScore(int32_t **trigger)
{
  int32_t      owner;   /* dummy */
  char         *str;    /* dummy */
  int32_t      par;     /* score */
  int32_t      type = NO_TYPE;
  resultStruct result;

  resultStruct par_list;  /* for debugging */

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
    return( (resultStruct) {QUIT, NONE, 0} );

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = NONE;
    par_list.value = par;
    DebugLevel_2_pars("score()", &par_list, 1);
  }

  if (CheckPars(SCORE, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /*return( (resultStruct) {CONTINUE, NONE, 0} );*/
    /*return(par);*/
    disambig_score += par;
    result = (resultStruct) {CONTINUE, NONE, 0};
  }
  else {
    result =  (resultStruct) {QUIT, NONE, 0};
  }

  DebugLevel_2_result(result);

  return(result);
}


resultStruct XeqSendJson(int32_t **trigger)
{
  char    *json_key_1 = NULL;
  char    *json_key_2 = NULL;
  int32_t request     = IFI_NO_IFI;;
  int     index       = 1; /* skip the '{' */

  /* 0 parameters */

  NextOpcode(trigger);

  if (debug_level == 2) {
    DebugLevel_2_pars("sendjson()", NULL, 0);
  }

  /* check if the front-end wants to receive this json */
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

  DebugLevel_2_result( (resultStruct) {CONTINUE, NONE, 0});

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqSetAttribute(int32_t **trigger)
{
  int32_t      par1;            /* for attribute id                               */
  int32_t      owner1;          /* par1 owner                                     */
  int32_t      type1 = NO_TYPE; /* par1 type                                      */
  int32_t      par2;            /* for value that must be stored in the attribute */
  int32_t      owner2;          /* par2 owner                                     */
  int32_t      type2=NO_TYPE;   /* par2 type                                      */
  char         *str;            /* dummy                                          */
  resultStruct result;

  attrInfo     *attributes;    /* No malloc(), we only need the pointer          */
  int32_t      attribute_index;

  resultStruct par_list[2];  /* for debugging */

  /* Syntax: setattribute([loc/obj].attribute, loc/obj/attribute/direction/value/word) */

  /* Skip number of parameters (which will always be 2). */
  NextOpcode(trigger);

  /* Read first parameter, which will be the attribute id. */
  /* We must use GetLvaluePar() here, because otherwise    */
  /* GetPar() will return the attribute's contents and not */
  /* the attribute itself.                                 */

   if (!GetLvaluePar(&owner1, &par1, &type1, &str, trigger))
     return( (resultStruct) {QUIT, NONE, 0} );

  /* Read second parameter.                                     */
  /* In case the second par is an attribute, its value and type */
  /* will be returned by GetPar().                              */
  if (!GetPar(&owner2, &par2, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (debug_level == 2) {
    par_list[0].tag   = type1;
    par_list[0].owner = owner1;
    par_list[0].value = par1;
    par_list[1].tag   = type2;
    par_list[1].owner = owner2;
    par_list[1].value = par2;
    DebugLevel_2_pars("setattribute()", par_list, 2);
  }

  /* Retrieve the par1 attribute info struct */
  if (!GetAttributeInfo(par1, owner1, &attributes, &attribute_index))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (CheckPars(SETATTRIBUTE, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* write undo info */
    PushUndoItem(ATTRIBUTES,
                 par1,
                 owner1,
                 attributes[attribute_index].value_owner,
                 attributes[attribute_index].type,
                 attributes[attribute_index].value);  /* @!@ */

    /* Store par2 in attribute for par1. */
    /* 10 march2017: also set owner in case it is a description id */
    attributes[attribute_index].value_owner = owner2;
    attributes[attribute_index].type        = type2;
    attributes[attribute_index].value       = par2;
    result = (resultStruct) {CONTINUE, NONE, 0};
  }
  else {
    result = (resultStruct) {QUIT, NONE, 0};
  }

  DebugLevel_2_result(result);

  return(result);
}


resultStruct XeqSetCursor(int32_t **trigger)
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
    return( (resultStruct) {QUIT, NONE, 0} );

  /* It may be a timer id */
  if (IsTimerId(par1)) {
    index = par1-FIRST_TIMER_ID;
    par1 = timers[index].value;
    type1 = NUMBER;
  }

  /* Read second parameter. */
  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  /* It may be a timer id */
  if (IsTimerId(par2)) {
    index = par2-FIRST_TIMER_ID;
    par2  = timers[index].value;
    type2 = NUMBER;
  }
  /* Now both pars are numeric. */

  if (CheckPars(SETCURSOR, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* do nothing, just get this function off the stack */
    return( (resultStruct) {CONTINUE, NONE, 0} );
  } /* if */
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqShuffle(int32_t **trigger)
{
  /* shuffle(loc/obj) */

  /* This function shuffles the parameter's contained */
  /* object list to create randomness in the order in */
  /* which contained objects respond.                 */

  int32_t      owner;   /* dummy */
  char         *str;    /* dummy */
  int32_t      par;
  int32_t      type = NO_TYPE;
  resultStruct result;

  resultStruct par_list;  /* for debugging */

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return( (resultStruct) {ERROR, NONE, 0} );

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = owner;
    par_list.value = par;
    DebugLevel_2_pars("shuffle()", &par_list, 1);
  }

  if (CheckPars(SHUFFLE, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (Shuffle(par))
      return( (resultStruct) {CONTINUE, NONE, 0} );
    else
      result = (resultStruct) {ERROR, NONE, 0};
  }
  else {
    result = (resultStruct) {QUIT, NONE, 0};
  }

  DebugLevel_2_result(result);

  return(result);
}


resultStruct XeqStartTimer(int32_t **trigger)  /* @!@ */
{
  /* Always has one parameter. */

  int32_t      owner;           /* dummy */
  char         *str;            /* dummy */
  int32_t      par  = NO_ID;    /* counter id. */
  int32_t      type = NO_TYPE;
  int32_t      index;           /* To correct a compiler bug. */
  resultStruct result;

  resultStruct par_list;  /* for debugging */

  /* Skip nr of pars. */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = owner;
    par_list.value = par;
    DebugLevel_2_pars("starttimer()", &par_list, 1);
  }

  if (CheckPars(STARTTIMER, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    index = par-FIRST_TIMER_ID;
    /* compiler chokes on timers[par-FIRST_TIMER_ID]. */

    /* write undo info */
    PushUndoItem(TIMERS, STATE, par, NO_ID, NO_ID, timers[index].state);

    timers[index].state = GO;
    result = (resultStruct) {CONTINUE, NONE, 0};
  }
  else {
    result = (resultStruct) {QUIT, NONE, 0};
  }

  DebugLevel_2_result(result);

  return(result);
}


resultStruct XeqStopTimer(int32_t **trigger)  /* @!@ */
{
  /* Always has one parameter. */

  int32_t      owner;          /* dummy */
  char         *str;           /* dummy */
  int32_t      par  = NO_ID;   /* counter id. */
  int32_t      type = NO_TYPE;
  int32_t      index;          /* To correct a compiler bug. */
  resultStruct result;

  resultStruct par_list;  /* for debugging */

  /* Skip nr of pars. */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = owner;
    par_list.value = par;
    DebugLevel_2_pars("stoptimer()", &par_list, 1);
  }

  if (CheckPars(STOPTIMER, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    index = par-FIRST_TIMER_ID;
    /* compiler chokes on timers[par-FIRST_TIMER_ID]. */

    /* write undo info */
    PushUndoItem(TIMERS, STATE, par, NO_ID, NO_ID, timers[index].state);

    timers[index].state = STOP;
    result = (resultStruct) {CONTINUE, NONE, 0};
  }
  else {
    result = (resultStruct) {QUIT, NONE, 0};
  }

  DebugLevel_2_result(result);

  return(result);
}


resultStruct XeqSynchro(int32_t **trigger, usrActionRec *action_rec, int32_t subject_index)
{
  /* Synchronize(loc/obj, trigger, common flag, 0/1 [,level]) */
  int32_t      owner;       /* dummy */
  char         *str;        /* dummy */
  int          i = 0;
  int32_t      nr_of_pars;
  int32_t      par[5];      /* 0 .. 4 */
  int32_t      type[5];     /* 0 .. 4 */
  resultStruct result;

  resultStruct par_list[5];  /* for debugging */

  for (i=0; i<5; i++)
    type[i] = NO_TYPE;

  nr_of_pars = NextOpcode(trigger); /* either 4 or 5 */

  /* Read the parameters. */
  if (!GetPar(&owner, par, type, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, par+1, type+1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, par+2, type+2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, par+3, type+3, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (nr_of_pars == 5) {
    if (!GetPar(&owner, par+4, type+4, &str, trigger))
      return( (resultStruct) {QUIT, NONE, 0} );
  }
  else {
    par[4]  = 1;       /* 0 is default level */
    type[4] = NUMBER;
  }

  if (debug_level == 2) {
    for (i=0; i<5; i++) {
      par_list[i].tag   = type[i];
      par_list[i].owner = NONE;
      par_list[i].value = par[i];
    }
    DebugLevel_2_pars("synchronize()", par_list, 5);
  }

  if (CheckPars(SYNCHRONIZE, type[0], type[1], type[2], type[3], type[4])) {
    /* types are correct, now check whether */
    /* par[3] is either 0 or 1.               */
    if (par[3] != 0 && par[3] != 1) {
      PrintString("synchronize(): parameter 3 must be 0 or 1.\n", 0);
      Output();
      return( (resultStruct) {QUIT, NONE, 0} );
    }
    result.tag   = NUMBER;
    result.value = Synchronize(par[0], par[1], par[2], par[3], par[4], action_rec, subject_index);
    return(result);
  }
  else {
    result = (resultStruct) {QUIT, NONE, 0};
  }

  DebugLevel_2_result(result);

  return(result);
}


resultStruct XeqTestmode(int32_t **trigger)
{
  /* Skip nr of parameters. */
  NextOpcode(trigger);

  if (testmode) {
    /* testmode already active */
    PrintError(90, NULL, NULL);
    return( (resultStruct) {CONTINUE, NONE, 0} );
  }

  /* Open the file with testdata */
  if ((testfile = fopen(TESTFILE, "r")) == NULL) {
    /* not a severe error */
    PrintError(40, NULL, TESTFILE);
    return( (resultStruct) {CONTINUE, NONE, 0} );
  }
  PrintError(91, NULL, NULL);  /* not an error */
  testmode = 1;
  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqText(int32_t **trigger)
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
    return( (resultStruct) {QUIT, NONE, 0} );

  if (CheckPars(TEXT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (par == LookUpId(TranslateKeyword("BLUE"))) {
      /* set text color to blue        */
      /* this is system dependent code */
      /* system("COLOR F1"); */
      return( (resultStruct) {CONTINUE, NONE, 0} );
    }
    else if (par == LookUpId(TranslateKeyword("BLACK"))) {
      /* set text color to black       */
      /* this is system dependent code */
      /* system("COLOR F0"); */
      return( (resultStruct) {CONTINUE, NONE, 0} );
    }
    else {
      PrintError(87, NULL, NULL);
      return( (resultStruct) {CONTINUE, NONE, 0} ); /* not a severe error */
    }
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}


resultStruct XeqTranscript(int32_t **trigger)
{
  /* Skip nr of parameters. */
  NextOpcode(trigger);

  if (transcript) {
    /* transcript mode already activated, turn it off */
    PrintError(93, NULL, NULL);  /* not an error */
    transcript = 0;
    fclose(transcriptfile);
    return( (resultStruct) {CONTINUE, NONE, 0} );
  }
  /* Open the transcript output file */
  /* add to end of file              */
  if ((transcriptfile = fopen(TRANSCRIPTFILE, "a")) == NULL) {
    /* not a severe error */
    PrintError(40, NULL, TRANSCRIPTFILE);
    return( (resultStruct) {CONTINUE, NONE, 0} );
  }

  transcript = 1;

  /* write headers to transcript file */
  WriteTranscript("\n=====================================\n"
                  "XVAN transcript for: ");
  WriteTranscript(story_info.title);
  WriteTranscript("version ");
  WriteTranscript(story_info.version);
  WriteTranscript("=====================================\n");

  PrintError (92, NULL, NULL);  /* not an error */

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqUnderline(int32_t **trigger)
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
    return( (resultStruct) {QUIT, NONE, 0} );

  if (par == LookUpId(TranslateKeyword("ON"))) {
    /* set underline on */
    return( (resultStruct) {CONTINUE, NONE, 0} );
  }
  if (par == LookUpId(TranslateKeyword("OFF"))) {
    /* set underline off */
    return( (resultStruct) {CONTINUE, NONE, 0} );
  }

  PrintError(83, NULL, "XeqUnderline()");
  return( (resultStruct) {CONTINUE, NONE, 0} ); /* not a severe error */
}


resultStruct XeqUndo(int32_t **trigger)  /* @!@ */
{
  /* this function pops the undo stack until it reaches an */
  /* EOU (End Of Undo) or NO_ID value in the item member   */
  /* (EOU reached will be the EOU of the next undo)        */
  /* The last thing that is written for a move's undo is   */
  /* the EOU record                                        */

  /* Syntax: undo([clear])  */

  int32_t nr_of_pars = 0;
  int32_t owner;          /* dummy */
  char    *str;           /* dummy */
  int32_t par;            /* color */
  int32_t type = NO_TYPE;

  int32_t item1 = NO_ID;
  int32_t item2 = NO_ID;
  int32_t item3 = NO_ID;
  int32_t item4 = NO_ID;
  int32_t item5 = NO_ID;
  int32_t value = 0;

  attrInfo *attributes     = NULL;
  int32_t  attribute_index = 0;

  resultStruct par_list;  /* for debugging */

  /* Read nr of parameters. */
  nr_of_pars = NextOpcode(trigger);

  if (nr_of_pars == 1) {
    /* Read parameter. */
    if (!GetPar(&owner, &par, &type, &str, trigger)) {
      return( (resultStruct) {QUIT, NONE, 0} );
    }

    if (debug_level == 2) {
      par_list.tag   = WORD_ID;
      par_list.owner = NONE;
      par_list.value = par;
      DebugLevel_2_pars("undo()", &par_list, 1);
    }

    if (par == LookUpId(TranslateKeyword("CLEAR"))) {
      /* reset the undo stack */
      InitUndoStack();
      write_undo = 0;
      return( (resultStruct) {CONTINUE, NONE, 0} );
    }
    else {
      /* error in parameter */
      PrintError(83, NULL, "XeqUndo()");
      return( (resultStruct) {CONTINUE, NONE, 0} );  /* not a severe error */
    }
  }

  if (debug_level == 2) {
    DebugLevel_2_pars("undo()", NULL, 0);
  }

  PopUndoItem(&item1, &item2, &item3, &item4, &item5, &value);

  /* check if there is data on the undo stack */
  if (item1 == NO_ID) {
    /* it was an empty stack */
    PrintError(110, NULL, NULL);
    return( (resultStruct) {CONTINUE, NONE, 0} );
  }

  /* ok, there is data on the stack. now check if we are  */
  /* at the beginning of an undo sequence                 */

  if (item1 != EOU) {
    /* error, corrupted stack */
    PrintError(111, NULL, NULL);

    /* reset the undo stack */
    InitUndoStack();

    DebugLevel_2_result( (resultStruct) {CONTINUE, NONE, 0});
    return( (resultStruct) {CONTINUE, NONE, 0} );
  }

  /* ok, valid stack with undo data */
  /* we have popped the EOU, now    */
  /* pop the first undo item        */
  PopUndoItem(&item1, &item2, &item3, &item4, &item5, &value);

  while (item1 != EOU && item1 != NO_ID) {
    switch (item1) {
      case MOVE:
        /* move item2 to value */
        if (!Move(item2, value)) {
          return( (resultStruct) {QUIT, NONE, 0} );
        }
        break;
      case CURR_LOC:
        /* set curr_loc to value */
        curr_loc = value;
        break;
      case FLAGS:
        /* item 2: flag id, item3: owner, value: value */
        if (IsCFlagId(item2)) {
          ProcCFlagVal(item3, item2, value);
        }
        else {
          ProcLFlagVal(item2, value);
        }
        break;
      case ATTRIBUTES:
        /* item2: id, item3 owner, item4: value owner, item5: type, value: value */
        /* Retrieve the par1 attribute info struct */
        if (!GetAttributeInfo(item2, item3, &attributes, &attribute_index)) {
          return( (resultStruct) {QUIT, NONE, 0} );
        }
        attributes[attribute_index].value_owner = item4;
        attributes[attribute_index].type        = item5;
        attributes[attribute_index].value       = value;
        break;
      case TIMERS:
        /* item2: action type, item3: id, item4,item5: NO_ID, value: value */
        switch (item2) {
          case STATE:
            timers[item3-FIRST_TIMER_ID].state = value;
            break;
          case INTERVAL:
            timers[item3-FIRST_TIMER_ID].update = value;
            break;
          case INIT:
            timers[item3-FIRST_TIMER_ID].value = value;
            break;
        }
        break;
      default:
        /* unknown undo code */
        PrintError(112, NULL, NULL);
        InitUndoStack();
        return( (resultStruct) {CONTINUE, NONE, 0} );
    }
    PopUndoItem(&item1, &item2, &item3, &item4, &item5, &value);
  }

  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqWait(int32_t **trigger, usrActionRec *action_rec, int32_t subject_index)
{
  int32_t      owner;   /* dummy */
  char         *str;    /* dummy */
  int32_t      par;     /* number of turns to wait. */
  int32_t      type = NO_TYPE;
  int32_t      i;
  resultStruct result;

  resultStruct par_list;  /* for debugging */

  /* Always has one parameter. */

  /* Skip nr of pars. */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (debug_level == 2) {
    par_list.tag   = type;
    par_list.owner = owner;
    par_list.value = par;
    DebugLevel_2_pars("wait()", &par_list, 1);
  }

  if (CheckPars(WAIT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* 15aug2015 - HandleTimers() is executed after each move. as wait() is a move, */
    /* for "wait 1" we don't need to execute HandleTimers() in this function.       */

    if (par>1) {
      /* Execute 1 less HandleTimers() then par indicates because        */
      /* the interpreter will kick off HandleTimers as part of the move. */
      for (i=0; i<par-1; i++)
        if (HandleTimers(action_rec, subject_index) == QUIT)
          return( (resultStruct) {QUIT, NONE, 0} );
    }
    result = (resultStruct) {CONTINUE, NONE, 0};
  }
  else {
    result = (resultStruct) {QUIT, NONE, 0};
  }

  DebugLevel_2_result(result);

  return(result);
}


resultStruct XeqIntAct(int32_t opcode, int32_t **trigger, usrActionRec *action_rec, int32_t subject_index)
{
  switch (opcode) {
    /* Internal actions are listed in alfabetical order. */
    case ADD:
      return(XeqBasicOperator(opcode, trigger));
    case ADDCHOICE:
      return(XeqAddChoice(trigger));
    case ADDJSON:
      return(XeqAddJson(trigger));
    case AGREE:
      return(XeqAgree(trigger));
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
      return(XeqGetSubjectOrSpec(trigger, GET_SUBJECT));
    case GET_SPECIFIER:
      return(XeqGetSubjectOrSpec(trigger, GET_SPECIFIER));
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
    case PICKONE:
      return(XeqPickOne(trigger));
    case PLAYMODE:
      return(XeqPlayMode(trigger));
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
    case UNDO:  /* @!@ */
      return(XeqUndo(trigger));
    case WAIT:
      return(XeqWait(trigger, action_rec, subject_index));
	case SAVE:					/* dec 21 07 */
	  return(XeqSave(trigger));
	case RESTORE:					/* dec 21 07 */
	  return(XeqRestore(trigger));
    default:
      PrintError(94, &((resultStruct) {VALUE, NONE, opcode}), NULL);
      return( (resultStruct) {ERROR, NONE, 0} );
  } /* switch */
}
