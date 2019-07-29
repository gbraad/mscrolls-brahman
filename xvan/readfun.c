
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "readfun.h"

/*************************/
/* function declarations */
/*************************/

int32_t CheckedStore(int32_t*, int32_t, int32_t*, int32_t);
int32_t ReadFun(int32_t*, int32_t*, int32_t);

/************************/
/* function definitions */
/************************/

int32_t CheckedStore(int32_t *array, int32_t upper_limit, int32_t *index, int32_t value)
{
  /* Stores value in array at position index. */
  /* Checks for array bound overflow.         */

  if (*index<upper_limit)
    array[(*index)++] = value;
  else {
    PrintError(51, NULL, NULL);
    return(ERROR);
  }

  return(OK);
}


int32_t ReadFun(int32_t *trigger_code, int32_t *index, int32_t fun_code)
{
  int32_t  nr_of_pars  = 0;
  int32_t  par         = NO_ID;
  char     *string_par = NULL;
  int32_t  str_len     = 0;
  int32_t  ratio;            /* Needed to read string address.       */
  int32_t  arec_len    = 0;  /* Needed to read action record members */
  int32_t  i           = 0;
  int32_t  j           = 0;
  int32_t  counter     = 0;

  ratio = sizeof(char *)/sizeof(int32_t);  /* Must be > 0 */

  if (!CheckedStore(trigger_code, MAX_TRIGG_LEN, index, fun_code))
    return(ERROR);

  /* Read the number of parameters. */
  if (!GetNextCode32(&nr_of_pars))
    return(ERROR);

  /* Store the nr of parameters in trigger_code. */
  if (!CheckedStore(trigger_code, MAX_TRIGG_LEN, index, nr_of_pars))
    return(ERROR);

  for (i=0; i<nr_of_pars; i++) {
    while (par != END_OF_PAR) {
      /* Read the parameter */
      if (!GetNextCode32(&par))
        return(ERROR);

      /* We assume the syntax is correct; has been   */
      /* checked at compile time.                    */
      switch (par) {
        case NUMBER:
          if (!CheckedStore(trigger_code, MAX_TRIGG_LEN, index, par))
            return(ERROR);

          /* Read rest of number par. */
          if (!GetNextCode32(&par))
            return(ERROR);

          /* Store par. */
          if (!CheckedStore(trigger_code, MAX_TRIGG_LEN, index, par))
            return(ERROR);

          /* set par to NO_ID, because if the number would have    */
          /* the value of the END_OF_PAR keyword, we have an issue */
          par = NO_ID;
          break;
        case STRING:
          /* -1 means a char pointer is coming next. */
          if (!CheckedStore(trigger_code, MAX_TRIGG_LEN, index, -1))
            return(ERROR);

          /* Read length of string. */
          if (!GetNextCode32(&str_len))
            return(ERROR);

          /* Read the string. */
          /* Syntax differs from expected by ReadString() function: */
          /* STRING<str_len><char string>.                          */
          /* <char string> was stored as int arr with length        */
          /* str_len. Therefore we read it back as an int array.    */

          /* malloc space for string */
          if ((string_par = (char *) malloc((str_len)*sizeof(int32_t))) == NULL) {
            PrintError(15, NULL, "ReadFun()");
            return(ERROR);
          }

          /* Read the string. */
          for (counter=0; counter<str_len; counter++) {
            /* WARNING: because we stored a char string as an integer */
            /* array, do not use the following in the if-clause:      */
            /* (int32_t) (string_par+counter) or                      */
            /* (int32_t) &(string_par[counter])                       */
            /* Both will add counter as a char size, which is 1.      */
            /* But we must add counter as an int32_t size which is 4. */
            /* Therefore, first cast string_par to int32_t and then   */
            /* add counter.                                           */
            if (!GetNextCode32(((int32_t *) (string_par))+counter)) {
              PrintError(52, NULL, NULL);
              return(ERROR);
            }
          }

          /* Store address of string_par in trigger_code (we don't  */
          /* want to store the string itself in trigger_code for    */
          /* space reasons).                                        */

          for (j=0; j<ratio; j++) {
            if (!CheckedStore(trigger_code, MAX_TRIGG_LEN, index,
                                              ((int32_t *) &string_par)[j]))
              return(ERROR);
          } /* for */

          break;
        case ACTION_REC:
          /* next, the members of an action record are coming. */
          /* They are stored as a row of int32_t values. We    */
          /* cannot keep reading int32_t values until we       */
          /* encounter an END_OF_PAR, because the action       */
          /* record may have contained a number or ordinal     */
          /* with the same value as the END_OF_PAR keyword.    */
          /* We know how many int32_t values the compiler      */
          /* wrote, so just read that number of values.        */

          /* store the ACTION_REC keyword */
          if (!CheckedStore(trigger_code, MAX_TRIGG_LEN, index, par))
              return(ERROR);

          arec_len = 11 + MAX_PARSE_ADVERBS+1 + MAX_PARSE_PREPOS+1;

          /* Read the action record members */
          for (j=0; j<arec_len; j++) {
            if (!GetNextCode32(&par))
              return(ERROR);

            /* Store par. */
            if (!CheckedStore(trigger_code, MAX_TRIGG_LEN, index, par))
              return(ERROR);
          }

          /* set par to NO_ID, because if the last read member */
          /*  would have the value of the END_OF_PAR keyword,  */
          /* we have an issue                                  */
          par = NO_ID;
          break;
        default:
          /* It may be a function. */
          if (IsTestFun(par) || IsIntAct(par)) {
            if (!ReadFun(trigger_code, index, par))
              return(ERROR);
          } /* We do need the {}s here! */
          else {
            /* It is an `ordinary' id or END_OF_PAR. */
            if (!CheckedStore(trigger_code, MAX_TRIGG_LEN, index, par))
              return(ERROR);}
          break;
      } /* switch */
    } /* while */
    /* Reset par for next pass through while loop. */
    par = NO_ID;
  } /* for */

  return(OK);
}
