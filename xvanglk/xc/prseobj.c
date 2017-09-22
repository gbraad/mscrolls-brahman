
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>     /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "prseobj.h"

/*************************/
/* function declarations */
/*************************/

int32_t PContIn(char**, int32_t*, int32_t, FILE**);
int32_t ParseObj(int64_t, FILE**);

int32_t PContIn(word, keyword, owner, source)
 char    **word;
 int32_t *keyword;
 int32_t owner;
 FILE    **source;
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
    *word = GetNextWord(keyword, owner, 0, source);
    switch (*keyword) {
      /* No case for NO_KEYWORD, since we may get a preposition. */
      case EOF:
        ErrHdr();
        printf("PContIn(): unexpected EOF\n");
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
              printf("\nLine %d: preposition for location %s is missing.\n", line_num, *word);
            else
              printf("\nLine %d: unexpected location id: %s\n", line_num, *word);
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
              printf("\nLine %d: preposition for location %s is missing.\n", line_num, *word);
            else
              printf("\nLine %d: unexpected location id: %s\n", line_num, *word);
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
              printf("\nLine %d: unknown word: (%s). Preposition expected\n", line_num, *word);
              free(*word);
              return(ERROR);
            }

            /* Ok, it's a word from the vocabulary. */
            /* Check whether it's a preposition.    */
            if (!HasType(voc_word_id, PREPOSITIONS)) {
              ErrHdr();
              printf("\nLine %d: preposition expected.\n", line_num);
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
            printf("\nLine %d: unexpected word: %s.\n", line_num, *word);
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


int32_t ParseObj(offset, source)
 int64_t offset; /* object's position in datafile */
 FILE    **source;
{
  char    **word = (char **) malloc(sizeof(char *));
  int32_t keyword;
  int32_t id    = 0;  /* needed to convert object id; also used as owner_id */
  int32_t state = 1;

  /* Due to a removed keyword, state 4 is not used. */

  *word = GetNextWord(&keyword, id, 0, source);
  while (1) {
    switch (keyword) {
      case EOF:
        ErrHdr();
        printf("ParseObj(): unexpected EOF\n");
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
            printf("\nLine %d: unexpected object identifier %s\n",
                    line_num, *word);
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
      case CONTAINED:
        switch (state) {
          case 3:
            state = 5;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected CONTAINED keyword\n", line_num);
            return(ERROR);
        }
        if (!PContIn(word, &keyword, id, source))
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
            printf("\nLine %d: unexpected FLAGS keyword\n", line_num);
            return(ERROR);
        }
        if (!ParseFlags(word, &keyword, id, source)) /* id denotes owner */
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
            printf("\nLine %d: unexpected ATTRIBUTES keyword\n", line_num);
            return(ERROR);
        }
        /* id denotes the owner here. */
        if (!ParseAttributes(word, &keyword, id, source))   /* was ParseLAttributes */
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
            printf("\nLine %d: unexpected TRIGGERS keyword\n", line_num);
            return(ERROR);
        }
        if (!ParseTriggers(word, &keyword, id, source)) /* id denotes owner */
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
            printf("\nLine %d: unexpected END_OBJ keyword.\n", line_num);
            return(ERROR);
        }
        if (!StoreKeyword(END_OBJ))
          return(ERROR);
        return(OK);
      default:
        ErrHdr();
        printf("\nLine %d: unexpected keyword %s.\n", line_num, *word);
        return(ERROR);
    } /* switch */
  } /* while */
}
