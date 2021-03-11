
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
/*                                                                      */
/* <marnix@xvan.nl>                                                     */
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
#include "tables.h"

/***********/
/* globals */
/***********/

locationData *loc_table;
objectData   *obj_table;
flagData     *flag_table;
attrData     *attr_table;
triggerData  *trigg_table;
timerData    *tim_table;
descrData    *descr_table;

int32_t nr_of_locations  = 0;
int32_t nr_of_directions = 0;
int32_t nr_of_objects    = 0;
int32_t nr_of_cflags     = 0;
int32_t nr_of_lflags     = 0;
int32_t nr_of_ctrigs     = 0;
int32_t nr_of_ltrigs     = 0;
int32_t nr_of_cdescrs    = 0;
int32_t nr_of_ldescrs    = 0;
int32_t nr_of_cattrs     = 0;
int32_t nr_of_lattrs     = 0;
int32_t nr_of_timers     = 0;
int32_t nr_of_words      = 0; /* total nr of words (verbs included).  */
int32_t nr_of_verbs      = 0; /* needed to malloc() a verb-directory. */
int32_t nr_of_syn_verbs  = 0; /* Needed to calculate size of verb_dir */
                              /* in Pass2Voc().  (module prsevoc.c)   */

/* directories */

verbDir *verb_dir = NULL;

int32_t  next_verb_id      = FIRST_VERB_ID;
int32_t  next_word_id      = FIRST_WORD_ID;
int32_t  next_location_id  = FIRST_LOCATION_ID;
int32_t  next_object_id    = FIRST_OBJECT_ID;
int32_t  next_cflag_id     = FIRST_COMMON_FLAG_ID;
int32_t  next_lflag_id     = FIRST_LOCAL_FLAG_ID;
int32_t  next_cattr_id     = FIRST_COMMON_ATTR_ID;
int32_t  next_lattr_id     = FIRST_LOCAL_ATTR_ID;
int32_t  next_ctrig_id     = FIRST_COMMON_TRIGGER_ID;
uint32_t next_ltrig_id     = FIRST_LOCAL_TRIGGER_ID;
uint32_t next_timer_id     = FIRST_TIMER_ID;
uint32_t next_cdescr_id    = FIRST_COMMON_DESCR_ID;
uint32_t next_ldescr_id    = FIRST_LOCAL_DESCR_ID;

/* md_start and md_end surround a chain of timerInfo structs */
/* no need for a len_of_md_chain var, since we have a */
/* nr_of_timers var */
timerInfo *md_start = NULL;
timerInfo *md_end   = NULL;

/* vocabulary information is built up in word_list */
/* and later converted to word_table */
wordInfo  *word_list  = NULL;
wordTable *word_table = NULL;

/* All map information is stored together.        */
/* exit_table will be malloc'ed after Pass1Voc(), */
/* when we know the number of directions.         */
int32_t *exit_table;


/*************************/
/* function declarations */
/*************************/

void         PrintWordList(void);
int32_t      SetUpTables(void);
int32_t      InitFileList(fileList**, char*, FILE*);
void         CleanUpFileList(fileList**);
int32_t      InitExits(void);
void         InitOffsets(void);
int32_t      InitVerbDir(void);
void         FreeTables(void);
void         FreeFileList(fileList**);
int32_t      GetNewVerbId(int32_t*);
int32_t      GetNewWordId(int32_t*);
void         CheckLocWildCard(char*, resultStruct*);
void         CheckObjWildCard(char*, resultStruct*);
int32_t      GetLocationId(char*, int32_t*, int32_t, int64_t);
int32_t      GetObjectId(char*, int32_t*, int32_t, int64_t);
resultStruct GetFlagId(char*, int32_t, int32_t*, int32_t, int32_t, int32_t);
resultStruct GetTriggerId(char*, int32_t, int32_t*, int32_t, int32_t);
int32_t      GetTimerId(char*, int32_t*, int32_t);
int32_t      GetDescrId(char*, int32_t*, int32_t, int32_t);
int32_t      GetAttrId(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t); /* 10march2017 */
void         PrintLocationNameForId(int32_t);
void         PrintObjectNameForId(int32_t);
int32_t      ProcWordInfo(wordInfo*);
int32_t      CreateWordTable(void);
int32_t      ProcLocDSys(char*, int32_t);
int32_t      ProcObjDSys(char*, int32_t);
int32_t      ProcCData(int32_t, int32_t);
void         ProcTimerInfo(timerInfo*);
int32_t      ProcCIData(int32_t, int32_t, int32_t);
int32_t      ProcExit(int32_t, int32_t, int32_t);
int32_t      CheckTables(void);
int32_t      IsLocId(int32_t);
int32_t      IsObjId(int32_t);
int32_t      IsCDescrId(int32_t);
int32_t      IsLDescrId(int32_t);
int32_t      IsCFlagId(int32_t);
int32_t      IsLFlagId(int32_t);
int32_t      IsCAttrId(int32_t);
int32_t      IsLAttrId(int32_t);
int32_t      IsCTriggId(int32_t);
int32_t      IsLTriggId(int32_t);
int32_t      IsTimerId(int32_t);
int32_t      IsValue(int32_t);
int32_t      IsVerbId(int32_t);
int32_t      IsSpecId(int32_t);


/************************/
/* function definitions */
/************************/

void PrintWordList()
{
  wordInfo *wp = word_list;
  int32_t i = 0;

  printf("*** Wordlist ***\n");
  printf("word, id, type(s)\n");
  while (wp != NULL) {
    printf("%s %d ", wp->word, wp->id);
    while ((wp->types[i] != NO_TYPE) && (i<MAX_TYPES))
      printf("%d ", wp->types[i++]);
    printf("\n");
    wp = wp->next;
    i = 0;  /* reset for next word in word_list */
  }
  printf("\n");
  return;
}


int32_t SetUpTables()
{
  int32_t i;

  /* creates global tables and arrays                          */
  /* word_table is created after the number of words is known. */
  /* It is defined with value NULL.                            */

  if ((loc_table = (locationData *) malloc(sizeof(locationData))) == NULL) {
    ErrHdr();
    PrintError(1, NULL, "loc_table");
    return(ERROR);
  }
  else {
    strncpy(loc_table->l_word, TERMINATOR, MAX_ID_LENGTH);
    loc_table->l_id = UNKNOWN;
    loc_table->exists = 0;
    loc_table->dir_info.nr_of_dsys = 0;
    for (i=0; i<MAX_DSYS; i++) {
      loc_table->dir_info.descr[i].dynamic                = NULL;
      loc_table->dir_info.descr[i].part1.article          = NO_ID;
      loc_table->dir_info.descr[i].part1.nr_of_adjectives = 0;
      loc_table->dir_info.descr[i].part1.noun             = NO_ID;
      loc_table->dir_info.descr[i].connect_prepos         = NO_ID;
      loc_table->dir_info.descr[i].part2.article          = NO_ID;
      loc_table->dir_info.descr[i].part2.nr_of_adjectives = 0;
      loc_table->dir_info.descr[i].part2.noun             = NO_ID;
    }
    loc_table->dir_info.contained_objs.nr_of_objects = 0;
    loc_table->dir_info.held_by = -1;  /* unused for locations */
    loc_table->dir_info.offset = -1;
    loc_table->next = NULL;
  }
  if ((obj_table = (objectData *)malloc(sizeof(objectData))) == NULL) {
    ErrHdr();
    PrintError(1, NULL, "obj_table");
    return(ERROR);
  }
  else {
    strncpy(obj_table->o_word, TERMINATOR, MAX_ID_LENGTH);
    obj_table->o_id = UNKNOWN;
    obj_table->exists = 0;
    obj_table->dir_info.nr_of_dsys = 0;
    for (i=0; i<MAX_DSYS; i++) {
      obj_table->dir_info.descr[i].dynamic                = NULL;
      obj_table->dir_info.descr[i].part1.article          = NO_ID;
      obj_table->dir_info.descr[i].part1.nr_of_adjectives = 0;
      obj_table->dir_info.descr[i].part1.noun             = NO_ID;
      obj_table->dir_info.descr[i].connect_prepos         = NO_ID;
      obj_table->dir_info.descr[i].part2.article          = NO_ID;
      obj_table->dir_info.descr[i].part2.nr_of_adjectives = 0;
      obj_table->dir_info.descr[i].part2.noun             = NO_ID;
    }
    obj_table->dir_info.contained_objs.nr_of_objects = 0;
    obj_table->dir_info.held_by = NO_ID;
    obj_table->dir_info.offset = -1;
    obj_table->next = NULL;
  }

  if ((flag_table = (flagData *)malloc(sizeof(flagData))) == NULL) {
    ErrHdr();
    PrintError(1, NULL, "flag_table");
    return(ERROR);
  }
  else {
    strncpy(flag_table->f_word, TERMINATOR, MAX_ID_LENGTH);
    flag_table->owner = UNKNOWN;
    flag_table->f_id = UNKNOWN;
    flag_table->exists = 0;
    flag_table->value = -1;
    flag_table->next = NULL;
  }

  if ((attr_table = (attrData *) malloc(sizeof(attrData))) == NULL) {
    ErrHdr();
    PrintError(1, NULL, "attr_table");
    return(ERROR);
  }
  else {
    strncpy(attr_table->r_word, TERMINATOR, MAX_ID_LENGTH);
    attr_table->owner       = UNKNOWN;
    attr_table->r_id        = UNKNOWN;
    attr_table->exists      = 0;
    attr_table->type        = NO_TYPE;
    attr_table->value       = NO_ID;   /* 23jul2015: change this to NONE?? Must check the impact */
    attr_table->value_owner = NO_ID;   /*10march2017 */
    attr_table->next        = NULL;
  }

  if ((trigg_table = (triggerData *)malloc(sizeof(triggerData))) == NULL) {
    ErrHdr();
    PrintError(1, NULL, "trigg_table");
    return(ERROR);
  }
  else {
    strncpy(trigg_table->t_word, TERMINATOR, MAX_ID_LENGTH);
    trigg_table->owner = UNKNOWN;
    trigg_table->t_id = UNKNOWN;
    trigg_table->exists = 0;
    trigg_table->next = NULL;
  }
  if ((tim_table = (timerData *)malloc(sizeof(timerData))) == NULL) {
    ErrHdr();
    PrintError(1, NULL, "tim_table");
    return(ERROR);
  }
  else {
    strncpy(tim_table->m_word, TERMINATOR, MAX_ID_LENGTH);
    tim_table->m_id = UNKNOWN;
    tim_table->exists = 0;
    tim_table->next = NULL;
  }
  if ((descr_table = (descrData *)malloc(sizeof(descrData))) == NULL) {
    ErrHdr();
    PrintError(1, NULL, "descr_table");
    return(ERROR);
  }
  else {
    strncpy(descr_table->d_word, TERMINATOR, MAX_ID_LENGTH);
    descr_table->owner = UNKNOWN;
    descr_table->d_id = UNKNOWN;
    descr_table->exists = 0;
    descr_table->next = NULL;
  }

  return(OK);
}


int32_t InitFileList(file_list, name, fp)
  fileList **file_list;
  char     *name;
  FILE     *fp;
{
  /* now create the first struct and fill with input file data */
  if ( (*file_list = (fileList *) malloc(sizeof(fileList))) == NULL) {
    ErrHdr();
    PrintError(1, NULL, "*file_list");
    return(ERROR);
  }

  strncpy((*file_list)->name, name, MAX_FILENAME_LEN+1);
  (*file_list)->current_line = -1;  /* dummy value, fill in at first $insert */
  (*file_list)->lines        = -1;
  (*file_list)->fp           = fp;
  (*file_list)->previous     = NULL;

  return(OK);
}


int32_t InitExits()
{
  /* This function needs to know the number of exits. Therefore, */
  /* this code cannot be integrated with SetUpTables().          */
  int32_t i = 0;
  int32_t max_locs = LAST_LOCATION_ID-FIRST_LOCATION_ID;

  if ((exit_table = (int32_t *) malloc(sizeof(int32_t)*max_locs*
                                     nr_of_directions)) == NULL) {
    ErrHdr();
    PrintError(1, NULL, "exit_table");
    return(ERROR);
  }
  /* Initialize the exit_table. */
  for (i=0; i<max_locs*nr_of_directions; i++)
    exit_table[i] = NO_ID;

  return(OK);
}

void FreeTables()
{
  locationData *lp  = loc_table;
  objectData   *op  = obj_table;
  flagData     *fp  = flag_table;
  attrData     *rp  = attr_table;
  triggerData  *tp  = trigg_table;
  timerData    *mp  = tim_table;
  descrData    *dp  = descr_table;
  timerInfo    *mip = md_start;
  wordInfo     *wp  = word_list;

  printf("\n\n\n");

  /* some pointers may still be NULL, if an error */
  /* occurred early during ParseStory()           */

  /* free the location table */
  while (lp != NULL) {
    loc_table = loc_table->next;
    free(lp);
    lp = loc_table;
  }
  PrintError(94, &((resultStruct){VALUE, nr_of_locations}), "locations");

  /* free the object table */
  while (op != NULL) {
    obj_table = obj_table->next;
    free(op);
    op = obj_table;
  }
  PrintError(94, &((resultStruct){VALUE, nr_of_objects}), "objects");

  /* free the flag table */
  while (fp != NULL) {
    flag_table = flag_table->next;
    free(fp);
    fp = flag_table;
  }
  PrintError(94, &((resultStruct){VALUE, nr_of_cflags}), "common flags");
  PrintError(94, &((resultStruct){VALUE, nr_of_lflags}), "local flags");

  /* free the attribute table */
  while (rp != NULL) {
    attr_table = attr_table->next;
    free(rp);
    rp = attr_table;
  }
  PrintError(94, &((resultStruct){VALUE, nr_of_cattrs}), "common attributes");
  PrintError(94, &((resultStruct){VALUE, nr_of_lattrs}), "local attributes");

  /* free the trigger table */
  while (tp != NULL) {
    trigg_table = trigg_table->next;
    free(tp);
    tp = trigg_table;
  }
  PrintError(94, &((resultStruct){VALUE, nr_of_ctrigs}), "common triggers");
  PrintError(94, &((resultStruct){VALUE, nr_of_ltrigs}), "local triggers");

  /* free the timer table */
  while (mp != 0) {
    tim_table = tim_table->next;
    free(mp);
    mp = tim_table;
  }
  PrintError(94, &((resultStruct){VALUE, nr_of_timers}), "timers");

  /* free the description table */
  while (dp != NULL) {
    descr_table = descr_table->next;
    free(dp);
    dp = descr_table;
  }
  PrintError(94, &((resultStruct){VALUE, nr_of_cdescrs}), "common descriptions");
  PrintError(94, &((resultStruct){VALUE, nr_of_ldescrs}), "local descriptions");

  /* free the timer info chain */
  while (mip != NULL) {
    md_start = md_start->next;
    free(mip);
    mip = md_start;
  }

  /* free the word_list */
  while (wp != NULL) {
    word_list = word_list->next;
    free(wp);
    wp = word_list;
  }

  free(exit_table);              /* free the exit table     */
  free(word_table);              /* free the word table     */

  free(verb_dir);                /* free the verb directory */

  PrintError(94, &((resultStruct){VALUE, nr_of_words}), "words (including verbs)");
  PrintError(94, &((resultStruct){VALUE, nr_of_verbs}), "verbs");
}


/* 15march2018: function CleanUpFileList() is no longer used because */
/* we clear the file_list structs immediately after we reach EOF in  */
/* the file. We also close the file at that time so we do not have   */
/* lots of files open at teh same time.                              */

void CleanUpFileList(file_list)
 fileList **file_list;
{
  fileList *runner;

  /* This function is used to free all but the    */
  /* first struct in the file list. We need this  */
  /* in case a routine decides to exit and it has */
  /* opened addtl files because of $inserts       */

  /* file_list should not be NULL, but just make sure */
  if (file_list == NULL) {
    printf("Internal error: CleanUpFileList() was called for an empty file list.\n");
    return;
  }
  else
    runner = *file_list;

  while (runner->previous != NULL) {
    *file_list = runner->previous;
    /* close the file */
    fclose(runner->fp);
    free(runner);
    runner = *file_list;
  }
  /* restore source_file */
  source_file = (*file_list)->fp;
  strncpy(current_filename, (*file_list)->name, MAX_FILENAME_LEN);
}


void FreeFileList(file_list)
 fileList **file_list;
{
  /* 15march2018: since we now delete file_list structs as */
  /* soon as we no longer need them, this function will    */
  /* allways be called with only one struct in the list,   */
  /* the first file.                                       */

  fileList *runner;

  if (file_list == NULL)
    return;
  else
    runner = *file_list;

  while (runner != NULL) {
    *file_list = runner->previous;
    total_lines += runner->lines;
    free(runner);
    runner = *file_list;
  }
  free(file_list);
}


void InitOffsets()
{
  /* Store -1 in all dir_data entries. This way, the interpreter */
  /* will know which directories and tables are present.         */

 dir_data.story_info_offset     = -1;
 dir_data.word_data_offset      = -1;
 dir_data.loc_dir_offset        = -1;
 dir_data.obj_dir_offset        = -1;
 dir_data.common_descr_offset   = -1;
 dir_data.common_trigg_offset   = -1;
 dir_data.exit_data_offset      = -1;
 dir_data.flag_data_offset      = -1;
 dir_data.trigg_owners_offset   = -1;
 dir_data.descr_owners_offset   = -1;
 dir_data.counter_data_offset   = -1;
 dir_data.attribute_data_offset = -1;
}


int32_t InitVerbDir()
{
  int i =0;

  /* The size of the verb_dir is not necessarily equal to the */
  /* number of verbs. Any synonym verbs defined in the        */
  /* vocabulary file will not have their own directory entry, */
  /* since there id is equal to that of their synonym(s).     */

  /* 04jan2018: now that we allow verb redefinition, synonyms  */
  /* may get their own entry in the verb_dir whichs means      */
  /* there can be more than nr_of_verbs-nr_of_syn_verb entries */
  /* To be on the safe side, we allocate nr_of_verbs entries   */
  /* in the verb table.                                        */

  int32_t size = nr_of_verbs; /* Make code readable. */

  if ((verb_dir = (verbDir *) malloc(size*sizeof(verbDir))) == NULL) {
    ErrHdr();
    PrintError(47, NULL, "verb_dir");
    return(ERROR);
  }

  /* 04jan2018: because we now allow redefinition of verbs, there   */
  /* is a change that an entry in the verb_dir will not be written  */
  /* (when a verb and all of its synonyms get redefined). This will */
  /* cause an error when the interpreter preloads verbs. Therefore  */
  /* we will now intitialize all entries in the verb_dir with scope */
  /* CURR_LOC_ONLY and offset -1.                                   */
  for (i=0; i<nr_of_verbs; i++) {
    verb_dir[i].scope  = CURR_LOC_ONLY;
    verb_dir[i].offset = -1;
  }
  return(OK);
}


int32_t GetNewVerbId(verb_id)
 int32_t  *verb_id;
{
  /* Returns the next unused verb_id.                  */
  /* No checking is done for existing words.           */
  /* This is done by AddWord()                         */
  /* Words may only be defined up to three times with  */
  /* different types.                                  */
  /* So, there will always be a new id issued here.    */

  if (next_verb_id < LAST_VERB_ID) {
    *verb_id = next_verb_id++;
    return(OK);
  }
  else {
    ErrHdr();
    PrintError(95, &((resultStruct){VALUE,LAST_VERB_ID-FIRST_VERB_ID}), NULL);
    *verb_id = NO_ID;
    return(ERROR);
  }

  return(OK);
}


int32_t GetNewWordId(word_id)
 int32_t *word_id;
{
  /* Returns the next unused word_id                  */
  /* no checking is done for existing words           */
  /* This is done by AddWord() in module parsevoc.c   */
  /* words may only be defined up to three times with */
  /* different types                                  */
  /* so, there will always be a new id issued here    */

  if (next_word_id < LAST_WORD_ID) {
    *word_id = next_word_id++;
    return(OK);
  }
  else {
    ErrHdr();
    PrintError(96, &((resultStruct){VALUE,LAST_WORD_ID-FIRST_WORD_ID}), NULL);
    *word_id = -1;
    return(ERROR);
  }

   return(OK);
}


void CheckLocWildCard(name, result)
 char         *name;
 resultStruct *result;
{
  strlwr(name);

  switch(xvan_language) {
    case ENG:
      ENG_CheckLocWildCard(name, result);
      break;
    case NL:
      NL_CheckLocWildCard(name, result);
      break;
    default:
      /* we should never get here, use English */
      ENG_CheckLocWildCard(name, result);
  }
}


void CheckObjWildCard(name, result)
 char         *name;
 resultStruct *result;
{
/*  strlwr(name);*/

  switch(xvan_language) {
    case ENG:
      ENG_CheckObjWildCard(name, result);
      break;
    case NL:
      NL_CheckObjWildCard(name, result);
      break;
    default:
      /* we should never get here, use English */
      ENG_CheckObjWildCard(name, result);
  }
}


int32_t GetLocationId(id_string, id, parsed, offset)
 char    *id_string; /* location name                        */
 int32_t *id;        /* location id                          */
 int32_t parsed;     /* 1 means location definition          */
                     /* 0 means location reference           */
                     /* This is an INPUT parameter           */
 int64_t offset;     /* location's offset in datafile        */
                     /* used in location directory that will */
                     /* be stored at the end of the file     */
{
  /* returns a location id                   */
  /* loc_table is checked if                 */
  /* id_string has already been stored       */
  /* if so, then the existing id is returned */
  /* if not, id_string is added to loc_table */
  /* and a the next unused id is returned    */
  /* so, in some cases, no changes to        */
  /* loc_table will occur and no new id will */
  /* be issued                               */

  int32_t found    = 0;
  int32_t at_end   = 0;
  int32_t index    = 1;
  int32_t i        = 0;
  locationData *lp = loc_table;
  resultStruct result;

  /* special locations are:        */
  /* make sure l_<name> doesn't exceed MAX_ID_LENGTH */
  /* l_location, return CURR_LOC   */

  CheckLocWildCard(id_string, &result);
  if (result.tag == OK) {
    if (parsed) {
      ErrHdr();
      PrintError(97, NULL, id_string);
      return(ERROR);
    }
    else {
      *id = result.value;
      return(OK);
    }
  }

  while (!found && !at_end) {
    if (strcmp(lp->l_word, id_string) == 0)
      found = 1;
    else if (strcmp(lp->l_word, TERMINATOR) == 0)
      at_end = 1;
    else {
      lp = lp->next;
      index++;
    }
  } /* while */
  if (found) {
    if (parsed) {
      if (lp->exists) {
        ErrHdr();
        PrintError(98, NULL, id_string);
        return(ERROR);
      }
      else {
        lp->exists = 1;
        lp->dir_info.offset = offset;
      }
    }
    *id = lp->l_id;
    return(OK);
  }
  /* not found, use terminator locationData struct */
  if (index < LAST_LOCATION_ID - FIRST_LOCATION_ID) {
    if ((lp->next = (locationData *) malloc(sizeof(locationData))) != NULL) {
      /* fill current terminator struct with location data */
      strncpy(lp->l_word, id_string, MAX_ID_LENGTH);
      lp->l_id   = next_location_id;
      lp->exists = parsed;
      if (parsed)
        lp->dir_info.offset = offset;

      /* fill newly created struct with terminator data */
      strncpy(lp->next->l_word, TERMINATOR, MAX_ID_LENGTH);
      lp->next->l_id = UNKNOWN;
      lp->next->exists = 0;
      lp->next->dir_info.nr_of_dsys = 0;
      for (i=1; i<MAX_DSYS; i++) {
        lp->next->dir_info.descr[i].dynamic                = NULL;
        lp->next->dir_info.descr[i].part1.article          = NO_ID;
        lp->next->dir_info.descr[i].part1.nr_of_adjectives = 0;
        lp->next->dir_info.descr[i].part1.noun             = NO_ID;
        lp->next->dir_info.descr[i].connect_prepos         = NO_ID;
        lp->next->dir_info.descr[i].part2.article          = NO_ID;
        lp->next->dir_info.descr[i].part2.nr_of_adjectives = 0;
        lp->next->dir_info.descr[i].part2.noun             = NO_ID;
      }
      lp->next->dir_info.contained_objs.nr_of_objects = 0;
      lp->next->dir_info.held_by = -1;  /* unused for locations */
      lp->next->dir_info.offset = -1;
      lp->next->next = NULL;

      nr_of_locations++;
      *id = next_location_id++;
      return(OK);
    }
    else {
      ErrHdr();
      PrintError(1, NULL, "GetLocationId()");
      return(ERROR);
    }
  }
  ErrHdr();
  PrintError(99, &((resultStruct){VALUE,LAST_LOCATION_ID-FIRST_LOCATION_ID}), NULL);
  return(ERROR);
}


int32_t GetObjectId(id_string, id, parsed, offset)
 char    *id_string; /* object name */
 int32_t *id;        /* object id */
 int32_t parsed;     /* 1 means object definition */
                     /* 0 means object reference  */
                     /* This is an INPUT parameter  */
 int64_t offset;     /* object's offset in datafile */
                     /* used in object directory that will be */
                     /* stored at the end of the file */
{
  int32_t found    = 0;
  int32_t at_end   = 0;
  int32_t index    = 1;
  int32_t i        = 0;
  objectData   *op = obj_table;
  resultStruct result;

  /* special objects are:          */
  /* make sure o_<name> doesn't exceed MAX_ID_LENGTH */
  /* o_actor,     return ACTOR     */
  /* o_subject,   return SUBJECT   */
  /* o_spec,      return SPECIFIER */

  CheckObjWildCard(id_string, &result);
  if (result.tag == OK) {
    if (parsed) {
      ErrHdr();
      PrintError(97, NULL, id_string);
      return(ERROR);
    }
    else {
      *id = result.value;
      return(OK);
    }
  }

  while (!found && !at_end) {
    if (strcmp(op->o_word, id_string) == 0)
      found = 1;
    else if (strcmp(op->o_word, TERMINATOR) == 0)
      at_end = 1;
    else {
      op = op->next;
      index++;
    }
  } /* while */

  if (found) {
    if (parsed) {
      if (op->exists) {
        ErrHdr();
        PrintError(101, NULL, id_string);
        return(ERROR);
      }
      else {
        op->exists = 1;
        op->dir_info.offset = offset;
        if (offset == -1)
          printf("WTF offset cannot be -1 here\n");
      }
    }
    *id = op->o_id;
    return(OK);
  }

  /* Not found, use terminator struct. */
  if (index < LAST_OBJECT_ID - FIRST_OBJECT_ID) {
    if ((op->next = (objectData *) malloc(sizeof(objectData))) != NULL) {
      /* fill current terminator struct with object data */
      strncpy(op->o_word, id_string, MAX_ID_LENGTH);
      op->o_id = next_object_id;
      op->exists = parsed;
      if (parsed)
        op->dir_info.offset = offset;

      /* fill newly created struct with terminator data */
      strncpy(op->next->o_word, TERMINATOR, MAX_ID_LENGTH);
      op->next->o_id = UNKNOWN;
      op->next->exists = 0;
      op->next->dir_info.nr_of_dsys = 0;
      for (i=0; i<MAX_DSYS; i++) {
        op->next->dir_info.descr[i].dynamic                = NULL;
        op->next->dir_info.descr[i].part1.article          = NO_ID;
        op->next->dir_info.descr[i].part1.nr_of_adjectives = 0;
        op->next->dir_info.descr[i].part1.noun             = NO_ID;
        op->next->dir_info.descr[i].connect_prepos         = NO_ID;
        op->next->dir_info.descr[i].part2.article          = NO_ID;
        op->next->dir_info.descr[i].part2.nr_of_adjectives = 0;
        op->next->dir_info.descr[i].part2.noun             = NO_ID;
      }
      op->next->dir_info.contained_objs.nr_of_objects = 0;
      op->next->dir_info.held_by = NO_ID;
      op->next->dir_info.offset = -1;
      op->next->next = NULL;

      nr_of_objects++;
      *id = next_object_id++;
      return(OK);
    }
    else {
      ErrHdr();
      PrintError(1, NULL, "objectData");
      return(ERROR);
    }
  }
  ErrHdr();
  PrintError(102, &((resultStruct){VALUE,LAST_OBJECT_ID-FIRST_OBJECT_ID}), NULL);
  return(ERROR);
}


resultStruct GetFlagId(id_string, redefine, id, value, owner_id, parsed)
 char    *id_string; /* flag name (different flags in different  */
                     /* locations/objects may have the same name */
 int32_t redefine;   /* input parameter                          */
 int32_t *id;        /* flag id                                  */
 int32_t value;      /* initial flag value (0 or 1)              */
 int32_t owner_id;   /* location or object that owns the flag    */
 int32_t parsed;     /* 1 means flag definition                  */
                     /* 0 means flag reference                   */
                     /* this is an INPUT parameter               */
{
  int32_t found       = 0;
  int32_t at_end      = 0;
  int32_t common      = 0;   /* Denotes whether it is a common flag. */
  resultStruct result = {ERROR, ERROR};

  flagData *fp = flag_table;

  /* Test for mising owner. In case of a flag reference from */
  /* within verb default code, the owner must be explicitly  */
  /* specified. If not, the owner will be set to NO_ID.      */
  if (owner_id == NO_ID) {
    ErrHdr();
    PrintError(145, NULL, id_string);
    result.tag   = ERROR;
    return(result);
  }

  while (!found && !at_end) {
    if (strcmp(fp->f_word, id_string) == 0) {
      /* The name matches. */
      if (fp->owner == COMMON_FLAGS) {
        /* There is a common flag with this name. */
        common = 1;
        *id = fp->f_id;
      }
      if (fp->owner == owner_id || owner_id == FUNCTION)
        /* Name and owner match, or the owner is a function. */
        /* In case the owner is a function, we must not make */
        /* an extra struct for this id and owner.            */
        found = 1;
      else {
        fp = fp->next;
        /* index++; */
      } /* else */
    } /* if */
    else if (strcmp(fp->f_word, TERMINATOR) == 0)
      at_end = 1;
    else {
      fp = fp->next;
      /* index++; */
    } /* else */
  } /* while */

  /* Ok, now we know whether it's a common flag or not. In case it */
  /* is not a common flag, make sure they didn't use a special id  */
  /* (ACTOR, SUBJECT. SPECIFIER. CURR_LOC) or a function with it.  */

  if (!common)
    if (IsSpecId(owner_id) || owner_id == FUNCTION) {
      ErrHdr();
      PrintError(71, NULL, id_string);
      result.tag = ERROR;
      return(result);
    }

  if (found) {
    if (parsed) {
      if (fp->exists) {
        if (fp->redefine == redefine) {
          ErrHdr();
          if (redefine == 0) {
            /* multiple flag definition */
            PrintError(103, NULL, id_string);
          }
          else {
            /* multiple redefinition of flag */
            PrintError(146, NULL, id_string);
          }
          result.tag = ERROR;
          return(result);
        }
        else {
          if (fp->redefine == 0) {
            fp->value    = value;
            fp->redefine = 1;
          }
        }
      }
      else {
        /* There already was a struct because of an earlier */
        /* reference. Now fill in the value.                */
        fp->value  = value;
        fp->exists = 1;
      }
    }
    *id = fp->f_id;
    result.tag = VALUE;
    result.value = fp->redefine == 1 ? REDEFINE_FLAGS : 0;
    return(result);
  }

  /* Not found, but be careful. If this is a reference */
  /* (parsed = 0) to a COMMON flag, we must not create */
  /* a new struct since we already have the flag id.   */

  if (owner_id == COMMON_FLAGS) {
    if (nr_of_cflags >= LAST_COMMON_FLAG_ID - FIRST_COMMON_FLAG_ID+1) {
      ErrHdr();
      PrintError(104, &((resultStruct){VALUE,LAST_COMMON_FLAG_ID-FIRST_COMMON_FLAG_ID+1}), NULL);
      result.tag = ERROR;
      return(result);
    }
    /* As this is a new flag, common hasn't been set yet. */
    common = 1;

    *id = next_cflag_id++;
    nr_of_cflags++;
  }
  else if (!common) {
    /* Local flag. */
    if (nr_of_lflags >= LAST_LOCAL_FLAG_ID - FIRST_LOCAL_FLAG_ID+1) {
      ErrHdr();
      PrintError(105, &((resultStruct){VALUE,LAST_LOCAL_FLAG_ID-FIRST_LOCAL_FLAG_ID+1}), NULL);
      result.tag = ERROR;
      return(result);
    }

    *id = next_lflag_id++;
    nr_of_lflags++;
  }

  /* We must create only structs for:                       */
  /* - local flags: common != 1                             */
  /* - common flags definitions: common == 1 && parsed == 1 */
  /* - common flags overrides: common == 1 && parsed == 1   */
  /* Simplified condition: !common || parsed                */

  if (!common || parsed) {
    /* Create space for new flagData struct. */
    if ((fp->next = (flagData *) malloc(sizeof(flagData))) != NULL) {
      /* fill current terminator struct with flag data */
      strncpy(fp->f_word, id_string, MAX_ID_LENGTH);
      fp->owner    = owner_id;
      fp->f_id     = *id;
      fp->value    = value;
      fp->exists   = parsed;
      fp->common   = common;
      fp->redefine = redefine;

      /* fill newly created struct with terminator data */
      strncpy(fp->next->f_word, TERMINATOR, MAX_ID_LENGTH);
      fp->next->f_id     = NO_ID;
      fp->next->value    = -1;
      fp->next->owner    = NO_ID;
      fp->next->exists   = 0;
      fp->next->common   = 0;
      fp->next->redefine = 0;
      fp->next->next     = NULL;

      result.tag   = VALUE;
      result.value = fp->redefine == 1 ? REDEFINE_FLAGS : 0;
      return(result);
    }
    else {
        ErrHdr();
        PrintError(1, NULL, id_string);
        result.tag = ERROR;
        return(result);
    }
  }
  result.tag = VALUE;
  result.value = fp->redefine == 1 ? REDEFINE_FLAGS : 0;
  return(result);
}


resultStruct GetTriggerId(id_string, redefine, id, owner_id, parsed)
 char *id_string;     /* trigger name (different triggers in different */
                      /* locations/objects may have the same name)     */
 int32_t  *id;        /* trigger id */
 int32_t  redefine;   /* input parameter */
 int32_t  owner_id;   /* location or object that owns the trigger */
 int32_t  parsed;     /* 1 means trigger definition */
                      /* 0 means trigger reference  */
                      /* this is an INPUT parameter */
{
  int32_t found       = 0;
  int32_t at_end      = 0;
  int32_t index       = 0;
  int32_t common      = 0;   /* Denotes whether it's a common trigger. */
  resultStruct result = {ERROR, ERROR};

  triggerData *tp = trigg_table;

  /* Test for missing owner. In case of a trigger reference from */
  /* within verb default code, the owner must be explicitly      */
  /* specified. If not, the owner will be set to NO_ID.          */
    if (owner_id == NO_ID) {
    ErrHdr();
    PrintError(106, NULL, id_string);
    result.tag   = ERROR;
    return(result);
  }

  while (!found && !at_end) {
    if (strcmp(tp->t_word, id_string) == 0) {
      /* The name matches. */
      if (tp->owner == COMMON_TRIGGERS) {
        /* There is a common trigger with this name. */
        common = 1;
        *id = tp->t_id;
      }
      if (tp->owner == owner_id || owner_id == FUNCTION)
        /* Name and owner match, or the owner is a function. */
        /* In case the owner is a function, we must not make */
        /* an extra struct for this id and owner.            */
        found = 1;
      else {
        tp = tp->next;
        index++;
      } /* else */
    } /* if */
    else if (strcmp(tp->t_word, TERMINATOR) == 0)
      at_end = 1;
    else {
      tp = tp->next;
      index++;
    } /* else */
  } /* while */

  /* Ok, now we know whether it's a common trigger or not. In     */
  /* case it is not a common trigger, make sure they didn't use a */
  /* special id (ACTOR, SUBJECT. SPECIFIER. CURR_LOC) or a        */
  /* function with it. */


/* Is the above correct?? If they used a special id, it may also */
/* be a common trigger that is defined later */

  if (!common)
    if (IsSpecId(owner_id) || owner_id == FUNCTION) {
      ErrHdr();
      PrintError(73, NULL, id_string);
      result.tag   = ERROR;
      return(result);
    }

  if (found) {
    if (parsed) {
      if (tp->exists && strcmp(id_string, entrance_trigger) != 0 &&
                        strcmp(id_string, exit_trigger) != 0 &&
                        strcmp(id_string, default_trigger) != 0) {
        /* t_entrance, t_exit and t_default may be redefined,   */
        /* because default code may be added. They must in any  */
        /* case be defined by the compiler because we have      */
        /* special keywords assigned to them.                   */
        /* 17oct2017 now that we have FindCTriggerIds(), the    */
        /* check for predefined triggers is no longer necessary */

        if (tp->redefine == redefine) {
          ErrHdr();
          if (redefine == 0) {
            /* multiple trigger definition */
            PrintError(107, NULL, id_string);
          }
          else {
            /* multiple redefinition of trigger */
            PrintError(140, NULL, id_string);
          }
          result.tag = ERROR;
          return(result);
        }
        else {
          tp->redefine = 1;
        }
      }
      else
        /* There already was a struct because of an earlier */
        /* reference.                                       */
        tp->exists = 1;
    }
    *id = tp->t_id;
    result.tag   = VALUE;
    result.value = tp->redefine == 1 ? REDEFINE_TRIGGERS : 0;
    return(result);
  }

  /* Not found, but be careful. If this is a reference    */
  /* (parsed = 0) to a COMMON trigger, we must not create */
  /* a new struct since we already have the trigger id.   */

  if (owner_id == COMMON_TRIGGERS) {
    if (index >= LAST_COMMON_TRIGGER_ID - FIRST_COMMON_TRIGGER_ID) {
      ErrHdr();
      PrintError(108, &((resultStruct){LAST_COMMON_TRIGGER_ID-FIRST_COMMON_TRIGGER_ID+1}), NULL);
      result.tag = ERROR;
      return(result);
    }
    /* As this is a new trigger, common hasn't been set yet. */
    common = 1;

    *id = next_ctrig_id++;
    nr_of_ctrigs++;
  }
  else if (!common) {
    /* 12oct2017 ?? common will always be false here */
    /* Local trigger. */
    if (index >= LAST_LOCAL_TRIGGER_ID - FIRST_LOCAL_TRIGGER_ID) {
      ErrHdr();
      PrintError(109, &((resultStruct){LAST_LOCAL_TRIGGER_ID-FIRST_LOCAL_TRIGGER_ID+1}), NULL);
      result.tag = ERROR;
      return(result);
    }

    *id = next_ltrig_id++;
    nr_of_ltrigs++;
  }

  /* Test for too many common triggers has already been done.  */
  /* Common trigger code defs within locations and objects do  */
  /* not count as a definition.                                */

  /* We must create only structs for:                         */
  /* - local triggers: common != 1                            */
  /* - common trigger definitions: common == 1 && parsed == 1 */
  /* - common trigger code defs: common == 1 && parsed == 1   */
  /*   (last is to prevent duplicate common trigger defs in   */
  /*    the locations and objects.)                           */
  /* Simplified condition: !common || parsed                  */

  if (!common || parsed) {
    /* Create space for new triggerData struct. */
    if ((tp->next = (triggerData *) malloc(sizeof(triggerData))) != NULL) {
      /* fill current terminator struct with trigger data */
      strncpy(tp->t_word, id_string, MAX_ID_LENGTH);
      tp->owner    = owner_id;
      tp->t_id     = *id;
      tp->exists   = parsed;
      tp->common   = common;
      tp->redefine = redefine;

      /* fill newly created struct with terminator data */
      strncpy(tp->next->t_word, TERMINATOR, MAX_ID_LENGTH);
      tp->next->t_id     = NO_ID;
      tp->next->owner    = NO_ID;
      tp->next->exists   = 0;
      tp->next->common   = 0;     /* Important for StoreTrigOwners() */
      tp->next->redefine = 0;
      tp->next->next     = NULL;

      result.tag   = VALUE;
      result.value = tp->redefine == 1 ? REDEFINE_TRIGGERS : 0;
      return(result);
    }
    else {
        ErrHdr();
        PrintError(1, NULL, id_string);
        result.tag = ERROR;
        return(result);
    }
  }
  result.tag   = VALUE;
  result.value = tp->redefine == 1 ? REDEFINE_TRIGGERS : 0;
  return(result);
}


int32_t GetTimerId(id_string, id, parsed)
 char *id_string; /* timer name */
 int32_t  *id;    /* timer id */
 int32_t  parsed; /* 1 means timer definition  */
                  /* 0 means timer reference   */
                  /* This is an INPUT parameter */
{
  int32_t found  = 0;
  int32_t at_end = 0;
  int32_t index  = 0;
  timerData *mp = tim_table;

  while (!found && !at_end) {
    if (strcmp(mp->m_word, id_string) == 0)
      found = 1;
    else if (strcmp(mp->m_word, TERMINATOR) == 0)
      at_end = 1;
    else {
      mp = mp->next;
      index++;
    }
  } /* while */

  if (found) {
    if (parsed) {
      if (mp->exists) {
        ErrHdr();
        PrintError(110, NULL, id_string);
        return(ERROR);
      }
      else
        mp->exists = 1;
    }
    *id = mp->m_id;
    return(OK);
  }

  if (index < LAST_TIMER_ID - FIRST_TIMER_ID) {
    if ((mp->next = (timerData *) malloc(sizeof(timerData))) != NULL) {
      /* fill current terminator struct with timer data */
      strncpy(mp->m_word, id_string, MAX_ID_LENGTH);
      mp->m_id = next_timer_id;
      mp->exists = parsed;

      /* fill newly created struct with terminator data */
      strncpy(mp->next->m_word, TERMINATOR, MAX_ID_LENGTH);
      mp->next->m_id = UNKNOWN;
      mp->next->exists = 0;
      mp->next->next = NULL;

      nr_of_timers++;
      *id = next_timer_id++;
      return(OK);
    }
    else {
      ErrHdr();
      PrintError(1, NULL, "GetTimerId()");
      return(ERROR);
    }
  }
  ErrHdr();
  PrintError(111, &((resultStruct){LAST_TIMER_ID-FIRST_TIMER_ID+1}), NULL);
  return(ERROR);
}


int32_t GetDescrId(id_string, id, owner_id, parsed)
 char     *id_string; /* Description name (different descriptions */
                      /* in different locations/objects may have  */
                      /* the same name.                           */
 int32_t  *id;        /* Description id. */
 int32_t  owner_id;   /* location or object that owns the description. */
 int32_t  parsed;     /* 1 means description definition */
                      /* 0 means description reference  */
                      /* this is an INPUT parameter */
{
  int32_t found  = 0;
  int32_t at_end = 0;
  int32_t index  = 0;
  int32_t common = 0;   /* Denotes whether it is a common description. */

  descrData *dp = descr_table;

  /* Test for mising owner. In case of a description reference   */
  /* from within verb default code, the owner must be explicitly */
  /* specified. If not, the owner will be set to NO_ID.          */
  if (owner_id == NO_ID) {
    ErrHdr();
    PrintError(112, NULL, id_string);
    return(ERROR);
  }

  while (!found && !at_end) {
    if (strcmp(dp->d_word, id_string) == 0) {
      /* The name matches. */
      if (dp->owner == COMMON_DESCRS) {
        /* There is a common description with this name. */
        common = 1;
        *id = dp->d_id;
      }
      if (dp->owner == owner_id || owner_id == FUNCTION)
        /* Name and owner match, or the owner is a function. */
        /* In case the owner is a function, we must not make */
        /* an extra struct for this id and owner.            */
        found = 1;
      else {
        dp = dp->next;
        index++;
      } /* else */
    } /* if */
    else if (strcmp(dp->d_word, TERMINATOR) == 0)
      at_end = 1;
    else {
      dp = dp->next;
      index++;
    } /* else */
  } /* while */

  /* Ok, now we know whether it's a common description or not. In */
  /* case it is not a common description, make sure they didn't   */
  /* use a special id (ACTOR, SUBJECT. SPECIFIER, CURR_LOC) or a  */
  /* function with it. */

  if (!common)
    if (IsSpecId(owner_id) || owner_id == FUNCTION) {
      ErrHdr();
      PrintError(57, NULL, id_string);
      return(ERROR);
    }

  if (found) {
    if (parsed) {
      if (dp->exists) {
        ErrHdr();
        PrintError(113, NULL, id_string);
        return(ERROR);
      }
      else
        /* There already was a struct because of an earlier */
        /* reference.                                       */
        dp->exists = 1;
    }
    *id = dp->d_id;
    return(OK);
  }

  /* Not found, but be careful. If this is a reference    */
  /* (parsed = 0) to a COMMON description, we must not    */
  /* create a new struct since we already have the        */
  /* description id.                                      */

  if (owner_id == COMMON_DESCRS) {
    if (index >= LAST_COMMON_DESCR_ID - FIRST_COMMON_DESCR_ID) {
      ErrHdr();
      PrintError(114, &((resultStruct){LAST_COMMON_DESCR_ID-FIRST_COMMON_DESCR_ID+1}), NULL);
      return(ERROR);
    }
    /* As this is a new description, common hasn't been set yet. */
    common = 1;

    *id = next_cdescr_id++;
    nr_of_cdescrs++;
  }
  else if (!common) {
    /* Local description. */
    if (index >= LAST_LOCAL_DESCR_ID - FIRST_LOCAL_DESCR_ID) {
      ErrHdr();
      PrintError(115, &((resultStruct){LAST_LOCAL_DESCR_ID-FIRST_LOCAL_DESCR_ID+1}), NULL);
      return(ERROR);
    }

    *id = next_ldescr_id++;
    nr_of_ldescrs++;
  }

  /* Test for too many common descriptions has already been done. */
  /* Common descr code defs within locations and objects do not   */
  /* count as a definition.                                       */

  /* We must create only structs for:                         */
  /* - local descriptions: common != 1                        */
  /* - common descr declarations: common == 1 && parsed == 1  */
  /* - common descr code defs: common == 1 && parsed == 1     */
  /*   (last is to prevent duplicate common descr defs in     */
  /*    the locations and objects.)                           */
  /* Simplified condition: !common || parsed                  */

  if (!common || parsed) {
    /* Create space for new descrData struct. */
    if ((dp->next = (descrData *) malloc(sizeof(descrData))) != NULL) {
      /* fill current terminator struct with description data */
      strncpy(dp->d_word, id_string, MAX_ID_LENGTH);
      dp->owner  = owner_id;
      dp->d_id   = *id;
      dp->exists = parsed;
      dp->common = common;

      /* fill newly created struct with terminator data */
      strncpy(dp->next->d_word, TERMINATOR, MAX_ID_LENGTH);
      dp->next->d_id   = NO_ID;
      dp->next->owner  = NO_ID;
      dp->next->exists = 0;
      dp->next->common = 0;     /* Important for StoreDescrOwners() */
      dp->next->next   = NULL;

      return(OK);
    }
    else {
        ErrHdr();
        PrintError(1, NULL, "GetDescrId()");
        return(ERROR);
    }
  }
  return(OK);
}


int32_t GetAttrId(id_string, redefine, owner_id, id, type, value, value_owner, parsed) /* 10march2017 */

/* July 1st 2015. Function has been overhauled after       */
/* changing the attribute name syntax so that it must be   */
/* prefixed with r_ . Function now looks more like the     */
/* other GetxxxID() functions.                             */
/* Function parameters have been kept the same to avoid    */
/* changes to the code for calling functions for now.      */

 char     *id_string;  /* attribute name (different attributes in       */
                       /* different locations/objects may have the same */
                       /* name.                                         */ /* 10march2017 */
 int32_t  redefine;    /* tells whether a redefine of a global attr     */
 int32_t  owner_id;    /* Location or object that owns the attribute.   */ /* 10march2017 */
 int32_t  *id;         /* attribute id.                                 */
 int32_t  type;        /* Used to distinguish numbers.                  */
 int32_t  value;       /* Initial attribute value.                      */
 int32_t  value_owner; /* in case the value is a description id, the    */ /* 10march2017 */
                       /* location or object that owns the description. */ /* 10march2017 */
 int32_t  parsed;      /* 1 means attribute definition.                 */
                       /* 0 means attribute reference.                  */
                       /* This is an INPUT parameter                    */
{
  int32_t found  = 0;
  int32_t at_end = 0;
  int32_t common = 0;   /* Denotes whether it is a common attribute. */

  attrData *rp = attr_table;

  /* Test for missing owner. In case of a attribute reference */
  /* from within verb default code or common triggers, the   */
  /* owner must be explicitly specified. If not, the owner   */
  /* will be set to NO_ID by calling function.               */

  if (owner_id == NO_ID) {
    ErrHdr();
    PrintError(116, NULL, id_string);
    return(ERROR);
  }

  while (!found && !at_end) {
    if (strcmp(rp->r_word, id_string) == 0) {
      /* The name matches. */
      if (rp->owner == COMMON_ATTRS) {
        /* There is a common attribute with this name. */
        common = 1;
        *id = rp->r_id;
      }
      if (rp->owner == owner_id || owner_id == FUNCTION)
        /* Name and owner match, or the owner is a function. */
        /* In case the owner is a function, we must not make */
        /* an extra struct for this id and owner.            */
        found = 1;
      else {
        rp = rp->next;
        /* index++; */
      } /* else */
    } /* if */
    else if (strcmp(rp->r_word, TERMINATOR) == 0)
      at_end = 1;
    else {
      rp = rp->next;
      /* index++; */
    } /* else */
  } /* while */

  /* Ok, now we know - in case of an already existing attribute - whether  */
  /* it's a common attribute or not. In case it is not a common attribute, */
  /* make sure they didn't use a special id (ACTOR, SUBJECT, SPECIFIER,    */
  /* CURR_LOC) or a function with it.                                      */

  if (!common)
    if (IsSpecId(owner_id) || owner_id == FUNCTION) {
      ErrHdr();
      PrintError(55, NULL, id_string);
      return(ERROR);
    }

  if (found) {
    if (parsed) {
      if (rp->exists) {
        if (rp->redefine == redefine) {
          ErrHdr();
          if (redefine == 0) {
            /* multiple attribute definition */
            PrintError(117, NULL, id_string);
          }
          else {
            /* multiple redefinition of attribute */
            PrintError(147, NULL, id_string);
            return(ERROR);
          }
        }
        else {
          if (rp->redefine == 0) {
            rp->value       = value;
            rp->type        = type;
            rp->value_owner = value_owner;
            rp->redefine    = 1;
          }
        }
      }
      else {
        /* There already was a struct because of an earlier */
        /* reference. Now fill in the value.                */
        rp->value       = value;
        rp->type        = type;    /* line was missing in old version */
        rp->value_owner = value_owner; /* 10march2017 */
        rp->exists      = 1;
      }
    }
    *id = rp->r_id;
    return(OK);
  }

  /* Not found, but be careful. If this is a reference     */
  /* (parsed = 0) to a COMMON attribute, we must not create */
  /* a new struct since we already have the attribute id.   */

  if (owner_id == COMMON_ATTRS) {
    if (nr_of_cattrs >= LAST_COMMON_ATTR_ID - FIRST_COMMON_ATTR_ID+1) {
      ErrHdr();
      PrintError(118, &((resultStruct){LAST_COMMON_ATTR_ID-FIRST_COMMON_ATTR_ID+1}), NULL);
      return(ERROR);
    }

    /* As this is a new attribute, common hasn't been set yet. */
    common = 1;  /* 01 jul 2015, why not call the function with common=1? */

    *id = next_cattr_id++;
    nr_of_cattrs++;
  }
  else if (!common) {
    /* Local attribute. */
    if (nr_of_lattrs >= LAST_LOCAL_ATTR_ID - FIRST_LOCAL_ATTR_ID+1) {
      ErrHdr();
      PrintError(119, &((resultStruct){LAST_LOCAL_ATTR_ID-FIRST_LOCAL_ATTR_ID+1}), NULL);
      return(ERROR);
    }
    *id = next_lattr_id++;
    nr_of_lattrs++;
  }

  /* We must create only structs for:                           */
  /* - local attributes: common != 1                            */
  /* - common attribute definitions: common == 1 && parsed == 1 */
  /* - common attribute overrides: common == 1 && parsed == 1   */
  /* Simplified condition: !common || parsed                    */

  if (!common || parsed) {
    /* Create space for new attrData struct. */
    if ((rp->next = (attrData *) malloc(sizeof(attrData))) != NULL) {
      /* fill current terminator struct with attribute data */
      strncpy(rp->r_word, id_string, MAX_ID_LENGTH);
      rp->owner       = owner_id;
      rp->r_id        = *id;
      rp->type        = type;
      rp->value       = value;
      rp->value_owner = value_owner; /* 10march2017 */
      rp->exists      = parsed;
      rp->common      = common;
      rp->redefine    = redefine;

      /* fill newly created struct with terminator data */
      strncpy(rp->next->r_word, TERMINATOR, MAX_ID_LENGTH);
      rp->next->r_id        = NO_ID;
      rp->next->type        = NO_TYPE;
      rp->next->value       = NO_ID;
      rp->next->owner       = NO_ID;
      rp->next->value_owner = NO_ID; /* 10march2017 */
      rp->next->exists      = 0;
      rp->next->common      = 0;
      rp->next->redefine    = 0;
      rp->next->next        = NULL;
      return(OK);
    }
    else {
        ErrHdr();
        PrintError(1, NULL, "GetAttrId()");
        return(ERROR);
    }
  }
  return(OK);
}


int32_t ProcWordInfo(info)
 wordInfo *info;
{
  /* Inserts a wordInfo struct into the word_list . If the    */
  /* word already exists, but with a different type, the type */
  /* is added to the existing struct.                         */
  /* The info parameter will always only have type[0] filled  */

  /* info must be malloc-ed */

  wordInfo *wp1;
  wordInfo *wp2;
  int32_t  result;
  int32_t  i = 0;

  if (word_list == NULL) {
    /*first call of ProcWordInfo() */
    word_list = info;

    /* check for verb */
    if (info->types[0] == VERB)
      nr_of_verbs++;
    nr_of_words++;     /* the first word */
    return(OK);
  }

  wp1 = NULL;
  wp2 = word_list;
                              /*   A-->B-->F-->K  */
                              /*       ^   ^      */
                              /*       |   |      */
                              /*      wp1 wp2     */
  while (wp2 != NULL) {
    result = strcmp(info->word, wp2->word);

    if (result<0) {
      /* new word is alphabetically before wp2->word, but */
      /* after wp1->word */

      info->next = wp2;    /* add before wp2        */
      if (wp1 == NULL)
        word_list = info;  /* before start of chain */
      else
        wp1->next = info;

      /* check for verb */
      if (info->types[0] == VERB)
        nr_of_verbs++;
      nr_of_words++;       /* one more word         */
      return(OK);
    }

    if (result==0) {
      /* same word, check for different types              */
      /* info struct of word to be added only has one type */

      while ((i<MAX_TYPES) && (info->types[0] != wp2->types[i])
              && (wp2->types[i] != NO_TYPE))
        i++;
      if (i == MAX_TYPES) {
        ErrHdr();
        PrintError(120, NULL, info->word);
        return(ERROR);
      }
      if (info->types[0] == wp2->types[i]) {
        /* Same word, same type. Must check for verb redefinition.    */
        /* All verb names are processed first, so if info is a verb,  */
        /* then wp2 will only have one type.                          */
        /* Possibilities are:                                         */
        /* 1. info = verb and wp2 = verb -> error                     */
        /* 2. info = verb and wp2->redefined = true -> ok             */
        /* 3. info = redefined verb and wp2 = verb -> ok              */
        /* 4. info = redefined verb and wp2 = redefined verb -> error */
        /* (for words other than verbs, redefine will always be 0.    */

        if (info->redefine == wp2->redefine) {
          /* multiple defined word */
          ErrHdr();
          if (info->redefine == 1) {
            /* multiple redefine */
            PrintError(138, NULL, info->word);
          }
          else {
            PrintError(121, NULL, info->word);
          }
          return(ERROR);
        }
        else {
          /* different redefine values for info and wp2 */
          if (info->redefine == 1) {
            strncpy(wp2->print_word, info->print_word, MAX_WORD_LEN);
            wp2->redefine = 1;
            wp2->id       = info->id;
          }
          return(OK);
        }
      }
      /* ok, same word, not a redefined verb, different type */
      /* add new type to existing wordInfo struct            */
      wp2->types[i] = info->types[0];

      /* set info->id to id of the existing wordInfo struct, */
      /* so any plural definitions will point to the right   */
      /* single                                              */
      info->id = wp2->id;

      /* we will not use the info struct */
      free(info);

      /* don't increase nr_of_words                  */
      /* words with multiple types count as one word */
      return(OK);
    }
    else {
      /* result > 0 */
      /* new word is alphabetically after wp2->word */
      /* move wp1 and wp2                           */
      wp1 = wp2;
      wp2 = wp2->next;
    }
  } /* while */

  /* wp2 is NULL, end of chain */
  wp1->next = info;

  /* check for verb */
  if (info->types[0] == VERB)
    nr_of_verbs++;
  nr_of_words++;      /* one more word */
  return(OK);
}


int32_t CreateWordTable()
{
  /* During Pass1(), a linked list of wordInfo structs is    */
  /* created, which is alphabetically ordered. After Pass1() */
  /* and before Pass2(), CreateWordTable is called. This     */
  /* function converts the linked list to an array of type   */
  /* wordTable (which is the same type as wordInfo, but for  */
  /* the 'next' pointer). This array is used during Pass2(), */
  /* since an array allows a binary search algorithm to be   */
  /* used. In our case, the algorithm is implemented in the  */
  /* function ScanTable().                                   */

  /* word_list is not free'ed here. It may be needed else-   */
  /* where. It is free'ed, like all tables, in FreeTables(). */

  wordInfo *wp = word_list;
  int32_t      i = 0;
  int32_t      j = 0;

  if ((word_table = (wordTable *)
                     malloc(nr_of_words*sizeof(wordTable))) == NULL) {
    ErrHdr();
    PrintError(1, NULL, "CreateWordTable()");
    return(ERROR);
  }

  /* now convert word_list to word_table */
  while (wp != NULL) {
    strncpy(word_table[i].word, wp->word, MAX_WORD_LEN);
    strncpy(word_table[i].print_word, wp->print_word, MAX_WORD_LEN);
    word_table[i].id        = wp->id;
    word_table[i].single_id = wp->single_id;
    for (j=0; j<MAX_TYPES; j++) {
      word_table[i].types[j] = wp->types[j];
    }
    word_table[i].redefine = wp->redefine;
    i++;
    wp = wp->next;
  }

  return(OK);
}


int32_t ProcLocDSys(description, owner_id)
 char *description;
 int32_t  owner_id;
{
  /* description contains a description of location 'owner_id'.    */
  /* Syntax of description is the same as actor/subject/specifier. */
  /* description is parsed and added to loc_table, for later use   */
  /* by compiler or interpreter.                                   */

  locationData     *lp = loc_table;
  int32_t          i   = 0;
  extendedSysDescr descr;   /* local var; don't build refs to it       */
  int32_t          dsys;    /* tells which dsys entry in array to fill */

  /* Set descr struct to default values */
  descr.dynamic                = NULL;
  descr.part1.article          = NO_ID;
  descr.part1.nr_of_adjectives = 0;
  descr.part1.noun             = NO_ID;
  descr.connect_prepos         = NO_ID;
  descr.part2.article          = NO_ID;
  descr.part2.nr_of_adjectives = 0;
  descr.part2.noun             = NO_ID;

  if (ParseDSys(description, &descr) == ERROR)
    return(ERROR);

  /* ParseDSys() returned either UNKNOWN_WORD or OK   */
  /* In each case we must copy descr to the loc_table */   /* WHY????? */
  /* now find this location's struct in loc_table     */

  while (lp->l_id != owner_id && (strcmp(lp->l_word, TERMINATOR) != 0))
    lp = lp->next;

  if (strcmp(lp->l_word, TERMINATOR) == 0) {
    ErrHdr();
    PrintError(122, NULL, lp->l_word);
    return(ERROR);
  }

  /* We found the correct struct. */
  /* Copy descr in loc_table.     */
  /* ParseDSys must check for exceeding of MAX_DESCR_ADJ  */


  /* check if we have room for one more d_sys description */
  if (lp->dir_info.nr_of_dsys >= MAX_DSYS) {
    ErrHdr();
    PrintError(123, NULL, lp->l_word);
    return(ERROR);
  }

  /* ok, we have room for another d_sys */
  dsys = lp->dir_info.nr_of_dsys++;
  lp->dir_info.descr[dsys].dynamic                = descr.dynamic;
  lp->dir_info.descr[dsys].part1.article          = descr.part1.article;
  lp->dir_info.descr[dsys].part1.nr_of_adjectives = descr.part1.nr_of_adjectives;

  for (i=0; i<descr.part1.nr_of_adjectives; i++)
    lp->dir_info.descr[dsys].part1.adjectives[i] = descr.part1.adjectives[i];

  lp->dir_info.descr[dsys].part1.noun = descr.part1.noun;

  lp->dir_info.descr[dsys].connect_prepos = descr.connect_prepos;

  lp->dir_info.descr[dsys].part2.article = descr.part2.article;
  lp->dir_info.descr[dsys].part2.nr_of_adjectives = descr.part2.nr_of_adjectives;

  for (i=0; i<descr.part2.nr_of_adjectives; i++)
    lp->dir_info.descr[dsys].part2.adjectives[i] = descr.part2.adjectives[i];

  lp->dir_info.descr[dsys].part2.noun = descr.part2.noun;

  return(OK);
}


int32_t ProcObjDSys(description, owner_id)
 char *description;
 int32_t  owner_id;
{
  /* description contains a description of the object  'owner_id'. */
  /* Syntax of description is the same as actor/subject/specifier. */
  /* description is parsed and added to  obj_table, for later use  */
  /* by compiler or interpreter.                                   */

  objectData *op = obj_table;
  int32_t            i   = 0;
  extendedSysDescr   descr;   /* local var; don't build refs to it         */
  int32_t            dsys;    /* tells which dsys entry in array to fill   */

  /* Set descr struct to default values */
  descr.dynamic                = NULL;
  descr.part1.article          = NO_ID;
  descr.part1.nr_of_adjectives = 0;
  descr.part1.noun             = NO_ID;
  descr.connect_prepos         = NO_ID;
  descr.part2.article          = NO_ID;
  descr.part2.nr_of_adjectives = 0;
  descr.part2.noun             = NO_ID;

  if (ParseDSys(description, &descr) == ERROR)
    return(ERROR);

  /* ParseDSys() returned either UNKNOWN_WORD or OK   */
  /* In each case we must copy descr to the loc_table */

  /* now find this object's struct in obj_table */
  while (op->o_id != owner_id && (strcmp(op->o_word, TERMINATOR) != 0))
    op = op->next;

  if (strcmp(op->o_word, TERMINATOR) == 0) {
    ErrHdr();
    PrintError(124, NULL, op->o_word);
    return(ERROR);
  }

  /* We found the correct struct */
  /* Copy descr in obj_table     */
  /* ParseDSys must check for exceeding of MAX_DESCR_ADJ */

  /* check if we have room for one more d_sys description */
  if (op->dir_info.nr_of_dsys >= MAX_DSYS) {
    ErrHdr();
    PrintError(125, NULL, op->o_word);
    return(ERROR);
  }

  /* ok, we have room for another d_sys */
  dsys = op->dir_info.nr_of_dsys++;
  op->dir_info.descr[dsys].dynamic                = descr.dynamic;
  op->dir_info.descr[dsys].part1.article          = descr.part1.article;
  op->dir_info.descr[dsys].part1.nr_of_adjectives = descr.part1.nr_of_adjectives;

  for (i=0; i<descr.part1.nr_of_adjectives; i++)
    op->dir_info.descr[dsys].part1.adjectives[i] = descr.part1.adjectives[i];

  op->dir_info.descr[dsys].part1.noun = descr.part1.noun;

  op->dir_info.descr[dsys].connect_prepos = descr.connect_prepos;

  op->dir_info.descr[dsys].part2.article = descr.part2.article;
  op->dir_info.descr[dsys].part2.nr_of_adjectives = descr.part2.nr_of_adjectives;

  for (i=0; i<descr.part2.nr_of_adjectives; i++)
    op->dir_info.descr[dsys].part2.adjectives[i] = descr.part2.adjectives[i];

  op->dir_info.descr[dsys].part2.noun = descr.part2.noun;

  return(OK);
}


int32_t ProcCData(contains, contained)
 int32_t contains;   /* Containing object or location. */
 int32_t contained;  /* Contained object.              */
{
  locationData *lp = loc_table;
  objectData   *op = obj_table;

  contData     *cp = NULL;  /* For easy to read code. */

  if (IsLocId(contains)) {
    /* Find the right struct in loc_table */
    while (lp->l_id != contains)
      lp = lp->next;

    cp = &(lp->dir_info.contained_objs);

    /* OK, found it; now add contained object to loc_table struct. */
    /* Ofcourse MAX_CONTAINED_OBJECTS may not be exceeded.         */
    /* Indexing is tricky here. Use old nr_of_objects to index cp. */
    /* I.e. if there are already two contained objects, the next   */
    /* one is inserted at position 2.                              */

    if (cp->nr_of_objects < MAX_CONTAINED_OBJECTS)
      cp->object_ids[cp->nr_of_objects++] = contained;
    else {
      ErrHdr();
      PrintError(126, &((resultStruct){VALUE,MAX_CONTAINED_OBJECTS}), NULL);
      return(ERROR);
    }
  }
  else if (IsObjId(contains)) {
    /* Find the right struct in obj_table */
    while (op->o_id != contains)
      op = op->next;

    cp = &(op->dir_info.contained_objs);

    /* OK, found it; now add contained object to obj_table struct. */
    /* Ofcourse MAX_CONTAINED_OBJECTS may not be exceeded.         */
    if (cp->nr_of_objects < MAX_CONTAINED_OBJECTS-1)
      cp->object_ids[cp->nr_of_objects++] = contained;
    else {
      ErrHdr();
      PrintError(126, &((resultStruct){VALUE,MAX_CONTAINED_OBJECTS}), NULL);
      return(ERROR);
    }
  }
  else {
    ErrHdr();
    PrintError(127, NULL, "ProcCData()");
    return(ERROR);
  }

  return(OK);
}


void ProcTimerInfo(info)
 timerInfo *info;
{
  info->next = NULL;  /* just to make sure */

  if (md_start == NULL) {
    /* first time this function is called */
    md_start = info;
    md_end   = info;
  }
  else {
    /* add the info struct to the end of the chain */
    md_end->next = info;
    md_end = info;
  }
}


int32_t ProcCIData(id, contained_in, preposition)
 int32_t id;            /* contained object                    */
 int32_t contained_in;  /* containing object/location          */
 int32_t preposition;   /* position of id (e.g. in, on, under) */
{
  /* id is contained in contained_in */

  objectData  *op = obj_table;
  int32_t         dummy_id;

  if (IsObjId(id)) {
    /* Find the right struct in obj_table */
    while (op->o_id != id)
      op = op->next;

    /* OK, found it; store containment info in obj_table struct */
    op->dir_info.held_by     = contained_in;

    /* set value of r_preposition */
    /* type is WORD_ID and not PREPOSITIONS, to allow the */
    /* attribute to be handled as a word.                 */
    if (!GetAttrId(TranslateKeyword("preposition"), 0, id, &dummy_id, WORD_ID, preposition, NO_ID, 1)) /* 10march2017 */
      /* 0 means not a redefine            */
      /* 1 means attribute is defined here */
      return(ERROR);

  }
  else {
    ErrHdr();
    PrintError(127, NULL, "ProcCIData()");
    return(ERROR);
  }

  return(OK);
}


int32_t ProcExit(from_loc, direction, to_loc)
 int32_t from_loc;   /* current location         */
 int32_t direction;  /* direction in which to go */
 int32_t to_loc;     /* new location             */
{
  if (exit_table[(from_loc-FIRST_LOCATION_ID)*nr_of_directions
                                             +direction] != NO_ID) {
    ErrHdr();
    PrintError(128, NULL, NULL);
    return(ERROR);
  }
  exit_table[(from_loc-FIRST_LOCATION_ID)*nr_of_directions
                                                +direction] = to_loc;
  return(OK);
}


int32_t CheckTables()
{
  locationData *lp;
  objectData   *op;
  flagData     *fp;
  attrData     *rp;
  triggerData  *tp;
  timerData    *mp;
  descrData    *dp;

  int32_t status = OK; /* no undefined identifiers found yet */

  /* check locations */
  lp = loc_table;
  while (strcmp(lp->l_word, TERMINATOR) != 0) {
    if (!(lp->exists)) {
      printf("\n\n l_%s:", lp->l_word);
      PrintError(90, NULL, "location");
      status = ERROR;
    }
    lp = lp->next;
  }

  /* check objects */
  op = obj_table;
  while (strcmp(op->o_word, TERMINATOR) != 0) {
    if (!(op->exists)) {
      printf("\n\n o_%s:", op->o_word);
      PrintError(90, NULL, "object");
      status = ERROR;
    }
    op = op->next;
  }

  /* check flags */
  /* Caution: global flag overrides will not have a        */
  /* fp->exists equal to 1, but global flags always exist. */
  fp = flag_table;
  while (strcmp(fp->f_word, TERMINATOR) != 0) {
    if (!(fp->common) && !(fp->exists)) {
      if (IsLocId(fp->owner)) {
        printf("\nLocation ");
        PrintLocationNameForId(fp->owner);
        PrintError(129, NULL, fp->f_word);
      }
      else {
        printf("\nObject ");
        PrintObjectNameForId(fp->owner);
        PrintError(129, NULL, fp->f_word);
      }
      status = ERROR;
    }
    fp = fp->next;
  }

  /* Check attributes. */
  /* Caution: global attribute overrides will not have a        */
  /* rp->exists equal to 1, but global attributes always exist. */
  rp = attr_table;
  while (strcmp(rp->r_word, TERMINATOR) != 0) {
    if (!(rp->common) && !(rp->exists)) {
      if (IsLocId(rp->owner)) {
        printf("\nLocation ");
        PrintLocationNameForId(rp->owner);
        PrintError(130, NULL, rp->r_word);
      }
      else {
        printf("\nObject ");
        PrintObjectNameForId(rp->owner);
        PrintError(130, NULL, rp->r_word);
      }
      status = ERROR;
    }
    rp = rp->next;
  }

  /* check triggers */
  tp = trigg_table;
  while (strcmp(tp->t_word, TERMINATOR) != 0) {
    if (!(tp->common) && !(tp->exists)) {
      if (IsLocId(tp->owner)) {
        printf("\nLocation ");
        PrintLocationNameForId(tp->owner);
        PrintError(131, NULL, tp->t_word);
      }
      else {
        printf("\nObject ");
        PrintObjectNameForId(tp->owner);
        PrintError(131, NULL, tp->t_word);
      }
      status = ERROR;
    }
    tp = tp->next;
  }

  /* check timers */
  mp = tim_table;
  while (strcmp(mp->m_word, TERMINATOR) != 0) {
    if (!(mp->exists)) {
      PrintError(132, NULL, mp->m_word);
      status = ERROR;
    }
    mp = mp->next;
  }

  /* check descriptions */
  dp = descr_table;
  while (strcmp(dp->d_word, TERMINATOR) != 0) {
    if (!(dp->common) && !(dp->exists)) {
      if (IsLocId(dp->owner)) {
        printf("\nLocation ");
        PrintLocationNameForId(dp->owner);
        PrintError(133, NULL, dp->d_word);
      }
      else {
        printf("\nObject ");
        PrintObjectNameForId(dp->owner);
        PrintError(133, NULL, dp->d_word);
      }
      status = ERROR;
    }
    dp = dp->next;
  }
  return(status);
}


void PrintLocationNameForId(id)
 int32_t id;
{
  /* searches the name for a location id                  */
  /* used by CheckTables() to print better error messages */

  int          found  = 0;
  int          at_end = 0;
  locationData *lp    = loc_table;

  if (IsLocId(id)) {
    while (!found && !at_end) {
      if (lp->l_id == id) {
        /* found it */
        found = 1;
        printf("%s", lp->l_word);
      }
      else {
        if ((lp = lp->next) == NULL) {
          PrintError(134, &((resultStruct){VALUE,id}), NULL);
          at_end = 1;
        }
      }
    }
  }
  else {
    PrintError(135, &((resultStruct){VALUE,id}), NULL);
  }
  return;
}


void PrintObjectNameForId(id)
 int32_t id;
{
  /* searches the name for an object id                   */
  /* used by CheckTables() to print better error messages */

  int          found  = 0;
  int          at_end = 0;
  objectData *op      = obj_table;

  if (IsObjId(id)) {
    while (!found && !at_end) {
      if (op->o_id == id) {
        /* found it */
        found = 1;
        printf("%s", op->o_word);
      }
      else {
        if ((op = op->next) == NULL) {
          PrintError(136, &((resultStruct){VALUE,id}), NULL);
          at_end = 1;
        }
      }
    }
  }
  else {
    PrintError(137, &((resultStruct){VALUE,id}), NULL);
  }
  return;
}


int32_t IsLocId(id)
 int32_t id;
{
  switch (id) {
  /* test for special location ids */
    case CURR_LOC: ;
    case THIS:
    case NONE:
      /* THIS and NONE are either location or object ids. */
      return(OK);
    default:
      if ((id>=FIRST_LOCATION_ID) && (id<=LAST_LOCATION_ID))
        return(OK);
      break;
  } /* switch */

  return(ERROR);
}


int32_t IsObjId(id)
 int32_t id;
{
  switch (id) {
    /* test for special object ids */
    case ACTOR    : ;
    case SUBJECT  : ;
    case SPECIFIER: ;
    case THIS:
    case NONE:
      /* THIS and NONE are either location or object ids. */
      return(OK);
    default:         /* test for normal object ids */
      if ((id>=FIRST_OBJECT_ID) && (id<=LAST_OBJECT_ID))
        return(OK);
      break;
  }  /* switch */
  return(ERROR);
}


int32_t IsCDescrId(id)
 int32_t id;
{
  if ((id>=FIRST_COMMON_DESCR_ID) && (id<=LAST_COMMON_DESCR_ID))
    return(OK);
  return(ERROR);
}


int32_t IsLDescrId(id)
 int32_t id;
{
  if ((id>=FIRST_LOCAL_DESCR_ID) && (id<=LAST_LOCAL_DESCR_ID))
    return(OK);
  return(ERROR);
}


int32_t IsCFlagId(id)
 int32_t id;
{
  if ((id>=FIRST_COMMON_FLAG_ID) && (id<=LAST_COMMON_FLAG_ID))
    return(OK);
  return(ERROR);
}


int32_t IsLFlagId(id)
 int32_t id;
{
  if ((id>=FIRST_LOCAL_FLAG_ID) && (id<=LAST_LOCAL_FLAG_ID))
    return(OK);
  return(ERROR);
}


int32_t IsCAttrId(id)
 int32_t id;
{
  if ((id>=FIRST_COMMON_ATTR_ID) && (id<=LAST_COMMON_ATTR_ID))
    return(OK);
  return(ERROR);
}


int32_t IsLAttrId(id)
 int32_t id;
{
  if ((id>=FIRST_LOCAL_ATTR_ID) && (id<=LAST_LOCAL_ATTR_ID))
    return(OK);
  return(ERROR);
}


int32_t IsCTriggId(id)
 int32_t id;
{
  if ((id>=FIRST_COMMON_TRIGGER_ID) && (id<=LAST_COMMON_TRIGGER_ID))
    return(OK);
  return(ERROR);
}


int32_t IsLTriggId(id)
 int32_t id;
{
  if ((id>=FIRST_LOCAL_TRIGGER_ID) && (id<=LAST_LOCAL_TRIGGER_ID))
    return(OK);
  return(ERROR);
}


int32_t IsTimerId(id)
 int32_t id;
{
  if ((id>=FIRST_TIMER_ID) && (id<=LAST_TIMER_ID))
    return(OK);
  return(ERROR);
}


int32_t IsVerbId(id)
 int32_t id;
{
  /* next 2 lines added on oct 21st 2015 */
  if (id == ACTION)
    return(OK);

  if ((id>=FIRST_VERB_ID) && (id<=LAST_VERB_ID))
    return(OK);
  return(ERROR);
}


int32_t IsWordId(id)
 int32_t id;
{
  switch (id) {
    case PREPOS:  ;
    case ACTION:
      return(OK);
    default:
      if ((id>=FIRST_WORD_ID) && (id<=LAST_WORD_ID))
        return(OK);

      /* If it's a verb, it is also a word. */
      return(IsVerbId(id));
  } /* switch */
}


int32_t IsSpecId(id)
 int32_t id;
{
  switch (id) {
    case THIS:       ;
    case NONE:       ;
    case ACTOR:      ;
    case SUBJECT:    ;
    case SPECIFIER:  ;
    case CURR_LOC:
      return(OK);
    default:
      return(ERROR);
  }
}
