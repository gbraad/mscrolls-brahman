
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
/*                                                                      */
/* <marnix@xvan.nl>                                                     */
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
#include <stdlib.h>    /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"


/*************************/
/* function declarations */
/*************************/

void ENG_CheckLocWildCard(char*, resultStruct*);
void ENG_CheckObjWildCard(char*, resultStruct*);


/* one external, don want to make a header file */
extern char *strlwr(char*);


/************************/
/* function definitions */
/************************/

void ENG_CheckLocWildCard(char *name, resultStruct *result)
{
  char word_lwr[MAX_WORD_LEN+1];

  /* if we don't copy to word_lwr but    */
  /* feed name directly into strlwr()    */
  /* it will crash. Don;t understand why */

  strncpy(word_lwr, name, MAX_WORD_LEN);
  strlwr(word_lwr);

  if (strcmp(word_lwr, "location") == 0) {
    result->tag = OK;
    result->value = CURR_LOC;
  }
  else {
    result->tag = ERROR;
    result->value = NO_ID;
  }
}


void ENG_CheckObjWildCard(char *name, resultStruct *result)
{
  char word_lwr[MAX_WORD_LEN+1];

  /* if we don't copy to word_lwr but    */
  /* feed name directly into strlwr()    */
  /* it will crash. Don;t understand why */

  strncpy(word_lwr, name, MAX_WORD_LEN);
  strlwr(word_lwr);

  result->tag   = ERROR;
  result->value = NO_ID;

  if (strcmp(word_lwr, "actor") == 0) {
    result->tag   = OK;
    result->value = ACTOR;
    return;
  }

  if (strcmp(word_lwr, "subject") == 0) {
    result->tag   = OK;
    result->value = SUBJECT;
    return;
  }

  if (strcmp(word_lwr, "spec") == 0) {
    result->tag   = OK;
    result->value = SPECIFIER;
    return;
  }
}
