
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
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
#include <string.h>
#include <ctype.h>    /* toupper() */
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "ENG-article.h"

/*************************/
/* Function declarations */
/*************************/

void ENG_PrintArticle(sysDescr*, int32_t, int);

/************************/
/* Function definitions */
/************************/

void ENG_PrintArticle(sysDescr *descr, int32_t type, int use_json)
{
  int       i   = 0;
  int32_t   id  = NO_ID;
  wordTable *wt = word_table;

  if (descr->article != NO_ID) {
    /* there is an article, so check if we */
    /* must print something                */
    switch (article) {
      case ARTICLE_THE:
        /* clear article or else the white space will */
        /* not be printed by PrintString()            */
        article = 0;
        PrintWord(descr->article, use_json);
        PrintString(" ", use_json);
        break;
      case ARTICLE_A:
        /* We must print either 'a' or 'an'.             */
        /* Check if the next word starts with a vowel.   */
        /* Next word is either an adjective or the noun. */
        if ((i = descr->nr_of_adjectives-1) == -1) {
          /* there are no adjectives, we must have the noun */
          id = descr->noun;
          /* set i to 0 for indexing wt in next while loop */
          i = 0;
        }
        else
          id = descr->adjectives[i];

        /* Find word with id. */
        while (i<nr_of_words && wt[i].id != id)
          i++;
        if (i == nr_of_words) {
          /* Internal error; this shouldn't happen. */
          PrintError(50, &((resultStruct) {VALUE, NONE, id}), NULL);
        }
        else {
          /* clear article or else the white space will */
          /* not be printed by PrintString()            */
          article = 0;
          if (IsVowel( (wt[i].print_word)[0]))
            PrintString("an ", use_json);
          else
            PrintString("a ", use_json);
        }
        break;
      default:
        /* There is an article in the system description,  */
        /* but no %a or %the in the string, so don't print */
        /* the article.                                    */
        break;
    } /* switch */
  }
  /* july 6th 2028: added this because 'use' skipped the space */
  /* when the object had no noun. Must check if this is the    */
  /* right solution                                            */
  else  {
    article = 0;
  }
}
