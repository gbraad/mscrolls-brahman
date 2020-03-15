
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
/*                                                                      */
/* <marnix@xvan.nl>                                                     */
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
#include <stdlib.h>     /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "prseloc.h"

/*************************/
/* function declarations */
/*************************/

int32_t ParseExits(char**, int32_t*, int32_t, FILE**, fileList**);
int32_t ParseLoc(int64_t, FILE**, fileList**);


/************************/
/* function definitions */
/************************/

int32_t ParseExits(char **word, int32_t *keyword, int32_t owner_id, FILE **source, fileList **file_list)
 /* owner_id is the location that exits belong to. */
{
  int32_t state = 1;
  int32_t id;              /* needed to convert location ids */
  int32_t direction = 0;   /* needed to fill right place in exit_arr */

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
        PrintError(47, NULL, "ParseExits()");
        return(ERROR);
      case ERROR:
        return(ERROR);
      case ARROW:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, "->");
            return(ERROR);
        }
        /* skip the arrow */
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source, file_list);
        break;
      case LOC_ID:
        switch (state) {
          case 3:
            state = 4;
            break;
          default:
            ErrHdr();
            PrintError(59, NULL, *word);
            return(ERROR);
        }
        if (!GetLocationId((*word)+2, &id, 0, 0))
          return(ERROR);

        if (!ProcExit(owner_id, direction - first_direction_id, id))
          /* id denotes next loc */
          return(ERROR);
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source, file_list);
        break;
      default:
        /* word may be an exit */
        if (IsDirection(*word, &direction)) {
          switch (state) {
            case 1:
             state = 2;
             break;
            case 4:
              state = 2;
              break;
            default:
              ErrHdr();
              PrintError(75, NULL, *word);
              return(ERROR);
          } /* switch */
          free(*word);
          *word = GetNextWord(keyword, owner_id, 0, source, file_list);
        } /* if */
        else {
          switch (state) {
            case 1: ; /* same as 4 */
            case 4:
              break;
            default:
              ErrHdr();
              PrintError(48, NULL, *word);
              return(ERROR);
          } /* switch */
          return(OK);
        } /* else */
    } /* switch */
  } /* while */
}


int32_t ParseLoc(int64_t offset, FILE **source, fileList **file_list)
{
  char     **word = (char **) malloc(sizeof(char *));
  int32_t  keyword;
  int32_t  id    = NO_ID; /* needed to convert loc id; also used as owner_id */
  int32_t  state = 1;

  /* Due to a removed keyword, state 5 is unused. */

  *word = GetNextWord(&keyword, id, 0, source, file_list);
  while (1) {
    switch (keyword) {
      case EOF:
        ErrHdr();
        PrintError(47, NULL, "ParseLoc()");
        return(ERROR);
      case ERROR:
        return(ERROR);
      case LOC_ID:
        /*printf("%s ", (*word)+2);*/   /* Skip 'l_' */
        printf(".");
        switch (state) {
          case 1:
            state = 2;
            break;
          default:
            ErrHdr();
            PrintError(59, NULL, *word);
            return(ERROR);
        }
        /* Get a new location id and store the offset in the */
        /* location table.                                   */
        if (!GetLocationId((*word)+2, &id, 1, offset))
          return(ERROR);

        /* No need to store the location id. Locations will be */
        /* accessed only via the locations directory.          */
        free(*word);
        *word = GetNextWord(&keyword, id, 0, source, file_list);
      case DESCRIPTIONS:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("DESCRIPTIONS"));
            return(ERROR);
        }
        if (!ParseDescriptions(word, &keyword, id, source, file_list)) /* id denotes owner */
          return(ERROR);
        /* next word is returned by ParseDescriptions() */

        break;
      case EXITS:
        switch (state) {
          case 3:
            state = 4;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("EXITS"));
            return(ERROR);
        }
        /* EXITS keyword will be stored by ParseExits() */
        /* since EXITS clause may be empty */
        if (!ParseExits(word, &keyword, id, source, file_list))
          return(ERROR);
        /* next word is returned by ParseExits() */

        break;
      case FLAGS:
        switch (state) {
          case 3: ; /* same as 4 */
          case 4:
            state = 6;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("FLAGS"));
            return(ERROR);
        }
        if (!ParseFlags(word, &keyword, id, source, file_list)) /* id denotes owner */
          return(ERROR);

        /* next word is returned by ParseFlags */
        break;
      case ATTRIBUTES:
        switch (state) {
          case 3: ; /* added on 10may2018 */
          case 4: ; /* same as 6 */
          case 6:
            state = 8;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("ATTRIBUTES"));
            return(ERROR);
        }
        /* id denotes the owner here. */
        if (!ParseAttributes(word, &keyword, id, source, file_list)) /* was ParseLAttributes() */
          return(ERROR);

        /* Next word is returned by ParseAttributes(). */
        break;
      case TRIGGERS:
        switch (state) {
          case 3: ; /* same as 4 */
          case 4: ; /* same as 6 */
          case 6: ; /* same as 8 */
          case 8:
            state = 7;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("TRIGGERS"));
            return(ERROR);
        }
        if (!ParseTriggers(word, &keyword, id, source, file_list)) /* id denotes owner */
          return(ERROR);

        /* next word is returned by ParseTriggers() */
        break;
      case END_LOC:
        switch (state) {
          case 3: ; /* same as 4 */
          case 4: ; /* same as 6 */
          case 6: ; /* same as 7 */
          case 7: ; /* same as 8 */
          case 8:
            break;  /* ready */
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("END_LOC"));
            return(ERROR);
        }
        if (!StoreKeyword(END_LOC))
          return(ERROR);
        return(OK);
      default:
        ErrHdr();
        PrintError(51, NULL, *word);
        return(ERROR);
    } /* switch */
  } /* while */
}
