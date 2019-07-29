
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
#include <stdlib.h>  /* malloc(), realloc(), free() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "debug.h"


/*************************/
/* Function declarations */
/*************************/

void    PrintFileList(fileList**);
void    PrintStoryInfo(storyInfo*);
int32_t Test4SpecId(int32_t);
void    PrintVerbDir(void);
void    PrintActionRec(actionRec*);
void    PrintWTable(void);

int32_t CreateLocDebugInfo(debugInfo**);
int32_t CreateObjDebugInfo(debugInfo**);
int32_t CreateAttrDebugInfo(debugInfo**, debugInfo**);
int32_t CreateFlagDebugInfo(debugInfo**, debugInfo**);
int32_t CreateTimerDebugInfo(debugInfo**);
int32_t CreateTriggerDebugInfo(debugInfo**, debugInfo**);

/************************/
/* Function definitions */
/************************/


void PrintFileList(fileList **file_list)
{
  fileList *runner = *file_list;
  printf("\n\n******** Filelist ********\n\n");

  while (runner != NULL) {
    printf("-------------------------------------\n");
    printf("Filename    : %s\n", runner->name);
    printf("current line: %d\n", runner->current_line);
    printf("Nr of lines : %d\n", runner->lines);
    printf("File pointer: %ld\n", (long) runner->fp);
    printf("Previous    : %s\n", (runner->previous)->name);
    runner = runner->previous;
    printf("-------------------------------------\n");
  }
    printf("\n\n**************************\n\n");
}

void PrintStoryInfo(storyInfo *info)
{
  printf("\n\n******** Meta data ********\n\n");
  printf("Title: %s\n", info->title);
  printf("Author: %s\n", info->author);
  printf("Organization: %s\n", info->organization);
  printf("Cover text: %s\n", info->cover_text);
  printf("Credits: %s\n", info->credits);
  printf("Version: %s\n", info->version);
  printf("Android market: %s\n", info->android_mkt);
  printf("ios market: %s\n", info->ios_mkt);
  printf("Back image: %s\n", info->backimage);
  printf("Effect: %s\n", info->effect);
  printf("Sidebar: %d\n", info->ui_sidebar);
  printf("Textinput: %d\n", info->ui_textinput);
  printf("Compass: %d\n", info->ui_compass);
  printf("Autolink: %d\n", info->autolink);
  printf("Compiler version: %s\n", info->compiler_version);
  printf("XVAN language: %d\n", info->xvan_language);
  printf("Story language: %d\n", info->story_language);

  printf("\n********************************\n\n");
}

int32_t Test4SpecId(int32_t id)
{
  /* This function tests whether id has one of the following */
  /* values: NO_ID, CURR_LOC, SUBJECT, ACTOR, SPECIFIER, and */
  /* prints a corresponding message.                         */

  switch (id) {
    case NO_ID:
      printf("<none>");
      return(OK);
    case CURR_LOC:
      printf("<the current location>\n");
      return(OK);
    case SUBJECT:
      printf("<the current subject>\n");
      return(OK);
    case ACTOR:
      printf("<the current actor>\n");
      return(OK);
    case SPECIFIER:
      printf("<the current specifier>\n");
      return(OK);
    default:
      /* Not a special id. */
      return(ERROR);
  } /* switch */

  return(OK);
}


void PrintVerbDir(void)
{
  int i  = 0;

  printf("\n\n******** VERB DIRECTORY ********\n\n");

  for(i=0; i<nr_of_verbs; i++) {
    printf("\nverb id: %d (%d)\n", i+FIRST_VERB_ID, i);
    printf("scope: %d\n", verb_dir[i].scope);
    printf("offset: %ld\n", verb_dir[i].offset);
  }
  printf("\n\n********************************\n\n");
}

void PrintActionRec(actionRec *action_rec)
{
  int32_t      i = 0;

  printf("\n**** Action Record ****\n");

  printf("\nAction 1: ");
  if (action_rec->action1 == NO_ID)
    printf("<none>\n");
  else
    printf("%d", action_rec->action1);
  printf("\n");

  printf("\nAction 2: ");
  if (action_rec->action2 == NO_ID)
    printf("<none>\n");
  else
    printf("%d", action_rec->action2);
  printf("\n");

  printf("\nActor: ");
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->actor))
    /* Not a special id. */
    printf("%d", action_rec->actor);
  printf("\n");

  printf("\nAdverbs:\n");
  printf("  %d adverbs\n ", action_rec->adverbs.nr_of_adverbs);
  for (i=0;i<MAX_PARSE_ADVERBS;i++) {
    printf("%d", action_rec->adverbs.adverb[i]);
    printf(" ");
  }
  printf("\n");

  printf("\nQ-word: ");
  if (action_rec->q_word == NO_ID)
    printf("<none>");
  else
    printf("%d", action_rec->q_word);
  printf("\n");

  printf("\nDirection: ");
  if (action_rec->direction == NO_ID)
    printf("<none>");
  else
    printf("%d", action_rec->direction);
  printf("\n");

  printf("\nSubject: %d ", action_rec->subject);
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->subject))
    /* Not a special id. */
    printf("%d", action_rec->subject);
  printf("\n");

  printf("\nSpecifier: ");
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->specifier))
    /* Not a special id. */
    printf("%d", action_rec->specifier);
  printf("\n");

  printf("\nPrepositions:\n");
  printf(" %d prepositions\n ", action_rec->prepositions.nr_of_prepositions);
  for (i=0; i<MAX_PARSE_PREPOS; i++) {
    printf("%d", action_rec->prepositions.preposition[i]);
    printf(" ");
  }
  printf("\n");

  printf("\nValue: %d\n", action_rec->value);

  printf("\nOrdinal: %d\n", action_rec->ordinal);

  printf("\n********************\n\n");
}

void PrintWTable(void)
{
  int32_t i,j = 0;

  printf("\n\n*** Word Table ***\n");

  for (i=0; i<nr_of_words; i++) {
    printf("\n%d: %s | ", word_table[i].id, word_table[i].word);
    for (j=0; j<MAX_TYPES; j++) {
      printf("%d ", word_table[i].types[j]);
    }
  }
}


void PrintWList(void)
{
  wordInfo *wp = word_list;
  int32_t i = 0;

  printf("\n\n*** Word List ***\n");

  while (wp != NULL) {
    printf("redefine: %d\n", wp->redefine);
    printf("word: %s\n", wp->word);
    printf("print_word: %s\n", wp->print_word);
    printf("id: %d\n", wp->id);
    for (i=0; i<MAX_TYPES; i++)
      printf("types[%d]: %d\n", i, wp->types[i]);
    printf("\n\n");
    wp = wp->next;
  }
  printf("nr_of_words: %d\n", nr_of_words);
  printf("nr_of_verbs: %d\n", nr_of_verbs);
  printf("nr_of_directions: %d\n", nr_of_directions);
  printf("first_direction_id: %d\n", first_direction_id);
}

/*****************************************************************/
/* following routines are for adding debug info to the game file */
/*****************************************************************/

int32_t CreateLocDebugInfo(debugInfo **loc_dbug)
{
  locationData *ld = loc_table;
  int i;

  /* Malloc() space for location debug info. */
  if ((*loc_dbug = (debugInfo *) malloc(nr_of_locations*sizeof(debugInfo))) == NULL) {
    PrintError(1, NULL, "location debug info");
    return(ERROR);
  }

  /* Initialize location debug info.     */
  for (i=0; i<nr_of_locations; i++) {
    strcpy((*loc_dbug)[i].name, "<unused>");
    (*loc_dbug)[i].owner = NO_ID;
  }

  while (ld->next != NULL) {
    strcpy( (*loc_dbug)[ld->l_id - FIRST_LOCATION_ID].name, ld->l_word);
    ld = ld->next;
  }

  return(OK);
}


int32_t CreateObjDebugInfo(debugInfo **obj_dbug)
{
  objectData *od = obj_table;
  int i;

  /* Malloc() space for object debug info. */
  if ((*obj_dbug = (debugInfo *) malloc(nr_of_objects*sizeof(debugInfo))) == NULL) {
    PrintError(1, NULL, "object debug info");
    return(ERROR);
  }

  /* Initialize object debug info.     */
  for (i=0; i<nr_of_objects; i++) {
    strcpy((*obj_dbug)[i].name, "<unused>");
    (*obj_dbug)[i].owner = NO_ID;
  }

  while (od->next != NULL) {
    strcpy( (*obj_dbug)[od->o_id - FIRST_OBJECT_ID].name, od->o_word);
    od = od->next;
  }

  return(OK);
}


int32_t CreateTriggerDebugInfo(debugInfo **com_trigg_dbug, debugInfo **loc_trigg_dbug)
{
  triggerData *tp = trigg_table;
  int i = 0;

  /* Malloc() space for common triggers debug info. */
  if ((*com_trigg_dbug = (debugInfo *) malloc(nr_of_ctrigs*sizeof(debugInfo))) == NULL) {
    PrintError(1, NULL, "common triggers debug info");
    return(ERROR);
  }

  /* Malloc() space for local triggers debug info. */
  if ((*loc_trigg_dbug = (debugInfo *) malloc(nr_of_ltrigs*sizeof(debugInfo))) == NULL) {
    PrintError(1, NULL, "local triggers debug info");
    return(ERROR);
  }

  /* Initialize common triggers debug info.     */
  for (i=0; i<nr_of_ctrigs; i++) {
    strcpy((*com_trigg_dbug)[i].name, "<unused>");
    (*com_trigg_dbug)[i].owner = NO_ID;
  }

  /* Initialize local triggers debug info.     */
  for (i=0; i<nr_of_ltrigs; i++) {
    strcpy((*loc_trigg_dbug)[i].name, "<unused>");
    (*loc_trigg_dbug)[i].owner = NO_ID;
  }

  /* Next, we have to go through the trigg_table and put the */
  /* names and owners in the debug info array.               */
  /* We have to check tp->next rather than tp, since the     */
  /* last struct is a TERMINATOR struct.                     */
  while (tp->next != NULL) {
    if (tp->common) {
      /* It is a common trigger, no need to store the owner */
      strcpy((*com_trigg_dbug)[tp->t_id - FIRST_COMMON_TRIGGER_ID].name, tp->t_word);
    }
    else {
      /* It is a local trigger */
      strcpy((*loc_trigg_dbug)[tp->t_id - FIRST_LOCAL_TRIGGER_ID].name, tp->t_word);
      (*loc_trigg_dbug)[tp->t_id - FIRST_LOCAL_TRIGGER_ID].owner = tp->owner;
    }

    tp = tp->next;
  } /* while */
  return(OK);
}


int32_t CreateTimerDebugInfo(debugInfo **timer_dbug)
{
  timerData *td = tim_table;;

  int i = 0;

  /* Malloc() space for timer debug info. */
  if ((*timer_dbug = (debugInfo *) malloc(nr_of_timers*sizeof(debugInfo))) == NULL) {
    PrintError(1, NULL, "timer debug info");
    return(ERROR);
  }

  /* Initialize timer debug info.     */
  for (i=0; i<nr_of_timers; i++) {
    strcpy((*timer_dbug)[i].name, "<unused>");
    (*timer_dbug)[i].owner = NO_ID;
  }

  /* Next, we have to go through the timers          */
  /* and put the names in the debug info array.      */
  /* We have to check tp->next rather than tp, since */
  /* the last struct is a TERMINATOR struct.         */
  while (td->next != NULL) {
    strcpy((*timer_dbug)[td->m_id - FIRST_TIMER_ID].name, td->m_word);
    td = td->next;
  }

  return(OK);
}


int32_t CreateFlagDebugInfo(debugInfo **com_flag_dbug, debugInfo **loc_flag_dbug)
{
  flagData  *ft = flag_table;

  int i = 0;

  /* Malloc() space for common flags debug info. */
  if ((*com_flag_dbug = (debugInfo *) malloc(nr_of_cflags*sizeof(debugInfo))) == NULL) {
    PrintError(1, NULL, "common flags debug info");
    return(ERROR);
  }

  /* Malloc() space for local flags debug info. */
  if ((*loc_flag_dbug = (debugInfo *) malloc(nr_of_lflags*sizeof(debugInfo))) == NULL) {
    PrintError(1, NULL, "local flags debug info");
    return(ERROR);
  }

  /* Initialize common flag debug info.     */
  for (i=0; i<nr_of_cflags; i++) {
    strcpy((*com_flag_dbug)[i].name, "<unused>");
    (*com_flag_dbug)[i].owner = NO_ID;
  }

  /* Initialize local flag debug info.     */
  for (i=0; i<nr_of_lflags; i++) {
    strcpy((*loc_flag_dbug)[i].name, "<unused>");
    (*loc_flag_dbug)[i].owner = NO_ID;
  }

  /* Next, we have to go through the flag_table and put the */
  /* names and owners in the debug info array.              */
  /* We have to check ft->next rather than ft, since the    */
  /* last struct is a TERMINATOR struct.                    */
  while (ft->next != NULL) {
    if (ft->common) {
      /* It is a common flag, no need to store the owner */
      strcpy((*com_flag_dbug)[ft->f_id - FIRST_COMMON_FLAG_ID].name, ft->f_word);
    }
    else {
      /* It is a local flag */
      strcpy((*loc_flag_dbug)[ft->f_id - FIRST_LOCAL_FLAG_ID].name, ft->f_word);
      (*loc_flag_dbug)[ft->f_id - FIRST_LOCAL_FLAG_ID].owner = ft->owner;
    }

    ft = ft->next;
  } /* while */

  return(OK);
}


int32_t CreateAttrDebugInfo(debugInfo **com_attr_dbug, debugInfo **loc_attr_dbug)
{
  attrData  *rt = attr_table;

  int i = 0;

  /* Malloc() space for common attributes debug info. */
  if ((*com_attr_dbug = (debugInfo *) malloc(nr_of_cattrs*sizeof(debugInfo))) == NULL) {
    PrintError(1, NULL, "common attributes debug info");
    return(ERROR);
  }

  /* Malloc() space for local attributes debug info. */
  if ((*loc_attr_dbug = (debugInfo *) malloc(nr_of_lattrs*sizeof(debugInfo))) == NULL) {
    PrintError(1, NULL, "local attributes debug info");
    return(ERROR);
  }

  /* Initialize common attribute debug info.     */
  for (i=0; i<nr_of_cattrs; i++) {
    strcpy((*com_attr_dbug)[i].name, "<unused>");
    (*com_attr_dbug)[i].owner = NO_ID;
  }

  /* Initialize local attribute debug info.     */
  for (i=0; i<nr_of_lattrs; i++) {
    strcpy((*loc_attr_dbug)[i].name, "<unused>");
    (*loc_attr_dbug)[i].owner = NO_ID;
  }

  /* Next, we have to go through the attr_table and put the */
  /* names and owners in the debug info array.              */
  /* We have to check rt->next rather than rt, since the    */
  /* last struct is a TERMINATOR struct.                    */
  while (rt->next != NULL) {
    if (rt->common) {
      /* It is a common attribute, no need to store the owner */
      strcpy((*com_attr_dbug)[rt->r_id - FIRST_COMMON_ATTR_ID].name, rt->r_word);
    }
    else {
      /* It is a local attribute. */
      strcpy((*loc_attr_dbug)[rt->r_id - FIRST_LOCAL_ATTR_ID].name, rt->r_word);
      (*loc_attr_dbug)[rt->r_id - FIRST_LOCAL_ATTR_ID].owner = rt->owner;
    }

    rt = rt->next;
  } /* while */

  return(OK);
}
