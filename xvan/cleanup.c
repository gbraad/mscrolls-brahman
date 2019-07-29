
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
#include <stdlib.h>   /* free() */
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "cleanup.h"

/*************************/
/* function declarations */
/*************************/

void CleanUp(void);
void FreeAllLocations(void);
void FreeAllObjects(void);
void FreeLocation(locationInfo*);
void FreeObject(objectInfo*);
void FreeVerb(verbInfo*);

void FreeDescrs(descrInfo*);
void FreeCompActionRecs(compActionRec*);
void FreeTriggers(triggerInfo*);
void FreeCommonTriggers(void);

/************************/
/* Function definitions */
/************************/

void FreeCommonTriggers(void)
{
  int32_t size = LAST_COMMON_TRIGGER_ID-FIRST_COMMON_TRIGGER_ID+1;
  int32_t i=0;

  if (common_trigs == NULL) {
    /* it went wrong very early in the program */
    /* when they were not yet  initialized     */
    return;
  }
  else {
    for (i=0; i<size; i++)
      if (common_trigs[i] != NULL)
        free(common_trigs[i]);
    free(common_trigs);
  }
}


void FreeDescrs(descrInfo *start_descr)
{
  if (start_descr->next == NULL)
    return;
  else {
    FreeDescrs(start_descr->next);
    free(start_descr);
  }
}


void FreeCompActionRecs(compActionRec *start_actionrec)
{
  if (start_actionrec == NULL)
    return;
  else {
    FreeCompActionRecs(start_actionrec->next);
    free(start_actionrec);
  }
}


void FreeTriggers(triggerInfo *start_trigger)
{
  if (start_trigger == NULL)
    return;
  else {
    FreeTriggers(start_trigger->next);
    if (start_trigger->code != NULL)
      free(start_trigger->code);
    free(start_trigger);
  }
}


void FreeVerb(verbInfo *verb)
{
  if (verb->next != NULL)
    FreeVerb(verb->next);

  /* free action records */
  FreeCompActionRecs(verb->action_rec);

  /* free code */
  free(verb->code);

  /* free verb itself */
  free(verb);
}


void FreeObject(objectInfo *obj)
{
  /* free descriptions */
  FreeDescrs(obj->descriptions);

  /* free action records */
  FreeCompActionRecs(obj->action_recs);

  /* free triggers */
  FreeTriggers(obj->triggers);

  /* free object itself */
  free(obj);
}


void FreeAllLocations(void)
{
  int i = 0;

  for (i=0; i<nr_of_locs; i++) {
    /* it may be swapped out of memory */
    if (locations[i] != NULL)
      FreeLocation(locations[i]);
  }
  free(locations);
}


void FreeAllObjects(void)
{
  int i = 0;

  for (i=0; i<nr_of_objs; i++) {
    /* it may be swapped out of memory */
    if (objects[i] != NULL)
      FreeObject(objects[i]);
  }
  free(objects);
}


void FreeLocation(locationInfo *loc)
{
  /* free descriptions */
  FreeDescrs(loc->descriptions);

  /* free action records */
  FreeCompActionRecs(loc->action_recs);

  /* free triggers */
  FreeTriggers(loc->triggers);

  /* free location itsels */
  free(loc);
}


void CleanUp(void)
{
  int32_t i = 0;
  int32_t j = 0;

  /* complicated Free()'s */

  FreeAllLocations();
  FreeAllObjects();

  /* free all verbs */
  for (i=0; i<nr_of_verbs; i++) {
    if (verbs[i] != NULL)
      FreeVerb(verbs[i]);
  }
  free(verbs);

  /* free common triggers */
  FreeCommonTriggers();

  /* free dynamic sysdescr strings */
  for (i=0; i<nr_of_locs; i++) {
    for (j=0; j<loc_dir[i].nr_of_dsys; j++) {
      ResetString( (loc_dir[i]).descr[j].dynamic);
    }
  }

  for (i=0; i<nr_of_objs; i++) {
    for (j=0; j<obj_dir[i].nr_of_dsys; j++) {
      ResetString( (obj_dir[i]).descr[j].dynamic);
    }
  }

  /* simple Free()'s */
  free(loc_dbug);
  free(obj_dbug);
  free(com_attr_dbug);
  free(loc_attr_dbug);
  free(com_flag_dbug);
  free(loc_flag_dbug);
  free(com_trig_dbug);
  free(loc_trig_dbug);
  free(timer_dbug);
  free(timers);
  free(exit_data);
  free(com_loc_flags);
  free(com_obj_flags);
  free(local_flags);
  free(word_table);
  free(verb_dir);
  free(loc_dir);
  free(obj_dir);
  free(c_loc_attrs);
  free(c_obj_attrs);
  free(local_attrs);
  free(trigg_owners);
  free(descr_owners);
  free(outputline);
  free(stack);
}
