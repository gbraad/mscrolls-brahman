
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
#include <stdlib.h>     /* malloc(), free() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "readloc.h"

/*************************/
/* function declarations */
/*************************/

void         InitLoc(locationInfo*);
locationInfo *ReadLocation(int64_t);

/************************/
/* Function definitions */
/************************/

void InitLoc(loc)
 locationInfo *loc;
{
  loc->descriptions = NULL;
  loc->action_recs  = NULL;
  loc->triggers     = NULL;
}


locationInfo *ReadLocation(offset)
 int64_t offset;
{
  locationInfo  *loc;
  descrInfo     *last_descr      = NULL;
  descrInfo     *descr           = NULL;
  compActionRec *last_action_rec = NULL;
  compActionRec *action_rec      = NULL;
  triggerInfo   *last_trigg      = NULL;
  triggerInfo   *trigger         = NULL;
  int32_t code;

  /* Create space on heap */
  if ((loc = (locationInfo *) malloc(sizeof(locationInfo))) == NULL) {
    PrintError(15, NULL, "ReadLocation()");
    return(NULL);
  }

  /* Fill struct with default values. */
  InitLoc(loc);

  /* Set filepointer to start of location */
  if (fseek(datafile, offset, 0) == -1) {    /* 0 means ???? */
    PrintError(16, NULL, " ReadLocation()");
    return(NULL);
  }

  if (!GetNextCode32(&code))
    return(NULL);

  while (1) {
    switch (code) {
      case ERROR:
        return(NULL);

      case ACTION_REC:
        /* Read the action_rec and trigger to execute.     */
        /* action_rec will be malloced by ReadActionRec(). */
        if ((action_rec = ReadActionRec(&code)) == NULL)
          return(NULL);

        if (last_action_rec == NULL)
          /* First action_rec for this location. */
          loc->action_recs = action_rec;
        else
          last_action_rec->next = action_rec;
        last_action_rec = action_rec;

        /* Next code is returned by ReadActionRec(). */
        break;

      case END_LOC:
        return(loc);

      default:
        /* Check for description/trigger id. */
        if (IsDescrId(code)) {
          /* It's a description id. */
          if ((descr = ReadDescr(&code)) == NULL)
            return(NULL);
          if (last_descr == NULL)
            /* First description for this location */
            loc->descriptions = descr;
          else
            last_descr->next = descr;
          last_descr = descr;

          /* Next code is returned by ReadDescr(). */
        }
        else if (IsTriggId(code)) {
          /* It's a trigger id. */
          if ((trigger = ReadTrigger(&code)) == NULL)
            return(NULL);
          if (last_trigg == NULL)
            /* First trigger for this location */
            loc->triggers = trigger;
          else
            last_trigg->next = trigger;
          last_trigg = trigger;

          /* Next code is returned by ReadTrigger(). This  */
          /* will either be an END_LOC or TRIGG_ID code.   */
        }
        else {
          PrintError(53, &((resultStruct) {VALUE,code}), "ReadLocation()");
          return(ERROR);
        }

        break;
    } /* switch */
  } /* while */
}
