
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
#include <stdlib.h>     /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "storyinf.h"

/*************************/
/* function declarations */
/*************************/

int32_t ReadStoryInfo(storyInfo*, char*, fileList**);


/*************************/
/* function definitions  */
/*************************/

int32_t ReadStoryInfo(info, filename, file_list)
 storyInfo *info;
 char      *filename;
 fileList  **file_list;
{
  char    *word     = NULL;
  int32_t keyword   = NO_KEYWORD;
  int32_t state     = 1;
  char    *target   = NULL;
  int16_t *language = NULL;
  int16_t *ui_par   = NULL;

  printf("\n\nReading story information\n\n");

  /* initialize the story_info struct */

  strcpy(info->title, "");
  strcpy(info->author, "");
  strcpy(info->organization, "");
  strcpy(info->cover_text, "");
  strcpy(info->credits, "");
  strcpy(info->version, "");;
  strcpy(info->android_mkt, "");
  strcpy(info->ios_mkt, "");
  strcpy(info->backimage, "");
  strcpy(info->effect, "");
  strcpy(info->primary_color, "");
  info->ui_sidebar   = 1;   /* true  */
  info->ui_textinput = 1;   /* true  */
  info->ui_compass   = 1;   /* true  */
  info->autolink     = 0;   /* false */
  strcpy(info->compiler_version, "2.5");
  info->xvan_language  = ENG;
  info->story_language = ENG;
  info->play_mode      = INTERPRETER_MODE;

  /* debugging mode was entered from the command */
  /* line with "-d"                              */
  if (debug) {
    info->debug = 1;
  }
  else {
    info->debug =0;
  }

  /* Word hasn't been malloc'ed yet; before all calls to   */
  /* GetNextWord() after this one, word must be free'ed in */
  /* order to keep memory clean.                           */

  word = GetNextWord(&keyword, NO_ID, 0, &source_file, file_list);

  while (1) {
    switch (keyword) {
      case NO_KEYWORD:
        ErrHdr();
        PrintError(51, NULL, word);
        return(ERROR);
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 0, &source_file, file_list)) {
          break;
        }
        ErrHdr();
        PrintError(47, NULL, "ReadStoryInfo()");;
        return(ERROR);
        /* no break needed here */

      case STORY_INFO:
        switch (state) {
          case 0:
            state = 1;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword("STORY_INFO"));
            return(ERROR);
        }
        break;

      case TITLE:
        if (!target)
          target = info->title;
      case AUTHOR:
        if (!target)
          target = info->author;
      case ORGANIZATION:
        if (!target)
          target = info->organization;
      case COVERTEXT:
        if (!target)
          target = info->cover_text;
      case CREDITS:
        if (!target)
          target = info->credits;
      case VERSION:
        if (!target)
          target = info->version;
      case ANDROID_MARKET:
        if (!target)
          target = info->android_mkt;
      case IOS_MARKET:
        if (!target)
          target = info->ios_mkt;
      case BACKIMAGE:
        if (!target)
          target = info->backimage;
      case EFFECT:
        if (!target)
          target = info->effect;
      case PRIMARY_COLOR:
        if (!target)
          target = info->primary_color;

        switch (state) {
          case 1:
            if (strlen(target) != 0) {
              /* double definition of the parameter */
              ErrHdr();
              PrintError(144, NULL, TranslateKeyword(word));
              return(ERROR);
            }
            else {
              state = 2;
              break;
            }
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword(word));
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, &source_file, file_list);
        break;

      case CHOICE_MODE:
        /* switch off text input */
        info->ui_textinput = 0;
        if (!info->ui_compass) {
          PrintError(150, NULL, NULL);
          info->ui_compass = 1;
        }
        /* no break, continue with next case */
      case HYBRID_MODE:
        if (!info->ui_sidebar) {
          PrintError(149, NULL, NULL);
          info->ui_sidebar = 1;
        }
        switch (state) {
          case 1:
            if (info->play_mode != INTERPRETER_MODE) {
              /* double definition of the play_mode parameter */
              ErrHdr();
              PrintError(144, NULL, "Play Mode");
              return(ERROR);
            }
            else {
              /* state remains the same */
              info->play_mode = keyword;
            }
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword(word));
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, &source_file, file_list);
        break;

      case NO_SIDEBAR:
        if (info->play_mode != INTERPRETER_MODE) {
          PrintError(151, NULL, NULL);
          free(word);
          word = GetNextWord(&keyword, NO_ID, 0, &source_file, file_list);
          break;
        }
        if (!ui_par)
          ui_par = &(info->ui_sidebar);
      case NO_TEXTINPUT:
        if (info->play_mode != CHOICE_MODE && keyword == NO_TEXTINPUT) {
          PrintError(152, NULL, NULL);
          free(word);
          word = GetNextWord(&keyword, NO_ID, 0, &source_file, file_list);
          break;
        }
        if (!ui_par)
          ui_par = &(info->ui_textinput);
      case NO_COMPASS:
        if (info->play_mode == CHOICE_MODE && keyword == NO_COMPASS) {
          PrintError(153, NULL, NULL);
          free(word);
          word = GetNextWord(&keyword, NO_ID, 0, &source_file, file_list);
          break;
        }
        if (!ui_par)
          ui_par = &(info->ui_compass);
        switch (state) {
          case 1:
            if (*ui_par == 0) {
              /* double definition of the ui parameter */
              ErrHdr();
              PrintError(144, NULL, TranslateKeyword(word));
              return(ERROR);
            }
            else {
              /* state remains the same */
              *ui_par = 0;

              /* reset ui_par to prevent error in the next run */
              ui_par = NULL;
            }
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword(word));
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, &source_file, file_list);
        break;

      case AUTOLINK:
        switch (state) {
          case 1:
            if (info->autolink == 1) {
              /* double definition of the autolink parameter */
              ErrHdr();
              PrintError(144, NULL, TranslateKeyword("AUTOLINK"));
              return(ERROR);
            }
            else {
              /* state remains the same */
              info->autolink = 1;
            }
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword(word));
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, &source_file, file_list);
        break;

      case XVAN_LANGUAGE:
        if (!language)
          language = &(info->xvan_language);
      case STORY_LANGUAGE:
        if (!language)
          language = &(info->story_language);
        switch (state) {
          case 1:
            state = 3;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword(word));
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, &source_file, file_list);
        break;

      case ENG: ;
      case NL:
       switch (state) {
          case 3:
            /* we have no option to detect multiple   */
            /* language definitions, because they are */
            /* set to ENG by default.                 */
            state     = 1;
            *language = (int16_t) keyword;

            /* QaD fix fo setting language global vars */
            if (language == &(info->xvan_language))
              xvan_language = keyword;
            else
              story_language = keyword;

            /* reset language_par to prevent error in */
            /* the next run                           */
            language = NULL;
            break;
          default:
            ErrHdr();
            PrintError(48, NULL, TranslateKeyword(word));
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, &source_file, file_list);
        break;

      case STRING:
        switch (state) {
          case 2:
            state = 1;
            if (strlen(word) > INPUT_LINE_LEN-1) {
              ErrHdr();
              PrintError(91, NULL, word);
              return(ERROR);
            }
            strncpy(target, word, INPUT_LINE_LEN);
            /* reset target to prevent error in next round */
            target = NULL;
            break;
          default:
            ErrHdr();
            PrintError(49, NULL, word);
            return(ERROR);
        }
        free(word);
        word = GetNextWord(&keyword, NO_ID, 0, &source_file, file_list);
        break; /* string */

      default:
        /* ok to exit if a keyword in state 0 or 1 */
        if (state != 0 && state != 1) {
          ErrHdr();
          PrintError(48, NULL, word);
          return(ERROR);
        }
        else {
          CleanUpFileList(file_list);
          return(OK);
        }
    } /* switch */
  } /* while */
  return(OK);
}
