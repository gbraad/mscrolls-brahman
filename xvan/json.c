
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
#include <stdlib.h>   /* free()    */
#include <string.h>   /* strlen()  */
#include <ctype.h>    /* tolower() */
#include <stdint.h>

#include "json.h"

/* DO NOT INCLUDE keyword.h */

/*************************/
/* Function declarations */
/*************************/

/* Helper functions */
int32_t IsDigit(char);
void    ResetKVPair(kvPair*);
void    GetDigitString(char*, char*, int*, int);
void    ConvertIntegral(char*, int, int32_t*);
void    ConvertFraction(char*, float*);
void    ConvertExponent(char*, int, float*);

/* Validation functions */
int32_t ValidateJsonString(char*, int*);
int32_t ValidateJsonNumber(char*, int*);
int32_t ValidateJsonArray(char*, int*);
int32_t ValidateJsonObject(char*, int*);
int32_t ValidateJson(char*);

/* Processing functions */
char    *ReadJsonObject(char*, int*);
char    *ReadJsonArray(char*, int*);
char    *ReadJsonString(char*, int*);

void    ReadJsonNumber(char*, int*, int32_t*, float*, int*);
void    ReadJsonValue(char*, jsonValue*, int*);
int32_t GetNextKVpair(char*, int*, kvPair*);

/************************/
/* Function definitions */
/************************/

/********************/
/* Helper functions */
/********************/

int32_t IsDigit(char digit)
{
  return (digit >= '0' && digit <= '9');
}


<<<<<<< HEAD
=======
void ResetKVPair(kvPair *kv)
{
  kv->key                 = ResetString(kv->key);
  kv->value.type          = 0;
  (kv->value).textstring  = ResetString((kv->value).textstring);
  kv->value.int_number    = 0;
  kv->value.float_number  = 0;

  return;
}


>>>>>>> 72d7449e33257b77bc124b16a988a408eddcf5b1
void GetDigitString(char *source, char *dest, int *index, int max_len)
{
  int start = *index;
  int len;

  while (IsDigit(source[*index])) {
    (*index)++;
  }

  /* we now have the digit string */
  len = (*index) - start;
  len = (len < max_len ? len : max_len-1);
  strncpy(dest, source+start, len);

  /* terminate dest */
  dest[len] = '\0';

  return;
}


void ConvertIntegral(char *integral, int sign, int32_t *int_value)
{
  int len = strlen(integral);
  int i   = 0;

  /* format: 12345 */

  *int_value = 0;

  for (i=0; i<len; i++)
    *int_value = 10*(*int_value)+integral[i]-'0';

  if (sign == -1)
    *int_value = (*int_value)*-1;

  return;
}


void ConvertFraction(char *fraction, float *fract_value)
{
  int len = strlen(fraction);
  int i   = 0;

  /* format: 12345 */

  *fract_value = 0;

  for (i=len-1; i>=0; i--)
    *fract_value = ((*fract_value)/10) + (float)(fraction[i]-'0')/10;

  return;
}


void ConvertExponent(char *exponent, int sign, float *exp_value)
{
  int32_t exp;
  int     i = 0;

  /* format: 12 */

  /* first, comvert the exponent itself to its integer value */
  /* set sign to 1 because exp must be positive in for-loop  */
  ConvertIntegral(exponent, 1, &exp);

  *exp_value = 1;

  /* now calculate the actual exponent */
  if (sign == 1) {
    for (i=0; i<exp; i++)
      *exp_value = (*exp_value)*10;
  }
  else {
    for (i=0; i<exp; i++)
      *exp_value = (*exp_value)/10;
  }

  return;
}


/************************/
/* Validation functions */
/************************/

int32_t ValidateJsonString(char *string, int *index)
{
  int state  = 0;

  /* string[*index] is start quote */

  while (1) {
    switch (string[(*index)++]) {
      case '\0':
        /* unexpected end of string */
        return(0);

      case '"':
        switch (state) {
          case 0:
            state = 1;
            break;
          case 1: ;
          case 3:
            /* ok */
            return(1);
            break;
          case 2:
            state = 3;
            break;
          default:
            /* error */
            return(0);
        }
        break;

      case '\\':
        switch (state) {
          case 1:
            state = 2;
            break;
          case 2:
            state = 3;
            break;
          case 3:
            state = 2;
            break;
          default:
            /* error */
            return(0);
        }
        break;

      case '/': ;
      case 'b': ;
      case 'f': ;
      case 'n': ;
      case 'r': ;
      case 't':
        switch (state) {
          case 1: ;
          case 2: ;
          case 3:
            state = 3;
            break;
          default:
            /* error */
            return(0);
        }
        break;

      case 'u':
        switch (state) {
          case 1: ;
          case 3:
            state = 3;
            break;
          case 2:
            state = 4;
            break;
          default:
            /* error */
            return(0);
        }
        break;

      case '0': ;
      case '1': ;
      case '2': ;
      case '3': ;
      case '4': ;
      case '5': ;
      case '6': ;
      case '7': ;
      case '8': ;
      case '9': ;
      case 'A': ;
      case 'B': ;
      case 'C': ;
      case 'D': ;
      case 'E': ;
      case 'F':
        switch (state) {
          case 1: ;
          case 3:
            state = 3;
            break;
          case 4:
            state = 5;
            break;
          case 5:
            state = 6;
            break;
          case 6:
            state = 7;
            break;
          case 7:
            state = 3;
            break;
          default:
            /* error */
            return(0);
        }
        break;

      default:
        /* any other unicode char */
        switch (state) {
          case 1: ;
          case 3:
            state = 3;
            break;
          default:
            /* error */
            return(0);
        }
        break;
    } /* switch */
  } /* while */
}


int32_t ValidateJsonNumber(char *json_number, int *index)
{
  int state = 0;

  /* this function checks whether number_string */
  /* complies with the json number syntax       */

  while (1) {
    switch (json_number[*index]) {
      case '-':
        switch (state) {
          case 0:
            state = 1;
            break;
          case 5:
            state = 6;
            break;
          default:
            return(0);
        }
        break;

      case '+':
        switch (state) {
          case 5:
            state = 6;
            break;
          default:
            return(0);
        }
        break;

      case '.':
        switch (state) {
          case 2: ;
          case 3:
            state = 4;
            break;
          default:
            return(0);
        }
        break;

      case 'e': ;
      case 'E':
        switch (state) {
          case 2: ;
          case 3: ;
          case 4:
            state = 5;
            break;
          default:
            return(0);
        }
        break;

      case '0':
        switch (state) {
          case 0: ;
          case 1:
            state = 2;
            break;
          case 5:
            state =6;
            break;
          case 3: ;
          case 4: ;
          case 6:
            /* state remains the same */
            break;
          default:
            return(0);
        }
        break;

      case '1': ;
      case '2': ;
      case '3': ;
      case '4': ;
      case '5': ;
      case '6': ;
      case '7': ;
      case '8': ;
      case '9':
        switch (state) {
          case 0: ;
          case 1:
            state =3;
            break;
          case 5:
            state = 6;
            break;
          case 3: ;
          case 4: ;
          case 6:
            /* state remains the same */
            break;
          default:
            return(0);
        }
        break;

      default:
        switch (state) {
          case 2: ;
          case 3: ;
          case 4: ;
          case 6:
            return(1);
          default:
            return(0);
        }
        break;
    } /* switch(number_string[*index]) */
    (*index)++;
  } /* while */
}


int32_t ValidateJsonArray(char *json_array, int *index)
{
  int state = 1;

  /* this function checks whether json_string */
  /* complies with the json array syntax      */

  /* we use this function recursive, so skip */
  /* the '[' to prevent an endless loop      */

  if (json_array[(*index)++] != '[')
    return(0);

  while (1) {
    switch (json_array[*index]) {
      case '\0':
        /* end of string before ']'*/
        return(0);

      case '{':
        switch (state) {
          case 1: ;
          case 3:
            state = 2;
            /* an object */
            if (!ValidateJsonObject(json_array, index))
              return(0);
            break;
          default:
            return(0);
        }
        break;

      case '[':
        switch (state) {
          case 1: ;
          case 3:
            state = 2;
            /* a nested array */
            if (!ValidateJsonArray(json_array, index))
              return(0);
            break;
          default:
            return(0);
        }
        break;

      case ' ':
        /* all states allow 1 or more spaces */
        (*index)++;
        break;

      case '"':
        switch (state) {
          case 1: ;
          case 3: ;
            state = 2;
            /* a string */
            if (!ValidateJsonString(json_array, index))
              return(0);
            break;
          default:
            return(0);
        }
        break;

      case 't':
        switch (state) {
          case 1: ;
          case 3:
            state = 2;
            /* a true */
            if (strncmp(json_array+(*index), "true", 4) != 0)
              return(0);
            (*index) += 4;
            break;
          default:
            return(0);
        }
        break;

      case 'f':
        switch (state) {
          case 1: ;
          case 3:
            state = 2;
            /* a false */
            if (strncmp(json_array+(*index), "false", 5) != 0)
              return(0);
            (*index) += 5;
            break;
          default:
            return(0);
        }
        break;

      case 'n':
        switch (state) {
          case 1: ;
          case 3:
            state = 2;
            /* a null */
            if (strncmp(json_array+(*index), "null", 4) != 0)
              return(0);
            (*index) += 4;
            break;
          default:
            return(0);
        }
        break;

      case ',':
        switch (state) {
          case 2:
            /* a comma */
            state = 3;
            (*index)++;
            break;
          default:
            return(0);
        }
        break;

      case ']':
        switch (state) {
          case 1: ;
          case 2:
            /* end of the array */
            (*index)++;
            return(1);
            break;
          default:
            return(0);
        }
        break;

      default:
        switch (state) {
          case 1:
          case 3:
            state = 2;
            /* check for a number */
            if (!ValidateJsonNumber(json_array, index))
              return(0);
            break;
          default:
            return(0);
        }
        break;
    } /* switch(json_array[*index]) */
  } /* while */
}


int32_t ValidateJsonObject(char *json_string, int *index)
{
  int state = 1;

  /* this function checks whether json_string */
  /* complies with the json object syntax     */

  /* we use this function recursive, so skip */
  /* the '{' to prevent an endless loop      */

  if (json_string[(*index)++] != '{')
    return(0);

  while (1) {
    switch (json_string[*index]) {
      case '\0':
        /* end of string before '}'*/
        return(0);

      case '{':
        switch (state) {
          case 3:
            state = 4;
            /* a nested object */
            if (!ValidateJsonObject(json_string, index))
              return(0);
            break;
          default:
            return(0);
        }
        break;

      case ' ':
        /* all states allow 1 or more spaces */
        (*index)++;
        break;
      case '[':
        switch (state) {
          case 3:
            state = 4;
            /* an array */
            if (!ValidateJsonArray(json_string, index))
              return(0);
            break;
          default:
            return(0);
        }
        break;

      case '}':
        switch (state) {
          case 1: ;
          case 4:
            (*index)++;
            return(1);
          default:
            return(0);
        }
        break;

      case '"':
        switch (state) {
          case 1:
            state = 2;
            break;
          case 3:
            state = 4;
            break;
          case 5:
            state = 2;
            break;
          default:
            return(0);
        }
        /* a string */
        if (!ValidateJsonString(json_string, index))
          return(0);
        break;

      case 't':
        switch (state) {
          case 3:
            state = 4;
            /* a true */
            if (strncmp(json_string+(*index), "true", 4) != 0)
              return(0);
            (*index) += 4;
            break;
          default:
            return(0);
        }
        break;

      case 'f':
        switch (state) {
          case 3:
            state = 4;
            /* a false */
            if (strncmp(json_string+(*index), "false", 5) != 0)
              return(0);
            (*index) += 5;
            break;
          default:
            return(0);
        }
        break;

      case 'n':
        switch (state) {
          case 3:
            state = 4;
            /* a null */
            if (strncmp(json_string+(*index), "null", 4) != 0)
              return(0);
            (*index) += 4;
            break;
          default:
            return(0);
        }
        break;

      case ',':
        switch (state) {
          case 4:
            /* a comma */
            state = 5;
            (*index)++;
            break;
          default:
            return(0);
        }
        break;

      case ':':
        switch (state) {
          case 2:
            /* a colon */
            state = 3;
            (*index)++;
            break;
          default:
            return(0);
        }
        break;
      default:
        switch (state) {
          case 3:
            state = 4;
            /* check for a number */
            if (!ValidateJsonNumber(json_string, index))
              return(0);
            break;
          default:
            return(0);
        }
        break;
    } /* switch(json_string[*index]) */
  } /* while */
}


int32_t ValidateJson(char *json_string)
{
  /* a json string always starts with */
  /* '{' (object) or '[' (array).     */

  int index = 0;
  int done  = 0;

  /* json_string may contain more than 1 */
  /* object or array separated by spaces  */

  while (!done) {
    switch (json_string[index]) {
      case '{':
        if (!ValidateJsonObject(json_string, &index)) {
          /* error */
          return(0);
        }
        break;

      case '[':
        if (!ValidateJsonArray(json_string, &index)) {
          /* error */
          return(0);
        }
        break;

      default:
        /* error */
        return(0);
      }
      /* look up the next object or array */
      while (json_string[index] == ' ') {
        index++;
      }
      if (json_string[index] == '\0') {
        /* we're done */
        done = 1;
      }
    } /* while */

    /* ok */
    return(1);
}


/************************/
/* Processing functions */
/************************/

void ReadJsonNumber(char *json_string, int *number_type, int32_t *int_value, float *float_value, int *index)
{
  int integral_sign = 1;
  int exponent_sign = 1;

  char integral[MAX_INTEGRAL_LEN] = "";
  char fraction[MAX_FRACTION_LEN] = "";
  char exponent[MAX_EXPONENT_LEN] = "";

  int32_t integral_number = 0;
  float   fraction_number = 0;
  float   exp_number      = 1;

  /* sample: +123.456E-78  */
  /* '+'   : integral_sign */
  /* '123' : integral      */
  /* '-'   : exponent_sign */
  /* '456' : exponent      */


  /* we don't know yet if it is a float or an int */
  /* set default to int */
  *number_type = JSON_VAL_INT;

  if (json_string[*index] == '-' || json_string[*index] == '+') {
    if (json_string[*index] == '-')
      integral_sign = -1;
    (*index)++;
  }

  /* now we have digits for the integral part coming */
  GetDigitString(json_string, integral, index, MAX_INTEGRAL_LEN);

  /* we are now either at the '.', the 'e', 'E' or end */
  if (json_string[*index] == '.') {
    *number_type = JSON_VAL_FLOAT;
    (*index)++;
    GetDigitString(json_string, fraction, index, MAX_FRACTION_LEN);
  }

  /* we are now at the 'e', 'E' or end */
  if (json_string[*index] == 'e' || json_string[*index] == 'E') {
    *number_type = JSON_VAL_FLOAT;
    (*index)++;
    if (json_string[*index] == '-' || json_string[*index] == '+') {
      if (json_string[*index] == '-')
        exponent_sign = -1;
      (*index)++;
    }
    /* we are now at the exponent digits */
    GetDigitString(json_string, exponent, index, MAX_EXPONENT_LEN);
  }

  /* we are at the end of the number string */
  /* now convert everything to real numbers */

  /* there is always an integral part */
  ConvertIntegral(integral, integral_sign, &integral_number);

  if (strlen(fraction) != 0) {
    /* there is a fraction part */
    ConvertFraction(fraction, &fraction_number);
  }

  if (strlen(exponent) != 0) {
    /* there is an exponent */
    ConvertExponent(exponent, exponent_sign, &exp_number);
  }

  /* now make the final result */
  if (*number_type == JSON_VAL_INT) {
    *int_value = integral_number;
  }
  else {
    *float_value = (integral_number + fraction_number) * exp_number;
  }

  return;
}


char* ReadJsonString(char *json_string, int *index)
 /* json_string is string to read from, input par   */
{
  char *string_var = NULL;
  int  start       = (*index)++;
  int  len;

  /* start points at the opening " and *index points */
  /* at the first character after the opening ".     */

  /* we want to read until the end ", */
  /* but keep in mind there may be \" */
  /* occurences in the string.        */

  while (json_string[*index] != '"') {
    if (json_string[*index] == '\\') {
      /* skip the next char, which is   */
      /* part of an escape sequence and */
      /* could be a "                   */
      (*index)++;
    }
    (*index)++;
  }

  /* the string is in between start and *index */
  /* copy it to the string_var                 */
  /* We do NOT include the quotes (")          */

  len = (*index)-start-1;  /* exclude quotes */

  string_var = ExpandString(string_var, len);
  strncpy(string_var, json_string+start+1, len);

  /* terminate string_var */
  string_var[len] = '\0';

  /* move *index to first char after the string */
  (*index)++;

  return(string_var);
}


char *ReadJsonObject(char *json_string, int *index)
 /* json_string is input par */
{
  char *object_var = NULL;
  int  start       = *index;
  int  len;
  int  level       = 1;  /* denotes the { } level */

  /* an object is delimited by {} but we must */
  /* take into account that objecets may be   */
  /* nested. So, whenever level == 0, we have */
  /* our object.                              */

  /* *index points at the { */

  do {
    (*index)++;

    switch (json_string[*index]) {
      case '{':
        level++;
        break;
      case '}':
        level--;
        break;
      default:
        /* do nothing */
        break;
    }
  } while (level > 0);

  /* start points to { and *index to } */

  len = (*index)-start+1;  /* include { and } */

  object_var = ExpandString(object_var, len);
  strncpy(object_var, json_string+start, len);

  /* terminate object_var */
  object_var[len] = '\0';

  return(object_var);
}


char *ReadJsonArray(char *json_string, int *index)
 /* json_string is input par */
{
  char *array_var = NULL;
  int start       = *index;
  int len;
  int level       = 1;  /* denotes the [ ] level */

  /* an array is delimited by {} but we must  */
  /* take into account that arrays may be     */
  /* nested. So, whenever level == 0, we have */
  /* our array.                               */

  /* *index points at the { */

  do {
    (*index)++;

    switch (json_string[*index]) {
      case '[':
        level++;
        break;
      case ']':
        level--;
        break;
      default:
        /* do nothing */
        break;
    }
  } while (level > 0);

  /* start points to [ and *index to ] */

  len = (*index)-start+1;  /* include [ and ] */

  array_var = ExpandString(array_var, len);
  strncpy(array_var, json_string+start, len);

  /* terminate array_var */
  array_var[len] = '\0';

  return(array_var);
}


void ReadJsonValue(char *json_string, jsonValue *value_var, int *index)
 /* json_string is input var                  */
 /* value_var is output par, must be maloc-ed */

{
  int     number_type;
  int32_t int_value;
  float   float_value;

  /* read until not a space or a : */
  while (json_string[(*index)] == ' ' || json_string[(*index)] == ':') {
    (*index)++;
  }

  /* now check what kind of value we have here */
  switch (json_string[*index]) {
    case '"':
      /* it's a string */
      value_var->textstring = ReadJsonString(json_string, index);
      value_var->type = JSON_VAL_STRING;
      break;

    case '{':
      value_var->textstring = ReadJsonObject(json_string, index);
      value_var->type = JSON_VAL_OBJECT;
      /* it's an object */
      break;

    case '[':
      /* it's an array */
      value_var->textstring = ReadJsonArray(json_string, index);
      value_var->type = JSON_VAL_ARRAY;
      break;

    case 't':
      /* it's a true value */
      value_var->type = JSON_VAL_TRUE;
      break;

    case 'f':
      /* it's a false value */
      value_var->type = JSON_VAL_FALSE;
      break;

    case 'n':
      /* it's a null value */
      value_var->type = JSON_VAL_NULL;
      break;

    default:
      /* it's a number */
      ReadJsonNumber(json_string, &number_type, &int_value, &float_value, index);
      value_var->type         = number_type;
      value_var->int_number   = int_value;
      value_var->float_number = float_value;
      break;
  }
  return;
}


int32_t GetNextKVpair(char *json_string, int *index, kvPair *kv)
{
  /* json syntax has been checked, so we expect no errors */

  /* move forward to the " */
  while (json_string[*index] != '"' && json_string[*index] != '\0') {
    (*index)++;
  }

  if (json_string[*index] == '\0') {
    /* end of string */
    return(0);
  }

  /* read the key, which is a string */
  kv->key = ReadJsonString(json_string, index);

  /* now read the value */
  ReadJsonValue(json_string, &(kv->value), index);

  return(1);

}
