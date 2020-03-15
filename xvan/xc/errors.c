
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
#include <stdlib.h>   /* malloc() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"

#include "errors.h"

#include "./Languages/NL/NL-kwtable.h"

#include "./Languages/ENG/ENG-errors.h"
#include "./Languages/NL/NL-errors.h"


/*************************/
/* Function declarations */
/*************************/

void  ErrHdr();
void  PrintError(int16_t, resultStruct*, char*);
void  NrErr(char*, char*);
void  TypeErr(int32_t, char*, char*);
char  *TranslateKeyword(char*);
char  *ScanKeywordTable(char*, int32_t, int32_t);


/************************/
/* Function definitions */
/************************/

void ErrHdr(void)
{
  switch (xvan_language) {
    case ENG:
      printf("\n\n\n******** Error ********\n\n");
      printf("File: %s, line: %d\n", current_filename, line_num);
      break;
    case NL:
      printf("\n\n\n***** Foutmelding *****\n\n");
      printf("Bestand: %s, regel: %d\n", current_filename, line_num);
      break;
    default:
      /* only get here when there is an error with */
      /* reading the language. Use English.        */
      printf("\n\n\n******** Error ********\n\n");
      printf("File: %s, line: %d\n", current_filename, line_num);
  }
}

void PrintError(int16_t err_num, resultStruct *par1, char *par2)
{
  const char **errors;

  /* set the error set for the language */

  switch(xvan_language) {
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

  printf("%s", errors[err_num]);

  if (par1 != NULL) {
    if (par1->tag != NONE) {
      printf(" %d", par1->value);
    }
  }

  if (par2 != NULL) {
    printf(" %s", par2);
  }
  printf(".\n");
}

void NrErr(char *fun_name, char *nr_of_pars)
{
  ErrHdr();

  switch (xvan_language) {
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

void TypeErr(int32_t par_nr, char *fun_name, char *type)
{
  ErrHdr();

  switch (xvan_language) {
    case ENG:
      printf("\nError, parameter %d for function %s must have type %s\n",
              par_nr, fun_name, type);
      break;
    case NL:
      printf("\nFoutmelding, parameter %d van functie %s moet van type %s zijn.n",
              par_nr, fun_name, type);
      break;
    default:
      /* we should never get here, use English */
      printf("\nError, parameter %d for function %s must have type %s\n",
              par_nr, fun_name, type);
  }
}

char *TranslateKeyword(char *word)
{
  return(ScanKeywordTable(word, 0, sizeof(kw_table)/sizeof(kwTable)-1));
}


char *ScanKeywordTable(char *word, int32_t lower, int32_t upper)
{
  int32_t i;
  int32_t result;
  char word_upr[MAX_WORD_LEN+1];

  /* this function is used to find the right translation */
  /* for keywords to print in error messages.            */

  /* convert to upper case */
  strncpy(word_upr, word, MAX_WORD_LEN);
  strupr(word_upr);

  /* check for NULL word must be done by caller          */
  if (upper<lower)
    /* not found */
    /* return word itself, so we can at least print something */
    return(word);

  i = (lower+upper)/2;

  if ((result = strcmp(word_upr, kw_table[i].keyword_eng)) == 0) {
    /* found it */
    switch(story_language) {
      case NL:
        return(kw_table[i].keyword_nl);
        break;
      default:
        /*return(kw_table[i].keyword_eng);*/
        return(word);
    }
  }

  if (result<0)
    upper = i-1;
  else
    lower = i+1;

  return(ScanKeywordTable(word, lower, upper));
}
