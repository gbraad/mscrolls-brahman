
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
#include "NL-xeqfun.h"

/*************************/
/* Function declarations */
/*************************/

int32_t NL_XeqYesNo(void);
<<<<<<< HEAD
=======
void    NL_XeqHitAnyKey(void);
>>>>>>> 72d7449e33257b77bc124b16a988a408eddcf5b1

/****************************/
/* Testfunction definitions */
/****************************/

int32_t NL_XeqYesNo(void)
{
  char   yes_or_no[INPUT_LINE_LEN];
  kvPair kv = {NULL, {0, NULL, 0, 0}};

  while (1) {
    GetAddtlInput(&kv, "j/n: ", IFI_REQ_COMMAND, 1);
    /* 1 means do not process other jsons   */
    /* what we want is in the kv textstring */
    strncpy(yes_or_no, kv.value.textstring, INPUT_LINE_LEN);
    yes_or_no[INPUT_LINE_LEN-1] = '\0';
    ResetKVPair(&kv);  /* free mallocs */
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


void NL_XeqHitAnyKey(void)
{
  kvPair kv = {NULL, {0, NULL, 0, 0}};

  /* send the choice */
  ifi_emitResponse("{\"choice\":[{\"text\":{\"text\":\"Toets enter...\",\"color\":\"blue\"},\"chosen\":\"{\\\"keyhit\\\":true}\"}]}");

  /* now wait for a key to be hit */
  GetAddtlInput(&kv, "", IFI_REQ_KEYHIT, 1);
  /* 1 means do not process other jsons      */
  /* we don't need anything from the kv-pair */
  ResetKVPair(&kv);  /* free mallocs */

  return;
}
