
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

int32_t UpdateChoicesMenu(void);

/************************/
/* Function definitions */
/************************/

int32_t UpdateChoicesMenu(void)
{
  /* this function is called in choice or hybrid mode   */
  /* it calls t_choice for all locations and objects in */
  /* scope and builds a choice list that is send to the */
  /* front-end                                          */

  int32_t list[nr_of_locs+nr_of_objs];
  int          i      = 0;
  int          size   = nr_of_locs+nr_of_objs;
  int          len    = 0;
  resultStruct result = {OK, NONE, OK};
  kvPair       kv     = {NULL, {0, NULL, 0, 0}};
  usrActionRec dummy_rec;
  char         response_txt[INPUT_LINE_LEN];

char text_to_print[OUTPUT_LINE_LEN];

Log("In UpdateChoicesMenu()\n", "", "");
  /* we need dummy_rec to enforce an error when t_choice */
  /* ends up in XeqVerbDefault()                         */
  dummy_rec.action1 = NO_ID;

  /* determine the set of locations/objects in scope */
  /* unused items in list[] will have value NO_ID    */
  ContList(curr_loc, list, CURR_LOC);

  /* set up the {choice} json start */
  ResetString(json_msg_from_story);

  if (story_info.play_mode == CHOICE_MODE) {
    PrintString("{\"choice\":{\"text\":\"What do you want to do next?\",\"choice\":[", 1);
  }
  else {
    /* must be hybrid mode */
    PrintString("{\"choice\":{\"text\":\"Suggestions:\", \"choice\":[", 1);
  }
Log("Before executing t_choice triggers\n", "", "");
  /* now execute t_choice for each list member      */
  /* each member may add options to the choice list */
  while (i<size && list[i] != NO_ID) {
sprintf(text_to_print, "before xeqtrigger %d\n", i);
Log(text_to_print,"","");
    result = XeqTrigger(list[i], CHOICE, &dummy_rec, 0);
sprintf(text_to_print, "na xeqtrigger %d, result is %d\n", i, result.tag);
Log(text_to_print,"","");
    switch (result.tag) {
      case QUIT:
        return(QUIT);
        break;
      case DISAGREE:
        /* disagree for a t_choice means ok for the caller */
        return(OK);
        break;
      default:
Log("In default for XeqTrigger() result\n", "", "");
        /* do nothing */
        break;
    }
    i++;
  }

  /* now finalize the {joice} json */
  /* we have built something like: */
  /* {"text":"Suggestions", "choice":[{"text":"string1":"chosen":"response1"}, {..}, {..}, */
  /* if it ends with a comma, remove the comma and add ]} to close the json */

  len = strlen(json_msg_from_story);
  if (json_msg_from_story[len] == COMMA) {
    /* at least 1 entry in the choice list */
    json_msg_from_story[len] = '\0';
  }
  PrintString("]}", 1);

  /* now send the choice */
  ifi_emitResponse(json_msg_from_story);

  /* in case of CHOICE_MODE we must block until the player made a choice */
  /* in case of HYBRID_MODE we can just continue                         */
  /* 0 means to also process other messages sent by the GUI, e.g. save   */
  GetAddtlInput(&kv, "", IFI_REQ_COMMAND, 0);

  strncpy(response_txt, kv.value.textstring, INPUT_LINE_LEN);
  response_txt[INPUT_LINE_LEN] = '\0';
  


Log("Einde UpdateChoicesMenu()\n", "", "");
  return(OK);
}
