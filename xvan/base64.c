
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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "keyword.h"
#include "typedefs.h"
#include "base64.h"

/*************************/
/* Function declarations */
/*************************/

/************************/
/*  Encoding functions  */
/************************/

char    GetBase64Value(int8_t);
char    *FinalizeBase64(char*, int, int8_t);
int32_t ByteToBase64(int8_t, int8_t*, int*, int8_t*);


/************************/
/*  Decoding functions  */
/************************/

char    *GetBytesFromBase64String(char*, int*, int);
int8_t  ConvertBase64Value(char);
int32_t Base64ToBytes(char*, int*, int, int8_t*, int*, int8_t*);


/************************/
/* Function definitions */
/************************/

/*********************************/
/* Encoding function definitions */
/*********************************/

char GetBase64Value(int8_t n)
{
  char base64[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M',
                   'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                   'a','b','c','d','e','f','g','h','i','j','k','l','m',
                   'n','o','p','q','r','s','t','u','v','w','x','y','z',
                   '0','1','2','3','4','5','6','7','8','9','+','/'};

  return(base64[n]);
}


char *FinalizeBase64(char *source, int trailer_len, int8_t trailer)
{
  int8_t add_value[4] = {0,0,0,0};  /* we need space for        */
                                    /* 2 '=' and a '\0' as well */

  /* This function finalizes a base64 character string.   */
  /* The trailer contains any remaning bits from the last */
  /* conversion. If there are remaining bits, they are    */
  /* padded with 0 bits until length 8. To indicate the   */
  /* padding '=' characters are added to the end of the   */
  /* source string.                                       */

  switch (trailer_len) {
    case 0:
      /* do nothing */
      break;
    case 2:

      trailer  = trailer << 4;  /* |-|-|-|-|-|-|1|2| => |-|-|1|2|-|-|-|-| */
      trailer &= 48;            /* |-|-|1|2|-|-|-|-| => |0|0|1|2|0|0|0|0| */

      add_value[0] = GetBase64Value(trailer);
      add_value[1] = '=';
      add_value[2] = '=';
      break;
    case 4:
      trailer  = trailer << 2;  /* |-|-|-|-|1|2|3|4| => |-|-|1|2|3|4|-|-| */
      trailer &= 60;            /* |-|-|1|2|3|4|-|-| => |0|0|1|2|3|4|0|0| */

      add_value[0] = GetBase64Value(trailer);
      add_value[1] = '=';
      break;
    default:
      /* we should never get here */
      PrintError(98, NULL, NULL);
      return(ERROR);
      break;
  }

  if ( (source = AddToString(source, (char*) add_value)) == NULL) {
    return(NULL);
  }

  return(source);
}


int32_t ByteToBase64(int8_t source, int8_t *result, int *trailer_len, int8_t *trailer)
{
  /* This function converts a byte to base64. The result is  */
  /* 1 or 2 byte(s) with bit 0 to 6 filled, with bit 0 the   */
  /* lsb.                                                    */

  /* The trailer var is both input and output:                */
  /* Input: remaining bits from a previous conversion.       */
  /* Output: bits that are left over from this conversion    */

  /* depending on the number of trailer bits the following    */
  /* masks are used (x = relevant bit)                       */

  /****************************************************** ****/
  /* mask for int8_t (1 byte) without prior remainder        */
  /***********************************************************/
  /*                   |x|x|x|x|x|x|-|-|                     */
  /*                                                         */
  /* new remainder:    |-|-|-|-|-|-|x|x|                     */
  /***********************************************************/

  /***********************************************************/
  /* mask for int8_t (1 byte) with prior remainder of 2 bits */
  /***********************************************************/
  /*                                                         */
  /*                   |x|x|x|x|-|-|-|-|                     */
  /*                                                         */
  /* new remainder:    |-|-|-|-|x|x|x|x|                     */
  /***********************************************************/

  /************************************************************/
  /* masks for int8_t (1 byte) with prior remainder of 4 bits */
  /************************************************************/
  /*                                                          */
  /*                   |x|x|-|-|-|-|-|-|                      */
  /*                   |-|-|x|x|x|x|x|x|                      */
  /*                                                          */
  /* no remainder                                             */
  /************************************************************/

  int8_t source_cp  = 0;
  int    result_len = 1;

  result[0] = 0;
  result[1] = 0;

  /* Note: a right shift may introduce new '1' bits when the source is  */
  /* a signed type. This is implementation dependent behavior. We could */
  /* cast all types to unsigned, but we choose to do an extra bitwise   */
  /* AND (&) after a right shift operation.                             */

  switch (*trailer_len) {
    case 0:
      *result     = source >> 2;   /* |1|2|3|4|5|6|7|8| => |-|-|1|2|3|4|5|6| */
      *result     &= 63;           /* |?|?|1|2|3|4|5|6| => |-|-|1|2|3|4|5|6| */
      *trailer     = source & 3;   /* |1|2|3|4|5|6|7|8| => |-|-|-|-|-|-|7|8| */
      *trailer_len = 2;
      break;
   case 2:
      *result    = *trailer << 4; /* |-|-|-|-|-|-|7|8| => |-|-|7|8|-|-|-|-| */
      source_cp  = source >> 4;   /* |1|2|3|4|5|6|7|8| => |-|-|-|-|1|2|3|4| */
      source_cp  &= 15;           /* |?|?|?|?|1|2|3|4| => |-|-|-|-|1|2|3|4| */
      *result    |= source_cp;    /* |-|-|7|8|-|-|-|-| => |-|-|7|8|1|2|3|4| */
      *trailer    = source & 15;  /* |1|2|3|4|5|6|7|8| => |-|-|-|-|5|6|7|8| */
      *trailer_len = 4;
      break;
   case 4:
      result[0]   = *trailer << 2; /* |-|-|-|-|5|6|7|8| => |-|-|5|6|7|8|-|-| */
      source_cp   = source >> 6;   /* |1|2|3|4|5|6|7|8| => |-|-|-|-|-|-|1|2| */
      source_cp  &= 3;             /* |?|?|?|?|?|?|1|2| => |-|-|-|-|-|-|1|2| */
      result[0]  |= source_cp;     /* |-|-|5|6|7|8|-|-| => |-|-|5|6|7|8|1|2| */

      result[1]    = source & 63;  /* |1|2|3|4|5|6|7|8| => |-|-|3|4|5|6|7|8| */
      result_len  = 2;
      *trailer     = 0;
      *trailer_len = 0;
      break;
    default:
      /* we should never get here */
      PrintError(98, NULL, NULL);
      return(ERROR);
      break;
  }

  /* convert result[] to base64 values */
  if (result_len == 2)
    result[1] = GetBase64Value(result[1]);

  result[0] = GetBase64Value(result[0]);

  return(OK);
}


/*********************************/
/* Decoding function definitions */
/*********************************/

char *GetBytesFromBase64String(char *base64_string, int *byte_index, int nr_of_bytes)
{
  char *byte_string = NULL;

  /* create space for byte string */
  /* no null char, so reserve exactly nr_of_bytes chars */
  if ( (byte_string = (char *) malloc((nr_of_bytes)*sizeof(char))) == NULL) {
    PrintError(15, NULL, "GetBytesFromBase64String()");
    return(NULL);
  }

  if (strncpy(byte_string, base64_string+(*byte_index), nr_of_bytes) == NULL) {
    /* error in copying string */
    PrintError(99, NULL, "GetBytesFromBase64String()");
    return(NULL);
  }

  (*byte_index) += nr_of_bytes;
  return(byte_string);
}


int8_t ConvertBase64Value(char value)
{
  /* we have following sequences in base64 */

  /* 'A' .. 'Z'  (ASCII 65 ..  90, base64  0 ..  25 */  /* minus 65 */
  /* 'a' .. 'z'  (ASCII 97 .. 122, base64 26 ..  51 */  /* minus 71 */
  /* '0' .. '9'  (ASCII 48 ..  57, base64 52 ..  61 */  /* plus 4   */
  /* '+' and '/' (ASCII 43 and 47, base64 62 and 63 */

  if (value >= 'A' && value <= 'Z')
    return( ((int8_t) value) - 65);

  if (value >= 'a' && value <= 'z')
    return( ((int8_t) value) - 71);

  if (value >= '0' && value <= '9')
    return( ((int8_t) value) + 4);

  if (value == '+')
    return(62);

  if (value == '/')
    return(63);

  /* error decoding base64: not a bas64 character */
  PrintError(1, NULL, NULL);
  return(-1);
}


int32_t Base64ToBytes(char *base64_string, int *byte_index, int nr_of_bytes,
                      int8_t *result, int *leader_len, int8_t *leader)
{
  /* some explanation about the parameter list */
  /* nr_of_bytes:  is the number of bytes wanted in the result, not */
  /*               the number of bytes that must be converted from  */
  /*               base64_string. E.g. if we want an int64, there   */
  /*               must be 8 bytes in the result, but we must read  */
  /*               10 or 11 bytes from base64_string.               */
  /* nr_of_bits:   is the number of bits needed. Is calculated by   */
  /*               substracting leader_len from nr_of_bytes*8       */
  /* base64_nr:    is the number of bytes that must be read from    */
  /*               the base64 string.                               */
  /* result:       must be at least of size nr_of_bytes. This is    */
  /*               responsability of the caller.                    */
  /*               the caller.                                      */
  /* leader:       Possible left-over from the previous conversion. */
  /*               Will always have length 0, 2 or 4 bits.          */

  int    i             = 0;  /* to index source string with base64 bytes */
  int    j             = 0;  /* to index result string                   */
  int    base64_nr     = 0;
  int    nr_of_bits    = 0;
  int8_t next          = 0;
  char   *base64_bytes = NULL; /* malloced by GetBytesFromBase64String() */

  /* initialize result */
  for (i=0; i<nr_of_bytes; i++) {
    result[i] = 0;
  }

  /* calculate how many bytes we need from base64_string */
  nr_of_bits = (nr_of_bytes * 8) - *leader_len;
  base64_nr = nr_of_bits / 6;  /* integer division */
  if (base64_nr*6 < nr_of_bits) {
    base64_nr++;
  }

  if ( (base64_bytes = GetBytesFromBase64String(base64_string, byte_index, base64_nr)) == NULL) {
    /* error was already printed by GetBytesFromBase64String() */
    return(ERROR);
  }

  /* convert to base64 indexes */
  for (i=0; i<base64_nr; i++) {
    base64_bytes[i] = ConvertBase64Value(base64_bytes[i]);
  }

  /* now we got a series of bytes with groups of 6 base64 bits that we must concat to bytes */
  i = 0;
  while (i<base64_nr) {
    switch (*leader_len) {
      case 0:
        result[j]   = base64_bytes[i] << 2;   /* |-|-|1|2|3|4|5|6| => |1|2|3|4|5|6|-|-| */
        next        = base64_bytes[i+1] >> 4; /* |-|-|a|b|c|d|e|f| => |?|?|?|?|?|?|a|b| */
        next       &= 3;                      /* |?|?|?|?|?|?|a|b| => |-|-|-|-|-|-|a|b| */
        result[j]  |= next;                   /* |1|2|3|4|5|6|-|-| => |1|2|3|4|5|6|a|b| */
        *leader     = base64_bytes[i+1] & 15; /* |-|-|a|b|c|d|e|f| => |-|-|-|-|c|d|e|f| */
        *leader_len = 4;
        i +=2;  /* the remainder of base64_bytes[i+1] is in the leader */
        j++;
        break;
     case 2:
        result[j]   = *leader << 6;           /* |-|-|-|-|-|-|1|2| => |1|2|-|-|-|-|-|-| */
        result[j]  |= base64_bytes[i];        /* |1|2|-|-|-|-|-|-| => |1|2|a|b|c|d|e|f| */
        *leader     = 0;
        *leader_len = 0;
        i++;
        j++;
        break;
     case 4:
        result[j]  = *leader << 4;            /* |-|-|-|-|1|2|3|4| => |1|2|3|4|-|-|-|-| */
        next       = base64_bytes[i] >> 2;    /* |-|-|a|b|c|d|e|f| => |?|?|?|?|a|b|c|d| */
        next      &= 15;                      /* |?|?|?|?|a|b|c|d| => |-|-|-|-|a|b|c|d| */
        result[j] |= next;                    /* |1|2|3|4|-|-|-|-| => |1|2|3|4|a|b|c|d| */
        *leader = base64_bytes[i] & 3;       /* |-|-|a|b|c|d|e|f| => |-|-|-|-|-|-|e|f| */
        *leader_len = 2;
        i++;
        j++;
        break;
      default:
        /* we should never get here */
        PrintError(100, NULL, NULL);
        return(ERROR);
        break;
    }
  }  /* while */

  free(base64_bytes);

  return(OK);
}
