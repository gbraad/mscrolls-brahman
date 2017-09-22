
/* Copyright 2016, 2017 Marnix van den Bos */

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
int32_t  ReadExtendedSysDescr(extendedSysDescr*);
int32_t  ReadSysDescr(sysDescr*);
int32_t  ReadContData(contData*);
int32_t  InMem(int32_t);
int32_t  Load(int32_t);

/***********************/
/* file input routines */
/***********************/

char *ReadString()
{
  int32_t len;   /* length of the string */
  char *str;

  /* First, read the length of the string. */
  if (!GetNextCode32(&len)) {
    sprintf(outputline, "\n\nReadString(): error reading length.\n\n");
    Output(outputline);
    str = NULL;
    return(ERROR);
  }

  /* create space on heap */
  if ((str = (char *) malloc(len*sizeof(char))) == NULL) {
    sprintf(outputline, "\n\nReadString(): error in malloc().\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read the string */
  if (fread((void *) str, sizeof(char), len, datafile) != len) {
    sprintf(outputline, "\n\nReadString(): error in reading string.\n\n");
    Output(outputline);
    str = NULL;
    return(ERROR);
  }

  /* all went well */
  /* don't forget to free(str) in calling function */
  return(str);
}

int32_t GetNextCode16(code)
 int16_t *code;
{
  if (fread((void *) code, sizeof(int16_t), 1, datafile) != 1) {
    sprintf(outputline, "\n\nGetNextCode16(): error reading code.\n\n");
    Output(outputline);
    code = ERROR;
    return(ERROR);
  }

  /* convert from big endian to host format */
  *code = ntohs(*code);

  /* all went well */
  return(OK);
}

int32_t GetNextCode32(code)
 int32_t *code;
{
  if (fread((void *) code, sizeof(int32_t), 1, datafile) != 1) {
    sprintf(outputline, "\n\nGetNextCode32(): error reading code.\n\n");
    Output(outputline);
    code = ERROR;
    return(ERROR);
  }

  /* convert from big endian to host format */
  *code = ntohl(*code);

  /* all went well */
  return(OK);
}

int32_t GetNextCode64(code)
 int64_t *code;
{
  if (fread((void *) code, sizeof(int64_t), 1, datafile) != 1) {
    sprintf(outputline, "\n\nGetNextCode64(): error reading code.\n\n");
    Output(outputline);
    code = ERROR;
    return(ERROR);
  }

  /* convert from big endian to host format */
  *code = ntohll(*code);

  /* all went well */
  return(OK);
}

int32_t ReadDirOffsets(dirs)
 dirData *dirs;
{
  int64_t offset; /* remember position in file */

  /* save current position in file */
  offset = ftell(datafile);

  /* go to start of file (where dirData are) */
  if (fseek(datafile, 0, 0) == -1) {
    sprintf(outputline, "\n\nReadDirOffsets(): error in fseek(0).\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* Read the dirs struct, member by member */
  if (!GetNextCode64(&dirs->story_info_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading story info offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  if (!GetNextCode64(&dirs->word_data_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading word data offset.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->loc_dir_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading location directory offset.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->obj_dir_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading object directory offset.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->common_descr_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading common descriptions offset.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->common_trigg_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading common triggers offset.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->exit_data_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading exit data offset.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->flag_data_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading flag data offset.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->trigg_owners_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading trigger owners offset.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->descr_owners_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading description owners offset.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->counter_data_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading timer data offset.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (!GetNextCode64(&dirs->attribute_data_offset)) {
    sprintf(outputline, "\n\nReadDirOffsets(): error reading attribute data offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* restore the file position */
  if (fseek(datafile, offset, 0) == -1) {
    sprintf(outputline, "\n\nReadDirOffsets(): error restoring position in file.\n\n");
    Output(outputline);
    return(ERROR);
  }

  return(OK);
}

int32_t ReadTimers(offset)
 int64_t     offset;
{
  int32_t code;
  int32_t id;
  int32_t i;
  int32_t index;

  /* go to offset in datafile. */
  if (fseek(datafile, offset, 0) == -1) {
    sprintf(outputline, "ReadTimers(): illegal offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    sprintf(outputline, "\n\nReadTimers(): error reading keyword rec.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (code != TIMERS) {
    sprintf(outputline, "\n\nReadTimers(): wrong offset\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read the number of timers */
  if (!GetNextCode32(&nr_of_timers)) {
    sprintf(outputline, "\n\nReadTimers(): error reading number of timers\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* Malloc() an array of timerInfo structs (not pointers!!) */
  if ((timers = (timerInfo *) malloc(nr_of_timers*sizeof(timerInfo))) == NULL) {
    sprintf(outputline, "ReadTimers(): error in malloc().\n");
    Output(outputline);
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
      sprintf(outputline, "ReadTimers(): error reading timer id.\n");
      Output(outputline);
      return(ERROR);
    }

    index = id-FIRST_TIMER_ID;

    if (!GetNextCode32(&(timers[index].value))) {
      sprintf(outputline, "ReadTimers(): error reading timer value.\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&(timers[index].step))) {
      sprintf(outputline, "ReadTimers(): error reading timer step.\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&(timers[index].interval))) {
      sprintf(outputline, "ReadTimers(): error reading timer interval.\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&(timers[index].update))) {
      sprintf(outputline, "ReadTimers(): error reading timer update.\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode16(&(timers[index].direction))) {
      sprintf(outputline, "ReadTimers(): error reading timer direction.\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode16(&(timers[index].state))) {
      sprintf(outputline, "ReadTimers(): error reading timer state.\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&(timers[index].trigger_at))) {
      sprintf(outputline, "ReadTimers(): error reading timer threshold.\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&(timers[index].trigger_spec))) {
      sprintf(outputline, "ReadTimers(): error reading timer trigger_spec.\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&(timers[index].execute[0]))) {
      sprintf(outputline, "ReadTimers(): error reading timer trigger.\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&(timers[index].execute[1]))) {
      sprintf(outputline, "ReadTimers(): error reading timer trigger.\n");
      Output(outputline);
      return(ERROR);
    }
  } /* for */

  return(OK);
}

int32_t ReadMapData(offset)
 int64_t offset;
{
  int32_t i;
  int32_t code;
  int32_t size;     /* size of map_data */

  /* Make sure that nr_of_locs has been read yet. */
  /* This is done by ReadLocDir().                */

  /* go to offset in datafile */
  if (fseek(datafile, offset, 0) == -1) {
    sprintf(outputline, "\n\nReadMapData(): illegal offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    sprintf(outputline, "\n\nReadMapdata(): error reading keyword rec\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (code != EXITS) {
    sprintf(outputline, "\n\nReadExits(): wrong offset\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* Read the number of directions. */
  if (!GetNextCode32(&nr_of_directions)) {
    sprintf(outputline, "\n\nReadMapdata(): error reading nr_of_directions.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* Read the first direction id. */
  if (!GetNextCode32(&first_direction_id)) {
    sprintf(outputline, "\n\nReadMapdata(): error reading first_direction_id.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* Calculate size of exit_data */
  size = nr_of_locs*nr_of_directions;

  /* create space on heap */
  if ((exit_data = (int32_t *) malloc(size*sizeof(int32_t))) == NULL) {
    sprintf(outputline, "\n\nReadMapData(): error in malloc().\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read the information */
  for (i=0; i<size; i++) {
    if (!GetNextCode32(&(exit_data[i]))) {
    sprintf(outputline, "\n\nReadMapData(): read error.\n\n");
    Output(outputline);
    return(ERROR);
    }
  }

  return(OK);
}

int32_t ReadFlags(offset)
 int64_t offset;
{
  int32_t code;
  int32_t com_loc_len = 0; /* Length of common location flags string.                                */
  int32_t com_obj_len = 0; /* Length of common object flags string.                                  */
                       /* Length of local flags string is a global variable for save() function. */
   /* go to offset in datafile */
  if (fseek(datafile, offset, 0) == -1) {
    sprintf(outputline, "\n\nReadFlags(): illegal offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    sprintf(outputline, "\n\nReadFlags(): error reading keyword rec\n\n");
    return(ERROR);
  }
  if (code != FLAGS) {
    sprintf(outputline, "\n\nReadFlags(): wrong offset\n\n");
    Output(outputline);
    return(ERROR);
  }
  /* Read number of common flagbits.                  */
  /* We need the number for the SetBitVal() function. */
  if (!GetNextCode32(&nr_of_cflags)) {
    sprintf(outputline, "\n\nReadFlags(): error reading nr_of_cflags.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* calculate com_loc_len and com_obj_len. */
  com_loc_len = ((nr_of_cflags*nr_of_locs)/WORD_LEN)+1;
  com_obj_len = ((nr_of_cflags*nr_of_objs)/WORD_LEN)+1;

  /* Create space on heap for com_loc_flags. */
  if ((com_loc_flags = (int32_t *) malloc(com_loc_len*sizeof(int32_t))) == NULL) {
    sprintf(outputline, "\n\nReadFlags(): error in malloc().\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* Create space on heap for com_obj_flags. */
  if ((com_obj_flags = (int32_t *) malloc(com_obj_len*sizeof(int32_t))) == NULL) {
    sprintf(outputline, "\n\nReadFlags(): error in malloc().\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* Read the com_loc_flags. */
  if (fread((void *) com_loc_flags, sizeof(int32_t), com_loc_len, datafile)
      != com_loc_len) {
    sprintf(outputline, "\n\nReadFlags(): error reading common location flags.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* Read the com_obj_flags. */
  if (fread((void *) com_obj_flags, sizeof(int32_t), com_obj_len, datafile)
      != com_obj_len) {
    sprintf(outputline, "\n\nReadFlags(): error reading common object flags.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* Read length of local flagbits string.           */
  /* We don't need the actual number of local flags. */
  if (!GetNextCode32(&loc_flags_string_len)) {
    sprintf(outputline, "\n\nReadFlags(): error reading loc_flags_string_len.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* Create space on heap.             */
  if ((local_flags = (int32_t *) malloc(loc_flags_string_len*sizeof(int32_t))) == NULL) {
    sprintf(outputline, "\n\nReadFlags(): error in malloc().\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read the information */
  if (fread((void *) local_flags, sizeof(int32_t), loc_flags_string_len, datafile)
      != loc_flags_string_len) {
    sprintf(outputline, "\n\nReadFlags(): error reading local flags.\n\n");
    Output(outputline);
    return(ERROR);
  }

  return(OK);
}

int32_t ReadStoryInfo(info)
 storyInfo *info;
{
  dirData dirs;

  if (!ReadDirOffsets(&dirs))
    return(ERROR);

  /* go to story_info in datafile. */
  if (fseek(datafile, dirs.story_info_offset, 0) == -1) {
    sprintf(outputline, "ReadStoryInfo(): illegal offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read story_info struct */
  if (fread((void *) info->title, MAX_TITLE_LEN, 1, datafile) != 1) {
    sprintf(outputline, "ReadStoryInfo(): title read error\n");
    Output(outputline);
    return(ERROR);
  }

  if (fread((void *) info->version, MAX_VERSION_LEN, 1, datafile) != 1) {
    sprintf(outputline, "ReadStoryInfo(): version read error\n");
    Output(outputline);
    return(ERROR);
  }

  if (fread((void *) info->compiler_version, MAX_COMPILER_LEN, 1, datafile) != 1) {
    sprintf(outputline, "ReadStoryInfo(): compuier version read error\n");
    Output(outputline);
    return(ERROR);
  }
  return(OK);
}


int32_t ReadWordTable(offset)
 int64_t offset;
{
  int32_t code;
  int   i=0;
  int   j=0;

  /* go to offset in datafile */
  if (fseek(datafile, offset, 0) == -1) {
    sprintf(outputline, "\n\nReadWordTable(): illegal offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    sprintf(outputline, "\n\nReadWordTable(): error reading keyword rec\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (code != VOCABULARY) {
    sprintf(outputline, "\n\nReadWordTable(): wrong offset\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read the number of words */
  if (!GetNextCode32(&nr_of_words)) {
    sprintf(outputline, "\n\nReadWordTable(): error reading nr_of_words.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* create space on heap */
  if ((word_table = (wordTable *) malloc(nr_of_words*sizeof(wordTable))) == NULL) {
    sprintf(outputline, "\n\nReadWordTable(): error in malloc().\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read the information */
  for (i=0; i<nr_of_words; i++) {
    if (fread((void *) &(word_table[i].word), sizeof(char), MAX_WORD_LEN+1,
              datafile) != MAX_WORD_LEN+1) {
      sprintf(outputline, "\n\nReadWordTable(): error reading word.\n\n");
      Output(outputline);
      return(ERROR);
    }
    if (fread((void *) &(word_table[i].print_word), sizeof(char),
              MAX_WORD_LEN+1, datafile) != MAX_WORD_LEN+1) {
      sprintf(outputline, "\n\nReadWordTable(): error reading printword.\n\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&word_table[i].id)) {
      sprintf(outputline, "\n\nReadWordTable(): error reading word id.\n\n");
      Output(outputline);
      return(ERROR);
    }
    for (j=0; j<MAX_TYPES; j++) {
      if (!GetNextCode32(&(word_table[i].types[j]))) {
        sprintf(outputline, "\n\nReadWordTable(): error reading word type.\n\n");
        Output(outputline);
        return(ERROR);
      }
    }
  }

  return(OK);
}

int32_t ReadVerbDir(offset)
 int64_t offset;
{
  int32_t code;
  int32_t i;

  /* set right position in file */
  if (fseek(datafile, offset, 0) == -1) {
    sprintf(outputline, "\n\nReadVerbDir(): wrong offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    sprintf(outputline, "\n\nReadVerbDir(): error reading keyword rec\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (code != VERB) {
    sprintf(outputline, "\n\nReadVerbDir(): wrong offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read length */
  if (!GetNextCode32(&nr_of_verbs))
    return(ERROR);

  /* initialize the default code directory */
  if ((verb_dir = (verbDir *) malloc(nr_of_verbs*sizeof(verbDir))) == NULL) {
    sprintf(outputline, "\n\nReadVerbDir(): error in malloc().\n\n");
    Output(outputline);
    return(ERROR);
  }

  for (i=0; i<nr_of_verbs; i++) {
    if (!GetNextCode32(&(verb_dir[i].scope))) {
      sprintf(outputline, "ReadVerbDir(): error reading verb directory).\n\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode64(&(verb_dir[i].offset))) {
      sprintf(outputline, "ReadVerbDir(): error reading verb directory).\n\n");
      Output(outputline);
      return(ERROR);
    }
  }

  return(OK);
}

int32_t ReadVoc(offset)
 int64_t offset;
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

int32_t ReadLocDir(offset)
 int64_t offset;
{
  int32_t code;
  int32_t i, j;

  /* set right position in file */
  if (fseek(datafile, offset, 0) == -1) {    /* 0 means ???? */
    sprintf(outputline, "\n\nReadLocDir(): wrong offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    sprintf(outputline, "\n\nReadLocDir(): error reading keyword rec\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (code != LOC_DIR) {
    sprintf(outputline, "\n\nReadLocDir(): wrong offset\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read number of locations */
  if (!GetNextCode32(&nr_of_locs))
    return(ERROR);

  /* initialize the location directory */
  if ((loc_dir = (dirInfo *) malloc(nr_of_locs*sizeof(dirInfo))) == NULL) {
    sprintf(outputline, "\n\nReadLocDir(): error in malloc().\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read the location directory */
  for (i=0; i<nr_of_locs; i++) {
    if (!GetNextCode32(&(loc_dir[i].nr_of_dsys))) {
      sprintf(outputline, "\n\nReadLocDir(): read error.\n\n");
      Output(outputline);
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
      sprintf(outputline, "\n\nReadLocDir(): read error.\n\n");
      Output(outputline);
      free(loc_dir);
      return(ERROR);
    }
    if (!GetNextCode64(&(loc_dir[i].offset))) {
      sprintf(outputline, "\n\nReadLocDir(): read error.\n\n");
      Output(outputline);
      free(loc_dir);
      return(ERROR);
    }
  }

  return(OK);
}

int32_t ReadObjDir(offset)
 int64_t offset;
{
  int32_t code;
  int32_t i, j;

  /* set right position in file */
  if (fseek(datafile, offset, 0) == -1) {    /* 0 means ???? */
    sprintf(outputline, "\n\nReadObjDir(): wrong offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    sprintf(outputline, "\n\nReadObjDir(): error reading keyword rec\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (code != OBJ_DIR) {
    sprintf(outputline, "\n\nReadObjDir(): wrong offset\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read length */
  if (!GetNextCode32(&nr_of_objs))
    return(ERROR);

  /* initialize the object directory */
  if ((obj_dir = (dirInfo *) malloc(nr_of_objs*sizeof(dirInfo))) == NULL) {
    sprintf(outputline, "\n\nReadObjDir(): error in malloc().\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read the object directory */
  for (i=0; i<nr_of_objs; i++) {
    if (!GetNextCode32(&(obj_dir[i].nr_of_dsys))) {
      sprintf(outputline, "\n\nReadObjDir(): read error.\n\n");
      Output(outputline);
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
      sprintf(outputline, "\n\nReadObjDir(): read error.\n\n");
      Output(outputline);
      free(obj_dir);
      return(ERROR);
    }
    if (!GetNextCode64(&(obj_dir[i].offset))) {
      sprintf(outputline, "\n\nReadObjDir(): read error.\n\n");
      Output(outputline);
      free(obj_dir);
      return(ERROR);
    }
  }

  return(OK);
}


int32_t RdCTriggs(offset)
 int64_t offset;
{
  int32_t size = LAST_COMMON_TRIGGER_ID-FIRST_COMMON_TRIGGER_ID+1;
  int32_t code;
  int32_t i;
  int32_t index;

  if ((common_trigs = (int32_t **) malloc(size*sizeof(int32_t*))) == NULL) {
    sprintf(outputline, "RdCTriggs(): error in malloc.\n");
    Output(outputline);
    return(ERROR);
  }

  /* Initialize the common trigger pointer array. */
  for (i=0; i<size; i++)
    common_trigs[i] = NULL;

  /* Go to offset in datafile. */
  if (fseek(datafile, offset, 0) == -1) {
    sprintf(outputline, "RdCTriggs(): illegal offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* Check for the keyword. */
  if (!GetNextCode32(&code)) {
    sprintf(outputline, "\n\nRdCTriggs(): error reading keyword rec\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (code != COMMON_TRIGGERS) {
    sprintf(outputline, "\n\nRdCTriggs(): wrong offset\n\n");
    Output(outputline);
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

int32_t InitVerbs()
{
  int32_t  i        = 0;
  int32_t  read_nr  = 0;
  int64_t  offset;
  verbInfo *verb    = NULL;

  /* Create an array with a pointer for each verb. The array */
  /* will be indexed by verb id.                             */

  if ((verbs = (verbInfo **) malloc(nr_of_verbs*sizeof(verbInfo*)))
                                                            == NULL) {
    sprintf(outputline, "InitVerbs(): error in malloc for pointer.\n");
    Output(outputline);
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

int32_t InitLocations()
{
  int32_t  i            = 0;
  int32_t  read_nr      = 0;
  int64_t offset;
  locationInfo *loc = NULL;

  /* Create an array with a pointer for each location. The array */
  /* will be indexed by location id.                             */

  if ((locations = (locationInfo **) malloc(nr_of_locs*sizeof(locationInfo*)))
                                                            == NULL) {
    sprintf(outputline, "InitLocations(): error in malloc for pointer.\n");
    Output(outputline);
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

int32_t InitObjects()
{
  int32_t        i       = 0;
  int32_t        read_nr = 0;
  int64_t       offset;
  objectInfo *obj    = NULL;

  /* Create an array with a pointer for each object. The array */
  /* will be indexed by object id.                             */

  if ((objects = (objectInfo **) malloc(nr_of_objs*sizeof(objectInfo*)))
                                                            == NULL) {
    sprintf(outputline, "InitObjects(): error in malloc for pointer.\n");
    Output(outputline);
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

int32_t InitAttributes()
{
  /* Make sure that nr_of_locs and nr_of_objs have been set. */

  int32_t  com_loc_len;
  int32_t  com_obj_len;
  int32_t  code;
  int32_t  i;

  /* Set right position in file.     */
  /* Don't know what '0' means here. */
  if (fseek(datafile, dirs.attribute_data_offset, 0) == -1) {
    sprintf(outputline, "\n\nInitAttributes(): wrong offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    sprintf(outputline, "\n\nInitAttributes(): error reading keyword rec.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (code != ATTRIBUTES) {
    sprintf(outputline, "\n\nInitAttributes(): wrong offset.\n\n");
    Output(outputline);
    return(ERROR);
  }
  /* read nr of common attributes */
  if (!GetNextCode32(&nr_of_cattrs))
    return(ERROR);

  com_loc_len = nr_of_cattrs*nr_of_locs;
  com_obj_len = nr_of_cattrs*nr_of_objs;

  /* Malloc() space for common location attributes. */
  if ((c_loc_attrs = (attrInfo *) malloc(com_loc_len*sizeof(attrInfo))) == NULL) {
    sprintf(outputline, "InitAttributes(): error in malloc for common location attributes.\n");
    Output(outputline);
    return(ERROR);
  }

  /* Malloc() space for common object attributes. */
  if ((c_obj_attrs = (attrInfo *) malloc(com_obj_len*sizeof(attrInfo))) == NULL) {
    sprintf(outputline, "InitAttributes(): error in malloc for common object attributes.\n");
    Output(outputline);
    return(ERROR);
  }

  /* Read the common location attributes. */
  for (i=0; i<com_loc_len; i++) {
    if (!GetNextCode32(&c_loc_attrs[i].type)) {
      sprintf(outputline, "InitAttributes(): error reading common location attribute (type).\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&c_loc_attrs[i].value)) {
      sprintf(outputline, "InitAttributes(): error reading common location attribute (value).\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&c_loc_attrs[i].value_owner)) { /* 10march2017 */
      sprintf(outputline, "InitAttributes(): error reading common location attribute (value owner).\n");
      Output(outputline);
      return(ERROR);
    }
  }

  /* Now read the common object attributes. */
  for (i=0; i<com_obj_len; i++) {
    if (!GetNextCode32(&c_obj_attrs[i].type)) {
      sprintf(outputline, "InitAttributes(): error reading common object attribute (type).\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&c_obj_attrs[i].value)) {
      sprintf(outputline, "InitAttributes(): error reading common object attribute (value).\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&c_obj_attrs[i].value_owner)) { /* 10march2017 */
      sprintf(outputline, "InitAttributes(): error reading common object attribute (value owner).\n");
      Output(outputline);
      return(ERROR);
    }
  }

  /* read nr of local attributes */
  if (!GetNextCode32(&nr_of_lattrs))
    return(ERROR);

  /* Malloc() space for local attributes. */
  if ((local_attrs = (attrInfo *) malloc(nr_of_lattrs*sizeof(attrInfo))) == NULL) {
    sprintf(outputline, "InitAttributes(): error in malloc for local attributes.\n");
    Output(outputline);
    return(ERROR);
  }

  /* Read the local attributes. */
  for (i=0; i<nr_of_lattrs; i++) {
    if (!GetNextCode32(&local_attrs[i].type)) {
      sprintf(outputline, "InitAttributes(): error reading local attribute (type).\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&local_attrs[i].value)) {
      sprintf(outputline, "InitAttributes(): error reading local attribute (value).\n");
      Output(outputline);
      return(ERROR);
    }
    if (!GetNextCode32(&local_attrs[i].value_owner)) { /* 10march2017 */
      sprintf(outputline, "InitAttributes(): error reading local attribute (value owner).\n");
      Output(outputline);
      return(ERROR);
    }
  }

  /* Ready. */
  return(OK);
}


int32_t RdTrigOwners(offset)
 int64_t offset;
{
  int32_t code;
  int32_t nr_of_triggers;
  int32_t i;

  /* set right position in file */
  if (fseek(datafile, offset, 0) == -1) {    /* 0 means ???? */
    sprintf(outputline, "\n\nRdTrigOwners(): wrong offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    sprintf(outputline, "\n\nRdTrigOwners(): error reading keyword rec\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (code != TRIGGERS) {
    sprintf(outputline, "\n\nRdTrigOwners(): wrong offset\n\n");
    Output(outputline);
    return(ERROR);
  }
  /* read length */
  if (!GetNextCode32(&nr_of_triggers))
    return(ERROR);

  /* initialize the trigger owners array */
  if ((trigg_owners = (int32_t *) malloc(nr_of_triggers*sizeof(int32_t))) == NULL) {
    sprintf(outputline, "\n\nReadtriggOwners(): error in malloc().\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read the trigger owners */
  for (i=0; i<nr_of_triggers; i++) {
    if (!GetNextCode32(&(trigg_owners[i]))) {
      sprintf(outputline, "\n\nRdTrigOwners(): error reading trigger owners.\n\n");
      Output(outputline);
      return(ERROR);
    }
  }

  return(OK);
}


int32_t RdDescOwners(offset)
 int64_t offset;
{
  int32_t code;
  int32_t nr_of_descrs;

  /* set right position in file */
  if (fseek(datafile, offset, 0) == -1) {    /* 0 means ???? */
    sprintf(outputline, "\n\nRdDescOwners(): wrong offset.\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* check for the keyword */
  if (!GetNextCode32(&code)) {
    sprintf(outputline, "\n\nRdDescOwners(): error reading keyword rec\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (code != DESCRIPTIONS) {
    sprintf(outputline, "\n\nRdDescOwners(): wrong offset\n\n");
    Output(outputline);
    return(ERROR);
  }
  /* read length */
  if (!GetNextCode32(&nr_of_descrs))
    return(ERROR);

  /* initialize the description owners array */
  if ((descr_owners = (int32_t *) malloc(nr_of_descrs*sizeof(int32_t))) == NULL) {
    sprintf(outputline, "\n\nRdDescOwners(): error in malloc().\n\n");
    Output(outputline);
    return(ERROR);
  }

  /* read the description owners */
  if (fread((void *) descr_owners, sizeof(int32_t), nr_of_descrs,
                                        datafile) != nr_of_descrs) {
    sprintf(outputline, "\n\nRdDescOwners(): error reading description owners.\n\n");
    Output(outputline);
    return(ERROR);
  }

  return(OK);
}


int32_t ReadExtendedSysDescr(extended_sys_descr)
 extendedSysDescr *extended_sys_descr;
{
  if (!ReadSysDescr(&(extended_sys_descr->part1)))
    return(ERROR);

  if (!GetNextCode32(&(extended_sys_descr->connect_prepos))) {
    sprintf(outputline, "\n\nReadExtendedSysDescr(): error reading connecting preposition.\n\n");
    Output(outputline);
    return(ERROR);
  }

  if (!ReadSysDescr(&(extended_sys_descr->part2)))
    return(ERROR);

  return(OK);
}


int32_t ReadSysDescr(sys_descr)
 sysDescr *sys_descr;
{
  int i=0;

  if (!GetNextCode32(&(sys_descr->article))) {
    sprintf(outputline, "\n\nReadSysDescr(): read error.\n\n");
    Output(outputline);
    return(ERROR);
  }
  if (!GetNextCode32(&(sys_descr->nr_of_adjectives))) {
    sprintf(outputline, "\n\nreadSysDescr): read error.\n\n");
    Output(outputline);
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_ADJ; i++) {
    if (!GetNextCode32(&(sys_descr->adjectives[i]))) {
      sprintf(outputline, "\n\nReadSysDescr: read error.\n\n");
      Output(outputline);
      return(ERROR);
    }
  }
  if (!GetNextCode32(&(sys_descr->noun))) {
    sprintf(outputline, "\n\nReadSysDescr(): read error.\n\n");
    Output(outputline);
    return(ERROR);
  }

  return(OK);
}


int32_t ReadContData(cont_data)
 contData *cont_data;
{
  int i=0;

  if (!GetNextCode32(&(cont_data->nr_of_objects))) {
    sprintf(outputline, "\n\nReadContData(): read error.\n\n");
    Output(outputline);
    return(ERROR);
  }
  for (i=0; i<MAX_CONTAINED_OBJECTS; i++) {
    if (!GetNextCode32(&(cont_data->object_ids[i]))) {
    sprintf(outputline, "\n\nReadContData(): read error.\n\n");
    Output(outputline);
    return(ERROR);
    }
  }
return(OK);
}

int32_t InMem(id)
 int32_t id;
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

int32_t Load(id)
 int32_t id;
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
        sprintf(outputline, "Error reading verb default code");
        Output(outputline);
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

