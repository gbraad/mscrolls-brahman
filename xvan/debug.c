
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
#include "defs.h"


/*************************/
/* Function declarations */
/*************************/
void    PrintStoryInfo(storyInfo*);
void    Debug(void);  /* used by XeqDebug() function */

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

/************************/
/* Function definitions */
/************************/

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


void Debug()
{
  PrintSpecialIds();
  PrintLocationDirectory();
  PrintObjectDirectory();
  PrintAllTimers();

}


void PrintLocationDirectory()
{
  /* Uses the following Global vars: */
  /*   - loc_dir (type dirInfo*)     */
  /*   - obj_dir (type dirInfo)      */
  /*   - nr_of_locs (type int)       */

  int32_t i, j;
  int32_t offset = 0;

  PrintString("\n\n**************************\n", 0);
  PrintString("*** LOCATION DIRECTORY ***\n", 0);
  PrintString("**************************\n\n", 0);

  for (i=0; i < nr_of_locs; i++) {
    sprintf(outputline, "\nOffset: %" _PRId64 "\n\n", loc_dir[i].offset);
    Output(outputline, 0);
    sprintf(outputline, "Location id: %d\n", i+FIRST_LOCATION_ID);
    Output(outputline, 0);
    for (j=0; j<loc_dir[i].nr_of_dsys; j++) {
      PrintExtendedSysDescr(&(loc_dir[i].descr[j]), 0);
      sprintf(outputline, "\n");
      Output(outputline, 0);
    }

    /* There is no containing object or location */

    /* Print the contained objects */
    sprintf(outputline, "Contained objects:\n");
    Output(outputline, 0);
    for (j=0; j<(loc_dir[i].contained_objs).nr_of_objects; j++) {
      sprintf(outputline, "  ");
      Output(outputline, 0);
      offset = ((loc_dir[i].contained_objs).object_ids)[j]-FIRST_OBJECT_ID;
      PrintExtendedSysDescr(&(obj_dir[offset].descr[0]), 0);
      sprintf(outputline, " (%d)\n", offset+FIRST_OBJECT_ID);
      Output(outputline, 0);
    }
  }
}


void PrintObjectDirectory()
{
  /* Uses the following Global vars: */
  /*   - loc_dir (type dirInfo*)     */
  /*   - obj_dir (type dirInfo)      */
  /*   - nr_of_objs (type int)       */

  int32_t i, j;
  int32_t offset = 0;

  PrintString("\n\n************************\n", 0);
  PrintString("*** OBJECT DIRECTORY ***\n", 0);
  PrintString("************************\n\n", 0);

  for (i=0; i < nr_of_objs; i++) {
    sprintf(outputline, "\nOffset: %" _PRId64 "\n\n", obj_dir[i].offset);
    Output(outputline, 0);
    sprintf(outputline, "Object id: %d\n", i+FIRST_OBJECT_ID);
    Output(outputline, 0);
    for (j=0; j<obj_dir[i].nr_of_dsys; j++) {
      PrintWord(obj_dir[i].descr[j].part1.article, 0); PrintString(" ", 0);
      PrintExtendedSysDescr(&(obj_dir[i].descr[j]), 0);
      sprintf(outputline, "\n");
      Output(outputline, 0);
    }

    /* Print the containing object or location */
    sprintf(outputline, "Contained in: ");
    Output(outputline, 0);
    if (IsLocId((obj_dir[i]).held_by)) {
      offset = (obj_dir[i]).held_by - FIRST_LOCATION_ID;
      PrintExtendedSysDescr(&(loc_dir[offset].descr[0]), 0);         /* print the description, use loc_dir here to find sysDescr as it is a location */
      sprintf(outputline, " (%d)\n", obj_dir[i].held_by); /* print the id */
      Output(outputline, 0);
    }
    else if (IsObjId(obj_dir[i].held_by)) {
      offset = (obj_dir[i]).held_by - FIRST_OBJECT_ID;
      PrintExtendedSysDescr(&(obj_dir[offset].descr[0]), 0);
      sprintf(outputline, " (%d)\n", obj_dir[i].held_by);
      Output(outputline, 0);
    }
    else {
      sprintf(outputline, "PrintObjectDirectory(): Error: container is not a location or object\n");
      Output(outputline, 0);
      return;
    }

    /* Print the contained objects */
    sprintf(outputline, "Contained objects:\n");
    Output(outputline, 0);
    for (j=0; j<(obj_dir[i].contained_objs).nr_of_objects; j++) {
      sprintf(outputline, "  ");
      Output(outputline, 0);
      offset = ((obj_dir[i].contained_objs).object_ids)[j]-FIRST_OBJECT_ID;
      PrintExtendedSysDescr(&(obj_dir[offset].descr[0]), 0);
      sprintf(outputline, " (%d)\n", offset+FIRST_OBJECT_ID);
      Output(outputline, 0);
    }
  }
}


void PrintExits()
{
  /* Uses the following Global vars: */
  /*   - exit_data (type *int)       */
  /*   - nr_of_directions (type int) */

  int32_t i, j;

  sprintf(outputline, "\n\n*****************\n");
  Output(outputline, 0);
  sprintf(outputline, "*** Exit data ***\n");
  Output(outputline, 0);
  sprintf(outputline, "*****************\n\n");
  Output(outputline, 0);

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
}


void PrintCommonAttributes(void)
{
  int32_t i;
  int32_t total_attrs; /* Total number of common attributes for all locations */

  sprintf(outputline, "\n\n*****************************************************************\n");
  Output(outputline, 0);
  sprintf(outputline, "*** Common Location Attributes (id, type, value, value owner) ***\n"); /* 10march2017 */
  Output(outputline, 0);
  sprintf(outputline, "*****************************************************************\n\n");
  Output(outputline, 0);

  total_attrs = nr_of_cattrs*nr_of_locs;

  for (i=0; i<total_attrs; i++) {
    sprintf(outputline, "%d, %d, %d %d\n", i+FIRST_COMMON_ATTR_ID, c_loc_attrs[i].type,
            c_loc_attrs[i].value, c_obj_attrs[i].value_owner);
    Output(outputline, 0);
  }
  sprintf(outputline, "\n");
  sprintf(outputline, "\n\n************************************************************\n");
  Output(outputline, 0);
  sprintf(outputline, "*** Common Object Attributes (type, value, value owner) ***\n"); /* 10march2017 */
  Output(outputline, 0);
  sprintf(outputline, "************************************************************\n\n");
  Output(outputline, 0);

  total_attrs = nr_of_cattrs*nr_of_objs;

  for (i=0; i<total_attrs; i++) {
    sprintf(outputline, "%d, %d, %d, %d\n", i+FIRST_COMMON_ATTR_ID, c_obj_attrs[i].type,
            c_obj_attrs[i].value, c_obj_attrs[i].value_owner); /* 10march2017 */
    Output(outputline, 0);
  }
  sprintf(outputline, "\n");
  Output(outputline, 0);
}


void PrintLocalAttributes(void)
{
  int32_t i;
  /* Total number of local attributes is stored in global variable nr_of_lattrs */

  sprintf(outputline, "\n\n***************************************************\n");
  Output(outputline, 0);
  sprintf(outputline, "*** Local Attributes (id, type, value, value owner) ***\n"); /* 10march2017 */
  Output(outputline, 0);
  sprintf(outputline, "*******************************************************\n\n");
  Output(outputline, 0);

  for (i=0; i<nr_of_lattrs; i++) {
    sprintf(outputline, "%d, %d, %d %d\n", i+FIRST_LOCAL_ATTR_ID, local_attrs[i].type, /* 10march2017 */
            local_attrs[i].value, local_attrs[i].value_owner);
    Output(outputline, 0);
  }
  sprintf(outputline, "\n");
  Output(outputline, 0);
}


void PrintCommonFlags(void)
{
  int32_t i;
  int32_t com_loc_flags_string_len = 0;
  int32_t com_obj_flags_string_len = 0;

  /* calculate the length of the common flag strings */
  com_loc_flags_string_len = ((nr_of_cflags*nr_of_locs)/WORD_LEN)+1;
  com_obj_flags_string_len = ((nr_of_cflags*nr_of_objs)/WORD_LEN)+1;

  sprintf(outputline, "\n\n************************************\n");
  Output(outputline, 0);
  sprintf(outputline, "*** Common location flags in hex ***\n");
  Output(outputline, 0);
  sprintf(outputline, "************************************\n\n");
  Output(outputline, 0);

  for (i=0; i<com_loc_flags_string_len; i++) {
    sprintf(outputline, "    %x", com_loc_flags[i]);
    Output(outputline, 0);
  }
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\n\n**********************************\n");
  Output(outputline, 0);
  sprintf(outputline, "*** Common object flags in hex ***\n");
  Output(outputline, 0);
  sprintf(outputline, "**********************************\n\n");
  Output(outputline, 0);

  for (i=0; i<com_obj_flags_string_len; i++) {
    sprintf(outputline, "    %x", com_obj_flags[i]);
    Output(outputline, 0);
  }
  sprintf(outputline, "\n");
  Output(outputline, 0);
}


void PrintLocalFlags(void)
{
  int32_t i;
  /* Global var loc_flags_string_len is used here */

  sprintf(outputline, "\n\n**************************\n");
  Output(outputline, 0);
  sprintf(outputline, "*** Local flags in hex ***\n");
  Output(outputline, 0);
  sprintf(outputline, "**************************\n\n");
  Output(outputline, 0);

  for (i=0; i<loc_flags_string_len; i++) {
    sprintf(outputline, "    %x", local_flags[i]);
    Output(outputline, 0);
  }
  sprintf(outputline, "\n");
  Output(outputline, 0);
}


void PrintLocation(loc)
 locationInfo *loc;
{
  descrInfo   *descr;
  triggerInfo *trigger;
  int32_t i = 0;

  sprintf(outputline, "\n\n**** Location Info ****\n\n");
  Output(outputline, 0);
  sprintf(outputline, "\n\n Descriptions:\n");
  Output(outputline, 0);

  descr = loc->descriptions;
  while (descr != NULL) {
    sprintf(outputline, "\n  Description %d: %s\n", descr->id, descr->text);
    Output(outputline, 0);
    descr = descr->next;
  }

  if (loc->triggers == NULL) {
    sprintf(outputline, "No triggers\n");
    Output(outputline, 0);
  }
  else {
    sprintf(outputline, "\n\n Triggers\n");
    Output(outputline, 0);
  }

  trigger = loc->triggers;
  while (trigger != NULL) {
    i = 0;
    sprintf(outputline, "\n  Trigger %d: \n", trigger->id);
    Output(outputline, 0);
    while (trigger->code[i] != END_OF_CODE) {
      sprintf(outputline, "   %d\n", trigger->code[i++]);
      Output(outputline, 0);
    }
    sprintf(outputline, "   %d\n", trigger->code[i]);     /* end of code */
    Output(outputline, 0);
    trigger = trigger->next;
  }

  sprintf(outputline, "\n***********************\n");
  Output(outputline, 0);
}


void PrintVerb(verb)
 verbInfo *verb;
{
  compActionRec *arec;
  int32_t           *code;
  int32_t           i = 0;

  sprintf(outputline, "\n\n**** Verb Info ****\n\n");
  Output(outputline, 0);
  sprintf(outputline, " Action records:\n");
  Output(outputline, 0);

  arec = verb->action_rec;
  while (arec != NULL) {
    PrintCompActionRec(arec);
    arec = arec->next;
  }

  sprintf(outputline, "Trigger code:\n");
  Output(outputline, 0);

  code = verb->code;
  while (code[i] != END_OF_CODE) {
    sprintf(outputline, "%d ", code[i++]);
    Output(outputline, 0);
  }
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\n***********************\n");
  Output(outputline, 0);
}


void PrintObject(obj)
 objectInfo *obj;
{
  descrInfo   *descr;
  triggerInfo *trigger;
  int32_t i = 0;

  sprintf(outputline, "\n\n**** Object Info ****\n\n");
  Output(outputline, 0);
  sprintf(outputline, "\n\n Descriptions:\n");
  Output(outputline, 0);

  descr = obj->descriptions;
  while (descr != NULL) {
    sprintf(outputline, "\n  Description %d: %s\n", descr->id, descr->text);
    Output(outputline, 0);
    descr = descr->next;
  }

  if (obj->triggers == NULL) {
    sprintf(outputline, "No triggers\n");
    Output(outputline, 0);
  }
  else {
    sprintf(outputline, "\n\n Triggers\n");
    Output(outputline, 0);
  }

  trigger = obj->triggers;
  while (trigger != NULL) {
    i = 0;
    sprintf(outputline, "\n  Trigger %d: \n", trigger->id);
    Output(outputline, 0);
    while (trigger->code[i] != END_OF_CODE) {
      sprintf(outputline, "%d ", trigger->code[i++]);
      Output(outputline, 0);
    }
    sprintf(outputline, "%d\n", trigger->code[i]);     /* end of code */
    Output(outputline, 0);
    trigger = trigger->next;
  }

  sprintf(outputline, "\n***********************\n");
  Output(outputline, 0);
}


void PrintSpecialIds()
{
  sprintf(outputline, "*** Special Ids ***\n");
  Output(outputline, 0);

  sprintf(outputline, "Current location: ");
  Output(outputline, 0);
  PrintId(curr_loc, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "Actor:            ");
  Output(outputline, 0);
  PrintId(actor, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "subject:          ");
  Output(outputline, 0);
  PrintId(subject, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "specifier:        ");
  Output(outputline, 0);
  PrintId(specifier, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "action:           ");
  Output(outputline, 0);
  PrintWord(action, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "direction:        ");
  Output(outputline, 0);
  PrintWord(direction, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "preposition:      ");
  Output(outputline, 0);
  PrintWord(prepos, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "This:             ");
  Output(outputline, 0);
  PrintId(THIS, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "Value:            ");
  Output(outputline, 0);
  PrintId(VALUE, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "Ordinal           ");
  Output(outputline, 0);
  PrintId(ORDINAL, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "*******************\n");
  Output(outputline, 0);
}


void PrintVerbDir(void)
{
  int i  = 0;

  printf("\n\n******** VERB DIRECTORY ********\n\n");
  printf("nr_of_verbs: %d\n", nr_of_verbs);
  for(i=0; i<nr_of_verbs; i++) {
    printf("\nverb id: %d (%d)\n", i+FIRST_VERB_ID, i);
    printf("scope: %d\n", verb_dir[i].scope);
    printf("offset: %" _PRId64 "\n", verb_dir[i].offset);
  }
  printf("\n\n********************************\n\n");
}


void PrintWTable()
{
  int32_t i = 0;

  /* print the word_table */
  sprintf(outputline, "\n\n*** word_table ***\n\n");
  Output(outputline, 0);
  sprintf(outputline, "word print_word\n\n");
  Output(outputline, 0);

  for (i=0; i<nr_of_words; i++) {
    sprintf(outputline, "%d %s, %s\n", word_table[i].id, word_table[i].word, word_table[i].print_word);
    Output(outputline, 0);
  }
  sprintf(outputline, "\n\n");
  Output(outputline, 0);
}


void PrintParsedInput(parsed_input)
 parsedInput *parsed_input;
{
  int32_t i = 0;
  int32_t j = 0;

  sprintf(outputline, "**** Parsed Input ****\n");
  Output(outputline, 0);

  sprintf(outputline, "Action1: ");
  Output(outputline, 0);
  PrintWord(parsed_input->action1, 0);
  Output("\n", 0);

  sprintf(outputline, "Action2: ");
  Output(outputline, 0);
  PrintWord(parsed_input->action2, 0);
  Output("\n", 0);

  sprintf(outputline, "\nActor - Part 1:\n");
  Output(outputline, 0);
  sprintf(outputline, " %d adjectives\n ", parsed_input->actor.part1.nr_of_adjectives);
  Output(outputline, 0);

  for (i=0; i<parsed_input->actor.part1.nr_of_adjectives; i++)
    PrintWord(parsed_input->actor.part1.adjectives[i], 0);

  sprintf(outputline, "\n noun: ");
  Output(outputline, 0);
  PrintWord(parsed_input->actor.part1.noun, 0);

  sprintf(outputline, "\n connecting preposition: ");
  Output(outputline, 0);
  PrintWord(parsed_input->actor.connect_prepos, 0);

  sprintf(outputline, "\nActor - Part 2:\n");
  Output(outputline, 0);
  sprintf(outputline, " %d adjectives\n ", parsed_input->actor.part2.nr_of_adjectives);
  Output(outputline, 0);

  for (i=0; i<parsed_input->actor.part2.nr_of_adjectives; i++)
    PrintWord(parsed_input->actor.part2.adjectives[i], 0);

  sprintf(outputline, "\n noun: ");
  Output(outputline, 0);
  PrintWord(parsed_input->actor.part2.noun, 0);

  sprintf(outputline, "\nAdverbs:\n");
  Output(outputline, 0);

  sprintf(outputline, "%d adverbs\n ", parsed_input->adverbs.nr_of_adverbs);
  Output(outputline, 0);

  for (i=0;i<parsed_input->adverbs.nr_of_adverbs;i++)
    PrintWord(parsed_input->adverbs.adverb[i], 0);

  sprintf(outputline, "\nQ-word: ");
  Output(outputline, 0);
  PrintWord(parsed_input->q_word, 0);

  sprintf(outputline, "\nDirection: ");
  Output(outputline, 0);
  PrintWord(parsed_input->direction, 0);

  sprintf(outputline, "\nSubject(s):\n");
  Output(outputline, 0);

  for (i=0; i<MAX_SUBJECTS; i++) {
    sprintf(outputline, "\nSubject %d - part 1\n ", i);
    Output(outputline, 0);
    sprintf(outputline, " %d adjectives\n ", parsed_input->subject[i].part1.nr_of_adjectives);
    Output(outputline, 0);
    for (j=0; j<parsed_input->subject[i].part1.nr_of_adjectives; j++)
      PrintWord(parsed_input->subject[i].part1.adjectives[j], 0);
    sprintf(outputline, "\n noun: ");
    Output(outputline, 0);
    PrintWord(parsed_input->subject[i].part1.noun, 0);

    sprintf(outputline, "\n connecting preposition: ");
    Output(outputline, 0);
    PrintWord(parsed_input->subject[i].connect_prepos, 0);

    sprintf(outputline, "\nSubject %d - part 2\n ", i);
    Output(outputline, 0);
    sprintf(outputline, " %d adjectives\n ", parsed_input->subject[i].part2.nr_of_adjectives);
    Output(outputline, 0);
    for (j=0; j<parsed_input->subject[i].part2.nr_of_adjectives; j++)
      PrintWord(parsed_input->subject[i].part2.adjectives[j], 0);
    sprintf(outputline, "\n noun: ");
    Output(outputline, 0);
    PrintWord(parsed_input->subject[i].part2.noun, 0);
    sprintf(outputline, "\n");
    Output(outputline, 0);
  }

  sprintf(outputline, "\nSpecifier - part 1:\n");
  Output(outputline, 0);
  sprintf(outputline, " %d adjectives\n ", parsed_input->specifier.part1.nr_of_adjectives);
  Output(outputline, 0);
  for (i=0; i<parsed_input->specifier.part1.nr_of_adjectives; i++)
    PrintWord(parsed_input->specifier.part1.adjectives[i], 0);
  sprintf(outputline, "\n noun: ");
  Output(outputline, 0);
  PrintWord(parsed_input->specifier.part1.noun, 0);

  sprintf(outputline, "\n connecting preposition: ");
  Output(outputline, 0);
  PrintWord(parsed_input->specifier.connect_prepos, 0);

  sprintf(outputline, "\nSpecifier - part 2:\n");
  Output(outputline, 0);
  sprintf(outputline, " %d adjectives\n ", parsed_input->specifier.part2.nr_of_adjectives);
  Output(outputline, 0);
  for (i=0; i<parsed_input->specifier.part2.nr_of_adjectives; i++)
    PrintWord(parsed_input->specifier.part2.adjectives[i], 0);
  sprintf(outputline, "\n noun: ");
  Output(outputline, 0);
  PrintWord(parsed_input->specifier.part2.noun, 0);

  sprintf(outputline, "\nPrepositions:\n");
  Output(outputline, 0);
  sprintf(outputline, " %d prepositions\n ", parsed_input->prepositions.nr_of_prepositions);
  Output(outputline, 0);
  for (i=0; i<parsed_input->prepositions.nr_of_prepositions; i++)
    PrintWord(parsed_input->prepositions.preposition[i], 0);

  sprintf(outputline, "\nValue:\n");
  Output(outputline, 0);
  sprintf(outputline, " %d\n ", parsed_input->value);
  Output(outputline, 0);

  sprintf(outputline, "\nOrdinal:\n");
  Output(outputline, 0);
  sprintf(outputline, " %d\n ", parsed_input->ordinal);
  Output(outputline, 0);

  sprintf(outputline, "\n**********************\n");
  Output(outputline, 0);
}


int32_t Test4SpecId(id)
 int32_t id;
{
  /* This functin test whether id has one of the following values: */
  /* NO_ID, CURR_LOC, SUBJECT, ACTOR, SPECIFIER, and prints a      */
  /* corresponding message.                                        */

  switch (id) {
    case NO_ID:
      sprintf(outputline, "<none>");
      Output(outputline, 0);
      return(OK);
    case CURR_LOC:
      sprintf(outputline, "<the current location>\n");
      Output(outputline, 0);
      return(OK);
    case SUBJECT:
      sprintf(outputline, "<the current subject>\n");
      Output(outputline, 0);
      return(OK);
    case ACTOR:
      sprintf(outputline, "<the current actor>\n");
      Output(outputline, 0);
      return(OK);
    case SPECIFIER:
      sprintf(outputline, "<the current specifier>\n");
      Output(outputline, 0);
      return(OK);
    default:
      /* Not a special id. */
      return(ERROR);
  } /* switch */
  /* No OK here, makes compiler happy. */
}


void PrintCompActionRec(action_rec)
 compActionRec *action_rec;
{
  int32_t      i = 0;

  sprintf(outputline, "**** Computer Action Record ****\n");
  Output(outputline, 0);

  sprintf(outputline, "\nAction1: ");
  Output(outputline, 0);
  if (action_rec->action1 == NO_ID) {
    sprintf(outputline, "<none>\n");
    Output(outputline, 0);
  }
  else
    if (action_rec->action1 == DEFAULT) {
      sprintf(outputline, "DEFAULT");
      Output(outputline, 0);
    }
    else
      PrintWord(action_rec->action1, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nAction2: ");
  Output(outputline, 0);
  if (action_rec->action2 == NO_ID) {
    sprintf(outputline, "<none>\n");
    Output(outputline, 0);
  }
  else
    if (action_rec->action2 == DEFAULT) {
      sprintf(outputline, "DEFAULT");
      Output(outputline, 0);
    }
    else
      PrintWord(action_rec->action2, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nActor: ");
  Output(outputline, 0);
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->actor))
    /* Not a special id. */
      PrintId(action_rec->actor, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nAdverbs:\n");
  Output(outputline, 0);
  sprintf(outputline, "  %d adverbs\n ", action_rec->adverbs.nr_of_adverbs);
  Output(outputline, 0);
  for (i=0;i<action_rec->adverbs.nr_of_adverbs;i++) {
    PrintWord(action_rec->adverbs.adverb[i], 0);
    sprintf(outputline, " ");
    Output(outputline, 0);
  }
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nQ-word: ");
  Output(outputline, 0);
  if (action_rec->q_word == NO_ID) {
    sprintf(outputline, "<none>");
    Output(outputline, 0);
  }
  else
    PrintWord(action_rec->q_word, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nDirection: ");
  Output(outputline, 0);
  if (action_rec->direction == NO_ID) {
    sprintf(outputline, "<none>");
    Output(outputline, 0);
  }
  else {
    sprintf(outputline, "%d\n", action_rec->direction);
    Output(outputline, 0);
  }

  sprintf(outputline, "\nSubject: %d ", action_rec->subject);
  Output(outputline, 0);
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->subject))
    /* Not a special id. */
    PrintId(action_rec->subject, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nSpecifier: ");
  Output(outputline, 0);
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->specifier))
    /* Not a special id. */
    PrintId(action_rec->specifier, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nPrepositions:\n");
  Output(outputline, 0);
  sprintf(outputline, " %d prepositions\n ", action_rec->prepositions.nr_of_prepositions);
  Output(outputline, 0);
  for (i=0; i<action_rec->prepositions.nr_of_prepositions; i++) {
    PrintWord(action_rec->prepositions.preposition[i], 0);
    sprintf(outputline, " ");
    Output(outputline, 0);
  }
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nValue: %d\n", action_rec->value);
  Output(outputline, 0);

  sprintf(outputline, "\nOrdinal: %d\n", action_rec->ordinal);
  Output(outputline, 0);

  sprintf(outputline, "Trigger: %d %d\n", action_rec->execute[0], action_rec->execute[1]);
  Output(outputline, 0);
  sprintf(outputline, "\n********************\n");
  Output(outputline, 0);
}


void PrintUsrActionRec(action_rec)
 usrActionRec *action_rec;
{
  int32_t      i = 0;

  sprintf(outputline, "**** User Action Record ****\n");
  Output(outputline, 0);

  sprintf(outputline, "\nAction1: ");
  Output(outputline, 0);
  if (action_rec->action1 == NO_ID) {
    sprintf(outputline, "<none>\n");
    Output(outputline, 0);
  }
  else
    if (action_rec->action1 == DEFAULT) {
      sprintf(outputline, "DEFAULT");
      Output(outputline, 0);
    }
    else
      PrintWord(action_rec->action1, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

    sprintf(outputline, "\nAction2: ");
    Output(outputline, 0);
  if (action_rec->action2 == NO_ID) {
    sprintf(outputline, "<none>\n");
    Output(outputline, 0);
  }
  else
    if (action_rec->action2 == DEFAULT) {
      sprintf(outputline, "DEFAULT");
      Output(outputline, 0);
    }
    else
      PrintWord(action_rec->action2, 0);
  Output("\n", 0);

  sprintf(outputline, "\nActor: ");
  Output(outputline, 0);
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->actor))
    /* Not a special id. */
      PrintId(action_rec->actor, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nAdverbs:\n");
  Output(outputline, 0);
  sprintf(outputline, "  %d adverbs\n ", action_rec->adverbs.nr_of_adverbs);
  Output(outputline, 0);
  for (i=0;i<action_rec->adverbs.nr_of_adverbs;i++) {
    PrintWord(action_rec->adverbs.adverb[i], 0);
    sprintf(outputline, " ");
    Output(outputline, 0);
  }
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nQ-word: ");
  Output(outputline, 0);
  if (action_rec->q_word == NO_ID) {
    sprintf(outputline, "<none>");
    Output(outputline, 0);
  }
  else
    PrintWord(action_rec->q_word, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nDirection: ");
  Output(outputline, 0);
  if (action_rec->direction == NO_ID) {
    sprintf(outputline, "<none>");
    Output(outputline, 0);
  }
  else {
    sprintf(outputline, "%d\n", action_rec->direction);
    Output(outputline, 0);
  }

  for (i=0; i<MAX_SUBJECTS; i++) {
    sprintf(outputline, "\nSubject: %d ", action_rec->subject[i]);
    Output(outputline, 0);
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->subject[i]))
      /* Not a special id. */
      PrintId(action_rec->subject[i], 0);
    sprintf(outputline, "\n");
    Output(outputline, 0);
  }

  sprintf(outputline, "\nSpecifier: ");
  Output(outputline, 0);
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->specifier))
    /* Not a special id. */
    PrintId(action_rec->specifier, 0);
  sprintf(outputline, "\n");
  Output(outputline, 0);

  sprintf(outputline, "\nPrepositions:\n");
  Output(outputline, 0);
  sprintf(outputline, " %d prepositions\n ", action_rec->prepositions.nr_of_prepositions);
  Output(outputline, 0);
  for (i=0; i<action_rec->prepositions.nr_of_prepositions; i++) {
    PrintWord(action_rec->prepositions.preposition[i], 0);
    sprintf(outputline, " ");
    Output(outputline, 0);
  }

  sprintf(outputline, "Value: %d\n ", action_rec->value);
  Output(outputline, 0);

  sprintf(outputline, "Ordinal: %d\n ", action_rec->ordinal);
  Output(outputline, 0);

  sprintf(outputline, "\n********************\n");
  Output(outputline, 0);
}


void PrintParserActionRec(action_rec)
 parserActionRec *action_rec;
{
  int32_t      i = 0;

  while (action_rec != NULL) {
    PrintString("**** Parser Action Record ****\n", 0);

    PrintString("\nAction1: ", 0);
    if (action_rec->action1 == NO_ID) {
      sprintf(outputline, "<none>\n");
      Output(outputline, 0);
    }
    else
      if (action_rec->action1 == DEFAULT) {
        sprintf(outputline, "DEFAULT");
        Output(outputline, 0);
      }
      else
        PrintWord(action_rec->action1, 0);
    sprintf(outputline, "\n");
    Output(outputline, 0);

    PrintString("\nAction2: ", 0);
    if (action_rec->action2 == NO_ID) {
      sprintf(outputline, "<none>\n");
      Output(outputline, 0);
    }
    else
      if (action_rec->action2 == DEFAULT) {
        sprintf(outputline, "DEFAULT");
        Output(outputline, 0);
      }
      else
        PrintWord(action_rec->action2, 0);
    sprintf(outputline, "\n");
    Output(outputline, 0);

    sprintf(outputline, "\nActor: ");
    Output(outputline, 0);
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->actor))
      /* Not a special id. */
        PrintId(action_rec->actor, 0);
    sprintf(outputline, "\n");
    Output(outputline, 0);

    sprintf(outputline, "\nAdverbs:\n");
    Output(outputline, 0);
    sprintf(outputline, "  %d adverbs\n ", action_rec->adverbs.nr_of_adverbs);
    Output(outputline, 0);
    for (i=0;i<action_rec->adverbs.nr_of_adverbs;i++) {
      PrintWord(action_rec->adverbs.adverb[i], 0);
      sprintf(outputline, " ");
      Output(outputline, 0);
    }
    sprintf(outputline, "\n");
    Output(outputline, 0);

    sprintf(outputline, "\nQ-word: ");
    Output(outputline, 0);
    if (action_rec->q_word == NO_ID) {
      sprintf(outputline, "<none>");
      Output(outputline, 0);
    }
    else
      PrintWord(action_rec->q_word, 0);
    sprintf(outputline, "\n");
    Output(outputline, 0);

    sprintf(outputline, "\nDirection: ");
    Output(outputline, 0);
    if (action_rec->direction == NO_ID) {
      sprintf(outputline, "<none>");
      Output(outputline, 0);
    }
    else {
      sprintf(outputline, "%d\n", action_rec->direction);
      Output(outputline, 0);
    }

    sprintf(outputline, "\nSubject: ");
    Output(outputline, 0);
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->subject))
      /* Not a special id. */
      PrintId(action_rec->subject, 0);
    else {
      sprintf(outputline, "special id: %d", action_rec->subject);
      Output(outputline, 0);
    }
    sprintf(outputline, "\n");
    Output(outputline, 0);

    sprintf(outputline, "\nSpecifier: ");
    Output(outputline, 0);
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->specifier))
      /* Not a special id. */
      PrintId(action_rec->specifier, 0);
    sprintf(outputline, "\n");
    Output(outputline, 0);

    sprintf(outputline, "\nPrepositions:\n");
    Output(outputline, 0);
    sprintf(outputline, " %d prepositions\n ", action_rec->prepositions.nr_of_prepositions);
    Output(outputline, 0);
    for (i=0; i<action_rec->prepositions.nr_of_prepositions; i++) {
      PrintWord(action_rec->prepositions.preposition[i], 0);
      sprintf(outputline, " ");
      Output(outputline, 0);
    }

    sprintf(outputline, "\nValue: %d\n ", action_rec->value);
    Output(outputline, 0);

    sprintf(outputline, "Ordinal: %d\n ", action_rec->ordinal);
    Output(outputline, 0);

    sprintf(outputline, "Score: %d\n ", action_rec->score);
    Output(outputline, 0);

    sprintf(outputline, "\n********************\n");
    Output(outputline, 0);

    action_rec = action_rec->next;
  }
}


void    PrintHits(hits, scope, target)
 match   *hits;
 int32_t scope;
 int32_t target;
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
  sprintf(outputline, "\n\n");
  Output(outputline, 0);
}


void PrintTimer(t)
 timerInfo *t;
{
  sprintf(outputline, "**********Timer info**********\n\n");
  Output(outputline, 0);

  sprintf(outputline, "value: %d\n", t->value);
  Output(outputline, 0);
  sprintf(outputline, "step: %d\n", t->step);
  Output(outputline, 0);
  sprintf(outputline, "interval: %d\n", t->interval);
  Output(outputline, 0);
  sprintf(outputline, "update: %d\n", t->update);
  Output(outputline, 0);
  sprintf(outputline, "direction: %d\n", t->direction);
  Output(outputline, 0);
  sprintf(outputline, "state: %d\n", t->state);
  Output(outputline, 0);
  sprintf(outputline, "trigger_at: %d\n", t->trigger_at);
  Output(outputline, 0);
  sprintf(outputline, "trigger_spec: %d\n", t->trigger_spec);
  Output(outputline, 0);
  sprintf(outputline, "owner execute: %d\n", t->execute[0]);
  Output(outputline, 0);
  sprintf(outputline, "id execute: %d\n", t->execute[1]);
  Output(outputline, 0);

  sprintf(outputline, "\n******************************\n");
  Output(outputline, 0);
}


void PrintSpanTree(tree)
 spanTree *tree;
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


void PrintRoute(route)
 int32_t *route;
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
