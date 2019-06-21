
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
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include "keyword.h"

extern int16_t xvan_language;               /* init.c        */

extern int32_t ENG_CheckIfKeyword(char*);   /* ENG-keyword.c */
extern int32_t NL_CheckIfKeyword(char*);    /* NL-keyword.c  */


/*************************/
/* function declarations */
/*************************/

char    *strlwr(char*);
char    *strupr(char*);
int32_t IsTestFun(int32_t);
int32_t IsIntAct(int32_t);
int32_t IsValidKeyword(int32_t);
int32_t CheckIfKeyword(char*);


/************************/
/* function definitions */
/************************/

char *strlwr(char *str)
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower(*p);
      p++;
  }
  return str;
}


char *strupr(char *str)
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = toupper(*p);
      p++;
  }
  return str;
}


int32_t IsTestFun(int32_t code)
{
  if ((code > LOWER_BOUND_TESTFUN) && (code < UPPER_BOUND_TESTFUN))
    return (OK);
  else
    return (ERROR);
}


int32_t IsIntAct(int32_t code)
{
  if ((code > LOWER_BOUND_INT_ACT) && (code < UPPER_BOUND_INT_ACT))
    return (OK);
  else
    return (ERROR);
}


int32_t IsValidKeyword(int32_t kw)
{
  /* added on sep 4th 2017 */
  /* This function checks whether kw is within 2    */
  /* ranges. If so, the keyword is a valid keyword  */
  /* that may follow a section that was just parsed */
  /* We need this because we no longer have one     */
  /* predefined flow with a state machine now that  */
  /* we allow inserted files.                       */

 return(
  (kw == EOF)
  ||
   (kw >= MIN_VALID_COMPILER_KEYWORD && kw <= MAX_VALID_COMPILER_KEYWORD)
  ||
  (kw >= MIN_VALID_VOC_KEYWORD && kw <= MAX_VALID_VOC_KEYWORD)
  );
}


int32_t CheckIfKeyword(char *word)
{
  char word_lwr[MAX_WORD_LEN+1];

  /* all words are converted to lowercase, but */
  /* we must preserve the original case for    */
  /* printing the word by the interpreter.     */
  strncpy(word_lwr, word, MAX_WORD_LEN);
  strlwr(word_lwr);

  /* default language is English, so all */
  /* language keywords in the beginning  */
  /* will be sent to the English keyword */
  /* checker                             */
  switch(xvan_language) {
    case ENG:
      return(ENG_CheckIfKeyword(word_lwr));
      break;
    case NL:
      return(NL_CheckIfKeyword(word_lwr));
      break;
    default:
      /* we should never get here, use English */
      return(ENG_CheckIfKeyword(word_lwr));
  }
}
