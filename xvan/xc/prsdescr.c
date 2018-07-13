
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
#include <string.h>
#include <stdlib.h>         /* malloc(), free() */
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "prsdescr.h"


/*************************/
/* function declarations */
/*************************/

int32_t  ParseCDescrs(char**, int32_t*, FILE**, fileList**);
int32_t  ParseDescriptions(char**, int32_t*, int32_t, FILE**, fileList**);


int32_t ParseCDescrs(word, keyword, source, file_list)
 char     **word;
 int32_t  *keyword;
 FILE     **source;
 fileList **file_list;
{
  int32_t state = 1;
  int32_t id;        /* Not used, needed for GetDescrID(). */

  free(*word);
  *word = GetNextWord(keyword, NO_ID, 0, source, file_list);
  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source, file_list)) {
          break;
        }
        ErrHdr();
        PrintError(47, NULL, NULL);
        return(ERROR);
      case ERROR:
        return(ERROR);
      case DESCR_ID:
        switch (state) {
          case 1: ;
          case 3:
            state = 2;
            break;
          default:
            ErrHdr();
            PrintError(56, NULL, *word);
            return(ERROR);
        }
        if (!GetDescrId((*word)+2, &id, COMMON_DESCRS, 1))
          /* 1 means it is defined here */
          return(ERROR);

        free(*word);
        *word = GetNextWord(keyword, NO_ID, 0, source, file_list);
        break;
      case COMMA:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, "','");
            return(ERROR);
        }

        free(*word);
        *word = GetNextWord(keyword, NO_ID, 0, source, file_list);
        break;
      default:
        switch (state) {
          case 1: ;
          case 2:    /* ok to exit */
           return(OK);
          default:
            ErrHdr();
            PrintError(51, NULL, *word);
            return(ERROR);
        }
    } /* switch */
  } /* while */
}

int32_t ParseDescriptions(word, keyword, owner_id, source, file_list)
 char     **word;
 int32_t  *keyword;
 int32_t  owner_id; /* needed to look up or store the description id */
 FILE     **source;
 fileList **file_list;
{
  int32_t state = 1;
  int32_t id;            /* Needed to convert description ids.  */
  int32_t sys_descr = 0; /* System description not yet defined. */

  free(*word);
  *word = GetNextWord(keyword, owner_id, 0, source, file_list);

  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source, file_list)) {
          break;
        }
        ErrHdr();
        PrintError(47, NULL, NULL);
        return(ERROR);
      case ERROR:
        return(ERROR);
      case DESCR_ID:
        switch (state) {
          case 1: ; /* same as 5 */
          case 5:
            if (strcmp(*word, "d_sys") == 0) {
              sys_descr = 1;
              state = 3;
            }
            else
              state = 2;
            break;
          case 4:
          case 8:
            state = 7;
            break;
          default:
            ErrHdr();
            PrintError(56, NULL, *word);
            return(ERROR);
        }
        if (!GetDescrId((*word)+2, &id, owner_id, 1))
          /* 1 means it is defined here */
          return(ERROR);

        if (!StoreId(id))
          return(ERROR);

        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source, file_list);
        break;
      case COMMA:
        switch (state) {
          case 4:
            state = 6;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, "','");
            return(ERROR);
        }
        /* do nothing */
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source, file_list);
        break;
      case STRING:
        switch (state) {
          case 2:
            if (!StoreString(*word))
              return(ERROR);
            state = 5;
            break;
          case 3:
            /* it's the first d_sys string */
            if (!StoreString(*word))
              return(ERROR);
            /* no break here, code for case 6 must be executed for 3 as well */
          case 6:
            /* It's a system description.                   */
            /* All system descriptions are stored in the    */
            /* location or object directory. Only the first */
            /* d_sys is available for printing (also see    */
            /* next comment.                                */
            /* nov 2nd 2015, still true, but we changed the */
            /* interpreter's print() functions so they will */
            /* use PrintId() to print System Descriptions.  */
            /* This because System Descriptions in          */
            /* descrInfo can switch places (see interpreter */
            /* code for printing descriptions.              */

            if (IsLocId(owner_id)) {
              if (!ProcLocDSys(*word, owner_id))
                return(ERROR);
            }
            else
              if (!ProcObjDSys(*word, owner_id))
                return(ERROR);
            state = 4;
            break;
          case 7:
            if (!StoreString(*word))
              return(ERROR);
            state = 8;
            break;
          default:
             ErrHdr();
             PrintError(49, NULL, *word);
             return(ERROR);
         }

         /* We allow more than one d_sys description to enable to  */
         /* refer to locations and objects in more than one way.   */
         /* However, only the first d_sys string is stored as a    */
         /* printable description (printcr(d_sys)). The others are */
         /* only stored in the location and object directory so    */
         /* they can be used to identify the location or object.   */

         /* state 6 denotes that the string belongs to a second or */
         /* later d_sys and that it must not be stored as a        */
         /* description.                                           */

         free(*word);
         *word = GetNextWord(keyword, owner_id, 0, source, file_list);
         break;
      default:
        switch (state) {
          case 4:
          case 5:
          case 8: /* ok to exit */
            /* Warning in case no d_sys defined */
            if (sys_descr == 0)
              ;
              /* printf("Warning: No system description defined!\n");*/
            return(OK);
          default:
            ErrHdr();
            PrintError(51, NULL, *word);
            return(ERROR);
        }
    } /* switch */
  } /* while */
}

