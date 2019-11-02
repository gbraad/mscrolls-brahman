
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
#include <ctype.h>

#include "keyword.h"
#include "typedefs.h"
#include "ENG-plural.h"

/*************************/
/* Function declarations */
/*************************/

int32_t ENG_CheckPlural(char*);

/************************/
/* Function definitions */
/************************/

int32_t ENG_CheckPlural(char *word)
{
  char    check_word[MAX_WORD_LEN+1];
  int     index       = 0;
  int32_t singular_id = NO_ID;

  /* this routine checks for an unknown word (i.e.  */
  /* not in the word table) if it is a plural form  */
  /* irregular plurality is always a word in word   */
  /* table, so of no concern here.                  */

  /* we take into account following plurality rules */
  /* word + es */
  /* word + s  */
  /* we must check "+ s" first, in case the single  */
  /* ends with an 'e'. E.g. if we check '+ es'      */
  /* first, "stables" would become "stabl".          */

  if (strlen(word) <3) {
    return(NO_ID);
  }

  /* first check 's' */
  index = strlen(word) - 1;

  if (word[index] == 's') {
    strncpy(check_word, word, index);
    check_word[index] = '\0';

    if ( (singular_id = GetSingularId(check_word)) != NO_ID) {
      return(singular_id);
    }
  }
  else {
    /* now check 'es' */
    if (word[index-1] == 'e' && word[index] == 's') {
      strncpy(check_word, word, index-2);
      check_word[index-1] = '\0';

      if ( (singular_id = GetSingularId(check_word)) != NO_ID) {
        return(singular_id);
      }
    }
  }
  /* word was not a plural form */
  return(NO_ID);
}
