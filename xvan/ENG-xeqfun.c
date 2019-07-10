
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
#include "json.h"
#include "ifi.h"
#include "ENG-xeqfun.h"

extern const char *ifi_getRequest(void);

/*************************/
/* Function declarations */
/*************************/

int32_t ENG_XeqYesNo(void);
void    ENG_XeqHitAnyKey(void);

/****************************/
/* Testfunction definitions */
/****************************/

int32_t ENG_XeqYesNo(void)
{
  char yes_or_no[INPUT_LINE_LEN];

  while (1) {
    GetAddtlInput(yes_or_no, "y/n: ", IFI_REQ_COMMAND);
    xv_strlwr(yes_or_no);

    if ((strcmp(yes_or_no, "yes") == 0) || (strcmp(yes_or_no, "y") == 0))
      return(OK);
    else
      if ((strcmp(yes_or_no, "no") == 0) || (strcmp(yes_or_no, "n") == 0))
        return(ERROR);
      else
        PrintString("Please enter Yes or No:", 0);
  }
}


void ENG_XeqHitAnyKey(void)
{
  char response_txt[INPUT_LINE_LEN];
Log("In ENG-hitanykey()\n", "", "");
  /* send the choice */
  ifi_emitResponse("{\"choice\":[{\"text\":{\"text\":\"Hit enter...\",\"color\":\"blue\"},\"chosen\":\"{\\\"keyhit\\\":true}\"}]}");

  /* now wait for a key to be hit */
  GetAddtlInput(response_txt, "", IFI_REQ_KEYHIT);
Log("ENG_XeqHitAnyKey(): na GetAddtlInput(), responsetext is: ", response_txt, "\n");
  return;
}

