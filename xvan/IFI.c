
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
#include <stdlib.h>    /* malloc(), free() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "json.h"
#include "IFI.h"

/***************************************************/
/* NOTE: because xvan is serving as a back-end, in */
/* case of an error when processing json requests, */
/* xvan will send a json message tot the front-end */
/* to report the error, but will try to continue   */
/* processing other requests.                      */
/* It is the front-end's responsibility to decide  */
/* to abort the program.                           */
/***************************************************/

/*************************/
/* Function declarations */
/*************************/

char    *CreateLocObjeJson(int32_t);
char    *CreateJsonObjectMap(void);
int32_t CheckIFI(char*);
int32_t XeqIFIrequest(int32_t, jsonValue*);
int32_t ProcessJson(char*, char*);
void    SendIFIerror(char*, char*);

/************************/
/* Function definitions */
/************************/

char *CreateLocObjJson(int32_t id)
{
  char      *loc_obj      = NULL;
  char      *object_json  = NULL;
  jsonValue value         = {NO_TYPE, NULL, 0, 0};

  /* this function creates a json string with an         */
  /* object or location: {"id":<value>, "name":<string>} */

  if ( (object_json = NewJsonObject(object_json)) == NULL ) {
     return(NULL);
  }

  /* add the id */
  value.type       = JSON_VAL_INT;
  value.int_number = id;

  if ( (object_json = AddKV(object_json, "id", &value)) == NULL ) {
   return(NULL);
  }

  /* add the name            */
  /* 1 means add the article */
  if ( (loc_obj = GetId(id, 1)) == NULL ) {
    return(NULL);
  }

  value.type       = JSON_VAL_STRING;
  value.textstring = ResetString(value.textstring);
  value.textstring = AddToString(value.textstring, loc_obj);

  loc_obj = ResetString(loc_obj);

  if ( (object_json = AddKV(object_json, "name", &value)) == NULL ) {
    value.textstring = ResetString(value.textstring);
    free(loc_obj);
    return(NULL);
  }

  if ( (object_json = CloseObject(object_json)) == NULL ) {
    value.textstring = ResetString(value.textstring);
    free(loc_obj);
    return(NULL);
  }

  free(loc_obj);
  return(object_json);
}


char *CreateJsonObjectMap(void)
{
  int       i             = 0;
  int       j             = 0;
  int       start_value   = 0;
  int       end_value     = 0;
  char      *object_json  = NULL;
  char      *object_array = NULL;
  jsonValue value;

  /* NOTE: we do locations and objects */

  /* init the jsonValue struct */
  value.type          = 0;
  value.textstring    = NULL;
  value.int_number    = 0;
  value.float_number  = 0;

  if ( (object_array = NewJsonArray(object_array)) == NULL ) {
    return(NULL);
  }

  /* now make an {"id":int, "name":string} for each */
  /* location and object and add to the array. We   */
  /* re-use the loop, because for locations and     */
  /* objects it only differs in the start id value. */
  /* We do not send the optional icon               */

  start_value = FIRST_LOCATION_ID;
  end_value   = FIRST_LOCATION_ID + nr_of_locs-1;

  for (j=0; j<=1; j++) {
    for (i=start_value; i<=end_value; i++) {
      object_json = CreateLocObjJson(i);

      /* if NULL then just skip this object  */
      /* there may have been a dynamic d_sys */
      /* error or so                         */

      if (object_json != NULL) {
        /* now add the object_json to the array */
        value.type       = JSON_VAL_OBJECT;
        value.textstring = object_json;

        if ( (object_array = AddKV(object_array, NULL, &value)) == NULL ) {
          value.textstring = ResetString(value.textstring);
           return(NULL);
        }
      }
      object_json      = NULL;
      value.textstring = ResetString(value.textstring);
    }
    /* all locations have been added to the object-array */
    /* prepare for the next round                        */
    start_value = FIRST_OBJECT_ID;
    end_value   = FIRST_OBJECT_ID + nr_of_objs-1;
  }

  /* all objects have been added to object_json too */
  if ( (object_array = CloseArray(object_array)) == NULL ) {
    return(NULL);
  }

  /* now create a new json object for the response with the array */
  if ( (object_json = NewJsonObject(object_json)) == NULL ) {
    return(NULL);
  }

  value.type = JSON_VAL_ARRAY;
  value.textstring = AddToString(value.textstring, object_array);

  if ( (object_json = AddKV(object_json, "objects", &value)) == NULL ) {
    value.textstring = ResetString(value.textstring);
    object_array = ResetString(object_array);
    return(NULL);
  }

  object_array = ResetString(object_array);

  if ( (object_json = CloseObject(object_json)) == NULL ) {
    return(NULL);
  }

  return(object_json);
}


int32_t CheckIFI(char *request)
{
  /* request labels must be in lowercase */
  if (strcmp(request, "begin") == 0)
    return(IFI_REQ_BEGIN);
  if (strcmp(request, "command") == 0)
    return(IFI_REQ_COMMAND);
  if (strcmp(request, "configdir") == 0)
    return(IFI_REQ_CONFIGDIR);
  if (strcmp(request, "datadir") == 0)
    return(IFI_REQ_DATADIR);
  if (strcmp(request, "items") == 0)
    return(IFI_REQ_ITEMS);
  if (strcmp(request, "loaddata") == 0)
    return(IFI_REQ_LOADDATA);
/*  if (strcmp(request, "location") == 0) */
/*    return(IFI_REQ_LOCATION); */
  if (strcmp(request, "map") == 0)
    return(IFI_REQ_MAP);
  if (strcmp(request, "meta") == 0)
    return(IFI_REQ_META);
  if (strcmp(request, "moves") == 0)
    return(IFI_REQ_MOVES);
  if (strcmp(request, "objects") == 0) {
    return(IFI_REQ_OBJECTS);
  }
  if (strcmp(request, "picture") == 0)
    return(IFI_REQ_PICTURE);
  if (strcmp(request, "savedata") == 0)
    return(IFI_REQ_SAVEDATA);
  if (strcmp(request, "story") == 0)
    return(IFI_REQ_STORY);
  if (strcmp(request, "randomseed") == 0)
    return(IFI_REQ_RANDOM_SEED);
  if (strcmp(request, "people") == 0)
    return(IFI_REQ_PEOPLE);
  if (strcmp(request, "keyhit") == 0)
    return(IFI_REQ_KEYHIT);

  /* not an IFI request */

  return(IFI_NO_IFI);
}


int32_t XeqIFIrequest(int32_t request, jsonValue *value)
{
  int32_t   result        = 1;
  jsonValue val;
  char      *json_string  = NULL;
  char      *value_string = NULL;
  char      *game_state   = NULL;
  char      interpreter_input[INPUT_LINE_LEN];

  /* init the jsonValue struct */
  val.type          = 0;
  val.textstring    = NULL;
  val.int_number    = 0;
  val.float_number  = 0;

  switch (request) {
    case IFI_REQ_COMMAND:
      /* value->textstring contains the user input line */
      /* copy it to interpreter_input, because Play()   */
      /* needs a char[] an not a malloced array.        */
      /* otherwise MoreInfo() calls will go wrong       */

      strcpy(interpreter_input, value->textstring);
      result = Play(interpreter_input);
      break;

    case IFI_REQ_CONFIGDIR:
      /* this is an input parameter for the game */
      break;

    case IFI_REQ_DATADIR:
      /* this is an input parameter for the game */
      break;

    case IFI_REQ_ITEMS:
      /* if true we must send items updates */
      ifi_stats.items = (value->type == JSON_VAL_TRUE ? 1 : 0);
      if (ifi_stats.items) {
        strcpy(interpreter_input, "ifi_items");
        /* prevent timers from being fired */
        fire_timers = 0;
        Play(interpreter_input);
      }
      break;

    case IFI_REQ_LOADDATA:
      /* we must now load a saved game             */
      /* value->textstring contains the game state */
      /* check if the value actually is a string   */
      if (value->type == JSON_VAL_STRING) {
        if (!Base64Restore(value->textstring)) {
          PrintError(101, NULL, NULL);
        }
      }
      /* now, tell the front-end the new location */
      strcpy(interpreter_input, "ifi_update_gui");
      /* prevent timers from being fired */
      fire_timers = 0;
      Play(interpreter_input);

      value->textstring = ResetString(value->textstring);
      break;
#if 0
    case IFI_REQ_LOCATION:
      /* if true we must send location updates */
      ifi_stats.location = (value->type == JSON_VAL_TRUE ? 1 : 0);
      if (ifi_stats.location) {
        strcpy(interpreter_input, "ifi_loc");
        /* prevent timers from being fired */
        fire_timers = 0;
        Play(interpreter_input);
      }
      break;
#endif
    case IFI_REQ_MAP:
      /* if true we must send map updates */
      ifi_stats.map = (value->type == JSON_VAL_TRUE ? 1 : 0);
      if (ifi_stats.map) {
        strcpy(interpreter_input, "ifi_map");
        /* prevent timers from being fired */
        fire_timers = 0;
        Play(interpreter_input);
      }
      break;

    case IFI_REQ_META:
      /* check if we must do something */
      if (value->type != JSON_VAL_TRUE) {
        /* nothing to do */
        break;
      }
      /* start a new json string */
      if ( !(json_string = NewJsonObject(json_string))) {
        /* failed */
        break;
      }
      /* start a new value string */
      if ( !(value_string = NewJsonObject(value_string))) {
        /* failed */
        json_string = ResetString(json_string);
        break;
      }
      /************************/
      /* mandatory meta stuff */
      /************************/
      if (value->type == JSON_VAL_TRUE) {
        val.type = JSON_VAL_STRING;
        val.textstring = ResetString(val.textstring);
        val.textstring = AddToString(val.textstring, story_info.title);
        if ( !(value_string = AddKV(value_string, "title", &val))) {
          val.textstring = ResetString(val.textstring);
          break;
        }
        val.type = JSON_VAL_STRING;
        val.textstring = ResetString(val.textstring);
        val.textstring = AddToString(val.textstring, story_info.author);
        if ( !(value_string = AddKV(value_string, "author", &val))) {
          val.textstring = ResetString(val.textstring);
          break;
        }
        val.type = JSON_VAL_STRING;
        val.textstring = ResetString(val.textstring);
        val.textstring = AddToString(val.textstring, story_info.organization);
        if ( !(value_string = AddKV(value_string, "organisation", &val))) {
          val.textstring = ResetString(val.textstring);
          break;
        }
        val.type = JSON_VAL_OBJECT;
        val.textstring = ResetString(val.textstring);
        val.textstring = AddToString(val.textstring, "{\"color\":\"blue\",\"font\":\"Kanit Thin\",\"weight\":100}");
        if ( !(value_string = AddKV(value_string, "covertext", &val))) {
          val.textstring = ResetString(val.textstring);
          break;
        }
        val.type = JSON_VAL_STRING;
        val.textstring = ResetString(val.textstring);
        val.textstring = AddToString(val.textstring, story_info.credits);
        if ( !(value_string = AddKV(value_string, "credits", &val))) {
          val.textstring = ResetString(val.textstring);
          break;
        }
        val.type = JSON_VAL_STRING;
        val.textstring = ResetString(val.textstring);
        val.textstring = AddToString(val.textstring, story_info.version);
        if ( !(value_string = AddKV(value_string, "version", &val))) {
          val.textstring = ResetString(val.textstring);
          break;
        }
        /***********************/
        /* optional meta stuff */
        /***********************/
        if (story_info.play_mode != INTERPRETER_MODE) {
          val.type = JSON_VAL_STRING;
          val.textstring = ResetString(val.textstring);
          if (story_info.play_mode == CHOICE_MODE) {
            val.textstring = AddToString(val.textstring, "choice_mode");
          }
          else {
            val.textstring = AddToString(val.textstring, "hybrid_mode");
          }
          if ( !(value_string = AddKV(value_string, "play_mode", &val))) {
            val.textstring = ResetString(val.textstring);
            break;
          }
        }

        if (strlen(story_info.android_mkt) != 0) {
          val.type = JSON_VAL_STRING;
          val.textstring = ResetString(val.textstring);
          val.textstring = AddToString(val.textstring, story_info.android_mkt);
          if ( !(value_string = AddKV(value_string, "android_market", &val))) {
            val.textstring = ResetString(val.textstring);
            break;
          }
        }
        if (strlen(story_info.ios_mkt) != 0) {
          val.type = JSON_VAL_STRING;
          val.textstring = ResetString(val.textstring);
          val.textstring = AddToString(val.textstring, story_info.ios_mkt);
          if ( !(value_string = AddKV(value_string, "ios_market", &val))) {
            val.textstring = ResetString(val.textstring);
            break;
          }
        }
        if (strlen(story_info.backimage) != 0) {
          val.type = JSON_VAL_STRING;
          val.textstring = ResetString(val.textstring);
          val.textstring = AddToString(val.textstring, story_info.backimage);
          if ( !(value_string = AddKV(value_string, "backimage", &val))) {
            val.textstring = ResetString(val.textstring);
            break;
          }
        }
        if (strlen(story_info.effect) != 0) {
          val.type = JSON_VAL_STRING;
          val.textstring = AddToString(val.textstring, story_info.effect);
          if ( !(value_string = AddKV(value_string, "effect", &val)))
            break;
        }
        if (strlen(story_info.primary_color) != 0) {
          val.type = JSON_VAL_STRING;
          val.textstring = ResetString(val.textstring);
          val.textstring = AddToString(val.textstring, story_info.primary_color);
          if ( !(value_string = AddKV(value_string, "primary_color", &val))) {
            val.textstring = ResetString(val.textstring);
            break;
          }
        }
        if (story_info.ui_sidebar == 1) {
          val.type = JSON_VAL_TRUE;
          if ( !(value_string = AddKV(value_string, "ui_sidebar", &val)))
            break;
        }
        if (story_info.ui_textinput == 1) {
          val.type = JSON_VAL_TRUE;
          if ( !(value_string = AddKV(value_string, "ui_textinput", &val)))
            break;
        }
        if (story_info.ui_compass == 1) {
          val.type = JSON_VAL_TRUE;
          if ( !(value_string = AddKV(value_string, "ui_compass", &val)))
            break;
        }
        if (story_info.autolink == 0) {
          val.type = JSON_VAL_FALSE;
          if ( !(value_string = AddKV(value_string, "autolink", &val)))
            break;
        }
        /* for now, always request saveload UI        */
        /* CHANGE TO SELECTABLE COMPILER OPTION LATER */
        val.type = JSON_VAL_TRUE;
        if ( !(value_string = AddKV(value_string, "saveload", &val)))
          break;

        /* end the value string */
        if ( !(value_string = CloseObject(value_string))) {
          /* failed */
          break;
        }
        val.type = JSON_VAL_OBJECT;
        val.textstring = ResetString(val.textstring);
        val.textstring = AddToString(val.textstring, value_string);
        if ( !(json_string = AddKV(json_string, "meta", &val))) {
          val.textstring = ResetString(val.textstring);
          break;
        }
        /* end the jsonstring */
        if ( !(json_string = CloseObject(json_string))) {
          /* failed */
          break;
        }
      }

      ifi_emitResponse(json_string);
      value_string = ResetString(value_string);
      json_string = ResetString(json_string);
      val.textstring = ResetString(val.textstring);
      break;

    case IFI_REQ_MOVES:
      /* if true we must send move counter updates */
      ifi_stats.move = (value->type == JSON_VAL_TRUE ? 1 : 0);
      printf("Sending response to front-end: {\"dummy response for\" : \"moves\"}\n");
      break;

    case IFI_REQ_OBJECTS:
      if ( (json_string = CreateJsonObjectMap()) != NULL ) {
        ifi_emitResponse(json_string);
        free(json_string);
      }
      break;

    case IFI_REQ_PICTURE:
      /* if true we must send picture updates */
      ifi_stats.picture = (value->type == JSON_VAL_TRUE ? 1 : 0);
      break;

    case IFI_REQ_SAVEDATA:
      /* we must now send the game state to the front-end  */
      /* we have a nested json object here that is always     */
      /* the same, so we will build it using string functions */
      if ( (json_string = AddToString(json_string, "{\"savedata\":{\"data\":\"")) == NULL) {
        PrintError(102, NULL, NULL);
        break;
      }

      /* get the game state (save info) in a base64 encoded string */
      if ( (game_state = Base64Save(game_state)) == NULL) {
        /* something went wrong, error was already printed */
        PrintError(102, NULL, NULL);
        break;
      }

      if ( (json_string = AddToString(json_string, game_state)) == NULL) {
        PrintError(102, NULL, NULL);
        game_state = ResetString(game_state);
        break;
      }

      if ( (json_string = AddToString(json_string, "\"}}")) == NULL) {
        PrintError(102, NULL, NULL);
        game_state = ResetString(game_state);
        break;
      }

      /* now send the response to the front-end */
      ifi_emitResponse(json_string);

      json_string = ResetString(json_string);
      game_state  = ResetString(game_state);

      break;

    case IFI_REQ_STORY:
      printf("Sending response to front-end: {\"dummy response for\" : \"story\"}\n");
      break;

    case IFI_REQ_RANDOM_SEED:
      /* do nothing, we have our own seed */
      break;

    case IFI_REQ_PEOPLE:
      /* if true we must send people updates */
      ifi_stats.people = (value->type == JSON_VAL_TRUE ? 1 : 0);
      break;

    default:
      /* unknown IFI request code */
      /* do nothing               */

      break;
  } /* switch */

  return(result);
}


int32_t ProcessJson(char *json_string, char *line_buf)
{
  kvPair    kv;
  int       index         = 0;
  short     prologue_json = 0;
  int32_t   IFI_request   = IFI_NO_IFI;
  int32_t   result        = 1;

  /* json_string must have been checked for correct ECMA-404 syntax */

  /* init the kv struct */
  kv.key                 = NULL;
  kv.value.type          = 0;
  kv.value.textstring    = NULL;
  kv.value.int_number    = 0;
  kv.value.float_number  = 0;

  while (json_string[index] != '\0' && result == 1) {
    /* read the next key-value pair */
    if (!GetNextKVpair(json_string, &index, &kv)) {
      /* ready */
      kv.value.textstring = ResetString(kv.value.textstring);
      kv.key              = ResetString(kv.key);
      return(prologue_json ? IFI_REQ_BEGIN : result);
    }

    /* check if the key is an IFI-request */
    IFI_request = CheckIFI(kv.key);

    /* Remember an IFI_REQ_META because that's in the */
    /* prologue json. We may not start the game (kick */
    /* off the timers) until after we processed the   */
    /* prologue json.                                 */

    if (IFI_request == IFI_REQ_BEGIN) {
      prologue_json = 1;
    }

    /* must decide here what we do if not an IFI-request       */
    /* for now, send error msg and stop processing this string */
    if (IFI_request == IFI_NO_IFI) {
      SendIFIerror("UNKNOWN IFI REQUEST: ", kv.key);
    }

   /* now execute the IFI request */
   result = XeqIFIrequest(IFI_request, &(kv.value));
  }  /* while */

  kv.value.textstring = ResetString(kv.value.textstring);
  kv.key              = ResetString(kv.key);

  return(prologue_json ? IFI_REQ_BEGIN : result);
}


void SendIFIerror(char *key, char *error_text)
{
  /* must change this, does not work with printf */

  /* send the error as a json object */
  printf("{\"%s\" : \"%s\"}", key, error_text);
  printf("\n\n");
}
