
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>   /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "prsetims.h"

/*************************/
/* Function declarations */
/*************************/
int32_t ParseTimers(char**, int32_t*, FILE**);

int32_t ParseTimers(word, keyword, source)
 char    **word;
 int32_t *keyword;
 FILE    **source;
{
  int32_t   state = 1;
  int32_t   id;
  int32_t   owner = NO_ID;
  int32_t   n = 0;        /* needed to convert numstrings */
  timerInfo *info = NULL; /* make compiler happy          */

  free(*word);
  *word = GetNextWord(keyword, owner, 0, source);
  while (1) {
    switch (*keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source)) {
          break;
        }
        switch (state) {
            case 1:  ; /* same as 4  */
            case 4:  ; /* same as 12 */
            case 12: ; /* same as 17 */
            case 17:
              ProcTimerInfo(info);
              return(OK);  /* ready */
          default:
            ErrHdr();
            printf("ParseTimers(): unexpected end of inputfile\n");
            return(ERROR);
        }
      case TIM_ID:
        switch (state) {
          case 1:
            /* this is the first timer id */
            state = 2;
            break;
          /* in the states below, tis isn't the first timer id  */
          /* so we have to store the previous info struct first */
          case 4:  ; /* same as 8  */
          case 8:  ; /* same as 12 */
          case 12: ; /* same as 17 */
          case 17:
            ProcTimerInfo(info);
            state = 2;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected timer id.\n", line_num);
            return(ERROR);
        }
        if (!GetTimerId((*word)+2, &id, 1))
          return(ERROR);

        /* create space on heap for new info struct */
        if ((info = (timerInfo *) malloc(sizeof(timerInfo))) == NULL) {
          ErrHdr();
          printf("ParseTimers(): error in malloc().\n");
          return(ERROR);
        }

        /* init the info struct */
        info->id           = id;
        info->value        = 0;
        info->step         = 0;
        info->interval     = 0;
        info->update       = 0;
        info->direction    = UP;
        info->state        = STOP;
        info->trigger_at   = 0;
        info->trigger_spec = EXACT;
        info->execute[0]   = NO_ID;
        info->execute[1]   = NO_ID;
        info->next         = NULL;

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case INIT:
        /* initial value, to be stored in info->value */
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected INIT keyword\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case STEP:
        switch (state) {
          case 4:
            state = 5;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected STEP keyword\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case DIRECTION:
        switch (state) {
          case 6:
            state = 7;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected DIRECTION keyword\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case INTERVAL:
        switch (state) {
          case 8:
            state = 9;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected INTERVAL keyword\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case UP: ; /* same as DOWN */
      case DOWN:
        switch (state) {
          case 7:
            state = 8;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected UP or DOWN keyword\n", line_num);
            return(ERROR);
        }
        info->direction = (int16_t) *keyword;  /* UP or DOWN */

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case STATE:
        switch (state) {
          case 10:
            state = 11;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected STATE keyword\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case GO: ; /* same as STOP */
      case STOP:
        switch (state) {
          case 11:
            state = 12;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected GO or STOP keyword\n", line_num);
            return(ERROR);
        }
        info->state = *keyword; /* GO or STOP */

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case TRIGGER_AT:
        switch (state) {
          case 8:  ; /* same as 12 */
          case 12:
            state = 13;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected TRIGGER_AT keyword\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case EXACT:   ;
      case OR_MORE: ;
      case OR_LESS:
        switch (state) {
          case 14:
            state = 18;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: Unexpected EXACT/OR_MORE/OR_LESS.\n", line_num);
            return(ERROR);
        }
        info->trigger_spec = *keyword;
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case EXECUTE:
        switch (state) {
          case 14: ;
          case 18:
            state = 15;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected EXECUTE keyword\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case LOC_ID:
        switch (state) {
          case 15:
            state = 18;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected trigger owner: %s.\n", line_num, *word);
            return(ERROR);
        }
        if (!GetLocationId((*word)+2, &owner, 0, 0))
          return(ERROR);

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case OBJ_ID:
        switch (state) {
          case 15:
            state = 18;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected trigger owner: %s.\n", line_num, *word);
            return(ERROR);
        }
        if (!GetObjectId((*word)+2, &owner, 0, 0))
          return(ERROR);

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case DOT:
        switch (state) {
          case 18:
            state = 16;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected DOT.\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      case TRIGG_ID:
        switch (state) {
          case 16:
            state = 17;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected trigger id %s\n", line_num, *word);
            return(ERROR);
        }
        if (!GetTriggerId((*word)+2, &id, owner, 0))
          return(ERROR);

        /* Put the owner and trigger id in the action_rec.    */
        /* In case of a local trigger we don't have to store  */
        /* the owned_by, but since it must be stored for      */
        /* common triggers, we'll store it anyway.            */
        info->execute[0] = owner;
        info->execute[1] = id;

        free(*word);
        *word = GetNextWord(keyword, owner, 0, source);
        break;
      default:
        /* it may be a numstring */
        if (StringToNum(*word, &n)) {
          switch (state) {
            case 3:
              info->value = n;
              state = 4;
              break;
            case 5:
              info->step = (int16_t) n;
              state = 6;
              break;
            case 9:
              info->interval = n;
              info->update   = n;
              state = 10;
              break;
            case 13:
              info->trigger_at = n;
              state = 14;
              break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected value: %d.\n", line_num, n);
              return(ERROR);
          }
          free(*word);
          *word = GetNextWord(keyword, owner, 0, source);
        } /* if */
        else {
          switch (state) {
            case 1:  ; /* same as 4  */
            case 4:  ; /* same as 12 */
            case 12: ; /* same as 17 */
            case 17:
              /* process the last info struct */
              ProcTimerInfo(info);
              return(OK);           /* ready */
            default:
              ErrHdr();
              printf("\nLine %d: unexpected word: %s.\n", line_num, *word);
              return(ERROR);
          }
        } /*else */
        break;
    } /* switch */
  } /* while */
}
