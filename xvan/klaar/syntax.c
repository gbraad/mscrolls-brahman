
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
#include <stdlib.h>  /* malloc(). realloc(), free() */
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "keyword.h"
#include "typedefs.h"
#include "syntax.h"

/*************************/
/* Function declarations */
/*************************/

char     *xv_strlwr(char*);
int32_t  ScanWordTable(char*, wordTable*, int32_t, int32_t);
int32_t  LookUpId(char*);
int32_t  NextWordId(char**, int32_t*, int32_t*, int32_t*);  /* @!@ */
void     InitParsedInput(parsedInput*);
int32_t  ParseInput(char*, parsedInput*, int32_t);
int32_t  CheckSyntax(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, parsedInput*);

/* Following functions are used exclusively by module trnslate.c */
resultStruct MakeSysDescr(char*, char**, int32_t, int32_t, int32_t*, int32_t, int32_t, sysDescr*);
int32_t      ParseDSys(char*, extendedSysDescr*);

/************************/
/* Function definitions */
/************************/

char *xv_strlwr(char* str)
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower(*p);
      p++;
  }
  return str;
}


int32_t ScanWordTable(char *word, wordTable *wt_rec, int32_t lower, int32_t upper)
 /* word is word to look for in word_table */
 /* wt_rec is struct to copy info into       */
{
  int32_t  value;
  int32_t  i;
  int32_t  j;
  int32_t  result;
  char look_for[MAX_WORD_LEN+1];  /* +1 for '\0' */  /* @!@ */

  /* check for NULL word must be done by caller */

  /* convert to lowercase, but keep the original word       */
  /* word is max MAX_WORD_LEN chars, no need to check again */
  strncpy(look_for, word, MAX_WORD_LEN);
  look_for[MAX_WORD_LEN] = '\0';
  xv_strlwr(look_for);

  /* Check for numberstring. */
  if (StringToNum(look_for, &value)) {
    wt_rec->id = value;
    wt_rec->types[0] = NUMBER;
    wt_rec->types[1] = NO_TYPE;
    return(value);
  }

  /* Not a number, it should be a word from the vocabulary. */
  if (upper<lower)
    /* not found */
    return(NO_ID);

  i = (lower+upper)/2;
  if ( (result = strcmp(look_for, word_table[i].word)) == 0) {  /* @!@ */
    /* found it */
    strncpy(wt_rec->word, word_table[i].word, MAX_WORD_LEN);
    strncpy(wt_rec->print_word, word_table[i].print_word, MAX_WORD_LEN);
    wt_rec->id = word_table[i].id;
    wt_rec->single_id = word_table[i].single_id;
    for (j=0; j<MAX_TYPES; j++)
      wt_rec->types[j] = word_table[i].types[j];

    return(word_table[i].id);
  }

  if (result<0)
    upper = i-1;
  else
    lower = i+1;

  return(ScanWordTable(word, wt_rec, lower, upper));
}


int32_t LookUpId(char *word)
{
  /* This function is used to quickly find a word's id, without     */
  /* bothering about types etc. All it does is call ScanWordTable() */
  /* with a dummy wt_rec parameter.                                 */

  wordTable dummy_wt_rec;

  return(ScanWordTable(word, &dummy_wt_rec, 0, nr_of_words-1));
}


int32_t NextWordId(char **line_buf, int32_t *nr_of_types, int32_t *types, int32_t *single_id)  /* @!@ */
{
  char      *start = *line_buf;   /* remember start of line_buf     */
  int32_t   i = 0;                /* counter to go through line_buf */
  char      word[MAX_WORD_LEN+1]; /* include a NULL char            */
  short     plural = 0;           /* to remember plurality          */
  int32_t   len;                  /* word length                    */
  int32_t   result = NO_ID;
  wordTable wt_rec;               /* returned by ScanWordTable      */

  /* 01oct2019: we introduced plurality. Words with regular plural  */  /* @!@ */
  /* (e, es in english) are not defined as such, only as their      */
  /* single form.                                                   */
  /* Words with irregular plurality are defined as single and       */
  /* plural.                                                        */
  /* Samples of what this function may return:                      */
  /* regular plural:                                                */
  /*     word to look for: jewels (not found, we do have jewel)     */
  /*     function returns: id from jewel                            */
  /*     types[0]: PLURAL                                           */
  /*     nr_of_types: 1                                             */
  /*     single id: id from jewel.                                  */
  /* irregular plural:                                              */
  /*     word to look for: oxen (found, with types PLURAL and ADJ)  */
  /*     function returns: id from oxen                             */
  /*     types[0]: PLURAL, types[1]: ADJECTIVES                     */
  /*     nr_of_types: 2                                             */
  /*     single id: id from ox.                                     */

  /* Test for comma. Comma needs special treatment since it is not  */
  /* recognized as a word by ScanWordTable().                       */
  if (**line_buf == ',') {
    result = COMMA; /* Not a word id. May cause error when printed. */
    *single_id   = COMMA;  /* @!@ */
    types[0]     = COMMA;
    types[1]     = NO_TYPE;
    *nr_of_types = 1;
    (*line_buf)++;
    return(result);
  }

  /* First, skip leading spaces */
  while (start[i] == SPACE)
    i++;
  *line_buf = start+i;
  start = *line_buf;
  i = 0;

  /* check for empty string */
  if (strlen(*line_buf) == 0)
    return(EOS);

  while (!(start[i] == '\0' || start[i] == ',' || start[i] == SPACE))
    i++;

  /* copy the word; but no more than MAX_WORD_LEN chars */
  len = (i<MAX_WORD_LEN? i: MAX_WORD_LEN);
  strncpy(word, start, len);

  /* add a '\0' char */
  word[len] = '\0';

  /* 0, nr_of_words-1 denote first and last element of word_table */
  result = ScanWordTable(word, &wt_rec, 0, nr_of_words-1);

  if (result == NO_ID) {   /* @!@ */
    /* not found, check if it's a plural from a word we know */
    if ( (result = CheckPlural(word)) == NO_ID) {
      /* not a plural */
      /* This msg must be printed here, because in main(), */
      /* we don't have word anymore.                       */

      /* I don't know the word */
      PrintError(58, NULL, word);
    }
    else {
      /* it's a plural from a word we know */
      *single_id = result;
      plural     = 1;
    }
  }
  else {  /* @!@ */
    /* we know the word, it may be an irregular plural */
    *single_id = wt_rec.single_id;
  }

  /* Set line_buf to remainder of string.                */
  /* We do this always; also in case of an unknown word. */

  switch (start[i]) {
    case '\0':
      (*line_buf) += i;    /* end of line */
      break;
    case SPACE:
      (*line_buf) += i+1;
      break;
    case ',':
      (*line_buf) += i;
      break;
    default:
      /* the impossible just happened */
      PrintError(13, NULL, "NextWordId()");
      return(NO_ID);
  } /* switch */

  /* In case of an unknown word, exit here */
  if (result == NO_ID)
    return(NO_ID);

  if (plural) {    /* @!@ */
    /* plural can only be of type NOUNS  */
    /* to tell it from a single noun, we */
    /* introduced the type PLURAL        */
    types[0] = PLURAL;
    for (i=1; i<MAX_TYPES; i++) {
      types[i] = NO_TYPE;
    }
    *nr_of_types = 1;
  }
  else {
    /* copy types array and set nr_of_types */
    i = 0;
    for (i=0; i<MAX_TYPES; i++) {
      types[i] = wt_rec.types[i];
    }

    /* find nr_of_types */
    i = 0;
    while (i<MAX_TYPES && types[i] != NO_TYPE) {
      i++;
    }
    *nr_of_types = i;
  }
  return(result);  /* result is the word id */
}


void InitParsedInput(parsedInput *parsed_input)
{
  int i = 0;

  (parsed_input->actor).dynamic                     = NULL;
  InitSysDescr(&((parsed_input->actor).part1));
  (parsed_input->actor).connect_prepos              = NO_ID;
  InitSysDescr(&((parsed_input->actor).part2));

  parsed_input->action1                             = NO_ID;
  parsed_input->action2                             = NO_ID;
  parsed_input->scope                               = CURR_LOC_ONLY;
  parsed_input->adverbs.nr_of_adverbs               = 0;
  parsed_input->q_word                              = NO_ID;
  parsed_input->direction                           = NO_ID;

  for (i=0; i<MAX_SUBJECTS; i++) {
    (parsed_input->subject[i]).dynamic                = NULL;
    parsed_input->single[i]                        = NO_ID;    /* @!@ */
    InitSysDescr(&((parsed_input->subject[i]).part1));
    (parsed_input->subject[i]).connect_prepos         = NO_ID;
    InitSysDescr(&((parsed_input->subject[i]).part2));
  }

  (parsed_input->specifier).dynamic                 = NULL;
  InitSysDescr(&((parsed_input->specifier).part1));
  (parsed_input->specifier).connect_prepos          = NO_ID;
  InitSysDescr(&((parsed_input->specifier).part2));

  parsed_input->prepositions.nr_of_prepositions     = 0;
  parsed_input->value                               = 0;
  parsed_input->ordinal                             = 0;
}


int32_t ParseInput(char *line_buf, parsedInput *parsed_input, int32_t syntax)
{
  int32_t state       = 1;
  int32_t nr_of_types = 0;
  int32_t i;
  int32_t types[MAX_TYPES];

  /* Initialize types array */
  for (i=0; i<MAX_TYPES; i++)
    types[i] = NO_TYPE;

  /* NO_ID is word_id (means parse the next word) */
  /* -1 is type_index  (any value will do here)   */
  /* 0 is subject_index                           */

  /* Set initial state for this syntax. */
  switch (syntax) {
    case LINE:
      state = 1;
      break;
    case SUBJECT:
      state = 2;
      break;
    case SPECIFIER:
      state = 8;     /* Used to be 8, 6 sept. 1995 */
      break;         /* aug 24 2015, set back to 8 */
    case ANSWER:
      PrintError(61, NULL, NULL);
      return(ERROR);
    default:
      /* We should never get here. */
      PrintError(13, NULL, "ParseInput()");
      return(ERROR);
  }

  return(CheckSyntax(line_buf, NO_ID, nr_of_types,
                     types, -1, 0, state, parsed_input));
}


int32_t CheckSyntax(char *line_buf, int32_t id, int32_t nr_of_types, int32_t *types, int32_t type_index,
                    int32_t subject_index, int32_t state, parsedInput *parsed_input)
{
  /* select the right function, depending on the story language */

  switch(story_info.story_language) {
    case ENG:
      return(ENG_CheckSyntax(line_buf, id, nr_of_types, types, type_index,
                                  subject_index, state, parsed_input));
      break;
    case NL:
      return(NL_CheckSyntax(line_buf, id, nr_of_types, types, type_index,
                                  subject_index, state, parsed_input));
      break;
    default:
      /* we should never get here, use English */
      return(ENG_CheckSyntax(line_buf, id, nr_of_types, types, type_index,
                                  subject_index, state, parsed_input));
  }
}


resultStruct MakeSysDescr(char *line_buf, char **rest_of_line_buf, int32_t id, int32_t nr_of_types,
                          int32_t *types, int32_t type_index, int32_t state, sysDescr *descr)
{
  /* descr must be set to default values by caller. */

  int32_t      i      = 0;
  int32_t dummy_single_id;   /* @!@ */
  resultStruct result = {OK, NONE, OK};
  int32_t old_state   = state;    /* Remember state for retry in case of  */
                                  /* a type clash.                        */
  int32_t new_types[MAX_TYPES];   /* Needed in case the next word will    */
                                  /* be read in a recursive call, If call */
                                  /* returns ERROR, we must still have    */
                                  /* the types array for a retry.         */

  /* No check for empty line_buf here. We may be in the next  */
  /* typecheck for the last word. Check for empty line_buf is */
  /* done by NextWordId().                                    */

  /* If this function is called with id = NO_ID, we have to   */
  /* read the next word from the line_buf.                    */

  /* If, after a NextWordId() call the id equals NO_ID, we    */
  /* have an error (unknown word).                            */

  /* Initialize new_types */
  for (i=0; i<MAX_TYPES; i++)
    new_types[i] = NO_TYPE;

  if (id == NO_ID) {
    /* Get the next word from the user input               */
    /* NextWordId() also returns the remainder of line_buf */

    if ( (id = NextWordId(&line_buf, &nr_of_types, types)) == NO_ID) {
      /* unknown word; caller must stop recursive calls */
      result.tag = UNKNOWN_WORD;
      return(result);
    }
    type_index = 0;
  }

  if (id == EOS)      /* End Of String */
    switch (state) {
      case 2: ;
      case 3: ;
      case 6: ;
      case 7:
        result.tag = OK;
        return(result);
      default:
        result.tag = ERROR;
        return(result);
    }

  /* In case a word has MAX_TYPE types, we will not encounter */
  /* a NO_TYPE value in its types array. Therefore check here */
  /* for MAX_TYPES exceeding.                                 */

  if (type_index == MAX_TYPES) {
    result.tag = ERROR;
    return(result);
  }

  /* check type(s) of word */
  switch (types[type_index]) {
    case NO_TYPE:
      /* no more types, this definitely is a wrong syntax */
      result.tag = ERROR;
      return(result);
    case ARTICLES:
      switch (state) {
        case 1:
           state = 4;
          break;
        case 5:
          state = 8;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                              types, ++type_index, state, descr));
      } /* switch */

      /* OK, assume it's an article */
      result = MakeSysDescr(line_buf, rest_of_line_buf, NO_ID,nr_of_types,
                            new_types,-1,state,descr);
      if (result.tag == OK || result.tag == PREPOSITIONS) {
        /* articles in user input must be ignored ?? */
        /* 2019May21 turned on again for dynamic d_sys */
        descr->article = id;  /* @!@ */
        return(result);
      }
      else if (result.tag == UNKNOWN_WORD) {
        return(result);
      }
      else /* syntax error */
        /* Oops! not an article after all; try again with next */
        /* type and original state.                            */
        return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                            types, ++type_index, old_state, descr));
    case ADJECTIVES:
      switch (state) {
        case 1: ;
        case 2: ;
        case 4:
          state = 2;
          break;
        case 5: ;
        case 6: ;
        case 8:
          state = 6;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                              types, ++type_index, state, descr));
      } /* switch */

      /* OK, assume it's an adjective */
      result = MakeSysDescr(line_buf, rest_of_line_buf, NO_ID, nr_of_types,
                            new_types, -1, state, descr);
      if (result.tag == OK || result.tag == PREPOSITIONS) {
        /* Yes, it's an adjective.                            */
        /* Check for double adjectives. Double adjectives may */
        /* occur when the user is asked for more information. */
        i = 0;
        while (i<descr->nr_of_adjectives && (descr->adjectives)[i] != id)
          i++;

        if (i == descr->nr_of_adjectives) {
          /* Ok, not a double adjective. */
          /* Put adjective in descr struct */
          /* If MAX_PARSE_ADJ has been exceeded, we will */
          /* just skip this adjective.                   */
          if (descr->nr_of_adjectives < MAX_PARSE_ADJ)
            descr->adjectives[(descr->nr_of_adjectives)++] = id;
          else {
            /* too many adjectives, but will try */
            PrintError(59, NULL, NULL);
          }
        }
        else {
          /* Double adjective, do nothing. */
          PrintError(60, NULL, NULL);
        }
        result.tag = OK;  /* just make sure */
        return(result);
      }
      else if (result.tag == UNKNOWN_WORD) {
        return(result);
      }

      else /* syntax error */
        /* Oops! not an adjective after all; try again with next */
        /* type and original state.                              */
        return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                            types, ++type_index, old_state, descr));
    case NOUNS:
      switch (state) {
        case 1: ;
        case 2: ;
        case 4:
          state = 3;
          break;
        case 5: ;
        case 6: ;
        case 8:
          state = 7;
          break;
        default:
          /* This means state equals 3, which means error. */
          /* Error will occur when we run out of types for */
          /* this word.                                    */
          /* wrong syntax; try again with next type.       */

          return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                              types, ++type_index, state, descr));
      } /* switch */

      result = MakeSysDescr(line_buf, rest_of_line_buf, NO_ID,nr_of_types,
                            new_types, -1, state, descr);

      if (result.tag == OK || result.tag == PREPOSITIONS) {
        /* Yes, it's a noun. */
        descr->noun = id;
        return(result);
      }
      else if (result.tag == UNKNOWN_WORD) {
        return(result);
      }

      else /* syntax error */
        /* Oops! not a noun after all; try again with next type */
        /* and original state.  */

        return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                            types, ++type_index, old_state, descr));
    case CONNECT_PREPOSITIONS:
      switch (state) {
        case 3:
          /* this is a system description that consists of 2   */
          /* parts, like 'man with sword'. We have encountered */
          /* the preposition that connects the 2 parts.        */

          /* Set rest_of_line_buf to current line_buf, so the  */
          /* caller knows where to start parsing the second    */
          /* part of the system description. We cannot use     */
          /* line_buf for this, because making it a char** wil */
          /* screw up the recursive calls.                     */

          /* No need to set state to 5 as we will exit         */
          /* unconditionally. Calling function ParseDsys()     */
          /* will set state to 5 with the next call for part 2 */
          /* of the extendedSysDescr struct.                   */

          /* Return the preposition's word id to caller.       */
          *rest_of_line_buf = line_buf;
          result.tag   = PREPOSITIONS;
          result.value = id;
          return(result);
          break;
        default:
          /* wrong syntax; try again with next type */
          return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                              types, ++type_index, state, descr));
      } /* switch CONNECT_PREPOSITIONS */
    default:
      /* wrong syntax; try again with next type */
      return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                          types, ++type_index, state, descr));
  } /* switch */

  /* no return needed here */
}


int32_t ParseDSys(char *description, extendedSysDescr *descr)
{
  int32_t      nr_of_types    = 0;
  int32_t      types[MAX_TYPES];
  char         **descr_text_2 = NULL;;  /* for parsing descr.part2 */
  resultStruct result;

  types[0] = NO_TYPE;  /* indicates unused array */

  result.tag   = OK;
  result.value = OK;

  /* Malloc() space for descr_text_2 */
  if ((descr_text_2 = (char **) malloc(sizeof(char*))) == NULL) {
    PrintError(15, NULL, "ParseDSys()");
    result.tag = ERROR;
    return(result.tag);
  }

  /* Caller must have initialized the descr struct */

  result = MakeSysDescr(description, descr_text_2, NO_ID, nr_of_types,
                        types, -1, 1, &(descr->part1));

  /* If result is not OK, UNKNOWN_WORD or ERROR, it indicates that we */
  /* have a system description with two parts (e.g. 'hall of fame'),  */
  /* connected by a preposition. In that case the value of result     */
  /* will be the word id of the preposition.                          */

  if (result.tag == PREPOSITIONS) {
    /* fill the second part of the extendedSysDescr struct.   */
    /* The previous MakeSysDescr() call has returned the part */
    /* of descr_text after the preposition in descr_text_2.   */
    /* We cannot pass &descr_text because MakeSysDescr() is   */
    /* based on a recursive call mechanism.                   */

    descr->connect_prepos = result.value;
    /* 5 in par list means state must be set to 5 upon */
    /* entrance of MakeSysDescr(). Also see STD.       */
    result = MakeSysDescr(*descr_text_2, descr_text_2, NO_ID, nr_of_types,
                          types, -1, 5, &(descr->part2));
  }

  switch (result.tag) {
    case UNKNOWN_WORD:
    case OK:
      /* do nothing */
      break;
    default:
      /* result.tag is either ERROR or again PREPOSITIONS */
      /* A system description can only have 2 parts, so   */
      /* only 1 connecting preposition.                   */
      result.tag = ERROR;
      break;
  }
  free(descr_text_2);
  return(result.tag);
}
