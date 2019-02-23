
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
#include <stdlib.h>  /* malloc(). realloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "save.h"

/*************************/
/* function declarations */
/*************************/

char *SaveSpecialIds(char*, int*, int8_t*);
char *StoreInt8(int8_t, char*, int*, int8_t*);
char *StoreInt16(int16_t, char*, int*, int8_t*);
char *StoreInt32(int32_t, char*, int*, int8_t*);
char *StoreInt64(int64_t, char*, int*, int8_t*);
char *StoreString(char*, char*, int*, int8_t*);
char *StoreId(int32_t, char*, int*, int8_t*);
char *SaveStoryInfo(char*, int*, int8_t*);
char *SaveExtendedSysDescr(extendedSysDescr*, char*, int*, int8_t*);
char *SaveSysDescr(sysDescr*, char*, int*, int8_t*);
char *SaveContData(contData*, char*, int*, int8_t*);
char *SaveDirInfo(dirInfo*, char*, int*, int8_t*);
char *SaveLocationDirectory(char*, int*, int8_t*);
char *SaveObjectDirectory(char*, int*, int8_t*);
char *SaveExits(char*, int*, int8_t*);
char *SaveAttribute(attrInfo*, char*, int*, int8_t*);
char *SaveCommonAttributes(char*, int*, int8_t*);
char *SaveLocalAttributes(char*, int*, int8_t*);
char *SaveCommonFlags(char*, int*, int8_t*);
char *SaveLocalFlags(char*, int*, int8_t*);
char *SaveTimer(timerInfo*, char*, int*, int8_t*);
char *SaveAllTimers(char*, int*, int8_t*);
char *Base64Save(char*);

/************************/
/* function definitions */
/************************/

char *StoreInt8(int8_t n, char *json_save, int *trailer_len, int8_t *trailer)
{
  int8_t result[3]  = {0,0,0}; /* need a null char for AddToString() */

  /* no need to convert to big endian */

  /* int8_t is 1 byte */
  if (!ByteToBase64(n, result, trailer_len, trailer)) {
    /* error message will be printed by calling function */
    json_save = ResetString(json_save);
    return(NULL);
  }

  if ( (json_save = AddToString(json_save, (char*) result)) == NULL) {
    return(NULL);
  }

  return(json_save);
}


char *StoreInt16(int16_t n, char *json_save, int *trailer_len, int8_t *trailer)
{
  int    i          = 0;
  int8_t result[3]  = {0,0,0}; /* need a null char for AddToString()*/

  /* convert to big endian */
  n = xv_htons(n);

  /* int16_t is 2 bytes */
  for (i=0; i<2; i++) {
    /* go from left to right, so i[1] first */
    if (!ByteToBase64(((int8_t*) &n)[i], result, trailer_len, trailer)) {
      /* error message will be printed by calling function */
      json_save = ResetString(json_save);
      return(NULL);
    }

  if ( (json_save = AddToString(json_save, (char*) result)) == NULL) {
      return(NULL);
    }
  }

  return(json_save);
}


char *StoreInt32(int32_t n, char *json_save, int *trailer_len, int8_t *trailer)
{
  int    i          = 0;
  int8_t result[3]  = {0,0,0}; /* need a null char for AddToString() */

  /* convert to big endian */
  n = xv_htonl(n);

  /* int32_t is 4 bytes */
  for (i=0; i<4; i++) {
    if (!ByteToBase64( ((int8_t*) &n)[i], result, trailer_len, trailer)) {
      /* error message will be printed by calling function */
      json_save = ResetString(json_save);
      return(NULL);
    }

    if ( (json_save = AddToString(json_save, (char*) result)) == NULL) {
      return(NULL);
    }
  }

  return(json_save);
}


char *StoreInt64(int64_t n, char *json_save, int *trailer_len, int8_t *trailer)
{
  int    i          = 0;
  int8_t result[3]  = {0,0,0}; /* need a null char for AddToString() */

  /* convert to big endian */
  n = xv_htonll(n);

  /* int64_t is 8 bytes */
  for (i=0; i<8; i++) {
    if (!ByteToBase64( ((int8_t*) &n)[i], result, trailer_len, trailer)) {
      /* error message will be printed by calling function */
      json_save = ResetString(json_save);
      return(NULL);
    }

    if ( (json_save = AddToString(json_save, (char*) result)) == NULL) {
      return(NULL);
    }
  }

  return(json_save);
}


char *StoreString(char *str, char *json_save, int *trailer_len, int8_t *trailer)
{
  int     i   = 0;
  int32_t len = 0;

  len = strlen(str);

  /* save the length of the string */
  if ( (json_save = StoreInt32(len, json_save, trailer_len, trailer)) == NULL) {
     /* error msg will be printed by the calling function */
    return(NULL);
  }

  /* now save the string       */
  /* does not save a null char */
  for (i=0; i<len; i++) {
    if ( (json_save = StoreInt8((int8_t) str[i], json_save, trailer_len, trailer)) == NULL) {
      /* error msg will be printed by the calling function */
      return(NULL);
    }
  }

  return(json_save);
}


char *StoreId(int32_t id, char *json_save, int *trailer_len, int8_t *trailer)
{
  if ( (json_save = StoreInt32(id, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "StoreId()");
    return(NULL);
  }

  return(json_save);
}


char *SaveSpecialIds(char *json_save, int *trailer_len, int8_t *trailer)
{
  int32_t special_id_array[NR_OF_SPECIAL_IDS];
  int     i = 0;

  /* now store the special ids in the array so we can save them in the savefile */

  special_id_array[0] = action;
  special_id_array[1] = prepos;
  special_id_array[2] = direction;
  special_id_array[3] = active_entity;
  special_id_array[4] = curr_loc;
  special_id_array[5] = actor;
  special_id_array[6] = subject;
  special_id_array[7] = specifier;
  special_id_array[8] = value;
  special_id_array[9] = ordinal;

  /* save the special ids */
  for (i=0; i< 10; i++) {
    if ( (json_save = StoreInt32(special_id_array[i], json_save, trailer_len, trailer)) == NULL) {
      PrintError(56, NULL, "SaveSpecialIds()");
    return(NULL);
    }
  }

  return(json_save);
}


char *SaveStoryInfo(char *json_save, int *trailer_len, int8_t *trailer)
{

  /* Store the storyInfo struct. */
  if ( (json_save = StoreString(story_info.title, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreString(story_info.author, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreString(story_info.organization, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreString(story_info.cover_text, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreString(story_info.credits, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreString(story_info.version, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreString(story_info.android_mkt, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreString(story_info.ios_mkt, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreString(story_info.backimage, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreString(story_info.effect, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreString(story_info.primary_color, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreInt16(story_info.ui_sidebar, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(ERROR);
  }

  if ( (json_save = StoreInt16(story_info.ui_textinput, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(ERROR);
  }

  if ( (json_save = StoreInt16(story_info.ui_compass, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(ERROR);
  }

  if ( (json_save = StoreInt16(story_info.autolink, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(ERROR);
  }

  if ( (json_save = StoreString(story_info.compiler_version, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(NULL);
  }

  if ( (json_save = StoreInt16(story_info.xvan_language, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(ERROR);
  }

  if ( (json_save = StoreInt16(story_info.story_language, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveStoryInfo()");
    return(ERROR);
  }

  return(json_save);
}


char *SaveExtendedSysDescr(extendedSysDescr *extended_system_description, char *json_save, int *trailer_len, int8_t *trailer)
{
  if ( (json_save = SaveSysDescr(&(extended_system_description->part1), json_save, trailer_len, trailer)) == NULL)
    return(NULL);

  if ( (json_save = StoreInt32(extended_system_description->connect_prepos, json_save, trailer_len, trailer)) == NULL) {
      PrintError(56, NULL, "SaveExtendedSystemDescription()");
      return(NULL);
  }

  if ( (json_save = SaveSysDescr(&(extended_system_description->part2), json_save, trailer_len, trailer)) == NULL)
    return(NULL);

  return(json_save);
}


char *SaveSysDescr(sysDescr *system_description, char *json_save, int *trailer_len, int8_t *trailer)
{
  int32_t i = 0;

  if ( (json_save = StoreInt32(system_description->article, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveSysDescr()");
    return(NULL);
  }

  if ( (json_save = StoreInt32(system_description->nr_of_adjectives, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveSysDescr()");
    return(NULL);
  }

  for (i=0; i<MAX_PARSE_ADJ; i++)
    if ( (json_save = StoreId((system_description->adjectives)[i], json_save, trailer_len, trailer)) == NULL) {
      PrintError(56, NULL, "SaveSysDescr()");
      return(NULL);
    }

  if ( (json_save = StoreId(system_description->noun, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveSysDescr()");
    return(NULL);
  }
  return(json_save);
}


char *SaveContData(contData *cont_data, char *json_save, int *trailer_len, int8_t *trailer)
{
  int32_t i = 0;

  if ( (json_save = StoreInt32(cont_data->nr_of_objects, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveContData()");
    return(NULL);
  }

  for (i=0; i<MAX_CONTAINED_OBJECTS; i++)
    if ( (json_save = StoreId((cont_data->object_ids)[i], json_save, trailer_len, trailer)) == NULL) {
      PrintError(56, NULL, "SaveContData()");
      return(NULL);
    }
  return(json_save);
}


char *SaveDirInfo(dirInfo *dir_info, char *json_save, int *trailer_len, int8_t *trailer)
{
  int i = 0;

  /* store nr_of_dsys */
  if ( (json_save = StoreInt32(dir_info->nr_of_dsys, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveDirInfo()");
    return(NULL);
  }

  /* store dsys instances */
  for (i=0; i<MAX_DSYS; i++) {
    if ( (json_save = SaveExtendedSysDescr(dir_info->descr+i, json_save, trailer_len, trailer)) == NULL)
      return(NULL);
  }

  /* store contained objects */
  if ( (json_save = SaveContData(&(dir_info->contained_objs), json_save, trailer_len, trailer)) == NULL)
      return(NULL);

  /* store containing object or location */
  if ( (json_save = StoreInt32(dir_info->held_by, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveDirInfo()");
    return(NULL);
  }

  /* store offset */
  if ( (json_save = StoreInt64(dir_info->offset, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveDirInfo()");
    return(NULL);
  }

  return(json_save);
}


char *SaveLocationDirectory(char *json_save, int *trailer_len, int8_t *trailer)
{
  int32_t i = 0;

  for (i=0; i<nr_of_locs; i++) {
    if ( (json_save = SaveDirInfo(loc_dir+i, json_save, trailer_len, trailer)) == NULL)
      return(NULL);
  }

  return(json_save);
}


char *SaveObjectDirectory(char *json_save, int *trailer_len, int8_t *trailer)
{
  int32_t i=0;

  for (i=0; i<nr_of_objs; i++) {
    if ( (json_save = SaveDirInfo(obj_dir+i, json_save, trailer_len, trailer)) == NULL)
      return(NULL);
  }
  return(json_save);
}


char *SaveExits(char *json_save, int *trailer_len, int8_t *trailer)
{
  int32_t size; /* size of the exit_data array */
  int     i = 0;

  size = nr_of_locs * nr_of_directions;

  for (i=0; i<size; i++) {
    if ( (json_save = StoreInt32(exit_data[i], json_save, trailer_len, trailer)) == NULL) {
      PrintError(56, NULL, "SaveExits()");
      return(NULL);
    }
  }

  return(json_save);
}


char *SaveAttribute(attrInfo *attribute, char *json_save, int *trailer_len, int8_t *trailer)
{
  if ( (json_save = StoreInt32(attribute->type, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveAttribute()");
    return(NULL);
  }

  if ( (json_save = StoreInt32(attribute->value, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveAttribute()");
    return(NULL);
  }

  if ( (json_save = StoreInt32(attribute->value_owner, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveAttribute()");
    return(NULL);
  }

  return(json_save);
}


char *SaveCommonAttributes(char *json_save, int *trailer_len, int8_t *trailer)
{
  int32_t total_attrs; /* Total number of common attributes for all locations */
  int     i = 0;

  /* Store the common location attributes */
  total_attrs = nr_of_cattrs*nr_of_locs;

  for (i=0; i<total_attrs; i++) {
    if ( (json_save = SaveAttribute(c_loc_attrs+i, json_save, trailer_len, trailer)) == NULL)
      return(NULL);
  }

  /* Store the common object attributes */
  total_attrs = nr_of_cattrs*nr_of_objs;

  for (i=0; i<total_attrs; i++) {
    if ( (json_save = SaveAttribute(c_obj_attrs+i, json_save, trailer_len, trailer)) == NULL)
      return(NULL);
  }

  return(json_save);
}


char *SaveLocalAttributes(char *json_save, int *trailer_len, int8_t *trailer)
{
  int i = 0;
  /* Total number of local attributes is stored in global variable nr_of_lattrs */

  for (i=0; i<nr_of_lattrs; i++) {
    if ( (json_save = SaveAttribute(local_attrs+i, json_save, trailer_len, trailer)) == NULL)
      return(NULL);
  }

  return(json_save);
}


char *SaveCommonFlags(char *json_save, int *trailer_len, int8_t *trailer)
{
  int32_t com_loc_flags_string_len = 0;
  int32_t com_obj_flags_string_len = 0;
  int     i                        = 0;

  /* calculate the length of the common flag strings */
  com_loc_flags_string_len = ((nr_of_cflags*nr_of_locs)/WORD_LEN)+1;
  com_obj_flags_string_len = ((nr_of_cflags*nr_of_objs)/WORD_LEN)+1;

  for (i=0; i<com_loc_flags_string_len; i++) {
    if ( (json_save = StoreInt32(com_loc_flags[i], json_save, trailer_len, trailer)) == NULL) {
      PrintError(56, NULL, "SaveCommonFlags()");
      return(NULL);
    }
  }

  for (i=0; i<com_obj_flags_string_len; i++) {
    if ( (json_save = StoreInt32(com_obj_flags[i], json_save, trailer_len, trailer)) == NULL) {
      PrintError(56, NULL, "SaveCommonFlags()");
      return(NULL);
    }
  }

  return(json_save);
}


char *SaveLocalFlags(char *json_save, int *trailer_len, int8_t *trailer)
{
  int i = 0;

  /* Global var loc_flags_string_len is used here */
  for (i=0; i<loc_flags_string_len; i++) {
    if ( (json_save = StoreInt32(local_flags[i], json_save, trailer_len, trailer)) == NULL) {
      PrintError(56, NULL, "SaveLocalFlags()");
      return(NULL);
    }
  }

  return(json_save);
}


char *SaveTimer(timerInfo *timer, char *json_save, int *trailer_len, int8_t *trailer)
{
  if ( (json_save = StoreInt32(timer->value, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveTimer()");
    return(NULL);
  }
  if ( (json_save = StoreInt32(timer->step, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveTimer()");
    return(NULL);
  }
  if ( (json_save = StoreInt32(timer->interval, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveTimer()");
    return(NULL);
  }
  if ( (json_save = StoreInt32(timer->update, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveTimer()");
    return(NULL);
  }
  if ( (json_save = StoreInt16(timer->direction, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveTimer()");
    return(NULL);
  }
  if  ( (json_save = StoreInt16(timer->state, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveTimer()");
    return(NULL);
  }
  if ( (json_save = StoreInt32(timer->trigger_at, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveTimer()");
    return(NULL);
  }
  if ( (json_save = StoreInt32(timer->trigger_spec, json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveTimer()");
    return(NULL);
  }
  if ( (json_save = StoreInt32((timer->execute)[0], json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveTimer()");
    return(NULL);
  }
  if ( (json_save = StoreInt32((timer->execute[1]), json_save, trailer_len, trailer)) == NULL) {
    PrintError(56, NULL, "SaveTimer()");
    return(NULL);
  }

  return(json_save);
}


char *SaveAllTimers(char *json_save, int *trailer_len, int8_t *trailer)
{
  int i = 0;
 /* global vars timerInfo *timers and int nr_of_timers are used here */

  for (i=0; i<nr_of_timers; i++) {
    if ( (json_save = SaveTimer(timers+i, json_save, trailer_len, trailer)) == NULL) {
      return(NULL);
    }
  }

  return(json_save);
}


char *Base64Save(char *base64_save)
{
  int8_t trailer     = 0;
  int    trailer_len = 0;

  /* saves the game state to a base64 encoded string and      */
  /* returns a pointer to the string to the caller            */
  /* NOTE: the string is malloced and the caller must free it */

  if ( (base64_save = SaveStoryInfo(base64_save, &trailer_len, &trailer)) == NULL)
    return(NULL);

  if ( (base64_save = SaveSpecialIds(base64_save, &trailer_len, &trailer)) == NULL)
    return(NULL);

  if ( (base64_save = SaveLocationDirectory(base64_save, &trailer_len, &trailer)) == NULL)
    return(NULL);

  if ( (base64_save = SaveObjectDirectory(base64_save, &trailer_len, &trailer)) == NULL)
    return(NULL);

  if ( (base64_save = SaveExits(base64_save, &trailer_len, &trailer)) == NULL)
    return(NULL);

  if ( (base64_save = SaveCommonAttributes(base64_save, &trailer_len, &trailer)) == NULL)
    return(NULL);

  if ( (base64_save = SaveLocalAttributes(base64_save, &trailer_len, &trailer)) == NULL)
    return(NULL);

  if ( (base64_save = SaveCommonFlags(base64_save, &trailer_len, &trailer)) == NULL)
    return(NULL);

  if ( (base64_save = SaveLocalFlags(base64_save, &trailer_len, &trailer)) == NULL)
    return(NULL);

  if ( (base64_save = SaveAllTimers(base64_save, &trailer_len, &trailer)) == NULL)
    return(NULL);

  if ( (base64_save = FinalizeBase64(base64_save, trailer_len, trailer)) == NULL)
    return(NULL);

  return(base64_save);
}
