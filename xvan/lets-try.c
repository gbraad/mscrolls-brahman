
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
#include "lets-try.h"
#include "defs.h"

/*************************/
/* Function declarations */
/*************************/

int32_t LetsTry(int32_t, usrActionRec);

/************************/
/* Function definitions */
/************************/

int32_t LetsTry(int32_t target, usrActionRec action_rec)
 /* action_rec must be executed for target */
{
  int32_t      subject_index = 0; /* We have only 1 subject */
  int32_t*     cont_list     = _alloca((nr_of_locs+nr_of_objs)*sizeof(int32_t));
  int32_t      scope;
  resultStruct result_prologue;
  resultStruct result_arec;
  resultStruct result_verb_default;

  /* This routine executes the action record from the  */
  /* try() function.                                   */
  /* NOTE: we cannot handle GetSubject() and GetSpec() */
  /* situations here. This is because we do not have   */
  /* the record with parsed input that must be         */
  /* augmented after GetSubject() and GetSpec()        */

  /* set scope */
  scope = verb_dir[action_rec.action1-FIRST_VERB_ID].scope;

  /* output must start with a capital letter */
  capital = 1;

  /* Set the global subject variable. */
  subject = action_rec.subject[subject_index];

  result_prologue = XeqPrologue(action_rec.action1);
  switch (result_prologue.tag)  {
    case QUIT:
      /* exit */
      return(QUIT);
      break;
    case DISAGREE:
      /* MUST WE EXECUTE THE EPILOGUE HERE ??? */
      /* Forget the whole thing, continue with */
      /* next user input.                      */

      /* As executing this action record is part of a */
      /* trigger, do not fire the timers yet          */
      return(DISAGREE);
      break;
    case GET_SUBJECT:
      /* The subject wasn't specified in the input. */
      PrintError(96, NULL, TranslateKeyword("SUBJECT"));
      /* not a severe enough error to quit?? */
      return(DISAGREE);
      break;
    case GET_SPECIFIER:
      /* The specifier wasn't specified in the input. */
      PrintError(96, NULL, TranslateKeyword("SPECIFIER"));
      return(DISAGREE);
      break;
    case GET_ANSWER:
      /* We need an answer from the player. */
      return(DISAGREE);
      break;
    default:
      /* Either AGREE or NO_MATCH. */
      /* Build list with contained objs for curr_loc. */
      if (!ContList(target, cont_list, scope))
         return(QUIT);
      /* Execute action_rec for list. */
      result_arec = XeqActionRec(&action_rec, cont_list, subject_index);
      switch (result_arec.tag)  {
        case NO_MATCH:
          /* No match; execute the default verb code. */
          result_verb_default = XeqVerbDefault(&action_rec, subject_index);
          switch (result_verb_default.tag)  {
            case QUIT:
              /* Stop */
              return(QUIT);
            case GET_SUBJECT:
              /* The subject wasn't specified in the input. */
              PrintError(96, NULL, TranslateKeyword("SUBJECT"));
              return(DISAGREE);
              break;
            case GET_SPECIFIER:
              /* The specifier wasn't specified in the input. */
              PrintError(96, NULL, TranslateKeyword("SPECIFIER"));
              return(DISAGREE);
              break;
            case GET_ANSWER:
              /* We need an answer from the player. */
              return(DISAGREE);
              break;
            case NO_MATCH:
              /* The author of the story didn´t */
              /* create default code.           */
              return(NO_MATCH);
              break;
            default:
              /* Either AGREE or DISAGREE. */
              XeqEpilogue(action_rec.action1);

              /* As executing this action record is part of a */
              /* trigger, do not fire the timers yet          */

              return(result_verb_default.tag);
              break;
          } /* switch XeqVerbDefault() */
          break;
        case QUIT:
          /* Stop */
          return(QUIT);
        case GET_SUBJECT:
          /* The subject wasn't specified in the input. */
          PrintError(96, NULL, TranslateKeyword("SUBJECT"));
          return(DISAGREE);
          break;
        case GET_SPECIFIER:
          /* The specifier wasn't specified in the input. */
          PrintError(96, NULL, TranslateKeyword("SPECIFIER"));
          return(DISAGREE);
          break;
        case GET_ANSWER:
          /* We need an answer from the player. */
          return(DISAGREE);
          break;
        default:
          /* AGREE or DISAGREE     */
          /* Execute the epilogue. */
          XeqEpilogue(action_rec.action1);

          /* As executing this action record is part of a */
          /* trigger, do not fire the timers yet          */

           return(result_arec.tag);
           break;
       } /* switch XeqActionRec() */
       return(result_prologue.tag);
       break;
   } /* switch XeqPrologue() */
}
