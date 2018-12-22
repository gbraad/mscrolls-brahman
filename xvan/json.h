
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


#if !defined(__json)
#define __json

/***********/
/* Defines */
/***********/

#define MAX_INTEGRAL_LEN      6 /* includes space for '\0' */
#define MAX_FRACTION_LEN      6 /* includes space for '\0' */
#define MAX_EXPONENT_LEN      3 /* includes space for '\0' */

/* we need 11 chars extra to wrap a text string to a json string */
/* e.g. look -> {"text":"look"} (from 4 to 15 characters         */

#define JSON_TEXT_OVERHEAD   11


/* possible value types */
#define JSON_VAL_STRING     100
#define JSON_VAL_INT        101
#define JSON_VAL_FLOAT      102
#define JSON_VAL_TRUE       103
#define JSON_VAL_FALSE      104
#define JSON_VAL_NULL       105
#define JSON_VAL_OBJECT     106
#define JSON_VAL_ARRAY      107

/********************/
/* type definitions */
/********************/

typedef struct {
          int16_t type;
          char    *textstring;
          int32_t int_number;
          float   float_number;
        } jsonValue;

typedef struct {
          char      *key;
          jsonValue value;
        } kvPair;

/*************************/
/* External declarations */
/*************************/

extern int32_t ProcessJson(char*, char*);
extern char    *ExpandString(char*, int);

#endif
