
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>  /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "prsedict.h"


/***********/
/* Globals */
/***********/

int32_t first_direction_id = 0; /* Tells where in the word_id range the  */
                                /* directions start. Direction ids are   */
                                /* in a consecutive range. IsDirection() */
                                /* is used to determine whether a word   */
                                /* is a direction. f_d_id is substracted */
                                /* from the word_id to calculate the     */
                                /* index in the exit_array. We cannot    */
                                /* use the same approach as with loc ids */
                                /* because each user may define his own  */
                                /* direction words and we don't want to  */
                                /* reserve ids for directions (like with */
                                /* location ids and verb ids).           */

/*************************/
/* Function declarations */
/*************************/

int32_t AddWord(char*, char*, int32_t, int32_t, int32_t*);
int32_t ParseWordGroup(char**, int32_t*, int32_t, FILE**);


/************************/
/* Function definitions */
/************************/

int32_t ParseWordGroup(word, keyword, word_type, source)
 char    **word;
 int32_t *keyword;
 int32_t word_type;
 FILE    **source;
{
  int32_t state   = 1;
  char    print_word[MAX_WORD_LEN+1];
  int32_t last_id = -1; /* holds id of last parsed word, in case it */
                        /* is a synonym for the next word           */
  free(*word);
  *word = GetNextWord(keyword, NO_ID, 0, source);

  while (1) {
    switch (*keyword) {
      case ERROR:
        return(ERROR);
      /* unexpected EOFs will have occured during Pass1Voc() */
      case EOF:
        /* check for EOF of an inserted file             */
        /* ChangeInputFile() also returns the next word  */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source)) {
        /* returns the next word */
          return(OK);
        }
        /* EOF must be passed to calling function */
        return(OK);
      case COMMA:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d %d: unexpected COMMA.\n", line_num, state);
            return(ERROR);
        }
        break;
      case SYNONYM:
        switch (state) {
          case 2:
            state = 4;
            break;
          default:
            ErrHdr();
            printf("\nLine %d %d: unexpected SYNONYM keyword.\n", line_num, state);
            return(ERROR);
        }
        break;
      default:
        /* it's a vocabulary word that must be processed */
        switch (state) {
          case 1: ;/* it's a new word definition */
          case 3:
             /* make sure any synonyms get this word as print_word */
            strncpy(print_word, *word, MAX_WORD_LEN);
            print_word[MAX_WORD_LEN] = '\0';

            /* Prepositions get two types, because we must be able to   */
            /* identify prepositions that are used to connect two parts */
            /* of a System Description.                                 */
            if (word_type == PREPOSITIONS) {
              if (!AddWord(*word, print_word, CONNECT_PREPOSITIONS, -1, &last_id)) {
              free(word);
              return(ERROR);
              }
            }
            if (!AddWord(*word, print_word, word_type, -1, &last_id)) {
              free(word);
              return(ERROR);
            }
            state = 2;
            break;
          case 4:  /* it's a synonym  */
            /* Prepositions get two types, because we must be able to   */
            /* identify prepositions that are used to connect two parts */
            /* of a System Description.                                 */
            if (word_type == PREPOSITIONS) {
              if (!AddWord(*word, print_word, CONNECT_PREPOSITIONS, last_id, &last_id)) {
              free(word);
              return(ERROR);
              }
            }
            if (!AddWord(*word, print_word, word_type, last_id, &last_id)) {
              free(word);
              return(ERROR);
            }
            state = 2;
            break;
          default:
            /* a word group can only end with the next valid keyword */
            /* if *word is not a validkeyword, we have an error      */
            if (!IsValidKeyword(*keyword)) {
              ErrHdr();
              printf("%s, line %d: unexpected word: %s.\n", current_filename, line_num, *word);
              return(ERROR);
            }
            else
              /* let calling function handle this word */
              return(OK);
        }
        break;
    } /* switch (*keyword) */
    free(*word);
    *word = GetNextWord(keyword, NO_ID, 0, source);
  } /* while */
}


int32_t AddWord(word, print_word, type, syn_id, word_id)
 char     *word;       /* word that must be added                      */
 char     *print_word; /* word that must be printed by the interpreter */
 int32_t  type;        /* type of word                                 */
 int32_t  syn_id;      /* neq -1 means existing word, new type         */
 int32_t  *word_id;    /* id of added word, output par                 */
{
  wordInfo *info;
  int      i = 0;

  /* In case of synonyms, we must make sure which word    */
  /* will be printed by the interpreter. Depending on the */
  /* position of the synonyms in the word table, the      */
  /* "wrong" synonym may be printed (e.g. x in stead of   */
  /* examine. Therefore each entry in the word table also */
  /* gets a print_word field, that contains the word that */
  /* must be printed by the interpreter when it is        */
  /* referred to by its id.                               */

  if ((info = (wordInfo *) malloc(sizeof(wordInfo))) == NULL) {
    ErrHdr();
    printf("AddWord(): error in malloc().\n");
    return(ERROR);
  }

  /* Word is the word as read from the vocabulary file. */
  /* For search purposes, all words in the word table   */
  /* are stored in lowercase.                           */
  strlwr(word);

  /* 21nov2016: words with multiple types will cause    */
  /* gaps in the word id numbering. The word gets a new */
  /* and when it is inserted into the word table we     */
  /* will find out it already exists with another type  */
  /* and the new word id would not be used.             */
  /* LEAVIT IT FOR NOW, MUST FIX LATER  (note to self:  */
  /* cannot use LookUpId() here, because the word_table */
  /* does not exist yet.                                */


  /* fill the wordInfo struct */
  if (syn_id != -1)
    *word_id = syn_id; /* use existing synonym id */
  else
    switch (type) {
      case VERB:
        if (!GetNewVerbId(word_id))
          return(ERROR);
        break;
      default:
        if (!GetNewWordId(word_id)) /* get new id */
          return(ERROR);            /* word_id is needed by caller,   */
        break;                      /* in case synonyms are defined.  */
                                    /* If the word is already known   */
                                    /* with a different type, the new */
                                    /* word id will not be used.      */
                                    /* MUST FIX LATER, LEAVE FOR NOW  */
    }

  /* Test for verb_id. Verbs have their own id range, since  */
  /* in Pass2Voc() there will be built a verb directory that */
  /* is indexed by verb_id-FIRST_VERB_ID.                    */

  /* Truncate word.    */
  strncpy(info->word, word, MAX_WORD_LEN);

  /* if word exceeds MAX_WORD_LEN, no '\0' will be copied */
  (info->word)[MAX_WORD_LEN] = '\0';

  strncpy(info->print_word, print_word, MAX_WORD_LEN);

  if (type == DIRECTIONS) {
    if (first_direction_id == 0)
      first_direction_id = *word_id;
    if (syn_id == -1)
      nr_of_directions++; /* One more direction. */
  }

  info->id       = *word_id;
  info->types[0] = type;
  for (i=1; i<MAX_TYPES; i++) {
    info->types[i] = NO_TYPE;
  }
  info->next     = NULL;

  /* process the wordInfo struct */
  if (!ProcWordInfo(info))
    return(ERROR);

  return(OK);
}


/* comments from Pass2Voc() */
  /* During Pass1Voc, all words that are in the vocabulary file */
  /* are stored in a word_table. No verb default code is parsed */
  /* yet, since we don't know the identifiers that may occur in */
  /* the default code. Identifiers are bound during Pass1Story. */
  /* Exception: identifiers in strings are bound during         */
  /* Pass1Voc().												*/

  /* 28 jan 2017: now that we allow description and attribute   */
  /* parameters in strings, we must make sure that the          */
  /* ReadString() function that is called by GetNextWord() does */
  /* not try to bind parameters (common descriptions and common */
  /* attributes will be parsed as local). We therefore          */
  /* introduce the voc_pass1 variable to denote to tell         */
  /* GetNextWord() to not parse strings but just skip them.     */


/* comments from Pass2Voc() */
  /* In Pass2Voc(), the default verb code is parsed and written to  */
  /* the datafile. All words are listed in the word_table array,    */
  /* together with their id and types.                              */
  /* Since only verbs have default code, Pass2Voc() will end after  */
  /* the mandatory DIRECTIONS keyword.                              */

  /* Part of the syntax has already been checked during Pass1Voc(). */



  /* In other cases, with ParseLocation() and ParseObject(), the */
  /* directory is created in StoreXxxDir(). It is built from the */
  /* loc_table or obj_table and directly written to file.        */
  /* However, in this case, we need to build verb_dir in         */
  /* ParseVerb(), because we don't want to sequentially search   */
  /* the verb_list that has been created by Pass1Voc().          */
  /* We also choose not to add 'offset' to the wordTable typedef,*/
  /* since this would eliminate the possibility to store         */
  /* word_table in one instruction.                              */


