
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018 Marnix van den Bos.                   */
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "prsedsys.h"

/**************************/
/* Function  declarations */
/**************************/

int32_t      ScanWordTable(char*, wordTable*, int32_t, int32_t);
int32_t      NextWordId(char**, int32_t*, int32_t*);
resultStruct MakeSysDescr(char*, char**, int32_t, int32_t, int32_t*,
                          int32_t, int32_t, sysDescr*);
int32_t      ParseDSys(char*, extendedSysDescr*);

/************************/
/* Function definitions */
/************************/

int32_t    ScanWordTable(word, wt_rec, lower, upper)
 char      *word;    /* word to look for in word_table */
 wordTable *wt_rec;  /* struct to copy info into       */
 int32_t   lower;    /* elements of array between      */
 int32_t   upper;    /* which to search                */
{
  int32_t i;
  int32_t j;
  int32_t result;
  int32_t value;
  char    look_for[MAX_WORD_LEN+1];

  /* check for NULL word must be done by caller             */

  /* convert to lowercase, but keep the original word       */
  /* word is max MAX_WORD_LEN chars, no need to check again */
  strncpy(look_for, word, MAX_WORD_LEN);
  strlwr(look_for);

  /* Check for numberstring. */
  if (StringToNum(look_for, &value)) {
    wt_rec->id = value;
    wt_rec->types[0] = NUMBER;
    wt_rec->types[1] = NO_TYPE; /* in case caller did not initialize wt_rec */
    return(value);
  }

  if (upper<lower)
    /* not found */
    return(NO_ID);

  i = (lower+upper)/2;

  if ((result = strcmp(look_for, word_table[i].word)) == 0) {
    /* found it */

    strncpy(wt_rec->word, word_table[i].word, MAX_WORD_LEN);
    strncpy(wt_rec->print_word, word_table[i].print_word, MAX_WORD_LEN);
    wt_rec->id = word_table[i].id;
    for (j=0; j<MAX_TYPES; j++) {
      wt_rec->types[j] = word_table[i].types[j];
    }
    wt_rec->redefine = word_table[i].redefine;

    return(word_table[i].id);
  }

  if (result<0)
    upper = i-1;
  else
    lower = i+1;

  return(ScanWordTable(word, wt_rec, lower, upper));
}

int32_t NextWordId(line_buf, nr_of_types, types)
 char     **line_buf;
 int32_t  *nr_of_types;
 int32_t  *types;
{
  char      *start = *line_buf;   /* remember start of line_buf     */
  int32_t   i = 0;                /* counter to go through line_buf */
  char      word[MAX_WORD_LEN+1]; /* include a NULL char */
  int32_t   len;                  /* word length                    */
  int32_t   result = NO_ID;
  wordTable wt_rec;               /* returned by ScanWordTable      */

  /* First, skip leading CRs and spaces */
  while (start[i] == CR || start[i] == SPACE)
    i++;
  *line_buf = start+i;
  start = *line_buf;
  i = 0;

  /* check for empty string */
  if (strlen(*line_buf) == 0)
    return(EOS);

  /* Find end of next word */
  while (!(start[i] == '\0' || start[i] == CR || start[i] == SPACE))
    i++;

  /* copy the word; but no more than MAX_WORD_LEN chars */
  len = (i<MAX_WORD_LEN? i: MAX_WORD_LEN);
  strncpy(word, start, len);

  /* add a '\0' char */
  word[len] = '\0';

  /* 0, nr_of_words-1 denote first and last element of word_table */
  result = ScanWordTable(word, &wt_rec, 0, nr_of_words-1);

  if (result == NO_ID) {
    ErrHdr();
    /* I don't know the word .... */
    PrintError(79, NULL, word);
  }

  /* Set line_buf to remainder of string.                */
  /* We do this always; also in case of an unknown word. */

  (*line_buf) += i;

  /* In case of an unknown word, exit here */
  if (result == NO_ID)
    return(NO_ID);

  /* copy types array and set nr_of_types */
  i = 0;

  for (i=0; i<MAX_TYPES; i++)
    types[i] = wt_rec.types[i];

  /* find nr_of_types */
  i = 0;

  /* while clause requires conditional AND */
  while (i<MAX_TYPES && types[i] != NO_TYPE)
    i++;
  *nr_of_types = i;

  return(result);  /* result is the word id */
}

resultStruct MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                          types, type_index, state, descr)
 char      *line_buf;
 char      **rest_of_line_buf;
 int32_t   id;
 int32_t   nr_of_types;
 int32_t   *types;
 int32_t   type_index;
 int32_t   state;
 sysDescr  *descr;
{
  /* descr must be set to default values by caller */

  /* nov 18 2016, in case of an unknown word, consider  */
  /* to return ERROR instead of UNKNOWN_WORD. This will */
  /* quit the compilation, with UNKNOWN_WORD it will    */
  /* continue compiling.                                */

  int32_t      i = 0;
  resultStruct result;               /* Needed to test for unknown  words    */
  int32_t      old_state = state;    /* Remember state for retry in case of  */
                                     /* a type clash.                        */
  int32_t      new_types[MAX_TYPES]; /* Needed in case the next word will    */
                                     /* be read in a recursive call, If call */
                                     /* returns ERROR, we must still have    */
                                     /* the types array for a retry.         */

  /* No check for empty line_buf here. We may be in the next  */
  /* typecheck for the last word. Check for empty line_buf is */
  /* done by NextWordId().                                    */

  /* If this function is called with id = NO_ID, we have to */
  /* read the next word from the line_buf.                  */

  /* If, after a NextWordId() call the id equals NO_ID, we  */
  /* have an error (unknown word).                          */

  /* Initialize new_types */
  for (i=0; i<MAX_TYPES; i++)
    new_types[i] = NO_TYPE;

  /* Initialize result */
  result.tag   = OK;
  result.value = OK;

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
      case 3:
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
        default:
          /* wrong syntax; try again with next type */
          return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                              types, ++type_index, state, descr));
      } /* switch */

      /* OK, assume it's an article */
      result = MakeSysDescr(line_buf, rest_of_line_buf, NO_ID,nr_of_types,
                            new_types,-1,state,descr);
      if (result.tag == OK || result.tag == PREPOSITIONS) {

        /* store article in the descr struct. */
        descr->article = id;
        return(result);
      }
      else if (result.tag == UNKNOWN_WORD)
        return(result);
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
        default:
          /* wrong syntax; try again with next type */
          return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                              types, ++type_index, state, descr));
      } /* switch */

      /* OK, assume it's an adjective */
      result = MakeSysDescr(line_buf, rest_of_line_buf, NO_ID, nr_of_types,
                            new_types, -1, state, descr);
      if (result.tag == OK || result.tag == PREPOSITIONS) {
        /* Put adjective in descr struct */
        if (descr->nr_of_adjectives == MAX_PARSE_ADJ) {
          ErrHdr();
          PrintError(65, NULL, NULL);
          result.tag = ERROR;
          return(result);
        }
        descr->adjectives[(descr->nr_of_adjectives)++] = id;
        return(result);
      }
      else if (result.tag == UNKNOWN_WORD)
        return(result);

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
        default:
          /* This means state equals 3, which means error. */
          /* Error will occur when we run out of types for */
          /* this word.                                    */
          /* wrong syntax; try again with next type.       */
          return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                              types, ++type_index, state, descr));
      } /* switch */

      result = MakeSysDescr(line_buf, rest_of_line_buf, NO_ID, nr_of_types,
                            new_types, -1, state, descr);

      if (result.tag == OK || result.tag == PREPOSITIONS) {
        descr->noun = id;
        return(result);
      }
      else if (result.tag == UNKNOWN_WORD)
        return(result);

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
          *rest_of_line_buf = line_buf;

          /* Return the preposition's word id to caller.       */
          result.tag   = PREPOSITIONS;
          result.value = id;
          return(result);
          break;
        default:
          /* wrong syntax; try again with next type */
          return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                              types, ++type_index, state, descr));
      } /* switch */
    default:
      /* wrong syntax; try again with next type */
      return(MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                          types, ++type_index, state, descr));
  } /* switch */

  /* no return needed here */
}

int32_t ParseDSys(descr_text, descr)
 char             *descr_text;
 extendedSysDescr *descr;
{
  int32_t      nr_of_types = 0;
  int32_t      types[MAX_TYPES];
  char         **descr_text_2 = NULL;;  /* for parsing descr.part2 */
  resultStruct result;

  types[0] = NO_TYPE;  /* indicates unused array */

  result.tag   = OK;
  result.value = OK;

  /* Malloc() space for descr_text_2 */
  if ((descr_text_2 = (char **) malloc(sizeof(char*))) == NULL) {
    PrintError(1, NULL, "desc_text_2");
    result.tag = ERROR;
    return(result.tag);
  }

  /* Caller must have initialized the descr struct */

  result = MakeSysDescr(descr_text, descr_text_2, NO_ID, nr_of_types,
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
    result = MakeSysDescr(*descr_text_2, descr_text_2, NO_ID, nr_of_types,
                          types, -1, 1, &(descr->part2));
  }

  switch (result.tag) {
    case NO_KEYWORD:
    case OK:
      /* do nothing */
      break;
    default:
      /* result.tag is either ERROR or again PREPOSITIONS */
      /* A system description can only have 2 parts, so   */
      /* only 1 connecting preposition.                   */
      result.tag = ERROR;
      ErrHdr();
      PrintError(66, NULL, descr_text);
      break;
  }
  free(descr_text_2);
  return(result.tag);
}
