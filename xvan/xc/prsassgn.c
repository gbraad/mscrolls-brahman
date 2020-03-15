
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
/*                                                                      */
/* <marnix@xvan.nl>                                                     */
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
#include "prsassgn.h"


/*************************/
/* function declarations */
/*************************/

int32_t  ParseAssignment(char**, int32_t*, int32_t*, int32_t*, int32_t, FILE**, fileList**);

int32_t   ParseAssignment(char **word, int32_t *keyword, int32_t *target, int32_t *index, int32_t owner, FILE **source, fileList **file_list)
 /* index is first free position in target.         */
 /* owner is location or object that owns the code. */
{
  /* Function ParseAssignment() added on Sep 7th 2015. */

  int32_t  state      = 0;
  int32_t  id_owner   = NO_ID;         /* may differ from the owner par for a local attribute */
  int32_t  id;
  int32_t  fun_pos    = *index;
  int32_t  nr_of_pars = 0;
  int32_t  type       = NONE;
  int32_t  value      = NONE;
  int32_t  function   = NO_KEYWORD;    /* to distinct + en - in shared code      */
  int32_t  int_act    = SETATTRIBUTE;  /* default is setattribute() but may also be settimer() */

  target[(*index)+2] = LEFT_PAR;

  (*index) += 3; /* skip place for the function code, nr_of _pars and '(' */

  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source, file_list)) {
          break;
        }
        ErrHdr();
        PrintError(47, NULL, NULL);
        return(ERROR);

      case ERROR:
        return(ERROR);

      case ATTR_ID:
        switch (state) {
          case 11: target[fun_pos] = int_act;
          case 13: target[fun_pos] = int_act;
          case 14: target[fun_pos] = int_act;
          case 8:  /* these states are all valid states to exit */
          case 9:  /* must check these first because states may */
          case 18: /* be changed                                */
          case 19:
          case 20:
          case 21:
          case 22:
          case 23:
             target[fun_pos] = int_act;
            /*if (state == 11) { */
            if (state == 11 || state == 21 || state == 22 || state == 23) {
              /* we were expecting a DOT but must exit instead   */
              /* Must add EOP and increase number of pars by one */
              /* Instead of <loc>.<attribute> we only got <loc>  */
              nr_of_pars++;
              target[(*index)++] = END_OF_PAR;
            }

            /* store number of parameters in target and add ')' */
            target[fun_pos+1]  = nr_of_pars;
            target[(*index)++] = RIGHT_PAR;

            /* we have now built the function string */
            /* check the parameter syntaxes          */
            if (!CheckPars(target))
              return(ERROR);

            return(OK);  /* ok to exit, next word and keyword are for calling function */
            break;
          case 0:
            /* attribute owner is passed with function call */
            id_owner = owner;
            target[(*index)++] = id_owner;
            state = 3;
            break;
          case 2:
            state = 3;
            break;
          case 5:
            /* attribute owner is passed with function call */
            id_owner = owner;
            target[(*index)++] = id_owner;
            state = 8;
            break;
          case 7:
            state = 8;
            break;
          case 10:
            /* attribute owner is passed with function call */
            id_owner = owner;
            target[(*index)++] = id_owner;
            state = 13;
            break;
          case 12:
            state = 13;
            break;
          case 15:
            /* attribute owner is passed with function call */
            id_owner = owner;
            target[(*index)++] = id_owner;
            state = 18;
            break;
          case 17:
            state = 18;
            break;
          default:
            ErrHdr();
            PrintError(54, NULL, *word);
            return(ERROR);
        }

        if (!GetAttrId((*word)+2, 0, id_owner, &id, type, value, NO_ID, 0)) /* 10march2017 */
          /* 0 means attribute is not defined here, only referenced */
          /* type and value are dummies here.                       */
          return(ERROR);

        if (IsLAttrId(id)) {
          /* Check if they didn't use a local attribute with a */
          /* special id or a function as its owner.            */
          /* Check for RIGHT_PAR will become relevant whenever */
          /* we allow function calls in assignments .          */
          if (IsSpecId(target[(*index)-1]) ||
                         target[(*index)-1] == RIGHT_PAR) {
            ErrHdr();
            PrintError(55, NULL, *word);
            return(ERROR);
          }
        }
        nr_of_pars++;
        target[(*index)++] = id;
        target[(*index)++] = END_OF_PAR;

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source, file_list);
        break;

      case DESCR_ID:
        switch (state) {
          case 11: target[fun_pos] = int_act;
          case 13: target[fun_pos] = int_act;
          case 14: target[fun_pos] = int_act;
          case 8:  /* these states are all valid states to exit */
          case 9:  /* must check these first because states may */
          case 18: /* be changed                                */
          case 19:
          case 20:
          case 21:
          case 22:
          case 23:
             target[fun_pos] = int_act;
            /*if (state == 11) { */
            if (state == 11 || state == 21 || state == 22 || state == 23) {
              /* we were expecting a DOT but must exit instead   */
              /* Must add EOP and increase number of pars by one */
              /* Instead of <loc>.<attribute> we only got <loc>  */
              nr_of_pars++;
              target[(*index)++] = END_OF_PAR;
            }

            /* store number of pars in target and add ')' */
            target[fun_pos+1]  = nr_of_pars;
            target[(*index)++] = RIGHT_PAR;

            /* we have now built the function string */
            /* check the parameter syntaxes          */
            if (!CheckPars(target))
              return(ERROR);

            return(OK);  /* ok to exit, next word and keyword are for calling function */
            break;
          case 10:
            /* description owner is passed with function call */
            id_owner           = owner;
            target[(*index)++] = id_owner;
            target[fun_pos]    = int_act;
            state = 20;
            break;
          case 12:
            /* id owner has already been stored */
            target[fun_pos] = int_act;
            state = 20;
            break;
          default:
            ErrHdr();
            PrintError(56, NULL, *word);
            return(ERROR);
        } /* switch */

        if (!GetDescrId((*word)+2, &id, id_owner, 0))
          /* 0 means description is not defined here, only referenced */
          return(ERROR);

        if (IsLDescrId(id)) {
          /* Check if they didn't use a local description with */
          /* a special id or a function as its owner.          */
          /* Check for RIGHT_PAR will become relevant whenever */
          /* we allow function calls in assignments .          */
          if (IsSpecId(target[(*index)-1]) ||
                         target[(*index)-1] == RIGHT_PAR) {
            ErrHdr();
            PrintError(57, NULL, *word);
            return(ERROR);
          }
        }
        nr_of_pars++;
        target[(*index)++] = id;
        target[(*index)++] = END_OF_PAR;

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source, file_list);
        break; /* DESCR_ID */

      case TIM_ID:
        switch(state) {
          case 11: target[fun_pos] = int_act;
          case 13: target[fun_pos] = int_act;
          case 14: target[fun_pos] = int_act;
          case 8:  /* these states are all valid states to exit */
          case 9:  /* must check these first because states may */
          case 18: /* be changed                                */
          case 19:
          case 20:
          case 21:
          case 22:
          case 23:
            target[fun_pos] = int_act;
            if (state == 11 || state == 21 || state == 22 || state == 23) {
              /* we were expecting a DOT but must exit instead   */
              /* Must add EOP and increase number of pars by one */
              /* Instead of <loc>.<attribute> we only got <loc>  */
              nr_of_pars++;
              target[(*index)++] = END_OF_PAR;
            }

            /* store number of attributes in target and add ')' */
            target[fun_pos+1]  = nr_of_pars;
            target[(*index)++] = RIGHT_PAR;

            /* we have now built the function string */
            /* check the parameter syntaxes          */
            if (!CheckPars(target))
              return(ERROR);

            return(OK);  /* ok to exit, next word and keyword are for calling function */
            break;
          case 0:
            /* It's a timer assignment */
            state = 3;
            int_act = SETTIMER;
            target[fun_pos] = SETTIMER;
            break;
          case 5:
            state = 8;
            break;
          case 10:
            state = 13;
            break;
          case 15:
            state = 18;
            break;
          default:
            ErrHdr();
            PrintError(58, NULL, *word);
            return(ERROR);
        }
        /* get the timer id */
        if (!GetTimerId((*word)+2, &value, 0))
          /* 0 means timer is not defined here, only referenced */
          return(ERROR);

        /* store the id in the target string */
        target[(*index)++] = value;
        target[(*index)++] = END_OF_PAR;
        nr_of_pars++;

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source, file_list);
        break;

      case EQUAL_SIGN:
        switch (state) {
          case 11: target[fun_pos] = int_act;
          case 13: target[fun_pos] = int_act;
          case 14: target[fun_pos] = int_act;
          case 8:  /* these states are all valid states to exit */
          case 9:  /* must check these first because states may */
          case 18: /* be changed                                */
          case 19:
          case 20:
          case 21:
          case 22:
          case 23:
            target[fun_pos] = int_act;
            if (state == 11 || state == 21 || state == 22 || state == 23) {
              /* we were expecting a DOT but must exit instead   */
              /* Must add EOP and increase number of pars by one */
              /* Instead of <loc>.<attribute> we only got <loc>  */
              nr_of_pars++;
              target[(*index)++] = END_OF_PAR;
            }

            /* store number of attributes in target and add ')' */
            target[fun_pos+1]  = nr_of_pars;
            target[(*index)++] = RIGHT_PAR;

            /* we have now built the function string */
            /* check the parameter syntaxes          */
            if (!CheckPars(target))
              return(ERROR);

            return(OK);  /* ok to exit, next word and keyword are for calling function */
            break;
          case 3:
            state = 10;
            break;
          case 4:
            /* The function is '+=' or '-=' or '*=' or '/'.             */
            /* '+=', '-=', *= and /=  are stored the same way as '+',   */
            /* '-', '-', '*'and '/' so the interpreter needs only 1     */
            /* function. The second parameter is equal to the first     */
            /* one: par1 += par2 is the same as par1 = par1 + par2.     */
            /* To indicate to the interpreter that par1 and par2 are    */
            /* the same, we fill in NONE for par2.                      */
            /* We cannot copy par1, because in case the assignment was  */
            /* was precedes by an owner(xx). construction, we do not    */
            /* have the owner(xx) code as this was parsed by the        */
            /* ParseTrigger() function.                                 */

            target[(*index)++] = NONE;
            target[(*index)++] = END_OF_PAR;
            nr_of_pars++;
            state = 5;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, "=");
            return(ERROR);
        }
        /* skip the '=' */
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source, file_list);
        break;

      case LOC_ID:
      /* this is either the owner for the attribute to come, */
      /* or the right part of an assigment (attr = loc_id)   */
      /* We can tell from the state which one.               */
        switch (state) {
          case 11: target[fun_pos] = int_act;
          case 13: target[fun_pos] = int_act;
          case 14: target[fun_pos] = int_act;
          case 8:  /* these states are all valid states to exit */
          case 9:  /* must check these first because states may */
          case 18: /* be changed                                */
          case 19:
          case 20:
          case 21:
          case 22:
          case 23:
            target[fun_pos] = int_act;
            if (state == 11 || state == 21 || state == 22 || state == 23) {
              /* we were expecting a DOT but must exit instead   */
              /* Must add EOP and increase number of pars by one */
              /* Instead of <loc>.<attribute> we only got <loc>  */
              nr_of_pars++;
              target[(*index)++] = END_OF_PAR;
            }
            /* store number of attributes in target and add ')' */
            target[fun_pos+1]  = nr_of_pars;
            target[(*index)++] = RIGHT_PAR;

            /* we have now built the function string */
            /* check the parameter syntaxes          */
            if (!CheckPars(target))
              return(ERROR);

            return(OK);  /* ok to exit, next word and keyword are for calling function */
            break;
          case 0:
            state = 1;
            break;
          case 5:
            state = 6;
            break;
          case 10:
            state = 11;
            break;
          case 15:
            state = 16;
            break;
          default:
            ErrHdr();
            PrintError(59, NULL, *word);
            return(ERROR);
        }
          /* Now get the location identifier */
        if (!GetLocationId((*word)+2, &value, 0, 0))
          /* 0 means location is not defined here, only referenced */
          return(ERROR);

          /* store the id in the target string */
        id_owner = value;         /* need this when we look up the attribute id */
        target[(*index)++] = value;

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source, file_list);
        break;

      case OBJ_ID:
        switch (state) {
          case 11: target[fun_pos] = int_act;
          case 13: target[fun_pos] = int_act;
          case 14: target[fun_pos] = int_act;
          case 8:  /* these states are all valid states to exit */
          case 9:  /* must check these first because states may */
          case 18: /* be changed                                */
          case 19:
          case 20:
          case 21:
          case 22:
          case 23:
            target[fun_pos] = int_act;
            /*if (state == 11) { */
            if (state == 11 || state == 21 || state == 22 || state == 23) {
              /* we were expecting a DOT but must exit instead   */
              /* Must add EOP and increase number of pars by one */
              /* Instead of <loc>.<attribute> we only got <loc>  */
              nr_of_pars++;
              target[(*index)++] = END_OF_PAR;
            }
            /* store number of attributes in target and add ')' */
            target[fun_pos+1]  = nr_of_pars;
            target[(*index)++] = RIGHT_PAR;

            /* we have now built the function string */
            /* check the parameter syntaxes          */
            if (!CheckPars(target))
              return(ERROR);

            return(OK);  /* ok to exit, next word and keyword are for calling function */
            break;
          case 0:
            state = 1;
            break;
          case 5:
            state = 6;
            break;
          case 10:
            state = 11;
            break;
          case 15:
            state = 16;
            break;
          default:
            ErrHdr();
            PrintError(60, NULL, *word);
            return(ERROR);
        }
         /* get the object identifier */
         if (!GetObjectId((*word)+2, &value, 0, 0))
           /* 0 means object is not defined here, only referenced */
           return(ERROR);

        /* store the id in the target string */
        id_owner = value;         /* need this when we look up the attribute id */
        target[(*index)++] = value;

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source, file_list);
        break;

      case THIS:
        switch (state) {
          case 11: target[fun_pos] = int_act;
          case 13: target[fun_pos] = int_act;
          case 14: target[fun_pos] = int_act;
          case 8:  /* these states are all valid states to exit  */
          case 9:  /* must check these first because states may  */
          case 18: /* be changed                                 */
          case 19:
          case 20:
          case 21:
          case 22:
          case 23:
            target[fun_pos] = int_act;
            if (state == 11 || state == 21 || state == 22 || state == 23) {
              /* we were expecting a DOT but must exit instead   */
              /* Must add EOP and increase number of pars by one */
              /* Instead of <loc>.<attribute> we only got <loc>  */
              nr_of_pars++;
              target[(*index)++] = END_OF_PAR;
            }
            /* store number of attributes in target and add ')'  */
            target[fun_pos+1]  = nr_of_pars;
            target[(*index)++] = RIGHT_PAR;

            /* we have now built the function string */
            /* check the parameter syntaxes          */
            if (!CheckPars(target))
              return(ERROR);

            return(OK);  /* ok to exit, next word and keyword are for calling function */
            break;
          case 0:
            state = 1;
            break;
          case 10:
            state = 11;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, "%this");
            return(ERROR);
        }

        /* store the THIS keyword in the target string */
        id_owner = value;         /* need this when we look up the attribute id */
        target[(*index)++] = THIS;

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source, file_list);
        break;

      case DOT:
       switch (state) {
          case 13: target[fun_pos] = int_act;
          case 14: target[fun_pos] = int_act;
          case 8:  /* these states are all valid states to exit */
          case 9:  /* must check these first because states may */
          case 18: /* be changed                                */
          case 19:
          case 20:
            target[fun_pos] = int_act;
            /* store number of attributes in target and add ')' */
            target[fun_pos+1]  = nr_of_pars;
            target[(*index)++] = RIGHT_PAR;

            /* we have now built the function string */
            /* check the parameter syntaxes          */
            if (!CheckPars(target))
              return(ERROR);

            return(OK);  /* ok to exit, next word and keyword are for calling function */
            break;
          case 1:
            state = 2;
            break;
          case 6:
            state = 7;
            break;
          case 11:
            state = 12;
            break;
          case 16:
            state = 17;
            break;
          case 21:
            state = 12;
            break;
          case 22:
            state = 17;
            break;
          case 23:
            state = 7;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, "'.'");
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source, file_list);
        break;

      case PLUS:
        if (function == NO_KEYWORD)
          function = ADD;
      case MINUS:
        if (function == NO_KEYWORD)
          function = SUB;
      case ASTERIX:
        if (function == NO_KEYWORD)
          function = ASTERIX;
      case QUOT:
        if (function == NO_KEYWORD)
          function = QUOT;
      case REM:
        if (function == NO_KEYWORD)
          function = REM;
        switch (state) {
          case 11: target[fun_pos] = int_act;
          case 8:  /* these states are all valid states to exit */
          case 9:  /* must check these first because states may */
          case 18: /* be changed                                */
          case 19:
          case 20:
            target[fun_pos] = int_act;
            if (state == 11) {
              /* we were expecting a DOT but must exit instead   */
              /* Must add EOP and increase number of pars by one */
              /* Instead of <loc>.<attribute> we only got <loc>  */
              nr_of_pars++;
              target[(*index)++] = END_OF_PAR;
            }

            /* store number of attributes in target and add ')' */
            target[fun_pos+1]  = nr_of_pars;
            target[(*index)++] = RIGHT_PAR;

            /* we have now built the function string */
            /* check the parameter syntaxes          */
            if (!CheckPars(target))
              return(ERROR);

            return(OK);  /* ok to exit, next word and keyword are for calling function */
            break;
          case 3:
            /* Function is '+=' or '-=', but we store it as function */
            /* '+' (add) or , '-'(sub) so the interpreter only needs */
            /* one function.                                         */

            /* target[fun_pos] = function; */
            int_act = function;
            state = 4;
            break;
          case 13:
          case 14:
            /* function is '+' or '-' or '*' or '/' */
            /* target[fun_pos] = function; */
            int_act = function;
            state = 15;
            break;
          case 21:
            /* '+' after an internal action. It might have also been */
            /* a DOT so we haven't increased the nr_of_pars yet.     */
            int_act = function;
            nr_of_pars++;
            target[(*index)++] = END_OF_PAR;
            state = 15;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, "'+' or '-'");
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source, file_list);
        break;
      /* check the wildcards     */
      /* THIS has its own case:  */
      case ACTION:
      case DIR:
      case PREPOS:
      case VALUE:
      case NONE:
        switch (state) {
          case 11: target[fun_pos] = int_act;
          case 13: target[fun_pos] = int_act;
          case 14: target[fun_pos] = int_act;
          case 8:  /* these states are all valid states to exit */
          case 9:  /* must check these first because states may */
          case 18: /* be changed                                */
          case 19:
          case 20:
          case 21:
          case 22:
          case 23:
            target[fun_pos] = int_act;
            if (state == 11) {
              /* we were expecting a DOT but must exit instead   */
              /* Must add EOP and increase number of pars by one */
              /* Instead of <loc>.<attribute> we only got <loc>  */
              nr_of_pars++;
              target[(*index)++] = END_OF_PAR;
            }

            /* store number of pars in target and add ')' */
            target[fun_pos+1]  = nr_of_pars;
            target[(*index)++] = RIGHT_PAR;

            /* we have now built the function string */
            /* check the parameter syntaxes          */
            if (!CheckPars(target))
              return(ERROR);

            return(OK);  /* ok to exit, next word and keyword are for calling function */
            break;
          case 10:
            state = 20;
            /* store wildcard in target string */
            target[fun_pos]    = int_act;
            target[(*index)++] = *keyword;
            target[(*index)++] = END_OF_PAR;
            nr_of_pars++;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, *word);
            return(ERROR);
        } /* switch */

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source, file_list);
        break; /* NONE */

      default:
        /* It could be a value */
        if (StringToNum(*word, &value)) {
          switch (state) {
            case 11: target[fun_pos] = int_act;
            case 13: target[fun_pos] = int_act;
            case 14: target[fun_pos] = int_act;
            case 8:  /* these states are all valid states to exit */
            case 9:  /* must check these first because states may */
            case 18: /* be changed                                */
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
              target[fun_pos] = int_act;
              if (state == 11 || state == 21 || state == 22 || state == 23) {
                /* we were expecting a DOT but must exit instead   */
                /* Must add EOP and increase number of pars by one */
                /* Instead of <loc>.(attribute> we only got <loc>  */
                nr_of_pars++;
                target[(*index)++] = END_OF_PAR;
              }
              /* store number of pars in target and add ')' */
              target[fun_pos+1]  = nr_of_pars;
              target[(*index)++] = RIGHT_PAR;

              /* we have now built the function string */
              /* check the parameter syntaxes          */
              if (!CheckPars(target))
                return(ERROR);

              return(OK);  /* ok to exit, next word and keyword are for calling function */
              break;
            case 5:
              state = 9;
              break;
            case 10:
              state = 14;
              break;
            case 15:
              state = 19;
              break;
            default:
              ErrHdr();
              PrintError(61, &((resultStruct){VALUE,value}), NULL);
              return(ERROR);
          } /* switch */
          /* store number in target string */
          target[(*index)++] = NUMBER;
          target[(*index)++] = value;
          target[(*index)++] = END_OF_PAR;
          nr_of_pars++;
        } /* if */
        else if (IsIntAct(*keyword)) {
          /* It could be an internal action */
          switch (state) {
            case 11: target[fun_pos] = int_act;
            case 13: target[fun_pos] = int_act;
            case 14: target[fun_pos] = int_act;
            case 8:  /* these states are all valid states to exit */
            case 9:  /* must check these first because states may */
            case 18: /* be changed                                */
            case 19:
            case 20:
            case 21: target[fun_pos] = int_act;
            case 22:
            case 23:
              target[fun_pos] = int_act;
              if (state == 11 || state == 21 || state == 22 || state == 23) {
                /* we were expecting a DOT but must exit instead   */
                /* Must add EOP and increase number of pars by one */
                /* Instead of <loc>.<attribute> we only got <loc>  */
                nr_of_pars++;
                target[(*index)++] = END_OF_PAR;
              }
              /* store number of attributes in target and add ')' */
              target[fun_pos+1]  = nr_of_pars;
              target[(*index)++] = RIGHT_PAR;

              /* we have now built the function string */
              /* check the parameter syntaxes          */
              if (!CheckPars(target))
                return(ERROR);

              return(OK);  /* ok to exit, next word and keyword are for calling function */
              break;
            case 0:
              state = 1;   /* syntax: int_act().attribute */
              break;
            case 5:
              state = 23;
              break;
            case 10:
              state = 21;
              break;
            case 15:
              state = 22;
              break;
            default:
              ErrHdr();
              PrintError(62, NULL, *word);
              return(ERROR);
          }
          if (!ParseFun(target, index, *keyword, owner, source, file_list))
           return(ERROR);
          id_owner = FUNCTION;
        } /* else if */
        else if (IsDirection(*word, &value)) {
          /* It could be a direction. GetNextWord() will not have returned a    */
          /* DIRECTION keyword as we don't know beforehand which directions the */
          /* story will have. We therefore have to check for a direction word.  */
            switch (state) {
              case 11: target[fun_pos] = int_act;
              case 13: target[fun_pos] = int_act;
              case 14: target[fun_pos] = int_act;
              case 8:  /* these states are all valid states to exit */
              case 9:  /* must check these first because states may */
              case 18: /* be changed                                */
              case 19:
              case 20:
              case 21:
              case 22:
              case 23:
                target[fun_pos] = int_act;
                if (state == 11 || state == 21 || state == 22 || state == 23) {
                  /* we were expecting a DOT but must exit instead   */
                  /* Must add EOP and increase number of pars by one */
                  /* Instead of <loc>.<attribute> we only got <loc>  */
                  nr_of_pars++;
                  target[(*index)++] = END_OF_PAR;
                }

              /* store number of pars in target and add ')' */ /* added nov 30 2015 */
              target[fun_pos+1]  = nr_of_pars;
              target[(*index)++] = RIGHT_PAR;

                /* we have now built the function string */
                /* check the parameter syntaxes          */
                if (!CheckPars(target))
                  return(ERROR);

                return(OK);  /* ok to exit, next word and keyword are for calling function */
                break;
              case 10:
                state = 20;
                /* store direction in target string */
                target[fun_pos]    = int_act;
                target[(*index)++] = DIRECTIONS;
                target[(*index)++] = value;
                target[(*index)++] = END_OF_PAR;
                nr_of_pars++;
                break;
              default:
                ErrHdr();
                PrintError(63, NULL, *word);
                return(ERROR);
            } /* switch */

        } /* if */
        else {
          /* It could be a word from the vocabulary                                */
          /* Make sure this is done after checking for DIRECTION (is also a word). */
          /* LookUpId() returns the word id for existing words, otherwise NO_ID    */
          /* CAREFULL: for numberstrings it returns the integer value              */
          /* Have to change this in the future, no issue here because we already   */
          /* checked for a numstring above.                                        */
          value = LookUpId(*word);
          if (value != NO_ID) {
            switch (state) {
            case 11: target[fun_pos] = int_act;
            case 13: target[fun_pos] = int_act;
            case 14: target[fun_pos] = int_act;
            case 8:  /* these states are all valid states to exit */
            case 9:  /* must check these first because states may */
            case 18: /* be changed                                */
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
              target[fun_pos] = int_act;
              if (state == 11 || state == 21 || state == 22 || state == 23) {
                /* we were expecting a DOT but must exit instead   */
                /* Must add EOP and increase number of pars by one */
                /* Instead of <loc>.<attribute> we only got <loc>  */
                nr_of_pars++;
                target[(*index)++] = END_OF_PAR;
              }

              /* store number of pars in target and add ')' */ /* added nov 30 2015 */
              target[fun_pos+1]  = nr_of_pars;
              target[(*index)++] = RIGHT_PAR;

              /* we have now built the function string */
              /* check the parameter syntaxes          */
              if (!CheckPars(target))
                return(ERROR);

              return(OK);  /* ok to exit, next word and keyword are for calling function */
              break;
            case 10:
              state = 20;
              /* store word in target string */
              target[fun_pos]    = int_act;
              target[(*index)++] = WORD_ID;
              target[(*index)++] = value;
              target[(*index)++] = END_OF_PAR;
              nr_of_pars++;
              break;
            default:
              ErrHdr();
              PrintError(64, NULL, *word);
              return(ERROR);
            } /* switch */
          } /* if */
          else {
            /* none of the above */
            switch (state) {
              case 11: target[fun_pos] = int_act;
              case 13: target[fun_pos] = int_act;
              case 14: target[fun_pos] = int_act;
              case 8:  /* these states are all valid states to exit */
              case 9:  /* must check these first because states may */
              case 18: /* be changed                                */
              case 19:
              case 20:
              case 21:
              case 22:
              case 23:
                target[fun_pos] = int_act;
                if (state == 11 || state == 21 || state == 22 || state == 23) {
                  /* we were expecting a DOT but must exit instead   */
                  /* Must add EOP and increase number of pars by one */
                  /* Instead of <loc>.<attribute> we only got <loc>  */
                  nr_of_pars++;
                  target[(*index)++] = END_OF_PAR;
                }
                /* store number of attributes in target and add ')' */
                target[fun_pos+1]  = nr_of_pars;
                target[(*index)++] = RIGHT_PAR;

                /* we have now built the function string */
                /* check the parameter syntaxes          */
                if (!CheckPars(target))
                  return(ERROR);

                return(OK);  /* ok to exit, next word and keyword are for calling function */
                break;
              default:
                ErrHdr();
                PrintError(51, NULL, *word);
                return(ERROR);
            } /* switch */
          } /* else */
        } /* else */

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source, file_list);
        break; /* default */
    } /* switch */
  } /* while */
}
