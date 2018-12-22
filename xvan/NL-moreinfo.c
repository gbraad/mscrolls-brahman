
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
#include <stdlib.h>   /* malloc() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "NL-moreinfo.h"

/*************************/
/* Function declarations */
/*************************/

void NL_MoreInfo(extendedSysDescr*, match*, char*);
void NL_PrintNotFound(extendedSysDescr*);

/************************/
/* Function definitions */
/************************/

void NL_MoreInfo(descr, hits, line_buf)
 extendedSysDescr *descr;
 match            *hits;
 char             *line_buf;
{
  extendedSysDescr *sd;
  int32_t          i =0;

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

  GetAddtlInput(line_buf, "> ");
}


void NL_PrintNotFound(descr)
 extendedSysDescr *descr;
{
  if (actor == PLAYER) {
    PrintString("Je", 0);
  }
  else {
  }
  PrintString(" ziet dat hier niet.\n", 0);
}
