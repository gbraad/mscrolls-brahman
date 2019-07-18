
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "json.h"
#include "IFI.h"
#include "choices.h"

#include "ifiglue.h"

/*************************/
/* Function declarations */
/*************************/

resultStruct UpdateChoicesMenu(void);

/************************/
/* Function definitions */
/************************/

resultStruct UpdateChoicesMenu(void)
{
  /* this function is called in choice or hybrid mode   */
  /* it calls t_choice for all locations and objects in */
  /* scope and builds a choice list that is send to the */
  /* front-end                                          */

  int32_t      list[nr_of_locs+nr_of_objs];
  int          i = 0;
  usrActionRec dummy_rec;
  resultStruct result = {OK, NONE, OK};
Log("In UpdateChoicesMenu()\n", "", "");
  /* we need dummy_rec to enforce an error when t_choice */
  /* ends up in XeqVerbDefault()                         */
  dummy_rec.action1 = NO_ID;

  /* determine the set of locations/objects in scope */
  ContList(curr_loc, list, CURR_LOC);

  /* set up the {choice} json start */
  ResetString(json_msg_from_story);
Log("Na resetstring()\n", "", "");
  if (story_info.play_mode == CHOICE_MODE) {
    PrintString("{\"text\":\"What do you want to do next?\",\"choice\":[", 1);
  }
  else {
    /* must be hybrid mode */
    PrintString("{\"text\":\"Suggestions:\", \"choice\":[", 1);
  }

  /* now execute t_choice for each list member      */
  /* each member may add options to the choice list */
Log("for loop\n", "", "");
  for (i=0; i<nr_of_locs+nr_of_objs; i++) {
Log("in loop\n", "",""); if (list[i]==NO_ID) Log("NO_ID\n", "", "");
    result = XeqTrigger(list[i], CHOICE, &dummy_rec, 0);
    if (result.tag == QUIT) {
      return(result);
    }
  }

  /* now finalize the {joice} json */
Log("Einde UpdateChoicesMenu()\n", "", "");



}

