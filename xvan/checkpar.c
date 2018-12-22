
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018 , 2019 Marnix van den Bos.            */
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
        TypeErr(1, "CANSEE", "location or object id");
        return(ERROR);
      }
      if (type2 != LOC_ID && type2 != OBJ_ID) {
        TypeErr(1, "CANSEE", "location or object id");
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
        TypeErr(1, "LT", "number");
        return(ERROR);
      }
      if (type2 != NUMBER) {
        TypeErr(2, "LT", "number");
        return(ERROR);
      }
      return(OK);
      break;

    case GREATER_THAN:
      /****************/
      /* gt(num, num) */
      /****************/

      if (type1 != NUMBER) {
        TypeErr(1, "GT", "number");
        return(ERROR);
      }
      if (type2 != NUMBER) {
        TypeErr(2, "GT", "number");
        return(ERROR);
      }
      return(OK);
      break;

    case SETCURSOR:
      /***********************/
      /* setcursor(num, num) */
      /***********************/

      if (type1 != NUMBER) {
        TypeErr(1, "SETCURSOR", "number");
        return(ERROR);
      }
      if (type2 != NUMBER) {
        TypeErr(2, "SETCURSOR", "number");
        return(ERROR);
      }
      return(OK);
      break;

    case EXIT:
      /*****************/
      /* exit(loc/obj) */
      /*****************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "EXIT", "location or object");
        return(ERROR);
      }
      return(OK);
      break;

    case ISLIT:
      /******************/
      /* IsLit(loc/obj) */
      /******************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "ISLIT", "location or object");
        return(ERROR);
      }
      return(OK);
      break;

    case ISOBJECT:
      /*********************/
      /* IsObject(loc/obj) */
      /*********************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "ISOBJECT", "location or object");
        return(ERROR);
      }
      return(OK);
      break;

    case COUNT:
      /******************************************************/
      /*    Count(loc/obj, common flag, 0/1 [,level]) */
      /******************************************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "COUNT", "location or object id");
        return(ERROR);
      }
      if (type2 != COMMON_FLAGS) {
        TypeErr(2, "COUNT", "flag");
        return(ERROR);
      }
      if (type3 != NUMBER) {
        TypeErr(3, "COUNT", "number");
        return(ERROR);
      }
      if (type4 != NUMBER) {
        TypeErr(4, "COUNT", "number");
        return(ERROR);
      }
      return(OK);
      break;

    case SYNCHRONIZE:
      /*********************************************************************/
      /*    Synchronize(loc/obj, trigger, common flag, 0/1 [,level]) */
      /*********************************************************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "SYNCHRONIZE", "location or object");
        return(ERROR);
      }
      if (type2 != COMMON_TRIGGERS) {
        TypeErr(2, "SYNCHRONIZE", "common trigger");
        return(ERROR);
      }
      if (type3 != COMMON_FLAGS) {
        TypeErr(3, "SYNCHRONIZE", "flag");
        return(ERROR);
      }
      if (type4 != NUMBER) {
        TypeErr(4, "SYNCHRONIZE", "number");
        return(ERROR);
      }
      if (type5 != NUMBER) {
        TypeErr(5, "SYNCHRONIZE", "number");
        return(ERROR);
      }
      return(OK);
      break;

        case OWNS:
      /*************************************/
      /* owns(loc/obj, obj, num [,prepos]) */
      /*************************************/
      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "OWNS", "location or object.");
        return(ERROR);
      }
      if (type2 != LOC_ID && type2 != OBJ_ID)  {
        /* we must also allow type location for par2  */
        /* because they may use the 'this' parameter. */
        TypeErr(2, "OWNS", "location or object.");
        return(ERROR);
      }
      if (type4 != NO_TYPE) {
        /* there are 4 parameters */
        if (type3 != NUMBER) {
          TypeErr(3, "OWNS", "number.");
          return(ERROR);
        }
        if (type4 != WORD_ID) {
          TypeErr(4, "OWNS", "word (preposition).");
          return(ERROR);
        }
      }
      else {
        /* there are 3 parameters */
        if (!(type3 == NUMBER || type3 == WORD_ID)) {
          TypeErr(3, "OWNS", "number or word (preposition).");
          return(ERROR);
        }
      }
      return(OK);
      break;

    case TESTFLAG:
      /*****************/
      /* tstflag(flag) */
      /*****************/

      if (type1 != COMMON_FLAGS && type1 != FLAG_ID) {
        TypeErr(1, "TESTFLAG", "flag");
        return(ERROR);
      }
      return(OK);
      break;

    case TRY:
      /********************************************************/
      /* try(loc/obj, number, number, action record members). */
      /********************************************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, TranslateKeyword("TRY"), "location or object");
        return(ERROR);
      }

      if (type2 != NUMBER) {
        TypeErr(2, TranslateKeyword("TRY"), TranslateKeyword("number"));
        return(ERROR);
      }

      if (type3 != NUMBER) {
        TypeErr(3, TranslateKeyword("TRY"), TranslateKeyword("number"));
        return(ERROR);
      }

      if (type4 != ACTION_REC) {
        TypeErr(4, TranslateKeyword("TRY"), "action record");
        return(ERROR);
      }
      return(OK);
      break;

    case VALID_DIRECTION:
      /***************************/
      /* valdir(loc, direction). */
      /***************************/

      if (type1 != LOC_ID) {
        TypeErr(1, "VALDIR", "location");
        return(ERROR);
      }
      if (type2 != DIRECTIONS) {
        TypeErr(2, "VALDIR", "direction");
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
        TypeErr(1, "ADD", "attribute or timer");
        return(ERROR);
      }
      if (type2 != NUMBER) {
        TypeErr(2, "ADD", "number");
        return(ERROR);
      }
      if (type3 != NUMBER) {
        TypeErr(3, "ADD", "number");
        return(ERROR);
      }
      return(OK);
      break;

    case ADDJSON:
      if (type1 != STRING) {
        TypeErr(1, "ADDJSON", "string");
        return(ERROR);
      }
      if (type2 != LOC_ID && type2 != OBJ_ID) {
        TypeErr(2, "ADDJSON", "location or object");
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
        TypeErr(1, "MUL", "attribute or timer");
        return(ERROR);
      }
      if (type2 != NUMBER) {
        TypeErr(2, "MUL", "number");
        return(ERROR);
      }
      if (type3 != NUMBER) {
        TypeErr(3, "MUL", "attribute or timer");
        return(ERROR);
      }
      return(OK);
      break;

    case BACKGROUND:
     /********************/
     /* background(word) */
     /********************/

      if (type1 != WORD_ID) {
        TypeErr(1, "BACKGROUND", "word");
        return(ERROR);
      }
      return(OK);
      break;

    case BLOCK_EXIT:
      /***********************/
      /* blockexit(loc, dir) */
      /***********************/

      if (type1 != LOC_ID) {
        TypeErr(1, "BLOCKEXIT", "location");
        return(ERROR);
      }
      if (type2 != DIRECTIONS) {
        TypeErr(2, "BLOCKEXIT", "direction");
        return(ERROR);
      }
      return(OK);
      break;

    case BOLD:
      /**************/
      /* bold(word) */
      /**************/

      if (type1 != WORD_ID) {
        TypeErr(1, "BOLD", "word");
        return(ERROR);
      }
      return(OK);
      break;

    case CLEARFLAG:
      /*******************/
      /* clearflag(flag) */
      /*******************/

      if (type1 != COMMON_FLAGS && type1 != FLAG_ID) {
        TypeErr(1, "CLEARFLAG", "flag");
        return(ERROR);
      }
      return(OK);
      break;

    case CONTENTS:
      /*********************/
      /* Contents(loc/obj) */
      /*********************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "CONTENTS", "location or object");
        return(ERROR);
      }
      return(OK);
      break;

    case DEST:
      /****************************/
      /* dest(loc/obj, direction) */
      /****************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "DEST", "location or object");
        return(ERROR);
      }
      if (type2 != DIRECTIONS) {
        TypeErr(2, "DEST", "direction");
        return(ERROR);
      }
      return(OK);
      break;

    case DISTANCE:
      /******************************/
      /* distance(loc/obj, loc/obj) */
      /******************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "DISTANCE", "location or object");
        return(ERROR);
      }
      if (type2 != LOC_ID && type2 != OBJ_ID) {
        TypeErr(2, "DISTANCE", "location or object");
        return(ERROR);
      }
      return(OK);
      break;

    case ENTRANCE:
      /*********************/
      /* Entrance(loc/obj) */
      /*********************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "ENTRANCE", "location or object");
        return(ERROR);
      }
      return(OK);
      break;

    case FIRSTDIR:
      /******************************/
      /* firstdir(loc/obj, loc/obj) */
      /******************************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "FIRSTDIR", "location or object");
        return(ERROR);
      }
      if (type2 != LOC_ID && type2 != OBJ_ID) {
        TypeErr(2, "FIRSTDIR", "location or object");
        return(ERROR);
      }
      return(OK);
      break;

    case GO_TO:
      /***********************/
      /* goto(obj, loc, num) */
      /***********************/

      if (type1 != OBJ_ID) {
        TypeErr(1, "GOTO", "object");
        return(ERROR);
      }
      if (type2 != LOC_ID) {
        TypeErr(2, "GOTO", "location");
        return(ERROR);
      }
      if (type3 != NUMBER) {
        TypeErr(3, "GOTO", "number");
        return(ERROR);
      }
      return(OK);
      break;

    case INDENT:
      /*****************/
      /* indent([num]) */
      /*****************/

      if (type1 != NUMBER) {
        TypeErr(1, "INDENT", "number");
        return(ERROR);
      }
      return(OK);
      break;

   case ITALIC:
      /****************/
      /* italic(word) */
      /****************/

      if (type1 != WORD_ID) {
        TypeErr(1, "ITALIC", "word");
        return(ERROR);
      }
      return(OK);
      break;

    case MOVE:
      /************************************************/
      /* move(obj, loc/obj/direction [, preposition]) */
      /************************************************/

      if (type1 != OBJ_ID) {
        TypeErr(1, "MOVE", "object");
        return(ERROR);
      }
      if (type2 != LOC_ID && type2 != OBJ_ID && type2 != DIRECTIONS) {
        TypeErr(2, "MOVE", "location, object or direction");
        return(ERROR);
      }
      if (type3 != WORD_ID) {
        TypeErr(3, "MOVE", "word");
        return(ERROR);
      }
      return(OK);
      break;

    case NEW_EXIT:
      /**************************/
      /* newexit(loc, dir, loc) */
      /**************************/

      if (type1 != LOC_ID) {
        TypeErr(1, "NEWEXIT", "location");
        return(ERROR);
      }
      if (type2 != DIRECTIONS) {
        TypeErr(2, "NEWEXIT", "direction");
        return(ERROR);
      }
      if (type3 != LOC_ID) {
        TypeErr(3, "NEWEXIT", "location");
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
        TypeErr(1, "OWNER", "location or object");
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
          PrintError(4, NULL, NULL);
          return(ERROR);
      }
      break;

    case QUOT:
      /***********************************************************************/
      /* quot(attribute/timer, attribute/timer/value, attribute/timer/value) */
      /***********************************************************************/

      if (type1 != COMMON_ATTRS && type1 != ATTRIBUTES &&
          type1 != TIM_ID) {
        TypeErr(1, "DIV", "attribute or timer");
        return(ERROR);
      }
      if (type2 != NUMBER) {
        TypeErr(2, "DIV", "number");
        return(ERROR);
      }
      if (type3 != NUMBER) {
        TypeErr(3, "DIV", "number");
        return(ERROR);
      }
      return(OK);
      break;

    case RAND:
      /*********************/
      /* rnd(value, value) */
      /*********************/

      if (type1 != NUMBER && type1 != TIM_ID) {
        TypeErr(1, "RND", "number or timer");
        return(ERROR);
      }
      if (type2 != NUMBER && type2 != TIM_ID) {
        TypeErr(2, "RND", "number or timer");
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
        TypeErr(1, "REM", "attribute or timer");
        return(ERROR);
      }
      if (type2 != NUMBER) {
        TypeErr(2, "REM", "number");
        return(ERROR);
      }
      if (type3 != NUMBER) {
        TypeErr(3, "REM", "number");
        return(ERROR);
      }
      return(OK);
      break;

    case SCORE:
      /****************/
      /* score(value) */
      /****************/

      if (type1 != NUMBER) {
        TypeErr(1, "SCORE", "number");
        return(ERROR);
      }
      return(OK);
      break;

    case SETTIMER:
      /****************************/
      /* settimer(timer, value) */
      /****************************/
      if (type1 != TIM_ID) {
        TypeErr(1, "SETTIMER", "timer");
        return(ERROR);
      }
      if (type2 != NUMBER) {
        TypeErr(2, "SETTIMER", "number");
        return(ERROR);
      }
      return(OK);
      break;

    case SETFLAG:
      /*****************/
      /* setflag(flag) */
      /*****************/

      if (type1 != COMMON_FLAGS && type1 != FLAG_ID) {
        TypeErr(1, "SETFLAG", "flag");
        return(ERROR);
      }
      return(OK);
      break;

    case SETATTRIBUTE:
      /*****************************************************************************/
      /* setattribute([attribute, loc/obj/attribute/direction/value/word) */
      /*****************************************************************************/
      if (type1 != COMMON_ATTRS && type1 != ATTRIBUTES) {
        TypeErr(1, "SETATTRIBUTE", "location or object");
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
          PrintError(5, NULL, NULL);
          return(ERROR);
      }
      break;

    case STARTTIMER:
      /*********************/
      /* starttimer(timer) */
      /*********************/

      if (type1 != TIM_ID) {
        TypeErr(1, "STARTTIMER", "timer");
        return(ERROR);
      }
      return(OK);
      break;

    case SHUFFLE:
      /********************/
      /* shuffle(loc/obj) */
      /********************/

      if (type1 != LOC_ID && type1 != OBJ_ID) {
        TypeErr(1, "SHUFFLE", "location or object");
        return(ERROR);
      }
      return(OK);
      break;

    case STOPTIMER:
      /********************/
      /* stoptimer(timer) */
      /********************/

      if (type1 != TIM_ID) {
        TypeErr(1, "STOPTIMER", "timer");
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
        TypeErr(1, "SUB", "attribute or timer");
        return(ERROR);
      }
      if (type2 != NUMBER && type2) {
        TypeErr(2, "SUB", "number");
        return(ERROR);
      }
      if (type3 != NUMBER) {
        TypeErr(3, "SUB", "number");
        return(ERROR);
      }
      return(OK);
      break;

    case TEXT:
      /**************/
      /* text(word) */
      /**************/

      if (type1 != WORD_ID) {
        TypeErr(1, "TEXT", "word");
        return(ERROR);
      }
      return(OK);
      break;

    case TRIGGER:
      /********************/
      /* trigger(trigger) */
      /********************/

      if (type1 != COMMON_TRIGGERS && type1 != TRIGG_ID) {
        TypeErr(1, "TRIGGER", "trigger");
        return(ERROR);
      }
      return(OK);
      break;

    case UNDERLINE:
      /*******************/
      /* underline(word) */
      /*******************/

      if (type1 != WORD_ID) {
        TypeErr(1, "UNDERLINE", "word");
        return(ERROR);
      }
      return(OK);
      break;

    case WAIT:
      /**********/
      /* wait() */
      /**********/

      if (type1 != NUMBER) {
        TypeErr(1, "WAIT", "number");
        return(ERROR);
      }
      return(OK);
      break;

    default:
      PrintError(6, &((resultStruct) {VALUE, fun_code}), NULL);
      return(ERROR);
  } /* switch */
}
