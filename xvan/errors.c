
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
#include <stdlib.h>   /* malloc() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"

#include "errors.h"

#include "NL-kwtable.h"

#include "ENG-errors.h"
#include "NL-errors.h"

/*************************/
/* Function declarations */
/*************************/

void  PrintError(int16_t, resultStruct*, char*);
void  NrErr(char*, char*);
void  TypeErr(int32_t, char*, char*);
char  *TranslateKeyword(char*);
char  *ScanKeywordTable(char*, int32_t, int32_t);

/************************/
/* Function definitions */
/************************/

void PrintError(err_num, par1, par2)
 int16_t      err_num;
 resultStruct *par1;
 char         *par2;
{
  const char **errors;

  /* set the error set for the language */

  switch(story_info.story_language) {
    case ENG:
      errors = ENG_errors;
      break;
    case NL:
      errors = NL_errors;
      break;
    default:
      /* we should never get here, use English */
      errors = ENG_errors;
  }

  sprintf(outputline, "%s", errors[err_num]);
  Output(outputline, 0);

  if (par1 != NULL) {
    if (par1->tag != NONE) {
      sprintf(outputline, " %d", par1->value);
      Output(outputline, 0);
    }
  }

  if (par2 != NULL) {
    sprintf(outputline, " %s", par2);
    Output(outputline, 0);
  }
  Output(".\n", 0);
}


void NrErr(fun_name, nr_of_pars)
 char *fun_name;
 char *nr_of_pars;
{
  switch (story_info.story_language) {
    case ENG:
      printf("\nError, function %s must have %s parameters.\n",
              fun_name, nr_of_pars);
      break;
    case NL:
      printf("\nFoutmelding, functie %s moet %s parameters hebben.\n",
              fun_name, nr_of_pars);
    default:
      /* we should never get here, use English */
      printf("\nError, function %s must have %s parameters.\n",
              fun_name, nr_of_pars);
  }
}


void TypeErr(par_nr, fun_name, type)
 int32_t  par_nr;
 char *fun_name;
 char *type;
{
  switch (story_info.story_language) {
    case ENG:
      sprintf(outputline, "\nError, parameter %d for function %s must have type %s\n",
              par_nr, TranslateKeyword(fun_name), TranslateKeyword(type));
      Output(outputline, 0);
      break;
    case NL:
      printf(outputline, "\nFoutmelding, parameter %d van functie %s moet van type %s zijn.n",
              par_nr, TranslateKeyword(fun_name), TranslateKeyword(type));
      Output(outputline, 0);
      break;
    default:
      /* we should never get here, use English */
      printf(outputline, "\nError, parameter %d for function %s must have type %s\n",
              par_nr, TranslateKeyword(fun_name), TranslateKeyword(type));
      Output(outputline, 0);
  }
}


char *TranslateKeyword(word)
 char *word;
{
  return(ScanKeywordTable(word, 0, sizeof(kw_table)/sizeof(kwTable)-1));
}


char *ScanKeywordTable(word, lower, upper)
 char      *word;    /* word to look for in word_table */
 int32_t   lower;    /* elements of array between      */
 int32_t   upper;    /* which to search                */
{
  int32_t i;
  int32_t result;

  /* this function is used to find the right translation */
  /* for keywords to print in error messages.            */

  /* check for NULL word must be done by caller          */
  if (upper<lower)
    /* not found */
    /* return word itself, so we can at least print something */
    return(word);

  i = (lower+upper)/2;

  if ((result = strcmp(word, kw_table[i].keyword_eng)) == 0) {
    /* found it */
    switch(story_info.story_language) {
      case NL:
        return(kw_table[i].keyword_nl);
        break;
      default:
        return(kw_table[i].keyword_eng);
    }
  }

  if (result<0)
    upper = i-1;
  else
    lower = i+1;

  return(ScanKeywordTable(word, lower, upper));
}
