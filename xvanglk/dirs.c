
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "dirs.h"

/***********/
/* globals */
/***********/

dirData   dirs;


int32_t InitDirs()
{
  /* read directory offsets */
  if (!ReadDirOffsets(&dirs))
    return(ERROR);

  /* read the vocabulary (word_table and default code directory) */
  /* There will always be a vocabulary.                          */
  if (!ReadVoc(dirs.word_data_offset))
    return(ERROR);

  /* read the location directory */
  /* There will always be a location directory. */
  /* Make sure the location directory is read before the mapdata. */
  if (!ReadLocDir(dirs.loc_dir_offset))
    return(ERROR);

  /* read the object directory */
  if (dirs.obj_dir_offset != -1)
    if (!ReadObjDir(dirs.obj_dir_offset))
      return(ERROR);

  /* Read the common triggers. */
  if (dirs.common_trigg_offset != -1)
    if (!RdCTriggs(dirs.common_trigg_offset))
      return(ERROR);

  /* Read the common descriptions. */
  /* Read the (local) trigger owners. */
  if (dirs.trigg_owners_offset != -1)
    if (!RdTrigOwners(dirs.trigg_owners_offset))
      return(ERROR);

  /* Read the (local) description owners. */
  if (dirs.descr_owners_offset != -1)
    if (!RdDescOwners(dirs.descr_owners_offset))
      return(ERROR);

  /* read the map data */
  /* Make sure the location directory has already been read. */
  if (dirs.exit_data_offset != -1)
    if (!ReadMapData(dirs.exit_data_offset))
      return(ERROR);

  /* read the flag data */
  if (dirs.flag_data_offset != -1)
    if (!ReadFlags(dirs.flag_data_offset))
      return(ERROR);

  /* read data about timers */
  if (dirs.counter_data_offset != -1)
    if (!ReadTimers(dirs.counter_data_offset))
      return(ERROR);

  return(OK);
}
