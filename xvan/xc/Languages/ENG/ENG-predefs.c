
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
#include <stdlib.h>    /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "ENG-predefs.h"


/*************************/
/* Function declarations */
/*************************/

int32_t ENG_PreDefs(void);


/************************/
/* Function definitions */
/************************/

int32_t ENG_PreDefs(void)
{
  /* Descriptions: d_sys will be used to hold the system       */
  /* description of a location or an object. The system        */
  /* description is used by the interpreter to refer to a      */
  /* location/object.                                          */
  /* Attributes: prepos is used to store a preposition, to be  */
  /* used for containment.                                     */
  /* Objects: make sure the player gets the first object id,   */
  /* which is PLAYER.                                          */
  /* Objects: make sure the no_such_thing object gets the      */
  /* second object id, which is NO_SUCH_THING.                 */
  /* Flags: f_lit will be used by the interpreter to indicate  */
  /* that locations and objects are lit (i.e. they are a       */
  /* lightsource). f_opaque indicates that an object or        */
  /* location is opaque. In the story code, the flags can be   */
  /* treated as any other flags (testflag, setflag). f_lit     */
  /* must always be the first common flag;  f_opaque the       */
  /* second. If this code is changed, make sure f_lit and      */
  /* f_opaque are always defined first.                        */
  /* Triggers: t_entrance is triggered when the player enters  */
  /* a location or an object. t_exit is triggered when the     */
  /* player leaves a location or an object. entrance and exit  */
  /* triggers are triggered for all objects that are contained */
  /* in the location/object the player enters/leaves.          */
  /* Make sure the order in which t_entrance and t_exit are    */
  /* defined is not changed and that these are the first       */
  /* common trigger definitions.                               */

  int32_t id = NO_ID;

  if (!GetDescrId("sys", &id, COMMON_DESCRS, 1))
    return(ERROR);

  /* r_preposition must have default value in, but we have not yet */
  /* parsed the vocabulary yet. Therefore, we will now make a      */
  /* reference and define it with default value "in" after parsing */
  /* the vocabulary.                                               */

  /* Make sure r_preposition gets value FIRST_COMMON_ATTR_ID, because   */
  /* the interpreter will use it in the move() function.                */
  if (!GetAttrId("preposition", 0, COMMON_ATTRS, &id, WORD_ID, NONE, NO_ID, 1)) /* 10march2017 */
    return(ERROR);

  if (!GetObjectId("player", &id, 0, -1))
    return(ERROR);

  /* predefine no_such_thing */
  if (!GetObjectId("nst", &id, 0, -1))
    return(ERROR);

  /* predefine it */
  if (!GetObjectId("it", &id, 0, -1))
    return(ERROR);

  if (!GetAttrId("it", 0, IT, &id, WORD_ID, NONE, NO_ID, 0))
    return(ERROR);

  if (GetFlagId("lit", 0, &id, 0, COMMON_FLAGS, 1).tag == ERROR)
    return(ERROR);

  if (GetFlagId("opaque", 0, &id, 0, COMMON_FLAGS, 1).tag == ERROR)
    return(ERROR);

  if (GetFlagId("bypass", 0, &id, 0, COMMON_FLAGS, 1).tag == ERROR)
    return(ERROR);

  if (GetFlagId("hidden", 0, &id, 0, COMMON_FLAGS, 1).tag == ERROR)
    return(ERROR);

  if (GetFlagId("swap", 0, &id, 0, COMMON_FLAGS, 1).tag == ERROR)
    return(ERROR);

  if (GetFlagId("json_mode", 0, &id, 0, PLAYER, 1).tag == ERROR)
    /* this is a local flag */
    return(ERROR);

  /* triggers t_entrance, t_exit and t_default can be   */
  /* redefined in the story. GetTriggerId() will check  */
  /* for this, so we provide parameters to check.       */

  /* Load the parameters with the English names.        */
  strncpy(entrance_trigger, "entrance", MAX_ID_LENGTH);
  strncpy(exit_trigger, "exit", MAX_ID_LENGTH);
  strncpy(default_trigger, "default", MAX_ID_LENGTH);


  if (GetTriggerId("entrance", 0, &id, COMMON_TRIGGERS, 1).tag == ERROR)
    return(ERROR);

  if (GetTriggerId("exit", 0, &id, COMMON_TRIGGERS, 1).tag == ERROR)
    return(ERROR);

  if (GetTriggerId("default", 0, &id, COMMON_TRIGGERS, 1).tag == ERROR)
    return(ERROR);

  return(OK);
}

