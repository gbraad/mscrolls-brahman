
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>   /* malloc() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "output.h"

/*************************/
/* Function declarations */
/*************************/
void ErrHdr(void);

void ErrHdr()
{
  printf("\n\n\n******** Error ********\n\n");
  printf("File: %s\n", current_filename);
}
