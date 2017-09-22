
/* Copyright 2016, 2017 Marnix van den Bos */

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


int32_t  ParseAssignment(char**, int32_t*, int32_t*, int32_t*, int32_t, FILE**);

int32_t   ParseAssignment(word, keyword, target, index, owner, source)
 char     **word;
 int32_t  *keyword;
 int32_t  *target;
 int32_t  *index;     /* First free position in target.         */
 int32_t  owner;      /* Location or object that owns the code. */
 FILE     **source;
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
        if (ChangeInputFile(word, keyword, NO_ID, 0, source)) {
          break;
        }
        ErrHdr();
        printf("ParseAssignment(): unexpected end of inputfile\n");
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
            printf("\nLine %d: unexpected attribute id: %s\n", line_num, *word);
            return(ERROR);
        }

        if (!GetAttrId((*word)+2, id_owner, &id, type, value, NO_ID, 0)) /* 10march2017 */
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
            printf("\nLine %d: illegal local attribute owner for %s.\n", line_num, *word);
            return(ERROR);
          }
        }
        nr_of_pars++;
        target[(*index)++] = id;
        target[(*index)++] = END_OF_PAR;

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
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
            printf("\nLine %d: unexpected description parameter: %s\n", line_num, *word);
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
            printf("\nLine %d: illegal local description owner for %s.\n", line_num, *word);
            return(ERROR);
          }
        }
        nr_of_pars++;
        target[(*index)++] = id;
        target[(*index)++] = END_OF_PAR;

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
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
            printf("\nLine %d: unexpected timer\n", line_num);
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
        *word = GetNextWord(keyword, owner, 0, source);
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
            printf("\nLine %d: unexpected =\n", line_num);
            return(ERROR);
        }
        /* skip the '=' */
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
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
            printf("\nLine %d: unexpected location Identifier\n", line_num);
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
        *word = GetNextWord(keyword, owner, 0, source);
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
            printf("\nLine %d: unexpected object identifier\n", line_num);
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
        *word = GetNextWord(keyword, owner, 0, source);
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
            /*if (state == 11) { */
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
            printf("\nLine %d: unexpected %%this wildcard\n", line_num);
            return(ERROR);
        }

        /* store the THIS keyword in the target string */
        id_owner = value;         /* need this when we look up the attribute id */
        target[(*index)++] = THIS;

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
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
            printf("\nLine %d: unexpected '.' keyword\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
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
            printf("\nLine %d: unexpected '+' or '-' keyword\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
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
            printf("\nLine %d: unexpected wildcard parameter: %s\n", line_num, *word);
            return(ERROR);
        } /* switch */

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
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
              /*if (state == 11) { */
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
              printf("\nLine %d: unexpected value: %d\n", line_num, value);
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
              state = 1;   /* syntax: int_act().attribute */
              break;
            case 5:
              /*state = 6;*/
              state = 23;
              break;
            case 10:
              /*state = 11;*/
              state = 21;
              break;
            case 15:
              /*state = 16;*/
              state = 22;
              break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected internal action: %s\n", line_num, *word);
              return(ERROR);
          }
          if (!ParseFun(target, index, *keyword, owner, source))
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
                /*if (state == 11) { */
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
                printf("\nLine %d: unexpected direction: %s\n", line_num, *word);
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
              /*if (state == 11) { */
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
              printf("\nLine %d: illegal word as attribute value, must be word from vocabulary: %s\n", line_num, *word);
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
              default:
                ErrHdr();
                printf("\nLine %d: unexpected word: %s\n", line_num, *word);
                return(ERROR);
            } /* switch */
          } /* else */
        } /* else */

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break; /* default */
    } /* switch */
  } /* while */
}

