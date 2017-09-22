
/* Copyright 2016, 2017 Marnix van den Bos */

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

int32_t CheckedStore(array, upper_limit, index, value)
 int32_t *array;
 int32_t upper_limit;
 int32_t *index;
 int32_t value;
{
  /* Stores value in array at position index. */
  /* Checks for array bound overflow.         */

  if (*index<upper_limit)
    array[(*index)++] = value;
  else {
    sprintf(outputline, "Internal error: array bound overflow %d.\n", upper_limit);
    Output(outputline);
    return(ERROR);
  }

  return(OK);
}


int32_t ReadFun(trigger_code, index, fun_code)
 int32_t *trigger_code;
 int32_t *index;
 int32_t fun_code;
{
  int32_t  nr_of_pars  = 0;
  int32_t  par         = NO_ID;
  char     *string_par = NULL;
  int32_t  str_len     = 0;
  int32_t  ratio;                 /* Needed to read string address. */
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
      /* checked at compiletime.                     */
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
            sprintf(outputline, "ReadFun(): error in malloc for string_par %d.\n", str_len);
            Output(outputline);
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
              sprintf(outputline, "ReadFun(): error reading string_par %d.\n", str_len);
              Output(outputline);
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
        default:
          /* It may be a function. */
          if (IsTestFun(par) || IsIntAct(par)) {
            if (!ReadFun(trigger_code, index, par))
              return(ERROR);
          } /* We do need the {}s here! */
          else {
            /* It is an `ordinary' id. */
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
