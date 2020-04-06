
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
#include "prseobj.h"

/*************************/
/* function declarations */
/*************************/

int32_t PContIn(char**, int32_t*, int32_t, FILE**, fileList**);
int32_t ParseObj(int64_t, FILE**, fileList**);


/************************/
/* function definitions */
/************************/

int32_t PContIn(char **word, int32_t *keyword, int32_t owner, FILE **source, fileList **file_list)
{
  int32_t state = 1;
  int32_t id;         /* used to convert object id or location id */
  int32_t voc_word_id;
  int32_t cont = 1;

  /* *word will be CONTAINED               */

  /* Set voc_word_id to default value: in. */
  voc_word_id = LookUpId("in");

  while (cont) {
    /* read the next word from the inputfile */
    free(*word);
    *word = GetNextWord(keyword, owner, 0, source, file_list);
    switch (*keyword) {
      /* No case for NO_KEYWORD, since we may get a preposition. */
      case EOF:
        ErrHdr();
        PrintError(47, NULL, "PContIn()");
        return(ERROR);
      case ERROR:
        return(ERROR);
      case LOC_ID:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            if (state == 1)
              PrintError(76, NULL, *word);
            else
              PrintError(59, NULL, *word);
            return(ERROR);
        }
        if (!GetLocationId((*word)+2, &id, 0, 0))
          return(ERROR);
        break;
      case OBJ_ID:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            if (state == 1)
              PrintError(76, NULL, *word);
            else
              PrintError(59, NULL, *word);
            return(ERROR);
        }
        if (!GetObjectId((*word)+2, &id, 0, 0))
          return(ERROR);
        break;
      default:
        switch (state) {
          case 1:
            /* It should be a preposition. */
            if ((voc_word_id = LookUpId(*word)) == NO_ID) {
              ErrHdr();
              PrintError(77, NULL, *word);
              free(*word);
              return(ERROR);
            }

            /* Ok, it's a word from the vocabulary. */
            /* Check whether it's a preposition.    */
            if (!HasType(voc_word_id, PREPOSITIONS)) {
              ErrHdr();
              PrintError(78, NULL, NULL);
              free(*word);
              return(ERROR);
            }

            /* Ok, it's a preposition. */
            state = 2;
            break;
          case 3:
            cont = 0;  /* stop */
            break;
          default:
            ErrHdr();
            PrintError(51, NULL, *word);
            return(ERROR);
        } /* switch */

        break;
    } /* switch */
  } /* while */

  /* Now update the containment data */
  /* Owner is contained in id. */
  if (!ProcCIData(owner, id, voc_word_id))
    return(ERROR);

  /* Id contains owner.        */
  if (!ProcCData(id, owner))
    return(ERROR);

  /* Ready. */
  return(OK);
}


int32_t ParseObj(int64_t offset, FILE **source, fileList **file_list)
{
  char    **word = (char **) malloc(sizeof(char *));
  int32_t keyword;
  int32_t id    = 0;  /* needed to convert object id; also used as owner_id */
  int32_t state = 1;

  /* Due to a removed keyword, state 4 is not used. */

  *word = GetNextWord(&keyword, id, 0, source, file_list);
  while (1) {
    switch (keyword) {
      case EOF:
        ErrHdr();
        PrintError(47, NULL, "ParseObj()");
        return(ERROR);
      case ERROR:
        return(ERROR);
      case OBJ_ID:
        /* printf("%s ", (*word)+2); */   /* Skip 'o_' */
        printf(".");
        switch (state) {
          case 1:
            state = 2;
            break;
          default:
            ErrHdr();
            PrintError(60, NULL, *word);
            return(ERROR);
        }
        /* get a new object id and store the offset in the */
        /* object table */
        if (!GetObjectId((*word)+2, &id, 1, offset))
          return(ERROR);
        /* No need to store the id in the outputfile, since */
        /* objects will only be accessed through the object */
        /* directory.                                       */
        free(*word);
        *word = GetNextWord(&keyword, id, 0, source, file_list);
        break;
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
      case CONTAINED:
        switch (state) {
          case 3:
            state = 5;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("CONTAINED"));
            return(ERROR);
        }
        if (!PContIn(word, &keyword, id, source, file_list))
          return(ERROR);
        /* The next word will be returned by PContIn(). */
        break;
      case FLAGS:
        switch (state) {
          case 5:
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
          case 5: ;
          case 6:
            state = 8;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("ATTRIBUTES"));
            return(ERROR);
        }
        /* id denotes the owner here. */
        if (!ParseAttributes(word, &keyword, id, source, file_list))   /* was ParseLAttributes */
          return(ERROR);
        /* Next word is returned by ParseAttributes(). */
        break;
      case TRIGGERS:
        switch (state) {
          case 5: ; /* same as 6 */
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
      case END_OBJ:
        switch (state) {
          case 5: ; /* same as 6 */
          case 6: ; /* same as 7 */
          case 7: ; /* same as 8 */
          case 8:
            break;  /* ready */
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("END_OBJ"));
            return(ERROR);
        }
        if (!StoreKeyword(END_OBJ))
          return(ERROR);
        return(OK);
      default:
        ErrHdr();
        PrintError(48, NULL, *word);
        return(ERROR);
    } /* switch */
  } /* while */
}
