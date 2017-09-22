
/* Copyright 2016, 2017 Marnix van den Bos */

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

void  SaveError(char*);
int32_t SaveSpecialIds(FILE*);
int32_t StoreInt16(int16_t, FILE*);
int32_t StoreInt32(int32_t, FILE*);
int32_t Storeint64(int64_t, FILE*);
int32_t StoreId(int32_t, FILE*);
int32_t SaveStoryInfo(FILE*);
int32_t SaveExtendedSysDescr(extendedSysDescr*, FILE*);
int32_t SaveSysDescr(sysDescr*, FILE*);
int32_t SaveContData(contData*, FILE*);
int32_t SaveDirInfo(dirInfo*, FILE*);
int32_t SaveLocationDirectory(FILE*);
int32_t SaveObjectDirectory(FILE*);
int32_t SaveExits(FILE*);
int32_t SaveAttribute(attrInfo*, FILE*);
int32_t SaveCommonAttributes(FILE*);
int32_t SaveLocalAttributes(FILE*);
int32_t SaveCommonFlags(FILE*);
int32_t SaveLocalFlags(FILE*);
int32_t SaveTimer(timerInfo*, FILE*);
int32_t SaveAllTimers(FILE*);



void SaveError(error_string)
 char *error_string;
{
  sprintf(outputline, "%s", error_string);
  Output(outputline);
}

int32_t StoreInt16(n, savefile)
 int16_t  n;
 FILE *savefile;
{
  /* convert to big endian */
  n = htons(n);

  if (fwrite((void *) &n, sizeof(int16_t), 1, savefile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);
  return(OK);
}

int32_t StoreInt32(n, savefile)
 int32_t  n;
 FILE *savefile;
{
  /* convert to big endian */
  n = htonl(n);

  if (fwrite((void *) &n, sizeof(int32_t), 1, savefile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);
  return(OK);
}

int32_t StoreInt64(n, savefile)
 int64_t n;
 FILE *savefile;
{
  /* convert to big endian */
  n = htonll(n);

  if (fwrite((void *) &n, sizeof(int64_t), 1, savefile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);
  return(OK);
}

int32_t StoreId(id, savefile)
 int32_t id;
 FILE *savefile;
{
  if (!StoreInt32(id, savefile)) {
    SaveError("StoreId(): error writing id.\n");
    return(ERROR);
  }
  return(OK);
}


int32_t SaveSpecialIds(savefile)
  FILE *savefile;
{
  int32_t special_id_array[NR_OF_SPECIAL_IDS];
  int i = 0;

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
    if (!StoreInt32(special_id_array[i], savefile)) {
      SaveError("SaveSpecialIds(): error writing data.\n");
    return(ERROR);
    }
  }

  return(OK);
}

int32_t SaveStoryInfo(savefile)
 FILE *savefile;
{
  /* Store the storyInfo struct. */
  if (fwrite((void *) story_info.title, 1, MAX_TITLE_LEN, savefile) != MAX_TITLE_LEN) {
    SaveError("SaveStoryInfo(): write error.\n");
    return(ERROR);
  }

  if (fwrite((void *) story_info.version, 1, MAX_VERSION_LEN, savefile) != MAX_VERSION_LEN) {
    SaveError("SaveStoryInfo(): write error.\n");
    return(ERROR);
  }

  if (fwrite((void *) story_info.compiler_version, 1, MAX_COMPILER_LEN, savefile) != MAX_COMPILER_LEN) {
    SaveError("SaveStoryInfo(): write error.\n");
    return(ERROR);
  }
  return(OK);
}

int32_t SaveExtendedSysDescr(extended_system_description, savefile)
 extendedSysDescr *extended_system_description;
 FILE             *savefile;
{
  if (!SaveSysDescr(&(extended_system_description->part1), savefile))
    return(ERROR);

  if (!StoreInt32(extended_system_description->connect_prepos, savefile)) {
      SaveError("SaveExtendedSystemDescription(): error writing connecting preposition.\n");
      return(ERROR);
  }

  if (!SaveSysDescr(&(extended_system_description->part2), savefile))
    return(ERROR);

  return(OK);
}

int32_t SaveSysDescr(system_description, savefile)
 sysDescr *system_description;
 FILE     *savefile;
{
  int32_t i=0;

  if (!StoreInt32(system_description->article, savefile)) {
    SaveError("SaveSysDescr(): error writing data.\n");
    return(ERROR);
  }

  if (!StoreInt32(system_description->nr_of_adjectives, savefile)) {
    SaveError("SaveSysDescr(): error writing data.\n");
    return(ERROR);
  }

  for (i=0; i<MAX_PARSE_ADJ; i++)
    if (!StoreId((system_description->adjectives)[i], savefile)) {
      SaveError("SaveSysDescr(): error writing data.\n");
      return(ERROR);
    }

  if (!StoreId(system_description->noun, savefile)) {
    SaveError("SaveSysDescr(): error writing data.\n");
    return(ERROR);
  }
  return(OK);
}

int32_t SaveContData(cont_data, savefile)
 contData *cont_data;
 FILE     *savefile;
{
  int32_t i=0;

  if (!StoreInt32(cont_data->nr_of_objects, savefile)) {
    SaveError("SaveContData(): error writing data.\n");
    return(ERROR);
  }

  for (i=0; i<MAX_CONTAINED_OBJECTS; i++)
    if (!StoreId((cont_data->object_ids)[i], savefile)) {
      SaveError("StoreContData(): error writing data.\n");
      return(ERROR);
    }
  return(OK);
}

int32_t SaveDirInfo(dir_info, savefile)
 dirInfo *dir_info;
 FILE*    savefile;
{
  int i = 0;

  /* store nr_of_dsys */
  if (!StoreInt32(dir_info->nr_of_dsys, savefile)) {
    SaveError("SaveDirInfo(): error writing data.\n");
    return(ERROR);
  }

  /* store dsys instances */
  for (i=0; i<MAX_DSYS; i++) {
    if (!SaveExtendedSysDescr(dir_info->descr+i, savefile))
      return(ERROR);
  }

  /* store contained objects */
  if (!SaveContData(&(dir_info->contained_objs), savefile))
      return(ERROR);

  /* store containing object or location */
  if (!StoreInt32(dir_info->held_by, savefile)) {
    SaveError("SaveDirInfo(): error writing data.\n");
    return(ERROR);
  }

  /* store offset */
  if (!StoreInt64(dir_info->offset, savefile)) {
    SaveError("SaveDirInfo(): error writing data.\n");
    return(ERROR);
  }

  return(OK);
}


int32_t SaveLocationDirectory(savefile)
 FILE *savefile;
{
  int32_t i = 0;

  for (i=0; i<nr_of_locs; i++) {
    if (!SaveDirInfo(loc_dir+i, savefile))
      return(ERROR);
  }

  return(OK);
}

int32_t SaveObjectDirectory(savefile)
 FILE *savefile;
{
  int32_t i=0;

  for (i=0; i<nr_of_objs; i++) {
    if (!SaveDirInfo(obj_dir+i, savefile))
      return(ERROR);
  }
  return(OK);
}

int32_t SaveExits(savefile)
 FILE *savefile;
{
  int32_t size; /* size of the exit_data array */
  int     i = 0;

  size = nr_of_locs * nr_of_directions;

  for (i=0; i<size; i++) {
    if (!StoreInt32(exit_data[i], savefile)) {
      SaveError("SaveExits(): error writing data.\n");
      return(ERROR);
    }
  }

  return(OK);
}

int32_t SaveAttribute(attribute, savefile)
 attrInfo *attribute;
 FILE* savefile;
{
  if (!StoreInt32(attribute->type, savefile)) {
    SaveError("SaveAttribute(): error writing data.\n");
    return(ERROR);
  }

  if (!StoreInt32(attribute->value, savefile)) {
    SaveError("SaveAttribute(): error writing data.\n");
    return(ERROR);
  }

  if (!StoreInt32(attribute->value_owner, savefile)) {
    SaveError("SaveAttribute(): error writing data.\n"); /* 13march2017 */
    return(ERROR);
  }

  return(OK);
}


int32_t SaveCommonAttributes(savefile)
 FILE *savefile;
{
  int32_t total_attrs; /* Total number of common attributes for all locations */
  int i = 0;

  /* Store the common location attributes */
  total_attrs = nr_of_cattrs*nr_of_locs;

  for (i=0; i<total_attrs; i++) {
    if (!SaveAttribute(c_loc_attrs+i, savefile))
      return(ERROR);
  }

  /* Store the common object attributes */
  total_attrs = nr_of_cattrs*nr_of_objs;

  for (i=0; i<total_attrs; i++) {
    if (!SaveAttribute(c_obj_attrs+i, savefile))
      return(ERROR);
  }

  return(OK);
}

int32_t SaveLocalAttributes(savefile)
 FILE *savefile;
{
  int i = 0;
  /* Total number of local attributes is stored in global variable nr_of_lattrs */

  for (i=0; i<nr_of_lattrs; i++) {
    if (!SaveAttribute(local_attrs+i, savefile))
      return(ERROR);
  }

  return(OK);
}

int32_t SaveCommonFlags(savefile)
 FILE *savefile;
{
  int32_t com_loc_flags_string_len = 0;
  int32_t com_obj_flags_string_len = 0;
  int i = 0;

  /* calculate the length of the common flag strings */
  com_loc_flags_string_len = ((nr_of_cflags*nr_of_locs)/WORD_LEN)+1;
  com_obj_flags_string_len = ((nr_of_cflags*nr_of_objs)/WORD_LEN)+1;

  for (i=0; i<com_loc_flags_string_len; i++) {
    if (!StoreInt32(com_loc_flags[i], savefile)) {
      SaveError("SaveCommonFlags(): error writing common location flag data.\n");
      return(ERROR);
    }
  }

  for (i=0; i<com_obj_flags_string_len; i++) {
    if (!StoreInt32(com_obj_flags[i], savefile)) {
      SaveError("SaveCommonFlags(): error writing common object flag data.\n");
      return(ERROR);
    }
  }

  return(OK);
}

int32_t SaveLocalFlags(savefile)
 FILE *savefile;
{
  int i = 0;

  /* Global var loc_flags_string_len is used here */
  for (i=0; i<loc_flags_string_len; i++) {
    if (!StoreInt32(local_flags[i], savefile)) {
      SaveError("SaveLocalFlags(): error writing local flag data.\n");
      return(ERROR);
    }
  }

  return(OK);
}

int32_t SaveTimer(timer, savefile)
 timerInfo *timer;
 FILE      *savefile;
{
  if (!StoreInt32(timer->value, savefile)) {
    SaveError("SaveTimer(): error writing timerdata.\n");
    return(ERROR);
  }
  if (!StoreInt32(timer->step, savefile)) {
    SaveError("SaveTimer(): error writing timerdata.\n");
    return(ERROR);
  }
  if (!StoreInt32(timer->interval, savefile)) {
    SaveError("SaveTimer(): error writing timerdata.\n");
    return(ERROR);
  }
  if (!StoreInt32(timer->update, savefile)) {
    SaveError("SaveTimer(): error writing timerdata.\n");
    return(ERROR);
  }
  if (!StoreInt16(timer->direction, savefile)) {
    SaveError("SaveTimer(): error writing timerdata.\n");
    return(ERROR);
  }
  if (!StoreInt16(timer->state, savefile)) {
    SaveError("SaveTimer(): error writing timerdata.\n");
    return(ERROR);
  }
  if (!StoreInt32(timer->trigger_at, savefile)) {
    SaveError("SaveTimer(): error writing timerdata.\n");
    return(ERROR);
  }
  if (!StoreInt32(timer->trigger_spec, savefile)) {
    SaveError("SaveTimer(): error writing timerdata.\n");
    return(ERROR);
  }
  if (!StoreInt32((timer->execute)[0], savefile)) {
    SaveError("SaveTimer(): error writing timerdata.\n");
    return(ERROR);
  }
  if (!StoreInt32((timer->execute[1]), savefile)) {
    SaveError("SaveTimer(): error writing timerdata.\n");
    return(ERROR);
  }

  return(OK);
}


int32_t SaveAllTimers(savefile)
 FILE *savefile;
{
  int i = 0;
 /* global vars timerInfo *timers and int nr_of_timers are used here */

  for (i=0; i<nr_of_timers; i++) {
    if (!SaveTimer(timers+i, savefile))
      return(ERROR);
  }

  return(OK);
}


