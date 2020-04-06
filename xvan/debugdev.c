
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
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
#include "debugdev.h"
#include "defs.h"


/*************************/
/* Function declarations */
/*************************/
void    PrintStoryInfo(storyInfo*);
void    PrintLocationDirectory(void);
void    PrintObjectDirectory(void);
void    PrintExits(void);
void    PrintCommonAttributes(void);
void    PrintLocalAttributes(void);
void    PrintCommonFlags(void);
void    PrintLocalFlags(void);
void    PrintVerb(verbInfo*);
void    PrintLocation(locationInfo*);
void    PrintObject(objectInfo*);
void    PrintSpecialIds(void);
void    PrintVerbDir(void);
void    PrintWTable(void);
int32_t Test4SpecId(int32_t);
void    PrintParsedInput(parsedInput*);
void    PrintCompActionRec(compActionRec*);
void    PrintUsrActionRec(usrActionRec*);
void    PrintParserActionRec(parserActionRec*);
void    PrintHits(match*, int32_t, int32_t);
void    PrintAllTimers(void);
void    PrintTimer(timerInfo*);
void    PrintSpanTree(spanTree*);
void    PrintRoute(int32_t*);
void    PrintUndoStack(void);  /* @!@ */

/************************/
/* Function definitions */
/************************/

void PrintStoryInfo(storyInfo *info)
{
  char text_to_print[OUTPUT_LINE_LEN];

  sprintf(text_to_print, "\n\n******** Meta data ********\n\n");
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Title: %s\n", info->title);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Author: %s\n", info->author);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Organization: %s\n", info->organization);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Cover text: %s\n", info->cover_text);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Credits: %s\n", info->credits);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Version: %s\n", info->version);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Android market: %s\n", info->android_mkt);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "ios market: %s\n", info->ios_mkt);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Back image: %s\n", info->backimage);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Effect: %s\n", info->effect);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Sidebar: %d\n", info->ui_sidebar);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Textinput: %d\n", info->ui_textinput);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Compass: %d\n", info->ui_compass);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Autolink: %d\n", info->autolink);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Compiler version: %s\n", info->compiler_version);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "XVAN language: %d\n", info->xvan_language);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "Story language: %d\n", info->story_language);
  outputline = AddToString(outputline, text_to_print);

  PrintString("\n********************************\n\n", 0);
}


void PrintLocationDirectory(void)
{
  /* Uses the following Global vars: */
  /*   - loc_dir (type dirInfo*)     */
  /*   - obj_dir (type dirInfo)      */
  /*   - nr_of_locs (type int)       */

  int32_t i, j;
  int32_t offset = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("\n\n**************************\n", 0);
  PrintString("*** LOCATION DIRECTORY ***\n", 0);
  PrintString("**************************\n\n", 0);

  for (i=0; i < nr_of_locs; i++) {
    sprintf(text_to_print, "\nOffset: %" _PRId64 "\n\n", loc_dir[i].offset);
    outputline = AddToString(outputline, text_to_print);
    Output();
    sprintf(text_to_print, "Location id: %d\n", i+FIRST_LOCATION_ID);
    outputline = AddToString(outputline, text_to_print);
    Output();
    for (j=0; j<loc_dir[i].nr_of_dsys; j++) {
      PrintExtendedSysDescr(&(loc_dir[i].descr[j]), 0);
      PrintString("\n", 0);
      Output();
    }

    /* There is no containing object or location */

    /* Print the contained objects */
    PrintString("Contained objects:\n", 0);
    Output();
    for (j=0; j<(loc_dir[i].contained_objs).nr_of_objects; j++) {
      PrintString("  ", 0);
      Output();
      offset = ((loc_dir[i].contained_objs).object_ids)[j]-FIRST_OBJECT_ID;
      PrintExtendedSysDescr(&(obj_dir[offset].descr[0]), 0);
      sprintf(text_to_print, " (%d)\n", offset+FIRST_OBJECT_ID);
      outputline = AddToString(outputline, text_to_print);
      Output();
    }
  }
}


void PrintObjectDirectory(void)
{
  /* Uses the following Global vars: */
  /*   - loc_dir (type dirInfo*)     */
  /*   - obj_dir (type dirInfo)      */
  /*   - nr_of_objs (type int)       */

  int32_t i, j;
  int32_t offset = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("\n\n************************\n", 0);
  PrintString("*** OBJECT DIRECTORY ***\n", 0);
  PrintString("************************\n\n", 0);

  for (i=0; i < nr_of_objs; i++) {
    sprintf(text_to_print, "\nOffset: %" _PRId64 "\n\n", obj_dir[i].offset);
    outputline = AddToString(outputline, text_to_print);
    Output();
    sprintf(text_to_print, "Object id: %d\n", i+FIRST_OBJECT_ID);
    outputline = AddToString(outputline, text_to_print);
    Output();
    for (j=0; j<obj_dir[i].nr_of_dsys; j++) {
      PrintWord(obj_dir[i].descr[j].part1.article, 0); PrintString(" ", 0);
      PrintExtendedSysDescr(&(obj_dir[i].descr[j]), 0);
      PrintString("\n", 0);
      Output();
    }

    /* Print the containing object or location */
    PrintString("Contained in: ", 0);
    Output();
    if (IsLocId((obj_dir[i]).held_by)) {
      offset = (obj_dir[i]).held_by - FIRST_LOCATION_ID;
      PrintExtendedSysDescr(&(loc_dir[offset].descr[0]), 0);         /* print the description, use loc_dir here to find sysDescr as it is a location */
      sprintf(text_to_print, " (%d)\n", obj_dir[i].held_by); /* print the id */
      outputline = AddToString(outputline, text_to_print);
      Output();
    }
    else if (IsObjId(obj_dir[i].held_by)) {
      offset = (obj_dir[i]).held_by - FIRST_OBJECT_ID;
      PrintExtendedSysDescr(&(obj_dir[offset].descr[0]), 0);
      sprintf(text_to_print, " (%d)\n", obj_dir[i].held_by);
      outputline = AddToString(outputline, text_to_print);
      Output();
    }
    else {
      sprintf(text_to_print, "PrintObjectDirectory(): Error: container is not a location or object\n");
      outputline = AddToString(outputline, text_to_print);
      Output();
      return;
    }

    /* Print the contained objects */
    PrintString("Contained objects:\n", 0);
    Output();
    for (j=0; j<(obj_dir[i].contained_objs).nr_of_objects; j++) {
      PrintString("  ", 0);
      Output();
      offset = ((obj_dir[i].contained_objs).object_ids)[j]-FIRST_OBJECT_ID;
      PrintExtendedSysDescr(&(obj_dir[offset].descr[0]), 0);
      sprintf(text_to_print, " (%d)\n", offset+FIRST_OBJECT_ID);
      outputline = AddToString(outputline, text_to_print);
      Output();
    }
  }
}


void PrintExits(void)
{
  /* Uses the following Global vars: */
  /*   - exit_data (type *int)       */
  /*   - nr_of_directions (type int) */

  int32_t i, j;

  PrintString("\n\n*****************\n", 0);
  PrintString("*** Exit data ***\n", 0);
  PrintString("*****************\n\n", 0);
  Output();

  i = 0;
  while (i<nr_of_locs) {
    PrintId(i+FIRST_LOCATION_ID, 0);
    PrintString(":\n", 0);
    for (j=0; j<nr_of_directions; j++) {
      if (exit_data[i*nr_of_directions+j] != NO_ID) {
        PrintId(j+first_direction_id, 0);
        PrintString("-> ", 0);
        PrintId(exit_data[i*nr_of_directions+j], 0);
        PrintString("\n", 0);
      }
    }
    PrintString("---------------\n", 0);
    i++;
  }
  Output();
}


void PrintCommonAttributes(void)
{
  int32_t i;
  int32_t total_attrs; /* Total number of common attributes for all locations */

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("\n\n*****************************************************************\n", 0);
  PrintString("*** Common Location Attributes (id, type, value, value owner) ***\n", 0); /* 10march2017 */
  PrintString("*****************************************************************\n\n", 0);
  Output();

  total_attrs = nr_of_cattrs*nr_of_locs;

  for (i=0; i<total_attrs; i++) {
    sprintf(text_to_print, "%d, %d, %d %d\n", i+FIRST_COMMON_ATTR_ID, c_loc_attrs[i].type,
            c_loc_attrs[i].value, c_obj_attrs[i].value_owner);
    outputline = AddToString(outputline, text_to_print);
    Output();
  }
  PrintString("\n", 0);
  PrintString("\n\n************************************************************\n", 0);
  PrintString("*** Common Object Attributes (type, value, value owner) ***\n", 0); /* 10march2017 */
  PrintString("************************************************************\n\n", 0);
  Output();

  total_attrs = nr_of_cattrs*nr_of_objs;

  for (i=0; i<total_attrs; i++) {
    sprintf(text_to_print, "%d, %d, %d, %d\n", i+FIRST_COMMON_ATTR_ID, c_obj_attrs[i].type,
            c_obj_attrs[i].value, c_obj_attrs[i].value_owner); /* 10march2017 */
    outputline = AddToString(outputline, text_to_print);
    Output();
  }
  PrintString("\n", 0);
  Output();
}


void PrintLocalAttributes(void)
{
  int32_t i;
  /* Total number of local attributes is stored in global variable nr_of_lattrs */

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("\n\n***************************************************\n", 0);
  PrintString("*** Local Attributes (id, type, value, value owner) ***\n", 0); /* 10march2017 */
  PrintString("*******************************************************\n\n", 0);
  Output();

  for (i=0; i<nr_of_lattrs; i++) {
    sprintf(text_to_print, "%d, %d, %d %d\n", i+FIRST_LOCAL_ATTR_ID, local_attrs[i].type, /* 10march2017 */
            local_attrs[i].value, local_attrs[i].value_owner);
    outputline = AddToString(outputline, text_to_print);
    Output();
  }
  PrintString("\n", 0);
  Output();
}


void PrintCommonFlags(void)
{
  int32_t i;
  int32_t com_loc_flags_string_len = 0;
  int32_t com_obj_flags_string_len = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  /* calculate the length of the common flag strings */
  com_loc_flags_string_len = ((nr_of_cflags*nr_of_locs)/WORD_LEN)+1;
  com_obj_flags_string_len = ((nr_of_cflags*nr_of_objs)/WORD_LEN)+1;

  PrintString("\n\n************************************\n", 0);
  PrintString("*** Common location flags in hex ***\n", 0);
  PrintString("************************************\n\n", 0);
  Output();

  for (i=0; i<com_loc_flags_string_len; i++) {
    sprintf(text_to_print, "    %x", com_loc_flags[i]);
    outputline = AddToString(outputline, text_to_print);
    Output();
  }
  PrintString("\n", 0);
  Output();

  PrintString("\n\n**********************************\n", 0);
  PrintString("*** Common object flags in hex ***\n", 0);
  PrintString("**********************************\n\n", 0);
  Output();

  for (i=0; i<com_obj_flags_string_len; i++) {
    sprintf(text_to_print, "    %x", com_obj_flags[i]);
    outputline = AddToString(outputline, text_to_print);
    Output();
  }
  PrintString("\n", 0);
  Output();
}


void PrintLocalFlags(void)
{
  int32_t i;

  char text_to_print[OUTPUT_LINE_LEN];

  /* Global var loc_flags_string_len is used here */

  PrintString("\n\n**************************\n", 0);
  PrintString("*** Local flags in hex ***\n", 0);
  PrintString("**************************\n\n", 0);
  Output();

  for (i=0; i<loc_flags_string_len; i++) {
    sprintf(text_to_print, "    %x", local_flags[i]);
    outputline = AddToString(outputline, text_to_print);
    Output();
  }
  PrintString("\n", 0);
  Output();
}


void PrintLocation(locationInfo *loc)
{
  descrInfo   *descr;
  triggerInfo *trigger;
  int32_t i = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("\n\n**** Location Info ****\n\n", 0);
  PrintString("\n\n Descriptions:\n", 0);
  Output();

  descr = loc->descriptions;
  while (descr != NULL) {
    sprintf(text_to_print, "\n  Description %d: %s\n", descr->id, descr->text);
    outputline = AddToString(outputline, text_to_print);
    Output();
    descr = descr->next;
  }

  if (loc->triggers == NULL) {
    PrintString("No triggers\n", 0);
    Output();
  }
  else {
    PrintString("\n\n Triggers\n", 0);
    Output();
  }

  trigger = loc->triggers;
  while (trigger != NULL) {
    i = 0;
    sprintf(text_to_print, "\n  Trigger %d: \n", trigger->id);
    outputline = AddToString(outputline, text_to_print);
    Output();
    while (trigger->code[i] != END_OF_CODE) {
      sprintf(text_to_print, "   %d\n", trigger->code[i++]);
      outputline = AddToString(outputline, text_to_print);
      Output();
    }
    sprintf(text_to_print, "   %d\n", trigger->code[i]);     /* end of code */
    outputline = AddToString(outputline, text_to_print);
    Output();
    trigger = trigger->next;
  }

  PrintString("\n***********************\n", 0);
  Output();
}


void PrintVerb(verbInfo *verb)
{
  compActionRec *arec;
  int32_t           *code;
  int32_t           i = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("\n\n**** Verb Info ****\n\n", 0);
  PrintString(" Action records:\n", 0);
  Output();

  arec = verb->action_rec;
  while (arec != NULL) {
    PrintCompActionRec(arec);
    arec = arec->next;
  }

  PrintString("Trigger code:\n",0);
  Output();

  code = verb->code;
  while (code[i] != END_OF_CODE) {
    sprintf(text_to_print, "%d ", code[i++]);
    outputline = AddToString(outputline, text_to_print);
    Output();
  }
  PrintString("\n", 0);
  Output();

  PrintString("\n***********************\n", 0);
  Output();
}


void PrintObject(objectInfo *obj)
{
  descrInfo   *descr;
  triggerInfo *trigger;
  int32_t i = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("\n\n**** Object Info ****\n\n", 0);
  PrintString("\n\n Descriptions:\n", 0);
  Output();

  descr = obj->descriptions;
  while (descr != NULL) {
    sprintf(text_to_print, "\n  Description %d: %s\n", descr->id, descr->text);
    outputline = AddToString(outputline, text_to_print);
    Output();
    descr = descr->next;
  }

  if (obj->triggers == NULL) {
    PrintString("No triggers\n", 0);
    Output();
  }
  else {
    PrintString("\n\n Triggers\n", 0);
    Output();
  }

  trigger = obj->triggers;
  while (trigger != NULL) {
    i = 0;
    sprintf(text_to_print, "\n  Trigger %d: \n", trigger->id);
    outputline = AddToString(outputline, text_to_print);
    Output();
    while (trigger->code[i] != END_OF_CODE) {
      sprintf(text_to_print, "%d ", trigger->code[i++]);
      outputline = AddToString(outputline, text_to_print);
      Output();
    }
    sprintf(text_to_print, "%d\n", trigger->code[i]);     /* end of code */
    outputline = AddToString(outputline, text_to_print);
    Output();
    trigger = trigger->next;
  }

  PrintString("\n***********************\n", 0);
  Output();
}


void PrintSpecialIds(void)
{
  PrintString("*** Special Ids ***\n", 0);
  PrintString("Current location: ", 0);
  PrintId(curr_loc, 0);
  PrintString("\n", 0);
  Output();

  PrintString("Actor:            ", 0);
  PrintId(actor, 0);
  PrintString("\n", 0);
  Output();

  PrintString("subject:          ", 0);
  PrintId(subject, 0);
  PrintString("\n", 0);
  Output();

  PrintString("specifier:        ", 0);
  PrintId(specifier, 0);
  PrintString("\n", 0);
  Output();

  PrintString("action:           ",0);
  PrintWord(action, 0);
  PrintString("\n", 0);
  Output();

  PrintString("direction:        ",  0);
  PrintWord(direction, 0);
  PrintString("\n", 0);
  Output();

  PrintString("preposition:      ", 0);
  PrintWord(prepos, 0);
  PrintString("\n", 0);
  Output();

  PrintString("This:             ", 0);
  PrintId(THIS, 0);
  PrintString("\n", 0);
  Output();

  PrintString("Value:            ", 0);
  PrintId(VALUE, 0);
  PrintString("\n", 0);
  Output();

  PrintString("Ordinal           ", 0);
  PrintId(ORDINAL, 0);
  PrintString("\n", 0);
  Output();

  PrintString("*******************\n", 0);
  Output();
}


void PrintVerbDir(void)
{
  int i  = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("\n\n******** VERB DIRECTORY ********\n\n", 0);
  sprintf(text_to_print, "nr_of_verbs: %d\n", nr_of_verbs);
  outputline = AddToString(outputline, text_to_print);
  for(i=0; i<nr_of_verbs; i++) {
    sprintf(text_to_print, "\nverb id: %d (%d)\n", i+FIRST_VERB_ID, i);
    outputline = AddToString(outputline, text_to_print);
    sprintf(text_to_print, "scope: %d\n", verb_dir[i].scope);
    outputline = AddToString(outputline, text_to_print);
    sprintf(text_to_print, "offset: %" _PRId64 "\n", verb_dir[i].offset);
    outputline = AddToString(outputline, text_to_print);
  }
  PrintString("\n\n********************************\n\n", 0);
}


void PrintWTable(void)
{
  int32_t i = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  /* print the word_table */
  PrintString("\n\n*** word_table ***\n\n", 0);
  PrintString("word print_word\n\n", 0);
  Output();

  for (i=0; i<nr_of_words; i++) {
    sprintf(text_to_print, "%d %s, %s\n", word_table[i].id, word_table[i].word, word_table[i].print_word);
    outputline = AddToString(outputline, text_to_print);
    Output();
  }
  PrintString("\n\n", 0);
  Output();
}


void PrintParsedInput(parsedInput *parsed_input)
{
  int32_t i = 0;
  int32_t j = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("**** Parsed Input ****\n", 0);
  Output();

  PrintString("Action1: ", 0);
  Output();
  PrintWord(parsed_input->action1, 0);
  Output();

  PrintString("Action2: ", 0);
  Output();
  PrintWord(parsed_input->action2, 0);
  Output();

  PrintString("\nActor - Part 1:\n", 0);
  Output();
  sprintf(text_to_print, " %d adjectives\n ", parsed_input->actor.part1.nr_of_adjectives);
  outputline = AddToString(outputline, text_to_print);
  Output();

  for (i=0; i<parsed_input->actor.part1.nr_of_adjectives; i++)
    PrintWord(parsed_input->actor.part1.adjectives[i], 0);

  PrintString("\n noun: ", 0);
  Output();
  PrintWord(parsed_input->actor.part1.noun, 0);

  PrintString("\n connecting preposition: ", 0);
  Output();
  PrintWord(parsed_input->actor.connect_prepos, 0);

  PrintString("\nActor - Part 2:\n", 0);
  sprintf(text_to_print, " %d adjectives\n ", parsed_input->actor.part2.nr_of_adjectives);
  outputline = AddToString(outputline, text_to_print);
  Output();

  for (i=0; i<parsed_input->actor.part2.nr_of_adjectives; i++)
    PrintWord(parsed_input->actor.part2.adjectives[i], 0);

  PrintString("\n noun: ", 0);
  Output();
  PrintWord(parsed_input->actor.part2.noun, 0);

  PrintString("\nAdverbs:\n", 0);

  sprintf(text_to_print, "%d adverbs\n ", parsed_input->adverbs.nr_of_adverbs);
  outputline = AddToString(outputline, text_to_print);
  Output();

  for (i=0;i<parsed_input->adverbs.nr_of_adverbs;i++)
    PrintWord(parsed_input->adverbs.adverb[i], 0);

  PrintString("\nQ-word: ", 0);
  Output();
  PrintWord(parsed_input->q_word, 0);

  PrintString("\nDirection: ", 0);
  Output();
  PrintWord(parsed_input->direction, 0);

  PrintString("\nSubject(s):\n", 0);
  Output();

  for (i=0; i<MAX_SUBJECTS; i++) {
    sprintf(text_to_print, "\nSubject %d - part 1\n ", i);
    outputline = AddToString(outputline, text_to_print);
    Output();
    sprintf(text_to_print, " %d adjectives\n ", parsed_input->subject[i].part1.nr_of_adjectives);
    outputline = AddToString(outputline, text_to_print);
    Output();
    for (j=0; j<parsed_input->subject[i].part1.nr_of_adjectives; j++)
      PrintWord(parsed_input->subject[i].part1.adjectives[j], 0);
    PrintString("\n noun: ", 0);
    Output();
    PrintWord(parsed_input->subject[i].part1.noun, 0);

    PrintString("\n connecting preposition: ", 0);
    Output();
    PrintWord(parsed_input->subject[i].connect_prepos, 0);

    sprintf(text_to_print, "\nSubject %d - part 2\n ", i);
    outputline = AddToString(outputline, text_to_print);
    Output();
    sprintf(text_to_print, " %d adjectives\n ", parsed_input->subject[i].part2.nr_of_adjectives);
    outputline = AddToString(outputline, text_to_print);
    Output();
    for (j=0; j<parsed_input->subject[i].part2.nr_of_adjectives; j++)
      PrintWord(parsed_input->subject[i].part2.adjectives[j], 0);
    PrintString("\n noun: ", 0);
    Output();
    PrintWord(parsed_input->subject[i].part2.noun, 0);
    PrintString("\n", 0);
    Output();
  }

  PrintString("\nSpecifier - part 1:\n", 0);
  Output();
  sprintf(text_to_print, " %d adjectives\n ", parsed_input->specifier.part1.nr_of_adjectives);
  outputline = AddToString(outputline, text_to_print);
  Output();
  for (i=0; i<parsed_input->specifier.part1.nr_of_adjectives; i++)
    PrintWord(parsed_input->specifier.part1.adjectives[i], 0);
  PrintString("\n noun: ", 0);
  Output();
  PrintWord(parsed_input->specifier.part1.noun, 0);

  PrintString("\n connecting preposition: ", 0);
  Output();
  PrintWord(parsed_input->specifier.connect_prepos, 0);

  PrintString("\nSpecifier - part 2:\n", 0);
  Output();
  sprintf(text_to_print, " %d adjectives\n ", parsed_input->specifier.part2.nr_of_adjectives);
  outputline = AddToString(outputline, text_to_print);
  Output();
  for (i=0; i<parsed_input->specifier.part2.nr_of_adjectives; i++)
    PrintWord(parsed_input->specifier.part2.adjectives[i], 0);
  PrintString("\n noun: ", 0);
  Output();
  PrintWord(parsed_input->specifier.part2.noun, 0);

  PrintString("\nPrepositions:\n", 0);
  Output();
  sprintf(text_to_print, " %d prepositions\n ", parsed_input->prepositions.nr_of_prepositions);
  outputline = AddToString(outputline, text_to_print);
  Output();
  for (i=0; i<parsed_input->prepositions.nr_of_prepositions; i++)
    PrintWord(parsed_input->prepositions.preposition[i], 0);

  PrintString("\nValue:\n", 0);
  sprintf(text_to_print, " %d\n ", parsed_input->value);
  outputline = AddToString(outputline, text_to_print);
  Output();

  PrintString("\nOrdinal:\n", 0);
  Output();
  sprintf(text_to_print, " %d\n ", parsed_input->ordinal);
  outputline = AddToString(outputline, text_to_print);
  Output();

  PrintString("\n**********************\n", 0);
  Output();
}


int32_t Test4SpecId(int32_t id)
{
  /* This functin test whether id has one of the following values: */
  /* NO_ID, CURR_LOC, SUBJECT, ACTOR, SPECIFIER, and prints a      */
  /* corresponding message.                                        */

  switch (id) {
    case NO_ID:
      PrintString("<none>", 0);
      Output();
      return(OK);
    case CURR_LOC:
      PrintString("<the current location>\n", 0);
      Output();
      return(OK);
    case SUBJECT:
      PrintString("<the current subject>\n", 0);
      Output();
      return(OK);
    case ACTOR:
      PrintString("<the current actor>\n", 0);
      Output();
      return(OK);
    case SPECIFIER:
      PrintString("<the current specifier>\n", 0);
      Output();
      return(OK);
    default:
      /* Not a special id. */
      return(ERROR);
  } /* switch */
  /* No OK here, makes compiler happy. */
}


void PrintCompActionRec(compActionRec *action_rec)
{
  int32_t      i = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("**** Computer Action Record ****\n", 0);
  Output();

  PrintString("\nAction1: ", 0);
  Output();
  if (action_rec->action1 == NO_ID) {
    PrintString("<none>\n", 0);
    Output();
  }
  else
    if (action_rec->action1 == DEFAULT) {
      PrintString("DEFAULT", 0);
      Output();
    }
    else
      PrintWord(action_rec->action1, 0);
  PrintString("\n", 0);
  Output();

  PrintString("\nAction2: ", 0);
  Output();
  if (action_rec->action2 == NO_ID) {
    PrintString("<none>\n", 0);
    Output();
  }
  else
    if (action_rec->action2 == DEFAULT) {
      PrintString("DEFAULT", 0);
      Output();
    }
    else
      PrintWord(action_rec->action2, 0);
  PrintString("\n", 0);
  Output();

  PrintString("\nActor: ", 0);
  Output();
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->actor))
    /* Not a special id. */
      PrintId(action_rec->actor, 0);
  PrintString("\n", 0);
  Output();

  PrintString("\nAdverbs:\n", 0);
  Output();
  sprintf(text_to_print, "  %d adverbs\n ", action_rec->adverbs.nr_of_adverbs);
  outputline = AddToString(outputline, text_to_print);
  Output();
  for (i=0;i<action_rec->adverbs.nr_of_adverbs;i++) {
    PrintWord(action_rec->adverbs.adverb[i], 0);
    PrintString(" ", 0);
    Output();
  }
  PrintString("\n", 0);
  Output();

  PrintString("\nQ-word: ", 0);
  Output();
  if (action_rec->q_word == NO_ID) {
    PrintString("<none>", 0);
    Output();
  }
  else
    PrintWord(action_rec->q_word, 0);
  PrintString("\n", 0);
  Output();

  PrintString("\nDirection: ", 0);
  Output();
  if (action_rec->direction == NO_ID) {
    PrintString("<none>", 0);
    Output();
  }
  else {
    sprintf(text_to_print, "%d\n", action_rec->direction);
    outputline = AddToString(outputline, text_to_print);
    Output();
  }

  sprintf(text_to_print, "\nSubject: %d ", action_rec->subject);
  outputline = AddToString(outputline, text_to_print);
  Output();
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->subject))
    /* Not a special id. */
    PrintId(action_rec->subject, 0);
  PrintString("\n", 0);
  Output();

  PrintString("\nSpecifier: ", 0);
  Output();
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->specifier))
    /* Not a special id. */
    PrintId(action_rec->specifier, 0);
  PrintString("\n", 0);
  Output();

  PrintString("\nPrepositions:\n", 0);
  Output();
  sprintf(text_to_print, " %d prepositions\n ", action_rec->prepositions.nr_of_prepositions);
  outputline = AddToString(outputline, text_to_print);
  Output();
  for (i=0; i<action_rec->prepositions.nr_of_prepositions; i++) {
    PrintWord(action_rec->prepositions.preposition[i], 0);
    PrintString(" ", 0);
    Output();
  }
  PrintString("\n", 0);
  Output();

  sprintf(text_to_print, "\nValue: %d\n", action_rec->value);
  outputline = AddToString(outputline, text_to_print);
  Output();

  sprintf(text_to_print, "\nOrdinal: %d\n", action_rec->ordinal);
  outputline = AddToString(outputline, text_to_print);
  Output();

  sprintf(text_to_print, "Trigger: %d %d\n", action_rec->execute[0], action_rec->execute[1]);
  outputline = AddToString(outputline, text_to_print);
  Output();
  PrintString("\n********************\n", 0);
  Output();
}


void PrintUsrActionRec(usrActionRec *action_rec)
{
  int32_t      i = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("**** User Action Record ****\n", 0);
  Output();

  PrintString("\nAction1: ", 0);
  Output();
  if (action_rec->action1 == NO_ID) {
    PrintString("<none>\n", 0);
    Output();
  }
  else
    if (action_rec->action1 == DEFAULT) {
      PrintString("DEFAULT", 0);
      Output();
    }
    else
      PrintWord(action_rec->action1, 0);
  PrintString("\n", 0);
  Output();

    PrintString("\nAction2: ", 0);
    Output();
  if (action_rec->action2 == NO_ID) {
    PrintString("<none>\n", 0);
    Output();
  }
  else
    if (action_rec->action2 == DEFAULT) {
      PrintString("DEFAULT", 0);
      Output();
    }
    else
      PrintWord(action_rec->action2, 0);
  Output();

  PrintString("\nActor: ", 0);
  Output();
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->actor))
    /* Not a special id. */
      PrintId(action_rec->actor, 0);
  PrintString("\n", 0);
  Output();

  PrintString("\nAdverbs:\n", 0);
  Output();
  sprintf(text_to_print, "  %d adverbs\n ", action_rec->adverbs.nr_of_adverbs);
  outputline = AddToString(outputline, text_to_print);
  Output();
  for (i=0;i<action_rec->adverbs.nr_of_adverbs;i++) {
    PrintWord(action_rec->adverbs.adverb[i], 0);
    PrintString(" ", 0);
    Output();
  }
  PrintString("\n", 0);
  Output();

  PrintString("\nQ-word: ", 0);
  Output();
  if (action_rec->q_word == NO_ID) {
    PrintString("<none>", 0);
    Output();
  }
  else
    PrintWord(action_rec->q_word, 0);
  PrintString("\n", 0);
  Output();

  PrintString("\nDirection: ", 0);
  Output();
  if (action_rec->direction == NO_ID) {
    PrintString("<none>", 0);
    Output();
  }
  else {
    sprintf(text_to_print, "%d\n", action_rec->direction);
    outputline = AddToString(outputline, text_to_print);
    Output();
  }

  for (i=0; i<MAX_SUBJECTS; i++) {
    sprintf(text_to_print, "\nSubject: %d ", action_rec->subject[i]);
    outputline = AddToString(outputline, text_to_print);
    Output();
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->subject[i]))
      /* Not a special id. */
      PrintId(action_rec->subject[i], 0);
    PrintString("\n", 0);
    Output();
  }

  PrintString("\nSpecifier: ", 0);
  Output();
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->specifier))
    /* Not a special id. */
    PrintId(action_rec->specifier, 0);
  PrintString("\n", 0);
  Output();

  PrintString("\nPrepositions:\n", 0);
  Output();
  sprintf(text_to_print, " %d prepositions\n ", action_rec->prepositions.nr_of_prepositions);
  outputline = AddToString(outputline, text_to_print);
  Output();
  for (i=0; i<action_rec->prepositions.nr_of_prepositions; i++) {
    PrintWord(action_rec->prepositions.preposition[i], 0);
    PrintString(" ", 0);
    Output();
  }

  sprintf(text_to_print, "Value: %d\n ", action_rec->value);
  outputline = AddToString(outputline, text_to_print);
  Output();

  sprintf(text_to_print, "Ordinal: %d\n ", action_rec->ordinal);
  outputline = AddToString(outputline, text_to_print);
  Output();

  PrintString("\n********************\n", 0);
  Output();
}


void PrintParserActionRec(parserActionRec *action_rec)
{
  int32_t      i = 0;

  char text_to_print[OUTPUT_LINE_LEN];

  while (action_rec != NULL) {
    PrintString("**** Parser Action Record ****\n", 0);

    PrintString("\nAction1: ", 0);
    if (action_rec->action1 == NO_ID) {
      PrintString("<none>\n", 0);
      Output();
    }
    else
      if (action_rec->action1 == DEFAULT) {
        PrintString("DEFAULT", 0);
        Output();
      }
      else
        PrintWord(action_rec->action1, 0);
    PrintString("\n", 0);
    Output();

    PrintString("\nAction2: ", 0);
    if (action_rec->action2 == NO_ID) {
      PrintString("<none>\n", 0);
      Output();
    }
    else
      if (action_rec->action2 == DEFAULT) {
        PrintString("DEFAULT", 0);
        Output();
      }
      else
        PrintWord(action_rec->action2, 0);
    PrintString("\n", 0);
    Output();

    PrintString("\nActor: ", 0);
    Output();
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->actor))
      /* Not a special id. */
        PrintId(action_rec->actor, 0);
    PrintString("\n", 0);
    Output();

    PrintString("\nAdverbs:\n", 0);
    Output();
    sprintf(text_to_print, "  %d adverbs\n ", action_rec->adverbs.nr_of_adverbs);
    outputline = AddToString(outputline, text_to_print);
    Output();
    for (i=0;i<action_rec->adverbs.nr_of_adverbs;i++) {
      PrintWord(action_rec->adverbs.adverb[i], 0);
      PrintString(" ", 0);
      Output();
    }
    PrintString("\n", 0);
    Output();

    PrintString("\nQ-word: ", 0);
    Output();
    if (action_rec->q_word == NO_ID) {
      PrintString("<none>", 0);
      Output();
    }
    else
      PrintWord(action_rec->q_word, 0);
    PrintString("\n", 0);
    Output();

    PrintString("\nDirection: ", 0);
    Output();
    if (action_rec->direction == NO_ID) {
      PrintString("<none>", 0);
      Output();
    }
    else {
      sprintf(text_to_print, "%d\n", action_rec->direction);
      outputline = AddToString(outputline, text_to_print);
      Output();
    }

    PrintString("\nSubject: ", 0);
    Output();
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->subject))
      /* Not a special id. */
      PrintId(action_rec->subject, 0);
    else {
      sprintf(text_to_print, "special id: %d", action_rec->subject);
      outputline = AddToString(outputline, text_to_print);
      Output();
    }
    PrintString("\n", 0);
    Output();

    PrintString("\nSpecifier: ", 0);
    Output();
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->specifier))
      /* Not a special id. */
      PrintId(action_rec->specifier, 0);
    PrintString("\n", 0);
    Output();

    PrintString("\nPrepositions:\n", 0);
    Output();
    sprintf(text_to_print, " %d prepositions\n ", action_rec->prepositions.nr_of_prepositions);
    outputline = AddToString(outputline, text_to_print);
    Output();
    for (i=0; i<action_rec->prepositions.nr_of_prepositions; i++) {
      PrintWord(action_rec->prepositions.preposition[i], 0);
      PrintString(" ", 0);
      Output();
    }

    sprintf(text_to_print, "\nValue: %d\n ", action_rec->value);
    outputline = AddToString(outputline, text_to_print);
    Output();

    sprintf(text_to_print, "Ordinal: %d\n ", action_rec->ordinal);
    outputline = AddToString(outputline, text_to_print);
    Output();

    sprintf(text_to_print, "Score: %d\n ", action_rec->score);
    outputline = AddToString(outputline, text_to_print);
    Output();

    PrintString("\n********************\n", 0);
    Output();

    action_rec = action_rec->next;
  }
}


void    PrintHits(match *hits, int32_t scope, int32_t target)
{
  int i = 0;

  PrintString("\n********* Hits *********\n\n", 0);
  PrintString("Hits for ", 0);
  switch (scope) {
    case ALL_LOCS:
      PrintString("All locations, ", 0);
      break;
    case ACTOR_ONLY:
      PrintString("Actor only, ", 0);
      break;
    case CURR_LOC_ONLY:
      PrintString("Current location, ", 0);
      break;
    default:
      PrintString("<unknown scope>, ", 0);
      break;
  }

  switch (target) {
    case ACTOR:
      PrintString("Actor.\n", 0);
      break;
    case SUBJECT:
      PrintString("Subject.\n", 0);
      break;
    case SPECIFIER:
      PrintString("Specifier.\n", 0);
      break;
    default:
      PrintString("<unknown target>\n", 0);
      break;
  }

  for (i=0; i<hits->nr_of_hits; i++) {
    PrintId(hits->matched_objs[i], 0);
    PrintString("\n", 0);
  }

  PrintString("\n", 0);
  PrintString("\n************************\n\n", 0);
}


void PrintAllTimers(void)
{
  int32_t i = 0;

  for (i=0; i<nr_of_timers; i++)
    PrintTimer(timers+i);
  PrintString("\n\n", 0);
  Output();
}


void PrintTimer(timerInfo *t)
{

  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("**********Timer info**********\n\n", 0);

  sprintf(text_to_print, "value: %d\n", t->value);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "step: %d\n", t->step);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "interval: %d\n", t->interval);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "update: %d\n", t->update);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "direction: %d\n", t->direction);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "state: %d\n", t->state);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "trigger_at: %d\n", t->trigger_at);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "trigger_spec: %d\n", t->trigger_spec);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "owner execute: %d\n", t->execute[0]);
  outputline = AddToString(outputline, text_to_print);
  sprintf(text_to_print, "id execute: %d\n", t->execute[1]);
  outputline = AddToString(outputline, text_to_print);
  Output();

  PrintString("\n******************************\n", 0);
  Output();
}


void PrintSpanTree(spanTree *tree)
{
  int i = 0;

  PrintString("****** Spanning Tree ******\n\n", 0);

  for (i=0; i<nr_of_locs; i++) {
    PrintString("Location: ", 0);
    PrintId(i+FIRST_LOCATION_ID, 0);
    PrintString(":\n from_loc: ",  0);
    PrintId(tree[i].from_loc, 0);
    PrintString("\n from_dir: ", 0);
    PrintId(tree[i].from_dir, 0);
    PrintString("\n level: ", 0);
    PrintNumber(tree[i].level, 0);
    PrintString("\n\n", 0);
  }
  PrintString("***************************\n\n", 0);
}


void PrintRoute(int32_t *route)
{
  int nr_of_hops = 0;
  int i = 0;

  PrintString("****** Route ******\n\n", 0);
  /* find starting location */
  while (route[nr_of_hops] != NO_ID)
    nr_of_hops++;

     for (i=nr_of_hops-1; i>=0; i--) {
       PrintId(route[i], 0);
      PrintString(", ", 0);
     }
     PrintString("\n", 0);
  PrintString("*******************\n\n", 0);
}


void PrintUndoStack()  /* @!@ */
{
  int  i = undo_sp;
  char text_to_print[OUTPUT_LINE_LEN];

  PrintString("\n***** undo stack *****\n", 0);
  if (undo_stack == NULL) {
    PrintString("   uninitialized undo stack\n********\n", 0);
    return;
  }

  if (undo_sp == -1) {
    PrintString("   empty undo stack\n********\n", 0);
    return;
  }

  sprintf(text_to_print, "position\titem1\titem2\titem3\titem4\tvalue\t(sp = %d)\n", undo_sp);
  PrintString(text_to_print, 0);

  for (i=0; i<undo_stack_size; i++) {
    sprintf(text_to_print, "%d\t\t%d\t%d\t%d\t%d\t%d\n",
                           i,
                           undo_stack[i].item1,
                           undo_stack[i].item2,
                           undo_stack[i].item3,
                           undo_stack[i].item4,
                           undo_stack[i].value);
    PrintString(text_to_print, 0);
  }
  return;
  /* undo_stack runs from 0 to undo_stack_size-1 */
  while (1) {
    sprintf(text_to_print, "%d\t\t%d\t%d\t%d\t%d\t%d\n",
                           i,
                           undo_stack[i].item1,
                           undo_stack[i].item2,
                           undo_stack[i].item3,
                           undo_stack[i].item4,
                           undo_stack[i].value);
    PrintString(text_to_print, 0);
    if (i == undo_stack_size - 1) {
      /* roll over */
      i = 0;
    }
    else {
      i++;
    }

    if (i == undo_sp) {
      /* second time i equals undo_sp, means */
      /* that we printed the whole stack     */
      PrintString("**********************\n", 0);
      return;
    }
  }
}
