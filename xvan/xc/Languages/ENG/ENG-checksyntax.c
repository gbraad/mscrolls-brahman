
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
#include "ENG-checksyntax.h"


/**********************************************************************/
/* This module is used to parse and translate strings that are used   */
/* to determine which trigger to execute for which user commands.     */
/* These sentences are located in the source file in the TRIGGERS     */
/* clause. Example: "take [o_lamp]" -> t_take.                        */
/* It is IMPORTANT that locations and objects that are used in the    */
/* sentences are referred to by their id and NOT by their system      */
/* description. The word `lamp' will be stored as its word id and     */
/* NOT as its object id, whereas o_lamp will.                         */
/* First, the syntax of the sentence is checked by ENG_CheckSyntax(). */
/* This is a simplified version of the CheckSyntax as used by the     */
/* interpreter, since it doesn't have to parse system descryptions.   */
/* CheckSyntax returns an action_rec, which will be stored in the     */
/* storyfile (it doesn't create a parsed_input struct, like the       */
/* interpreter's version of CheckSyntax does.                         */
/* The NextWordId() function as used by the interpreter cannot be     */
/* used here, since there may be [l_xxxx], [o_xxxx] and words in      */
/* the sentence, which cannot be handled correctly by the             */
/* interpreter's function.                                            */
/**********************************************************************/


/*************************/
/* Function Declarations */
/*************************/

int32_t ENG_CheckSyntax(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, actionRec*);


/************************/
/* Function definitions */
/************************/


int32_t ENG_CheckSyntax(char *line_buf, int32_t id, int32_t nr_of_types, int32_t *types, int32_t type_index, int32_t state, actionRec *action_rec)
 /* id, nr_of_types, type_index and state cannot be changed by      */
 /* recursive calls. The rest of the parameters can and therefore   */
 /* we will define duplicates when needed.                          */
{
  int32_t  result;               /* needed to test for unknown words and */
                                 /* stop recursive calling               */
  int32_t  i = 0;
  int32_t  id1 = NO_ID;
  int32_t  id2 = NO_ID;
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
    if ( (id = GiveNextId(&line_buf, &id1, &id2, &nr_of_types, types)) == NO_ID) {
      /* it may be a parameter */
      /* word ids are returned as function result. */
      /* parameters are returned in id1 and id2    */
      if (id1 != NO_ID) {
        /* id2 must be NO_ID */
        if (id2 != NO_ID) {
          return(ERROR);
        }
        id = id1;
      }
      else {
        /* unknown word */
        return(UNKNOWN_WORD);
      }
    }
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

      return(ENG_CheckSyntax(line_buf, NO_ID, 0, types, -1, state, action_rec));
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
      return(ENG_CheckSyntax(line_buf, NO_ID, 0, types, -1, state,
                                                           action_rec));
    } /* if */

    if (id == DIR) {  /* [dir] in sentence */
      switch (state) {
        case  1: ;
        case 18:
          action_rec->action1 = LookUpId("go");  /* No error checking? */
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

      return(ENG_CheckSyntax(line_buf, NO_ID, 0, types, -1, state,
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
      action_rec->action1 = ACTION;

      return(ENG_CheckSyntax(line_buf, NO_ID, 0, types, -1, state,
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

      return(ENG_CheckSyntax(line_buf, NO_ID, 0, types, -1, state,
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

      return(ENG_CheckSyntax(line_buf, NO_ID, 0, types, -1, state,
                                                           action_rec));
    } /* if */

    if (id == EOS)   /* End Of String */
      switch (state) {
        case  2: ;
        case  3: ;   /* nov 19 2017 */
        case  4: ;
        case  6: ;
        case  8: ;   /* nov 19 2017 */
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
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* Fill action_rec, either value or indexnum. */
        switch (state) {
            case 22: ;  /* 20oct2017 changed from 8 to 22 */
            case 23:    /* 20oct2017 changed from 2 to 23 */
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
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, state, action_rec));
      }
      /* For now, assume it's a verb                     */
      /* See if rest of input matches if this is a verb. */
      /* -1, NULL params will be read by called function */
      /* NO_ID tells CheckSyntax to read next word from  */
      /* line_buf.                                       */

      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* Fill action_rec. */
        action_rec->action1 = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD) {
        return(UNKNOWN_WORD);}
      else /* error */ {
        /* Oops! it wasn't a verb after all; try again */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, state, action_rec));
      } /* switch */

      /* OK, assume it's a direction */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* Yes, it was a direction. */
        /* Fill action_rec.       */
        if (old_state == 1 || old_state == 18)
          /* In this state, we have to add the verb ourselfs. */
          /* I.e. "n" is parsed as "go north". If this        */
          /* syntaxcheck fails (not a direction after all),   */
          /* restore the action to NO_ID.                     */
          action_rec->action1 = LookUpId("go");
        action_rec->direction = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a direction after all; try again */
        /* Check to see if we must restore action.          */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
        case 21:    /*10feb19: for 'why am I here' */
          state = 28;
          break;
        default:
          /* wrong syntax; try again with next type */
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, state, action_rec));
      }
      /* assume it's an adverb */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
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
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));}    /* yes, it's a parameter. */
    /* now check if it's a location id, object id or THIS */
    if (IsLocId(id1) || IsObjId(id1) || id1 == THIS) {
      /* if id2 is not NO_ID, id1 was an owner of a flag, an   */
      /* attribute or a description, which we don't allow here */
      if (id2 != NO_ID) {
        /* error */
        ErrHdr();
        printf("\nLine %d: error in string, unexpected id: %d %d.\n", line_num, id1, id2);
        return(NO_ID);
      }
    }
    case PREPOSITIONS:
    case CONNECT_PREPOSITIONS:  /* same type as PREPOSITIONS */
      switch (state) {
        case  2: ;
        case 23:    /* 19 nov 2017 */
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, state, action_rec));
      }

      /* assume it's a preposition */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
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
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
      return(ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, state, action_rec));
      }
      /* assume it's a conjunction */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);    /* yes, it's a parameter. */
    /* now check if it's a location id, object id or THIS */
    if (IsLocId(id1) || IsObjId(id1) || id1 == THIS) {
      /* if id2 is not NO_ID, id1 was an owner of a flag, an   */
      /* attribute or a description, which we don't allow here */
      if (id2 != NO_ID) {
        /* error */
        ErrHdr();
        printf("\nLine %d: error in string, unexpected id: %d %d.\n", line_num, id1, id2);
        return(NO_ID);
      }
    }
      if (result == OK) {
        /* fill the action record */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a conjunction after all; try again */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, state, action_rec));
      }
      /* assume it's a question word */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
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
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));
      }
      /* assume it's an article */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, state, action_rec);
      if (result == OK) {
        /* fill the action record */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't an article after all; try again */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));}
    case NOUNS:
        /* It shouldn't be a noun, try next type.               */
        /* They may only refer to objects and locations by %x_  */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));
    case ADJECTIVES:
        /* It shouldn't be an adjective, try next type.         */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, old_state, action_rec));
    default:
      ErrHdr();
      PrintError(80, &((resultStruct){VALUE, types[type_index]}), NULL);
      return(ERROR);
  } /* switch */
  /* no return here */
}
