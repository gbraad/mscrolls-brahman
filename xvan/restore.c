
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
#include "restore.h"

/*************************/
/* function declarations */
/*************************/

int32_t ReadBase64String(char*, char*, int*, int*, int8_t*);
int32_t RestoreSpecialIds(char*, int*, int*, int8_t*);
int32_t ReadInt8(int8_t*, char*, int*, int*, int8_t*);
int32_t ReadInt16(int16_t*, char*, int*, int*, int8_t*);
int32_t ReadInt32(int32_t*, char*, int*, int*, int8_t*);
int32_t Readint64(int64_t*, char*, int*, int*, int8_t*);
int32_t ReadId(int32_t*, char*, int*, int*, int8_t*);
int32_t RestoreStoryInfo(storyInfo*, char*, int*, int*, int8_t*);
int32_t RestoreExtendedSysDescr(extendedSysDescr*, char*, int*, int*, int8_t*);
int32_t RestoreSysDescr(sysDescr*, char*, int*, int*, int8_t*);
int32_t RestoreContData(contData*, char*, int*, int*, int8_t*);
int32_t RestoreDirInfo(dirInfo*, char*, int*, int*, int8_t*);
int32_t RestoreLocationDirectory(char*, int*, int*, int8_t*);
int32_t RestoreObjectDirectory(char*, int*, int*, int8_t*);
int32_t RestoreExits(char*, int*, int*, int8_t*);
int32_t ReadAttribute(attrInfo*, char*, int*, int*, int8_t*);
int32_t RestoreCommonAttributes(char*, int*, int*, int8_t*);
int32_t RestoreLocalAttributes(char*, int*, int*, int8_t*);
int32_t RestoreCommonFlags(char*, int*, int*, int8_t*);
int32_t RestoreLocalFlags(char*, int*, int*, int8_t*);
int32_t ReadTimer(timerInfo*, char*, int*, int*, int8_t*);
int32_t RestoreTimers(char*, int*, int*, int8_t*);
int32_t Base64Restore(char*);

/************************/
/* function definitions */
/************************/

int32_t ReadInt8(int8_t *n, char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int8_t result = 0;  /* int8_t is 1 byte */

  if (!Base64ToBytes(base64_string, byte_index, 1, &result, leader_len, leader))
    /* error message will be printed by calling function */
    return(ERROR);

  *n = result;

  /* no need to convert 1 byte to host endian format */

  return(OK);
}


int32_t ReadInt16(int16_t *n, char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int8_t result[2] = {0, 0};  /* int16_t is 2 bytes */

  if (!Base64ToBytes(base64_string, byte_index, 2, result, leader_len, leader))
    /* error message will be printed by calling function */
    return(ERROR);

  ((int8_t *) n)[0] = result[0];
  ((int8_t *) n)[1] = result[1];

  /* convert to host endian format */
  *n = xv_ntohs(*n);

  return(OK);
}


int32_t ReadInt32(int32_t *n, char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int    i         = 0;
  int8_t result[4] = {0,0,0,0}; /* int32_t is 4 bytes */

  if (!Base64ToBytes(base64_string, byte_index, 4, result, leader_len, leader))
    /* error message will be printed by calling function */
    return(ERROR);

  for (i=0; i<4; i++) {
    ((int8_t *) n)[i] = result[i];
  }

  /* convert to host endian format */
  *n = xv_ntohl(*n);

  return(OK);
}


int32_t ReadInt64(int64_t *n, char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int    i         = 0;
  int8_t result[8] = {0,0,0,0,0,0,0,0}; /* int32_t is 4 bytes */

  if (!Base64ToBytes(base64_string, byte_index, 8, result, leader_len, leader))
    /* error message will be printed by calling function */
    return(ERROR);

  for (i=0; i<8; i++) {
    ((int8_t *) n)[i] = result[i];
  }

  /* convert to host endian format */
  *n = xv_ntohll(*n);

  return(OK);
}


int32_t ReadBase64String(char *dest, char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
 /* dest must have enough room */
{
  int i       = 0;
  int32_t len = 0;

  /* read the length of the string */
  if (!ReadInt32(&len, base64_string, byte_index, leader_len, leader)) {
    return(ERROR);
  }

  for (i=0; i<len; i++) {
    if (!ReadInt8( ((int8_t*) dest)+i, base64_string, byte_index, leader_len, leader)) {
      return(ERROR);
    }
  }

  /* add null char */
  dest[len] = '\0';

  return(OK);
}


int32_t ReadId(int32_t *id,  char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  if (!ReadInt32(id, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadId()");
    return(ERROR);
  }
  return(OK);
}


int32_t RestoreSpecialIds(char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int32_t special_id_array[NR_OF_SPECIAL_IDS];
  int i = 0;

  /* read the special ids */
  for (i=0; i< 10; i++) {
    if (!ReadInt32(special_id_array+i, base64_string, byte_index, leader_len, leader)) {
      PrintError(14, NULL, "RestoreSpecialIds()");
      return(ERROR);
    }
  }
  /* now assign the special ids from the array */

  action        = special_id_array[0];
  prepos        = special_id_array[1];
  direction     = special_id_array[2];
  active_entity = special_id_array[3];
  curr_loc      = special_id_array[4];
  actor         = special_id_array[5];
  subject       = special_id_array[6];
  specifier     = special_id_array[7];
  value         = special_id_array[8];
  ordinal       = special_id_array[9];

  return(OK);
}


int32_t RestoreExtendedSysDescr(extendedSysDescr *extended_system_description, char *base64_string, 
                                int *byte_index, int *leader_len, int8_t *leader)
{
  int32_t code;

  if (!ReadInt32(&code, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreExtendedSysDescr()");
    return(ERROR);
  }

  if (code == DYN_DSYS) {
    if (!ReadBase64String(extended_system_description->dynamic, base64_string, byte_index, leader_len, leader)) {
      PrintError(14, NULL, "RestoreExtendedSysDescr()");
      return(ERROR);
    }
    else {
      return(OK);
    }
  }

  /* it's a 'normal' system description */

  if (!RestoreSysDescr(&(extended_system_description->part1), base64_string, byte_index, leader_len, leader))
    return(ERROR);

  if (!ReadInt32(&(extended_system_description->connect_prepos), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreExtendedSysDescr()");
    return(ERROR);
  }

  if (!RestoreSysDescr(&(extended_system_description->part2), base64_string, byte_index, leader_len, leader))
    return(ERROR);

  return(OK);
}


int32_t RestoreSysDescr(sysDescr *system_description,  char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int32_t i=0;

  if (!ReadInt32(&(system_description->article), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreSysDescr()");
    return(ERROR);
  }

  if (!ReadInt32(&(system_description->nr_of_adjectives), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreSysDescr()");
    return(ERROR);
  }

  for (i=0; i<MAX_PARSE_ADJ; i++)
    if (!ReadId((system_description->adjectives)+i, base64_string, byte_index, leader_len, leader)) {
      PrintError(14, NULL, "RestoreSysDescr()");
      return(ERROR);
    }

  if (!ReadId(&(system_description->noun), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreSysDescr()");
    return(ERROR);
  }

  return(OK);
}


int32_t RestoreContData(contData *cont_data,  char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int32_t i=0;

  if (!ReadInt32(&(cont_data->nr_of_objects), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreContData()");
    return(ERROR);
  }

  for (i=0; i<MAX_CONTAINED_OBJECTS; i++)
    if (!ReadId((cont_data->object_ids)+i, base64_string, byte_index, leader_len, leader)) {
      PrintError(14, NULL, "RestoreContData()");
      return(ERROR);
    }

  return(OK);
}


int32_t RestoreStoryInfo(storyInfo *story_info,  char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  /* Read the storyInfo struct. */
  if (!ReadBase64String(story_info->title, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadBase64String(story_info->author, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadBase64String(story_info->organization, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadBase64String(story_info->cover_text, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadBase64String(story_info->credits, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadBase64String(story_info->version, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadBase64String(story_info->android_mkt, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadBase64String(story_info->ios_mkt, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadBase64String(story_info->backimage, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadBase64String(story_info->effect, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadBase64String(story_info->primary_color, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadInt16(&(story_info->ui_sidebar), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadInt16(&(story_info->ui_textinput), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadInt16(&(story_info->ui_compass), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadInt16(&(story_info->autolink), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadBase64String(story_info->compiler_version, base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadInt16(&(story_info->xvan_language), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  if (!ReadInt16(&(story_info->story_language), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreStoryInfo()");
    return(ERROR);
  }

  return(OK);
}


int32_t RestoreDirInfo(dirInfo *dir_info,  char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int i = 0;

  /* read nr_of_dsys */
  if (!ReadInt32(&(dir_info->nr_of_dsys), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreDirInfo()");
    return(ERROR);
  }

  /* read dsys instances */
  for (i=0; i<MAX_DSYS; i++) {
    if (!RestoreExtendedSysDescr((dir_info->descr)+i, base64_string, byte_index, leader_len, leader))
      return(ERROR);
  }

  /* read contained objects */
  if (!RestoreContData(&(dir_info->contained_objs), base64_string, byte_index, leader_len, leader))
      return(ERROR);

  /* read containing object or location */
  if (!ReadInt32(&(dir_info->held_by), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreDirInfo()");
    return(ERROR);
  }

  /* read offset */
  if (!ReadInt64(&(dir_info->offset), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "RestoreDirInfo()");
    return(ERROR);
  }

  return(OK);
}


int32_t RestoreLocationDirectory(char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int32_t i = 0;

  for (i=0; i<nr_of_locs; i++) {
    if (!RestoreDirInfo(loc_dir+i, base64_string, byte_index, leader_len, leader))
      return(ERROR);
  }

  return(OK);
}


int32_t RestoreObjectDirectory(char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int32_t i=0;

  for (i=0; i<nr_of_objs; i++) {
    if (!RestoreDirInfo(obj_dir+i, base64_string, byte_index, leader_len, leader))
      return(ERROR);
  }
  return(OK);
}


int32_t RestoreExits(char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int32_t size; /* size of the exit_data array */
  int     i = 0;

  size = nr_of_locs * nr_of_directions;

  for (i=0; i<size; i++) {
    if (!ReadInt32(exit_data+i, base64_string, byte_index, leader_len, leader)) {
      PrintError(14, NULL, "RestoreExits()");
      return(ERROR);
    }
  }

  return(OK);
}


int32_t ReadAttribute(attrInfo *attribute,  char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  if (!ReadInt32(&(attribute->type), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadAttribute()");
    return(ERROR);
  }

  if (!ReadInt32(&(attribute->value), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadAttribute()");
    return(ERROR);
  }

  if (!ReadInt32(&(attribute->value_owner), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadAttribute()");
    return(ERROR);
  }

  return(OK);
}


int32_t RestoreCommonAttributes(char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int32_t total_attrs; /* Total number of common attributes for all locations */
  int i = 0;

  /* Store the common location attributes */
  total_attrs = nr_of_cattrs*nr_of_locs;

  for (i=0; i<total_attrs; i++) {
    if (!ReadAttribute(c_loc_attrs+i, base64_string, byte_index, leader_len, leader))
      return(ERROR);
  }

  /* Store the common object attributes */
  total_attrs = nr_of_cattrs*nr_of_objs;

  for (i=0; i<total_attrs; i++) {
    if (!ReadAttribute(c_obj_attrs+i, base64_string, byte_index, leader_len, leader))
      return(ERROR);
  }

  return(OK);
}


int32_t RestoreLocalAttributes(char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int i = 0;
  /* Total number of local attributes is stored in global variable nr_of_lattrs */

  for (i=0; i<nr_of_lattrs; i++) {
    if (!ReadAttribute(local_attrs+i, base64_string, byte_index, leader_len, leader))
      return(ERROR);
  }

  return(OK);
}


int32_t RestoreCommonFlags(char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int32_t com_loc_flags_string_len = 0;
  int32_t com_obj_flags_string_len = 0;
  int i                            = 0;

  /* calculate the length of the common flag strings */
  com_loc_flags_string_len = ((nr_of_cflags*nr_of_locs)/WORD_LEN)+1;
  com_obj_flags_string_len = ((nr_of_cflags*nr_of_objs)/WORD_LEN)+1;

  for (i=0; i<com_loc_flags_string_len; i++) {
    if (!ReadInt32(com_loc_flags+i, base64_string, byte_index, leader_len, leader)) {
      PrintError(14, NULL, "RestoreCommonFlags()");
      return(ERROR);
    }
  }

  for (i=0; i<com_obj_flags_string_len; i++) {
    if (!ReadInt32(com_obj_flags+i, base64_string, byte_index, leader_len, leader)) {
      PrintError(14, NULL, "RestoreCommonFlags()");
      return(ERROR);
    }
  }

  return(OK);
}


int32_t RestoreLocalFlags(char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int i = 0;

  /* Global var loc_flags_string_len is used here */
  for (i=0; i<loc_flags_string_len; i++) {
    if (!ReadInt32(local_flags+i, base64_string, byte_index, leader_len, leader)) {
      PrintError(14, NULL, "RestoreLocalFlags()");
      return(ERROR);
    }
  }

  return(OK);
}


int32_t ReadTimer(timerInfo *timer, char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  if (!ReadInt32(&(timer->value), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadTimer()");
    return(ERROR);
  }
  if (!ReadInt32(&(timer->step), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadTimer()");
    return(ERROR);
  }
  if (!ReadInt32(&(timer->interval), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadTimer()");
    return(ERROR);
  }
  if (!ReadInt32(&(timer->update), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadTimer()");
    return(ERROR);
  }
  if (!ReadInt16(&(timer->direction), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadTimer()");
    return(ERROR);
  }

  if (!ReadInt16(&(timer->state), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadTimer()");
    return(ERROR);
  }

  if (!ReadInt32(&(timer->trigger_at), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadTimer()");
    return(ERROR);
  }

  if (!ReadInt32(&(timer->trigger_spec), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadTimer()");
    return(ERROR);
  }

  if (!ReadInt32(&((timer->execute)[0]), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadTimer()");
    return(ERROR);
  }

  if (!ReadInt32((&(timer->execute[1])), base64_string, byte_index, leader_len, leader)) {
    PrintError(14, NULL, "ReadTimer()");
    return(ERROR);
  }

  return(OK);
}


int32_t RestoreTimers(char *base64_string, int *byte_index, int *leader_len, int8_t *leader)
{
  int i = 0;
 /* global vars timerInfo *timers and int nr_of_timers are used here */

  for (i=0; i<nr_of_timers; i++) {
    if (!ReadTimer(timers+i, base64_string, byte_index, leader_len, leader))
      return(ERROR);
  }

  return(OK);
}


int32_t Base64Restore(char *base64_restore)
{
  int8_t leader        = 0;
  int    leader_len    = 0;
  int    byte_index    = 0;

  storyInfo saved_story_info;

  /***************************************************************/
  /* It is important that the restore functions below are called */
  /* in the same sequence as the corresponding save functions.   */
  /* There are no syntax checks done.                            */
  /***************************************************************/

  /* Read the story info from the restore string.                */
  /* Do not overwrite the current story's storyInfo struct as we */
  /* need it to check the compatibility of the savefile with the */
  /* current story.                                              */

  if (!RestoreStoryInfo(&saved_story_info, base64_restore, &byte_index, &leader_len, &leader))
    return(ERROR);
  else {
    /* Check savefile compatibility with current story */
    /* Title and version must match.                   */
    if (strcmp(story_info.title, saved_story_info.title) != 0) {
      PrintError(88, NULL, saved_story_info.title);
      return(ERROR);
    }

    if (strcmp(story_info.version, saved_story_info.version) != 0) {
      PrintError(89, NULL, saved_story_info.version);
      return(ERROR);
    }
  } /* else */

  if (!RestoreSpecialIds(base64_restore, &byte_index, &leader_len, &leader)) {
    base64_restore = ResetString(base64_restore);
    return(ERROR);
  }

  if (!(RestoreLocationDirectory(base64_restore, &byte_index, &leader_len, &leader))) {
    base64_restore = ResetString(base64_restore);
    return(ERROR);
  }

  if (!(RestoreObjectDirectory(base64_restore, &byte_index, &leader_len, &leader))) {
    base64_restore = ResetString(base64_restore);
    return(ERROR);
  }

  if (!(RestoreExits(base64_restore, &byte_index, &leader_len, &leader))) {
    base64_restore = ResetString(base64_restore);
    return(ERROR);
  }

  if (!(RestoreCommonAttributes(base64_restore, &byte_index, &leader_len, &leader))) {
    base64_restore = ResetString(base64_restore);
    return(ERROR);
  }

  if (!(RestoreLocalAttributes(base64_restore, &byte_index, &leader_len, &leader))) {
    base64_restore = ResetString(base64_restore);
    return(ERROR);
  }

  if (!(RestoreCommonFlags(base64_restore, &byte_index, &leader_len, &leader))) {
    base64_restore = ResetString(base64_restore);
    return(ERROR);
  }

  if (!(RestoreLocalFlags(base64_restore, &byte_index, &leader_len, &leader))) {
    base64_restore = ResetString(base64_restore);
    return(ERROR);
  }

  if (!(RestoreTimers(base64_restore, &byte_index, &leader_len, &leader))) {
    base64_restore = ResetString(base64_restore);
    return(ERROR);
  }

  return(OK);
}
