
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
#include <stdlib.h>         /* malloc(), free() */
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "fndctrig.h"


/*************************/
/* function declarations */
/*************************/

int32_t IsSectionKeyword(int32_t);
int32_t SkipCommonTrigger(char**, int32_t*, FILE**, fileList**);
int32_t FindCTriggerIds(char**, int32_t*, int32_t, FILE**, fileList**);

/************************/
/* function definitions */
/************************/

int32_t IsSectionKeyword(int32_t kw)
{
  switch (kw) {
    case VERB:
    case REDEFINE_VERB:
    case DIRECTIONS:
    case NOUNS:
    case ADJECTIVES:
    case PREPOSITIONS:
    case ADVERBS:
    case ARTICLES:
    case Q_WORDS:
    case CONJUNCTION:
    case COMMON_DESCRS:
    case COMMON_FLAGS:
    case REDEFINE_FLAGS:
    case COMMON_ATTRS:
    case REDEFINE_ATTRS:
    case COMMON_TRIGGERS:
    case REDEFINE_TRIGGERS:
    case LOCATION:
    case OBJECT:
    case TIMERS:
      return(OK);
    default:
      return(ERROR);
  }
}


int32_t SkipCommonTrigger(char **word, int32_t *keyword, FILE **source, fileList **file_list)
{
  /* 10jan2018: this function is used to skip a common */
  /* trigger in the source text, when we found that    */
  /* the trigger is redefined.                         */

  int32_t state = 1; /* added one state prior to 1.  */

  /* *word will be a trigger id */
  free(*word);
  *word = GetNextWord(keyword, NO_ID, 0, source, file_list);

  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source, file_list)) {
          break;
        }
        ErrHdr();
        PrintError(47, NULL, NULL);
        return(ERROR);
      case ERROR:
        return(ERROR);
      case TRIGG_ID:
        if (state == 1) {
          /* id of the next trigger, ready with skipping the current one */
          return(OK);
        }
        else {
          /* trigger used as a parameter in the current trigger */
          state = 1;
          free(*word);
          *word = GetNextWord(keyword, NO_ID, 0, source, file_list);
        }
        break;
      case LOC_ID:   ;
      case OBJ_ID:   ;
      case LEFT_PAR: ;
      case DOT:      ;
      case COMMA:
        /* If we find a trigger id as the next word after this, */
        /* case it is used as a parameter in a function, not as */
        /* the start of a new trigger definition.               */
        state = 2;
        free(*word);
        *word = GetNextWord(keyword, NO_ID, 0, source, file_list);
        break;
      default:
        if (IsSectionKeyword(*keyword)) {
          return(OK);
        }
        else {
          /* not a word we were looking for */
          state = 1;
          free(*word);
          *word = GetNextWord(keyword, NO_ID, 0, source, file_list);
        }
        break;
    } /* switch */
  } /* while */
}


int32_t FindCTriggerIds(char **word, int32_t *keyword, int32_t redefine, FILE **source, fileList **file_list)
{
  /* 15oct2017 When a common trigger executes another common  */
  /* trigger that is defined later, the executed trigger will */
  /* be assigned a local trigger id by the compiler. This is  */
  /* because there is no reference to the common trigger that */
  /* is executed in the trigger id table. Because we do not   */
  /* want to enforce a certain order in the common trigger    */
  /* definitions we do - prior to parsing the common trigger  */
  /* code - a search through all COMMON_TRIGGERS sections to  */
  /* find the common ntrigger ids and declare them as         */
  /* referenced in the trigger id table.                      */

  /* 08jan2018: must also look for REDEFINE_TRIGGERS keywords  */

  int32_t state = 1; /* added one state prior to 1.  */
  int32_t id;        /* Not used, needed for GetTriggerId(). */

  /* *word will be COMMON_TRIGGERS. */
  free(*word);
  *word = GetNextWord(keyword, NO_ID, 0, source, file_list);

  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source, file_list)) {
          break;
        }
        ErrHdr();
        PrintError(47, NULL, NULL);
        return(ERROR);
      case ERROR:
        return(ERROR);
      case TRIGG_ID:
        if (state == 1) {
          state = 2;
          if (GetTriggerId((*word)+2, redefine, &id, COMMON_TRIGGERS, 1).tag == ERROR)
            /* set parsed to 1 to force GetTriggerId() */
            /* to make a struct in the trigger table   */
            return(ERROR);
        }
        else {
          free(*word);
          *word = GetNextWord(keyword, NO_ID, 0, source, file_list);
        }
        break;
      case LOC_ID:   ;
      case OBJ_ID:   ;
      case LEFT_PAR: ;
      case DOT:      ;
      case COMMA:
        /* If we find a trigger id as the next word after this, */
        /* case it is used as a parameter in a function, not as */
        /* the start of a new trigger definition.               */
        state = 2;
        free(*word);
        *word = GetNextWord(keyword, NO_ID, 0, source, file_list);
        break;
      default:
        if (IsSectionKeyword(*keyword)) {
          return(OK);
        }
        else {
          /* not a word we were looking for */
          state = 1;
          free(*word);
          *word = GetNextWord(keyword, NO_ID, 0, source, file_list);
        }
        break;
    } /* switch */
  } /* while */
}
