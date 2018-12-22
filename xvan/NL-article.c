

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
#include <stdlib.h>   /* malloc()  */
#include <string.h>
#include <ctype.h>    /* toupper() */
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "NL-article.h"


/*************************/
/* Function declarations */
/*************************/

void NL_PrintArticle(sysDescr*, int32_t, int);

/************************/
/* Function definitions */
/************************/

void NL_PrintArticle(descr, type, use_json)
 sysDescr *descr;
 int32_t  type;
 int use_json;
{
  if (descr->article != NO_ID) {
    switch (type) {
      case ARTICLE_THE:
        if (descr->article != NO_ID) {
          /* clear article or else the white space will */
          /* not be printed by PrintString()            */
          article = 0;
          PrintWord(descr->article, use_json); /* 'de','het' or 'een' */
          PrintString(" ", use_json);
        }
        break;

      case ARTICLE_A:
        if (descr->article != NO_ID) {
          /* clear article or else the white space will */
          /* not be printed by PrintString()            */
          article = 0;
          PrintString("een ", use_json);
        }
        break;

      default:
        /* There is an article in the system description,    */
        /* but no [a] or [the] in the string, so don't print */
        /* the article.                                      */
        break;
    } /* switch */
  }  /* if */
  /* july 6th 2028: added this because 'use' skipped the space */
  /* when the object had no noun. Must check if this is the    */
  /* right solution                                            */
  else
    article = 0;
}
