
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
#include <stdlib.h>   /* malloc()  */

#ifndef __APPLE__
#include <malloc.h>   /* _alloca() */
#endif

#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "play.h"
#include "defs.h"

<<<<<<< HEAD
=======
#include "ifiglue.h"


>>>>>>> 72d7449e33257b77bc124b16a988a408eddcf5b1
/*************************/
/* Function declarations */
/*************************/

int32_t Play(char*);

/************************/
/* Function definitions */
/************************/

int32_t Play(char *user_input)
{
  static parsedInput  parsed_input;
  static usrActionRec action_rec;
  static int32_t      parse_syntax = LINE;  /* Tells what syntax to parse. */
  int32_t             subject_index = 0;    /* Tells which subject to use  */
                                            /* from action_rec.            */

  int32_t* cont_list = _alloca((nr_of_locs+nr_of_objs)*sizeof(int32_t));

  static int32_t      parse_result; /* Needed to process result of ParseInput(). */
<<<<<<< HEAD
  static resultStruct translate_result = {OK, 0};
=======
  static resultStruct translate_result = {OK, NONE, 0};
>>>>>>> 72d7449e33257b77bc124b16a988a408eddcf5b1
  static resultStruct arec_result;
  static resultStruct prologue_result;
  static resultStruct verb_def_result;


  /* 17dec2016: added nr_of_subjects because introducing the parser  */
  /* rules makes it no longer possible to match all subjects at once */
  /* in trnslate.c. The parser rules may need to relate subject to   */
  /* specifier for disambiguation, so each subject must bne matched  */
  /* individually now under control of letsplay().                   */

  int32_t nr_of_subjects = 0;

  /* flush outputline */
  Output();

  /* Reset subject_index. */
  subject_index = 0;

  if (parse_syntax == LINE) {
    /* Fill the parsed input record with default values. */
    InitParsedInput(&parsed_input);

    /* Fill the action record with default values.       */
    InitUsrActionRec(&action_rec);
  }
  /* output must start with a capital letter */
  capital = 1;

  /* Parse the input. */
  parse_result = ParseInput(user_input, &parsed_input, parse_syntax);
  /* possible return values: OK, ERROR, UNKNOWN_WORD */
  if (parse_result == ERROR && parse_syntax != LINE) {
    /* The user was asked for a subject or specifier, but */
    /* parsing his response failed. He may have entered a */
    /* next line. Parse again, but now with LINE syntax.  */
    /* If parse error, skip rest of while loop.           */

    /* Fill the parsed input record with default values. */
    InitParsedInput(&parsed_input);

    /* Fill the action record with default values.        */
    InitUsrActionRec(&action_rec);

    parse_result = ParseInput(user_input, &parsed_input, LINE);
  }

  /* Reset parse_syntax. */
  parse_syntax = LINE;

  if (parse_result == OK) {

    /* Clear user_input. */
    user_input[0] = '\0';

    /* 17dec2016: we must translate for each subject            */
    /* individually because we have introduced the parser rules */
    /* where a subject may be checked against the specifier     */
    nr_of_subjects = CountSubjectsInParsedInput(&parsed_input);

    /* if more than 1 subject we have to go through them       */
    /* backwards, because of the exteded system description    */
    /* e.g. give rope, bone to fred won't work if we don't     */
    /* start with 'bone to fred' as the first subject          */
    subject_index = (nr_of_subjects == 0 ? 0 : nr_of_subjects-1);

    /* 17dec2016: added iteration through subjects (in case more */
    /* more than 1 subject was entered by the player). Iteration */
    /* used to be done by Translate(), but with the introduction */
    /* of parse rules the iteration had to be taken out of       */
    /* Translate() because of possible parse dependencies        */
    /* between subject and specifier.                            */

    /* number of subjects may be zero, but we must go through */
    /* the loop at least once (e.g. "look", "north", ...)     */

    do {
      translate_result = Translate(&parsed_input, subject_index, &action_rec, user_input);

      /* action_rec may contain the 'it' object. We must replace */
      /* it by the actual value before we can start comparing    */
      if (!ReplaceItObjects(&action_rec)) {
        /* error already printed and flushed */
        return(ERROR);
      }

      switch (translate_result.tag) {
        case OK:
          /* Cannot use 'while' here, because we must go through */
          /* the loop if the first subject equals NO_ID. If one  */
          /* of the other subjects equals NO_ID, we must stop.   */

          /* Successful translation.                  */
          /* Now execute the verb's prologue, if any. */

          /* Set the global subject variable. */
          subject = action_rec.subject[subject_index];

<<<<<<< HEAD
            prologue_result = XeqPrologue(action_rec.action1);
            switch (prologue_result.tag) {
            case QUIT:
              /* exit */
              return(QUIT);
            case DISAGREE:
              /* MUST WE EXECUTE THE EPILOGUE HERE ??? */
              /* Forget the whole thing, continue with */
              /* next user input.                      */

              /* Check if we must handle the timers. */
              if (CheckDoTimers()) {
                if (HandleTimers(&action_rec, subject_index) == QUIT)
                  return(QUIT);
              }
              break;
            case GET_SUBJECT:
                /* The subject wasn't specified in the input.  */
                /* prologue_result.value may contain a word id */
                /* that must be copied to the prepositions in  */
                /* the parsed_input struct                     */
                if (prologue_result.value != NO_ID) {
                   if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                    /* room for an extra preposition*/
                    parsed_input.prepositions.preposition[parsed_input.
                         prepositions.nr_of_prepositions++] = prologue_result.value;
                  }
                }
                parse_syntax = SUBJECT;
                break;
            case GET_SPECIFIER:
                /* The specifier wasn't specified in the input. */
                if (prologue_result.value != NO_ID) {
                   if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                    /* room for an extra preposition*/
                    parsed_input.prepositions.preposition[parsed_input.
                         prepositions.nr_of_prepositions++] = prologue_result.value;
                  }
                }
                parse_syntax = SPECIFIER;
                break;
            case GET_ANSWER:
              /* We need an answer from the player. */
              parse_syntax = ANSWER;
              break;
            default:
              /* Either AGREE or NO_MATCH. */
              /* Build list with contained objs for curr_loc. */
              if (!ContList(curr_loc, cont_list, parsed_input.scope))
                return(ERROR); /* actually, is an error */

              /* Execute action_rec for list. */
                arec_result = XeqActionRec(&action_rec, cont_list, subject_index);
                switch (arec_result.tag) {
                case NO_MATCH:
                  /* No match; execute the default verb code. */
                    verb_def_result = XeqVerbDefault(&action_rec, subject_index);
                    switch (verb_def_result.tag) {
=======
          prologue_result = XeqPrologue(action_rec.action1);
          Output();
          switch (prologue_result.tag) {
          case QUIT:
            /* exit */
            return(QUIT);
          case DISAGREE:

Log ("Prologue returned DISAGREE\n", "", "");

            /* MUST WE EXECUTE THE EPILOGUE HERE ??? */
            /* Forget the whole thing, continue with */
            /* next user input.                      */

            /* Check if we must handle the timers. */
            if (CheckDoTimers()) {
              if (HandleTimers(&action_rec, subject_index) == QUIT) {
                Output();
                return(QUIT);
              }
              Output();
            }
            break;
          case GET_SUBJECT:

Log ("Prologue returned GET_SUBJECT\n", "", "");

              /* The subject wasn't specified in the input.  */
              /* prologue_result.value may contain a word id */
              /* that must be copied to the prepositions in  */
              /* the parsed_input struct                     */
              if (prologue_result.value != NO_ID) {
                 if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                  /* room for an extra preposition*/
                  parsed_input.prepositions.preposition[parsed_input.
                       prepositions.nr_of_prepositions++] = prologue_result.value;
                }
              }
              parse_syntax = SUBJECT;
              break;
          case GET_SPECIFIER:

Log ("Prologue returned GET_SPECIFIER\n", "", "");

              /* The specifier wasn't specified in the input. */
              if (prologue_result.value != NO_ID) {
                 if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                  /* room for an extra preposition*/
                  parsed_input.prepositions.preposition[parsed_input.
                       prepositions.nr_of_prepositions++] = prologue_result.value;
                }
              }
              parse_syntax = SPECIFIER;
              break;
          case GET_ANSWER:
            /* We need an answer from the player. */
            parse_syntax = ANSWER;
            break;
          default:
            /* Either AGREE or NO_MATCH. */
            /* Build list with contained objs for curr_loc. */
            if (!ContList(curr_loc, cont_list, parsed_input.scope))
              return(ERROR); /* actually, is an error */

            /* Execute action_rec for list. */
              arec_result = XeqActionRec(&action_rec, cont_list, subject_index);
              Output();
              switch (arec_result.tag) {
              case NO_MATCH:
                /* No match; execute the default verb code. */
                  verb_def_result = XeqVerbDefault(&action_rec, subject_index);
                  Output();
                  switch (verb_def_result.tag) {
>>>>>>> 72d7449e33257b77bc124b16a988a408eddcf5b1
                    case QUIT:
                      /* Stop */
                      return(QUIT);
                    case GET_SUBJECT:
<<<<<<< HEAD
                        /* The subject wasn't specified in the input. */
                        /* verb_def_result.value may contain a word id  */
                        /* that must be copied to the prepositions in   */
                        /* the parsed_input struct                      */
                        if (verb_def_result.value != NO_ID) {
                          if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                            /* room for an extra preposition*/
                            parsed_input.prepositions.preposition[parsed_input.
                                 prepositions.nr_of_prepositions++] = verb_def_result.value;
                          }
                        }
                        parse_syntax = SUBJECT;
                        break;
                    case GET_SPECIFIER:
                        /* The specifier wasn't specified in the input. */
                        /* verb_def_result.value may contain a word id  */
                        /* that must be copied to the prepositions in   */
                        /* the parsed_input struct                      */ PrintString("Hoi", 0);
                        if (verb_def_result.value != NO_ID) {
                          if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                            /* room for an extra preposition*/
                            parsed_input.prepositions.preposition[parsed_input.
                                 prepositions.nr_of_prepositions++] = verb_def_result.value;
                          }
                        }
                        parse_syntax = SPECIFIER;
                        break;
=======
                      /* The subject wasn't specified in the input. */
                      /* verb_def_result.value may contain a word id  */
                      /* that must be copied to the prepositions in   */
                      /* the parsed_input struct                      */
                      if (verb_def_result.value != NO_ID) {
                        if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                          /* room for an extra preposition*/
                          parsed_input.prepositions.preposition[parsed_input.
                               prepositions.nr_of_prepositions++] = verb_def_result.value;
                        }
                      }
                      parse_syntax = SUBJECT;
                      break;
                    case GET_SPECIFIER:
                      /* The specifier wasn't specified in the input. */
                      /* verb_def_result.value may contain a word id  */
                      /* that must be copied to the prepositions in   */
                      /* the parsed_input struct                      */ PrintString("Hoi", 0);
                      if (verb_def_result.value != NO_ID) {
                        if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                          /* room for an extra preposition*/
                          parsed_input.prepositions.preposition[parsed_input.
                               prepositions.nr_of_prepositions++] = verb_def_result.value;
                        }
                      }
                      parse_syntax = SPECIFIER;
                      break;
>>>>>>> 72d7449e33257b77bc124b16a988a408eddcf5b1
                    case GET_ANSWER:
                      /* We need an answer from the player. */
                      parse_syntax = ANSWER;
                      break;
                    default:
                      /* Either AGREE, DISAGREE OR NO_MATCH.   */
                      /* 15jun2019 - added NO_MATCH to default */
                      /* it first had its own branch, but did  */
                      /* not execute the epilogue.             */

                      XeqEpilogue(action_rec.action1);
                      Output();
                      /* Check if we must handle the timers. */
                      if (CheckDoTimers()) {
                        if (HandleTimers(&action_rec, subject_index) == QUIT) {
                          Output();
                          return(QUIT);
                        }
                        Output();
                      }
                      break;
                  } /* switch XeqVerbDefault() */
<<<<<<< HEAD
                  break;
                case QUIT:
                  /* Stop */
                  /*scanf("%c", &ch);*/
                  return(QUIT);
                case GET_SUBJECT:
                    /* The subject wasn't specified in the input. */
                    /* arec_result.value may contain a word id that */
                    /* must be copied to the prepositions in the    */
                    /* parsed_input struct                          */
                    if (arec_result.value != NO_ID) {
                      if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                        /* room for an extra preposition*/
                        parsed_input.prepositions.preposition[parsed_input.
                             prepositions.nr_of_prepositions++] = arec_result.value;
                      }
                    }
                    parse_syntax = SUBJECT;
                    break;
                case GET_SPECIFIER:
                    /* The specifier wasn't specified in the input. */
                    /* arec_result.value may contain a word id that */
                    /* must be copied to the prepositions in the    */
                    /* parsed_input struct                          */
                    if (arec_result.value != NO_ID) {
                      if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                        /* room for an extra preposition*/
                        parsed_input.prepositions.preposition[parsed_input.
                             prepositions.nr_of_prepositions++] = arec_result.value;
                      }
                    }
                    parse_syntax = SPECIFIER;
                    break;
                case GET_ANSWER:
                    /* We need an answer from the player. */
                  parse_syntax = ANSWER;
                  break;
                default:
                  /* AGREE or DISAGREE     */
                  /* Execute the epilogue. */
                  XeqEpilogue(action_rec.action1);

                  /* Check if we must handle the timers. */
                  if (CheckDoTimers()) {
                    if (HandleTimers(&action_rec, subject_index) == QUIT)
                      return(QUIT);
=======
                break;
              case QUIT:
                /* Stop */
                /*scanf("%c", &ch);*/
                return(QUIT);
              case GET_SUBJECT:
                /* The subject wasn't specified in the input. */
                /* arec_result.value may contain a word id that */
                /* must be copied to the prepositions in the    */
                /* parsed_input struct                          */
                if (arec_result.value != NO_ID) {
                  if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                    /* room for an extra preposition*/
                    parsed_input.prepositions.preposition[parsed_input.
                        prepositions.nr_of_prepositions++] = arec_result.value;
>>>>>>> 72d7449e33257b77bc124b16a988a408eddcf5b1
                  }
                }
                parse_syntax = SUBJECT;
                break;
              case GET_SPECIFIER:
                /* The specifier wasn't specified in the input. */
                /* arec_result.value may contain a word id that */
                /* must be copied to the prepositions in the    */
                /* parsed_input struct                          */
                if (arec_result.value != NO_ID) {
                  if (parsed_input.prepositions.nr_of_prepositions != MAX_PARSE_PREPOS) {
                    /* room for an extra preposition*/
                    parsed_input.prepositions.preposition[parsed_input.
                         prepositions.nr_of_prepositions++] = arec_result.value;
                  }
                }
                parse_syntax = SPECIFIER;
                break;
              case GET_ANSWER:
                /* We need an answer from the player. */
                parse_syntax = ANSWER;
                break;
              default:
                /* AGREE or DISAGREE     */
                /* Execute the epilogue. */
                XeqEpilogue(action_rec.action1);
                Output();
                /* Check if we must handle the timers. */
                if (CheckDoTimers()) {
                  if (HandleTimers(&action_rec, subject_index) == QUIT) {
                    Output();
                    return(QUIT);
                  }
                  Output();
                }

                parse_syntax = LINE;
                break;
            } /* switch XeqActionRec() */
            break;
          } /* switch XeqPrologue() */
          break; /* Translate() OK */

        case OVERFLOW:
          /* More than MAX_HITS hits in the Find() function that */
          /* is called by Translate()                            */
          /* too many hits, use adjectives to narrow down */
          PrintError(45, NULL, NULL);
          break;

        case NEXT_SENTENCE:
          /* next sentence was entered */
          /* with console version we do nothing because it   */
          /* is in a loop and the new iinput will be         */
          /* processed automatically. But here we have no    */
          /* loop so we must call Play() again.              */

          /* check for QUIT in case of second call of Play() */
          /* (if not Play() will return to this same point   */
          /* and do nothing with the yes/no answer to quit() */
          if (Play(user_input) == QUIT) {
            return(QUIT);
          }
          break;

        case UNKNOWN_WORD:
          user_input[0] = '\0';
          break;

        case ERROR:
          /* error msg already printed */
          break;

        case ACTOR_ERROR:
          PrintNotFound(&(parsed_input.actor));
          break;

        case SPECIFIER_ERROR:
          PrintNotFound(&(parsed_input.specifier));
          break;

        case SUBJECT_ERROR:
          PrintNotFound(&(parsed_input.subject[translate_result.value]));
          break;
      } /* switch translate_result */
      subject_index--;
    } while (subject_index >= 0);
  } /* if (parse_result == OK)*/
  else {
    /* Parse error, reset line_buf. */
    /* Error msg (either unknown word or syntax error) has */
    /* already been printed .                              */
    if (parse_result != UNKNOWN_WORD) {
      /* unknown word error has already been printed by    */
      /* CheckSyntax().                                    */
      /* I don't understand that sentence */
      PrintError(46, NULL, NULL);
    }
    user_input[0] = '\0';
  }

  return(OK);
}
