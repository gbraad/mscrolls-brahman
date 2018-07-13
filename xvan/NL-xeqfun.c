
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018 Marnix van den Bos.                   */
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
#include "NL-xeqfun.h"


/*************************/
/* Function declarations */
/*************************/

int32_t NL_XeqYesNo(void);


/****************************/
/* Testfunction definitions */
/****************************/

int32_t NL_XeqYesNo()
{
  char yes_or_no[INPUT_LINE_LEN];

  while (1) {
    GetAddtlInput(yes_or_no, "j\\n");
    xv_strlwr(yes_or_no);

    if ((strcmp(yes_or_no, "ja") == 0) || (strcmp(yes_or_no, "j") == 0))
      return(OK);
    else
      if ((strcmp(yes_or_no, "nee") == 0) || (strcmp(yes_or_no, "n") == 0))
        return(ERROR);
      else
        PrintString("Ja of nee:", 0);
  }
}