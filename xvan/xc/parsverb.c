
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

synList      *NewSynListStruct(void);
void         FreeSynList(synList*);
resultStruct RedefinedVerb(char*, int32_t*);
int32_t      ParseVerb(int64_t, int32_t, FILE**, fileList**);


/************************/
/* Function definitions */
/************************/

synList *NewSynListStruct(void)
{
  synList *sl = NULL;

  if ((sl = (synList *) malloc(sizeof(synList))) == NULL) {
    PrintError(1, NULL, "syn_list");
    return(ERROR);
  }

  /* initialize sl */
  sl->id  = NO_ID;
  sl->next   = NULL;

  return(sl);
}


void FreeSynList(synList *sl)
{
  if ((sl != NULL)) {
    FreeSynList(sl->next);
    free(sl);
  }
  return;
}


resultStruct RedefinedVerb(char *verb, int32_t *verb_id)
{
  wordTable    wt_rec;
  resultStruct result;

  /* return whether the verb is a redefined verb */
  /* verb id is returned in par verb_id          */

  *verb_id = ScanWordTable(verb, &wt_rec, 0, nr_of_words-1);

  if (*verb_id == NO_ID) {
    result.tag = ERROR;
    result.value = 0;
  }
  else {
    result.tag = VALUE;
    result.value = wt_rec.redefine;
  }
  return(result);
}


int32_t ParseVerb(int64_t offset, int32_t redefined, FILE **source, fileList **file_list)
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

  char         *word      = NULL;
  int32_t      keyword    = NO_KEYWORD;
  int32_t      verb_id    = NO_ID;
  int32_t      state      = 1;         /* VERB keyword has already been */
                                    /* read in calling function.     */
  int32_t      fun_buf[MAX_COND_LEN];  /* used to parse a function      */
  int32_t      index      = 0;         /* used to index fun_buf         */
  int32_t      disambig   = 0;
  actionRec    action_rec;             /* Needed for parsing sentences. */
  synList      *syn_list  = NULL;      /* for keeping track of synonyms */
  synList      *runner    = NULL;
  int32_t      verb_scope = CURR_LOC_ONLY;
  resultStruct result;

  /* Read the verb name. */
  word = GetNextWord(&keyword, NO_ID, 0, source, file_list);

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
            PrintError(48, NULL, TranslateKeyword("SYNONYM"));
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
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
            PrintError(48, NULL, TranslateKeyword("PROLOGUE"));
            return(ERROR);
        }

        /* Initialize the verb action_rec. */
        InitVerbActionRec(&action_rec);

        /* Tell interpreter this is the prologue action_rec. */
        /* PROLOGUE is not a verb. Therefore this action_rec */
        /* cannot be printed by PrintActionRec().            */

        action_rec.action1 = PROLOGUE;

        /* Store the action record. */
        if (!StoreActionRec(&action_rec))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
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
            PrintError(48, NULL, TranslateKeyword("EPILOGUE"));
            return(ERROR);
        }

        /* Initialize the action_rec. */
        InitVerbActionRec(&action_rec);

        /* Tell interpreter this is the epilogue action_rec. */
        /* EPILOGUE is not a verb. Therefore this action_rec */
        /* cannot be printed by PrintActionRec().            */

        action_rec.action1 = EPILOGUE;

        /* Store the action record. */
        if (!StoreActionRec(&action_rec))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
        break;
      case STRING:
        switch (state) {
          case  2: ;
          case  4: ;
          case  6: ;
          case  7: ;
          case  9: ;
          case 14: ;
          case 15: ;
          case 17: ;
          case 18: ;
          case 27:
            state = 5;
            break;
          default:
            ErrHdr();
            PrintError(49, NULL, word);
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
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
        break;
      case COMMA:
        switch (state) {
          case 5:
            state = 27;
            break;
          default:
            ErrHdr();
            PrintError(141, NULL, NULL);
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
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
            PrintError(48, NULL, TranslateKeyword("DISAMBIGUATION_RULES"));
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
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
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
            PrintError(48, NULL, TranslateKeyword("END_RULES"));
            return(ERROR);
        }

        /* clear disambiguation mode for ParseIfThenElse() function */
        disambig = 0;

        /* tell interpreter that the parser rules end here */
        if (!StoreKeyword(END_RULES))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
        break;
      case SCOPE:
        switch (state) {
          case 2: ;
          case 4:
            state = 8;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("SCOPE"));
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
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
            PrintError(50, &((resultStruct){VALUE,keyword}), NULL);
            return(ERROR);
        }

        /* Put the scope with the verb info in the verb_dir. */
        /*verb_dir[verb_id-FIRST_VERB_ID].scope = keyword;*/
        verb_scope = keyword;
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
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
            PrintError(48, NULL, TranslateKeyword("DEFAULT"));
            return(ERROR);
        }

        /* Initialize the action_rec. */
        InitVerbActionRec(&action_rec);

        /* Tell interpreter this is the default action_rec. */
        /* DEFAULT is not a verb. Therefore this action_rec */
        /* cannot be printed by PrintActionRec().           */
        action_rec.action1 = DEFAULT;
        action_rec.action2 = DEFAULT;

        /* Store the action record. */
        if (!StoreActionRec(&action_rec))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
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
            PrintError(48, NULL, TranslateKeyword("IF"));
            return(ERROR);
        }
        /* 06oct2017 we must tell ParseIfThenElse() that we are parsing */
        /* a verb, because we may not use functions runverb() and       */
        /* runcommon() from within a verb                               */
        /* 5th par 1 means we are parsing a verb.                       */
        /* 6th par 0 means we are not parsing a common trigger          */
        if (!ParseIfThenElse(&word, &keyword, NO_ID, disambig, 1, 0, source, file_list))
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
            PrintError(48, NULL, TranslateKeyword("END_IF"));
            return(ERROR);
        }
        if (!StoreKeyword(ENDIF))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
        break;
      case ENDVERB:
        switch (state) {
          case 2: ;
          case 4: ;
          case 9:
            /* No verb code was defined.         */
            /* Don't store an ENDVERB keyword.   */
            /* Update the verb directory and set */
            /* offset to -1 so the interpreter   */
            /* knows there is no verb code       */
            runner = syn_list;
            while (runner != NULL) {
              verb_dir[runner->id - FIRST_VERB_ID].scope  = verb_scope;
              verb_dir[runner->id - FIRST_VERB_ID].offset = -1;
              runner = runner->next;
            }
            FreeSynList(syn_list);
            /*verb_dir[verb_id-FIRST_VERB_ID].offset = -1;*/
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

            /* update the verb directory */
            runner = syn_list;
            while (runner != NULL) {
              verb_dir[runner->id - FIRST_VERB_ID].scope  = verb_scope;
              verb_dir[runner->id - FIRST_VERB_ID].offset = offset;
              runner = runner->next;
            }
            FreeSynList(syn_list);
            break;

          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("END_VERB"));
            return(ERROR);
        }
        return(OK);

      /* 14march2017: for assignments that are outside an ITE loop. */
      /* We forgot these because is is rarely used in verb default  */
      /* code. NOTE: no branch for ATTR_ID because it cannot occur  */
      /* by itself in verb default code (no owner).                 */
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
            PrintError(51, NULL, word);
            return(ERROR);
            break;
        }

        if (!ParseAssignment(&word, &keyword, fun_buf, &index, NO_ID, source, file_list))
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
            /* Look up the verb id and check if it is a    */
            /* redefinition, but first truncate word if    */
            /* necessary.                                  */
            /* This is not very nice programming, but word */
            /* will be free()ed soon, anyway.              */

            if (strlen(word) > MAX_WORD_LEN)
              word[MAX_WORD_LEN] = '\0';

            result = RedefinedVerb(word, &verb_id);

            if (result.tag == ERROR) {
              ErrHdr();
              PrintError(52, NULL, word);
              return(ERROR);
            }

            if (result.value == redefined) {
              /* initialize the syn_list */
              if ((syn_list = NewSynListStruct()) == NULL) {
                return(ERROR);
              }
              runner = syn_list;
              runner->id = verb_id;
            }
            break;
          case 3:
            /* this is a synonym */
            /* 03jan2018: because of the new override functionality  */
            /* we must retrieve the synonym's verb id because it may */
            /* be different from the main word in case the main word */
            /* was redefined and got e new word id.                  */
            state = 4;
            /* Look up the verb id and check if it is a    */
            /* redefinition, but first truncate word if    */
            /* necessary.                                  */

            if (strlen(word) > MAX_WORD_LEN)
              word[MAX_WORD_LEN] = '\0';

            result = RedefinedVerb(word, &verb_id);

            if (result.tag == ERROR) {
              ErrHdr();
              PrintError(52, NULL, word);
              return(ERROR);
            }

            if (result.value == redefined) {
              /* add synonym to syn_list */
              /* in the unlikely case that redefine = 0 and the */
              /* main verb was skipped because of a redefine,   */
              /* syn_list will not have been initialized        */

              /* Example: $REDEFINE_VERB verb2 SYNONYM verb1 .. */
              /*          $VERB verb1, SYNONYM verb3 ..         */
              /* will not have syn_list initialized when        */
              /* compiling verb3                                */

              if (syn_list == NULL) {
                if ((syn_list = NewSynListStruct()) == NULL) {
                  return(ERROR);
                }
                runner = syn_list;
              }
              else {
                if ( (runner->next = NewSynListStruct()) == NULL) {
                  return(ERROR);
                }
                runner = runner->next;
              }

              /* save the verb id */
              runner->id = verb_id;
            }
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
              PrintError(51, NULL, word);
              return(ERROR);
            }

            /* Parse the function. */
            if (!ParseFun(fun_buf, &index, keyword, NO_ID, source, file_list))
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
            PrintError(51, NULL, word);
            if (state == 25 || state == 26) {
              PrintError(53, NULL, NULL);
              return(ERROR);
            }
        }  /* switch */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, source, file_list);
        break;
    }  /* switch */
  } /* while */
}
