
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018 Marnix van den Bos.                   */
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

/************************/
/* Function definitions */
/************************/


void PrintFileList(file_list)
 fileList **file_list;
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

void PrintStoryInfo(info)
 storyInfo *info;
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

int32_t Test4SpecId(id)
 int32_t id;
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
  /* No OK here, makes compiler happy. */
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

void PrintActionRec(action_rec)
 actionRec *action_rec;
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

void PrintWTable()
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


void PrintWList()
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
