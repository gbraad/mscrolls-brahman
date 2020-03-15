
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
#include <stdlib.h>    /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "init.h"

/***********/
/* globals */
/***********/

int32_t endian = 0;

/*************************/
/* Function declarations */
/*************************/

uint16_t xv_ntohs(uint16_t);
uint16_t xv_htons(uint16_t);
uint32_t xv_ntohl(uint32_t);
uint32_t xv_htonl(uint32_t);
uint64_t xv_ntohll(uint64_t);
uint64_t xv_htonll(uint64_t);

/************************/
/* Function definitions */
/************************/

uint16_t xv_ntohs(uint16_t val)
{
  uint8_t  *v = (uint8_t *)&val;
  uint16_t result = 0;
  int      i;

  for (i=0; i<2; i++) {
    result |= (uint16_t)v[i] << ((1-i) * 8);
  }

  return result;
}


uint16_t xv_htons(uint16_t val)
{
  uint8_t  v[8];
  uint16_t *result = (uint16_t *)v;
  int      i;

  for (i=0; i<2; i++) {
    v[i] = (uint8_t)(val >> ((1-i) * 8));
  }

   return *result;
}


uint32_t xv_ntohl(uint32_t val)
{
  uint8_t  *v = (uint8_t *)&val;
  uint32_t result = 0;
  int      i;

  for (i=0; i<4; i++) {
    result |= (uint32_t)v[i] << ((3-i) * 8);
  }

  return result;
}


uint32_t xv_htonl(uint32_t val)
{
  uint8_t  v[4];
  uint32_t *result = (uint32_t *)v;
  int      i;

  for (i=0; i<4; i++) {
    v[i] = (uint8_t)(val >> ((3-i) * 8));
  }

   return *result;
}


uint64_t xv_ntohll(uint64_t val)
{
  uint8_t  *v = (uint8_t *)&val;
  uint64_t result = 0;
  int      i;

  for (i=0; i<8; i++) {
    result |= (uint64_t)v[i] << ((7-i) * 8);
  }

  return result;
}


uint64_t xv_htonll(uint64_t val)
{
  uint8_t  v[8];
  uint64_t *result = (uint64_t *)v;
  int      i;

  for (i=0; i<8; i++) {
    v[i] = (uint8_t)(val >> ((7-i) * 8));
  }

   return *result;
}
