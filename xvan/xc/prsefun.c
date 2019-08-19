
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
#include <stdlib.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "prsefun.h"

/*************************/
/* Function declarations */
/*************************/

int32_t  HasType(int32_t, int32_t);
int32_t  IsType(int32_t, int32_t);
int32_t  NextPar(int32_t*, int32_t*);
void     ActionRecToIntArray(int32_t*, int32_t*, actionRec);
int32_t  CheckPars(int32_t*);
int32_t  ParseFun(int32_t*, int32_t*, int32_t, int32_t, FILE**, fileList**);
int32_t  ParsePar(char**, int32_t*, int32_t*, int32_t*, int32_t, int32_t, FILE**, fileList**);


/************************/
/* Function definitions */
/************************/

int32_t HasType(int32_t id, int32_t type)
/* id is either word id, function id, or attribute id. */
{
  /* This routine checks whether the word or function */
  /* denoted by id has a type of type `type'.         */

  /* This routine works for words, internal actions and */
  /* attributes only, because they are the only entities */
  /* that may have more than one type.                  */

  wordTable *wt = word_table;
  int32_t       i   = 0;
  int32_t       j   = 0;


  /* Check whether it's a word */

  if (IsWordId(id)) {
    if (type == WORD_ID)
      /* It may be any word. ACTION and PREPOS are included in IsWordId() */
      return(OK);

    /* 26jul15 -- First check for special words VERB and PREPOS.    */
    /* These are not in the word_table and will thus cause an error */
    switch (id) {
      case ACTION:
        return(type == VERB);
      case PREPOS:
        return(type == PREPOSITIONS);
    } /* switch */

    /* Find id. */
    while (i<nr_of_words && wt[i].id != id)
      i++;

    if (i == nr_of_words) {
      /* Internal error; this shouldn't happen. */
      ErrHdr();
      PrintError(67, NULL, NULL);
    }
    else {
      while (j<MAX_TYPES && wt[i].types[j] != NO_TYPE &&
                                                wt[i].types[j] != type)
        j++;

      if (j<MAX_TYPES && wt[i].types[j] != NO_TYPE)
        return(OK);}

    return(ERROR);
  }

  /* Check whether it's an internal action. */
  else if (IsIntAct(id)) {
    switch (id) {
      case OWNER:
        if (type != LOC_ID && type != OBJ_ID)
          return(ERROR);
        break;
      case RAND:
      case DISTANCE:
      case COUNT:
      case SYNCHRONIZE:
        if (type != NUMBER)
          return(ERROR);
        break;
      case FIRSTDIR:
        if (type != DIRECTIONS)
          return(ERROR);
        break;
      case DEST:
        if (type != LOC_ID)
          return(ERROR);
        break;
      case PICKONE:
        /* can have all types except string */
        if (type == STRING)
          return(ERROR);
        break;
      default:
        /* This internal action has no type. */
        return(ERROR);
    }  /* switch */
    return(OK);
  }

  /* Check whether it's an attribute. */
  /* Function HasType() checks the type of the value that is held by the attribute, */
  /* not the attribute type itself. This is done in function IsType()               */
  else if (IsCAttrId(id) || IsLAttrId(id)) {
    switch (type) {
      case LOC_ID:       /* attribute may hold a location id   */
      case OBJ_ID:       /* attribute may hold an object id    */
      case WORD_ID:      /* attribute may hold a word id       */
      case NUMBER:       /* attribute may hold a number        */
      case DIRECTIONS:   /* attribute may hold a direction     */
        return(OK);
        break;
      default:
        return(ERROR);
    } /* switch */
  }
  else {
    /* Neither a word, nor an internal action, nor an attribute. */
    /* Now, try IsType().                                        */
    return(IsType(id, type));
  }
}


int32_t IsType(int32_t id, int32_t type)
{
  /* This routine returns true if id has type as   */
  /* indicated by the type parameter.              */
  /* STRING and NUMBER types are handled a bit     */
  /* different, since such parameters are preceded */
  /* by a keyword.                                 */

  /* Attributes require special attention. If the type to be checked is         */
  /* Common or Local attribute, we must check whether ID is an attribute id. If   */
  /* type to be checked is something else and the id is an attribute id, we must */
  /* check whether the attribute's value is of the type to be checked (done by  */
  /* calling HasType() function.                                               */

  if (IsIntAct(id) || (IsCAttrId(id) && (type != COMMON_ATTRS)) || (IsLAttrId(id) && (type != ATTRIBUTES)))
    /* Internal actions and attributes may have more than one type. */
    return(HasType(id, type));

  switch (type) {
    case LOC_ID:
      return(IsLocId(id));
    case OBJ_ID:
      return(IsObjId(id));
    case COMMON_DESCRS:
      return(IsCDescrId(id));
    case DESCRIPTIONS:
      return(IsLDescrId(id));
    case COMMON_FLAGS:
      return(IsCFlagId(id));
    case FLAGS:
       return(IsLFlagId(id));
    case COMMON_ATTRS:
      return(IsCAttrId(id));
    case ATTRIBUTES:
      return(IsLAttrId(id));
    case COMMON_TRIGGERS:
      return(IsCTriggId(id));
    case TRIGGERS:
      return(IsLTriggId(id));
    case TIM_ID:
      return(IsTimerId(id));
    case DIRECTIONS:
      /* 'dir' parameter was used in a function        */
      /* 'normal' directions are handled by HasType(). */
      return(id == DIR);
    case STRING:
      return(id == STRING);
    case ACTION_REC:
      return(id == ACTION_REC);
    case NUMBER:
      /* Value denotes the 'value' parameter (current value). */
      return(id == NUMBER || id == VALUE || id == ORDINAL);
    default:
      /* Check cannot be done before switch statement, because   */
      /* then HasType() will print an error msg for all types    */
      /* other than a word-type (i.e. HasType(PREPOS, LOC_ID)).  */
      /* We cannot use a WORD_ID: case, because then we must use */
      /* cases for all word-types.                               */

      if (IsWordId(id))
        /* Words may have more than one type. */
        return(HasType(id, type));
      else
        return(ERROR);
  } /* switch */
}


int32_t NextPar(int32_t *fun_string, int32_t *index)
{
  /* This routine returns the parameter prior to the next   */
  /* END_OF_PAR keyword from a function's parameter list.   */
  /* Index is set to the parameter (if any). We only need   */
  /* to return this par, because it determines the type of  */
  /* the total parameter. Checks for special id - local par */
  /* combinations have already been done by ParsePar().     */
  /* In case of a NUMBER or STRING par, the keyword is      */
  /* returned. The par itself is skipped. In case of an     */
  /* internal action parameter, the keyword is returned and */
  /* the rest of the function (which has already been       */
  /* typechecked) is skipped.                               */

  int32_t level;
  int32_t par = NO_ID;
  int32_t len;

  switch (fun_string[*index]) {
    case STRING:
      /* Calculate len of string (include \0 character). */
      len = fun_string[(*index)+1];

      /* Skip the string and END_OF_PAR keyword. */
      *index += len+3;
      return(STRING);
    case ACTION_REC:
      /* Calculate length of action record */
      len = 11 + MAX_PARSE_PREPOS+1 + MAX_PARSE_ADVERBS+1;

      /* skip the action record and END_OF_PAR keyword */
      *index += len+3;
      return(ACTION_REC);
    case NUMBER:
      /* Skip the number and END_OF_PAR keyword. */
      *index += 3;
      return(NUMBER);
    default:
      /* Either a function or a `normal' parameter. */
      while (fun_string[*index] != END_OF_PAR) {
        if (IsIntAct(fun_string[*index])) {
          par = fun_string[(*index)];
          level = 0;

          /* Set index to the left parenthesis (skip nr_of_pars). */
          (*index) += 2;

          /* Skip the function. */
          do {
            if (fun_string[*index] == LEFT_PAR)
              level++;
            else if (fun_string[*index] == RIGHT_PAR)
              level--;

            (*index)++;
          } while (level != 0);
        } /* if */
        else {
          /* It's a `normal' id. */
          par = fun_string[*index];
          (*index)++;
        }
      } /* while */

      /* Skip END_OF_PAR. */
      (*index)++;

      return(par);
  } /* switch */
}


int32_t CheckPars(int32_t *fun_string)
{
  /* This routine checks the parameter syntax for the function  */
  /* contained in fun_string. Function syntax is:               */
  /* fun_code nr_of_pars left_parenthesis par_list right_par    */

  char    fun_name[MAX_WORD_LEN];  /* For error routine.                     */
  int32_t index = 3;               /* skip the function code, nr_of_pars and */
                                   /* left parenthesis                       */
  int32_t nr_of_pars;
  int     i;
  int32_t par;                     /* Needed for typechecking.               */

  nr_of_pars = fun_string[1];

  /* Initialize fun_name. */
  fun_name[0] = '\0';

  switch (fun_string[0]) {  /* function code */
    case CANSEE:            /* cansee(loc/obj, loc/obj) */
      if (nr_of_pars != 2) {
        NrErr("cansee()", "2");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID))) {
        TypeErr(1, "cansee()", "location id or object id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID))) {
        TypeErr(2, "cansee()", "location id or object id");
        return(ERROR);
      }
      break;

    case OWNS:              /* owns(loc/obj, obj [, number] [, preposition]) */
      if (nr_of_pars != 2 && nr_of_pars != 3 && nr_of_pars != 4) {
        NrErr("owns()", "2, 3 or 4");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID))) {
        TypeErr(1, "owns()",  "location id or object id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!IsType(par, OBJ_ID)) {
        TypeErr(2, "owns()", "object id");
        return(ERROR);
      }

      if (nr_of_pars == 3) {
        par = NextPar(fun_string, &index);
        if (!(IsType(par, NUMBER) || IsType(par, PREPOSITIONS))) {
          TypeErr(3, "owns()", "number or preposition");
          return(ERROR);
        }
      }

      if (nr_of_pars == 4) {
        /* read par 3 */
        par = NextPar(fun_string, &index);
        if (!IsType(par, NUMBER)) {
          TypeErr(3, "owns()", "number");
          return(ERROR);
        }
        /* read par 4 */
        par = NextPar(fun_string, &index);
        if (!IsType(par, PREPOSITIONS)) {
          TypeErr(3, "owns()", "preposition");
          return(ERROR);
        }
      }
      break;

    case EQUAL:
      /* equal(loc/obj/attr/timer/value/dir/descr/word, loc/obj/attr/timer/value/dir/descr/word) */
      if (nr_of_pars != 2) {
        /* nr of pars error */
        NrErr("equal()", "2");
        return(ERROR);
      }
      /* The check that both pars must be of the same type will be */
      /* done by the interpreter, because we can only check the    */
      /* actual type of attributes at runtime.                      */

      /* check first par */
      par  = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID) || IsType(par, COMMON_ATTRS) ||
            IsType(par, ATTRIBUTES) || IsType(par, TIM_ID) || IsType(par, NUMBER) ||
            HasType(par, DIRECTIONS) || IsType(par, WORD_ID) || IsType(par, COMMON_DESCRS) ||
            IsType(par, DESCRIPTIONS))) {
        TypeErr(1, "equal()", "location, object, attribute, timer, value, direction or word");
        return(ERROR);
      }
      /* check second par */
      par  = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID) || IsType(par, COMMON_ATTRS) ||
            IsType(par, ATTRIBUTES) || IsType(par, TIM_ID) || IsType(par, NUMBER) ||
            HasType(par, DIRECTIONS) || IsType(par, WORD_ID) || IsType(par, COMMON_DESCRS) ||
            IsType(par, DESCRIPTIONS))) {
        TypeErr(2, "equal()", "location, object, attribute, timer, value, direction or word");
        return(ERROR);
      }
      break;

    case LESS_THAN:
      /* lt(attr/timer/value, attr/timer/value) */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "lt()",MAX_WORD_LEN);
    case GREATER_THAN:
      /* gt(attr/timer/value, attr/timer/value) */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "gt()", MAX_WORD_LEN);
    case SETCURSOR:
      /* setcursor(attr/timer/value, attr/timer/value) */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "setcursor()", MAX_WORD_LEN);

      if (nr_of_pars != 2) {
        /* nr of pars error */
        NrErr("lt()", "2");
        return(ERROR);
      }
      /* The check that both pars must be of the same type will be */
      /* done by the interpreter, because we can only check the    */
      /* actual type of attributes at runtime.                      */

      /* check first par */
      par  = NextPar(fun_string, &index);
      if (!(IsType(par, COMMON_ATTRS) || IsType(par, ATTRIBUTES) ||
            IsType(par, TIM_ID) || IsType(par, NUMBER))) {
        TypeErr(1, fun_name, "attribute, timer or value.");
        return(ERROR);
      }
      /* check second par */
      par  = NextPar(fun_string, &index);
      if (!(IsType(par, COMMON_ATTRS) || IsType(par, ATTRIBUTES) ||
            IsType(par, TIM_ID) || IsType(par, NUMBER))) {
        TypeErr(2, fun_name, "attribute, timer or value.");
        return(ERROR);
      }
      break;

    case TRY:  /* try(loc/obj, int, int, string) */
      if (nr_of_pars == 4) {
        /* check first par */
        par = NextPar(fun_string, &index);
        if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID))) {
          TypeErr(1, TranslateKeyword("try"), TranslateKeyword("location id or object id"));
          return(ERROR);
        }
        /* check second par */
        par = NextPar(fun_string, &index);
        if (!IsType(par, NUMBER)) {
          TypeErr(2, TranslateKeyword("try"), TranslateKeyword("number"));
          return(ERROR);
        }
        /* check third par */
        par = NextPar(fun_string, &index);
        if (!IsType(par, NUMBER)) {
          TypeErr(3, TranslateKeyword("try"), TranslateKeyword("number"));
          return(ERROR);
        }
        /* check fourth par */
        par = NextPar(fun_string, &index);
        if (!IsType(par, ACTION_REC)) {
          TypeErr(4, TranslateKeyword("try"), TranslateKeyword("string"));
          /* error msg must say string or the user won't understand */
          return(ERROR);
        }
      } /* if */
      else {
        NrErr(TranslateKeyword("try"), "4");
        return(ERROR);
      }
      break;

    case VALID_DIRECTION:  /* valdir(loc, dir) */
      if (nr_of_pars == 2) {
        par = NextPar(fun_string, &index);
        if (!IsType(par, LOC_ID)) {
          TypeErr(1, "valdir()", "location id");
          return(ERROR);
        }
        par = NextPar(fun_string, &index);
        if (!HasType(par, DIRECTIONS)) {
          TypeErr(2, "valdir()", TranslateKeyword("directions"));
          return(ERROR);
        } /* if */
      } /* if */
      else {
        NrErr("valdir()", "2");
        return(ERROR);
      }
      break;

    case TESTFLAG:         /* XXXflag(loc/obj.commonflag) */
                           /* XXXflag(loc/obj.localflag)  */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "testflag()", MAX_WORD_LEN);
    case SETFLAG:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "setflag()", MAX_WORD_LEN);
    case CLEARFLAG:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "clearflag()", MAX_WORD_LEN);
      if (nr_of_pars != 1) {
        /* nr of pars error */
        NrErr(fun_name, "1");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, FLAGS) || IsType(par, COMMON_FLAGS))) {
        TypeErr(1, fun_name, "flag id");
        return(ERROR);
      }
      break;

    case ADD:
    /* plus(attribute/timer, attribute/timer, attribute/timer/value)  */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "+", MAX_WORD_LEN);
    case SUB:
      /* minus(attribute/timer, attribute/timer, attribute/timer/value)  */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "-", MAX_WORD_LEN);
    case ASTERIX:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "*", MAX_WORD_LEN);
    case QUOT:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "div", MAX_WORD_LEN);
    case REM:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "rem", MAX_WORD_LEN);

      if (nr_of_pars != 3) {
        NrErr(fun_name, "3");
        return(ERROR);
      }
      /* check parameter 1 */
      par = NextPar(fun_string, &index);
      if (!(IsType(par, ATTRIBUTES) || IsType(par, COMMON_ATTRS) ||
            IsType(par, TIM_ID))) {
        TypeErr(1, fun_name, "attribute or timer"); printf("\n%d\n", par);
        return(ERROR);
      }

      /* check parameter 2. In case of NONE, this means the function   */
      /* is the += or -= function which has one less parameter (we     */
      /* want to use ADD/SUB for both + and += and - and -= functions. */
      par = NextPar(fun_string, &index);
      if (!(IsType(par, ATTRIBUTES) || IsType(par, COMMON_ATTRS) ||
            IsType(par, TIM_ID) || IsType(par, NUMBER) || par == NONE)) {
        TypeErr(2, fun_name, "attribute, timer or number"); printf("\n%d\n", par);
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, ATTRIBUTES) || IsType(par, COMMON_ATTRS) ||
            IsType(par, TIM_ID) || IsType(par, NUMBER))) {
        TypeErr(3, fun_name, "attribute, timer or number"); printf("\n%d\n", par);
        return(ERROR);
      }
      break;

    /* do we still use the EQ_... funtions??? */
    case EQ_ADD:
      /* eq_plus(attribute/timer, attribute/timer/value) */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "+=", MAX_WORD_LEN);
    case EQ_SUB:
      /* eq_sub(attribute/timer, attribute/timer/value   */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "-=", MAX_WORD_LEN);
    case EQ_MUL:
      /* eq_mul(attribute/timer, attribute/timer/value   */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "*=", MAX_WORD_LEN);

      if (nr_of_pars != 2) {
        NrErr(fun_name, "2");
        return(ERROR);
      }
      par = NextPar(fun_string, &index);
      if (!(IsType(par, ATTRIBUTES) || IsType(par, COMMON_ATTRS) ||
            IsType(par, TIM_ID))) {
        TypeErr(1, fun_name, "attribute or timer"); printf("\n%d\n", par);
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, ATTRIBUTES) || IsType(par, COMMON_ATTRS) ||
            IsType(par, TIM_ID) || IsType(par, NUMBER))) {
        TypeErr(2, fun_name, "attribute, timer or number"); printf("\n%d\n", par);
        return(ERROR);
      }
      break;

    case RAND:
      if (nr_of_pars != 2) {
        NrErr("rnd()", "2");
        return(ERROR);
      }
      par = NextPar(fun_string, &index);
      if (!(IsType(par, ATTRIBUTES) || IsType(par, COMMON_ATTRS) ||
            IsType(par, NUMBER))) {
        TypeErr(1, "rnd()", "attribute id or value"); printf("\n%d\n", par);
        return(ERROR);
      }
      par = NextPar(fun_string, &index);
      if (!(IsType(par, ATTRIBUTES) || IsType(par, COMMON_ATTRS) ||
            IsType(par, NUMBER))) {
        TypeErr(2, "rnd()", "attribute id or value"); printf("\n%d\n", par);
        return(ERROR);
      }
      break;

    case SETATTRIBUTE:            /* setattribute([loc/obj].commonattribute, loc/obj/attribute/description/direction/value/word) */
      if (nr_of_pars != 2) {
        NrErr("setattribute()", "2");
        return(ERROR);
      }
      par = NextPar(fun_string, &index);
      if (!(IsType(par, ATTRIBUTES) || IsType(par, COMMON_ATTRS))) {
        TypeErr(1, "setattribute()", "attribute."); printf("\n%d\n", par);
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      /* location, object, direction, value or word             */
      /* check for value must be done first?                    */
      /* check for Direction must be done before check for Word */
      if (!(IsType(par, NUMBER) || IsType(par, LOC_ID) || IsType(par, TIM_ID) ||
            IsType(par, OBJ_ID) || HasType(par, DIRECTIONS) || IsType(par, WORD_ID) ||
            IsType(par, COMMON_DESCRS) || IsType(par, DESCRIPTIONS))) {
        TypeErr(2, "setattribute()", "attribute, value, timer, location, object, direction, description or word.");
        return(ERROR);
      }
      break;

    case EXIT:              /* exit(loc/obj)     */
      if (fun_name[0] == '\0')
        strncpy(fun_name,"exit()", MAX_WORD_LEN);

    case ISLIT:             /* islit(loc/obj)    */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "islit()", MAX_WORD_LEN);

    case CONTENTS:          /* contents(loc/obj) */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "contents()", MAX_WORD_LEN);

    case ENTRANCE:          /* entrance(loc/obj) */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "entrance()", MAX_WORD_LEN);

    case SHUFFLE:       /* shuffle(loc/obj) */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "shuffle()", MAX_WORD_LEN);

    case ISOBJECT:       /* isobject(loc/obj) */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "isobject()", MAX_WORD_LEN);

      if (nr_of_pars != 1) {
        NrErr(fun_name, "1");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID))) {
        TypeErr(1, fun_name, "location or object id");
        return(ERROR);
      }
      break;

    case TRIGGER:                /* trigger([loc/obj.]ltrig) */
      if (nr_of_pars != 1) {     /* trigger([loc/obj.]ctrig) */
        NrErr("trigger()", "1");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, TRIGGERS) || IsType(par, COMMON_TRIGGERS))) {
        TypeErr(1, "trigger()", "local or common trigger id");
        return(ERROR);
      }
      break;

    case OWNER:            /* owner(obj) */
      if (nr_of_pars == 1) {
        par = NextPar(fun_string, &index);

        if (!IsType(par, OBJ_ID)) {
          TypeErr(1, "owner", "object id");
          return(ERROR);
        }
      }
      else {
        NrErr("owner", "1");
        return(ERROR);
      }
      break;

    case GO_TO:          /* goto(obj, loc [, number]) */
      if (nr_of_pars !=2 && nr_of_pars != 3) {
        NrErr("goto()", "2 or 3");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!IsType(par, OBJ_ID)) {
        TypeErr(1, "goto()", "object id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!IsType(par, LOC_ID)) {
        TypeErr(2, "goto()", "location id");
        return(ERROR);
      }

      if (nr_of_pars == 3) {
        par = NextPar(fun_string, &index);
        if (!IsType(par, NUMBER)) {
          TypeErr(3, "goto()", "number");
          return(ERROR);
        }
      }
      break;

    case FIRSTDIR:          /* firstdir(loc/obj, loc/obj)   */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "firstdir()", MAX_WORD_LEN);

    case DISTANCE:          /* distance(loc/obj, loc/obj)   */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "distance()", MAX_WORD_LEN);
      if (nr_of_pars != 2) {
        NrErr("fun_name", "2");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID))) {
        TypeErr(1, "fun_name", "location or object");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID))) {
        TypeErr(2, "fun_name", "location or object id");
        return(ERROR);
      }
      break;

    case DEST:          /* dest(loc/obj, dir)   */
      if (nr_of_pars != 2) {
        NrErr("dest()", "2");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID))) {
        TypeErr(1, "dest()", "location or object.");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(HasType(par, DIRECTIONS))) {
        TypeErr(2, "dest()", "direction");
        return(ERROR);
      }
      break;

    case MOVE:          /* move(obj, loc/obj/dir [,word])   */
      if (nr_of_pars != 2 && nr_of_pars != 3) {
        NrErr("move()", "2 or 3");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!IsType(par, OBJ_ID)) {
        TypeErr(1, "move()", "object id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID) ||
            HasType(par, DIRECTIONS))) {
        TypeErr(2, "move()", "location id, object id, or direction");
        return(ERROR);
      }

      if (nr_of_pars == 3) {
        par = NextPar(fun_string, &index);
        if (!IsType(par, WORD_ID)) {
          TypeErr(3, "move()", "word (preposition)");
          return(ERROR);
        }
      }
      break;

    case COUNT:             /* count(loc/obj, common flag, 0/1 [, level]) */
      if (nr_of_pars != 3 && nr_of_pars != 4) {
        NrErr("count()", "3 or 4");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID))) {
        TypeErr(1, "count()", "location or object id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, COMMON_FLAGS))) {
        TypeErr(2, "count()", "common flag id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!IsType(par, NUMBER)) {
        TypeErr(3, "count()", "number");
        return(ERROR);
      }

      if (nr_of_pars == 4) {
        par = NextPar(fun_string, &index);
        if (!IsType(par, NUMBER)) {
          TypeErr(4, "count()", "number");
          return(ERROR);
        }
      }
      break;

    case SYNCHRONIZE:       /* synchronize(loc/obj, trigger, flag, value [, number]) */
      if (nr_of_pars != 4 && nr_of_pars != 5) {
        NrErr("synchronize()", "4 or 5");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID))) {
        TypeErr(1, "synchronize()", "location or object id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, COMMON_TRIGGERS))) {
        TypeErr(2, "synchronize()", "common trigger id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, COMMON_FLAGS))) {
        TypeErr(3, "synchronize()", "common flag id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!IsType(par, NUMBER)) {
        TypeErr(4, "synchronize()", "number");
        return(ERROR);
      }

      if (nr_of_pars == 5) {
        par = NextPar(fun_string, &index);
        if (!IsType(par, NUMBER)) {
          TypeErr(5, "synchronize()", "number");
          return(ERROR);
        }
      }
      break;

    case NEW_EXIT:         /* newexit(loc, dir, loc)  */
      if (nr_of_pars != 3) {
        NrErr("newexit()", "3");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!IsType(par, LOC_ID)) {
        TypeErr(1, "newexit()", "location id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(HasType(par, DIRECTIONS))) {
        TypeErr(2, "newexit()", "direction");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, LOC_ID))) {
        TypeErr(2, "newexit()", "location id");
        return(ERROR);
      }
      break;

    case BLOCK_EXIT:       /* blockexit(loc, dir)     */
      if (nr_of_pars != 2) {
        NrErr("blockexit()", "2");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!IsType(par, LOC_ID)) {
        TypeErr(1, "blockexit()", "location id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(HasType(par, DIRECTIONS))) {
        TypeErr(2, "blockexit()", "direction");
        return(ERROR);
      }
      break;

    case ADDJSON:  /* addjson("string [, loc/obj]") */
      if (nr_of_pars != 1 && nr_of_pars != 2) {
        NrErr("addjson()", "1 or 2");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!(IsType(par, STRING))) {
        TypeErr(1, "addjson()", "string");
        return(ERROR);
      }

      if (nr_of_pars == 2) {
        par = NextPar(fun_string, &index);
        if (!(IsType(par, LOC_ID) || IsType(par, OBJ_ID))) {
          TypeErr(2, "addjson()", "location or object");
          return(ERROR);
        }
      }
      break;

    case PRINTCR:          /* same as PRINT           */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "printcr()", MAX_WORD_LEN);
    case PRINTCRBOLD:          /* same as PRINT       */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "printcrbold()", MAX_WORD_LEN);
    case PRINTCRITALIC:          /* same as PRINT     */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "printcritalic()", MAX_WORD_LEN);
    case PRINTBOLD:          /* same as PRINT           */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "printbold()", MAX_WORD_LEN);
    case PRINTITALIC:          /* same as PRINT           */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "printitalic()", MAX_WORD_LEN);
    case PRINTSTATUS:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "printstatus()", MAX_WORD_LEN);
    case PRINTCRSTATUS:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "printcrstatus()", MAX_WORD_LEN);

    case PRINT:            /* print("string")           */
                           /* print(loc/obj/timer)      */
                           /* print(loc/obj.ldescr)     */
                           /* print(loc/obj.cdescr)     */
                           /* print(loc/obj.cattribute) */
                           /* print(loc/obj.lattribute) */
                           /* print(word id)            */
                           /* print(number)             */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "print", MAX_WORD_LEN);

      if (nr_of_pars != 1) {
        NrErr(fun_name, "1");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);

      if (!(IsType(par, STRING) || IsType(par, LOC_ID) || IsType(par, OBJ_ID) ||
            IsType(par, DESCRIPTIONS) || IsType(par, COMMON_DESCRS) ||
            IsType(par, TIM_ID) || IsType(par, DIRECTIONS) ||
            IsType(par, WORD_ID) || IsType(par, NUMBER))) {
        TypeErr(1, fun_name, "string, location, object, description, timer, word, attribute or number.");
        return(ERROR);
      }
      break;

    case SETTIMER:        /* setcounter(timer, val) */
      if (nr_of_pars != 2) {
        NrErr("SetTimer()", "2");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);

      if (!IsType(par, TIM_ID)) {
        TypeErr(1, "SetTimer()", "timer id");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);

      if (!(IsType(par, TIM_ID) || IsType(par, NUMBER))) {
        TypeErr(2, "SetTimer()", "value");
        return(ERROR);
      }
      break;

    case STARTTIMER: ;      /* starttimer(timer) */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "starttimer()", MAX_WORD_LEN);
    case STOPTIMER:  ;      /* stoptimer(timer)  */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "stoptimer()", MAX_WORD_LEN);
      if (nr_of_pars != 1) {
        NrErr(fun_name, "1");
        return(ERROR);
      }
      par = NextPar(fun_string, &index);

      if (!IsType(par, TIM_ID)) {
        TypeErr(1, fun_name, "timer id");
        return(ERROR);
      }
      break;

    case AGREE:            /* agree()    */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "agree()", MAX_WORD_LEN);
    case DISAGREE:         /* disagree() */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "disagree()", MAX_WORD_LEN);
    case NOMATCH:          /* nomatch()    */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "nomatch()", MAX_WORD_LEN);
    case QUIT:             /* quit()       */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "quit()", MAX_WORD_LEN);
    case SAVE:			   /* save()       */ /* dec 21 07 */
	  if (fun_name[0] == '\0')
		strncpy(fun_name, "save()", MAX_WORD_LEN);
    case RESTORE:		   /* restore()    */ /* dec 21 07 */
	  if (fun_name[0] == '\0')
		strncpy(fun_name, "restore()", MAX_WORD_LEN);
    case TESTMODE:		   /* testmode()   */
	  if (fun_name[0] == '\0')
		strncpy(fun_name, "testmode()", MAX_WORD_LEN);
    case TRANSCRIPT:	   /* transcript() */
	  if (fun_name[0] == '\0')
		strncpy(fun_name, "transcript()", MAX_WORD_LEN);
    case YES_NO:           /* yesno()      */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "yesno()", MAX_WORD_LEN);
    case RUNVERB:           /* yesno()      */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "runverb()", MAX_WORD_LEN);
    case RUNCOMMON:           /* yesno()      */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "runcommon()", MAX_WORD_LEN);
    case HITANYKEY:        /* hitanykey()  */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "hitanykey()", MAX_WORD_LEN);
      if (fun_name[0] == '\0')
    case CLEARSTATUS:      /* clearstatus() */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "clearstatus()", MAX_WORD_LEN);
    case CLEARSCREEN:      /* clearscreen() */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "clearscreen()", MAX_WORD_LEN);
    case CLEARJSON:      /* clearjson() */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "clearjson()", MAX_WORD_LEN);
    case SENDJSON:      /* sendjson() */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "sendjson()", MAX_WORD_LEN);
    case NOTIMERS:      /* notimers() */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "notimers()", MAX_WORD_LEN);
    case RESTART:      /* restart() */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "restart()", MAX_WORD_LEN);
      if (nr_of_pars != 0) {
        NrErr(fun_name, "0");
        return(ERROR);
      }
      break;

    case WAIT:  /* wait(number) */
	  if (fun_name[0] == '\0')
		strncpy(fun_name, "wait()", MAX_WORD_LEN);
    case DEBUG:  /* debug(number) */
	  if (fun_name[0] == '\0')
		strncpy(fun_name, "debug()", MAX_WORD_LEN);
    case SCORE: /* score(number) */
	  if (fun_name[0] == '\0')
		strncpy(fun_name, "score()", MAX_WORD_LEN);

      if (nr_of_pars != 1) {
        NrErr(fun_name, "1");
        return(ERROR);
      }
      par = NextPar(fun_string, &index);

      if (!IsType(par, NUMBER)) {
        TypeErr(1, "wait()", "value");
        return(ERROR);
      }
      break;

    case INDENT:             /* indent([number]) */
      if (nr_of_pars != 0 && nr_of_pars != 1) {
        NrErr("indent()", "0 or 1");
        return(ERROR);
      }
      if (nr_of_pars == 0)
        return(OK);

      /* nr_of_pars is 1. */
      par = NextPar(fun_string, &index);

      if (!IsType(par, NUMBER)) {
        TypeErr(1, "indent()", "value");
        return(ERROR);
      }
      break;

    case GET_SUBJECT:   /* getsubject([word]) */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "getsubject()", MAX_WORD_LEN);
    case GET_SPECIFIER:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "getspec()", MAX_WORD_LEN);

      if (nr_of_pars != 0 && nr_of_pars != 1) {
        NrErr(fun_name, "0 or 1");
        return(ERROR);
      }
      if (nr_of_pars == 1) {
        par = NextPar(fun_string, &index);

        if (!IsType(par, WORD_ID)) {
          TypeErr(1, fun_name, "word");
          return(ERROR);
        }
      }
      break;

    case BACKGROUND:  /* background(string) */
      if (fun_name[0] == '\0')
        strncpy(fun_name, "background()", MAX_WORD_LEN);
    case TEXT:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "text()", MAX_WORD_LEN);
    case BOLD:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "bold()", MAX_WORD_LEN);
    case ITALIC:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "italic()", MAX_WORD_LEN);
    case UNDERLINE:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "underline()", MAX_WORD_LEN);
    case PLAYMODE:
      if (fun_name[0] == '\0')
        strncpy(fun_name, "playmode()", MAX_WORD_LEN);
      if (nr_of_pars != 1) {
        NrErr(fun_name, "1");
        return(ERROR);
      }
      par = NextPar(fun_string, &index);

      if (!IsType(par, WORD_ID)) {
        TypeErr(1, fun_name, "word");
        return(ERROR);
      }
      break;

    case PICKONE:   /* pickone(par_1, par_2, .... par_n) */
      if (nr_of_pars == 0) {
        NrErr("pickone()", "at least 1");
        return(ERROR);
      }
      /* now check the par list. */
      /* strings are not allowed */
      for (i=0; i<nr_of_pars; i++) {
        par = NextPar(fun_string, &index);

        if (IsType(par, STRING)) {
          TypeErr(i+1, "pickone()", "anything but a string");
          return(ERROR);
        }
      }
      break;

    case ADDCHOICE:   /* addchoice(string, string) */
      if (nr_of_pars != 2) {
        NrErr("addchoice()", "2");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);

      if (!IsType(par, STRING)) {
        TypeErr(1, "addchoice()", "string");
        return(ERROR);
      }

      par = NextPar(fun_string, &index);
      if (!IsType(par, STRING)) {
        TypeErr(1, "addchoice()", "string");
        return(ERROR);
      }
      break;

    default:
      /* Not an attribute id. */
      /* Normally, this branch will never be reached */
      ErrHdr();
      printf("CheckPars(): FATAL ERROR, unknown function: %d %d.\n", fun_string[-1], fun_string[0]);
      return(ERROR);
  } /* switch */

  return(OK);
}


void ActionRecToIntArray(int32_t *target, int32_t *index, actionRec action_rec)
{
  int i = 0;

  /* this function copies the contents of an */
  /* action record to an integer string      */

  /* used for compiling function try()       */

  target[(*index)++] = action_rec.actor;
  target[(*index)++] = action_rec.action1;
  target[(*index)++] = action_rec.action2;
  target[(*index)++] = action_rec.adverbs.nr_of_adverbs;

  for (i=0; i<MAX_PARSE_ADVERBS; i++) {
    target[(*index)++] = action_rec.adverbs.adverb[i];
  }

  target[(*index)++] = action_rec.q_word;
  target[(*index)++] = action_rec.direction;
  target[(*index)++] = action_rec.subject;
  target[(*index)++] = action_rec.specifier;
  target[(*index)++] = action_rec.prepositions.nr_of_prepositions;

  for (i=0; i<MAX_PARSE_PREPOS; i++) {
    target[(*index)++] = action_rec.prepositions.preposition[i];
  }

  target[(*index)++] = action_rec.value;
  target[(*index)++] = action_rec.ordinal;
  target[(*index)++] = action_rec.execute[0];
  target[(*index)++] = action_rec.execute[1];

  return;
}


int32_t ParsePar(char **word, int32_t *keyword, int32_t *target, int32_t *index, int32_t par_owner,
                 int32_t fun_par, FILE **source, fileList **file_list)
  /* index is first free position in target.                       */
  /* par_owner is location or object that owns the parameter.      */
  /* fun_par tells whether or not parse as a function par or as an */
  /* attribute par (different delimiters). (No longer necessary??) */
{
  int32_t   state = 1;
  int32_t   id_code;
  int32_t   owner_id = par_owner;
  int32_t   n;              /* var for converting numstrings to ints */
  int32_t   len;            /* len of trunc_word.                    */
  char      trunc_word[MAX_WORD_LEN+1]; /* Include \0 */
  int32_t   voc_word_id;
  actionRec action_rec;
  int32_t   ratio;          /* Needed to store chars in int array. */
  int32_t   length;         /* Needed to store chars or action rec */
                            /*  in int array.                      */

  ratio = sizeof(int32_t)/sizeof(char);

  while (1) {
    if (*index == MAX_COND_LEN) {
      ErrHdr();
      PrintError(68, NULL, NULL);
      return (ERROR);
    }

    switch (*keyword) {
      case EOF:
        ErrHdr();
        PrintError(47, NULL, "ParsePar()");
        return (ERROR);
      case ERROR:
        return(ERROR);
      case COMMA: ;
      case RIGHT_PAR:
        switch (state) {
          case 2: ;
          case 4: ;
          case 5: ;
          case 7: ;
          case 8:
            /* Ok to exit. */
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(69, NULL, NULL);
              return(ERROR);
            }
            else
              return(OK);
        } /* switch */
        /* Test for MAX_COND_LEN has already been done. */
        if (fun_par)
          target[(*index)++] = END_OF_PAR;

        /* Right parenthesis or comma must be returned to caller. */
        return(OK);
      case LOC_ID:
        switch (state) {
          case 1:
            state = 2;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(59, NULL, *word);
              return (ERROR);
            }
            else
              return(OK);
        }
        if (!GetLocationId((*word)+2, &id_code, 0, 0))
          return(ERROR);

        /* We will put the id in the target string now, but it may */
        /* later be overwritten in case it was the owner of a      */
        /* desc/flag/trigger (see comments at the DESCR_ID case).  */

        target[(*index)++] = id_code;
        owner_id = id_code;
        break;
      case OBJ_ID:
        switch (state) {
          case 1:
            state = 2;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(60, NULL, *word);
              return (ERROR);
            }
            else
              return(OK);
        }
        if (!GetObjectId((*word)+2, &id_code, 0, 0))
          return(ERROR);

        /* We will put the id in the target string now, but it may */
        /* later be overwritten in case it was the owner of a      */
        /* desc/flag/trigger (see comments at the DESCR_ID case).  */

        target[(*index)++] = id_code;
        owner_id = id_code;
        break;

      case ATTR_ID:
        /* don't know whether it's a common or local attribute yet */
        switch (state) {
          case 1:
            state = 4;
            break;
          case 3:
            state = 7;
            break;
          case 6:
            state =8;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(54, NULL, *word);
              return (ERROR);
            }
            else
              return(OK);
        }

        if (!GetAttrId((*word)+2, 0, owner_id, &id_code, NO_TYPE, NONE, NO_ID, 0)) /* 10march2017 */
          return(ERROR);

        /* The attribute id must be preceded by its owner. For */
        /* local attributes this isn't necessary, but it makes */
        /* parameter evaluation easier for the interpreter.      */

        if ((IsCAttrId(id_code) || IsLAttrId(id_code))&& (state != 7 && state != 8)) {
          /* Owner id is missing. This means that the      */
          /* attribute owner is the function owner.      */
          target[(*index)++] = owner_id;
        }

        if (IsLAttrId(id_code)) {
          /* Check if they didn't use a local attribute with a  */
          /* special id, a relation or a function as its owner. */
          if (IsSpecId(target[(*index)-1]) ||
                         target[(*index)-1] == RIGHT_PAR || state == 8) {
            ErrHdr();
            PrintError(55, NULL, *word);
            return(ERROR);
          }
        }
        target[(*index)++] = id_code;
        owner_id = par_owner;   /* reset owner_id */
        break;
      case THIS:
        switch (state) {
          case 1:
            state = 4;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(48, NULL, "this");
              return (ERROR);
            }
            else
              return(OK);
        }
        target[(*index)++] = *keyword;
        owner_id = *keyword;
        break;
      case DOT:
        switch (state) {
          case 2:
            state =3;
            break;
          case 4:
            state = 6;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(48, NULL, "'.'");
              return(ERROR);
            }
            else
              return(OK);
        }
        break;
      case DESCR_ID:
        /* don't know whether it's a common or local description yet */
        switch (state) {
          case 1:
            state = 5;
            break;
          case 3:
            state = 7;
            break;
          case 6:
            state = 8;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(56, NULL, *word);
              return (ERROR);
            }
            else
              return(OK);
        }

        if (!GetDescrId((*word)+2, &id_code, owner_id, 0))
          return(ERROR);

        /* The description id must be preceded by its owner. For */
        /* local descriptions this isn't necessary, but it makes */
        /* parameter evaluation easier for the interpreter.      */

        if ((IsCDescrId(id_code) || IsLDescrId(id_code))&& (state != 7 && state != 8)) {
          /* Owner id is missing. This means that the      */
          /* description owner is the function owner.      */
          target[(*index)++] = owner_id;
        }

        if (IsLDescrId(id_code)) {
          /* Check if they didn't use a local descr with a      */
          /* special id, a relation or a function as its owner. */
          if (IsSpecId(target[(*index)-1]) ||
                         target[(*index)-1] == RIGHT_PAR || state == 8) {
            ErrHdr();
            PrintError(57, NULL, *word);
            return(ERROR);
          }
        }

        target[(*index)++] = id_code;
        owner_id = par_owner;   /* reset owner_id */
        break;
      case FLAG_ID:
        /* don't know whether it's a common or local flag yet */
        switch (state) {
          case 1:
            state = 5;
            break;
          case 3:
            state = 7;
            break;
          case 6:
            state = 8;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(70, NULL, *word);
              return (ERROR);
            }
            else
              return(OK);
        }
        /* value = -1 means no relevant value, as the flag isn't */
        /* defined here. It is only referred to.                 */
        if (GetFlagId((*word)+2, 0, &id_code, -1, owner_id, 0).tag == ERROR)
          return(ERROR);

        /* The flag id must be preceded by its owner. For   */
        /* local flags this isn't necessary, but it makes   */
        /* parameter evaluation easier for the interpreter. */

        if ((IsCFlagId(id_code) || IsLFlagId(id_code)) && (state != 7 && state != 8)) {
          /* Owner id is missing. This means the */
          /* flag owner is the function owner.   */

          /* WHY NOT MAKE _this_ THE OWNER? */
          /* Because we then cannot execute other object's */
          /* triggers correctly. */

          target[(*index)++] = owner_id;
        }

        if (IsLFlagId(id_code)) {
          /* Check if they didn't use a local flag with a       */
          /* special id, a relation or a function as its owner. */
          if (IsSpecId(target[(*index)-1]) ||
                         target[(*index)-1] == RIGHT_PAR || state == 8) {
            ErrHdr();
            PrintError(71, NULL, *word);
            return(ERROR);
          }
        }
        target[(*index)++] = id_code;
        owner_id = par_owner;   /* reset owner_id */
        break;
      case TRIGG_ID:
        /* don't know whether it's a common or local trigger yet */
        switch (state) {
          case 1:
            state = 5;
            break;
          case 3:
            state = 7;
            break;
          case 6:
            state = 8;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(72, NULL, *word);
              return (ERROR);
            }
            else
              return(OK);
        }
        if (GetTriggerId((*word)+2, 0, &id_code, owner_id, 0).tag == ERROR)
          return(ERROR);

        /* The trigger id must be preceded by its owner. For */
        /* local triggers this isn't necessary, but it makes */
        /* parameter evaluation easier for the interpreter.  */

        if ((IsCTriggId(id_code) || IsLTriggId(id_code)) && (state != 7 && state != 8)) {
          /* Owner id is missing. This means that     */
          /* the trigger owner is the function owner. */
          target[(*index)++] = owner_id;
        }

        /* In case of a local trigger with a preceding owner, the  */
        /* owner_id var is set to this value in the previous loop. */

        if (IsLTriggId(id_code)) {
          /* Check if they didn't use a local trigger with        */
          /* a special id, a relation or a function as its owner. */
          if (IsSpecId(target[(*index)-1]) ||
                         target[(*index)-1] == RIGHT_PAR || state == 8) {
            ErrHdr();
            PrintError(73, NULL, *word);
            return(ERROR);
          }
        }

        target[(*index)++] = id_code;
        owner_id = par_owner;   /* reset owner_id */
        break;
      case TIM_ID:
        switch (state) {
          case 1:
            state = 5;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(58, NULL, *word);
              return (ERROR);
            }
            else
              return(OK);
        }
        if (!GetTimerId((*word)+2, &id_code, 0))
          return(ERROR);

        target[(*index)++] = id_code;
        break;
      case ACTION:    /* %action */
        switch (state) {
          case 1:
            state = 5;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(48, NULL, "%action");
              return (ERROR);
            }
            else
              return(OK);
        }
        target[(*index)++] = ACTION;
        break;
      case PREPOS:   /* %prepos */
        switch (state) {
          case 1:
            state = 5;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(48, NULL, "%prepos");
              return (ERROR);
            }
            else
              return(OK);
        }
        target[(*index)++] = PREPOS;
        break;
      case DIR:  /* for %dir */
        switch (state) {
          case 1:
            state = 5;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(48, NULL, "%dir");
              return (ERROR);
            }
            else
              return(OK);
        }
        target[(*index)++] = DIR;
        break;
      case NONE:     /* %none */
        switch (state) {
          case 1:
            state = 5;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(48, NULL, "%none");
              return (ERROR);
            }
            else
              return(OK);
        }
        target[(*index)++] = *keyword;
        owner_id = *keyword;
        break;
      case ORDINAL: /* %ord */
      case VALUE:   /* %value */
        /* value parameter as a function argument.      */
        /* Value keyword is also used for timer values. */
        switch (state) {
          case 1:
            state = 5;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(48, NULL, "%value");
              return (ERROR);
            }
            else
              return(OK);
        }
        target[(*index)++] = *keyword;  /* VALUE or ORDINAL */
        break;
      case STRING:
        switch (state) {
          case 1: ;
            state = 5;
            break;
          default:
            if (fun_par) {
              ErrHdr();
              PrintError(49, NULL, *word);
              return (ERROR);
            }
            else
              return(OK);
        }
        /* store the string in fun_string           */
        /* correct for differences in size of types */
        length = strlen(*word)+1;   /* include the '\0' char */

        /* check if we have enough room for the string */
        if (length/ratio+(*index)+1 > MAX_COND_LEN) {
          ErrHdr();
          PrintError(68, NULL, "String too long.");
          return(ERROR);
        }

        /* Tell interpreter that a string is coming up. */
        target[(*index)++] = STRING;

        /* Store number of occupied INTS */
        target[(*index)++] = length/ratio + length%ratio;

        /* Store the string itself. */
        strcpy((char *) (target+(*index)), *word);
        (*index) += length/ratio + length%ratio;
        break;
      case ACTION_REC:
        /* actually will be a string, but must be */
        /* converted to an action record.         */
        switch (state) {
          case 1: ;
            state = 5;
            break;
          default:
            if (fun_par) {
              /* error msg will be about a string */
              /* not an action record             */
              ErrHdr();
              PrintError(49, NULL, *word);
              return (ERROR);
            }
            else
              return(OK);
        }
        /* check if we have enough room for an action record */
        /* we can NOT use sizeof() here, because it will not */
        /* return the net size due to padding/aligning       */

        length = 11 + MAX_PARSE_PREPOS+1 + MAX_PARSE_ADVERBS+1;
        if (length > MAX_COND_LEN) {
          ErrHdr();
          PrintError(68, NULL, "Action record too long.");
          return(ERROR);
        }

        /* parse the string to an action record */
        InitActionRec(&action_rec);
        if (!ParseSentence(*word, &action_rec)) {
          /* error msg was printed by ParseSentence() */
          return(ERROR);
        }

        /* Tell interpreter that an action record is coming up. */
        target[(*index)++] = ACTION_REC;

        /* Store the action record in target */
        /* no need to store the length, because the interpreter will know */
        ActionRecToIntArray(target, index, action_rec);

        break;
      default: /* for switch keyword) statement */
        /* It may be a number. */
        if (StringToNum(*word, &n)) {
          switch (state) {
            case 1:
              state = 5;
              break;
            default:
              if (fun_par) {
                ErrHdr();
                PrintError(61, NULL, *word);
                return(ERROR);
              }
              else
                return(OK);
          }
          /* Values are preceded by a NUMBER keyword, in order to */
          /* be able to tell a value from an id.                  */
          target[(*index)++] = NUMBER;
          target[(*index)++] = n;
        } /* if */
        else if (IsIntAct(*keyword)) {
          /* We have a function call within a function call here. */
          switch (state) {
            case 1:
              state = 4;
              break;
            default:
              if (fun_par) {
                ErrHdr();
                PrintError(74, NULL, NULL);
                return(ERROR);
              }
              else
                return(OK);
          } /* switch */

          /* Ok, its a function call in a function par list. */
          /* par_owner remains the same.                     */

          if (!ParseFun(target, index, *keyword, par_owner, source, file_list))
            return(ERROR);
          owner_id = FUNCTION;  /* Fool GetxxxId() */
        } /* if */
        else {
          /* If MAX_ID_LEN < MAX_WORD_LEN, LookUpId() may not   */
          /* recognize *word. Copy the word; but no more than   */
          /* MAX_WORD_LEN chars (we cannot insert a \0, because */
          /* we may need *word in the next else-clause.         */

          len = ((len=strlen(*word))<MAX_WORD_LEN? len: MAX_WORD_LEN);
          strncpy(trunc_word, *word, len);

          /* add a '\0' char */
          trunc_word[len] = '\0';

          if ((voc_word_id = LookUpId(trunc_word)) != NO_ID) {
            /* It may be a word from the vocabulary. */
            switch (state) {
              case 1:
                state = 5;
                break;
              default:
                if (fun_par) {
                  ErrHdr();
                  PrintError(51, NULL, *word);
                  return(ERROR);
                }
                else
                  return(OK);
            } /* switch */

            target[(*index)++] = voc_word_id;
          }

          /* Make sure the following else part always comes last.         */
          /* All tests failed, so it's either an unexpected word          */
          /* (in case we expected a function parameter) or the next word. */
          else {
            if (fun_par) {
              /* It's a function par, so this is an error. */
              ErrHdr();
              PrintError(51, NULL, *word);
              return(ERROR);
            }
            else
              /* This is the next word. */
              return(OK);
          }
        } /* else */
        break;
    } /* switch */
    /* Read the next word. */
    free(*word);
    *word = GetNextWord(keyword, owner_id, 0, source, file_list);
  } /* while */

}


int32_t ParseFun(int32_t *target, int32_t *index, int32_t fun_code, int32_t fun_owner, FILE **source, fileList **file_list)
 /* index is first free position in target. */
 /* fun_code is the function id */
 /* fun_owner is the location or object that owns the function call */
{
  int32_t start      = *index;
  int32_t state      = 1;
  char    *word;
  int32_t keyword;
  int32_t cont       = 1;
  int32_t nr_of_pars = 0; /* Number of parameters the function has.  */

  /* In case a function is called from the default verb code, the    */
  /* function will not have an owner. In this case fun_owner will    */
  /* have the value NO_ID.  Therefore, in all function calls made    */
  /* from verbs, the flag/trigger/description/attribute owners MUST  */
  /* be specified. The GetxxxId() function tests for missing owners. */

  if (*index == MAX_COND_LEN) {
    ErrHdr();
    PrintError(68, NULL, NULL);
    return (ERROR);
  }

  target[*index] = fun_code;

  /* Leave space for nr_of_pars (filled out later). */
  *index += 2;

  /* Read the next word (no free() here). */
  word = GetNextWord(&keyword, fun_owner, 0, source, file_list);

  while (cont) {
    if (*index == MAX_COND_LEN) {
      ErrHdr();
      PrintError(68, NULL, NULL);
      return (ERROR);
    }

    switch (keyword) {
      case EOF:
        ErrHdr();
        PrintError(47, NULL, "ParseFun()");
        return (ERROR);
      case ERROR:
        return(ERROR);
      case LEFT_PAR:
        switch (state) {
          case 1:
            state = 2;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, "'('");
            return (ERROR);
        }
        if (!(ParseLeftBanana(target, index)))
          return (ERROR);
        free(word);
        word = GetNextWord(&keyword, fun_owner, 0, source, file_list);
        break;
      case RIGHT_PAR:
        switch (state) {
          case 2: ; /* same as 3 */
          case 3:
            /* ready */
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, "')'");
            return(ERROR);
        }
        if (!(ParseRightBanana(target, index)))
          return (ERROR);
        cont = 0;  /* ready */
        break;
      case COMMA:
        switch (state) {
          case 3:
            state = 4;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, "','");
            return (ERROR);
        }
        /* no action needed, just skip the comma */
        free(word);
        word = GetNextWord(&keyword, fun_owner, 0, source, file_list);
        break;
      default:
        switch (state) {
          case 2: ;
          case 4:
            state = 3;
            break;
          default:
            ErrHdr();
            PrintError(51, NULL, word);
            return(ERROR);
        }
        /* It should be a parameter.              */
        /* 1 means parse as a function parameter. */

        /* 24jan2018: ok, watch out now. If the function is try() it will */
        /* have a string parameter, but this string must be converted to  */
        /* an action record in ParsePar(). We therefore set the type in   */
        /* the *keyword parameter to ACTION_REC, so ParsePar() will know  */
        /* that it must convert the string to an action record.           */
        if (fun_code == TRY && keyword == STRING) {
          keyword = ACTION_REC;
        }

        if (!ParsePar(&word, &keyword, target, index, fun_owner, 1, source, file_list))
          return(ERROR);

        /* One more parameter.                                 */
        /* A parameter can be more than one id; for example:   */
        /* obj.cdescr is parsed as obj cdescr, but counts as 1 */
        /* parameter. */
        nr_of_pars++;

        /* Next word is returned by ParsePar(). */
        break;
    } /* switch */
  } /* while */

  /* Ok, now we know the number of parameters. */
  target[start+1] = nr_of_pars;

  /* function syntax is correct                             */
  /* now typecheck the function's parameter list            */
  /* (*index)-start denotes len of parsed function          */
  /* (*index) is at next free position in target string     */
  /* the function starts at position start in target string */

  if (!CheckPars(target+start))
    return(ERROR);

  return (OK);
}
