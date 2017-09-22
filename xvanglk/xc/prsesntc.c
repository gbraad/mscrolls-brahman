
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "prsesntc.h"

extern void PrintActionrec(actionRec*);
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
int32_t GiveNextId(char**, int32_t*, int32_t*);
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


int32_t GiveNextId(line_buf, nr_of_types, types)
 char **line_buf;
 int32_t  *nr_of_types;
 int32_t  *types;
{
  /* This function is an adapted version of NextWordId(). It is  */
  /* capable of recognizing parameters  and return their id      */

  char      *start = *line_buf;   /* remember start of line_buf     */
  int32_t   i = 0;                /* counter to go through line_buf */
  int32_t   j = 0;
  char      word[MAX_WORD_LEN+1]; /* include a NULL char            */
  int32_t   len;                  /* word length                    */
  int32_t   result = NO_ID;
  int32_t   id2;
  wordTable wt_rec;               /* returned by ScanWordTable      */

  /* Initialize wt_rec */
  wt_rec.word[0]       = '\0';
  wt_rec.print_word[0] = '\0';
  wt_rec.id            = NO_ID;

  for (j=0; j<MAX_TYPES; j++)
    wt_rec.types[j] = NO_TYPE;

  /* Test for comma. Comma needs special treatment since it is not  */
  /* recognized as a word by ScanWordTable().                       */
  if (**line_buf == ',') {
    result       = COMMA; /* Not a word id. May cause error when printed. */
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
    result  = ( (int32_t) ( ((unsigned char) start[i+1]) & 0xF0) ==  0x10? 0: (int32_t) ( ((unsigned char) start[i+2]))*256);
    result += ( (int32_t) ( ((unsigned char) start[i+1]) & 0x0F) ==  0x01? 0: (int32_t) ((unsigned char) start[i+3]));

    /* Set *line_buf to end of id. */
    *line_buf += 4;

    /* check for location, object or THIS */
    if (IsLocId(result) || IsObjId(result) || result == THIS) {
      id2  = ( (int32_t) ( ((unsigned char) start[i+5]) & 0xF0) ==  0x10? 0: (int32_t) ( ((unsigned char) start[i+6]))*256);
      id2 += ( (int32_t) ( ((unsigned char) start[i+5]) & 0x0F) ==  0x01? 0: (int32_t) ((unsigned char) start[i+7]));

      if (id2 != NO_ID) {
        /* error */
        ErrHdr();
        printf("\nLine %d: error in string, unexpected id.\n", line_num);
        return(NO_ID);
      }
      /* Set *line_buf to end of id. */
      *line_buf += 4;
    }

	return(result);
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
  result = ScanWordTable(word, &wt_rec, 0, nr_of_words-1);

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
  if (result == NO_ID && wt_rec.types[0] == NO_TYPE) {
    /* check for type because it may also be a number */
    /* with the same value as the NO_ID #define       */
    ErrHdr();
    printf("I don't know the word %s\n", word);
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

  return(result);  /* result is the word id or the value in case of a number */
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
  /* Default actor is the player. */
  /* 09sep17 - changed default actor from */
  /* PLAYER to wildcard ACTOR */

  action_rec->actor                           = PLAYER;
  action_rec->action                          = NO_ID;
  action_rec->adverbs.nr_of_adverbs           = 0;
  action_rec->q_word                          = NO_ID;
  action_rec->direction                       = NO_ID;
  action_rec->subject                         = NO_ID;
  action_rec->specifier                       = NO_ID;
  action_rec->prepositions.nr_of_prepositions = 0;
  action_rec->value                           = 0;
  action_rec->ordinal                         = 0;
}

void InitVerbActionRec(action_rec)
 actionRec *action_rec;
{
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
  action_rec->action                          = NO_ID;
  action_rec->adverbs.nr_of_adverbs           = 0;
  action_rec->q_word                          = NO_ID;
  action_rec->direction                       = NO_ID;
  action_rec->subject                         = NO_ID;
  action_rec->specifier                       = NO_ID;
  action_rec->prepositions.nr_of_prepositions = 0;
  action_rec->value                           = 0;
  action_rec->ordinal                         = 0;
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
  int32_t  result;               /* needed to test for unknown words and */
                                 /* stop recursive calling               */
  int32_t  i = 0;
  int32_t  old_state = state;    /* Remember state to retry in case of   */
                                 /* a syntax clash.                      */
                                 /* Also used to fill action_rec for     */
                                 /* actor/subject/direction/specifier    */
  int32_t  new_types[MAX_TYPES]; /* Needed in case the next word will    */
                                 /* be read in a recursive call, If call */
                                 /* returns ERROR, we must still have    */
                                 /* the types array for a retry.         */

  /* action_rec must be set to default values by caller.     */

  /* No check for empty line_buf here. We may be in the next */
  /* typecheck for the last word. Check for empty line_buf   */
  /* is done by GiveNextId().                                */

  /* If this function is called with id = NO_ID, we have to  */
  /* read the next word from the line_buf.                   */

  /* If, after a GiveNextId() call the id equals NO_ID, we   */
  /* have an error (unknown word) or a number with the same  */
  /* value as the NO_ID #define.                             */

  /* Initialize new_types */
  for (i=0; i<MAX_TYPES; i++)
    new_types[i] = NO_TYPE;

  if (id == NO_ID) {
    /* Get the next word from the user input               */
    /* GiveNextId() also returns the remainder of line_buf */
    if ( (id = GiveNextId(&line_buf, &nr_of_types, types)) == NO_ID)
      /* unknown word */
      return(UNKNOWN_WORD);
    type_index = 0;
  }

  /* Test for location id, object id, %direction, %prepos, %action    */
  /* and %value must be done outside the switch loop, since they      */
  /* don't have a valid types array. They are filled out immediately. */

  /* But before we test for the above, we must rule out that the id   */
  /* parameter is a number. If we don't test for number, id - when it */
  /* is a number - may also match a location or object id range or    */
  /* any of the #defines for the wildcard parameters.                 */

  /* A number parameter will have 1 type, types[0] will be NUMBER.    */

  if (types[0] != NUMBER) {
    if (IsLocId(id) || IsObjId(id)) {
      switch (state) {
        case  2: ;
        case  3: ;
        case  5: ;
        case 23:
          state = 4; /* %o_subject */
          action_rec->subject = id;
          break;
        case 13: ;
        case 15:
          state = 14; /* %o_subject */
          action_rec->subject = id;
          break;
        case  8: ;
        case 10:
          state = 9; /* %o_specifier */
          action_rec->specifier = id;
          break;
        case 1:
          state = 17; /* o_actor */
          action_rec->actor = id;
          break;
        case 20:
          state = 21; /* o_subject */
          action_rec->subject = id;
          break;
        default:
          /* Error message will be printed by ParseSentence().    */
          /* This is because there is another error (using        */
          /* nouns etc. to denote actor etc.) that returns ERROR. */
          return(ERROR);
      } /* switch */

      return(CheckSyntax(line_buf, NO_ID, 0, types, -1, state, action_rec));
    } /* if */

    if (id == PREPOS) { /* [prepos] in sentence. */
      switch (state) {
          case 2:
            state = 3;
            break;
          case  4: ;
          case  6: ;
          case  8: ;
          case 10: ;
          case 14:
            state = 8;
            break;
          case 11:
            state = 13;
            break;
          default:
            /* Error message will be printed by ParseSentence(). */
            return(ERROR);
      }
      if (action_rec->prepositions.nr_of_prepositions != MAX_PARSE_PREPOS)
        action_rec->prepositions.preposition[action_rec->
                           prepositions.nr_of_prepositions++] = id;
      return(CheckSyntax(line_buf, NO_ID, 0, types, -1, state,
                                                           action_rec));
    } /* if */

    if (id == DIR) {  /* [dir] in sentence */
      switch (state) {
        case  1: ;
        case 18:
          action_rec->action = LookUpId("go");  /* No error checking? */
          /* No break here. */
        case 2: ;
        case 3: ;
        case 4: ;
        case 8: ;
        case 24:       /* 26 dec 2015 */
          state = 16;
          action_rec->direction = id;
          break;
        default:
          /* Error message will be printed by ParseSentence. */
          return(ERROR);
      } /* switch */

      return(CheckSyntax(line_buf, NO_ID, 0, types, -1, state,
                                                           action_rec));
    } /* if */

    if (id == ACTION) {     /* "[action]" in sentence. */
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
          /* Error message will be printed by ParseSentence. */
          return(ERROR);
      }
      action_rec->action = ACTION;

      return(CheckSyntax(line_buf, NO_ID, 0, types, -1, state,
                                                           action_rec));
    } /* if */

    if (id == VALUE) {     /* "[value]" in sentence. */
      switch (state) {
        case 2:
          state = 23;
          break;
        case 8:
          state = 22;
          break;
        default:
          /* Error message will be printed by ParseSentence. */
          return(ERROR);
      }
      action_rec->value = VALUE;

      return(CheckSyntax(line_buf, NO_ID, 0, types, -1, state,
                                                           action_rec));
    } /* if */

    if (id == ORDINAL) {     /* "[ord]" in sentence. */
      switch (state) {
        case 4:
          state = 24;
          break;
        case 9:
          state = 25;
          break;
        case 14:
          state = 26;
          break;
        case 17:
          state = 27;
          break;
        case 21:
          state = 28;
          break;
        default:
          /* Error message will be printed by ParseSentence. */
          return(ERROR);
      }
      action_rec->ordinal = ORDINAL;

      return(CheckSyntax(line_buf, NO_ID, 0, types, -1, state,
                                                           action_rec));
    } /* if */


    if (id == EOS)   /* End Of String */
      switch (state) {
        case  2: ;
        case  4: ;
        case  6: ;
        case  9: ;
        case 14: ;
        case 16: ;
        case 19: ;
        case 21: ;
        case 22: ;
        case 23: ;
        case 24: ;   /* 26 dec 2015 */
        case 25: ;   /* 26 dec 2015 */
        case 26: ;   /* 26 dec 2015 */
        case 28:     /* 26 dec 2015 */
          return(OK);
        default:
          /* error */
          return(ERROR);
      }
  }
  /* In case a word has MAX_TYPE types, we will not encounter */
  /* a NO_TYPE value in its types array. Therefore check here */
  /* for MAX_TYPES exceeding.                                 */

  if (type_index == MAX_TYPES)
    /* We've checked all types; negative result.              */
    return(ERROR);

  switch (types[type_index]) {
    case NO_TYPE:
      /* No more types, this definitely is a wrong syntax.    */
      return(ERROR);
    case NUMBER:
      /* The NUMBER type is set by ScanWordTable() when it    */
      /* detects a numstring.                                 */
      switch (state) {
        case 2:         /* cardinal, as in 'set dial to 1234' */
          state = 23;   /* or 'get 3 pebbles'                 */
          break;
        case 8:         /* cardinal, as in 'set dial to 1234' */
          state = 22;   /* or 'get 3 pebbles'                 */
          break;
        case 4:         /* ordinal, as in 'get pebble 3'      */
          state = 24;
          break;
        case 9:         /* ordinal, as in 'get pebble 3'      */
          state = 25;
          break;
        case 14:        /* ordinal, as in 'get pebble 3'      */
          state = 26;
          break;
        case 17:        /* ordinal, as in 'get pebble 3'      */
          state = 27;
          break;
        case 21:        /* ordinal, as in 'get pebble 3'      */
          state = 28;
          break;
        default:
          return(ERROR);
      }
      /* It is not necessary to use new_types here. */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* Fill action_rec, either value or indexnum. */
        switch (state) {
            case 2: ;
            case 8:
              /* cardinal number, store in value field */
              action_rec->value = id;
              break;
            default:
              /* ordinal number, store in ordinal field */
              action_rec->ordinal = id;
        }
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Don't need to do this, just want to be consistent. */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));}
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
                             ++type_index, state, action_rec));
      }
      /* For now, assume it's a verb                     */
      /* See if rest of input matches if this is a verb. */
      /* -1, NULL params will be read by called function */
      /* NO_ID tells CheckSyntax to read next word from  */
      /* line_buf.                                       */

      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* Fill action_rec. */
        action_rec->action = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD) {
        return(UNKNOWN_WORD);}
      else /* error */ {
        /* Oops! it wasn't a verb after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));}
    case DIRECTIONS:
      switch (state) {
        case  1: ;
        case  2: ; /* same as  3 */
        case  3: ; /* same as  4 */
        case  4: ; /* same as  8 */
        case  8: ; /* same as 18 */
        case 18: ;
        case 24:   /* 26 dec 2015 */
          state = 16;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, state, action_rec));
      } /* switch */

      /* OK, assume it's a direction */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* Yes, it was a direction. */
        /* Fill action_rec.       */
        if (old_state == 1 || old_state == 18)
          /* In this state, we have to add the verb ourselfs. */
          /* I.e. "n" is parsed as "go north". If this        */
          /* syntaxcheck fails (not a direction after all),   */
          /* restore the action to NO_ID.                     */
          action_rec->action = LookUpId("go");
        action_rec->direction = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a direction after all; try again */
        /* Check to see if we must restore action.          */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));}

    case ADVERBS:
      switch (state) {
        case  2: ;
        case 12:
          state = 11;
          break;
        case 4: ;
        case 7: ;
        case 24:    /* 26 dec 2015 */
          state = 6;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, state, action_rec));
      }
      /* assume it's an adverb */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* fill action_rec. */
        /* CAUTION!! If MAX_PARSE_ADVERBS has been exceeded, */
        /* we just skip this adverb.                         */
        if (action_rec->adverbs.nr_of_adverbs != MAX_PARSE_ADVERBS)
          action_rec->adverbs.adverb[action_rec->adverbs.
                                                nr_of_adverbs++] = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't an adverb after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));}
    case PREPOSITIONS:
    case CONNECT_PREPOSITIONS:  /* same type as PREPOSITIONS */
      switch (state) {
        case 2:
          state = 3;
          break;
        case  4: ;
        case  6: ;
        case  8: ;
        case 10: ;
        case 14: ;
        case 24: ;    /* 26 dec 2015 */
        case 26:      /* 26 dec 2015 */
          state = 8;
          break;
        case 11:
          state = 13;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, state, action_rec));
      }

      /* assume it's a preposition */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* fill action_rec. */
        /* CAUTION!! If MAX_PARSE_PREPOS has been exceeded, */
        /* we just skip this preposition.                   */
        if (action_rec->prepositions.nr_of_prepositions != MAX_PARSE_PREPOS)
          action_rec->prepositions.preposition[action_rec->
                             prepositions.nr_of_prepositions++] = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else {
        /* Oops! it wasn't a preposition after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));}

    case COMMA:
      switch (state) {
        case 4: ;
        case 24:    /* 26 dec 2015 */
          state = 5;
          break;
        case 9: ;
        case 25:    /* 26 dec 2015 */
          state = 10;
          break;
        case 14: ;
        case 26:    /* 26 dec 2015 */
          state = 15;
        case 17: ;
        case 27 :   /* 26 dec 2015 */
          state = 18;
          break;
        default:
          /* error */
          ErrHdr();
          printf("There is a comma in the wrong place.\n");
          return(ERROR);
      }
      /* OK to use types here rather than new_types. */
      return(CheckSyntax(line_buf, NO_ID, nr_of_types, types,
                         -1,state, action_rec));
    case CONJUNCTION:
      switch (state) {
        case 11:
          state = 12;
          break;
        case 4: ;
        case 24:   /* 26 dec 2015 */
          state = 5;
          break;
        case 6:
          state = 7;
          break;
        case 9: ;
        case 25:   /* 26 dec 2015 */
          state = 10;
          break;
        case 14: ;
        case 26:   /* 26 dec 2015 */
          state = 15;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, state, action_rec));
      }
      /* assume it's a conjunction */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* fill the action record */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a conjunction after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));}
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
                             ++type_index, state, action_rec));
      }
      /* assume it's a question word */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* fill action_rec */
        action_rec->q_word = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a question word after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));}
    case ARTICLES:
      /* 16aug2015, code changed to allow to skip articles. */
      /* Filter out the article, don't do anything with it. */
      switch (state) {
        case 2:
        case 3:
        case 5:
        case 8:
        case 10:
        case 13:
        case 15:
        case 20:
          /* Do nothing, just skip the article. */
          break;
        default:
          /* Wrong syntax, try next type.       */
          return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));
      }
      /* assume it's an article */
      result = CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* fill the action record */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't an article after all; try again */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));}
    case NOUNS:
        /* It shouldn't be a noun, try next type.               */
        /* They may only refer to objects and locations by %x_  */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));
    case ADJECTIVES:
        /* It shouldn't be an adjective, try next type.         */
        return(CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));
    default:
      ErrHdr();
      printf("CheckSyntax(): unknown type keyword %d\n", types[type_index]);
      return(ERROR);
  } /* switch */
  /* no return here */
}


int32_t ParseSentence(line_buf, action_rec)
 char        *line_buf;
 actionRec   *action_rec;
{
  int32_t         nr_of_types = 0;
  int32_t         i;
  int32_t         types[MAX_TYPES];
  int32_t         result;

  /* Initialize types array */
  for (i=0; i<MAX_TYPES; i++)
    types[i] = NO_TYPE;

  result = CheckSyntax(line_buf, NO_ID, nr_of_types,
                       types, -1, 1, action_rec);
  if (result == ERROR) {
    ErrHdr();
    printf("\nLine %d: syntax error in string\n", line_num);
    printf("Use [l_xx] [o_xx] [dir] etc to refer to XVAN concepts\n");
    printf("Nouns, adjectives etc don't work here.\n");
  }
  return(result);
}
