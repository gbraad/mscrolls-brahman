
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>     /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "storyinf.h"

/*************************/
/* function declarations */
/*************************/

int32_t ReadStoryInfo(storyInfo*, char*);


/*************************/
/* function definitions  */
/*************************/

int32_t ReadStoryInfo(info, filename)
 storyInfo *info;
 char      *filename;
{
  char    *word   = NULL;
  int32_t keyword = NO_KEYWORD;
  int32_t state   = 0;
  int32_t exit    = 0;

  printf("\n\nReading story information\n\n");

  /* Word hasn't been malloc'ed yet; before all calls to   */
  /* GetNextWord() after this one, word must be free'ed in */
  /* order to keep memory clean.                           */

  word = GetNextWord(&keyword, NO_ID, 0, &source_file);

  while (!exit) {
    switch (keyword) {
      case NO_KEYWORD:
        ErrHdr();
        printf("\nLine %d: illegal word %s\n", line_num, word);
        return(ERROR);
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 0, &source_file)) {
          break;
        }
        ErrHdr();
        printf("Unexpected end of inputfile\n");
        return(ERROR);
        /* no break needed here */
      case TITLE:
        switch (state) {
          case 0:
            state = 1;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected TITLE keyword\n", line_num);
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, &source_file);
        break;
      case VERSION:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected VERSION keyword\n", line_num);
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, &source_file);
        break;
      case STRING:
        switch (state) {
          case 1:   /* string is the title of the story */
            if (strlen(word) > MAX_TITLE_LEN-1) {
              ErrHdr();
              printf("Story title too long.\n");
              return(ERROR);
            }
            strncpy(info->title, word, MAX_TITLE_LEN);
            state = 2;
            free(word);
            word = GetNextWord(&keyword, NO_ID, 0, &source_file);
            break;
          case 3:  /* version */
            if (strlen(word) > MAX_VERSION_LEN-1) {
              ErrHdr();
              printf("Story version string too long.\n");
              return(ERROR);
            }
            strncpy(info->version, word, MAX_VERSION_LEN);
            exit = 1;
            free(word);  /* don't get the next word */
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected string: %s\n", line_num, word);
            return(ERROR);
        }
        break; /* string */
      default:
        ErrHdr();
        printf("\nLine %d: unexpected word: %s\n", line_num, word);
        return(ERROR);
    } /* switch */
  } /* while */
  return(OK);
}

