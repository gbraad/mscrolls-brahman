
/* Copyright 2016, 2017 Marnix van den Bos */

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

void    RestoreError(char*);
int32_t RestoreSpecialIds(FILE*);
int32_t ReadInt16(int16_t*, FILE*);
int32_t ReadInt32(int32_t*, FILE*);
int32_t Readint64(int64_t*, FILE*);
int32_t ReadId(int32_t*, FILE*);
int32_t RestoreStoryInfo(storyInfo*, FILE*);
int32_t RestoreExtendedSysDescr(extendedSysDescr*, FILE*);
int32_t RestoreSysDescr(sysDescr*, FILE*);
int32_t RestoreContData(contData*, FILE*);
int32_t RestoreDirInfo(dirInfo*, FILE*);
int32_t RestoreLocationDirectory(FILE*);
int32_t RestoreObjectDirectory(FILE*);
int32_t RestoreExits(FILE*);
int32_t ReadAttribute(attrInfo*, FILE*);
int32_t RestoreCommonAttributes(FILE*);
int32_t RestoreLocalAttributes(FILE*);
int32_t RestoreCommonFlags(FILE*);
int32_t RestoreLocalFlags(FILE*);
int32_t ReadTimer(timerInfo*, FILE*);
int32_t RestoreTimers(FILE*);



void RestoreError(error_string)
 char *error_string;
{
  sprintf(outputline, "%s", error_string);
  Output(outputline);
}

int32_t ReadInt16(n, savefile)
 int16_t  *n;
 FILE *savefile;
{
  if (fread((void *) n, sizeof(int16_t), 1, savefile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);

  /* convert to host endian format */
  *n = ntohs(*n);

  return(OK);
}

int32_t ReadInt32(n, savefile)
 int32_t  *n;
 FILE *savefile;
{
  if (fread((void *) n, sizeof(int32_t), 1, savefile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);

  /* convert to host endian format */
  *n = ntohl(*n);

  return(OK);
}

int32_t ReadInt64(n, savefile)
 int64_t *n;
 FILE *savefile;
{
  if (fread((void *) n, sizeof(int64_t), 1, savefile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);

  /* convert to host endian format */
  *n = ntohll(*n);

  return(OK);
}

int32_t ReadId(id, savefile)
 int32_t *id;
 FILE *savefile;
{
  if (!ReadInt32(id, savefile)) {
    RestoreError("StoreId(): error reading id.\n");
    return(ERROR);
  }
  return(OK);
}


int32_t RestoreSpecialIds(savefile)
  FILE *savefile;
{
  int32_t special_id_array[NR_OF_SPECIAL_IDS];
  int i = 0;

  /* read the special ids */
  for (i=0; i< 10; i++) {
    if (!ReadInt32(special_id_array+i, savefile)) {
      RestoreError("RestoreSpecialIds(): error writing data.\n");
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

int32_t RestoreExtendedSysDescr(extended_system_description, savefile)
 extendedSysDescr *extended_system_description;
 FILE             *savefile;
{
  if (!RestoreSysDescr(&(extended_system_description->part1), savefile))
    return(ERROR);

  if (!ReadInt32(&(extended_system_description->connect_prepos), savefile)) {
    RestoreError("RestoreExtendedSysDescr(): error reading conneecting preposition.\n");
    return(ERROR);
  }

  if (!RestoreSysDescr(&(extended_system_description->part2), savefile))
    return(ERROR);

  return(OK);
}

int32_t RestoreSysDescr(system_description, savefile)
 sysDescr *system_description;
 FILE     *savefile;
{
  int32_t i=0;

  if (!ReadInt32(&(system_description->article), savefile)) {
    RestoreError("RestoreSysDescr(): error reading data.\n");
    return(ERROR);
  }

  if (!ReadInt32(&(system_description->nr_of_adjectives), savefile)) {
    RestoreError("RestoreSysDescr(): error reading data.\n");
    return(ERROR);
  }

  for (i=0; i<MAX_PARSE_ADJ; i++)
    if (!ReadId((system_description->adjectives)+i, savefile)) {
      RestoreError("RestoreSysDescr(): error reading data.\n");
      return(ERROR);
    }

  if (!ReadId(&(system_description->noun), savefile)) {
    RestoreError("RestoreSysDescr(): error reading data.\n");
    Output(outputline);
    return(ERROR);
  }

  return(OK);
}

int32_t RestoreContData(cont_data, savefile)
 contData *cont_data;
 FILE     *savefile;
{
  int32_t i=0;

  if (!ReadInt32(&(cont_data->nr_of_objects), savefile)) {
    RestoreError("SaveContData(): error writing data.\n");
    return(ERROR);
  }

  for (i=0; i<MAX_CONTAINED_OBJECTS; i++)
    if (!ReadId((cont_data->object_ids)+i, savefile)) {
      RestoreError("SaveContData(): error writing data.\n");
      return(ERROR);
    }

  return(OK);
}

int32_t RestoreStoryInfo(story_info, savefile)
 storyInfo *story_info;
 FILE *savefile;
{
  /* Read the storyInfo struct. */
  if (fread((void *) story_info->title, sizeof(char), MAX_TITLE_LEN, savefile) != MAX_TITLE_LEN) {
    RestoreError("RestoreStoryInfo(): read error\n");
    return(ERROR);
  }

  if (fread((void *) story_info->version, sizeof(char), MAX_VERSION_LEN, savefile) !=MAX_VERSION_LEN) {
    RestoreError("RestoreStoryInfo(): read error\n");
    Output(outputline);
    return(ERROR);
  }

  if (fread((void *) story_info->compiler_version, sizeof(char), MAX_COMPILER_LEN, savefile) != MAX_COMPILER_LEN) {
    RestoreError("RestoreStoryInfo(): read error\n");
    return(ERROR);
  }
  return(OK);
}

int32_t RestoreDirInfo(dir_info, savefile)
 dirInfo *dir_info;
 FILE*    savefile;
{
  int i = 0;

  /* read nr_of_dsys */
  if (!ReadInt32(&(dir_info->nr_of_dsys), savefile)) {
    RestoreError("RestoreirInfo(): error reading data.\n");
    return(ERROR);
  }

  /* read dsys instances */
  for (i=0; i<MAX_DSYS; i++) {
    if (!RestoreExtendedSysDescr((dir_info->descr)+i, savefile))
      return(ERROR);
  }

  /* read contained objects */
  if (!RestoreContData(&(dir_info->contained_objs), savefile))
      return(ERROR);

  /* read containing object or location */
  if (!ReadInt32(&(dir_info->held_by), savefile)) {
    RestoreError("RestoreDirInfo(): error reading data.\n");
    return(ERROR);
  }

  /* read offset */
  if (!ReadInt64(&(dir_info->offset), savefile)) {
    RestoreError("RestoreDirInfo(): error reading data.\n");
    return(ERROR);
  }

  return(OK);
}


int32_t RestoreLocationDirectory(savefile)
 FILE *savefile;
{
  int32_t i = 0;

  for (i=0; i<nr_of_locs; i++) {
    if (!RestoreDirInfo(loc_dir+i, savefile))
      return(ERROR);
  }

  return(OK);
}

int32_t RestoreObjectDirectory(savefile)
 FILE *savefile;
{
  int32_t i=0;

  for (i=0; i<nr_of_objs; i++) {
    if (!RestoreDirInfo(obj_dir+i, savefile))
      return(ERROR);
  }
  return(OK);
}

int32_t RestoreExits(savefile)
 FILE *savefile;
{
  int32_t size; /* size of the exit_data array */
  int     i = 0;

  size = nr_of_locs * nr_of_directions;

  for (i=0; i<size; i++) {
    if (!ReadInt32(exit_data+i, savefile)) {
      RestoreError("RestoreExits(): error reading data.\n");
      return(ERROR);
    }
  }

  return(OK);
}

int32_t ReadAttribute(attribute, savefile)
 attrInfo *attribute;
 FILE* savefile;
{
  if (!ReadInt32(&(attribute->type), savefile)) {
    RestoreError("ReadAttribute(): error reading data.\n");
    return(ERROR);
  }

  if (!ReadInt32(&(attribute->value), savefile)) {
    RestoreError("RestoreAttribute(): error reading data.\n");
    return(ERROR);
  }

  if (!ReadInt32(&(attribute->value_owner), savefile)) {
    RestoreError("RestoreAttribute(): error reading data.\n"); /* 13march2017 */
    return(ERROR);
  }

  return(OK);
}


int32_t RestoreCommonAttributes(savefile)
 FILE *savefile;
{
  int32_t total_attrs; /* Total number of common attributes for all locations */
  int i = 0;

  /* Store the common location attributes */
  total_attrs = nr_of_cattrs*nr_of_locs;

  for (i=0; i<total_attrs; i++) {
    if (!ReadAttribute(c_loc_attrs+i, savefile))
      return(ERROR);
  }

  /* Store the common object attributes */
  total_attrs = nr_of_cattrs*nr_of_objs;

  for (i=0; i<total_attrs; i++) {
    if (!ReadAttribute(c_obj_attrs+i, savefile))
      return(ERROR);
  }

  return(OK);
}

int32_t RestoreLocalAttributes(savefile)
 FILE *savefile;
{
  int i = 0;
  /* Total number of local attributes is stored in global variable nr_of_lattrs */

  for (i=0; i<nr_of_lattrs; i++) {
    if (!ReadAttribute(local_attrs+i, savefile))
      return(ERROR);
  }

  return(OK);
}

int32_t RestoreCommonFlags(savefile)
 FILE *savefile;
{
  int32_t com_loc_flags_string_len = 0;
  int32_t com_obj_flags_string_len = 0;
  int i = 0;

  /* calculate the length of the common flag strings */
  com_loc_flags_string_len = ((nr_of_cflags*nr_of_locs)/WORD_LEN)+1;
  com_obj_flags_string_len = ((nr_of_cflags*nr_of_objs)/WORD_LEN)+1;

  for (i=0; i<com_loc_flags_string_len; i++) {
    if (!ReadInt32(com_loc_flags+i, savefile)) {
      RestoreError("RestoreCommonFlags(): error reading common location flag data.\n");
      return(ERROR);
    }
  }

  for (i=0; i<com_obj_flags_string_len; i++) {
    if (!ReadInt32(com_obj_flags+i, savefile)) {
      RestoreError("RestoreCommonFlags(): error reading common object flag data.\n");
      return(ERROR);
    }
  }

  return(OK);
}

int32_t RestoreLocalFlags(savefile)
 FILE *savefile;
{
  int i = 0;

  /* Global var loc_flags_string_len is used here */
  for (i=0; i<loc_flags_string_len; i++) {
    if (!ReadInt32(local_flags+i, savefile)) {
      RestoreError("RestoreLocalFlags(): error reading local flag data.\n");
      return(ERROR);
    }
  }

  return(OK);
}

int32_t ReadTimer(timer, savefile)
 timerInfo *timer;
 FILE      *savefile;
{
  if (!ReadInt32(&(timer->value), savefile)) {
    RestoreError("ReadTimer(): error reading timerdata.\n");
    return(ERROR);
  }
  if (!ReadInt32(&(timer->step), savefile)) {
    RestoreError("ReadTimer(): error reading timerdata.\n");
    return(ERROR);
  }
  if (!ReadInt32(&(timer->interval), savefile)) {
    RestoreError("ReadTimer(): error reading timerdata.\n");
    return(ERROR);
  }
  if (!ReadInt32(&(timer->update), savefile)) {
    RestoreError("ReadTimer(): error reading timerdata.\n");
    return(ERROR);
  }
  if (!ReadInt16(&(timer->direction), savefile)) {
    RestoreError("ReadTimer(): error reading timerdata.\n");
    return(ERROR);
  }

  if (!ReadInt16(&(timer->state), savefile)) {
    RestoreError("ReadTimer(): error reading timerdata.\n");
    return(ERROR);
  }

  if (!ReadInt32(&(timer->trigger_at), savefile)) {
    RestoreError("ReadTimers(): error reading timerdata.\n");
    return(ERROR);
  }

  if (!ReadInt32(&(timer->trigger_spec), savefile)) {
    RestoreError("ReadTimers(): error reading timerdata.\n");
    return(ERROR);
  }

  if (!ReadInt32(&((timer->execute)[0]), savefile)) {
    RestoreError("ReadTimers(): error reading timerdata.\n");
    return(ERROR);
  }

  if (!ReadInt32((&(timer->execute[1])), savefile)) {
    RestoreError("ReadTimers(): error reading timerdata.\n");
    return(ERROR);
  }

  return(OK);
}


int32_t RestoreTimers(savefile)
 FILE *savefile;
{
  int i = 0;
 /* global vars timerInfo *timers and int nr_of_timers are used here */

  for (i=0; i<nr_of_timers; i++) {
    if (!ReadTimer(timers+i, savefile))
      return(ERROR);
  }

  return(OK);
}


