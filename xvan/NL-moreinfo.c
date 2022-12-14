
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
#include <stdlib.h>   /* malloc() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "json.h"
#include "ifi.h"
#include "NL-moreinfo.h"

/*************************/
/* Function declarations */
/*************************/

void NL_MoreInfo(extendedSysDescr*, match*, char*);
void NL_PrintNotFound(extendedSysDescr*);

/************************/
/* Function definitions */
/************************/

void NL_MoreInfo(extendedSysDescr *descr, match *hits, char *line_buf)
{
  extendedSysDescr *sd;
  kvPair           kv = {NULL, {0, NULL, 0, 0}};
  int32_t          i  = 0;

  /* do not print a capital */
  capital = 0;

  PrintString("Bedoel je ", 0);

  for (i=0; i<hits->nr_of_hits; i++) {
    if (IsLocId(hits->matched_objs[i])) {
      sd = loc_dir[hits->matched_objs[i]-FIRST_LOCATION_ID].descr;
    }
    else {
      /* It must be an object; forget the test. */
      sd = obj_dir[hits->matched_objs[i]-FIRST_OBJECT_ID].descr;
    }

    if (i==0) {
      NL_PrintArticle(&(sd->part1), ARTICLE_THE, 0);
      PrintString(" ", 0);
    }

    if (i != 0 && i != hits->nr_of_hits-1) {
      PrintString(", ", 0);
      NL_PrintArticle(&(sd->part1), ARTICLE_THE, 0);
      PrintString(" ", 0);
    }
    else if (i == hits->nr_of_hits-1) {
      PrintString(" of ", 0);
      NL_PrintArticle(&(sd->part1), ARTICLE_THE, 0);
      PrintString(" ", 0);
    }
    PrintExtendedSysDescr(sd, 0);
  }

  PrintString("?\n", 0);

  GetAddtlInput(&kv, prompt, IFI_REQ_COMMAND, 1);
  /* 1 means do not process other jsons   */
  /* what we want is in the kv textstring */
  strncpy(line_buf, kv.value.textstring, INPUT_LINE_LEN);
  line_buf[INPUT_LINE_LEN-1] = '\0';
  ResetKVPair(&kv);  /* free mallocs */
}


void NL_PrintNotFound(extendedSysDescr *descr)
{
  if (actor == PLAYER) {
    PrintString("Je", 0);
  }
  else {
  }
  PrintString(" ziet dat hier niet.\n", 0);
  Output();
}
