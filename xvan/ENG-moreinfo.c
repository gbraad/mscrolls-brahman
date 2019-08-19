
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
#include <stdlib.h>   /* malloc() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "trnslate.h"
#include "json.h"
#include "ifi.h"

#include "ENG-moreinfo.h"

/*************************/
/* Function declarations */
/*************************/

void ENG_MoreInfo(extendedSysDescr*, match*, char*);
void ENG_PrintNotFound(extendedSysDescr*);

/************************/
/* Function definitions */
/************************/

void ENG_MoreInfo(extendedSysDescr *descr, match *hits, char *line_buf)
{
  extendedSysDescr *sd;
  kvPair           kv = {NULL, {0, NULL, 0, 0}};
  int32_t          i =0;

  /* do not print a capital */
  capital = 0;

  if ((descr->part1).noun == NO_ID) {
    PrintString("What", 0);
    Output();
  }
  else {
    PrintString("Which ", 0);
    Output();
    PrintExtendedSysDescr(descr, 0);
  }
  PrintString(" do you mean?\nThe ", 0);
  Output();

  for (i=0; i<hits->nr_of_hits; i++) {
    if (i != 0 && i != hits->nr_of_hits-1) {
      PrintString(", the ", 0);
      Output();
    }
    else if (i == hits->nr_of_hits-1) {
      PrintString(" or the ", 0);
      Output();
    }
    if (IsLocId(hits->matched_objs[i]))
      sd = loc_dir[hits->matched_objs[i]-FIRST_LOCATION_ID].descr;
    else
      /* It must be an object; forget the test. */
      sd = obj_dir[hits->matched_objs[i]-FIRST_OBJECT_ID].descr;
    PrintExtendedSysDescr(sd, 0);
  }

  PrintString("?\n", 0);
  Output();

  GetAddtlInput(&kv, prompt, IFI_REQ_COMMAND, 1);
  /* 1 means do not process other jsons   */
  /* what we want is in the kv textstring */
  strncpy(line_buf, kv.value.textstring, INPUT_LINE_LEN);
  line_buf[INPUT_LINE_LEN-1] = '\0';
  ResetKVPair(&kv);  /* free mallocs */
}


void ENG_PrintNotFound(extendedSysDescr *descr)
{
  if (actor == PLAYER) {
    PrintString("You don't ", 0);
  }
  else {
    PrintId(actor, 0);
    PrintString(" doesn't ", 0);
  }
  PrintString("see that here.\n", 0);
  Output();
}
