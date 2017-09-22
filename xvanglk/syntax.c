
/* Copyright 2016, 2017 Marnix van den Bos */

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

char     *strlwr(char*);
int32_t  ScanWordTable(char*, wordTable*, int32_t, int32_t);
int32_t  LookUpId(char*);
int32_t  NextWordId(char**, int32_t*, int32_t*);
void     InitParsedInput(parsedInput*);
int32_t  ParseInput(char*, parsedInput*, int32_t);
int32_t  CheckSyntax(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, parsedInput*);

/* Following functions are used exclusively by module trnslate.c */
resultStruct MakeSysDescr(char*, char**, int32_t, int32_t, int32_t*, int32_t, int32_t, sysDescr*);
int32_t      ParseDSys(char*, extendedSysDescr*);


char *strlwr(str)
 char *str;
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower(*p);
      p++;
  }
  return str;
}

int32_t    ScanWordTable(word, wt_rec, lower, upper)
 char      *word;    /* word to look for in word_table */
 wordTable *wt_rec;  /* struct to copy info into       */
 int32_t   lower;    /* elements of array between      */
 int32_t   upper;    /* which to search                */
{
  int32_t  value;
  int32_t  i;
  int32_t  j;
  int32_t  result;
  char look_for[MAX_WORD_LEN];

  /* check for NULL word must be done by caller */

  /* convert to lowercase, but keep the original word       */
  /* word is max MAX_WORD_LEN chars, no need to check again */
  strncpy(look_for, word, MAX_WORD_LEN);
  strlwr(look_for);


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
  if ( (result = strcmp(look_for, word_table[i].word)) == 0) {
    /* found it */

    strncpy(wt_rec->word, word_table[i].word, MAX_WORD_LEN);
    wt_rec->id = word_table[i].id;
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


int32_t LookUpId(word)
 char *word;
{
  /* This function is used to quickly find a word's id, without     */
  /* bothering about types etc. All it does is call ScanWordTable() */
  /* with a dummy wt_rec parameter.                                 */

  wordTable dummy_wt_rec;

  return(ScanWordTable(word, &dummy_wt_rec, 0, nr_of_words-1));
}


int32_t NextWordId(line_buf, nr_of_types, types)
 char     **line_buf;
 int32_t  *nr_of_types;
 int32_t  *types;
{
  char      *start = *line_buf;   /* remember start of line_buf     */
  int32_t   i = 0;                /* counter to go through line_buf */
  char      word[MAX_WORD_LEN+1]; /* include a NULL char            */
  int32_t   len;                  /* word length                    */
  int32_t   result = NO_ID;
  wordTable wt_rec;               /* returned by ScanWordTable      */

  /* Test for comma. Comma needs special treatment since it is not  */
  /* recognized as a word by ScanWordTable().                       */
  if (**line_buf == ',') {
    result = COMMA; /* Not a word id. May cause error when printed. */
    types[0] = COMMA;
    types[1] = NO_TYPE;
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

  if (result == NO_ID) {
    /* This msg must be printed here, because in main(), */
    /* we don't know word anymore.                       */
    sprintf(outputline, "I don't know the word %s\n", word);
    Output(outputline);
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
      sprintf(outputline, "INTERNAL ERROR, the impossible just happened.\n");
      Output(outputline);
      return(NO_ID);
  } /* switch */

  /* In case of an unknown word, exit here */
  if (result == NO_ID)
    return(NO_ID);

  /* copy types array and set nr_of_types */
  i = 0;
  for (i=0; i<MAX_TYPES; i++)
    types[i] = wt_rec.types[i];

  /* find nr_of_types */
  i = 0;
  while (i<MAX_TYPES && types[i] != NO_TYPE)
    i++;
  *nr_of_types = i;

  return(result);  /* result is the word id */
}


void InitParsedInput(parsed_input)
 parsedInput *parsed_input;
{
  int32_t i;

  (parsed_input->actor).part1.article               = NO_ID;
  (parsed_input->actor).part1.nr_of_adjectives      = 0;
  (parsed_input->actor).part1.noun                  = NO_ID;
  (parsed_input->actor).connect_prepos              = NO_ID;
  (parsed_input->actor).part2.article               = NO_ID;
  (parsed_input->actor).part2.nr_of_adjectives      = 0;
  (parsed_input->actor).part2.noun                  = NO_ID;

  parsed_input->action                              = NO_ID;
  parsed_input->scope                               = CURR_LOC_ONLY;
  parsed_input->adverbs.nr_of_adverbs               = 0;
  parsed_input->q_word                              = NO_ID;
  parsed_input->direction                           = NO_ID;

  for (i=0; i<MAX_SUBJECTS; i++) {
  (parsed_input->subject[i]).part1.article          = NO_ID;
  (parsed_input->subject[i]).part1.nr_of_adjectives = 0;
  (parsed_input->subject[i]).part1.noun             = NO_ID;
  (parsed_input->subject[i]).connect_prepos         = NO_ID;
  (parsed_input->subject[i]).part2.article          = NO_ID;
  (parsed_input->subject[i]).part2.nr_of_adjectives = 0;
  (parsed_input->subject[i]).part2.noun             = NO_ID;
  }

  (parsed_input->specifier).part1.article           = NO_ID;
  (parsed_input->specifier).part1.nr_of_adjectives  = 0;
  (parsed_input->specifier).part1.noun              = NO_ID;
  (parsed_input->specifier).connect_prepos          = NO_ID;
  (parsed_input->specifier).part2.article           = NO_ID;
  (parsed_input->specifier).part2.nr_of_adjectives  = 0;
  (parsed_input->specifier).part2.noun              = NO_ID;

  parsed_input->prepositions.nr_of_prepositions     = 0;
  parsed_input->value                               = 0;
  parsed_input->ordinal                             = 0;
}


int32_t ParseInput(line_buf, parsed_input, syntax)
 char        *line_buf;
 parsedInput *parsed_input;
 int32_t     syntax;
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
      sprintf(outputline, "Syntax ANSWER not yet implemented.\n");
      Output(outputline);
      return(ERROR);
    default:
      /* We should never get here. */
      sprintf(outputline, "@#§$, the impossible just happened (%d).\n", syntax);
      Output(outputline);
      return(ERROR);
  }

  return(CheckSyntax(line_buf, NO_ID, nr_of_types,
                     types, -1, 0, state, parsed_input));
}


int32_t CheckSyntax(line_buf, id, nr_of_types, types, type_index,
                                  subject_index, state, parsed_input)
 char        *line_buf;
 int32_t     id;
 int32_t     nr_of_types;
 int32_t     *types;
 int32_t     type_index;
 int32_t     subject_index; /* More than one subject allowed. */
 int32_t     state;
 parsedInput *parsed_input;

 /* id, nr_of_types, type_index and state cannot be changed by      */
 /* recursive calls. The rest of the parameters can and therefore   */
 /* we will define duplicates when needed.                          */

 /* As this is a recursive function, the adjectives belonging to    */
 /* actor, subject and specifier will be positioned in parsed_input */
 /* in reverse order (last one first). Same goes for adverbs.       */
{
  int32_t  result;               /* needed to test for unknown words and */
                                 /* stop recursive calling               */
  int32_t  i = 0;
  int32_t  old_state = state;    /* Remember state to retry in case of   */
                                 /* a syntax clash.                      */
                                 /* Also used to fill parsed_input for   */
                                 /* actor/subject/direction/specifier    */
  int32_t  new_types[MAX_TYPES]; /* Needed in case the next word will    */
                                 /* be read in a recursive call, If call */
                                 /* returns ERROR, we must still have    */
                                 /* the types array for a retry.         */

  /* parsed_input must be set to default values by caller.   */

  /* No check for empty line_buf here. We may be in the next */
  /* typecheck for the last word. Ceck for empty line_buf is */
  /* done by NextWordId().                                   */

  /* If this function is called with id = NO_ID, we have to  */
  /* read the next word from the line_buf.                   */

  /* If, after a NextWordId() call the id equals NO_ID, we   */
  /* have an error (unknown word).                           */

  /* Initialize new_types */
  for (i=0; i<MAX_TYPES; i++)
    new_types[i] = NO_TYPE;

  if (id == NO_ID) {
    /* Get the next word from the user input               */
    /* NextWordId() also returns the remainder of line_buf */
    if ( (id = NextWordId(&line_buf, &nr_of_types, types)) == NO_ID)
      /* unknown word */
      return(UNKNOWN_WORD);
    type_index = 0;
  }

  if (id == EOS)   /* End Of String */
    switch (state) {
      case  2: ;
      case  4: ;
      case  6: ;
      case  9: ;
      case 14: ;
      case 16: ;
      case 21: ;
      case 22: ; /* States 22 thru 30 are to allow for     */
      case 24: ; /* locations and objects to be referred   */
      case 26: ; /* to by an adjective.                    */
      case 30: ;
      case 32: ;
      case 33: ;
      case 34: ; /* states 34 thru 38 added on dec 23 2015 */
      case 35: ; /* to allow <numbers> after a noun        */
      case 36: ;
      case 38: ;
      case 41: ;
      case 44: ;
      case 47: ;
      case 50: ;
      case 53:
        return(OK);
      default:
        /* error */
        /* Error msg will be printed by LetsPlay(); except    */
        /* for an unknown word msg, which will be printed by  */
        /* NextWordId().                                      */
        return(ERROR);
    }

  /* In case a word has MAX_TYPE types, we will not encounter */
  /* a NO_TYPE value in its types array. Therefore check here */
  /* for MAX_TYPES exceeding.                                 */

  if (type_index == MAX_TYPES)
    /* We've checked all types; negative result.              */
    return(ERROR);

  switch (types[type_index]) {
    case NO_TYPE:
      /* no more types, this definitely is a wrong syntax */
      return(ERROR);
    case NUMBER:
      /* The NUMBER type is set by ScanWordTable() when it */
      /* detects a numstring. It cannot be used in the     */
      /* vocabulary file.                                  */

      switch (state) {
        case 2:
          state = 33;
          break;
        case 4:       /* added on dec 23 2015 to allow <numbers> after a noun */
          state = 34;
          break;
        case 8:
          state = 32;
          break;
        case 9:       /* added on dec 23 2015 to allow <numbers> after a noun */
          state = 35; /* this is an ordinal number e.g. 'take pebble 3'       */
          break;
        case 14:      /* added on dec 23 2015 to allow <numbers> after a noun */
          state = 36; /* this is an ordinal number e.g. 'take pebble 3'       */
          break;
        case 17:      /* added on dec 23 2015 to allow <numbers> after a noun */
          state = 37; /* this is an ordinal number e.g. 'take pebble 3'       */
          break;
        case 21:      /* added on dec 23 2015 to allow <numbers> after a noun */
          state = 38; /* this is an ordinal number e.g. 'take pebble 3'       */
          break;
        default:
          return(ERROR);
      }
      /* It is not necessary to use new_types here, */
      /* but we'll do it anyway.                    */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* Fill parsed_input. */
        switch (state) {
          case 32: ;
          case 33:
            parsed_input->value = id;
            break;
          default:
            parsed_input->ordinal = id;
            break;
        }
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Don't need to do this, just want to be consistent. */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));}
    case VERB:
      switch (state) {
        case 1:
          state = 2;
          break;
        case 18:
          state = 2;
          break;
        case 19:
          state = 20;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* For now, assume it's a verb                     */
      /* See if rest of input matches if this is a verb. */
      /* -1, NULL params will be read by called function */
      /* NO_ID tells CheckSyntax to read next word from  */
      /* line_buf.                                       */

      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* Fill parsed_input. */
        parsed_input->action = id;
        parsed_input->scope  = verb_dir[id-FIRST_VERB_ID].scope;
        return(OK);
      }
      else if (result == UNKNOWN_WORD) {
        return(UNKNOWN_WORD);}
      else /* error */ {
        /* Oops! it wasn't a verb after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));}
    case DIRECTIONS:
      switch (state) {
        case  1: ; /* same as  2 */
        case  2: ; /* same as  3 */
        case  3: ; /* same as  4 */
        case  4: ; /* same as  8 */
        case  8: ; /* same as 18 */
        case 18:
        case 23:   /* added on dec 23 2015 to allow 'look to the <dir>'       */
        case 25:   /* added on aug 27 2015 to allow 'throw subj to the <dir>' */
        case 34:   /* added on dec 23 2015 to allow <numbers> after a noun    */
        case 50:
          state = 16;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      } /* switch */

      /* OK, assume it's a direction */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* Yes, it was a direction. */
        /* Fill parsed_input.       */
        if (old_state == 1 || old_state == 18)
          /* In this state, we have to add the verb ourselfs. */
          /* I.e. "n" is parsed as "go north". If this        */
          /* syntaxcheck fails (not a direction after all),   */
          /* restore the action to NO_ID.                     */
          parsed_input->action = LookUpId("go");
        parsed_input->direction = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a direction after all; try again */
        /* Check to see if we must restore action.          */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));}
    case ADVERBS:
      switch (state) {
        case  2: ;
        case 12:
          state = 11;
          break;
        case 4: ;
        case 7:
        case 34:  /* added on dec 23 2015 to allow <numbers> after a noun */
        case 50:
          state = 6;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's an adverb */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* fill parsed_input. */
        /* CAUTION!! If MAX_PARSE_ADVERBS has been exceeded, */
        /* we just skip this adverb.                         */
        if (parsed_input->adverbs.nr_of_adverbs != MAX_PARSE_ADVERBS)
          parsed_input->adverbs.adverb[parsed_input->adverbs.
                                                nr_of_adverbs++] = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't an adverb after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));
      }
      break;
    case CONNECT_PREPOSITIONS:
      /* 26nov2016: added this to allow more complex system */
      /* descriptions like "hall of fame", "man with shiny  */
      /* sword", ... . Each preposition get automatically   */
      /* also assigned type CONNECT_PREPOSITIONS by the     */
      /* compiler. This is transparent to the user. We need */
      /* this extra type to resolve ambiguities like        */
      /* "hit the man with hammer".                         */
       switch (state) {
         case 4:
           state = 48;
           break;
         case 14:
           state = 45;
           break;
         case 17:
           state = 37;
           break;
         case 21:
           state = 42;
           break;
         case 9:
           state = 51;
           break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }

      /* assume it's a connect preposition */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);

      if (result == OK) {
        /* find out which connect_prepos to fill: actor, */
        /*subject or specifier.                          */
        switch (old_state) {
          case 17: ;      /* actor */
          case 14:
            (parsed_input->actor).connect_prepos = id;
            break;
          case 21: ;      /* subject */
          case 4:
            (parsed_input->subject[subject_index]).connect_prepos = id;

            break;
          case 9:         /* specifier */
            (parsed_input->specifier).connect_prepos = id;
            break;
          default:
            /* we should never get here */
            sprintf(outputline, "INTERNAL ERROR. unknown old_state for connecting preposition.\n");
            Output(outputline);
            return(ERROR);
            break;
        } /* switch */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else {
        /* Oops! it wasn't a connecting preposition after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));
      }
      break;
    case PREPOSITIONS:
      switch (state) {
        case 2:
          state = 3;
          break;
        case  4: ;
        case  6: ;
        case  8: ;
        case 10: ;
        case 14: ;
        case 34: ;    /* added on dec 23 2015 to allow <numbers> after a noun */
        case 36: ;    /* added on dec 23 2015 to allow <numbers> after a noun */
        case 47:
        case 50:
          state = 8;
          break;
        case 11:
          state = 13;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }

      /* assume it's a preposition */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* fill parsed_input. */
        /* CAUTION!! If MAX_PARSE_PREPOS has been exceeded, */
        /* we just skip this preposition.                   */
        if (parsed_input->prepositions.nr_of_prepositions != MAX_PARSE_PREPOS)
          parsed_input->prepositions.preposition[parsed_input->
                             prepositions.nr_of_prepositions++] = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else {
        /* Oops! it wasn't a preposition after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));
      }
      break;
    case COMMA:
      switch (state) {
        case 4:
        case 34:   /* state 34 added on dec 23 2015 to allow */
                   /* <numbers> after a noun                 */
        case 50:
          /* Ok, the next subject is coming up. */
          /* Test for too many subjects.        */
          if (++subject_index == MAX_SUBJECTS) {
            sprintf(outputline, "Too many subjects in input.\n");
            Output(outputline);
            return(ERROR);
          }
          state = 5;
          break;
        case 9: ;
        case 35: ;    /* added on dec 23 2015 to allow <numbers> after a noun */
        case 53:
          state = 10;
          break;
        case 14: ;
        case 36:      /* added on dec 23 2015 to allow <numbers> after a noun */
        case 47:
          state = 15;
          break; /* break seemed to be missing, added on dec 23rd 2015 */
        case 17: ;
        case 37:      /* added on dec 23 2015 to allow <numbers> after a noun */
        case 41:
          state = 18;
          break;
        default:
          /* error */
          sprintf(outputline, "There is a comma in the wrong place.\n");
          Output(outputline);
          return(ERROR);
      }
      /* OK to use types here rather than new_types. */
      return(CheckSyntax(line_buf, NO_ID, nr_of_types, types,
                         -1, subject_index, state, parsed_input));
    case CONJUNCTION:
      switch (state) {
        case 11:
          state = 12;
          break;
        case 4:
          state = 5;
          break;
        case 6:
          state = 7;
          break;
        case 9: ;
        case 35: ;    /* added on dec 23 2015 to allow <numbers> after a noun */
        case 53:
          state = 10;
          break;
        case 14: ;
        case 36:      /* added on dec 23 2015 to allow <numbers> after a noun */
        case 47:
          state = 15;
          break;
        case 34:      /* added on dec 23 2015 to allow <numbers> after a noun */
        case 50:
          state = 5;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's a conjunction */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* fill the action record */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a conjunction after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));}
    case Q_WORDS:
      switch (state) {
        case 1:
          state = 19;
          break;
        case 18:
          state = 19;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's a question word */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* fill parsed_input */
        parsed_input->q_word = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a question word after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));}
    case ARTICLES:
      switch (state) {
        case 1:
          state = 29;
          break;
        case 2:
        case 3:
        case 5:
          state = 23;
          break;
        case 8:
        case 10:
          state = 25;
          break;
        case 13:
        case 15:
          state = 27;
          break;
        case 20:
          state = 31;
          break;
        case 39:
          state = 40;
          break;
        case 42:
          state = 43;
          break;
        case 45:
          state = 46;
          break;
        case 48:
          state = 49;
          break;
        case 51:
          state = 52;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's an article */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* do nothing, articles are not needed in parsed_input */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't an article after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));}

    case NOUNS:
      switch (state) {
        case  2: ;    /* subject part 1 */
        case  3: ;
        case  5: ;
        case 22: ;
        case 23: ;
        case 33:
          state = 4;
          break;
        case  8: ;    /* specifier part 1*/
        case 10: ;
        case 24: ;
        case 25:
          state = 9;
          break;
        case 13: ;    /* subject part 1 */
        case 15: ;
        case 26: ;
        case 27:
          state = 14;
          break;
        case  1: ;    /* actor part 1*/
        case 28: ;
        case 29:
          state = 17;
          break;
        case 20: ;    /* subject part 1 */
        case 30: ;
        case 31:
          state = 21;
          break;
        case 39: ;    /* actor part 2 */
        case 40:
          state = 41;
          break;
        case 42: ;    /* subject part 2 */
        case 43:
          state = 44;
          break;
        case 45: ;    /* subject part 2 */
        case 46:
          state = 47;
          break;
        case 48: ;    /* subject part 2 */
        case 49:
          state = 50;
          break;
        case 51: ;    /* specifier part 2 */
        case 52:
          state = 53;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's a noun */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* fill parsed_input, depending on old_state */
        switch (old_state) {
          case  2: ;    /* subject part 1 noun */
          case  3: ;
          case  5: ;
          case 22: ;
          case 23: ;
          case 33: ;
          case 13: ;
          case 15: ;
          case 26: ;
          case 27: ;
          case 20: ;
          case 30: ;
          case 31:
            (parsed_input->subject[subject_index]).part1.noun = id;
            break;
          case  8: ;    /* specifier part 1 noun */
          case 10: ;
          case 24: ;
          case 25:
            (parsed_input->specifier).part1.noun = id;
            break;
          case  1: ;    /* actor part 1 noun*/
          case 28: ;
          case 29:
            (parsed_input->actor).part1.noun = id;
            break;
          case 39: ;    /* actor part 2 noun */
          case 40:
            (parsed_input->actor).part2.noun = id;
             break;
          case 42: ;    /* subject part 2 noun */
          case 43: ;
          case 45: ;
          case 46: ;
          case 48: ;
          case 49:
            (parsed_input->subject[subject_index]).part2.noun = id;
            break;
          case 51: ;    /* specifier part 2 */
          case 52:
            (parsed_input->specifier).part2.noun = id;
            break;
          default:
            /* we should never get here */
            sprintf(outputline, "INTERNAL ERROR. unknown old_state for noun.\n");
            Output(outputline);
            return(ERROR);
            break;
        } /* switch */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a noun after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));}
    case ADJECTIVES:
      switch (state) {
        case  2: ;     /* subject part 1 */
        case  3: ;
        case  5: ;
        case 22: ;
        case 23: ;
        case 33:
          state = 22;
          break;
        case  8: ;     /* specifier part 1 */
        case 10: ;
        case 24: ;
        case 25:
          state = 24;
          break;
        case 13: ;     /* subject part 1 */
        case 15: ;
        case 26: ;
        case 27:
          state = 26;
          break;
        case  1: ;     /* actor part 1 */
        case 28: ;
        case 29:
          state = 28;
          break;
        case 20: ;     /* subject part 1 */
        case 30: ;
        case 31:
          state = 30;
          break;
        case 39: ;     /* actor part 2 */
        case 40:
          state = 40;
          break;
        case 42: ;     /* subject part 2 */
        case 43:
          state = 43;
          break;
        case 45: ;     /* actor part 2 */
        case 46:
          state = 46;
          break;
        case 48: ;     /* subject part 2 */
        case 49:
          state = 49;
          break;
        case 51: ;     /* specifier part 2 */
        case 52:
          state = 52;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's an adjective */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* Fill parsed_input, depending on old_state */
        switch (old_state) {
          case  2: ;     /* subject part 1 adjective */
          case  3: ;
          case  5: ;
          case 22: ;
          case 23: ;
          case 33: ;
          case 13: ;
          case 15: ;
          case 26: ;
          case 27: ;
          case 20: ;
          case 30: ;
          case 31:
            /* If MAX_PARSE_ADJ has been exceeded, we will */
            /* just skip this adjective.                   */
            if (parsed_input->subject[subject_index].part1.nr_of_adjectives < MAX_PARSE_ADJ)
              parsed_input->subject[subject_index].part1.
                     adjectives[parsed_input->subject[subject_index].
                                                         part1.nr_of_adjectives++] = id;
            break;
          case  8: ;     /* specifier part 1 adjective */
          case 10: ;
          case 24: ;
          case 25:
            /* If MAX_PARSE_ADJ has been exceeded, we will */
            /* just skip this adjective.                   */
            if (parsed_input->specifier.part1.nr_of_adjectives < MAX_PARSE_ADJ)
              parsed_input->specifier.part1.adjectives[parsed_input->
                                   specifier.part1.nr_of_adjectives++] = id;
            break;
          case  1: ;     /* actor part 1 adjective */
          case 28: ;
          case 29:
            /* If MAX_PARSE_ADJ has been exceeded, we will */
            /* just skip this adjective.                   */
            if (parsed_input->actor.part1.nr_of_adjectives < MAX_PARSE_ADJ)
              parsed_input->actor.part1.adjectives[parsed_input->
                                     actor.part1.nr_of_adjectives++] = id;
            break;
          case 42: ;
          case 43:
          case 48:       /* subject part 2 adjective */
          case 49:
            if (parsed_input->subject[subject_index].part2.nr_of_adjectives < MAX_PARSE_ADJ)
              parsed_input->subject[subject_index].part2.
                     adjectives[parsed_input->subject[subject_index].
                                                         part2.nr_of_adjectives++] = id;
            break;
          case 39: ;     /* actor part 2 adjective */
          case 40: ;
          case 45: ;
          case 46:
            /* If MAX_PARSE_ADJ has been exceeded, we will */
            /* just skip this adjective.                   */
            if (parsed_input->actor.part2.nr_of_adjectives < MAX_PARSE_ADJ)
              parsed_input->actor.part2.adjectives[parsed_input->
                                     actor.part2.nr_of_adjectives++] = id;
          break;
          case 51: ;   /* specifier part 2 */
          case 52:
            if (parsed_input->specifier.part2.nr_of_adjectives < MAX_PARSE_ADJ)
              parsed_input->specifier.part2.adjectives[parsed_input->
                                   specifier.part2.nr_of_adjectives++] = id;
          break;
          default:
            /* we should never get here */
            sprintf(outputline, "INTERNAL ERROR. unknown old_state for adjective.\n");
            Output(outputline);
            return(ERROR);
            break;
        } /* switch */
        return(OK);
      } /* if */
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't an adjective after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));}
    default:
      sprintf(outputline, "INTERNAL ERROR. unknown keyword type: %d.\n", types[type_index]);
      Output(outputline);
      return(ERROR);
  } /* switch */
  /* no return here */
}


resultStruct MakeSysDescr(line_buf, rest_of_line_buf, id, nr_of_types,
                          types, type_index, state, descr)
 char     *line_buf;
 char     **rest_of_line_buf;
 int32_t  id;
 int32_t  nr_of_types;
 int32_t  *types;
 int32_t  type_index;
 int32_t  state;
 sysDescr *descr;
{
  /* descr must be set to default values by caller.               */

  int32_t i = 0;
  resultStruct result = {OK, OK};
  int32_t old_state = state;      /* Remember state for retry in case of  */
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
        /* descr->article = id; */
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
            sprintf(outputline, "You used too many adjectives, but i'll see what I can do.\n");
            Output(outputline);
          }
        }
        else {
          /* Double adjective, do nothing. */
          sprintf(outputline, "DOUBLE ADJECTIVE\n");
          Output(outputline);
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


int32_t ParseDSys(description, descr)
 char             *description;
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
    sprintf(outputline, "ParseDSys(): error in malloc for descr_text_2.\n");
    Output(outputline);
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


