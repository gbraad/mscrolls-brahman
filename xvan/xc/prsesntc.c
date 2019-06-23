
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
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "prsesntc.h"


/********************************************************************/
/* This module is used to parse and translate strings that are used */
/* to determine which trigger to execute for which user commands.   */
/* These sentences are located in the source file in the TRIGGERS   */
/* clause. Example: "take %o_lamp" -> t_take.                       */
/* It is IMPORTANT that locations and objects that are used in the  */
/* sentences are referred to by their id and NOT by their system    */
/* description. The word `lamp' will be stored as its word id and   */
/* NOT as its object id, whereas o_lamp will.                       */
/* First, the syntax of the sentence is checked by CheckSyntax().   */
/* This is a simplified version of the CheckSyntax as used by the   */
/* interpreter, since it doesn't have to parse system descryptions. */
/* CheckSyntax returns an action_rec, which will be stored in the   */
/* storyfile (it doesn't create a parsed_input struct, like the     */
/* interpreter's version of CheckSyntax does.                       */
/* The NextWordId() function as used by the interpreter cannot be   */
/* used here, since there may be [l_xxxx], [o_xxxx] and words in    */
/* the sentence, which cannot be handled correctly by the           */
/* interpreter's function.                                          */
/********************************************************************/


/*************************/
/* Function Declarations */
/*************************/

int32_t GiveNextId(char**, int32_t*, int32_t*, int32_t*, int32_t*);
int32_t IsDirection(char*, int32_t*);
int32_t LookUpId(char*);
void    InitActionRec(actionRec*);
int32_t CheckSyntax(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, actionRec*);
int32_t ParseSentence(char*, actionRec*);


/************************/
/* Function definitions */
/************************/

int32_t LookUpId(word)
 char *word;
{
  /* This function is used to quickly find a word's id, without     */
  /* bothering about types etc. All it does is call ScanWordTable() */
  /* with a dummy wt_rec parameter.                                 */

  wordTable dummy_wt_rec;

  return(ScanWordTable(word, &dummy_wt_rec, 0, nr_of_words-1));
}


int32_t GiveNextId(line_buf, id1, id2, nr_of_types, types)
 /* id1 and id2 are to hold parameter ids */
 char     **line_buf;
 int32_t  *id1;
 int32_t  *id2;
 int32_t  *nr_of_types;
 int32_t  *types;
{
  /* This function is an adapted version of NextWordId(). It is  */
  /* capable of recognizing parameters.                          */

  char      *start = *line_buf;   /* remember start of line_buf     */
  int32_t   i = 0;                /* counter to go through line_buf */
  int32_t   j = 0;
  int32_t   word_id = NO_ID;
  char      word[MAX_WORD_LEN+1]; /* include a NULL char            */
  int32_t   len;                  /* word length                    */
  wordTable wt_rec;               /* returned by ScanWordTable      */

  /* Initialize wt_rec */
  wt_rec.word[0]       = '\0';
  wt_rec.print_word[0] = '\0';
  wt_rec.id            = NO_ID;

  /* initialize parameter ids */
  *id1 = NO_ID;
  *id2 = NO_ID;

  for (j=0; j<MAX_TYPES; j++)
    wt_rec.types[j] = NO_TYPE;

  /* Test for comma. Comma needs special treatment since it is not  */
  /* recognized as a word by ScanWordTable().                       */
  if (**line_buf == ',') {
    word_id      = COMMA; /* Not a word id. May cause error when printed. */
    types[0]     = COMMA;
    types[1]     = NO_TYPE;
    *nr_of_types = 1;
    (*line_buf)++;
    return(word_id);
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

  /* Check for id in string. Syntax: -1  TAG MSB LSB.       */
  /* If either MSB or LSB is 0, this is indicated in TAG    */
  /* and a 1 is stored in order for strlen() to work ok.    */
  /* Don't forget to cast -1 to char, to avoid problems     */
  /* with unsigned char.                                    */
  /* Testing for -1 instead of (char) -1, will fail for     */
  /* unsigned chars.                                        */
  /* 25jan2017: in case of a location id, object id or THIS */
  /* parameter an addtl id will follow. This id must be a   */
  /* NO_ID, because otherwise this would mean they added a  */
  /* description or attribute, which is not allowed in      */
  /* TRIGGERS or VERB sections.                             */


  if (start[i] == (char) -1) {
    /* There is an id coming up. */
    *id1  = ( (int32_t) ( ((unsigned char) start[i+1]) & 0xF0) ==  0x10? 0: (int32_t) ( ((unsigned char) start[i+2]))*256);
    *id1 += ( (int32_t) ( ((unsigned char) start[i+1]) & 0x0F) ==  0x01? 0: (int32_t) ((unsigned char) start[i+3]));

    /* Set *line_buf to end of id. */
    *line_buf += 4;

    /* check for location, object or THIS */
    if (IsLocId(*id1) || IsObjId(*id1) || *id1 == THIS) {
      *id2  = ( (int32_t) ( ((unsigned char) start[i+5]) & 0xF0) ==  0x10? 0: (int32_t) ( ((unsigned char) start[i+6]))*256);
      *id2 += ( (int32_t) ( ((unsigned char) start[i+5]) & 0x0F) ==  0x01? 0: (int32_t) ((unsigned char) start[i+7]));

      /* Set *line_buf to end of id. */
      *line_buf += 4;
    }

	return(word_id);
  }

  while (!(start[i] == '\0' || start[i] == ',' || start[i] == SPACE
           || start[i] == (char) -1))
    i++;

  /* copy the word; but no more than MAX_WORD_LEN chars */
  len = (i<MAX_WORD_LEN? i: MAX_WORD_LEN);
  strncpy(word, start, len);

  /* add a '\0' char */
  word[len] = '\0';

  /* 0, nr_of_words-1 denote first and last element of word_table */
  word_id = ScanWordTable(word, &wt_rec, 0, nr_of_words-1);

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
    case (char) -1:
      /* A location, object or timer id is next. */
      (*line_buf) += i;
      break;
    default:
      ErrHdr();
      printf("GiveNextId(): ERROR, the impossible just happened.\n");
      return(NO_ID);
  } /* switch */

  /* In case of an unknown word, exit here */
  if (word_id == NO_ID && wt_rec.types[0] == NO_TYPE) {
    /* check for type because it may also be a number */
    /* with the same value as the NO_ID #define       */
    ErrHdr();
    /* I don't know the word ... */
    PrintError(79, NULL, word);
    return(NO_ID);
  }

  /* copy types array and set nr_of_types */
  i = 0;
  for (i=0; i<MAX_TYPES; i++)
    types[i] = wt_rec.types[i];

  /* find nr_of_types */
  i = 0;
  while (i<MAX_TYPES && types[i] != NO_TYPE)
    i++;
  *nr_of_types = i;

  return(word_id);  /* result is the word id or the value in case of a number */
}


int32_t IsDirection(word, id)
 char *word;
 int32_t  *id;
{
  int32_t       i = 0;
  wordTable wt_rec;

  *id = NO_ID;

  /* Truncate word if necessary. */
  if (strlen(word) > MAX_WORD_LEN)
    word[MAX_WORD_LEN] = '\0';

  if ((*id = ScanWordTable(word, &wt_rec, 0, nr_of_words-1)) != NO_ID) {
    while (i< MAX_TYPES) {
      if (wt_rec.types[i] == DIRECTIONS)
        return(OK);
      if (wt_rec.types[i] == NO_TYPE)
        return(ERROR);

      i++;
    } /* while */
  } /* if */
  return(ERROR);
}


void InitActionRec(action_rec)
 actionRec *action_rec;
{
  int i = 0;
  /* Default actor is the player. */
  /* 09sep17 - changed default actor from */
  /* PLAYER to wildcard ACTOR */

  action_rec->actor                           = PLAYER;
  action_rec->action1                         = NO_ID;
  action_rec->action2                         = NO_ID;
  action_rec->adverbs.nr_of_adverbs           = 0;
  for (i=0; i<MAX_PARSE_ADJ; i++)
    action_rec->adverbs.adverb[i] = NO_ID;
  action_rec->q_word                          = NO_ID;
  action_rec->direction                       = NO_ID;
  action_rec->subject                         = NO_ID;
  action_rec->specifier                       = NO_ID;
  action_rec->prepositions.nr_of_prepositions = 0;
  for (i=0; i<MAX_PARSE_PREPOS; i++)
    action_rec->prepositions.preposition[i] = NO_ID;
  action_rec->value                           = 0;
  action_rec->ordinal                         = 0;
  action_rec->execute[0]                      = NO_ID;
  action_rec->execute[1]                      = NO_ID;
}


void InitVerbActionRec(action_rec)
 actionRec *action_rec;
{
  int i = 0;

  /* 09sep17 - made this InitVerbActionRec() function   */
  /* specifically for the verb default code. The        */
  /* difference with InitActionRec() (used by locations */
  /* and objects) is that InitVerbActionRec() has ACTOR */
  /* as the default actor, whereas InitActionRec() has  */
  /* PLAYER as the default actor. For location and      */
  /* object triggers it matters who is the actor, for   */
  /* verb default code it doesn't (same default reply   */
  /* for all actors.                                    */

  action_rec->actor                           = ACTOR;
  action_rec->action1                         = NO_ID;
  action_rec->action2                         = NO_ID;
  action_rec->adverbs.nr_of_adverbs           = 0;
  for (i=0; i<MAX_PARSE_ADJ; i++)
    action_rec->adverbs.adverb[i] = NO_ID;
  action_rec->q_word                          = NO_ID;
  action_rec->direction                       = NO_ID;
  action_rec->subject                         = NO_ID;
  action_rec->specifier                       = NO_ID;
  action_rec->prepositions.nr_of_prepositions = 0;
  for (i=0; i<MAX_PARSE_PREPOS; i++)
    action_rec->prepositions.preposition[i] = NO_ID;
  action_rec->value                           = 0;
  action_rec->ordinal                         = 0;
  action_rec->execute[0]                      = NO_ID;
  action_rec->execute[1]                      = NO_ID;
}


int32_t CheckSyntax(line_buf, id, nr_of_types, types, type_index, state,
                                                          action_rec)
 char       *line_buf;
 int32_t    id;
 int32_t    nr_of_types;
 int32_t    *types;
 int32_t    type_index;
 int32_t    state;
 actionRec  *action_rec;

 /* id, nr_of_types, type_index and state cannot be changed by      */
 /* recursive calls. The rest of the parameters can and therefore   */
 /* we will define duplicates when needed.                          */
{
  switch(story_language) {
    case ENG:
      return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             type_index, state, action_rec));
      break;
    case NL:
      return(NL_CheckSyntax(line_buf, id, nr_of_types, types,
                            type_index, state, action_rec));
      break;
    default:
      /* we should never get here, use English */
      return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             type_index, state, action_rec));
  }
}


int32_t ParseSentence(line_buf, action_rec)
 char       *line_buf;
 actionRec  *action_rec;
{
  int32_t  nr_of_types = 0;
  int32_t  i;
  int32_t  types[MAX_TYPES];
  int32_t  result;

  /* Initialize types array */
  for (i=0; i<MAX_TYPES; i++)
    types[i] = NO_TYPE;

  result = CheckSyntax(line_buf, NO_ID, nr_of_types,
                       types, -1, 1, action_rec);
  if (result == ERROR) {
    ErrHdr();
    PrintError(81, NULL, NULL);
  }
  return(result);
}
