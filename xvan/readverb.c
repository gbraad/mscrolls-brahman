
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018 Marnix van den Bos.                   */
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
#include <stdlib.h>  /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "readverb.h"

/*************************/
/* Function declarations */
/*************************/

verbInfo *NewVerbInfo(void);
verbInfo *RdVerb(int64_t);

/************************/
/* Function definitions */
/************************/

verbInfo *NewVerbInfo()
{
  verbInfo *verb_info = NULL;

  if ((verb_info = (verbInfo *) malloc(sizeof(verbInfo))) == NULL) {
    PrintError(15, NULL, "NewVerbInfo()");
    return(NULL);
  }

  /* Fill struct with default values. */
  verb_info->action_rec   = NULL;
  verb_info->code         = NULL;  /* verb default code */
  verb_info->parser_rules = NULL;
  verb_info->next         = NULL;

  /* Ready */
  return(verb_info);
}


verbInfo *RdVerb(offset)
 int64_t offset;
{
  /* This function assumes that there is default code. It will   */
  /* always deliver at least one verbInfo struct.                */

  verbInfo  *verb           = NULL; /* Start of verbInfo chain.   */
  verbInfo  *last_verb_info = NULL; /* End of chain.              */
  int32_t   new_verb_info   = 0;    /* Tells when to create a new */
                                    /* verbInfo struct.           */
  compActionRec *last_a_r   = NULL; /* Last action_rec from the   */
                                    /* last verbInfo struct.      */
  int32_t code;

  /* Set filepointer to start of verb */
  if (fseek(datafile, offset, 0) == -1) {    /* 0 means ???? */
    PrintError(16, NULL, "RdVerb()");
    return(NULL);
  }

  /* Create and init a verbInfo struct. */
  if ((verb = NewVerbInfo()) == NULL)
    return(ERROR);
  last_verb_info = verb;

  /* From now on, we will work with last_verb_info. We only need */
  /* verb to remember the start of the chain.                    */

  if (!GetNextCode32(&code))
    return(NULL);

  while (1) {
    switch (code) {
      case ERROR:
        return(NULL);

      case ACTION_REC:
        if (new_verb_info) {
          /* Create and init a new struct.   */
          if ((last_verb_info->next = NewVerbInfo()) == NULL)
            return(ERROR);
          last_verb_info = last_verb_info->next;

          /* aug 13 2015: In case more action records follow right after this one, they  */
          /* all share the same default code. So we must reset the new_verb_info var. It */
          /* will be set to '1' again, after reading the trigger code.                   */
          new_verb_info = 0;
        }

        /* Read the action_rec and trigger to execute.        */
        /* action_rec will be malloced by ReadActionRec().    */

        if (last_verb_info->action_rec == NULL) {
          /* First action_rec for this verbInfo struct. */
          if ((last_verb_info->action_rec = ReadActionRec(&code)) == NULL)
            return(NULL);
          last_a_r = last_verb_info->action_rec;
        }
        else if ((last_a_r->next = ReadActionRec(&code)) == NULL)
          return(NULL);
        else
          last_a_r = last_a_r->next;

        /* Next code is returned by ReadActionRec(). */
        break;

      case ENDVERB:
        return(verb);
        break;

      default:
        /* This is the start of the default code.     */

        /* 28dec2016: added disambiguation stuff, the */
        /* default code section is now disambiguation */
        /* code plus default code, or only default    */
        /* code. Only disambiguation code is not      */
        /* possible (makes no sense).                 */
        /* Disambiguation code, when present, will    */
        /* be first.                                  */

        /* check if there is disambiguation code */
        if (code == DISAMBIG_RULES) {
          /* skip the DISAMBIG_RULES keyword */
          if (!GetNextCode32(&code))
            return(NULL);

          if ((last_verb_info->parser_rules = RdTrCode(&code)) == NULL)
            return(NULL);

          /* skip the END_RULES keyword that was returned by RdTrCode() */
          if (!GetNextCode32(&code))
            return(NULL);
        }

        /* there will always be default code */
        /* Read the trigger code. */

        if ((last_verb_info->code = RdTrCode(&code)) == NULL)
          return(NULL);

        /* Next time we have an action_rec, create a new */
        /* verbInfo struct.                              */
        new_verb_info = 1;

        /* Next code is returned by RdTrcode(). This will */
        /* either be an END_VERB or ACTION_REC code.      */
        break;
    } /* switch */
  } /* while */
}
