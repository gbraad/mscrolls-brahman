
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


/**************************************************************/
/* This module contains the code for the arithmetic functions */
/* that the user may use in his source file.                  */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "arithmtc.h"

/*************************/
/* Function declarations */
/*************************/

int32_t      XeqEqual(int32_t**);
int32_t      XeqLtGt(int32_t, int32_t**);
resultStruct XeqBasicOperator(int32_t, int32_t**);
resultStruct XeqRnd(int32_t**);
resultStruct XeqSetTimer(int32_t**);

/****************************/
/* Testfunction definitions */
/****************************/

int32_t XeqEqual(int32_t **trigger)
{
  /* This function compares two numerical values. Timer ids are  */
  /* converted to the timer value. In case of an attribute, the  */
  /* attribute's type and value will be retrieved.               */
  /* If par1 and par2 are of different types, the function will  */
  /* return ERROR.                                               */

  int32_t owner;
  char    *str;  /* dummy */
  int32_t par1;
  int32_t par2;  /* Equal() always has two parameters. */
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;
  int32_t  result;

  resultStruct par_list[2];  /* for debugging */

  int32_t index; /* For fixing bug(?).                 */

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Read first parameter. */
  /* For attribute parameters, GetPar() returns the */
  /* attributes type and value.                     */
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

  /* NONE is a special case for the Equal() function and wildcards. For example,   */
  /* if %dir is NONE, it will still have type DIRECTIONS. If %PREPOS is NONE, it   */
  /* will still have type PREPOSITIONS. So comparing %DIR and %PREPOS when both    */
  /* are NONE will fail on type values. But %none has value NONE and type NO_TYPE. */
  /* However, comparing %DIR or other wildcards with %none must succeed when the   */
  /* wildcard has value NONE. Therefore, when both values are NONE and either one  */
  /* of the types is NO_TYPE, the comparison must succeed.                         */

  if (debug_level == 2) {
     par_list[0].tag   = type1;
     par_list[0].owner = NO_ID;  /* only for common flags */
     par_list[0].value = par1;
     par_list[1].tag   = type2;
     par_list[1].owner = NO_ID;  /* only for common flags */
     par_list[1].value = par2;
     DebugLevel_2_pars("equal()", par_list, 2);
   }

   /* Not necessary to call CheckPars() here. All pars are */
   /* ok as long as they are of the same type.             */
   if ( ((type1 == NO_TYPE || type2 == NO_TYPE) || (type1 == type2)) && (par1 == par2) ) {
     result = OK;
   }
   else {
     result = ERROR; /* par1 and par2 are not equal */
   }

   DebugLevel_2_result( (resultStruct) {result, NONE, 0});

   return(result);
}


int32_t XeqLtGt(int32_t operation, int32_t **trigger)
{
  /* This function compares two numerical values. Timer ids are  */
  /* converted to the timer value.                               */

  int32_t owner;
  char    *str;  /* dummy */
  int32_t par1;
  int32_t par2;  /* Equal() always has two parameters. */
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;

  int32_t index; /* For fixing bug(?).                 */

  /* Skip nr of parameters. */
  NextOpcode(trigger);

  /* Read first parameter. */
  /* For attribute parameters, GetPar() returns the */
  /* attribute's type and value.                    */
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

  if (CheckPars(operation, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    /* check what we must do: less than or greater than */
    switch (operation) {
      case LESS_THAN:
        return(par1<par2 ? OK : ERROR);
        break;
      case GREATER_THAN:
        return(par1>par2 ? OK : ERROR);
        break;
      default:
        PrintError(0, &((resultStruct) {VALUE, operation}), NULL);
        return(ERROR);
    } /* switch */
  } /* if */
  else
    return(QUIT);
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

resultStruct XeqBasicOperator(int32_t opr, int32_t **trigger)
 /* opr is +, -, *, div or rem */
{
  int32_t  par1;              /* attribute id          */
  int32_t  owner1;            /* par1 owner            */
  int32_t  type1 = NO_TYPE;   /* par1 type             */
  int32_t  par2;              /* value or attribute id */
  int32_t  owner2;            /* par2 owner            */
  int32_t  type2 = NO_TYPE;   /* par2 type             */
  int32_t  par3;              /* value or attribute id */
  int32_t  owner3;            /* par3 owner            */
  int32_t  type3 = NO_TYPE;   /* par3 type             */
  char     *str;              /* dummy                 */

  div_t     div_result;       /* result of integer division */
  int32_t   result = 0;

  attrInfo *attributes     = NULL;  /* No malloc(), we only need the pointer */
  int32_t  attribute_index = 0;

  /* Syntax: operator(attribute/timer, attribute/timer/value, attribute/timer/value) */

  /* Skip number of parameters (which will always be 3).      */
  NextOpcode(trigger);

  /* In case par2 = not NONE, they used a par1 = par2 + par3 construction.  */
  /* In that case, if par1 is an attribute, it's type is not important,     */
  /* because it will be overwritten anyways.                                */

  /* In case par2 = NONE, this means they used a par1 += par3 construction. */
  /* In that case, if par1 is an attribute, it MUST have type NUMBER.       */

  /* Read first parameter. We must use GetLvaluePar() here */
  /* because getPar() will return the attribute or timer's */
  /*  value and not the attribute or timer itself.         */

  if (!GetLvaluePar(&owner1, &par1, &type1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  /* par1 is an attribute id or a timer id, checked by the    */
  /* compiler. If it is an attribute id, We must get the type */
  /* of par1's content. For the CheckPars() function.         */
  if (type1 == COMMON_ATTRS || type1 == ATTRIBUTES) {
    if (!GetAttributeInfo(par1, owner1, &attributes, &attribute_index))
      return( (resultStruct) {QUIT, NONE, 0} );
  }

  /* Read second parameter, which will resolve to a */
  /* value in case it is an attribute.              */
  if (!GetPar(&owner2, &par2, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (type2 == TIM_ID) {
    /* resolve timer to a value */
    par2  = timers[par2-FIRST_TIMER_ID].value;
    type2 = NUMBER;
  }

  if (par2 == NONE) {
    /* par2 must get the same type and value as par1 */
    if (type1 == COMMON_ATTRS || type1 == ATTRIBUTES) {
      if (!GetAttributeInfo(par1, owner1, &attributes, &attribute_index))
        return( (resultStruct) {QUIT, NONE, 0} );

      par2   = attributes[attribute_index].value;
      type2  = attributes[attribute_index].type;
      owner2 = owner1;
    }
    else {
      if (type1 == TIM_ID) {
        /* par1 is a timer id */
        par2  = timers[par1-FIRST_TIMER_ID].value;
        type2 = NUMBER;
        owner2 = owner1;
      }
      else {
        /* serious trouble */
        PrintError(1, NULL, NULL);
        return( (resultStruct) {ERROR, NONE, 0} );
      }
    }
  }

  /* Read third parameter, which will resolve to a */
  /* value in case it is an attribute.             */
  if (!GetPar(&owner3, &par3, &type3, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (type3 == TIM_ID) {
    /* resolve timer to a value */
    par3  = timers[par3-FIRST_TIMER_ID].value;
    type3 = NUMBER;
  }

  /**************************/
  /* Operator specific part */
  /**************************/

  switch (opr) {
    case ADD:
      if (!CheckPars(ADD, type1, type2, type3, NO_TYPE, NO_TYPE))
        return( (resultStruct) {QUIT, NONE, 0} );
      result = par2 + par3;
      break;
    case SUB:
      if (!CheckPars(SUB, type1, type2, type3, NO_TYPE, NO_TYPE))
        return( (resultStruct) {QUIT, NONE, 0} );
      result = par2 - par3;
      break;
    case ASTERIX:
      if (!CheckPars(ASTERIX, type1, type2, type3, NO_TYPE, NO_TYPE))
        return( (resultStruct) {QUIT, NONE, 0} );
      result = par2 * par3;
      break;
    case QUOT:
      if (!CheckPars(QUOT, type1, type2, type3, NO_TYPE, NO_TYPE))
        return( (resultStruct) {QUIT, NONE, 0} );
      if (par3 == 0) {
        PrintError(2, NULL, NULL);
        return( (resultStruct) {QUIT, NONE, 0} );
      }
      div_result = div(par2, par3);
      result = div_result.quot;
      break;
    case REM:
      if (!CheckPars(REM, type1, type2, type3, NO_TYPE, NO_TYPE))
        return( (resultStruct) {QUIT, NONE, 0} );
      if (par3 == 0) {
        PrintError(2, NULL, NULL);
        return( (resultStruct) {QUIT, NONE, 0} );
      }
      div_result = div(par2, par3);
      result = div_result.rem;
      break;
    default:
      PrintError(3, &((resultStruct) {VALUE, NONE, opr}), NULL);
      return( (resultStruct) {ERROR, NONE, 0} );
  } /* switch */

  if (type1 == COMMON_ATTRS || type1 == ATTRIBUTES) {
    /* write undo info */
    PushUndoItem(ATTRIBUTES,
                 par1,
                 owner1,
                 attributes[attribute_index].value_owner,
                 attributes[attribute_index].type,
                 attributes[attribute_index].value);  /* @!@ */
    attributes[attribute_index].type  = NUMBER;
    attributes[attribute_index].value = result;
  }
  else {
    if (type1 == TIM_ID) { /* @!@ */
      /* par 1 is a timer */
      /* write undo info */  /* @!@ */
      PushUndoItem(TIMERS, INIT, par1-FIRST_TIMER_ID, NO_ID, NO_ID, timers[par1-FIRST_TIMER_ID].value);
      timers[par1-FIRST_TIMER_ID].value = result;
    }
    else {
      /* serious trouble */
      PrintError(1, NULL, NULL);
      return( (resultStruct) {ERROR, NONE, 0} );
    }
  }
  return( (resultStruct) {CONTINUE, NONE, 0} );
}


resultStruct XeqRnd(int32_t **trigger)
{
  int32_t  owner;
  char    *str;  /* dummy */
  int32_t par1;
  int32_t par2;  /* Equal() always has two parameters. */
  int32_t type1 = NO_TYPE;
  int32_t type2 = NO_TYPE;
  int32_t index;
  int32_t result;

  /* This function generates a random number that is */
  /* between par1 and par2. Lower and upper are      */
  /* possible results as well.                       */

  /* Skip nr of parameters which will always be 2.   */
  NextOpcode(trigger);

  /* Read first parameter. */
  /* For attribute parameters, GetPar() returns the  */
  /* attribute's type and value.                     */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );
    /*return(QUIT);*/  /* march 23 2016: won't work, QUIT is also a number */
                       /* jan 21 2019: fixed with resultStruct type        */
  /* It may be a timer id */
  if (IsTimerId(par1)) {
    index = par1-FIRST_TIMER_ID;
    par1  = timers[index].value;
    type1 = NUMBER;
  }

  /* Read second parameter. */
  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );
    /*return(QUIT);*/  /* march 23 2016: won't work, QUIT is also a number */
                       /* jan 21 2019: fixed with resultStruct type        */

  /* It may be a timer id */
  if (IsTimerId(par2)) {
    index = par2-FIRST_TIMER_ID;
    par2  = timers[index].value;
    type2 = NUMBER;
  }

  /* Now both pars are numeric. */

  if (CheckPars(RAND, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    if (par2 < par1) {
      /* swap par1 and par2 as par2 must be greater than par 1 */
      par2 += par1;
      par1 =  par2-par1;
      par2 -= par1;
    }
    /* calculate the random number                    */
    /* value between par1 and par2, borders inclusive */
    result = rand();
    result = (par1-1) + result%(par2-par1+1)+1;
    return( (resultStruct) {NUMBER, NONE, result} );
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
    /*return(QUIT);*/  /* march 23 2016: won't work, QUIT is also a number */
                       /* jan 21 2019: fixed with resultStruct type        */
}


resultStruct XeqSetTimer(int32_t **trigger)  /* @!@ */
{
  /* Always has two parameters. */
  /* settimer(ctr_id, value/ctr_id). */

  int32_t  owner;  /* dummy */
  char     *str;   /* dummy */
  int32_t  par1;
  int32_t  par2;
  int32_t  type1 = NO_TYPE;
  int32_t  type2 = NO_TYPE;
  int32_t  index;

  /* Skip nr of pars. */
  NextOpcode(trigger);

  /* Get Parameter 1. */
  if (!GetPar(&owner, &par1, &type1, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  if (!GetPar(&owner, &par2, &type2, &str, trigger))
    return( (resultStruct) {QUIT, NONE, 0} );

  /* check if par2 is a timer id */
  if (IsTimerId(par2)) {
    /* compiler chokes on timers[par-FIRST_TIMER_ID]. */
    index = par2-FIRST_TIMER_ID;
    par2  = timers[index].value;
    type2 = NUMBER;
  }

  if (CheckPars(SETTIMER, type1, type2, NO_TYPE, NO_TYPE, NO_TYPE)) {
    index = par1-FIRST_TIMER_ID;
    /* write undo info */
    PushUndoItem(TIMERS, INIT, index+FIRST_TIMER_ID, NO_ID, NO_ID, timers[index].value);

    timers[index].value = par2;

    return( (resultStruct) {CONTINUE, NONE,0} );
  }
  else
    return( (resultStruct) {QUIT, NONE, 0} );
}
