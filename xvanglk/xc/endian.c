
/* Copyright 2016, 2017 Marnix van den Bos */

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

uint16_t ntohs(uint16_t);
uint16_t htons(uint16_t);
uint32_t ntohl(uint32_t);
uint32_t htonl(uint32_t);
uint64_t ntohll(uint64_t);
uint64_t htonll(uint64_t);

/************************/
/* Function definitions */
/************************/

uint16_t ntohs(val)
 uint16_t val;
{
  uint8_t *v = (uint8_t *)&val;
  uint16_t result = 0;
  int i;

  for (i=0; i<2; i++) {
    result |= (uint16_t)v[i] << ((1-i) * 2);
  }

  return result;
}

uint16_t htons(val)
 uint16_t val;
{
  uint8_t v[8];
  uint16_t *result = (uint16_t *)v;
  int i;

  for (i=0; i<2; i++) {
    v[i] = (uint8_t)(val >> ((1-i) * 2));
  }

   return *result;
}

uint32_t ntohl(val)
 uint32_t val;
{
  uint8_t *v = (uint8_t *)&val;
  uint32_t result = 0;
  int i;

  for (i=0; i<4; i++) {
    result |= (uint32_t)v[i] << ((3-i) * 8);
  }

  return result;
}

uint32_t htonl(val)
 uint32_t val;
{
  uint8_t v[4];
  uint32_t *result = (uint32_t *)v;
  int i;

  for (i=0; i<4; i++) {
    v[i] = (uint8_t)(val >> ((3-i) * 8));
  }

   return *result;
}

uint64_t ntohll(val)
 uint64_t val;
{
  uint8_t *v = (uint8_t *)&val;
  uint64_t result = 0;
  int i;

  for (i=0; i<8; i++) {
    result |= (uint64_t)v[i] << ((7-i) * 8);
  }

  return result;
}

uint64_t htonll(val)
 uint64_t val;
{
  uint8_t v[8];
  uint64_t *result = (uint64_t *)v;
  int i;

  for (i=0; i<8; i++) {
    v[i] = (uint8_t)(val >> ((7-i) * 8));
  }

   return *result;
}

