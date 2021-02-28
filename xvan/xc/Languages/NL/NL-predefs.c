
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
/*                                                                      */
/* <marnix@xvan.nl>                                                     */
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
#include "NL-predefs.h"


/*************************/
/* Function declarations */
/*************************/

int32_t NL_PreDefs(void);

/************************/
/* Function definitions */
/************************/

int32_t NL_PreDefs(void)
{
  /* for comments see ENG_Predefs() function */

  int32_t id         = NO_ID;

  if (!GetDescrId("sys", &id, COMMON_DESCRS, 1))
    return(ERROR);

  if (!GetAttrId("voorzetsel", 0, COMMON_ATTRS, &id, WORD_ID, NONE, NO_ID, 1))
    return(ERROR);

  if (!GetObjectId("speler", &id, 0, -1))
    return(ERROR);

  /* predefine no_such_thing */
  if (!GetObjectId("niets", &id, 0, -1))
    return(ERROR);

  /* predefine it */
  if (!GetObjectId("het", &id, 0, -1))
    return(ERROR);

  /* predefine unbound */  /* @!@ */
  if (!GetObjectId("unbound", &id, 0, -1))
    return(ERROR);

  if (!GetAttrId("het", 0, IT, &id, WORD_ID, NONE, NO_ID, 0))
    return(ERROR);

  if (!GetAttrId("unbound", 0, UNBOUND, &id, WORD_ID, NONE, NO_ID, 0))  /* @!@ */
    return(ERROR);

  if (GetFlagId("verlicht", 0, &id, 0, COMMON_FLAGS, 1).tag == ERROR)
    return(ERROR);

  if (GetFlagId("ondoorzichtig", 0, &id, 0, COMMON_FLAGS, 1).tag == ERROR)
    return(ERROR);

  if (GetFlagId("slaover", 0, &id, 0, COMMON_FLAGS, 1).tag == ERROR)
    return(ERROR);

  if (GetFlagId("verborgen", 0, &id, 0, COMMON_FLAGS, 1).tag == ERROR)
    return(ERROR);

  if (GetFlagId("wissel", 0, &id, 0, COMMON_FLAGS, 1).tag == ERROR)
    return(ERROR);

  if (GetFlagId("json_mode", 0, &id, 0, PLAYER, 1).tag == ERROR)
    /* this is a local flag */
    return(ERROR);

  if (GetTriggerId("unbound", 0, &id, UNBOUND, 0).tag == ERROR)  /* @!@ */
    /* this is a local trigger for the o_unbound object */
    return(ERROR);

  /* triggers t_naarbinnen, t_verlaat and t_default can   */
  /* be redefined in the story. GetTriggerId() will check */
  /* check for this, so we provide parameters to check.   */

  /* Load the parameters with the Dutch names.            */
  strncpy(entrance_trigger, "naarbinnen", MAX_ID_LENGTH);
  strncpy(exit_trigger, "verlaat", MAX_ID_LENGTH);
  strncpy(default_trigger, "default", MAX_ID_LENGTH);

  if (GetTriggerId("naarbinnen", 0, &id, COMMON_TRIGGERS, 1).tag == ERROR)
    return(ERROR);

  if (GetTriggerId("verlaat", 0, &id, COMMON_TRIGGERS, 1).tag == ERROR)
    return(ERROR);

  if (GetTriggerId("default", 0, &id, COMMON_TRIGGERS, 1).tag == ERROR)
    return(ERROR);

  if (GetTriggerId("keuze", 0, &id, COMMON_TRIGGERS, 1).tag == ERROR)
    return(ERROR);

  return(OK);
}
