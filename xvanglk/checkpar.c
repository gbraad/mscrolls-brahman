
/* Copyright 2016, 2017 Marnix van den Bos */

/******************************************************************/
/* This module contains code for checking the user functions      */
/* This check is also done at compile time, but because attribute */
/* parameter types are only know at runtime, we need to redo some */
/* checks.                                                        */
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "checkpar.h"

/*************************/
/* Function declarations */
/*************************/

int32_t CheckPars(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);


/*************************/
/* Function definitions  */
/*************************/

int32_t CheckPars(fun_code, type1, type2, type3, type4, type5)
 int32_t fun_code;
 int32_t type1;
 int32_t type2;
 int32_t type3;
 int32_t type4;
 int32_t type5;

{
  switch (fun_code) {
    /**************************************************/
    /* Testfunctions are listed in alfabetical order. */
    /**************************************************/

    case CANSEE:
      /****************************/
      /* cansee(loc/obj, loc/obj) */
      /****************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "cansee() (%d): parameter 1 must be of type location or object.\n", type1);
        Output(outputline);
        return(ERROR);
      }
      if (type2 != LOC_ID && type2 != OBJ_ID) {
        sprintf(outputline, "cansee() (%d): parameter 2 must be of type location or object.\n", type2);
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case EQUAL:
      /* You may can compare any 2 parameters.   */
      /* To prevent false positives, we return   */
      /* an error if the types are not the same. */
      if (type1 == type2)
        return(OK);
      else
        return(ERROR);
      break;

    case LESS_THAN:
      /****************/
      /* lt(num, num) */
      /****************/

      if (type1 != NUMBER) {
        sprintf(outputline, "Lt() (%d): parameter 1 must be of type number.\n", type1);
        Output(outputline);
        return(ERROR);
      }
      if (type2 != NUMBER) {
        sprintf(outputline, "Lt() (%d): parameter 2 must be of type number.\n", type2);
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case GREATER_THAN:
      /****************/
      /* gt(num, num) */
      /****************/

      if (type1 != NUMBER) {
        sprintf(outputline, "Gt(): parameter 1 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != NUMBER) {
        sprintf(outputline, "Gt(): parameter 2 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case SETCURSOR:
      /***********************/
      /* setcursor(num, num) */
      /***********************/

      if (type1 != NUMBER) {
        sprintf(outputline, "SetCursor(): parameter 1 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != NUMBER) {
        sprintf(outputline, "SetCursor(): parameter 2 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case EXIT:
      /*****************/
      /* exit(loc/obj) */
      /*****************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "exit(): parameter 1 must be of type location or object\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case ISLIT:
      /******************/
      /* IsLit(loc/obj) */
      /******************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "islit(): parameter 1 must be of type location or object\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case COUNT:
      /******************************************************/
      /*    Count(loc/obj, common flag, 0/1 [,level]) */
      /******************************************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "count(): parameter 1 must be of type location or object.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != COMMON_FLAGS) {
        sprintf(outputline, "count(): parameter must be of type flag.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type3 != NUMBER) {
        sprintf(outputline, "count(): parameter 3 must be of type number (%d).\n", type3);
        Output(outputline);
        return(ERROR);
      }
      if (type4 != NUMBER) {
        sprintf(outputline, "count(): parameter 4 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case SYNCHRONIZE:
      /*********************************************************************/
      /*    Synchronize(loc/obj, trigger, common flag, 0/1 [,level]) */
      /*********************************************************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "synchronize(): parameter 1 must be of type location or object.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != COMMON_TRIGGERS) {
        sprintf(outputline, "sunchronize(): parameter 2 must be of type common trigger.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type3 != COMMON_FLAGS) {
        sprintf(outputline, "synchronize(): parameter 3 must be of type flag.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type4 != NUMBER) {
        sprintf(outputline, "count(): parameter 4 must be of type number (%d).\n", type3);
        Output(outputline);
        return(ERROR);
      }
      if (type5 != NUMBER) {
        sprintf(outputline, "count(): parameter 5 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case OWNS:
      /***************************/
      /* owns(loc/obj, obj, num) */
      /***************************/
      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "owns(): parameter 1 must be of type location or object (%d).\n", type1);
        Output(outputline);
        return(ERROR);
            }
      if (type2 != LOC_ID && type2 != OBJ_ID)  {
        /* we must also allow type location for par2  */
        /* because they may use the 'this' parameter. */
        sprintf(outputline, "owns(): parameter 2 must be of type location or object (%d).\n", type2);
        Output(outputline);
        return(ERROR);
      }
      if (type3 != NUMBER) {
        sprintf(outputline, "owns(): parameter 3 must be of type number (%d).\n", type3);
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case TESTFLAG:
      /*****************/
      /* tstflag(flag) */
      /*****************/

      if (type1 != COMMON_FLAGS && type1 != FLAG_ID) {
        sprintf(outputline, "tstflg(): parameter must be of type flag.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case VALID_DIRECTION:
    /***************************/
    /* valdir(loc, direction). */
    /***************************/

    if (type1 != LOC_ID) {
      sprintf(outputline, "valdir(): parameter 1 must be of type location.\n");
      Output(outputline);
      return(ERROR);
    }
    if (type2 != DIRECTIONS) {
      sprintf(outputline, "valdir(): parameter 2 must be of type direction.\n");
      Output(outputline);
      return(ERROR);
    }
    return(OK);
    break;

    /******************************************************/
    /* Internal actions are listed in alphabetical order. */
    /******************************************************/

    case ADD:
      /****************************************************/
      /* add(attribute, attribute/value, attribute/value) */
      /****************************************************/

      if (type1 != COMMON_ATTRS && type1 != ATTRIBUTES &&
          type1 != TIM_ID) {
        sprintf(outputline, "add(): parameter 1 must be of type attribute or timer.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != NUMBER) {
        sprintf(outputline, "add(): parameter 2 must be of type NUMBER.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type3 != NUMBER) {
        sprintf(outputline, "add(): type 3 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case ASTERIX:
      /****************************************************/
      /* mul(attribute, attribute/value, attribute/value) */
      /****************************************************/

      if (type1 != COMMON_ATTRS && type1 != ATTRIBUTES &&
          type1 != TIM_ID) {
        sprintf(outputline, "mul(): parameter 1 must be of type attribute or timer.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != NUMBER) {
        sprintf(outputline, "mul(): parameter 2 must be of type NUMBER.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type3 != NUMBER) {
        sprintf(outputline, "mul(): type 3 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case BACKGROUND:
     /********************/
     /* background(word) */
     /********************/

      if (type1 != WORD_ID) {
        sprintf(outputline, "background(): parameter must be a word.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case BLOCK_EXIT:
      /***********************/
      /* blockexit(loc, dir) */
      /***********************/

      if (type1 != LOC_ID) {
        sprintf(outputline, "blockexit(): parameter 1 must be of type location.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != DIRECTIONS) {
        sprintf(outputline, "blockexit(): parameter 2 must be of type direction.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case BOLD:
      /**************/
      /* bold(word) */
      /**************/

      if (type1 != WORD_ID) {
        sprintf(outputline, "bold(): parameter must be a word.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case CLEARFLAG:
      /*******************/
      /* clearflag(flag) */
      /*******************/

      if (type1 != COMMON_FLAGS && type1 != FLAG_ID) {
        sprintf(outputline, "clearflg(): parameter must be of type flag.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case CONTENTS:
      /*********************/
      /* Contents(loc/obj) */
      /*********************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "contents(): parameter 1 must be of type location or object\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case DEST:
      /****************************/
      /* dest(loc/obj, direction) */
      /****************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "dest(): parameter 1 must be of type location or object.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != DIRECTIONS) {
        sprintf(outputline, "dest(): parameter 2 must be of type direction.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case DISTANCE:
      /******************************/
      /* distance(loc/obj, loc/obj) */
      /******************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "distance(): parameter 1 must be of type location or object.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != LOC_ID && type2 != OBJ_ID) {
        sprintf(outputline, "distance(): parameter 2 must be of type location or object.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case ENTRANCE:
      /*********************/
      /* Entrance(loc/obj) */
      /*********************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "entrance(): parameter 1 must be of type location or object.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case FIRSTDIR:
      /******************************/
      /* firstdir(loc/obj, loc/obj) */
      /******************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "firstdir(): parameter 1 must be of type location or object.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != LOC_ID && type2 != OBJ_ID) {
        sprintf(outputline, "firstdir(): parameter 2 must be of type location or object.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case GO_TO:
      /***********************/
      /* goto(obj, loc, num) */
      /***********************/

      if (type1 != OBJ_ID) {
        sprintf(outputline, "goto(): parameter 1 must be of type object.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != LOC_ID) {
        sprintf(outputline, "goto(): parameter 2 must be of type location.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type3 != NUMBER) {
        sprintf(outputline, "goto(): parameter 3 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case INDENT:
      /*****************/
      /* indent([num]) */
      /*****************/

      if (type1 != NUMBER) {
        sprintf(outputline, "indent(): parameter must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

   case ITALIC:
      /****************/
      /* italic(word) */
      /****************/

      if (type1 != WORD_ID) {
        sprintf(outputline, "italic(): parameter must be a word.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case MOVE:
      /************************************************/
      /* move(obj, loc/obj/direction [, preposition]) */
      /************************************************/

      if (type1 != OBJ_ID) {
        sprintf(outputline, "move(): parameter 1 must be of type object.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != LOC_ID && type2 != OBJ_ID && type2 != DIRECTIONS) {
        sprintf(outputline, "move(): parameter 2 must be of type location, object or direction.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type3 != WORD_ID) {
        sprintf(outputline, "move(): parameter 3 must be of type word.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case NEW_EXIT:
      /**************************/
      /* newexit(loc, dir, loc) */
      /**************************/

      if (type1 != LOC_ID) {
        sprintf(outputline, "newexit(): parameter 1 must be of type LOC_ID.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != DIRECTIONS) {
        sprintf(outputline, "newexit(): parameter 2 must be of type direction.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type3 != LOC_ID) {
        sprintf(outputline, "newexit(): parameter 3 must be of type location.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case OWNER:
      /**************/
      /* owner(obj) */
      /**************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        /* we must allow locations here, in case */
        /* they use the THIS parameter.          */
        sprintf(outputline, "owner() (%d): parameter must be location or object.\n", type1);
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case PRINT:
      /**************************************************************/
      /* print(string/number/description/location/object/direction/ */
      /* preposition/word)                                          */
      /**************************************************************/

      switch (type1) {
        case STRING:
        case NUMBER:
        case TIM_ID:
        case DESCR_ID:
        case COMMON_DESCRS:
        case LOC_ID:
        case OBJ_ID:
        case WORD_ID:
        case VERB:
        case DIRECTIONS:
        case PREPOSITIONS:
        case NO_TYPE:  /* for NONE parameter */
          return(OK);
          break;
        default:
          sprintf(outputline, "prt(): (%d) parameter must be of type string, number, timer,", type1);
          Output(outputline);
          sprintf(outputline, " description, location, object, direction, preposition or word.\n");
          Output(outputline);
          return(ERROR);
      }
      break;

    case QUOT:
      /***********************************************************************/
      /* quot(attribute/timer, attribute/timer/value, attribute/timer/value) */
      /***********************************************************************/

      if (type1 != COMMON_ATTRS && type1 != ATTRIBUTES &&
          type1 != TIM_ID) {
        sprintf(outputline, "div(): parameter 1 must be of type attribute or timer.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != NUMBER) {
        sprintf(outputline, "div(): parameter 2 must be of type NUMBER.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type3 != NUMBER) {
        sprintf(outputline, "div(): type 3 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case RAND:
      /*********************/
      /* rnd(value, value) */
      /*********************/

      if (type1 != NUMBER && type1 != TIM_ID) {
        sprintf(outputline, "rnd(): parameter 1 must be of type number or timer.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != NUMBER && type2 != TIM_ID) {
        sprintf(outputline, "rnd(): parameter 2 must be of type number or timer.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case REM:
      /****************************************************/
      /* rem(attribute, attribute/value, attribute/value) */
      /****************************************************/

      if (type1 != COMMON_ATTRS && type1 != ATTRIBUTES &&
          type1 != TIM_ID) {
        sprintf(outputline, "rem(): parameter 1 must be of type attribute or timer.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != NUMBER) {
        sprintf(outputline, "rem(): parameter 2 must be of type NUMBER.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type3 != NUMBER) {
        sprintf(outputline, "rem(): type 3 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case SCORE:
      /****************/
      /* score(value) */
      /****************/

      if (type1 != NUMBER) {
        sprintf(outputline, "score(): parameter must be a number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case SETTIMER:
      /****************************/
      /* settimer(timer, value) */
      /****************************/
      if (type1 != TIM_ID) {
        sprintf(outputline, "SetTimer(): parameter 1 must be of type timer.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != NUMBER) {
        sprintf(outputline, "SetTimer(): parameter 2 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case SETFLAG:
      /*****************/
      /* setflag(flag) */
      /*****************/

      if (type1 != COMMON_FLAGS && type1 != FLAG_ID) {
        sprintf(outputline, "setflag(): parameter must be of type flag.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case SETATTRIBUTE:
      /*****************************************************************************/
      /* setattribute([attribute, loc/obj/attribute/direction/value/word) */
      /*****************************************************************************/
      if (type1 != COMMON_ATTRS && type1 != ATTRIBUTES) {
        sprintf(outputline, "setattribute() (%d): parameter 1 must be of type location or object.\n", type1);
        Output(outputline);
        return(ERROR);
      }
      switch (type2) {
        case NO_TYPE:  /* %none has type NOTYPE */
        case LOC_ID:
        case OBJ_ID:
        case COMMON_DESCRS: /* common description */
        case DESCR_ID:      /* local description */
        case ATTR_ID:
        case TIM_ID:
        case NUMBER:
        case WORD_ID:
        case DIRECTIONS:
        case PREPOSITIONS:
        case VERB:
          return(OK);
          break;
        default:
          sprintf(outputline, "setattribute() (%d): parameter 2 must be of type location, object, ", type2);
          Output(outputline);
          sprintf(outputline, "attribute, timer, direction, number, word or NONE.\n");
          Output(outputline);
          return(ERROR);
      }
      break;

    case STARTTIMER:
      /*********************/
      /* starttimer(timer) */
      /*********************/

      if (type1 != TIM_ID) {
        sprintf(outputline, "StartTimer(): parameter 1 must be of type timer.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case SHUFFLE:
      /********************/
      /* shuffle(loc/obj) */
      /********************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        sprintf(outputline, "shuffle(): parameter 1 must be of type location or object\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case STOPTIMER:
      /********************/
      /* stoptimer(timer) */
      /********************/

      if (type1 != TIM_ID) {
        sprintf(outputline, "StopTimer(): parameter 1 must be of type timer.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case SUB:
      /****************************************************/
      /* sub(attribute, attribute/value, attribute/value) */
      /****************************************************/

      if (type1 != COMMON_ATTRS && type1 != ATTRIBUTES &&
          type1 != TIM_ID) {
        sprintf(outputline, "sub(): parameter 1 must be of type attribute or timer.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type2 != NUMBER && type2) {
        sprintf(outputline, "sub(): parameter 2 must be of type NUMBER.\n");
        Output(outputline);
        return(ERROR);
      }
      if (type3 != NUMBER) {
        sprintf(outputline, "sub(): type 3 must be of type number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case TEXT:
      /**************/
      /* text(word) */
      /**************/

      if (type1 != WORD_ID) {
        sprintf(outputline, "text(): parameter must be a word.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case TRIGGER:
      /********************/
      /* trigger(trigger) */
      /********************/

      if (type1 != COMMON_TRIGGERS && type1 != TRIGG_ID) {
        sprintf(outputline, "Execute(): parameter 1 must be of type trigger.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case UNDERLINE:
      /*******************/
      /* underline(word) */
      /*******************/

      if (type1 != WORD_ID) {
        sprintf(outputline, "underline(): parameter must be a word.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    case WAIT:
      /**********/
      /* wait() */
      /**********/

      if (type1 != NUMBER) {
        sprintf(outputline, "wait(): parameter must be a number.\n");
        Output(outputline);
        return(ERROR);
      }
      return(OK);
      break;

    default:
      sprintf(outputline, "CheckPars()(): error, undefined function code: %d.\n", fun_code);
      Output(outputline);
      return(ERROR);
  } /* switch */
}
