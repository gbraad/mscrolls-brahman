
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
char *AddCommaOrNot(char*);
char *ExpandString(char*, int);
char *ResetString(char*);

/* Build functions */
char *NewJsonObject(char*);
char *NewJsonArray(char*);
char *CloseArray(char*);
char *CloseObject(char*);
char *AddKV(char*, char*, jsonValue*);

/************************/
/* Function definitions */
/************************/

/********************/
/* Helper functions */
/********************/

char *AddCommaOrNot(char *source)
 /* source must be malloc-ed */
{
  int len = 0;

  if (!source) {
    /* NULL string */
    return(source);
  }
  /* we must add a comma if the last char of */
  /* source is not a '{' or a '['            */

  if ( (len = strlen(source)) == 0) {
    return(source);
  }

  if (source[len-1] != '{' && source[len-1] != '[') {
    if (!(source = ExpandString(source, 1))) {
    return(NULL);
    }
    strcat(source, ",");
  }

  return(source);
}


char *ExpandString(char *source, int amount)
{
  int  len = 0;
  char *temp;

  if (!source) {
    /* amount + 1 because of the '\0' */
    if ( (source = (char*) malloc((amount+1)*sizeof(char))) == NULL )  {
      return(NULL);
    }
    source[0] = '\0';
  }
  else {
    /* don't know why the F#$K realloc() crashes the debugger        */
    /* got the infamous "cannot find the bounds of current function" */

    /* my own realloc() */
    len = strlen(source);
    if ( !(temp = (char *) malloc((len+1)*sizeof(char))) )
      return(NULL);

    strcpy(temp, source);
    free(source);

    if ( !(source = (char *) malloc((len+amount+1)*sizeof(char))) )  /* +1 for the '\0\ */
      return(NULL);
    strcpy(source, temp);
    free(temp);
  }
  return(source);
}


char *ResetString(char *string)
 /* string must be maloc-ed */
{

  if (string == NULL) {
    return(NULL);
  }
  else {
    free(string);
    return(NULL);
  }
}

/*******************/
/* Build functions */
/*******************/

char *NewJsonObject(char *json_string)
 /* json_string must be malloc-ed */
{
  if (json_string != NULL) {
    /* new object, not within an existing object or array */
    if (!(json_string = AddCommaOrNot(json_string))) {
      return(NULL);
    }
  }

  if (!(json_string = ExpandString(json_string, 1))) {
    /* error was printed by ExpandString() */
    return(NULL);
  }

  strcat(json_string, "{");

  return(json_string);
}


char *NewJsonArray(char *json_string)
 /* json_string must be malloc-ed */
{
  if (json_string != NULL) {
    /* new object, not within an existing object or array */
    if (!(json_string = AddCommaOrNot(json_string))) {
      return(NULL);
    }
  }
  if (!(json_string = ExpandString(json_string, 1))) {
    /* error was printed by ExpandString() */
    return(NULL);
  }

  strcat(json_string, "[");

  return(json_string);
}


char *CloseObject(char *json_string)
 /* json_string must be malloc-ed */
{
  if (!(json_string = ExpandString(json_string, 1))) {
    /* error was printed by ExpandString() */
    return(NULL);
  }

  strcat(json_string, "}");

  return(json_string);
}


char *CloseArray(char *json_string)
 /* json_string must be malloc-ed */
{
  if (!(json_string = ExpandString(json_string, 1))) {
    /* error was printed by ExpandString() */
    return(NULL);
  }

  strcat(json_string, "]");

  return(json_string);
}


char *AddKV(char *json_string, char *key, jsonValue *value)
 {
  int  key_len;
  int  value_len = 0;
  int overhead   = 0;
  char str[MAX_INTEGRAL_LEN+MAX_FRACTION_LEN+MAX_EXPONENT_LEN+2];
  /* +2 for 'E' and '\0' */

  /* if key equals "", we have an array */
  /* and must only add the value        */

  key_len = (!key ? 0 : strlen(key));

  switch(value->type) {
    case JSON_VAL_OBJECT: ; /* treated as a string */
    case JSON_VAL_ARRAY: ;  /* treated as a string */
    case JSON_VAL_STRING:
      value_len = strlen(value->textstring);

      /* we need "key":"string" or ,"key":"string" */
      /* or "string" or ,"string" (array)          */

      if (!(json_string = AddCommaOrNot(json_string))) {
        return(NULL);
      }

      if (value->type == JSON_VAL_STRING) {
        overhead = (key_len == 0 ? 2 : 5);
      }
      else {
        overhead = (key_len == 0 ? 0 :3);
      }

      if (!(json_string = ExpandString(json_string, key_len+value_len+overhead))) {
        return(NULL);
      }
      if (key_len != 0) {
        strcat(json_string, "\"");
        strcat(json_string, key);
        strcat(json_string, "\":");
      }
      /* for object and array we must not use quotes */
      if (value->type == JSON_VAL_STRING) {
        strcat(json_string, "\"");
      }
      strcat(json_string, value->textstring);
      if (value->type == JSON_VAL_STRING) {
        strcat(json_string, "\"");
      }
      break;

    case JSON_VAL_TRUE:
      /* we need "key":true or ,"key":true */
      /* or true or ,true                  */

        value_len = 4;

      if (!(json_string = AddCommaOrNot(json_string))) {
        return(NULL);
      }

      overhead = (key_len == 0 ? 0 : 3);

      if (!(json_string = ExpandString(json_string, key_len+value_len+overhead))) {
        return(NULL);
      }

      if (key_len != 0) {
        strcat(json_string, "\"");
        strcat(json_string, key);
        strcat(json_string, "\":");
      }
      strcat(json_string, "true");
      break;

    case JSON_VAL_FALSE:
      /* we need "key":false or ,"key":false */
      /* or false or ,false                  */

        value_len = 5;

      if (!(json_string = AddCommaOrNot(json_string))) {
        return(NULL);
      }

      overhead = (key_len == 0 ? 0 : 3);

      if (!(json_string = ExpandString(json_string, key_len+value_len+overhead))) {
        return(NULL);
      }

      if (key_len != 0) {
        strcat(json_string, "\"");
        strcat(json_string, key);
        strcat(json_string, "\":");
      }
      strcat(json_string, "false");
      break;

    case JSON_VAL_NULL:
      /* we need "key":null or ,"key":null */
      /* or null or ,null                  */

      if (!(json_string = AddCommaOrNot(json_string))) {
        return(NULL);
      }

      overhead = (key_len == 0 ? 0 : 3);

      if (!(json_string = ExpandString(json_string, key_len+value_len+overhead))) {
        return(NULL);
      }

      if (key_len != 0) {
        strcat(json_string, "\"");
        strcat(json_string, key);
        strcat(json_string, "\":");
      }
      strcat(json_string, "null");
      break;

    case JSON_VAL_INT:
      /* we need "key":intvalue or ,"key":intvalue */
      /* or intvalue or ,intvalue                  */

      /* convert int  to a text string */
      /* sprintf return value does not include the '\0' */
      if ( sprintf(str, "%d", value->int_number) < 0) {
        free(json_string);
        return(NULL);
      }
      value_len = strlen(str);

      if (!(json_string = AddCommaOrNot(json_string))) {
        return(NULL);
      }

      overhead = (key_len == 0 ? 0 : 3);

      if (!(json_string = ExpandString(json_string, key_len+value_len+overhead))) {
        return(NULL);
      }

      if (key_len != 0) {
        strcat(json_string, "\"");
        strcat(json_string, key);
        strcat(json_string, "\":");
      }
      strcat(json_string, str);
      break;

    case JSON_VAL_FLOAT:
      /* we need "key":floatvalue or ,"key":floatvalue */
      /* or floatvalue or ,floatvalue                  */

      /* convert float to a text string */
      /* sprintf return value does not include the '\0' */
      if ( (value_len = sprintf(str, "%f", value->float_number)) < 0) {
        free(json_string);
        return(NULL);
      }

      if (!(json_string = AddCommaOrNot(json_string))) {
        return(NULL);
      }

      overhead = (key_len == 0 ? 0 : 3);

      if (!(json_string = ExpandString(json_string, key_len+value_len+3))) {
        return(NULL);
      }

      if (key_len != 0) {
        strcat(json_string, "\"");
        strcat(json_string, key);
        strcat(json_string, "\":");
      }
      strcat(json_string, str);
      break;

    default:
      return(NULL);
  }

  return(json_string);
 }
