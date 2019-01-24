
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
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "timers.h"

/***********/
/* Globals */
/***********/

short fire_timers = OK;

/*************************/
/* Function declarations */
/*************************/

int32_t CheckDoTimers();
int32_t HandleTimers(usrActionRec*, int32_t);

/************************/
/* Function definitions */
/************************/

int32_t CheckDoTimers()
{
  int32_t result = OK;

  /* For some actions  we must not execute the timers at  */
  /* the end of the move. These are typical meta commands */
  /* that are outside the story play, like save, restore, */
  /* score, all ifi_ commands...                          */

  /* this function checks if the timers must be fired by  */
  /* checking the fire_timers var. This var is set by the */
  /* notimers() functin in the story code. Disabling      */
  /* fire_timers only lasts for the current move, it is   */
  /* set again in this function, regardless of its value. */

  if (fire_timers) {
    result = OK;
  }
  else {
    result = ERROR;
  }

  fire_timers = OK;

  return(result);
}


int32_t HandleTimers(action_rec, subject_index)
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  int32_t      i = 0;
  resultStruct result;

  /* HandleTimers() returns either OK or QUIT. */
  for (i=0; i<nr_of_timers; i++) {
    if (timers[i].state == GO) {
      /* This is an active timer. */
      if (--(timers[i].update) == 0) {
        /* Timer must have an update.       */
        /* But first reset the update time. */
        timers[i].update = timers[i].interval;
        if (timers[i].direction == UP)
          timers[i].value += timers[i].step;
        else
          timers[i].value -= timers[i].step;
      }

      /* Check if trigger must be executed. */
      switch (timers[i].trigger_spec) {
        case EXACT:
          if (timers[i].value == timers[i].trigger_at)
            /* Trigger has fired. Trigger will be executed each   */
            /* turn if condition is fulfilled. If we want trigger */
            /* to be executed only once, set timer to STOP after  */
            /* first time executing the trigger.                  */

            result = XeqTrigger(timers[i].execute[0], timers[i].execute[1], action_rec, subject_index);
            if (result.tag == QUIT)
              return(QUIT);
          break;
        case OR_MORE:
          if (timers[i].value >= timers[i].trigger_at)
            /* Trigger has fired. */
            /* If trigger returns QUIT, we must exit immediately. */
            result = XeqTrigger(timers[i].execute[0], timers[i].execute[1], action_rec, subject_index);
            if (result.tag == QUIT)
              return(QUIT);
          break;
        case OR_LESS:
          if (timers[i].value <= timers[i].trigger_at)
            /* Trigger has fired. */
            /* If trigger returns QUIT, we must exit immediately. */
            result = XeqTrigger(timers[i].execute[0], timers[i].execute[1], action_rec, subject_index);
            if ( result.tag == QUIT)
              return(QUIT);
          break;
        default:
          PrintError(74, NULL, NULL);
          break;
      }
    } /* Not an active timer. */
  } /* for */
  /* Ready.*/
  return(OK);
}
