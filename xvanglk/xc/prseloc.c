
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>     /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "prseloc.h"

/*************************/
/* function declarations */
/*************************/

int32_t ParseExits(char**, int32_t*, int32_t, FILE**);
int32_t ParseLoc(int64_t, FILE**);

int32_t ParseExits(word, keyword, owner_id, source)
 char    **word;
 int32_t *keyword;
 int32_t owner_id;  /* Location that exits belong to. */
 FILE    **source;  /* Source file to read from.      */
{
  int32_t state = 1;
  int32_t id;              /* needed to convert location ids */
  int32_t direction = 0;   /* needed to fill right place in exit_arr */

  free(*word);
  *word = GetNextWord(keyword, owner_id, 0, source);

  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source)) {
          break;
        }
        ErrHdr();
        printf("ParseExits(): unexpected end of inputfile\n");
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
            printf("\nLine %d: unexpected ARROW keyword\n", line_num);
            return(ERROR);
        }
        /* skip the arrow */
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;
      case LOC_ID:
        switch (state) {
          case 3:
            state = 4;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected location id: %s\n", line_num, *word);
            return(ERROR);
        }
        if (!GetLocationId((*word)+2, &id, 0, 0))
          return(ERROR);

        if (!ProcExit(owner_id, direction - first_direction_id, id))
          /* id denotes next loc */
          return(ERROR);
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
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
              printf("\nLine %d: unexpected exit: %s\n", line_num, *word);
              return(ERROR);
          } /* switch */
          free(*word);
          *word = GetNextWord(keyword, owner_id, 0, source);
        } /* if */
        else {
          switch (state) {
            case 1: ; /* same as 4 */
            case 4:
              break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected keyword: %s\n", line_num, *word);
              return(ERROR);
          } /* switch */
          return(OK);
        } /* else */
    } /* switch */
  } /* while */
}

int32_t ParseLoc(offset, source)
 int64_t offset;  /* location's position in datafile */
 FILE **source;   /* sourcefile to read from         */
{
  char    **word = (char **) malloc(sizeof(char *));
  int32_t keyword;
  int32_t id    = NO_ID; /* needed to convert loc id; also used as owner_id */
  int32_t state = 1;

  /* Due to a removed keyword, state 5 is unused. */

  *word = GetNextWord(&keyword, id, 0, source);
  while (1) {
    switch (keyword) {
      case EOF:
        ErrHdr();
        printf("ParseLoc(): unexpected EOF\n");
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
            printf("\nLine %d: unexpected location identifier %s\n",
                    line_num, *word);
            return(ERROR);
        }
        /* Get a new location id and store the offset in the */
        /* location table.                                   */
        if (!GetLocationId((*word)+2, &id, 1, offset))
          return(ERROR);

        /* No need to store the location id. Locations will be */
        /* accessed only via the locations directory.          */
        free(*word);
        *word = GetNextWord(&keyword, id, 0, source);
        break;
      case DESCRIPTIONS:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected DESCRIPTIONS keyword\n", line_num);
            return(ERROR);
        }
        if (!ParseDescriptions(word, &keyword, id, source)) /* id denotes owner */
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
            printf("\nLine %d: unexpected EXITS keyword\n", line_num);
            return(ERROR);
        }
        /* EXITS keyword will be stored by ParseExits() */
        /* since EXITS clause may be empty */
        if (!ParseExits(word, &keyword, id, source))
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
            printf("\nLine %d: unexpected FLAGS keyword\n", line_num);
            return(ERROR);
        }
        if (!ParseFlags(word, &keyword, id, source)) /* id denotes owner */
          return(ERROR);

        /* next word is returned by ParseFlags */
        break;
      case ATTRIBUTES:
        switch (state) {
          case 4: ; /* same as 6 */
          case 6:
            state = 8;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected ATTRIBUTES keyword\n", line_num);
            return(ERROR);
        }
        /* id denotes the owner here. */
        if (!ParseAttributes(word, &keyword, id, source)) /* was ParseLAttributes() */
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
            printf("\nLine %d: unexpected TRIGGERS keyword\n", line_num);
            return(ERROR);
        }
        if (!ParseTriggers(word, &keyword, id, source)) /* id denotes owner */
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
            printf("\nLine %d: unexpected END_LOC keyword\n", line_num);
            return(ERROR);
        }
        if (!StoreKeyword(END_LOC))
          return(ERROR);
/*        printf("OK\n");*/
        return(OK);
      default:
        ErrHdr();
        printf("\nLine %d: unexpected word %s\n", line_num, *word);
        return(ERROR);
    } /* switch */
  } /* while */
}
