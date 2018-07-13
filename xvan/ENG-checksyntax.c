
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
#include <stdlib.h>  /* malloc(). realloc(), free() */
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "keyword.h"
#include "typedefs.h"
#include "ENG-checksyntax.h"


/*************************/
/* Function declarations */
/*************************/

int32_t  ENG_CheckSyntax(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, parsedInput*);

/************************/
/* Function definitions */
/************************/

int32_t ENG_CheckSyntax(line_buf, id, nr_of_types, types, type_index,
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
      case  3: ; /* 19 nov 2017 */
      case  6: ;
      case  8: ; /* 19 nov 2017 */
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
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
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
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* For now, assume it's a verb                     */
      /* See if rest of input matches if this is a verb. */
      /* -1, NULL params will be read by called function */
      /* NO_ID tells CheckSyntax to read next word from  */
      /* line_buf.                                       */

      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* Fill parsed_input. */
        parsed_input->action1 = id;
        parsed_input->scope  = verb_dir[id-FIRST_VERB_ID].scope;
        return(OK);
      }
      else if (result == UNKNOWN_WORD) {
        return(UNKNOWN_WORD);}
      else /* error */ {
        /* Oops! it wasn't a verb after all; try again */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      } /* switch */

      /* OK, assume it's a direction */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* Yes, it was a direction. */
        /* Fill parsed_input.       */
        if (old_state == 1 || old_state == 18)
          /* In this state, we have to add the verb ourselfs. */
          /* I.e. "n" is parsed as "go north". If this        */
          /* syntaxcheck fails (not a direction after all),   */
          /* restore the action to NO_ID.                     */
          parsed_input->action1 = LookUpId("go");
        parsed_input->direction = id;
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a direction after all; try again */
        /* Check to see if we must restore action.          */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's an adverb */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
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
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }

      /* assume it's a connect preposition */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
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
            Output(outputline, 0);
            return(ERROR);
            break;
        } /* switch */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else {
        /* Oops! it wasn't a connecting preposition after all; try again */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));
      }
      break;
    case PREPOSITIONS:
      switch (state) {
        case  2: ;
        case 33:      /* 19 nov 2017 for 'enter 1234 on dial' */
          state = 3;
          break;
        case  4: ;
        case  6: ;
        case  8: ;
        case 10: ;
        case 14: ;
        case 22: ;    /* 19 nov 2017 for 'turn machine on' */
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }

      /* assume it's a preposition */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
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
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
            Output(outputline, 0);
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
          break;      /* break seemed to be missing, added on dec 23rd 2015   */
        case 17: ;
        case 37:      /* added on dec 23 2015 to allow <numbers> after a noun */
        case 41:
          state = 18;
          break;
        default:
          /* error */
          sprintf(outputline, "There is a comma in the wrong place.\n");
          Output(outputline, 0);
          return(ERROR);
      }
      /* OK to use types here rather than new_types. */
      return(ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's a conjunction */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* fill the action record */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a conjunction after all; try again */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's a question word */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
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
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's an article */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
                           -1, subject_index, state, parsed_input);
      if (result == OK) {
        /* do nothing, articles are not needed in parsed_input */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't an article after all; try again */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's a noun */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
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
            Output(outputline, 0);
            return(ERROR);
            break;
        } /* switch */
        return(OK);
      }
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't a noun after all; try again */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
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
          return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, state,
                             parsed_input));
      }
      /* assume it's an adjective */
      result = ENG_CheckSyntax(line_buf, NO_ID, nr_of_types, new_types,
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
            Output(outputline, 0);
            return(ERROR);
            break;
        } /* switch */
        return(OK);
      } /* if */
      else if (result == UNKNOWN_WORD)
        return(UNKNOWN_WORD);
      else /* error */ {
        /* Oops! it wasn't an adjective after all; try again */
        return(ENG_CheckSyntax(line_buf, id, nr_of_types, types,
                             ++type_index, subject_index, old_state,
                             parsed_input));}
    default:
      sprintf(outputline, "INTERNAL ERROR. unknown keyword type: %d.\n", types[type_index]);
      Output(outputline, 0);
      return(ERROR);
  } /* switch */
  /* no return here */
}



