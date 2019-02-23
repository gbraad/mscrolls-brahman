
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
#include <stdlib.h>   /* malloc() */
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "restart.h"

/*************************/
/* function declarations */
/*************************/

int32_t Restart(void);

/************************/
/* Function definitions */
/************************/

int32_t Restart(void)
{
  char json_string[] = "{\"restart\":true}";

  /* init random number generator */
  srand((unsigned) time(NULL));

  ResetGlobals();

  /* Read the dynamic directories from the datafile */
  /* Static directories are skipped for a restart,  */
  /* as they will not have changed.                 */

  /* read the location directory */
  /* also reads nr_of_locations */
  /* Make sure the location directory is read before the mapdata. */
  free(loc_dir);
  if (!ReadLocDir(dirs.loc_dir_offset)) {
    return(ERROR);
  }

  /* free the current locations array */
  FreeAllLocations();

  /* Read the max possible number of locations. */
  /* Can only do so AFTER the location dir has been read. */
  if (!InitLocations()) {
    return(ERROR);
  }

  /* read the object directory */
  /* also reads nr_of_objects */
  free(obj_dir);
  if (dirs.obj_dir_offset != -1) {
    if (!ReadObjDir(dirs.obj_dir_offset)) {
      return(ERROR);
    }
  }

  /* free the current objects array */
  FreeAllObjects();

  /* Read the max possible number of objects. */
  /* Can only do so AFTER the object dir has been read. */
  if (!InitObjects()) {
    return(ERROR);
  }

  /* free the current map data */
  free(exit_data);

  /* read the map data */
  /* also reads nr_of_directions */
  /* Make sure the location directory has already been read. */
  if (dirs.exit_data_offset != -1) {
    if (!ReadMapData(dirs.exit_data_offset)) {
      return(ERROR);
    }
  }

  /* free the current flag data */
  free(com_loc_flags);
  free(com_obj_flags);
  free(local_flags);

  /* read the flag data */
  if (dirs.flag_data_offset != -1) {
    if (!ReadFlags(dirs.flag_data_offset)) {
      return(ERROR);
    }
  }

  /* free current attribute arrays */
  free(c_loc_attrs);
  free(c_obj_attrs);
  free(local_attrs);

  /* Read attributes.                                */
  /* Can only do this AFTER reading the dynamic      */
  /* directories, InitLocations() and InitObjects(). */
  if (!InitAttributes()) {
    return(ERROR);
  }

  /* free the current timers array */
  free(timers);

  /* read data about timers */
  if (dirs.counter_data_offset != -1) {
    if (!ReadTimers(dirs.counter_data_offset)) {
      return(ERROR);
    }
  }

  /* set the current location variable */
  curr_loc = obj_dir[PLAYER-FIRST_OBJECT_ID].held_by;

  /* now tell the front-end to restart */
  ifi_emitResponse(json_string);

  return(OK);
}
