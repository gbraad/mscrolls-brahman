
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>   /* malloc() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "letsplay.h"

/***********/
/* Globals */
/***********/

/* these must be defined as externals in the header files of the */
/* modules that need them.                                       */

short debug_mode  = 0;
short testmode    = 0;            /* read input from file is off  */
short transcript  = 0;            /* copy output to file is off   */

int32_t active_entity = NO_ID;    /* Location or object that is   */
                                  /* currently active (especially */
                                  /* for use with common          */
                                  /* triggers as 'this').         */
int32_t curr_loc      = NO_ID;    /* Current location.            */
int32_t direction     = NO_ID;    /* Current direction.           */
int32_t prepos        = NO_ID;    /* Last preposition.            */
int32_t action        = NO_ID;    /* Current action.              */
int32_t actor         = PLAYER;   /* Current actor.               */
int32_t subject       = NO_ID;    /* Current subject(s).          */
int32_t specifier     = NO_ID;    /* Current specifier.           */
int32_t value         = 0;        /* Current value.               */
int32_t ordinal       = 0;        /* Current ordinal value.       */

int32_t indent        = 0;        /* For indentation.                          */
int32_t capital       = 1;        /* Start printing with a capital character.  */
int32_t article       = 0;        /* Tells whether to print an article.        */


/*************************/
/* Function declarations */
/*************************/

void    PrintNotFound(extendedSysDescr*);
int32_t LetsPlay(void);


/************************/
/* Function definitions */
/************************/

int32_t LetsPlay()
{
  char         line_buf[INPUT_LINE_LEN];
  parsedInput  parsed_input;
  usrActionRec action_rec;
  int32_t      parse_syntax = LINE;  /* Tells what syntax to parse. */
  int32_t      subject_index;        /* Tells which subject to use  */
                                     /* from action_rec.            */
  int32_t      cont_list[nr_of_locs+nr_of_objs];

  int32_t      parse_result; /* Needed to process result of ParseInput(). */
  resultStruct translate_result = {OK, OK};
  int32_t      i = 0;


  /* 17dec2016: added nr_of_subjects because introducing the parser  */
  /* rules makes it no longer possible to match all subjects at once */
  /* in trnslate.c. The parser rules may need to relate subject to   */
  /* specifier for disambiguation, so each subject must bne matched  */
  /* individually now under control of letsplay().                   */
  int32_t      nr_of_subjects = 0;

  /* Print interpreter opening message. */
  for (i=0; i<25; i++)
    printf("\n");

  PrintString("-- XVAN 2.3 interpreter --\n\n");
  PrintString("(c) Marnix van den Bos\n\n\n\n");

  /* %s doesn't work with PrintString() */
  sprintf(outputline, "%s", story_info.title);
  Output(outputline);

  sprintf(outputline, " version %s\n\n\n\n", story_info.version);
  Output(outputline);

  /* Init current location. */
  curr_loc = obj_dir[PLAYER-FIRST_OBJECT_ID].held_by;

  /* Init line_buf. */
  line_buf[0] = '\0';

  /* Handle the timers to allow starting of the game */
  /* (printing opening messages etc).                */
  if (HandleTimers() == QUIT)
    return(OK);

  while (1) {
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

    if (strlen(line_buf) == 0) {
      /* Read the user's input */
      GetInput(line_buf, "> ");
    }

    /* Parse the input. */
    parse_result = ParseInput(line_buf, &parsed_input, parse_syntax);

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

      parse_result = ParseInput(line_buf, &parsed_input, LINE);
    }
    /* Reset parse_syntax. */
    parse_syntax = LINE;

    if (parse_result == OK) {

      /* Clear line_buf. */
      line_buf[0] = '\0';

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
        translate_result = Translate(&parsed_input, subject_index, &action_rec, line_buf);

        switch (translate_result.tag) {
          case OK:
            /* Cannot use 'while' here, because we must go through */
            /* the loop if the first subject equals NO_ID. If one  */
            /* of the other subjects equals NO_ID, we must stop.   */

            /* Successful translation.                  */
            /* Now execute the verb's prologue, if any. */

            /* Set the global subject variable. */
            subject = action_rec.subject[subject_index];

            switch (XeqPrologue(action_rec.action)) {
              case QUIT:
                /* exit */
                return(OK);
              case DISAGREE:
                /* MUST WE EXECUTE THE EPILOGUE HERE ??? */
                /* Forget the whole thing, continue with */
                /* next user input.                      */

                /* Check if we must handle the timers. */
                if (CheckDoTimers(action_rec.action)) {
                  if (HandleTimers() == QUIT)
                    return(OK);
                }
                break;
              case GET_SUBJECT:
                /* The subject wasn't specified in the input. */
                parse_syntax = SUBJECT;
                break;
              case GET_SPECIFIER:
                /* The specifier wasn't specified in the input. */
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
                  return(OK); /* actually, is an error */

                /* Execute action_rec for list. */
                switch (XeqActionRec(&action_rec, cont_list, subject_index)) {
                  case NO_MATCH:
                    /* No match; execute the default verb code. */
                    switch (XeqVerbDefault(&action_rec, subject_index)) {
                      case QUIT:
                        /* Stop */
                        return(OK);
                      case GET_SUBJECT:
                        /* The subject wasn't specified in the input. */
                        parse_syntax = SUBJECT;
                        break;
                      case GET_SPECIFIER:
                        /* The specifier wasn't specified in the input. */
                        parse_syntax = SPECIFIER;
                        break;
                      case GET_ANSWER:
                        /* We need an answer from the player. */
                        parse_syntax = ANSWER;
                        break;
                      case NO_MATCH:
                        /* The author of the story didn´t */
                        /* create default code.           */
                        /* 20apr2016: removed following statements when */
                        /* porting Bronze (see DEFAULT for verb listen) */

                        /*sprintf(outputline, "No comprendo\n"); */
                        /*Output(outputline); */
                        break;
                      default:
                        /* Either AGREE or DISAGREE. */
                        XeqEpilogue(action_rec.action);

                       /* Check if we must handle the timers. */
                        if (CheckDoTimers(action_rec.action)) {
                          if (HandleTimers() == QUIT)
                            return(OK);
                        }
                        break;
                    } /* switch XeqVerbDefault() */
                    break;
                  case QUIT:
                    /* Stop */
                    /*scanf("%c", &ch);*/
                    return(OK);
                  case GET_SUBJECT:
                    /* The subject wasn't specified in the input. */
                    parse_syntax = SUBJECT;
                    break;
                  case GET_SPECIFIER:
                    /* The specifier wasn't specified in the input. */
                    parse_syntax = SPECIFIER;
                    break;
                  case GET_ANSWER:
                    /* We need an answer from the player. */
                    parse_syntax = ANSWER;
                    break;
                  default:
                    /* AGREE or DISAGREE     */
                    /* Execute the epilogue. */
                    XeqEpilogue(action_rec.action);

                    /* Check if we must handle the timers. */
                    if (CheckDoTimers(action_rec.action)) {
                      if (HandleTimers() == QUIT)
                        return(OK);
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
            sprintf(outputline, "There are too many items that match your description.\n");
            Output(outputline);
            sprintf(outputline, "Try using some adjectives to narrow it down...\n");
            Output(outputline);
            break;
          case NEXT_SENTENCE:
            /* next sentence was entered, do nothing */
            break;
          case UNKNOWN_WORD:
            line_buf[0] = '\0';
            break;
          case ERROR:
            /* error msg already printed */
            break;
          case ACTOR_ERROR:
            PrintNotFound(&(parsed_input.actor));
            break;
          case SPECIFIER_ERROR:
            /* In case line_buf is not empty, the user was asked */
            /* additional information about a location or an     */
            /* object but he entered a new sentence instead.     */
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
        sprintf(outputline, "I don't understand that sentence.\n");
        Output(outputline);
      }
      line_buf[0] = '\0';
    }
  } /* while (1) */
  /* No return here, make compiler happy. */
}
