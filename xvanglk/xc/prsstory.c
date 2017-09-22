
/* Copyright 2016, 2017 Marnix van den Bos */

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

int32_t ParseStory(FILE*);
int32_t ParseVerbNames(FILE*);
int32_t ParseDictionary(FILE*);
int32_t ParseDirections(FILE*);
int32_t ParseCommons(FILE*);
int32_t ParseCommonTriggers(FILE*);
int32_t ParseLocsObjs(FILE*);
int32_t ParseTims(FILE*);
int32_t ParseVerbs(FILE*);


/************************/
/* Function definitions */
/************************/

int32_t ParseStory(source)
 FILE *source;
{

  /* Verbs have special id's because we will build a verb_dir    */
  /* which is indexed by verb_id. Because words may have         */
  /* multiple types it is important that verb names are parsed   */
  /* first in all input files. If not, multiple-typed words that */
  /* that are also verbs, might not get a verb id.               */
  /* Therefore we have function ParseVerbNames() to ensure that  */
  /* all verbs get a verb id.                                    */

  /* parse verb names */
  if (!ParseVerbNames(source))
    return(ERROR);

  /* parse directions */
  /* ParseDirections() is a separate function because */
  /* all directions must be in a consecutive id range */

  if (!ParseDirections(source))
    return(ERROR);

  /* parse dictionary words */
  if (!ParseDictionary(source))
    return(ERROR);

  if (!CreateWordTable()) {
    printf("\nError creating WordTable.\n");
    return(ERROR);
  }

  if (!InitExits())  /* needs nr of directions; known */
    return(ERROR);   /* after ParseDirections()       */

  /* parse common descriptions, flags and attributes */
  if (!ParseCommons(source))
    return(ERROR);

  /* parse common triggers */
  if (!ParseCommonTriggers(source))
    return(ERROR);

  /* parse verb default code */
  if (!ParseVerbs(source))
    return(ERROR);

  /* parse locations and objects */
  if (!ParseLocsObjs(source))
    return(ERROR);

  /* parse timers */
  if (!ParseTims(source))
    return(ERROR);

  return(OK);
}

 /* The following functions replace the former Pass1Voc() function. */
 /* Therefore, in the GetNextWord() and ChaneInputFile() functions  */
 /* we must set the voc_pass1 parameter to 1. If not, GetNextWord() */
 /* will try to parse the strings and will throw missing owner      */
 /* errors for attributes and descriptions.                         */

 int32_t ParseVerbNames(source)
 FILE *source;
{
  char     *word      = NULL;
  int32_t  keyword    = NO_KEYWORD;
  int      state      = 1;
  int      cont       = 1;
  char     print_word[MAX_WORD_LEN+1];
  int32_t  last_id = -1; /* holds id of last parsed word, in case it */
                         /* is a synonym for the next word           */

  rewind(source);
  line_num = 1;

  /************************************/
  /* now scan the file for verb names */
  /************************************/

  word = GetNextWord(&keyword, NO_ID, 1, &source);

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
            if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source)) {
              break;
            }
            cont = 0;
            break;
          case VERB:
            state = 2;
            free(word);
            word = GetNextWord(&keyword, NO_ID, 1, &source);
            break;
          default:
            state = 3;
            /* not a word we are looking for */
            free(word);
            word = GetNextWord(&keyword, NO_ID, 1, &source);
            break;
        }
        break;
      case 2:
        switch (keyword) {
          case EOF:
            /* check for EOF of an inserted file */
            if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source))
              break;
            else {
              ErrHdr();
             printf("%s, line %d: unexpected End Of File.\n", current_filename, line_num);
              return(ERROR);
            }
          default:
            /* it's a verb */
            /* make sure any synonyms get this word as print_word */
            strncpy(print_word, word, MAX_WORD_LEN);
            print_word[MAX_WORD_LEN] = '\0';
            if (!AddWord(word, print_word, VERB, -1, &last_id)) {
              free(word);
              return(ERROR);
            }
            state = 4;
            free(word);
            word = GetNextWord(&keyword, NO_ID, 1, &source);
            break;
        }
        break;
      case 4:
        switch (keyword) {
          case EOF:
            /* check for EOF of an inserted file */
            if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source))
              break;
            else {
              ErrHdr();
              printf("%s, line %d: unexpected End Of File.\n", current_filename, line_num);
              return(ERROR);
            }
          case SYNONYM:
            state = 5;
            break;
          case ENDVERB:
            state = 7;
            break;
          default:
            state = 6;
            break;
        } /* switch */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);
        break;
      case 5:
        switch (keyword) {
          case EOF:
            /* check for EOF of an inserted file */
            if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source))
              break;
            else {
              ErrHdr();
             printf("%s, line %d: unexpected End Of File.\n", current_filename, line_num);
              return(ERROR);
            }
          default:
            /* it's a synonym verb */
            if (!AddWord(word, print_word, VERB, last_id, &last_id)) {
              free(word);
              return(ERROR);      /* &last_id unused by AddWord() */
            }
            state = 4;
            free(word);
            word = GetNextWord(&keyword, NO_ID, 1, &source);
            break;

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
            if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source))
              break;
            else {
              ErrHdr();
              printf("%s, line %d: unexpected End Of File.\n", current_filename, line_num);
              return(ERROR);
            }
          case ENDVERB:
            state = 7;
            break;
          default:
            state = 6;
            break;
        } /* switch */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);
        break;
    } /* switch */
  } /* while */

  total_lines += line_num;

  return(OK);
}

int32_t ParseDictionary(source)
 FILE *source;
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

  word = GetNextWord(&keyword, NO_ID, 1, &source);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source)) {
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
        if (!ParseWordGroup(&word, &keyword, keyword, &source))
          return(ERROR);
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);
        break;
    } /* switch */
  } /* while */

  total_lines += line_num;

  return(OK);
}


int32_t ParseDirections(source)
 FILE *source;
{
  char     *word   = NULL;
  int32_t  keyword = NO_KEYWORD;
  int      cont    = 1;

  rewind(source);
  line_num = 1;

  /************************************/
  /* now scan the file for directions */
  /************************************/

  word = GetNextWord(&keyword, NO_ID, 1, &source);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source)) {
          break;
        }
        cont = 0;
        break;
      case DIRECTIONS:
        if (!ParseWordGroup(&word, &keyword, DIRECTIONS, &source))
          return(ERROR);

        /* next word is returned by ParseCDescrs() */
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);
        break;
    } /* switch */
  } /* while */

  /* no need to update total_lines, this has already been done by ParseDictionary() */

  return(OK);
}


int32_t ParseCommons(source)
 FILE *source;
{
  char     *word   = NULL;
  int32_t  keyword = NO_KEYWORD;
  int      cont    = 1;

  rewind(source);
  line_num = 1;

  /**********************************************************/
  /* now scan the file for common descrs, flags and attribs */
  /**********************************************************/

  word = GetNextWord(&keyword, NO_ID, 1, &source);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source)) {
          break;
        }
        cont = 0;
        break;
      case COMMON_DESCRS:
        if (!ParseCDescrs(&word, &keyword, &source))
          return(ERROR);

        /* next word is returned by ParseCDescrs() */
        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          printf("%s, line %d: unexpected word: %s.\n", current_filename, line_num, word);
          free(word);
          return(ERROR);
        }
        break;
      case COMMON_FLAGS:
        if (!ParseFlags(&word, &keyword, COMMON_FLAGS, &source))
          return(ERROR);

        /* the next word is returned by ParseFlags() */
        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          printf("%s, line %d: unexpected word: %s.\n", current_filename, line_num, word);
          free(word);
          return(ERROR);
        }
        break;
      case COMMON_ATTRS:
        if (!ParseAttributes(&word, &keyword, COMMON_ATTRS, &source))
          return(ERROR);

        /* Next word is returned by ParseAttributes() */
        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          printf("%s, line %d: unexpected word: %s.\n", current_filename, line_num, word);
          free(word);
          return(ERROR);
        }
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);
        break;
    } /* switch */
  } /* while */

  /* no need to update total_lines, this has already been done by ParseDictionary() */

  return(OK);
}


int32_t ParseCommonTriggers(source)
 FILE *source;
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

  word = GetNextWord(&keyword, NO_ID, 1, &source);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source)) {
          break;
        }
        cont = 0;
        break;
      case COMMON_TRIGGERS:
        if (!ParseCTrigs(&word, &keyword, &source))
          return(ERROR);

        /* Next word is returned by ParseCTrigs(). */
        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          printf("%s, line %d: unexpected word: %s.\n", current_filename, line_num, word);
          free(word);
          return(ERROR);
        }
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);
        break;
    } /* switch */
  } /* while */

  /* no need to update total_lines, this has already been done by ParseDictionary() */

  return(OK);
}


int32_t ParseLocsObjs(source)
 FILE *source;
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

  word = GetNextWord(&keyword, NO_ID, 1, &source);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source)) {
          break;
        }
        cont = 0;
        break;
      case LOCATION:
        /* Calculate current position in datafile. */
        offset = ftell(datafile);
        if (!ParseLoc(offset, &source))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);

        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          printf("%s, line %d: unexpected word: %s.\n", current_filename, line_num, word);
          free(word);
          return(ERROR);
        }
        break;
      case OBJECT:
        /* Calculate current position in datafile.      */
        /* this position will be stored in the object   */
        /* table by ParseObj()                          */
        offset = ftell(datafile);
        if (!ParseObj(offset, &source))
          return(ERROR);

        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);

        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          printf("%s, line %d: unexpected word: %s.\n", current_filename, line_num, word);
          free(word);
          return(ERROR);
        }
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);
        break;
    } /* switch */
  } /* while */

  /* no need to update total_lines, this has already been done by ParseDictionary() */

  return(OK);
}


int32_t ParseTims(source)
 FILE *source;
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
  word = GetNextWord(&keyword, NO_ID, 1, &source);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source)) {
          break;
        }
        cont = 0;
        break;
      case TIMERS:
        if (!ParseTimers(&word, &keyword, &source))
          return(ERROR);

        /* next word is returned by ParseTimers() */
        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          printf("%s, line %d: unexpected word: %s.\n", current_filename, line_num, word);
          free(word);
          return(ERROR);
        }
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);
        break;
    } /* switch */
  } /* while */

  /* no need to update total_lines, this has already been done by ParseDictionary() */

  return(OK);
}


int32_t ParseVerbs(source)
 FILE *source;
{
  int64_t  offset  = 0;
  char     *word   = NULL;
  int32_t  keyword = NO_KEYWORD;
  int      cont    = 1;

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

  word = GetNextWord(&keyword, NO_ID, 1, &source);

  while (cont) {
    switch (keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(&word, &keyword, NO_ID, 1, &source)) {
          break;
        }
        cont = 0;
        break;
      case VERB:
        /* get position in datafile */
        offset = ftell(datafile);

        if (!ParseVerb(offset, &source))
          return(ERROR);
        printf(".");

        /* read the next word */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);

        /* word must be a valid keyword */
        if (!IsValidKeyword(keyword)) {
          ErrHdr();
          printf("%s, line %d: unexpected word: %s.\n", current_filename, line_num, word);
          free(word);
          return(ERROR);
        }
        break;
      default:
        /* not a word that we are looking for */
        free(word);
        word = GetNextWord(&keyword, NO_ID, 1, &source);
        break;
    } /* switch */
  } /* while */

  /* no need to update total_lines, this has already been done by ParseDictionary() */

  return(OK);
}



