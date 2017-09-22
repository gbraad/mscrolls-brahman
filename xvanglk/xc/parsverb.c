
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>   /* free()   */
#include <string.h>   /* strlen() */
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "prsefun.h"  /* MAX_COND_LEN */
#include "parsverb.h"


/*************************/
/* Function declarations */
/*************************/

int32_t ParseVerb(int64_t, FILE**);


/************************/
/* Function definitions */
/************************/

int32_t ParseVerb(offset, source)
 int64_t offset;  /* current position in datafile */
 FILE **source;   /* file to read from            */
{
  /* With other parse functions (like ParseLocation()), the      */
  /* directory is not passed as a parameter. This is because in  */
  /* these cases, the directory is created at a later time (in   */
  /* StoreXxxDir(). However, in this case, this isn't possible.  */
  /* Since we need to search in the verb list, we need an array  */
  /* rather than a linked list. Thus, the word_list has been     */
  /* transformed into a word_table array. Therefore there's no   */
  /* need to store offsets in the word_list, since then we would */
  /* have to search it again to create the verb directory.       */

  char      *word   = NULL;
  int32_t   keyword = NO_KEYWORD;
  int32_t   verb_id = NO_ID;
  int32_t   state   = 1;           /* VERB keyword has already been */
                                   /* read in calling function.     */
  int32_t   fun_buf[MAX_COND_LEN]; /* used to parse a function      */
  int32_t   index    = 0;          /* used to index fun_buf         */
  int32_t   disambig = 0;
  actionRec action_rec;            /* Needed for parsing sentences. */

  /* Read the verb name. */
  word = GetNextWord(&keyword, NO_ID, 0, source);

  while (1) {
    switch (keyword) {
      /* no unexpected EOFs here; has been checked in ParseDictionary()   */
      case ERROR:
        return(ERROR);
      case SYNONYM:
        /* All synonyms have equal ids, so need to store the */
        /* offset only once. Offset is stored in state 2.    */
        switch (state) {
          case 2: ;
          case 4:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected SYNONYM keyword.\n", line_num);
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source);
        /* Syntax has been checked in Pass1(). 'word' will be   */
        /* overwritten before re-entering the while-loop.       */
        break;
      case PROLOGUE:
        switch (state) {
          case 2: ;
          case 4: ;
          case 9:
            state = 13;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected PROLOGUE keyword.\n", line_num);
            return(ERROR);
        }

        /* Initialize the verb action_rec. */
        InitVerbActionRec(&action_rec);

        /* Tell interpreter this is the prologue action_rec. */
        /* PROLOGUE is not a verb. Therefore this action_rec */
        /* cannot be printed by PrintActionRec().            */

        action_rec.action = PROLOGUE;

        /* Store the action record. */
        if (!StoreActionRec(&action_rec))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source);
        break;
      case EPILOGUE:
        switch (state) {
          case 2: ;
          case 4: ;
          case 9: ;
          case 14: ;
          case 15:
            state = 16;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected EPILOGUE keyword.\n", line_num);
            return(ERROR);
        }

        /* Initialize the action_rec. */
        InitVerbActionRec(&action_rec);

        /* Tell interpreter this is the epilogue action_rec. */
        /* EPILOGUE is not a verb. Therefore this action_rec */
        /* cannot be printed by PrintActionRec().            */

        action_rec.action = EPILOGUE;

        /* Store the action record. */
        if (!StoreActionRec(&action_rec))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source);
        break;
      case STRING:
        switch (state) {
          case  2: ;
          case  4: ;
          case  5: ;
          case  6: ;
          case  7: ;
          case  9: ;
          case 14: ;
          case 15: ;
          case 17: ;
          case 18:
            state = 5;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected string.\n", line_num);
            return(ERROR);
        }

        /* Initialize the action_rec. */
        InitVerbActionRec(&action_rec);

        /* Parse the sentence and create an action record */
        if (!ParseSentence(word, &action_rec))
          return(ERROR);

        /* Store the action record. */
        if (!StoreActionRec(&action_rec))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source);
        break;
      case DISAMBIG_RULES:
        switch (state) {
          case 5:
            state = 19;
            break;
          case 10:
            state = 22;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected DISAMBIGUATION_RULES keyword.\n", line_num);
            return(ERROR);
        }

        /* set disambiguation mode for ParseIfThenElse() function */
        disambig = 1;

        /* tell interpreter that the parser rules are coming  */
        /* we do not create a new action record here, because */
        /* disambiguation code and default code belong to the */
        /* same string.                                       */
        if (!StoreKeyword(DISAMBIG_RULES))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source);
        break;
      case END_RULES:
        switch (state) {
          case 20:
            state = 21;
            break;
          case 23:
            state = 24;
            break;
          case 25:
            state = 21;
            break;
          case 26:
            state = 24;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected END_RULES keyword.\n", line_num);
            return(ERROR);
        }

        /* clear disambiguation mode for ParseIfThenElse() function */
        disambig = 0;

        /* tell interpreter that the parser rules end here */
        if (!StoreKeyword(END_RULES))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source);
        break;
      case SCOPE:
        switch (state) {
          case 2: ;
          case 4:
            state = 8;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected SCOPE keyword.\n", line_num);
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source);
        break;
      case ACTOR_ONLY: ;
      case CURR_LOC_ONLY:
      case ALL_LOCS:
        switch (state) {
          case 8:
            state = 9;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected scope.\n", line_num);
            return(ERROR);
        }
        /* Put the scope with the verb info in the verb_dir. */
        verb_dir[verb_id-FIRST_VERB_ID].scope = keyword;

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source);
        break;
      case DEFAULT:
        switch (state) {
          case  2: ;
          case  4: ;
          case  6: ;
          case  7: ;
          case  9: ;
          case 14: ;
          case 15: ;
          case 17: ;
          case 18:
            state = 10;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected DEFAULT keyword.\n", line_num);
            return(ERROR);
        }

        /* Initialize the action_rec. */
        InitVerbActionRec(&action_rec);

        /* Tell interpreter this is the default action_rec. */
        /* DEFAULT is not a verb. Therefore this action_rec */
        /* cannot be printed by PrintActionRec().           */
        action_rec.action = DEFAULT;

        /* Store the action record. */
        if (!StoreActionRec(&action_rec))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source);
        break;
      case IF:
        switch (state) {
          case 5: ;
          case 6: ;
          case 21:      /* 27dec2016: for disambiguation */
            state = 7;
            break;
          case 10: ;
          case 11: ;
          case 24:      /* 27dec2016: for disambiguation */
            state = 12;
            break;
          case 13: ;
          case 14:
            state = 15;
            break;
          case 16: ;
          case 17:
            state = 18;
            break;
          case 19: ;    /* 27dec2016: states 19 - 24 were */
          case 25:      /* added for disambiguation rules */
            state = 20;
            break;
          case 22: ;
          case 26:
            state = 23;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected IF keyword.\n", line_num);
            return(ERROR);
        }
        if (!ParseIfThenElse(&word, &keyword, NO_ID, disambig, source))
          return(ERROR);
        break;
      case ENDIF:
        switch (state) {
          case 7:
            state = 6;
            break;
          case 12:
            state = 11;
            break;
          case 15:
            state = 14;
            break;
          case 18:
            state = 17;
            break;
          case 20:
            state = 25;
            break;
          case 23:
            state = 26;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected ENDIF keyword.\n", line_num);
            return(ERROR);
        }
        if (!StoreKeyword(ENDIF))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source);
        break;
      case ENDVERB:
        switch (state) {
          case 2: ;
          case 4: ;
          case 9:
            /* No verb code was defined.            */
            /* Don't store an ENDVERB keyword. */
            verb_dir[verb_id-FIRST_VERB_ID].offset = -1;
            break;
          case  6: ;
          case  7: ;
          case 11: ;
          case 12: ;
          case 14: ;
          case 15: ;
          case 17: ;
          case 18:
            /* Yes, there is verb code.                  */
            /* Tell interpreter we are at the end of the */
            /* verb code.                                */
            if (!StoreKeyword(ENDVERB))
              return(ERROR);

            /* Store offset in the verb directory. */
            verb_dir[verb_id-FIRST_VERB_ID].offset = offset;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected ENDVERB keyword.\n", line_num);
            return(ERROR);
        }
        return(OK);

      /* 14march2017: for assignments that are outside an ITE loop. */
      /* We forgot these because is is rarely used in verb default  */
      /* code. NOTE: no branch for ATTR_ID because it cannot occur  */
      /* by itself in verb default code (no owner.                  */
      case LOC_ID:
      case OBJ_ID:
      case OWNER:
      case TIM_ID:
        switch (state) {
          case 5: ;
          case 6:
            state = 6;
            break;
          case 10: ;
          case 11:
            state = 11;
            break;
          case 13: ;
          case 14:
            state = 14;
            break;
          case 16: ;
          case 17:
            state = 17;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: Unexpected word: %s (assignment error?).\n",
                   line_num, word);
            return(ERROR);
            break;
        }

        if (!ParseAssignment(&word, &keyword, fun_buf, &index, NO_ID, source))
          return(ERROR);

        if (!StoreFun(fun_buf, index))
          return(ERROR);
        index = 0;
        /* next word and keyword have already been read by ParseAssignment() */
      break; /* LOC_ID */


      default:
        /* It may be either an internal action, an attribute or a */
        /* verb name. Or a verb name that also is an internal     */
        /* action or an attribute name. The last possibility we   */
        /* can tell by the state var.                             */

        switch (state) {
          case 1:
            /* verb */
            state = 2;
            /* Look up the verb id in verb_list, but first */
            /* truncate word if necessary.                 */
            /* This is not very nice programming, but word */
            /* will be free()ed soon, anyway.              */

            if (strlen(word) > MAX_WORD_LEN)
              word[MAX_WORD_LEN] = '\0';

            if ((verb_id = LookUpId(word)) == NO_ID) {
              ErrHdr();
              printf("ParseVerb(): verb %s not found.\n", word);
              return(ERROR);
            }

            /* Set scope to the default value. */
            /* It may be overwritten later.    */
            verb_dir[verb_id-FIRST_VERB_ID].scope = CURR_LOC_ONLY;

            break;
          case 3:
            /* this is a synonym */
            /* Synonym verbs don't have their own entry in the  */
            /* verb table. They use the entry of their synonym. */
            state = 4;
            break;
          case  5: ;
          case  6: ;
          case 10: ;
          case 11: ;
          case 13: ;
          case 14: ;
          case 16: ;
          case 17: ;
          case 21: ;   /* 27dec2016: added for disambiguation */
          case 24: ;   /* 27dec2016: added for disambiguation */
            if (state == 5 || state == 6 || state == 21)
              state = 6;
            else if (state == 10 || state == 11 || state == 24)
              state = 11;
            else if (state == 13 || state == 14)
              state = 14;
            else
              state = 17;

            /* It should be an internal action */
            if (!(IsIntAct(keyword))) {
              ErrHdr();
              printf("\nLine %d: illegal word: %s\n", line_num, word);
              return(ERROR);
            }

            /* Parse the function. */
            if (!ParseFun(fun_buf, &index, keyword, NO_ID, source))
              return(ERROR);

            /* store the function */
            if (!StoreFun(fun_buf, index))
              return(ERROR);

            index = 0;  /* reset index for the next function */
            break;
          default:
            /* The verb default code has NOT been syntax */
            /* checked by Pass1().                       */
            ErrHdr();
            printf("\nLine %d: unexpected word: %s\n", line_num, word);
            if (state == 25 || state == 26)
              printf("IF or END_RULES keyword was expected.\n");
            return(ERROR);
            return(ERROR);
            return(ERROR);
        }  /* switch */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source);
        break;
    }  /* switch */
  } /* while */
}
