
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
#include <stdlib.h>    /* malloc() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "letsplay.h"
#include "json.h"      /* for ifi.h    */
#include "IFI.h"       /* IFI_REQ_META */
#include "defs.h"

/***********/
/* Globals */
/***********/

/* these must be defined as externals in the header files of the */
/* modules that need them.                                       */

short testmode    = 0;            /* read input from file is off  */
short transcript  = 0;            /* copy output to file is off   */

char  prompt[MAX_PROMPT_LEN+1];

int32_t active_entity = NO_ID;    /* Location or object that is   */
                                  /* currently active (especially */
                                  /* for use with common          */
                                  /* triggers as 'this').         */
int32_t curr_loc      = NO_ID;    /* Current location.            */
int32_t direction     = NO_ID;    /* Current direction.           */
int32_t prepos        = NO_ID;    /* Last preposition.            */
int32_t action        = NO_ID;    /* Current action.              */
int32_t actor         = PLAYER;   /* Current actor.               */
int32_t subject       = NO_ID;    /* Current subject(s).          */
int32_t specifier     = NO_ID;    /* Current specifier.           */
int32_t value         = 0;        /* Current value.               */
int32_t ordinal       = 0;        /* Current ordinal value.       */

int32_t indent        = 0;        /* For indentation.                          */
int32_t capital       = 1;        /* Start printing with a capital character.  */
int32_t article       = 0;        /* Tells whether to print an article.        */
int32_t muted         = 0;        /* Tells whether to print the output         */

/*************************/
/* Function declarations */
/*************************/

int32_t LetsPlay(void);

/************************/
/* Function definitions */
/************************/

int32_t LetsPlay(void)
{
  int  cont = OK;

  usrActionRec *dummy_action_rec   = NULL;
  int32_t      dummy_subject_index = 0;
  char         text_to_print[OUTPUT_LINE_LEN];

  /* set the prompt */
  if (debug_info)
    strncpy(prompt, "debug> ", MAX_PROMPT_LEN);
  else
    strncpy(prompt, "> ", MAX_PROMPT_LEN);

  /* Print interpreter opening message. */
  PrintString("-- XVAN 2.4 interpreter --\n\n", 0);
  PrintString("(c) Marnix van den Bos\n\n\n\n", 0);

  /* %s doesn't work with PrintString() */
  sprintf(text_to_print, "%s", story_info.title);
  PrintString(text_to_print, 0);

  switch(story_info.story_language) {
    case ENG:
      sprintf(text_to_print, " version %s - English\n\n\n\n", story_info.version);
      PrintString(text_to_print, 0);
      break;

    case NL:
      sprintf(text_to_print, " versie %s - Nederlands\n\n\n\n", story_info.version);
      PrintString(text_to_print, 0);
      break;

    default:
      /* we should never get here */
      sprintf(outputline, " version %s - Unknown language\n\n\n\n", story_info.version);
      PrintString(text_to_print, 0);
      break;
  }

  /* print the outputline */
  Output();

  /* Init current location. */
  curr_loc = obj_dir[PLAYER-FIRST_OBJECT_ID].held_by;

  while (cont == OK) {
    cont = ProcessInput(prompt);

    /* check if we already processed the prologue json */
    if (cont == IFI_REQ_META) {
      /* we processed the prologue json, now fire the timers */
      /* Handle the timers to allow starting of the game     */
      /* (printing opening messages etc).                    */
      /* 06oct2017 added action_rec and subject_index, but   */
      /* in this first call of HandleTimers() they have      */
      /* no valid values yet                                 */
      if (HandleTimers(dummy_action_rec, dummy_subject_index) == QUIT) {
        return(OK);
      }
      cont = OK;
    }
  }
  return(cont);
}
