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
#include <stdlib.h>   /* malloc()  */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "descr2str.h"

/*************************/
/* function declarations */
/*************************/

char *AddToString(char*, char*);
char *GetWord(int32_t);
char *GetArticle(sysDescr*);
char *GetSysDescr(sysDescr*, int);
char *GetExtendedSysDescr(extendedSysDescr*, int);
char *GetId(int32_t, int);

/************************/
/* function definitions */
/************************/

char *AddToString(char *dest, char *to_add)
{
  /* dest must be malloced */

  int len = 0;

  /* 21may2018: design decision: in case   */
  /* dest is NULL, we will first create an */
  /* empty string.                         */
  if (dest == NULL) {
    dest = ExpandString(dest, 0);
  }

  if (to_add == NULL) {
    return(dest);
  }

  if ( (len = strlen(to_add)) == 0 ) {
    return(dest);
  }

  /* ExpandString() will take care of '\0' in dest */
  if ( !(dest = ExpandString(dest, len))) {
    return(NULL);
  }

  strncat(dest, to_add, len);

  return(dest);
}


char *GetArticle(sysDescr *sys_descr)
{
  char *article = NULL;

  if (sys_descr->article != NO_ID) {
    /* there is an article */
    if ( !(article = GetWord(sys_descr->article)) ) {
      return(NULL);
    }
  }
  else {
    /* there is no article */
    /* we must return something other than NULL */
    if ( !(article = (char *) malloc(sizeof(char))) ) {
      return(NULL);
    }
    article[0] = '\0';
  }

  return(article);
}


char *GetWord(int32_t id)
{
  char      *word = NULL;
  wordTable *wt   = word_table;
  int       len   = 0;
  int32_t   i     = 0;

  /* in case of NONE we must print a white space,  */
  /* except when we are at the beginning of a line */

  /* Find id. */
  while (i<nr_of_words && wt[i].id != id)
    i++;

  if (i == nr_of_words) {
    return(NULL);
  }
  else {
    len = strlen(wt[i].print_word);
    if ( (word = (char *) malloc((len+1)*sizeof(char))) == NULL ) {
      return(NULL);
    }
    strcpy(word, wt[i].print_word);
  }
  return(word);
}


char *GetSysDescr(sysDescr *descr, int with_article)
{
  char    *description = NULL;
  char    *temp        = NULL;
  int32_t i = 0;

  /* Caller must print the leading space, if needed.           */
  /* Since we have a recursive syntax check, the adjectives    */
  /* have been inserted backwards (last one first). Therefore, */
  /* we will print the adjectives in reverse order.            */

  /* get the article (or not) */
  if (with_article) {
    if ( ((description = GetArticle(descr)) == NULL) ) {
      return(NULL);
    }
  }

  if (strlen(description) != 0) {
    /* there is an article, add a space */
    if ( !(description = AddToString(description, " ")) ) {
      return(NULL);
    }
  }

  for (i=descr->nr_of_adjectives-1; i>=0; i--) {
    if ( !(temp = GetWord(descr->adjectives[i])) ) {
      return(NULL);
    }
    if ( !(description = AddToString(description, temp)) ) {
      return(NULL);
    }
    free(temp);
    if ( !(description = AddToString(description, " ")) ) {
      return(NULL);
    }
  }

  if (descr->noun != NO_ID) {
    if ( !(temp = GetWord(descr->noun)) ) {
      return(NULL);
    }
    if ( !(description = AddToString(description, temp)) ) {
      return(NULL);
    }
    free(temp);
  }
  return(description);
}


char *GetExtendedSysDescr(extendedSysDescr *extended_sys_descr, int with_article)
{
  char *description = NULL;
  char *temp        = NULL;

  /* with_article = 0 means do not include the article */
  if ( !(description = GetSysDescr(&(extended_sys_descr->part1), with_article)) ) {
    return(NULL);
  }

  if (extended_sys_descr->connect_prepos != NO_ID) {
    if ( !(description = AddToString(description, " ")) ) {
      return(NULL);
    }

    if ( !(temp = GetWord(extended_sys_descr->connect_prepos)) ) {
      return(NULL);
    }

    if ( !(description = AddToString(description, temp))) {
      return(NULL);
    }
    free(temp);
    temp = NULL;
    if ( !(description = AddToString(description, " ")) ) {
      return(NULL);
    }

    /* 1 means also get the article */
    if ( !(temp = GetSysDescr(&(extended_sys_descr->part2), 1)) ) {
      return(NULL);
    }
    if ( !(description = AddToString(description, temp)) ) {
      return(NULL);
    }
  }
  return(description);
}


char *GetId(int32_t id, int with_article)
{
  int32_t  offset      = 0;
  int32_t  type        = NO_TYPE;
  dirInfo  *dir        = NULL;
  char     *text_descr = NULL;

  /* id must be a location or an object id */

  /* Convert in case it is a special id. */
  ConvSpecId(&id, &type);

  if (IsLocId(id)) {
    offset = id - FIRST_LOCATION_ID;
    dir = loc_dir;
  }
  else {
    if (IsObjId(id)) {
      offset = id - FIRST_OBJECT_ID;
      dir = obj_dir;
    }
    else {
      return(NULL);
    }
  }

  if (dir[offset].nr_of_dsys == 0) {
    if ( !(text_descr = ExpandString(text_descr, 0)) ) {
      return(NULL);
    }
  }
  else {
    if ( !(text_descr = GetExtendedSysDescr(&(dir[offset].descr[0]), with_article)) ) {
      return(NULL);
    }
  }
  return(text_descr);
}
