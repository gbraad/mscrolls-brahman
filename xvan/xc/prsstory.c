
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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "keyword.h"
#include "typedefs.h"
#include "prsstory.h"


/*************************/
/* Function declarations */
/*************************/

int32_t ParseStory(FILE*, fileList**);
int32_t ParseVerbNames(FILE*, fileList**);
int32_t ParseDictionary(FILE*, fileList**);
int32_t ParseDirections(FILE*, fileList**);
int32_t ParseCommons(FILE*, fileList**);
int32_t FindCommonTriggerIds(FILE*, fileList**);
int32_t ParseCommonTriggers(FILE*, fileList**);
int32_t ParseLocsObjs(FILE*, fileList**);
int32_t ParseTims(FILE*, fileList**);
int32_t ParseVerbs(FILE*, fileList**);

extern int32_t total_lines;

/************************/
/* Function definitions */
/************************/

int32_t ParseStory(source, file_list)
 FILE     *source;
 fileList **file_list;
{
  /* Verbs have special id's because we will build a verb_dir    */
  /* which is indexed by verb_id. Because words may have         */
  /* multiple types it is important that verb names are parsed   */
  /* first in all input files. If not, multiple-typed words that */
  /* that are also verbs, might not get a verb id.               */
  /* Therefore we have function ParseVerbNames() to ensure that  */
  /* all verbs get a verb id.                                    */

  /* parse verb names */
  if (!ParseVerbNames(source, file_list))
    return(ERROR);

  /* clean up the file list for the next */
  /* pass to prevent loop errors (remove */
  /* all but the first struct            */

  /* parse directions */
  /* ParseDirections() is a separate function because */
  /* all directions must be in a consecutive id range */
  /* parse dictionary words */
  if (!ParseDirections(source, file_list))
    return(ERROR);

  /* parse dictionary words */
  if (!ParseDictionary(source, file_list))
    return(ERROR);

  if (!CreateWordTable()) {
    PrintError(89, NULL, NULL);
    return(ERROR);
  }

  if (!InitExits())  /* needs nr of directions; known */
    return(ERROR);   /* after ParseDirections()       */

  /* parse common descriptions, flags and attributes */
  if (!ParseCommons(source, file_list))
    return(ERROR);

  /* find common trigger ids */
  if (!FindCommonTriggerIds(source, file_list))
    return(ERROR);

  /* parse common triggers */
  if (!ParseCommonTriggers(source, file_list))
    return(ERROR);

  /* parse verb default code */
  if (!ParseVerbs(source, file_list))
    return(ERROR);

  /* parse locations and objects */
  if (!ParseLocsObjs(source, file_list))
    return(ERROR);

  total_lines = 0;

  /* parse timers */
  if (!ParseTims(source, file_list))
    return(ERROR);

  return(OK);
}


 /* The following functions replace the former Pass1Voc() function. */
 /* Therefore, in the GetNextWord() and ChangeInputFile() functions */
 /* we must set the voc_pass1 parameter to 1. If not, GetNextWord() */
 /* will try to parse the strings and will throw missing owner      */
 /* errors for attributes and descriptions.                         */

 int32_t ParseVerbNames(source, file_list)
 FILE     *source;
 fileList **file_list;
{
  char     *word      = NULL;
  int32_t  keyword    = NO_KEYWORD;
  int      state      = 1;
  int      cont       = 1;
  char     print_word[MAX_WORD_LEN+1];
  int32_t  redefine   = 0;
  int32_t  last_id = -1; /* holds id of last parsed word, in case it */
                         /* is a synonym for the next word           */

  rewind(source);
  line_num = 1;

  /************************************/
  /* now scan the file for verb names */
  /************************************/

  word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

  while (cont) {
    if (keyword == ERROR)
      return(ERROR);

    switch (state) {
      case 1: ;
      case 3: ;
      case 7:
        switch (keyword) {
          case EOF:
            /* check for EOF of an inserted file */
            if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list)) {
              break;
            }
            cont = 0;
            break;
          case REDEFINE_VERB:
            redefine = 1;
            /* continue with VERB clause */
          case VERB:
            state = 2;
            free(word);
            word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
            break;
          default:
            state = 3;
            /* not a word we are looking for */
            free(word);
            word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
            break;
        }
        break;
      case 2:
        switch (keyword) {
          case EOF:
            /* check for EOF of an inserted file */
            if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list))
              break;
            else {
              ErrHdr();
              PrintError(47, NULL, current_filename);
              return(ERROR);
            }
          default:
            /* it's a verb  or a redefine verb */
            /* make sure any synonyms get this word as print_word */
            strncpy(print_word, word, MAX_WORD_LEN);
            print_word[MAX_WORD_LEN] = '\0';
            if (!AddWord(word, print_word, VERB, -1, &last_id, redefine)) {
              free(word);
              return(ERROR);
            }
            state = 4;
            free(word);
            word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
            break;
        }
        break;
      case 4:
        switch (keyword) {
          case EOF:
            /* check for EOF of an inserted file */
            if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list))
              break;
            else {
              ErrHdr();
              PrintError(47, NULL, current_filename);
              return(ERROR);
            }
          case SYNONYM:
            state = 5;
            break;
          case ENDVERB:
            redefine = 0;
            state = 7;
            break;
          default:
            state = 6;
            break;
        } /* switch */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
        break;
      case 5:
        switch (keyword) {
          case EOF:
            /* check for EOF of an inserted file */
            if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list))
              break;
            else {
              ErrHdr();
              PrintError(47, NULL, current_filename);
              return(ERROR);
            }
          default:
            /* it's a synonym verb */
            if (!AddWord(word, print_word, VERB, last_id, &last_id, redefine)) {
            /* if (!AddWord(word, print_word, VERB, -1, &last_id, redefine)) {*/
              free(word);
              return(ERROR);      /* &last_id unused by AddWord() */
            }
            state = 4;
            free(word);
            word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);


            /* Update nr of synonym verbs. Synonym verbs don't    */
            /* need their own entry in the verb_dir.              */
            nr_of_syn_verbs++;
            break;
        }
        break;
      case 6:
        switch (keyword) {
          case EOF:
            /* check for EOF of an inserted file */
            if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list))
              break;
            else {
              ErrHdr();
              PrintError(47, NULL, current_filename);
              return(ERROR);
            }
          case ENDVERB:
            redefine = 0;
            state = 7;
            break;
          default:
            state = 6;
            break;
        } /* switch */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
        break;
    } /* switch */
  } /* while */

  redefine = 0;
  return(OK);
}


int32_t ParseDictionary(source, file_list)
 FILE     *source;
 fileList **file_list;
{
  char     *word      = NULL;
  int32_t  keyword    = NO_KEYWORD;
  int      cont       = 1;

  rewind(source);
  line_num = 1;

  /*********************************************************/
  /* now scan the file for dictionary words                */
  /* verb names and direction words are parsed separately  */
  /*********************************************************/

  word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list)) {
          break;
        }
        cont = 0;
        break;
      case NOUNS:        ;
      case ADJECTIVES:   ;
      case PREPOSITIONS: ;
      case ADVERBS:      ;
      case ARTICLES:     ;
      case Q_WORDS:      ;
      case CONJUNCTION:
        /* parse the word group    */
        if (!ParseWordGroup(&word, &keyword, keyword, &source, file_list))
          return(ERROR);
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
        break;
    } /* switch */
  } /* while */

  return(OK);
}


int32_t ParseDirections(source, file_list)
 FILE     *source;
 fileList **file_list;
{
  char     *word   = NULL;
  int32_t  keyword = NO_KEYWORD;
  int      cont    = 1;

  rewind(source);
  line_num = 1;

  /************************************/
  /* now scan the file for directions */
  /************************************/

  word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list)) {
          break;
        }
        cont = 0;
        break;
      case DIRECTIONS:
        if (!ParseWordGroup(&word, &keyword, DIRECTIONS, &source, file_list))
          return(ERROR);

        /* next word is returned by ParseCDescrs() */
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
        break;
    } /* switch */
  } /* while */

  return(OK);
}


int32_t ParseCommons(source, file_list)
 FILE     *source;
 fileList **file_list;
{
  char     *word   = NULL;
  int32_t  keyword = NO_KEYWORD;
  int      cont    = 1;

  rewind(source);
  line_num = 1;

  /**********************************************************/
  /* now scan the file for common descrs, flags and attribs */
  /**********************************************************/

  word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list)) {
          break;
        }
        cont = 0;
        break;
      case COMMON_DESCRS:
        if (!ParseCDescrs(&word, &keyword, &source, file_list))
          return(ERROR);

        /* next word is returned by ParseCDescrs() */
        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          PrintError(51, NULL, word);
          free(word);
          return(ERROR);
        }
        break;
      case REDEFINE_FLAGS:
      case COMMON_FLAGS:
        if (!ParseFlags(&word, &keyword, COMMON_FLAGS, &source, file_list))
          return(ERROR);

        /* the next word is returned by ParseFlags() */
        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          PrintError(51, NULL, word);
          free(word);
          return(ERROR);
        }
        break;
      case REDEFINE_ATTRS:
      case COMMON_ATTRS:
        if (!ParseAttributes(&word, &keyword, COMMON_ATTRS, &source, file_list))
          return(ERROR);

        /* Next word is returned by ParseAttributes() */
        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          PrintError(51, NULL, word);
          free(word);
          return(ERROR);
        }
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
        break;
    } /* switch */
  } /* while */

  return(OK);
}


int32_t FindCommonTriggerIds(source, file_list)
 FILE     *source;
 fileList **file_list;
{
  char     *word    = NULL;
  int32_t  keyword  = NO_KEYWORD;
  int      cont     = 1;
  int32_t  redefine = 0;

  rewind(source);
  line_num = 1;
  cont     = 1;

  /**************************************************************/
  /* now scan the file for common triggers ids and declare them */
  /**************************************************************/

  word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list)) {
          break;
        }
        cont = 0;
        break;
      case REDEFINE_TRIGGERS:
        redefine = 1;
        /* continue with COMMON_TRIGGERS branch */
      case COMMON_TRIGGERS:
        if (!FindCTriggerIds(&word, &keyword, redefine, &source, file_list)) {
          return(ERROR);
        }
        redefine = 0;
        /* Next word is returned by FindCTriggIds(). */
        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          PrintError(51, NULL, word);
          free(word);
          return(ERROR);
        }
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
        break;
    } /* switch */
  } /* while */

  return(OK);
}


int32_t ParseCommonTriggers(source, file_list)
 FILE     *source;
 fileList **file_list;
{
  char     *word   = NULL;
  int32_t  keyword = NO_KEYWORD;
  int      cont    = 1;

  rewind(source);
  line_num = 1;
  cont     = 1;

  /**********************************************************/
  /* now scan the file for common triggers and compile them */
  /**********************************************************/

  word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list)) {
          break;
        }
        cont = 0;
        break;
      case REDEFINE_TRIGGERS: ;
      case COMMON_TRIGGERS:
        if (!ParseCTrigs(&word, &keyword, &source, file_list))
          return(ERROR);

        /* Next word is returned by ParseCTrigs(). */
        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          PrintError(51, NULL, word);
          free(word);
          return(ERROR);
        }
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
        break;
    } /* switch */
  } /* while */

  return(OK);
}


int32_t ParseLocsObjs(source, file_list)
 FILE     *source;
 fileList **file_list;
{
  int64_t  offset  = 0;
  char     *word   = NULL;
  int32_t  keyword = NO_KEYWORD;
  int      cont    = 1;

  rewind(source);
  line_num = 1;
  cont     = 1;

  /************************************************************/
  /* now scan the file for locations and objects compile them */
  /************************************************************/

  word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list)) {
          break;
        }
        cont = 0;
        break;
      case LOCATION:
        /* Calculate current position in datafile. */
        offset = ftell(datafile);
        if (!ParseLoc(offset, &source, file_list))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          PrintError(51, NULL, word);
          free(word);
          return(ERROR);
        }
        break;
      case OBJECT:
        /* Calculate current position in datafile.      */
        /* this position will be stored in the object   */
        /* table by ParseObj()                          */
        offset = ftell(datafile);
        if (!ParseObj(offset, &source, file_list))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          PrintError(51, NULL, word);
          free(word);
          return(ERROR);
        }
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
        break;
    } /* switch */
  } /* while */

  return(OK);
}


int32_t ParseTims(source, file_list)
 FILE     *source;
 fileList **file_list;
{
  char     *word   = NULL;
  int32_t  keyword = NO_KEYWORD;
  int      cont    = 1;

  rewind(source);
  line_num = 1;
  cont     = 1;

  /********************************/
  /* now scan the file for timers */
  /********************************/
  word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list)) {
          break;
        }
        cont = 0;
        break;
      case TIMERS:
        if (!ParseTimers(&word, &keyword, &source, file_list))
          return(ERROR);

        /* next word is returned by ParseTimers() */
        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          PrintError(51, NULL, word);
          free(word);
          return(ERROR);
        }
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
        break;
    } /* switch */
  } /* while */

  return(OK);
}


int32_t ParseVerbs(source, file_list)
 FILE     *source;
 fileList **file_list;
{
  int64_t  offset    = 0;
  int32_t  redefined = 0;
  char     *word     = NULL;
  int32_t  keyword   = NO_KEYWORD;
  int      cont      = 1;

  /* In other cases, with ParseLocation() and ParseObject(), the */
  /* directory is created in StoreXxxDir(). It is built from the */
  /* loc_table or obj_table and directly written to file.        */
  /* However, in this case, we need to build verb_dir in         */
  /* ParseVerb(), because we don't want to sequentially search   */
  /* the verb_list that has been created by ParseDictionary().   */
  /* We also choose not to add 'offset' to the wordTable typedef,*/
  /* since this would eliminate the possibility to store         */
  /* word_table in one instruction.                              */

  /* At this time, we know the size of verb_dir. */
  /* size is nr_of_verbs - nr_of_syn_verbs.      */
  if (!InitVerbDir())
    return(ERROR);

  rewind(source);
  line_num = 1;
  cont     = 1;

  /*****************************************************************/
  /* now scan the file for verbs and compile the verb default code */
  /*****************************************************************/

  word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source, file_list)) {
          break;
        }
        cont = 0;
        break;
      case REDEFINE_VERB:
        redefined = 1;
        /* continue with VERB clause */
      case VERB: ;
        /* get position in datafile */
        offset = ftell(datafile);

        if (!ParseVerb(offset, redefined, &source, file_list))
          return(ERROR);
        printf(".");

        /* reset redefined for next run */
        redefined = 0;

        /* read the next word */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);

        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          PrintError(51, NULL, word);
          free(word);
          return(ERROR);
        }
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source, file_list);
        break;
    } /* switch */
  } /* while */

  return(OK);
}
