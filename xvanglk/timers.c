
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "timers.h"


/*************************/
/* Function declarations */
/*************************/

int32_t CheckDoTimers(int32_t);
int32_t HandleTimers(void);


/************************/
/* Function definitions */
/************************/

int32_t CheckDoTimers(action)
 int32_t action;
{
  int32_t exclude_actions[4];
  int32_t result = OK;
  int32_t i=0;

  /* For some actions  we must not execute the timers at  */
  /* the end of the move. These are typical meta commands */
  /* that are outside the story play, like save, restore, */
  /* score, ...                                           */
  /* This function looks up the action parameter's id and */
  /* compares it to the set of actions for which the      */
  /* HandleTimers() function must no be executed.         */

  /* look up the word ids for the applicable actions. If  */
  /* an action word is not defined in the vocabulary file */
  /* the lookup function will return NO_ID.               */

  exclude_actions[0] = LookUpId("save");
  exclude_actions[1] = LookUpId("restore");
  exclude_actions[2] = LookUpId("score");
  exclude_actions[3] = LookUpId("debug");

  while (result != ERROR && i<4) {
    if (exclude_actions[i] == action)
      result = ERROR;
    i++;
  }
  return(result);
}

int32_t HandleTimers()
{
  int32_t i = 0;

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

            if (XeqTrigger(timers[i].execute[0],
                           timers[i].execute[1]) == QUIT)
              return(QUIT);
          break;
        case OR_MORE:
          if (timers[i].value >= timers[i].trigger_at)
            /* Trigger has fired. */
            /* If trigger returns QUIT, we must exit immediately. */
            if (XeqTrigger(timers[i].execute[0],
                           timers[i].execute[1]) == QUIT)
              return(QUIT);
          break;
        case OR_LESS:
          if (timers[i].value <= timers[i].trigger_at)
            /* Trigger has fired. */
            /* If trigger returns QUIT, we must exit immediately. */
            if (XeqTrigger(timers[i].execute[0],
                           timers[i].execute[1]) == QUIT)
              return(QUIT);
          break;
        default:
          sprintf(outputline, "HandleTimers(): INTERNAL ERROR. Bad trigger_spec\n");
          Output(outputline);
          break;
      }
    } /* Not an active timer. */
  } /* for */
  /* Ready.*/
  return(OK);
}

