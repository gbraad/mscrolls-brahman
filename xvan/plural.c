
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
#include "plural.h"

/*************************/
/* Function declarations */
/*************************/

int32_t GetSingularId(char*);
int32_t CheckPlural(char*);

/************************/
/* Function definitions */
/************************/

int32_t GetSingularId(char *word)
{
  int32_t id = NO_ID;

  /* this routine checks if we know */
  /* word and if it has type NOUNS  */

  if ( (id = LookUpId(word)) != NO_ID) {
    if (HasType(id, NOUNS)) {
      return(id);
    }
  }

  return(NO_ID);
}


int32_t CheckPlural(char *word)
{
  int32_t id = NO_ID;

  /* check if it is plural           */
  /* this part is language dependent */

  switch (story_info.story_language) {
    case NL:
      id = NL_CheckPlural(word);
      break;
    case ENG:
      id = ENG_CheckPlural(word);
      break;
    default:
      /* unknown language, use English */
      id = ENG_CheckPlural(word);
      break;
  }

  return(id);
}
