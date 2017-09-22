
/* Copyright 2016, 2017 Marnix van den Bos */

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

int32_t  ParseCDescrs(char**, int32_t*, FILE**);
int32_t  ParseDescriptions(char**, int32_t*, int32_t, FILE**);


int32_t ParseCDescrs(word, keyword, source)
 char **word;
 int32_t  *keyword;
 FILE **source;
{
  int32_t state = 1;
  int32_t id;        /* Not used, needed for GetDescrID(). */

  free(*word);
  *word = GetNextWord(keyword, NO_ID, 0, source);
  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source)) {
          break;
        }
        ErrHdr();
        printf("ParseCDescrs(): unexpected end of inputfile\n");
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
            printf("\nLine %d: unexpected description id: %s\n",
                    line_num, *word);
            return(ERROR);
        }
        if (!GetDescrId((*word)+2, &id, COMMON_DESCRS, 1))
          /* 1 means it is defined here */
          return(ERROR);

        free(*word);
        *word = GetNextWord(keyword, NO_ID, 0, source);
        break;
      case COMMA:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected comma.\n", line_num);
            return(ERROR);
        }

        free(*word);
        *word = GetNextWord(keyword, NO_ID, 0, source);
        break;
      default:
        switch (state) {
          case 1: ;
          case 2:    /* ok to exit */
           return(OK);
          default:
            ErrHdr();
            printf("\nLine %d: unexpected word: %s\n", line_num, *word);
            return(ERROR);
        }
    } /* switch */
  } /* while */
}

int32_t ParseDescriptions(word, keyword, owner_id, source)
 char     **word;
 int32_t  *keyword;
 int32_t  owner_id; /* needed to look up or store the description id */
 FILE     **source;
{
  int32_t state = 1;
  int32_t id;            /* Needed to convert description ids.  */
  int32_t sys_descr = 0; /* System description not yet defined. */

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
        printf("ParseDescriptions(): unexpected end of inputfile\n");
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
            printf("\nLine %d: unexpected description id: %s\n",
                    line_num, *word);
            return(ERROR);
        }
        if (!GetDescrId((*word)+2, &id, owner_id, 1))
          /* 1 means it is defined here */
          return(ERROR);

        if (!StoreId(id))
          return(ERROR);

        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;
      case COMMA:
        switch (state) {
          case 4:
            state = 6;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected comma.\n", line_num);
            return(ERROR);
        }
        /* do nothing */
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
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
             printf("\nLine %d: unexpected string\n", line_num);
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
         *word = GetNextWord(keyword, owner_id, 0, source);
         break;
      default:
        switch (state) {
          case 4:
          case 5:
          case 8: /* ok to exit */
            /* Warning in case no d_sys defined */
              if (sys_descr == 0) ;
              /* printf("Warning: No system description defined!\n");*/
            return(OK);
          default:
            ErrHdr();
            printf("\nLine %d: unexpected word: %s\n", line_num, *word);
            return(ERROR);
        }
    } /* switch */
  } /* while */
}

