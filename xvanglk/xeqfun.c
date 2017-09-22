
/* Copyright 2016, 2017 Marnix van den Bos */

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

#include "glk.h"
#include "keyword.h"
#include "typedefs.h"
#include "xeqfun.h"

/*************************/
/* Function declarations */
/*************************/

int32_t XeqCanSee(int32_t**);
int32_t XeqExecute(int32_t**);          /* Is called `trigger' in story source. */
int32_t XeqExit(int32_t**);
int32_t XeqFunPro(int32_t**);
int32_t XeqIsLit(int32_t**);
int32_t XeqOwns(int32_t**);
int32_t XeqTstFlag(int32_t**);
int32_t XeqTestFun(int32_t, int32_t**);
int32_t XeqValDir(int32_t**);
int32_t XeqYesNo(int**);

int32_t XeqAgree(int32_t**);
int32_t XeqBlockExit(int32_t**);
int32_t XeqBackground(int32_t**);
int32_t XeqClearWindow(int**, winid_t);
int32_t XeqDebug(int32_t**);
int32_t XeqDest(int32_t**);
int32_t XeqDisagree(int32_t**);
int32_t XeqDistance(int32_t**, int32_t);
int32_t XeqGetSubject(int32_t**);
int32_t XeqGetSpec(int32_t**);
int32_t XeqGetAnswer(int32_t**);
int32_t XeqGoTo(int32_t**);
int32_t XeqHitAnyKey(int**);
int32_t XeqCount(int32_t**);
int32_t XeqOwner(int32_t**);
int32_t XeqNoMatch(int32_t**);
int32_t XeqContents(int32_t**);
int32_t XeqEntrance(int32_t**);
int32_t XeqMove(int32_t**);
int32_t XeqNewExit(int32_t**);
int32_t XeqIndent(int32_t**);
int32_t XeqPrt(int32_t**, winid_t, glui32);
int32_t XeqPrtcr(int32_t**, winid_t, glui32);
int32_t XeqQuit(int32_t**);
int32_t XeqScore(int32_t**);
int32_t XeqSetAttribute(int32_t**);
int32_t XeqFlagVal(int32_t**, int32_t);
int32_t XeqShuffle(int32_t**);
int32_t XeqStartTimer(int32_t**);
int32_t XeqStopTimer(int32_t**);
int32_t XeqSynchro(int32_t**);
int32_t XeqText(int32_t**);
int32_t XeqWait(int32_t**);
int32_t XeqSave(int32_t**);			    /* dec 21 07 */
int32_t XeqRestore(int32_t**);			/* dec 21 07 */
int32_t XeqTestmode(int32_t**);
int32_t XeqTranscript(int32_t**);
int32_t XeqIntAct(int32_t, int32_t**);


/****************************/
/* Testfunction definitions */
/****************************/

int32_t XeqCanSee(trigger)
 int32_t **trigger;
{
  int32_t  owner;  /* dummy */
  char     *str;   /* dummy */
  int32_t  par1;
  int32_t  par2;   /* CanSee() always has two parameters. */
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Read parameters. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  if (CheckPars(CANSEE, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE))
    return(CanSee(par1, par2));
  else
    return(QUIT);
}

int32_t XeqExecute(trigger)
 int32_t **trigger;
{
  /* The corresponding XVAN-function is called `trigger',   */
  /* because EXECUTE is not an internal action in keyword.h */

  int32_t  owner;
  char     *str;  /* dummy */
  int32_t  par;
  int32_t  type = NO_TYPE;
  int32_t  result;

  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(TRIGGER, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    result = XeqTrigger(owner, par);

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
        sprintf(outputline, "XeqExecute(): Error: unexpected return value %d\n", result);
        Output(outputline);
        return(QUIT);
        break;
    }
    /* return(result != QUIT? CONTINUE : result); */
  }
  else
    return(QUIT);
}


int32_t XeqExit(trigger)
 int32_t **trigger;
{
  /* exit(loc/obj) */

  int32_t  owner;       /* dummy */
  char *str;            /* dummy */
  int32_t  par;
  int32_t  type  = NO_TYPE;

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(ERROR);

  if (CheckPars(EXIT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (Exit(par) == AGREE)
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
  int32_t  owner; /* dummy */
  char     *str;  /* dummy */
  int32_t  par;
  int32_t  type = NO_TYPE;

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

int32_t XeqOwns(trigger)
 int32_t **trigger;
{
  int32_t  owner; /* dummy */
  char *str;  /* dummy. */
  int32_t  par1;
  int32_t  par2;  /* Owns() has either two or three parameters. */
  int32_t  par3;
  int32_t  nr_of_pars;
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;
  int32_t  type3 = NO_TYPE;

  /* Read number of parameters. */
  nr_of_pars = NextOpcode(trigger);

  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return(QUIT);

  if(!GetPar(&owner, &par2, &type2, &str, trigger))
    return(QUIT);

  if (par1 == NONE || par2 == NONE) {
    sprintf(outputline, "XeqOwns(): NONE-parameter.\n");
    Output(outputline);
    return(QUIT);
  }

  if (nr_of_pars == 3) {
    /* Syntax: NUMBER int */
    if (!GetPar(&owner, &par3, &type3, &str, trigger))
      return(QUIT);
    }
  else {
    par3  = 1;
    type3 = NUMBER;
  }

  /* We now must test if par1 owns (contains) par2. */
  /* The depth of the containment may be max par3.  */
  if (CheckPars(OWNS, type1, type2, type3, NO_TYPE, NO_TYPE))
    return(Owns(par1, par2, par3));
  else
    return(QUIT);
}


int32_t XeqTstFlag(trigger)
 int32_t **trigger;
{
  /* Has one parameters. */
  int32_t  owner;
  char *str; /* dummy */
  int32_t  par;
  int32_t  type = NO_TYPE;

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (owner == NONE) {
    sprintf(outputline, "TstFlag(): NONE-parameter.\n");
    Output(outputline);
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

  int32_t  owner; /* dummy */
  char *str;  /* dummy */
  int32_t  par1;
  int32_t  par2;
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;

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
 int **trigger;
{
  char yes_or_no[INPUT_LINE_LEN];
  int gotline, len;
  event_t ev;

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  while (1) {
    glk_request_line_event(mainwin, yes_or_no, 255, 0);

    gotline = 0;
    while (!gotline) {
      glk_select(&ev);
      switch (ev.type) {
        case evtype_LineInput:
          if (ev.win == mainwin) {
            gotline = 1;
          }
          break;
        case evtype_Arrange:
          /* do nothing */
          break;
      }
    }

    len = ev.val1;
    yes_or_no[len] = '\0';

    strlwr(yes_or_no);

    if ((strcmp(yes_or_no, "yes") == 0) || (strcmp(yes_or_no, "y") == 0))
      return(OK);
    else
      if ((strcmp(yes_or_no, "no") == 0) || (strcmp(yes_or_no, "n") == 0))
        return(ERROR);
      else
        glk_put_string("Please enter \"yes\" or \"no\": ");
  }
}

int32_t XeqTestFun(opcode, trigger)
 int32_t opcode;
 int32_t **trigger;
{
  switch (opcode) {
    /* Testfunctions are listed in alfabetical order. */
    case CANSEE:
      return(XeqCanSee(trigger));
    case EQUAL:
      return(XeqEqual(trigger));
    case LESS_THAN:
      return(XeqLtGt(LESS_THAN, trigger));
    case GREATER_THAN:
      return(XeqLtGt(GREATER_THAN, trigger));
    case EXIT:
      return(XeqExit(trigger));
    case ISLIT:
      return(XeqIsLit(trigger));
    case OWNS:
      return(XeqOwns(trigger));
    case TESTFLAG:
      return(XeqTstFlag(trigger));
    case TRIGGER:
      return(XeqExecute(trigger));
    case VALID_DIRECTION:
      return(XeqValDir(trigger));
    case YES_NO:
      return(XeqYesNo(trigger));
    default:
      sprintf(outputline, "XeqTestFun(): error, undefined testfunction opcode: %d.\n", opcode);
      Output(outputline);
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

  int32_t  owner;   /* dummy */
  char     *str;    /* dummy */
  int32_t  par;     /* color */
  int32_t  type = NO_TYPE;


  /* Skip nr of parameters (which will be 1). */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(BACKGROUND, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (par == LookUpId("blue")) {
      /* set background color to blue   */
      /* this is  system dependent code */
      system("COLOR 17");
      return(CONTINUE);
    }
    if (par == LookUpId("black")) {
      /* set background color to black */
      /* this is system dependent code */
      system("COLOR 07");
      return(CONTINUE);
    }
        sprintf(outputline, "XeqBackground(): ");
        Output(outputline);
        PrintId(par);
        sprintf(outputline, " is not a supported background color (blue and black).");
        Output(outputline);
        sprintf(outputline, "\n");
        Output(outputline);
    return(CONTINUE); /* not a severe error */
  }
  else
    return(QUIT);
}


int32_t XeqBlockExit(trigger)
 int32_t **trigger;
{
  /* blockexit(loc, dir) */

  int32_t  owner; /* dummy */
  char *str;  /* dummy */
  int32_t  par1;
  int32_t  par2;
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;

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


int32_t XeqClearWindow(trigger, window)
 int32_t **trigger; /* Caller expects rest of trigger to be returned. */
 winid_t window;
{
  /* clearstatus() */
  /* clearscreen() */

  /* Clears the window */

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  glk_window_clear(window);

  return(CONTINUE);
}


int32_t XeqCount(trigger)
 int32_t **trigger;
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
  else
    par4  = 1;       /* 0 is default level */
    type4 = NUMBER;

  if (CheckPars(COUNT, type1, type2, type3, type4, NO_TYPE)) {
    /* types are correct, now check whether */
    /* par3 is either 0 or 1.               */
    if (par3 != 0 && par3 != 1) {
      sprintf(outputline, "count(): parameter 3 must be 0 or 1.\n");
      Output(outputline);
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
  Output(outputline);
  debug_mode = 1;

  Debug();

  /* turn off debug mode */
  debug_mode = 0;

  sprintf(outputline, "%s\n", "Done.");
  Output(outputline);

  fclose(debugfile);

  return(CONTINUE);
}

int32_t XeqDest(trigger)
 int32_t **trigger;
{
  /* Syntax: dest(loc/obj, direction) */

  int32_t  owner;   /* dummy */
  char     *str;    /* dummy */
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
    sprintf(outputline, "XeqDest(): NONE-parameter.\n");
    Output(outputline);
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
      sprintf(outputline, "XeqDistance(): illegal result_value.\n");
      Output(outputline);
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
    sprintf(outputline, "%s: NONE-parameter.\n", fun_name);
    Output(outputline);
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
 int **trigger;
{
  int gotchar;
  event_t ev;

  /* Skip the number of parameters value. */
  NextOpcode(trigger);

  glk_request_char_event(mainwin);

  gotchar = 0;
  while (!gotchar) {
    glk_select(&ev);
    switch (ev.type) {
      case evtype_CharInput:
        if (ev.win == mainwin) {
          gotchar = 1;
        }
        break;
      case evtype_Arrange:
        /* do nothing */
        break;
    }
  }
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

int32_t XeqEntrance(trigger)
 int32_t **trigger;
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
    result = Entrance(par);
    return(result != QUIT? CONTINUE : result);
  }
  else
    return(QUIT);
}

int32_t XeqContents(trigger)
 int32_t **trigger;
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
    result = Contents(par);
    return(result != QUIT? CONTINUE : result);
  }
  else
    return(QUIT);
}


int32_t XeqFlagVal(trigger, value)
 int32_t **trigger;
 int32_t value;
{
  int32_t  owner;
  char *str;   /* dummy */
  int32_t  par;
  int32_t  type = NO_TYPE;

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
      PrintString("No route found\n");
      return(CONTINUE);
    }

    /* There is a route, move par1.             */
    /* par3 denotes max nr of moves, stop if 0. */
    while (i >= 0 && par3 != 0 && !stop) {
       /* printf("%d Moving ", i);PrintId(par1);printf(" to ");PrintId(route[i]);printf("\n"); */
      if (!Move(par1, route[i]))
        return(QUIT);
      if (par1 == PLAYER || Owns(par1, PLAYER, -1))
        /* Update current location. */
        curr_loc = route[i];
      if (route[i] == par2)
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
    Output(outputline);
    return(QUIT);
  }

  if (CheckPars(MOVE, type1, type2, type3, NO_TYPE, NO_TYPE)) {
    /* Test for identical parameters. */
    if (par1 == par2) {
      sprintf(outputline, "XeqMove(): something cannot be moved into itself.\n");
      Output(outputline);
      return(DISAGREE);
    }

    if (!(IsLocId(par2) || IsObjId(par2)))
      /* Par2 is a direction; convert it to a location. */
      if ((par2 = exit_data[(curr_loc-FIRST_LOCATION_ID)*nr_of_directions+
                       par2-first_direction_id]) == NO_ID) {
        sprintf(outputline, "XeqMove(): Invalid direction.\n");
        Output(outputline);
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


int32_t XeqOwner(trigger)
 int32_t **trigger;
{
  /* syntax: owner(obj) */

  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
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
  int32_t owner;    /* dummy */
  char    *str;     /* dummy */
  int32_t par;
  int32_t type = NO_TYPE;
  char    spaces_string[OUTPUT_LINE_LEN-1];

  /* Has either 0 or 1 par. If 0 pars, then we must print the   */
  /* indent; if 1 par, then we must change the value of indent. */
  switch (NextOpcode(trigger)) {
    case 0:
      if (indent > OUTPUT_LINE_LEN-1) {
        sprintf(outputline, "\n\nXeqIndent(): indent too large: %d\n", indent);
        Output(outputline);
        return(ERROR);
      }
      else {
        /* build a string for PrintString(), who will check when */
        /* wrap around the line.                                 */
        for (par = 0; par<indent; par++)
          spaces_string[par] = SPACE;
        spaces_string[par] = '\0';
        PrintString(spaces_string);
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
      Output(outputline);
      break;
  } /* switch */
  return(CONTINUE);
}


int32_t XeqPrt(trigger, window, style)
 int32_t **trigger;
 winid_t window;
 glui32  style;
{
  /* Syntax: print(loc/obj, descr) or        */
  /*         print(<addrpart1> ..<addrpartn> */

  int32_t  owner;
  int32_t  par;
  int32_t  type = NO_TYPE;
  char     *str = NULL;

  /* set the window */
  glk_set_window(window);

  /* set the style */
  glk_set_style(style);

  /* Skip the number of parameters, which will be 1.   */
  NextOpcode(trigger);

  /* Read the parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger)) {
    glk_set_window(mainwin);
    glk_set_style(style_Normal);
    return(QUIT);
  }

  if (CheckPars(PRINT, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (type == STRING)
      PrintString(str);

    else if (type == NUMBER)
      PrintNumber(par);

    else if (owner == NONE || (type == NO_TYPE && par == NONE)) {
	  /* print nothing. */
	  glk_set_window(mainwin);
	  glk_set_style(style_Normal);
      return(CONTINUE);
    }

    /* now check for a d_sys parameter. We must not use GetDescr()  */
    /* to retrieve a d_sys, because multiple System Descriptions    */
    /* are allowed and GetDescr() will always return the only one   */
    /* that was stored in descrInfo: the first one.                 */

    else if (par == DSYS)
      /* DSYS check must be done before DescrId checks              */
      PrintId(owner);

    else if (par == THIS)
      /* DSYS check must be done before DescrId checks              */
      PrintId(THIS);

    else if (IsCDescrId(par) || IsLDescrId(par)) {
      if ((str = GetDescr(owner, par)) != NULL)
        PrintString(str);
    }
    /* 22 dec 2003: didn´t print attributes that are not NUMBERS.   */
    /* Following code was added to print locations or objects that  */
    /* are contained in an attributes parameter (attribute pars are */
    /* are returned with type NO_TYPE or type NUMBER).              */
    else if (IsLocId(par) || IsObjId(par) || IsTimerId(par)) {
      PrintId(par);
    }
    else if (IsWordId(par)) {
      PrintWord(par);
    }
    glk_set_window(mainwin);
    glk_set_style(style_Normal);
    return(CONTINUE);
  }
  else {
    glk_set_window(mainwin);
    glk_set_style(style_Normal);
    return(QUIT);
  }
}


int32_t XeqPrtcr(trigger, window, style)
 int32_t **trigger;
 winid_t window;
 glui32  style;
{

  XeqPrt(trigger, window, style);
  sprintf(outputline, "\n");
  Output(outputline);

  return(CONTINUE);
}

int32_t XeqScore(trigger)
 int32_t **trigger;
{
  int32_t owner;   /* dummy */
  char    *str;    /* dummy */
  int32_t par;     /* number of turns to wait. */
  int32_t type = NO_TYPE;
  /* Always has one parameter. */

  /* Skip nr of pars. */
  NextOpcode(trigger);

  /* Read parameter. */
  if (!GetPar(&owner, &par, &type, &str, trigger))
    return(QUIT);

  if (CheckPars(SCORE, type, NO_TYPE, NO_TYPE, NO_TYPE, NO_TYPE))
    /*return(CONTINUE);*/
    return(par);
  else
    return(QUIT);
}

int32_t XeqSetCursor(trigger)
 int32_t **trigger;
{
  /* setcursor(x-pos, y-pos) */

  int32_t owner;
  char    *str;  /* dummy */
  int32_t par1;
  int32_t par2;
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;

  int32_t index; /* For fixing bug(?).                 */

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
    par1  = timers[index].value;
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
    glk_window_move_cursor(statuswin, par1, par2);
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


int32_t XeqSetAttribute(trigger)
  int32_t **trigger;
{
  int32_t  par1;              /* for attribute id                               */
  int32_t  owner1;            /* par1 owner                                     */
  int32_t  type1 = NO_TYPE;   /* par1 type                                      */
  int32_t  par2;              /* for value that must be stored in the attribute */
  int32_t  owner2;            /* par2 owner                                     */
  int32_t  type2=NO_TYPE;     /* par2 type                                      */
  char     *str;              /* dummy                                          */

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

  int32_t  owner;       /* dummy */
  char *str;            /* dummy */
  int32_t  par;
  int32_t  type  = NO_TYPE;

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

int32_t XeqSynchro(trigger)
 int32_t **trigger;
{
  /* Synchronize(loc/obj, trigger, common flag, 0/1 [,level]) */
  int32_t  owner;       /* dummy */
  char     *str;        /* dummy */
  int      i = 0;
  int32_t  nr_of_pars;
  int32_t  par[5];      /* 0 .. 4 */
  int32_t  type[5];     /* 0 .. 4 */

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
  else
    par[4]  = 1;       /* 0 is default level */
    type[4] = NUMBER;

  if (CheckPars(SYNCHRONIZE, type[0], type[1], type[2], type[3], type[4])) {
    /* types are correct, now check whether */
    /* par[3] is either 0 or 1.               */
    if (par[3] != 0 && par[3] != 1) {
      sprintf(outputline, "count(): parameter 3 must be 0 or 1.\n");
      Output(outputline);
      return(QUIT);
    }
    return(Synchronize(par[0], par[1], par[2], par[3], par[4]));
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
    if (par == LookUpId("blue")) {
      /* set text color to blue        */
      /* this is system dependent code */
      system("COLOR F1");
      return(CONTINUE);
    }
    else if (par == LookUpId("black")) {
      /* set text color to black       */
      /* this is system dependent code */
      system("COLOR F0");
      return(CONTINUE);
    }
    else {
      sprintf(outputline, "XeqText(): only blue and black are supported text colors ");
      Output(outputline);
      PrintId(par);
      sprintf(outputline, "\n");
      Output(outputline);
      return(CONTINUE); /* not a severe error */
    }
  }
  else
    return(QUIT);
}


int32_t XeqWait(trigger)
 int32_t **trigger;
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
        if (HandleTimers() == QUIT)
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

  FILE *savefile = NULL;

  /* Skip nr of parameters.      */
  NextOpcode(trigger);

  if ((savefile = fopen(SAVEFILE, "wb")) == NULL) {
    sprintf(outputline, "Error opening %s\n", SAVEFILE);
    Output(outputline);
    return(ERROR);
  }

  if (!SaveStoryInfo(savefile))
    return(ERROR);

  if (!SaveSpecialIds(savefile))
    return(ERROR);

  if (!(SaveLocationDirectory(savefile)))
    return(ERROR);

  if (!(SaveObjectDirectory(savefile)))
    return(ERROR);

  if (!(SaveExits(savefile)))
    return(ERROR);

  if (!(SaveCommonAttributes(savefile)))
    return(ERROR);

  if (!(SaveLocalAttributes(savefile)))
    return(ERROR);

  if (!(SaveCommonFlags(savefile)))
    return(ERROR);

  if (!(SaveLocalFlags(savefile)))
    return(ERROR);

  if (!(SaveAllTimers(savefile)))
    return(ERROR);

  fclose(savefile);
  return(CONTINUE);
}

int32_t XeqRestore(trigger)
 int32_t **trigger;
{
  /* Syntax: restore() */
  /* Always has zero parameters. */

  FILE      *savefile = NULL;
  storyInfo saved_story_info;

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* open in mode "rb" for reading a binary file */
  if ((savefile = fopen(SAVEFILE, "rb")) == NULL) {
    sprintf(outputline, "Error opening %s\n", SAVEFILE);
    Output(outputline);
    return(ERROR);
  }

  /***************************************************************/
  /* It is important that the restore functions below are called */
  /* in the same sequence as the corresponding save functions.   */
  /* There are no syntax checks done.                            */
  /***************************************************************/

  /* Read the story info from the savefile. */
  /* Do not overwrite the current story's storyInfo struct as we */
  /* need it to check the compatibility of the savefile with the */
  /* current story.                                              */
  if (!RestoreStoryInfo(&saved_story_info, savefile))
    return(ERROR);
  else {
    /* Check savefile compatibility with current story */
    /* Title and version must match.                   */
    if (strcmp(story_info.title, saved_story_info.title) != 0) {
      sprintf(outputline, "Error, savefile is for story %s\n", saved_story_info.title);
      Output(outputline);
      return(ERROR);
    }

    if (strcmp(story_info.version, saved_story_info.version) != 0) {
      sprintf(outputline, "Error, savefile is for %s version %s.\n", saved_story_info.title,
              saved_story_info.version);
      Output(outputline);
      return(ERROR);
    }
  } /* else */

  if (!RestoreSpecialIds(savefile))
    return(ERROR);

  if (!(RestoreLocationDirectory(savefile)))
    return(ERROR);

  if (!(RestoreObjectDirectory(savefile)))
    return(ERROR);

  if (!(RestoreExits(savefile)))
    return(ERROR);

  if (!(RestoreCommonAttributes(savefile)))
    return(ERROR);

  if (!(RestoreLocalAttributes(savefile)))
    return(ERROR);

  if (!(RestoreCommonFlags(savefile)))
    return(ERROR);

  if (!(RestoreLocalFlags(savefile)))
    return(ERROR);

  if (!(RestoreTimers(savefile)))
    return(ERROR);

  fclose(savefile);
  return(CONTINUE);
}

int32_t XeqTestmode(trigger)
 int32_t **trigger;
{
  /* Skip nr of parameters. */
  NextOpcode(trigger);

  if (testmode) {
    /* testmode already active */
    sprintf(outputline, "Testmode is already active.\n");
    Output(outputline);
    return(CONTINUE);
  }

  /* Open the file with testdata */
  if ((testfile = fopen(TESTFILE, "r")) == NULL) {
    /* not a severe error */
    sprintf(outputline, "Error opening %s.\n", TESTFILE);
    Output(outputline);
    return(CONTINUE);
  }
  sprintf(outputline, "Switching to testmode...\n");
  Output(outputline);
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
    sprintf(outputline, "%s\n", "Turning off transcript mode.");
    Output(outputline);
    transcript = 0;
    fclose(transcriptfile);
    return(CONTINUE);
  }
  /* Open the transcript output file */
  if ((transcriptfile = fopen(TRANSCRIPTFILE, "w")) == NULL) {
    /* not a severe error */
    printf("Error opening %s.\n", TRANSCRIPTFILE);
    return(CONTINUE);
  }
  /* write headers to transcript file */
  fprintf(transcriptfile, "%s", "XVAN transcript for: ");
  fprintf(transcriptfile, "%s\n", story_info.title);
  fprintf(transcriptfile, "%s", "version: ");
  fprintf(transcriptfile, "%s\n", story_info.version);

  sprintf(outputline, "Transcript mode started...\n");
  Output(outputline);
  transcript = 1;
  return(CONTINUE);
}

int32_t XeqIntAct(opcode, trigger)
 int32_t opcode;
 int32_t **trigger; /* Caller expects remainder of *trigger to be */
                    /* returned.                                  */
{
  switch (opcode) {
    /* Internal actions are listed in alfabetical order. */
    case AGREE:
      return(XeqAgree(trigger));
    case ADD:
      return(XeqBasicOperator(opcode, trigger));
    case ASTERIX:
      return(XeqBasicOperator(opcode, trigger));
    case BACKGROUND:
      return(XeqBackground(trigger));
    case BLOCK_EXIT:
      return(XeqBlockExit(trigger));
    case CLEARFLAG:
      return(XeqFlagVal(trigger, 0));
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
      return(XeqContents(trigger));
    case ENTRANCE:
      return(XeqEntrance(trigger));
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
    case MOVE:
      return(XeqMove(trigger));
    case NEW_EXIT:
      return(XeqNewExit(trigger));
    case OWNER:
      return(XeqOwner(trigger));
    case PRINT:
      return(XeqPrt(trigger, mainwin, style_Normal));
    case PRINTBOLD:
      return(XeqPrt(trigger, mainwin, style_Alert));
    case PRINTITALIC:
      return(XeqPrt(trigger, mainwin, style_Note));
    case PRINTCR:
      return(XeqPrtcr(trigger, mainwin, style_Normal));
    case PRINTCRBOLD:
      return(XeqPrtcr(trigger, mainwin, style_Alert));
    case PRINTCRITALIC:
      return(XeqPrtcr(trigger, mainwin, style_Note));
    case PRINTSTATUS:
      return(XeqPrt(trigger, statuswin, style_Normal));
    case PRINTCRSTATUS:
      return(XeqPrtcr(trigger, statuswin, style_Normal));
    case SETCURSOR:
      return(XeqSetCursor(trigger));
    case CLEARSTATUS:
      return(XeqClearWindow(trigger, statuswin));
    case CLEARSCREEN:
      return(XeqClearWindow(trigger, mainwin));
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
      return(XeqSynchro(trigger));
    case TESTMODE:
      return(XeqTestmode(trigger));
    case TEXT:
      return(XeqText(trigger));
    case TRANSCRIPT:
      return(XeqTranscript(trigger));
    case WAIT:
      return(XeqWait(trigger));
	case SAVE:						/* dec 21 07 */
	  return(XeqSave(trigger));
	case RESTORE:					/* dec 21 07 */
	  return(XeqRestore(trigger));
    default:
      sprintf(outputline, "XeqIntAct(): error, undefined internal action opcode: %d.\n", opcode);
      Output(outputline);
      return(ERROR);
  } /* switch */
}
