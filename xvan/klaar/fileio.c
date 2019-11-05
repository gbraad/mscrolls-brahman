
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
#include "fileio.h"

/***********/
/* Globals */
/***********/

/* CHECK TO SEE IF ALL THESE NEED TO BE GLOBALS!!!! */

int32_t nr_of_locs           = 0;   /* Must be global for Find() and more                        */
int32_t nr_of_objs           = 0;   /* Must be global for Find() and more                        */
int32_t nr_of_cflags         = 0;   /* Must be global for SetBitVal()                            */
int32_t loc_flags_string_len = 0;   /* Must be global for Save() and Restore()                   */
int32_t nr_of_cattrs         = 0;
int32_t nr_of_lattrs         = 0;
int32_t nr_of_directions     = 0;   /* Global for GoDir()                                        */
int32_t nr_of_timers         = 0;   /* Must be global for UpdateTimers() and for save() function */
int32_t nr_of_words          = 0;
int32_t nr_of_verbs          = 0;

int32_t first_direction_id;

verbDir      *verb_dir        = NULL;
wordTable    *word_table      = NULL;
dirInfo      *loc_dir         = NULL;
dirInfo      *obj_dir         = NULL;
int32_t      **common_trigs   = NULL;
int32_t      *trigg_owners    = NULL;
int32_t      *descr_owners    = NULL;
attrInfo     *c_loc_attrs     = NULL;
attrInfo     *c_obj_attrs     = NULL;
attrInfo     *local_attrs     = NULL;
int32_t      *exit_data       = NULL;
int32_t      *com_loc_flags   = NULL;   /* Common location flags.   */
int32_t      *com_obj_flags   = NULL;   /* Common object flags.     */
int32_t      *local_flags     = NULL;   /* Local flags.             */
verbInfo     **verbs;                   /* Default verb code array. */
locationInfo **locations;               /* Location pointer array.  */
objectInfo   **objects;                 /* Object pointer array.    */
timerInfo    *timers          = NULL;   /* Timer array.             */

/*************************/
/* function declarations */
/*************************/

char     *ReadString(void);
int32_t  GetNextCode16(int16_t*);
int32_t  GetNextCode32(int32_t*);
int32_t  GetNextCode64(int64_t*);
int32_t  ReadDirOffsets(dirData*);
int32_t  ReadVoc(int64_t);
int32_t  ReadStoryInfo(storyInfo*);
int32_t  ReadWordTable(int64_t);
int32_t  ReadVerbDir(int64_t);
int32_t  ReadTimers(int64_t);
int32_t  ReadMapData(int64_t);
int32_t  ReadFlags(int64_t);
int32_t  ReadLocDir(int64_t);
int32_t  ReadObjDir(int64_t);
int32_t  RdCTriggs(int64_t);
int32_t  InitVerbs(void);
int32_t  InitLocations(void);
int32_t  InitObjects(void);
int32_t  InitAttributes(void);
int32_t  RdTrigOwners(int64_t);
int32_t  RdDescOwners(int64_t);
void     InitSysDescr(sysDescr*);   /* @!@ */
int32_t  ReadExtendedSysDescr(extendedSysDescr*);
int32_t  ReadSysDescr(sysDescr*);
int32_t  ReadContData(contData*);
int32_t  InMem(int32_t);
int32_t  Load(int32_t);

/***********************/
/* file input routines */
/***********************/

char *ReadString(void)
{
  int32_t len;   /* length of the string */
  char    *str;

  /* First, read the length of the string. */
  if (!GetNextCode32(&len)) {
    PrintError(14, NULL, "ReadString");
    return(NULL);
  }

  /* create space on heap */
  if ((str = (char *) malloc(len*sizeof(char))) == NULL) {
    PrintError(15, NULL, "ReadString()");
    return(NULL);
  }

  /* read the string */
  if (fread((void *) str, sizeof(char), len, datafile) != (size_t)len) {
    PrintError(15, NULL, "ReadString()");
    return(NULL);
  }

  /* all went well */
  /* don't forget to free(str) in calling function */
  return(str);
}


int32_t GetNextCode16(int16_t *code)
{
  if (fread((void *) code, sizeof(int16_t), 1, datafile) != 1) {
    PrintError(15, NULL, "GetNextCode16()");
    code = ERROR;
    return(ERROR);
  }

  /* convert from big endian to host format */
  *code = xv_ntohs(*code);

  /* all went well */
  return(OK);
}


int32_t GetNextCode32(int32_t *code)
{
  if (fread((void *) code, sizeof(int32_t), 1, datafile) != 1) {
    PrintError(15, NULL, "GetNextCode32()");
    code = ERROR;
    return(ERROR);
  }

  /* convert from big endian to host format */
  *code = xv_ntohl(*code);

  /* all went well */
  return(OK);
}


int32_t GetNextCode64(int64_t *code)
{
  if (fread((void *) code, sizeof(int64_t), 1, datafile) != 1) {
    PrintError(15, NULL, "GetNextCode64()");
    code = ERROR;
    return(ERROR);
  }

  /* convert from big endian to host format */
  *code = xv_ntohll(*code);

  /* all went well */
  return(OK);
}


int32_t ReadDirOffsets(dirData *dirs)
{
  int64_t offset; /* remember position in file */

  /* save current position in file */
  offset = ftell(datafile);

  /* go to start of file (where dirData are) */
  if (fseek(datafile, 0, 0) == -1) {
    PrintError(16, NULL, "ReadDirOffsets()");
    return(ERROR);
  }

  /* Read the dirs struct, member by member */
  if (!GetNextCode64(&dirs->story_info_offset)) {
    PrintError(17, NULL, "story_info offset");
    return(ERROR);
  }

  if (!GetNextCode64(&dirs->word_data_offset)) {
    PrintError(17, NULL, "word data offset");
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->loc_dir_offset)) {
    PrintError(17, NULL, "location directory offset");
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->obj_dir_offset)) {
    PrintError(17, NULL, "object directory offset");
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->common_descr_offset)) {
    PrintError(17, NULL, "common descriptions offset");
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->common_trigg_offset)) {
    PrintError(17, NULL, "common triggers offset");
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->exit_data_offset)) {
    PrintError(17, NULL, "exit data offset");
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->flag_data_offset)) {
    PrintError(17, NULL, "flag data offset");
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->trigg_owners_offset)) {
    PrintError(17, NULL, "trigger owners offset");
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->descr_owners_offset)) {
    PrintError(17, NULL, "description owners offset");
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->counter_data_offset)) {
    PrintError(17, NULL, "timer data offset");
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->attribute_data_offset)) {
    PrintError(17, NULL, "attribute dataoffset");
    return(ERROR);
  }

  /* restore the file position */
  if (fseek(datafile, offset, 0) == -1) {
    PrintError(18, NULL, NULL);
    return(ERROR);
  }

  return(OK);
}


int32_t ReadTimers(int64_t offset)
{
  int32_t code;
  int32_t id;
  int32_t i;
  int32_t index;

  /* go to offset in datafile. */
  if (fseek(datafile, offset, 0) == -1) {
    PrintError(20, NULL, "timers");
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    PrintError(19, NULL, "keyword record");
    return(ERROR);
  }
  if (code != TIMERS) {
    PrintError(21, NULL, "timers");
    return(ERROR);
  }

  /* read the number of timers */
  if (!GetNextCode32(&nr_of_timers)) {
    PrintError(19, NULL, "number of timers");
    return(ERROR);
  }

  /* Malloc() an array of timerInfo structs (not pointers!!) */
  if ((timers = (timerInfo *) malloc(nr_of_timers*sizeof(timerInfo))) == NULL) {
    PrintError(15, NULL, "ReadTimers()");
    return(ERROR);
  }

  /* The timerInfo structs stored by the compiler also had the     */
  /* timer id in them, since they were in a linked list. However,  */
  /* we now are going to read the timers in an array that is       */
  /* indexed by timer id. The timerInfo struct we use, doen't have */
  /* the id field. We therefore read the id first and then index   */
  /* the array with it.                                            */

  for (i=0; i<nr_of_timers; i++) {
    /* Read the timer id. */
    if (!GetNextCode32(&id)) {
      PrintError(19, NULL, "timer id");
      return(ERROR);
    }

    index = id-FIRST_TIMER_ID;

    if (!GetNextCode32(&(timers[index].value))) {
      PrintError(19, NULL, "timer value");
      return(ERROR);
    }

    if (!GetNextCode32(&(timers[index].step))) {
      PrintError(19, NULL, "timer step");
      return(ERROR);
    }

    if (!GetNextCode32(&(timers[index].interval))) {
      PrintError(19, NULL, "timer interval");
      return(ERROR);
    }

    if (!GetNextCode32(&(timers[index].update))) {
      PrintError(19, NULL, "timer update value");
      return(ERROR);
    }

    if (!GetNextCode16(&(timers[index].direction))) {
      PrintError(19, NULL, "timer direction");
      return(ERROR);
    }

    if (!GetNextCode16(&(timers[index].state))) {
      PrintError(19, NULL, "timer state");
      return(ERROR);
    }

    if (!GetNextCode32(&(timers[index].trigger_at))) {
      PrintError(19, NULL, "timer threshold");
      return(ERROR);
    }

    if (!GetNextCode32(&(timers[index].trigger_spec))) {
      PrintError(19, NULL, "trigger_spec");
      return(ERROR);
    }

    if (!GetNextCode32(&(timers[index].execute[0]))) {
      PrintError(19, NULL, "timer trigger");
      return(ERROR);
    }
    if (!GetNextCode32(&(timers[index].execute[1]))) {
      PrintError(19, NULL, "timer trigger");
      return(ERROR);
    }
  } /* for */

  /* Check if we must read debug info */
  if (debug_info) {
    /* check for the keyword */
    if (!GetNextCode32(&code)) {
      PrintError(23, NULL, "keyword record");
      return(ERROR);
    }
    if (code != DEBUG) {
      PrintError(21, NULL, "debug");
      return(ERROR);
    }

    /* Malloc() space for timer debug info. */
    if ((timer_dbug = (debugInfo *) malloc(nr_of_timers*sizeof(debugInfo))) == NULL) {
      PrintError(15, NULL, "timer debug info");
      return(ERROR);
    }

    for (i=0; i<nr_of_timers; i++) {
      if ( ((timer_dbug[i]).name = ReadString()) == NULL)
        return(ERROR);
    }
  }

  return(OK);
}


int32_t ReadMapData(int64_t offset)
{
  int32_t i;
  int32_t code;
  int32_t size;     /* size of map_data */

  /* Make sure that nr_of_locs has been read yet. */
  /* This is done by ReadLocDir().                */

  /* go to offset in datafile */
  if (fseek(datafile, offset, 0) == -1) {
    PrintError(16, NULL, "ReadMapData()");
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    PrintError(22, NULL, "keyword record");
    return(ERROR);
  }
  if (code != EXITS) {
    PrintError(21, NULL, "exits");
    return(ERROR);
  }

  /* Read the number of directions. */
  if (!GetNextCode32(&nr_of_directions)) {
    PrintError(22, NULL, "nr_of_directions");
    return(ERROR);
  }

  /* Read the first direction id. */
  if (!GetNextCode32(&first_direction_id)) {
    PrintError(22, NULL, "first_direction_id");
    return(ERROR);
  }

  /* Calculate size of exit_data */
  size = nr_of_locs*nr_of_directions;

  /* create space on heap */
  if ((exit_data = (int32_t *) malloc(size*sizeof(int32_t))) == NULL) {
    PrintError(15, NULL, "ReadMapData()");
    return(ERROR);
  }

  /* read the information */
  for (i=0; i<size; i++) {
    if (!GetNextCode32(&(exit_data[i]))) {
    PrintError(22, NULL, "exit_data");
    return(ERROR);
    }
  }

  return(OK);
}


int32_t ReadFlags(int64_t offset)
{
  int32_t code;
  size_t  com_loc_len = 0; /* Length of common location flags string.                            */
  size_t  com_obj_len = 0; /* Length of common object flags string.                              */
                       /* Length of local flags string is a global variable for save() function. */

  int32_t nr_of_lflags = 0;
  int i                = 0;

   /* go to offset in datafile */
  if (fseek(datafile, offset, 0) == -1) {
    PrintError(16, NULL, "ReadFlags()");
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    PrintError(23, NULL, "keyword record");
    return(ERROR);
  }
  if (code != FLAGS) {
    PrintError(21, NULL, "flags");
    return(ERROR);
  }
  /* Read number of common flagbits.                  */
  /* We need the number for the SetBitVal() function. */
  if (!GetNextCode32(&nr_of_cflags)) {
    PrintError(23, NULL, "nr_of_cflags");
    return(ERROR);
  }

  /* calculate com_loc_len and com_obj_len. */
  com_loc_len = ((nr_of_cflags*nr_of_locs)/WORD_LEN)+1;
  com_obj_len = ((nr_of_cflags*nr_of_objs)/WORD_LEN)+1;

  /* Create space on heap for com_loc_flags. */
  if ((com_loc_flags = (int32_t *) malloc(com_loc_len*sizeof(int32_t))) == NULL) {
    PrintError(15, NULL, "ReadFlags()");
    return(ERROR);
  }

  /* Create space on heap for com_obj_flags. */
  if ((com_obj_flags = (int32_t *) malloc(com_obj_len*sizeof(int32_t))) == NULL) {
    PrintError(15, NULL, "ReadFlags()");
    return(ERROR);
  }

  /* Read the com_loc_flags. */
  if (fread((void *) com_loc_flags, sizeof(int32_t), com_loc_len, datafile)
      != com_loc_len) {
    PrintError(23, NULL, "common location flags");
    return(ERROR);
  }

  /* Read the com_obj_flags. */
  if (fread((void *) com_obj_flags, sizeof(int32_t), com_obj_len, datafile)
      != com_obj_len) {
    PrintError(23, NULL, "common object flags");
    return(ERROR);
  }

  /* Read length of local flagbits string.           */
  /* We don't need the actual number of local flags. */
  if (!GetNextCode32(&loc_flags_string_len)) {
    PrintError(23, NULL, "loc_flags_string_len");
    return(ERROR);
  }

  /* Create space on heap.             */
  if ((local_flags = (int32_t *) malloc(loc_flags_string_len*sizeof(int32_t))) == NULL) {
    PrintError(15, NULL, "ReadFlags()");
    return(ERROR);
  }

  /* read the information */
  if (fread((void *) local_flags, sizeof(int32_t), loc_flags_string_len, datafile)
      != (size_t)loc_flags_string_len) {
    PrintError(23, NULL, "local flags");
    return(ERROR);
  }

  /* Check if we must read debug info */
  if (debug_info) {
    /* check for the keyword */
    if (!GetNextCode32(&code)) {
      PrintError(23, NULL, "keyword record");
      return(ERROR);
    }
    if (code != DEBUG) {
      PrintError(21, NULL, "debug");
      return(ERROR);
    }

    /* Malloc() space for common flags debug info. */
    if ((com_flag_dbug = (debugInfo *) malloc(nr_of_cflags*sizeof(debugInfo))) == NULL) {
      PrintError(15, NULL, "common flags debug info");
      return(ERROR);
    }

    for (i=0; i<nr_of_cflags; i++) {
      if ( ((com_flag_dbug[i]).name = ReadString()) == NULL)
        return(ERROR);
    }
    /* read the number of local flags */
    /* is is only needed when we have debug info */
    if (!GetNextCode32(&nr_of_lflags)) {
      PrintError(23, NULL, "nr_of_lflags");
      return(ERROR);
    }

    /* Malloc() space for local flags debug info. */
    if ((loc_flag_dbug = (debugInfo *) malloc(nr_of_lflags*sizeof(debugInfo))) == NULL) {
      PrintError(15, NULL, "local flags debug info");
      return(ERROR);
    }

    for (i=0; i<nr_of_lflags; i++) {
      if ( ((loc_flag_dbug[i]).name = ReadString()) == NULL)
        return(ERROR);

      if (!GetNextCode32(&(loc_flag_dbug[i]).owner))
        return(ERROR);
    }
  }

  return(OK);
}


int32_t ReadStoryInfo(storyInfo *info)
{
  dirData dirs;
  int32_t len;

  if (!ReadDirOffsets(&dirs))
    return(ERROR);

  /* go to story_info in datafile. */
  if (fseek(datafile, dirs.story_info_offset, 0) == -1) {
    PrintError(16, NULL, "ReadStoryInfo()");
    return(ERROR);
  }

  /* read story_info struct */
  if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "title");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->title, len, 1, datafile) != 1) {
      PrintError(24, NULL, "title");
      return(ERROR);
    }
  }

  if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "author");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->author, len, 1, datafile) != 1) {
      PrintError(24, NULL, "author");
      return(ERROR);
    }
  }

  if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "organization");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->organization, len, 1, datafile) != 1) {
      PrintError(24, NULL, "organization");
      return(ERROR);
    }
  }

  if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "cover text");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->cover_text, len, 1, datafile) != 1) {
      PrintError(24, NULL, "cover text");
      return(ERROR);
    }
  }

  if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "credits");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->credits, len, 1, datafile) != 1) {
      PrintError(24, NULL, "credits");
      return(ERROR);
    }
  }

  if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "version");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->version, len, 1, datafile) != 1) {
      PrintError(24, NULL, "version");
      return(ERROR);
    }
  }

   if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "android market");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->android_mkt, len, 1, datafile) != 1) {
      PrintError(24, NULL, "android market");
      return(ERROR);
    }
  }

  if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "ios market");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->ios_mkt, len, 1, datafile) != 1) {
      PrintError(24, NULL, "ios market");
      return(ERROR);
    }
  }

  if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "back image");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->backimage, len, 1, datafile) != 1) {
      PrintError(24, NULL, "back image");
      return(ERROR);
    }
  }

  if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "effect");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->effect, len, 1, datafile) != 1) {
      PrintError(24, NULL, "effect");
      return(ERROR);
    }
  }

  if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "primary_color");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->primary_color, len, 1, datafile) != 1) {
      PrintError(24, NULL, "primary_color");
      return(ERROR);
    }
  }

  if (!GetNextCode16(&(info->ui_sidebar))) {
    PrintError(24, NULL, "sidebar");
    return(ERROR);
  }

  if (!GetNextCode16(&(info->ui_textinput))) {
    PrintError(24, NULL, "text input");
    return(ERROR);
  }

  if (!GetNextCode16(&(info->ui_compass))) {
    PrintError(24, NULL, "compass");
    return(ERROR);
  }

  if (!GetNextCode16(&(info->autolink))) {
    PrintError(24, NULL, "autolink");
    return(ERROR);
  }

  if (!GetNextCode32(&len)) {
    PrintError(24, NULL, "compiler version");
    return(ERROR);
  }
  if (len != 0) {
    if (fread((void *) info->compiler_version, len, 1, datafile) != 1) {
      PrintError(24, NULL, "compiler version");
      return(ERROR);
    }
  }

  if (!GetNextCode16(&(info->xvan_language))) {
    PrintError(24, NULL, "XVAN language");
    return(ERROR);
  }

  if (!GetNextCode16(&(info->story_language))) {
    PrintError(24, NULL, "story language");
    return(ERROR);
  }

  if (!GetNextCode16(&(info->play_mode))) {
    PrintError(24, NULL, "play mode");
    return(ERROR);
  }

  if (!GetNextCode16(&(debug_info))) {
    PrintError(24, NULL, "debug mode");
    return(ERROR);
  }

  return(OK);
}


int32_t ReadWordTable(int64_t offset)
{
  int32_t code;
  int   i=0;
  int   j=0;

  /* go to offset in datafile */
  if (fseek(datafile, offset, 0) == -1) {
    PrintError(16, NULL, "ReadWOrdTable()");
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    PrintError(25, NULL, "keyword record");
    return(ERROR);
  }
  if (code != VOCABULARY) {
    PrintError(21, NULL, "word table");
    return(ERROR);
  }

  /* read the number of words */
  if (!GetNextCode32(&nr_of_words)) {
    PrintError(25, NULL, "nr_of_words");
    return(ERROR);
  }

  /* create space on heap */
  if ((word_table = (wordTable *) malloc(nr_of_words*sizeof(wordTable))) == NULL) {
    PrintError(15, NULL, "ReadWordTable()");
    return(ERROR);
  }

  /* read the information */
  for (i=0; i<nr_of_words; i++) {
    if (fread((void *) &(word_table[i].word), sizeof(char), MAX_WORD_LEN+1,
              datafile) != MAX_WORD_LEN+1) {
      PrintError(25, NULL, "word");
      return(ERROR);
    }
    if (fread((void *) &(word_table[i].print_word), sizeof(char),
              MAX_WORD_LEN+1, datafile) != MAX_WORD_LEN+1) {
      PrintError(25, NULL, "printword");
      return(ERROR);
    }
    if (!GetNextCode32(&word_table[i].id)) {
      PrintError(25, NULL, "word identifier");
      return(ERROR);
    }
    if (!GetNextCode32(&word_table[i].single_id)) {  /* @!@ */
      PrintError(25, NULL, "single identifier");
      return(ERROR);
    }
    for (j=0; j<MAX_TYPES; j++) {
      if (!GetNextCode32(&(word_table[i].types[j]))) {
        PrintError(25, NULL, "word type");
        return(ERROR);
      }
    }
  }

  return(OK);
}


int32_t ReadVerbDir(int64_t offset)
{
  int32_t code;
  int32_t i;

  /* set right position in file */
  if (fseek(datafile, offset, 0) == -1) {
    PrintError(16, NULL, "ReadVerbDir()");
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code))
    return(ERROR);

  if (code != VERB) {
    PrintError(21, NULL, "verb directory");
    return(ERROR);
  }

  /* read length */
  if (!GetNextCode32(&nr_of_verbs)) {
    PrintError(26, NULL, "nr_of_verbs");
    return(ERROR);
  }

  /* initialize the default code directory */
  if ((verb_dir = (verbDir *) malloc(nr_of_verbs*sizeof(verbDir))) == NULL) {
    PrintError(15, NULL, " ReadVerbDir()");
    return(ERROR);
  }

  for (i=0; i<nr_of_verbs; i++) {
    if (!GetNextCode32(&(verb_dir[i].scope))) {
      PrintError(15, NULL, "ReadVerbDir()");
      return(ERROR);
    }
    if (!GetNextCode64(&(verb_dir[i].offset))) {
      PrintError(26, NULL, "verb directory");
      return(ERROR);
    }
  }

  return(OK);
}


int32_t ReadVoc(int64_t offset)
{
  /* read the word_table */
  if (!ReadWordTable(offset))
    return(ERROR);

  /* Read the default verb code table */
  /* Give the current offset in file as the offset parameter */
  if (!ReadVerbDir(ftell(datafile)))
    return(ERROR);

  return(OK);
}


int32_t ReadLocDir(int64_t offset)
{
  int32_t code;
  int32_t i, j;

  /* set right position in file */
  if (fseek(datafile, offset, 0) == -1) {    /* 0 means ???? */
    PrintError(16, NULL, "ReadLocDir()");
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    PrintError(27, NULL, "keyword record");
    return(ERROR);
  }
  if (code != LOC_DIR) {
    PrintError(21, NULL, "location directory");
    return(ERROR);
  }

  /* read number of locations */
  if (!GetNextCode32(&nr_of_locs))
    return(ERROR);

  /* initialize the location directory */
  if ((loc_dir = (dirInfo *) malloc(nr_of_locs*sizeof(dirInfo))) == NULL) {
    PrintError(15, NULL, "ReadLocDir()");
    return(ERROR);
  }

  /* read the location directory */
  for (i=0; i<nr_of_locs; i++) {
    if (!GetNextCode32(&(loc_dir[i].nr_of_dsys))) {
      PrintError(27, NULL, "location directory");
      free(loc_dir);
      return(ERROR);
    }
    for (j=0; j<MAX_DSYS; j++) {
      if (!ReadExtendedSysDescr(&(loc_dir[i].descr[j]))) {
        free(loc_dir);
        return(ERROR); /* error message was printed by ReadSysDescr() */
      }
    }
    if (!ReadContData(&(loc_dir[i].contained_objs))) {
      free(loc_dir);
      return(ERROR); /* error message was printed by ReadSysDescr() */
    }
    if (!GetNextCode32(&(loc_dir[i].held_by))) {
      PrintError(27, NULL, "location directory");
      free(loc_dir);
      return(ERROR);
    }
    if (!GetNextCode64(&(loc_dir[i].offset))) {
      PrintError(27, NULL, "location directory");
      free(loc_dir);
      return(ERROR);
    }
  }

    /* Check if we must read debug info */
  if (debug_info) {
    /* check for the keyword */
    if (!GetNextCode32(&code)) {
      PrintError(23, NULL, "keyword record");
      return(ERROR);
    }
    if (code != DEBUG) {
      PrintError(21, NULL, "debug");
      return(ERROR);
    }

    /* Malloc() space for location debug info. */
    if ((loc_dbug = (debugInfo *) malloc(nr_of_locs*sizeof(debugInfo))) == NULL) {
      PrintError(15, NULL, "location debug info");
      return(ERROR);
    }

    for (i=0; i<nr_of_locs; i++) {
      if ( ((loc_dbug[i]).name = ReadString()) == NULL)
        return(ERROR);
    }
  }

  return(OK);
}


int32_t ReadObjDir(int64_t offset)
{
  int32_t code;
  int32_t i, j;

  /* set right position in file */
  if (fseek(datafile, offset, 0) == -1) {    /* 0 means ???? */
    PrintError(16, NULL, "ReadObjDir()");
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    PrintError(28, NULL, "keyword record");
    return(ERROR);
  }

  if (code != OBJ_DIR) {
    PrintError(21, NULL, "object directory");
    return(ERROR);
  }

  /* read length */
  if (!GetNextCode32(&nr_of_objs))
    return(ERROR);

  /* initialize the object directory */
  if ((obj_dir = (dirInfo *) malloc(nr_of_objs*sizeof(dirInfo))) == NULL) {
    PrintError(15, NULL, "ReadObjDir()");
    return(ERROR);
  }

  /* read the object directory */
  for (i=0; i<nr_of_objs; i++) {
    if (!GetNextCode32(&(obj_dir[i].nr_of_dsys))) {
      PrintError(28, NULL, "object directory");
      free(obj_dir);
      return(ERROR);
    }
    for (j=0; j<MAX_DSYS; j++) {
      if (!ReadExtendedSysDescr(&(obj_dir[i].descr[j]))) {
        free(obj_dir);
        return(ERROR); /* error message was printed by StoreSysDescr() */
      }
    }
    if (!ReadContData(&(obj_dir[i].contained_objs))) {
      free(obj_dir);
      return(ERROR); /* error message was printed by StoreSysDescr() */
    }
    if (!GetNextCode32(&(obj_dir[i].held_by))) {
      PrintError(28, NULL, "object directory");
      free(obj_dir);
      return(ERROR);
    }
    if (!GetNextCode64(&(obj_dir[i].offset))) {
      PrintError(28, NULL, "object directory");
      free(obj_dir);
      return(ERROR);
    }
  }

   /* Check if we must read debug info */
  if (debug_info) {
    /* check for the keyword */
    if (!GetNextCode32(&code)) {
      PrintError(23, NULL, "keyword record");
      return(ERROR);
    }
    if (code != DEBUG) {
      PrintError(21, NULL, "debug");
      return(ERROR);
    }

    /* Malloc() space for location debug info. */
    if ((obj_dbug = (debugInfo *) malloc(nr_of_objs*sizeof(debugInfo))) == NULL) {
      PrintError(15, NULL, "object debug info");
      return(ERROR);
    }

    for (i=0; i<nr_of_objs; i++) {
      if ( ((obj_dbug[i]).name = ReadString()) == NULL)
        return(ERROR);
    }
  }

  return(OK);
}


int32_t RdCTriggs(int64_t offset)
{
  int32_t size = LAST_COMMON_TRIGGER_ID-FIRST_COMMON_TRIGGER_ID+1;
  int32_t code;
  int32_t i;
  int32_t index;

  if ((common_trigs = (int32_t **) malloc(size*sizeof(int32_t*))) == NULL) {
    PrintError(15, NULL, "RdCTriggs()");
    return(ERROR);
  }

  /* Initialize the common trigger pointer array. */
  for (i=0; i<size; i++)
    common_trigs[i] = NULL;

  /* Go to offset in datafile. */
  if (fseek(datafile, offset, 0) == -1) {
    PrintError(16, NULL, "RdCTriggs()");
    return(ERROR);
  }

  /* Check for the keyword. */
  if (!GetNextCode32(&code)) {
    PrintError(29, NULL, "keyword record");
    return(ERROR);
  }

  if (code != COMMON_TRIGGERS) {
    PrintError(21, NULL, "common triggers");
    return(ERROR);
  }

  /* Ok, we're all set. Now read the triggers.              */
  /* We don't know how many common triggers are defined and */
  /* how many have their own code.                          */

  GetNextCode32(&code);

  while (1) {
    /* code should be a common trigger id. */
    if (IsCTriggId(code)) {
      index = code-FIRST_COMMON_TRIGGER_ID;
      /* Read next code. This is either the first code of the */
      /* trigger, the next trigger id, or something we don't  */
      /* know yet.                                            */

      GetNextCode32(&code);
      if ((common_trigs[index] = RdTrCode(&code)) == NULL)
        return(ERROR);
    }
    else
      /* Not a common trigger id. */
      return(code);
  } /* while */
  /* No return here. */
}


int32_t InitVerbs(void)
{
  int32_t  i        = 0;
  int32_t  read_nr  = 0;
  int64_t  offset;
  verbInfo *verb    = NULL;

  /* Create an array with a pointer for each verb. The array */
  /* will be indexed by verb id.                             */

  if ((verbs = (verbInfo **) malloc(nr_of_verbs*sizeof(verbInfo*)))
                                                            == NULL) {
    PrintError(15, NULL, "verbs pointer");
    return(ERROR);
  }

  /* Initialize the array. */
  for (i=0; i<nr_of_verbs; i++)
    verbs[i] = NULL;

  /* Read as many verbs as possible, which will be read_nr-1 */
  read_nr = (nr_of_verbs<MAX_VERBS_IN_MEM? nr_of_verbs:MAX_VERBS_IN_MEM);
  for (i=0; i<read_nr; i++) {
    /* Get verb's offset in datafile. */
    offset = verb_dir[i].offset;

    if (offset != -1) {
      /* Yes, there is default code. */
      if ((verb = RdVerb(offset)) == NULL)
        return(ERROR);

      verbs[i] = verb;
    }
    else
      /* This verb has no default code. */
      verbs[i] = NULL;

  } /* for */

  return(OK);
}


int32_t InitLocations(void)
{
  int32_t  i        = 0;
  int32_t  read_nr  = 0;
  int64_t offset;
  locationInfo *loc = NULL;

  /* Create an array with a pointer for each location. The array */
  /* will be indexed by location id.                             */

  if ((locations = (locationInfo **) malloc(nr_of_locs*sizeof(locationInfo*)))
                                                            == NULL) {
    PrintError(15, NULL, "locations pointer");
    return(ERROR);
  }

  /* Initialize the array. */
  for (i=0; i<nr_of_locs; i++)
    locations[i] = NULL;

  /* Read as many locations as possible, which will be read_nr-1 */
  read_nr = (nr_of_locs<MAX_LOCS_IN_MEM? nr_of_locs:MAX_LOCS_IN_MEM);

  for (i=0; i<read_nr; i++) {
    /* Get location's offset in datafile. */
    offset = loc_dir[i].offset;

    /* read the location.        */
    if ((loc = ReadLocation(offset)) == NULL)
      return(ERROR);

    locations[i] = loc;
  } /* for */

  return(OK);
}


int32_t InitObjects(void)
{
  int32_t    i       = 0;
  int32_t    read_nr = 0;
  int64_t    offset;
  objectInfo *obj    = NULL;

  /* Create an array with a pointer for each object. The array */
  /* will be indexed by object id.                             */

  if ((objects = (objectInfo **) malloc(nr_of_objs*sizeof(objectInfo*)))
                                                            == NULL) {
    PrintError(15, NULL, "objects pointer");
    return(ERROR);
  }

  /* Initialize the array. */
  for (i=0; i<nr_of_objs; i++)
    objects[i] = NULL;

  /* Read as many objects as possible, which will be read_nr */
  read_nr = (nr_of_objs<MAX_OBJS_IN_MEM? nr_of_objs:MAX_OBJS_IN_MEM);
  for (i=0; i<read_nr; i++) {
    /* Get object's offset in datafile. */
    offset = obj_dir[i].offset;

    /* read the object.                 */
    if ((obj = ReadObject(offset)) == NULL)
      return(ERROR);

    objects[i] = obj;
  } /* for */

  return(OK);
}


int32_t InitAttributes(void)
{
  /* Make sure that nr_of_locs and nr_of_objs have been set. */

  int32_t  com_loc_len;
  int32_t  com_obj_len;
  int32_t  code;
  int32_t  i;

  /* Set right position in file.     */
  /* Don't know what '0' means here. */
  if (fseek(datafile, dirs.attribute_data_offset, 0) == -1) {
    PrintError(16, NULL, "InitAttributes()");
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    PrintError(30, NULL, "keyword record");
    return(ERROR);
  }
  if (code != ATTRIBUTES) {
    PrintError(21, NULL, "attribute data");
    return(ERROR);
  }
  /* read nr of common attributes */
  if (!GetNextCode32(&nr_of_cattrs))
    return(ERROR);

  com_loc_len = nr_of_cattrs*nr_of_locs;
  com_obj_len = nr_of_cattrs*nr_of_objs;

  /* Malloc() space for common location attributes. */
  if ((c_loc_attrs = (attrInfo *) malloc(com_loc_len*sizeof(attrInfo))) == NULL) {
    PrintError(15, NULL, "InitAttributes()");
    return(ERROR);
  }

  /* Malloc() space for common object attributes. */
  if ((c_obj_attrs = (attrInfo *) malloc(com_obj_len*sizeof(attrInfo))) == NULL) {
    PrintError(15, NULL, "common object attributes");
    return(ERROR);
  }

  /* Read the common location attributes. */
  for (i=0; i<com_loc_len; i++) {
    if (!GetNextCode32(&c_loc_attrs[i].type)) {
      PrintError(30, NULL, "common location attribute type");
      return(ERROR);
    }
    if (!GetNextCode32(&c_loc_attrs[i].value)) {
      PrintError(30, NULL, "common location attribute value");
      return(ERROR);
    }
    if (!GetNextCode32(&c_loc_attrs[i].value_owner)) { /* 10march2017 */
      PrintError(30, NULL, "common location attribute value owner");
      return(ERROR);
    }
  }

  /* Now read the common object attributes. */
  for (i=0; i<com_obj_len; i++) {
    if (!GetNextCode32(&c_obj_attrs[i].type)) {
      PrintError(30, NULL, "common objectattribute type");
      return(ERROR);
    }
    if (!GetNextCode32(&c_obj_attrs[i].value)) {
      PrintError(30, NULL, "common objectattribute value");
      return(ERROR);
    }
    if (!GetNextCode32(&c_obj_attrs[i].value_owner)) { /* 10march2017 */
      PrintError(30, NULL, "common objectattribute value owner");
      return(ERROR);
    }
  }

  /* read nr of local attributes */
  if (!GetNextCode32(&nr_of_lattrs))
    return(ERROR);

  /* Malloc() space for local attributes. */
  if ((local_attrs = (attrInfo *) malloc(nr_of_lattrs*sizeof(attrInfo))) == NULL) {
    PrintError(15, NULL, "InitAttributes()");
    return(ERROR);
  }

  /* Read the local attributes. */
  for (i=0; i<nr_of_lattrs; i++) {
    if (!GetNextCode32(&local_attrs[i].type)) {
      PrintError(30, NULL, "local attribute type");
      return(ERROR);
    }
    if (!GetNextCode32(&local_attrs[i].value)) {
      PrintError(30, NULL, "local attribute value");
      return(ERROR);
    }
    if (!GetNextCode32(&local_attrs[i].value_owner)) { /* 10march2017 */
      PrintError(30, NULL, "local attribute value owner");
      return(ERROR);
    }
  }

  /* Check if we must read debug info */
  if (debug_info) {
    /* check for the keyword */
    if (!GetNextCode32(&code)) {
      PrintError(23, NULL, "keyword record");
      return(ERROR);
    }
    if (code != DEBUG) {
      PrintError(21, NULL, "debug");
      return(ERROR);
    }

    /* Malloc() space for common attribute debug info. */
    if ((com_attr_dbug = (debugInfo *) malloc(nr_of_cattrs*sizeof(debugInfo))) == NULL) {
      PrintError(15, NULL, "common attributes debug info");
      return(ERROR);
    }

    /* Malloc() space for local flags debug info. */
    if ((loc_attr_dbug = (debugInfo *) malloc(nr_of_lattrs*sizeof(debugInfo))) == NULL) {
      PrintError(15, NULL, "local attributes debug info");
      return(ERROR);
    }

    for (i=0; i<nr_of_cattrs; i++) {
      if ( ((com_attr_dbug[i]).name = ReadString()) == NULL)
        return(ERROR);
    }
    for (i=0; i<nr_of_lattrs; i++) {
      if ( ((loc_attr_dbug[i]).name = ReadString()) == NULL)
        return(ERROR);
      if (!GetNextCode32(&(loc_attr_dbug[i]).owner))
        return(ERROR);
    }
  }

  /* Ready. */
  return(OK);
}


int32_t RdTrigOwners(int64_t offset)
{
  int32_t code;
  int32_t nr_of_ltrigs;
  int32_t nr_of_ctrigs;    /* only for debug mode */
  int32_t i;


  /* set right position in file */
  if (fseek(datafile, offset, 0) == -1) {    /* 0 means ???? */
    PrintError(16, NULL, "RdTrigOwners()");
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    PrintError(31, NULL, "keyword record");
    return(ERROR);
  }
  if (code != TRIGGERS) {
    PrintError(21, NULL, "trigger owners");
    return(ERROR);
  }
  /* read length */
  if (!GetNextCode32(&nr_of_ltrigs))
    return(ERROR);

  /* initialize the trigger owners array */
  if ((trigg_owners = (int32_t *) malloc(nr_of_ltrigs*sizeof(int32_t))) == NULL) {
    PrintError(15, NULL, "RdTrigOwners()");

    return(ERROR);
  }

  /* read the trigger owners */
  for (i=0; i<nr_of_ltrigs; i++) {
    if (!GetNextCode32(&(trigg_owners[i]))) {
      PrintError(31, NULL, "trigger owners");
      return(ERROR);
    }
  }

  /* Check if we must read debug info */
  if (debug_info) {
    /* check for the keyword */
    if (!GetNextCode32(&code)) {
      PrintError(23, NULL, "keyword record");
      return(ERROR);
    }
    if (code != DEBUG) {
      PrintError(21, NULL, "debug");
      return(ERROR);
    }

    /* read the number of common triggers */
    if (!GetNextCode32(&nr_of_ctrigs)) {
      PrintError(29, NULL, "nr_of_ctrigs");
      return(ERROR);
    }

    /* Malloc() space for common trigger debug info. */
    if ((com_trig_dbug = (debugInfo *) malloc(nr_of_ctrigs*sizeof(debugInfo))) == NULL) {
      PrintError(15, NULL, "common triggersdebug info");
      return(ERROR);
    }

    /* Malloc() space for local trigger debug info. */
    if ((loc_trig_dbug = (debugInfo *) malloc(nr_of_ltrigs*sizeof(debugInfo))) == NULL) {
      PrintError(15, NULL, "local trigger debug info");
      return(ERROR);
    }

    for (i=0; i<nr_of_ctrigs; i++) {
      if ( ((com_trig_dbug[i]).name = ReadString()) == NULL)
        return(ERROR);
    }
    for (i=0; i<nr_of_ltrigs; i++) {
      if ( ((loc_trig_dbug)[i].name = ReadString()) == NULL)
        return(ERROR);
      if (!GetNextCode32(&(loc_trig_dbug[i]).owner))
        return(ERROR);
    }
  }

  return(OK);
}


int32_t RdDescOwners(int64_t offset)
{
  int32_t code;
  int32_t nr_of_descrs;

  /* set right position in file */
  if (fseek(datafile, offset, 0) == -1) {    /* 0 means ???? */
    PrintError(16, NULL, "RdDescrOwners()");
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    PrintError(32, NULL, "keyword record");
    return(ERROR);
  }

  if (code != DESCRIPTIONS) {
    PrintError(21, NULL, "description owners");
    return(ERROR);
  }

  /* read length */
  if (!GetNextCode32(&nr_of_descrs))
    return(ERROR);

  /* initialize the description owners array */
  if ((descr_owners = (int32_t *) malloc(nr_of_descrs*sizeof(int32_t))) == NULL) {
    PrintError(15, NULL, "RdDescrOwners()");
    return(ERROR);
  }

  /* read the description owners */
  if (fread((void *) descr_owners, sizeof(int32_t), nr_of_descrs,
                                        datafile) != (size_t)nr_of_descrs) {
    PrintError(32, NULL, "description owners");
    return(ERROR);
  }

  return(OK);
}


void InitSysDescr(sysDescr *descr)
{
  int i = 0;

  /* this function initializes a sysDescr struct */
  descr->article = NO_ID;
  descr->nr_of_adjectives = 0;
  for (i=0; i<MAX_PARSE_ADJ; i++) {
    descr->adjectives[i] = NO_ID;
  }
  descr->noun = NO_ID;
  return;
}


int32_t ReadExtendedSysDescr(extendedSysDescr *descr)
{
  int32_t code = NO_ID;

  /* initialize the extended system description */
  descr->dynamic = NULL;
  InitSysDescr(&(descr->part1));
  descr->connect_prepos = NO_ID;
  InitSysDescr(&(descr->part2));

  if (!GetNextCode32(&code)) {
    PrintError(33, NULL, "DYN_DSYS or DSYS keyword");
    return(ERROR);
  }

  if (code == DYN_DSYS) {
    descr->dynamic = ReadString();
    return(OK);
  }

  /* it is a 'normal'  system description */

  if (!ReadSysDescr(&(descr->part1)))
    return(ERROR);

  if (!GetNextCode32(&(descr->connect_prepos))) {
    PrintError(33, NULL, "connecting preposition");
    return(ERROR);
  }

  if (!ReadSysDescr(&(descr->part2)))
    return(ERROR);

  return(OK);
}


int32_t ReadSysDescr(sysDescr *sys_descr)
{
  int i=0;

  if (!GetNextCode32(&(sys_descr->article))) {
    PrintError(34, NULL, "article");
    return(ERROR);
  }
  if (!GetNextCode32(&(sys_descr->nr_of_adjectives))) {
    PrintError(34, NULL, "nr_of_adjectives");
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_ADJ; i++) {
    if (!GetNextCode32(&(sys_descr->adjectives[i]))) {
      PrintError(34, NULL, "adjective");
      return(ERROR);
    }
  }
  if (!GetNextCode32(&(sys_descr->noun))) {
    PrintError(34, NULL, "noun");
    return(ERROR);
  }

  return(OK);
}


int32_t ReadContData(contData *cont_data)
{
  int i=0;

  if (!GetNextCode32(&(cont_data->nr_of_objects))) {
    PrintError(35, NULL, "nr_of_objects");
    return(ERROR);
  }
  for (i=0; i<MAX_CONTAINED_OBJECTS; i++) {
    if (!GetNextCode32(&(cont_data->object_ids[i]))) {
    PrintError(35, NULL, "object id");
    return(ERROR);
    }
  }
return(OK);
}


int32_t InMem(int32_t id)
{
  /* Id must either be a location or an object id. */
  /* This must have been checked by caller.        */

  if (IsVerbId(id)) {
    if (verbs[id - FIRST_VERB_ID] == NULL)
      return(ERROR);
  } /* Brackets needed for coming else. */
  else if (IsLocId(id)) {
    if (locations[id - FIRST_LOCATION_ID] == NULL)
      return(ERROR);
  }
  else  /* It must be an object id. */
    if (objects[id - FIRST_OBJECT_ID] == NULL)
      return(ERROR);

  /* OK, id is in memory. */

  return(OK);
}


int32_t Load(int32_t id)
{
  /* Check for valid id (verb, location or object) must */
  /* have been done by caller.                          */
  int64_t offset = -1;

  if (IsVerbId(id)) {
    /* Read the verb default code, but test offset first. */
    offset = verb_dir[id - FIRST_VERB_ID].offset;
    if (offset == -1)
      /* No default code exists for this verb. */
      return(NO_MATCH);
    else
      if ((verbs[id-FIRST_VERB_ID] = RdVerb(offset)) == NULL) {
        PrintError(36, NULL, "verb default code");
        return(ERROR);
      }
  }
  else if (IsLocId(id)) {
    /* Read the location. */
    offset = loc_dir[id-FIRST_LOCATION_ID].offset;
    if ((locations[id-FIRST_LOCATION_ID] = ReadLocation(offset))
                                                          == NULL)
      return(ERROR);
  }
  else {
    /* Assume it's an object. */
    /* Read the object. */
    offset = obj_dir[id-FIRST_OBJECT_ID].offset;
    if ((objects[id-FIRST_OBJECT_ID] = ReadObject(offset)) == NULL)
      return(ERROR);
  }

  return(OK);
}
