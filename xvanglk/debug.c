
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "debug.h"


/*************************/
/* Function declarations */
/*************************/
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

void Debug()
{
  PrintSpecialIds();
  PrintLocationDirectory();
  PrintObjectDirectory();
  PrintAllTimers();

}

void PrintLocationDirectory()
{
  /* Uses the following Global vars:
     - loc_dir (type dirInfo*)
     - obj_dir (type dirInfo)
     - nr_of_locs (type int)
  */

  int32_t i, j;
  int32_t offset = 0;

  PrintString("\n\n**************************\n");
  PrintString("*** LOCATION DIRECTORY ***\n");
  PrintString("**************************\n\n");

  for (i=0; i < nr_of_locs; i++) {
    sprintf(outputline, "\nOffset: %ld\n\n", loc_dir[i].offset);
    Output(outputline);
    sprintf(outputline, "Location id: %d\n", i+FIRST_LOCATION_ID);
    Output(outputline);
    for (j=0; j<loc_dir[i].nr_of_dsys; j++) {
      PrintExtendedSysDescr(&(loc_dir[i].descr[j]));
      sprintf(outputline, "\n");
      Output(outputline);
    }

    /* There is no containing object or location */

    /* Print the contained objects */
    sprintf(outputline, "Contained objects:\n");
    Output(outputline);
    for (j=0; j<(loc_dir[i].contained_objs).nr_of_objects; j++) {
      sprintf(outputline, "  ");
      Output(outputline);
      offset = ((loc_dir[i].contained_objs).object_ids)[j]-FIRST_OBJECT_ID;
      PrintExtendedSysDescr(&(obj_dir[offset].descr[0]));
      sprintf(outputline, " (%d)\n", offset+FIRST_OBJECT_ID);
      Output(outputline);
    }
  }
}

void PrintObjectDirectory()
{
  /* Uses the following Global vars:
     - obj_dir (type dirInfo*)
     - loc_dir (type dirInfo*)
     - nr_of_objs (type int)
  */

  int32_t i, j;
  int32_t offset = 0;

  PrintString("\n\n************************\n");
  PrintString("*** OBJECT DIRECTORY ***\n");
  PrintString("************************\n\n");

  for (i=0; i < nr_of_objs; i++) {
    sprintf(outputline, "\nOffset: %ld\n\n", obj_dir[i].offset);
    Output(outputline);
    sprintf(outputline, "Object id: %d\n", i+FIRST_OBJECT_ID);
    Output(outputline);
    for (j=0; j<obj_dir[i].nr_of_dsys; j++) {
      PrintExtendedSysDescr(&(obj_dir[i].descr[j]));
      sprintf(outputline, "\n");
      Output(outputline);
    }

    /* Print the containing object or location */
    sprintf(outputline, "Contained in: ");
    Output(outputline);
    if (IsLocId((obj_dir[i]).held_by)) {
      offset = (obj_dir[i]).held_by - FIRST_LOCATION_ID;
      PrintExtendedSysDescr(&(loc_dir[offset].descr[0]));         /* print the description, use loc_dir here to find sysDescr as it is a location */
      sprintf(outputline, " (%d)\n", obj_dir[i].held_by); /* print the id */
      Output(outputline);
    }
    else if (IsObjId(obj_dir[i].held_by)) {
      offset = (obj_dir[i]).held_by - FIRST_OBJECT_ID;
      PrintExtendedSysDescr(&(obj_dir[offset].descr[0]));
      sprintf(outputline, " (%d)\n", obj_dir[i].held_by);
      Output(outputline);
    }
    else {
      sprintf(outputline, "PrintObjectDirectory(): Error: container is not a location or object\n");
      Output(outputline);
      return;
    }

    /* Print the contained objects */
    sprintf(outputline, "Contained objects:\n");
    Output(outputline);
    for (j=0; j<(obj_dir[i].contained_objs).nr_of_objects; j++) {
      sprintf(outputline, "  ");
      Output(outputline);
      offset = ((obj_dir[i].contained_objs).object_ids)[j]-FIRST_OBJECT_ID;
      PrintExtendedSysDescr(&(obj_dir[offset].descr[0]));
      sprintf(outputline, " (%d)\n", offset+FIRST_OBJECT_ID);
      Output(outputline);
    }
  }
}

void PrintExits()
{
  /* Uses the following Global vars:
     - exit_data (type *int)
     - nr_of_directions (type int)
  */

  int32_t i, j;

  sprintf(outputline, "\n\n*****************\n");
  Output(outputline);
  sprintf(outputline, "*** Exit data ***\n");
  Output(outputline);
  sprintf(outputline, "*****************\n\n");
  Output(outputline);

  i = 0;
  while (i<nr_of_locs) {
    PrintId(i+FIRST_LOCATION_ID);
    PrintString(":\n");
    for (j=0; j<nr_of_directions; j++) {
      if (exit_data[i*nr_of_directions+j] != NO_ID) {
        PrintId(j+first_direction_id);
        PrintString("-> ");
        PrintId(exit_data[i*nr_of_directions+j]);
        PrintString("\n");
      }
    }
    PrintString("---------------\n");
    i++;
  }
}

void PrintCommonAttributes(void)
{
  int32_t i;
  int32_t total_attrs; /* Total number of common attributes for all locations */

  sprintf(outputline, "\n\n*****************************************************************\n");
  Output(outputline);
  sprintf(outputline, "*** Common Location Attributes (id, type, value, value owner) ***\n"); /* 10march2017 */
  Output(outputline);
  sprintf(outputline, "*****************************************************************\n\n");
  Output(outputline);

  total_attrs = nr_of_cattrs*nr_of_locs;

  for (i=0; i<total_attrs; i++) {
    sprintf(outputline, "%d, %d, %d, %d\n", i+FIRST_COMMON_ATTR_ID, c_loc_attrs[i].type,
            c_loc_attrs[i].value, c_loc_attrs[i].value_owner); /* 10march2017 */
    Output(outputline);
  }
  sprintf(outputline, "\n");

  sprintf(outputline, "\n\n************************************************************\n");
  Output(outputline);
  sprintf(outputline, "*** Common Object Attributes (type, value, value owner) ***\n"); /* 10march2017 */
  Output(outputline);
  sprintf(outputline, "************************************************************\n\n");
  Output(outputline);

  total_attrs = nr_of_cattrs*nr_of_objs;

  for (i=0; i<total_attrs; i++) {
    sprintf(outputline, "%d, %d, %d, %d\n", i+FIRST_COMMON_ATTR_ID, c_obj_attrs[i].type,
            c_obj_attrs[i].value, c_obj_attrs[i].value_owner); /* 10march2017 */
    Output(outputline);
  }
  sprintf(outputline, "\n");
  Output(outputline);
}

void PrintLocalAttributes(void)
{
  int32_t i;
  /* Total number of local attributes is stored in global variable nr_of_lattrs */

  sprintf(outputline, "\n\n***************************************************\n");
  Output(outputline);
  sprintf(outputline, "*** Local Attributes (id, type, value, value owner) ***\n"); /* 10march2017 */
  Output(outputline);
  sprintf(outputline, "*******************************************************\n\n");
  Output(outputline);

  for (i=0; i<nr_of_lattrs; i++) {
    sprintf(outputline, "%d, %d, %d %d\n", i+FIRST_LOCAL_ATTR_ID, local_attrs[i].type, /* 10march2017 */
            local_attrs[i].value, local_attrs[i].value_owner);
    Output(outputline);
  }
  sprintf(outputline, "\n");
  Output(outputline);
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
  Output(outputline);
  sprintf(outputline, "*** Common location flags in hex ***\n");
  Output(outputline);
  sprintf(outputline, "************************************\n\n");
  Output(outputline);

  for (i=0; i<com_loc_flags_string_len; i++) {
    sprintf(outputline, "    %x", com_loc_flags[i]);
    Output(outputline);
  }
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\n\n**********************************\n");
  Output(outputline);
  sprintf(outputline, "*** Common object flags in hex ***\n");
  Output(outputline);
  sprintf(outputline, "**********************************\n\n");
  Output(outputline);

  for (i=0; i<com_obj_flags_string_len; i++) {
    sprintf(outputline, "    %x", com_obj_flags[i]);
    Output(outputline);
  }
  sprintf(outputline, "\n");
  Output(outputline);
}

void PrintLocalFlags(void)
{
  int32_t i;
  /* Global var loc_flags_string_len is used here */

  sprintf(outputline, "\n\n**************************\n");
  Output(outputline);
  sprintf(outputline, "*** Local flags in hex ***\n");
  Output(outputline);
  sprintf(outputline, "**************************\n\n");
  Output(outputline);

  for (i=0; i<loc_flags_string_len; i++) {
    sprintf(outputline, "    %x", local_flags[i]);
    Output(outputline);
  }
  sprintf(outputline, "\n");
  Output(outputline);
}


void PrintLocation(loc)
 locationInfo *loc;
{
  descrInfo   *descr;
  triggerInfo *trigger;
  int32_t i = 0;

  sprintf(outputline, "\n\n**** Location Info ****\n\n");
  Output(outputline);
  sprintf(outputline, "\n\n Descriptions:\n");
  Output(outputline);

  descr = loc->descriptions;
  while (descr != NULL) {
    sprintf(outputline, "\n  Description %d: %s\n", descr->id, descr->text);
    Output(outputline);
    descr = descr->next;
  }

  if (loc->triggers == NULL) {
    sprintf(outputline, "No triggers\n");
    Output(outputline);
  }
  else {
    sprintf(outputline, "\n\n Triggers\n");
    Output(outputline);
  }

  trigger = loc->triggers;
  while (trigger != NULL) {
    i = 0;
    sprintf(outputline, "\n  Trigger %d: \n", trigger->id);
    Output(outputline);
    while (trigger->code[i] != END_OF_CODE) {
      sprintf(outputline, "   %d\n", trigger->code[i++]);
      Output(outputline);
    }
    sprintf(outputline, "   %d\n", trigger->code[i]);     /* end of code */
    Output(outputline);
    trigger = trigger->next;
  }

  sprintf(outputline, "\n***********************\n");
  Output(outputline);
}

void PrintVerb(verb)
 verbInfo *verb;
{
  compActionRec *arec;
  int32_t           *code;
  int32_t           i = 0;

  sprintf(outputline, "\n\n**** Verb Info ****\n\n");
  Output(outputline);
  sprintf(outputline, " Action records:\n");
  Output(outputline);

  arec = verb->action_rec;
  while (arec != NULL) {
    PrintCompActionRec(arec);
    arec = arec->next;
  }

  sprintf(outputline, "Trigger code:\n");
  Output(outputline);

  code = verb->code;
  while (code[i] != END_OF_CODE) {
    sprintf(outputline, "%d ", code[i++]);
    Output(outputline);
  }
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\n***********************\n");
  Output(outputline);
}

void PrintObject(obj)
 objectInfo *obj;
{
  descrInfo   *descr;
  triggerInfo *trigger;
  int32_t i = 0;

  sprintf(outputline, "\n\n**** Object Info ****\n\n");
  Output(outputline);
  sprintf(outputline, "\n\n Descriptions:\n");
  Output(outputline);

  descr = obj->descriptions;
  while (descr != NULL) {
    sprintf(outputline, "\n  Description %d: %s\n", descr->id, descr->text);
    Output(outputline);
    descr = descr->next;
  }

  if (obj->triggers == NULL) {
    sprintf(outputline, "No triggers\n");
    Output(outputline);
  }
  else {
    sprintf(outputline, "\n\n Triggers\n");
    Output(outputline);
  }

  trigger = obj->triggers;
  while (trigger != NULL) {
    i = 0;
    sprintf(outputline, "\n  Trigger %d: \n", trigger->id);
    Output(outputline);
    while (trigger->code[i] != END_OF_CODE) {
      sprintf(outputline, "%d ", trigger->code[i++]);
      Output(outputline);
    }
    sprintf(outputline, "%d\n", trigger->code[i]);     /* end of code */
    Output(outputline);
    trigger = trigger->next;
  }

  sprintf(outputline, "\n***********************\n");
  Output(outputline);
}

void PrintSpecialIds()
{
  sprintf(outputline, "*** Special Ids ***\n");
  Output(outputline);

  sprintf(outputline, "Current location: ");
  Output(outputline);
  PrintId(curr_loc);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "Actor:            ");
  Output(outputline);
  PrintId(actor);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "subject:          ");
  Output(outputline);
  PrintId(subject);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "specifier:        ");
  Output(outputline);
  PrintId(specifier);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "action:           ");
  Output(outputline);
  PrintWord(action);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "direction:        ");
  Output(outputline);
  PrintWord(direction);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "preposition:      ");
  Output(outputline);
  PrintWord(prepos);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "This:             ");
  Output(outputline);
  PrintId(THIS);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "Value:            ");
  Output(outputline);
  PrintId(VALUE);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "Ordinal           ");
  Output(outputline);
  PrintId(ORDINAL);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "*******************\n");
  Output(outputline);
}

void PrintWTable()
{
  int32_t i = 0;

  /* print the word_table */
  sprintf(outputline, "\n\n*** word_table ***\n\n");
  Output(outputline);
  sprintf(outputline, "word print_word\n\n");
  Output(outputline);

  for (i=0; i<nr_of_words; i++) {
    sprintf(outputline, "%d %s, %s\n", word_table[i].id, word_table[i].word, word_table[i].print_word);
    Output(outputline);
  }
  sprintf(outputline, "\n\n");
  Output(outputline);
}

void PrintParsedInput(parsed_input)
 parsedInput *parsed_input;
{
  int32_t i = 0;
  int32_t j = 0;

  sprintf(outputline, "**** Parsed Input ****\n");
  Output(outputline);

  sprintf(outputline, "Action: ");
  Output(outputline);
  PrintWord(parsed_input->action);

  sprintf(outputline, "\nActor - Part 1:\n");
  Output(outputline);
  sprintf(outputline, " %d adjectives\n ", parsed_input->actor.part1.nr_of_adjectives);
  Output(outputline);

  for (i=0; i<parsed_input->actor.part1.nr_of_adjectives; i++)
    PrintWord(parsed_input->actor.part1.adjectives[i]);

  sprintf(outputline, "\n noun: ");
  Output(outputline);
  PrintWord(parsed_input->actor.part1.noun);

  sprintf(outputline, "\n connecting preposition: ");
  Output(outputline);
  PrintWord(parsed_input->actor.connect_prepos);

  sprintf(outputline, "\nActor - Part 2:\n");
  Output(outputline);
  sprintf(outputline, " %d adjectives\n ", parsed_input->actor.part2.nr_of_adjectives);
  Output(outputline);

  for (i=0; i<parsed_input->actor.part2.nr_of_adjectives; i++)
    PrintWord(parsed_input->actor.part2.adjectives[i]);

  sprintf(outputline, "\n noun: ");
  Output(outputline);
  PrintWord(parsed_input->actor.part2.noun);

  sprintf(outputline, "\nAdverbs:\n");
  Output(outputline);

  sprintf(outputline, "%d adverbs\n ", parsed_input->adverbs.nr_of_adverbs);
  Output(outputline);

  for (i=0;i<parsed_input->adverbs.nr_of_adverbs;i++)
    PrintWord(parsed_input->adverbs.adverb[i]);

  sprintf(outputline, "\nQ-word: ");
  Output(outputline);
  PrintWord(parsed_input->q_word);

  sprintf(outputline, "\nDirection: ");
  Output(outputline);
  PrintWord(parsed_input->direction);

  sprintf(outputline, "\nSubject(s):\n");
  Output(outputline);

  for (i=0; i<MAX_SUBJECTS; i++) {
    sprintf(outputline, "\nSubject %d - part 1\n ", i);
    Output(outputline);
    sprintf(outputline, " %d adjectives\n ", parsed_input->subject[i].part1.nr_of_adjectives);
    Output(outputline);
    for (j=0; j<parsed_input->subject[i].part1.nr_of_adjectives; j++)
      PrintWord(parsed_input->subject[i].part1.adjectives[j]);
    sprintf(outputline, "\n noun: ");
    Output(outputline);
    PrintWord(parsed_input->subject[i].part1.noun);

    sprintf(outputline, "\n connecting preposition: ");
    Output(outputline);
    PrintWord(parsed_input->subject[i].connect_prepos);


    sprintf(outputline, "\nSubject %d - part 2\n ", i);
    Output(outputline);
    sprintf(outputline, " %d adjectives\n ", parsed_input->subject[i].part2.nr_of_adjectives);
    Output(outputline);
    for (j=0; j<parsed_input->subject[i].part2.nr_of_adjectives; j++)
      PrintWord(parsed_input->subject[i].part2.adjectives[j]);
    sprintf(outputline, "\n noun: ");
    Output(outputline);
    PrintWord(parsed_input->subject[i].part2.noun);
    sprintf(outputline, "\n");
    Output(outputline);
  }

  sprintf(outputline, "\nSpecifier - part 1:\n");
  Output(outputline);
  sprintf(outputline, " %d adjectives\n ", parsed_input->specifier.part1.nr_of_adjectives);
  Output(outputline);
  for (i=0; i<parsed_input->specifier.part1.nr_of_adjectives; i++)
    PrintWord(parsed_input->specifier.part1.adjectives[i]);
  sprintf(outputline, "\n noun: ");
  Output(outputline);
  PrintWord(parsed_input->specifier.part1.noun);

  sprintf(outputline, "\n connecting preposition: ");
  Output(outputline);
  PrintWord(parsed_input->specifier.connect_prepos);

  sprintf(outputline, "\nSpecifier - part 2:\n");
  Output(outputline);
  sprintf(outputline, " %d adjectives\n ", parsed_input->specifier.part2.nr_of_adjectives);
  Output(outputline);
  for (i=0; i<parsed_input->specifier.part2.nr_of_adjectives; i++)
    PrintWord(parsed_input->specifier.part2.adjectives[i]);
  sprintf(outputline, "\n noun: ");
  Output(outputline);
  PrintWord(parsed_input->specifier.part2.noun);

  sprintf(outputline, "\nPrepositions:\n");
  Output(outputline);
  sprintf(outputline, " %d prepositions\n ", parsed_input->prepositions.nr_of_prepositions);
  Output(outputline);
  for (i=0; i<parsed_input->prepositions.nr_of_prepositions; i++)
    PrintWord(parsed_input->prepositions.preposition[i]);

  sprintf(outputline, "\nValue:\n");
  Output(outputline);
  sprintf(outputline, " %d\n ", parsed_input->value);
  Output(outputline);

  sprintf(outputline, "\nOrdinal:\n");
  Output(outputline);
  sprintf(outputline, " %d\n ", parsed_input->ordinal);
  Output(outputline);

  sprintf(outputline, "\n**********************\n");
  Output(outputline);
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
      Output(outputline);
      return(OK);
    case CURR_LOC:
      sprintf(outputline, "<the current location>\n");
      Output(outputline);
      return(OK);
    case SUBJECT:
      sprintf(outputline, "<the current subject>\n");
      Output(outputline);
      return(OK);
    case ACTOR:
      sprintf(outputline, "<the current actor>\n");
      Output(outputline);
      return(OK);
    case SPECIFIER:
      sprintf(outputline, "<the current specifier>\n");
      Output(outputline);
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
  Output(outputline);

  sprintf(outputline, "\nAction: ");
  Output(outputline);
  if (action_rec->action == NO_ID) {
    sprintf(outputline, "<none>\n");
    Output(outputline);
  }
  else
    if (action_rec->action == DEFAULT) {
      sprintf(outputline, "DEFAULT");
      Output(outputline);
    }
    else
      PrintWord(action_rec->action);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nActor: ");
  Output(outputline);
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->actor))
    /* Not a special id. */
      PrintId(action_rec->actor);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nAdverbs:\n");
  Output(outputline);
  sprintf(outputline, "  %d adverbs\n ", action_rec->adverbs.nr_of_adverbs);
  Output(outputline);
  for (i=0;i<action_rec->adverbs.nr_of_adverbs;i++) {
    PrintWord(action_rec->adverbs.adverb[i]);
    sprintf(outputline, " ");
    Output(outputline);
  }
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nQ-word: ");
  Output(outputline);
  if (action_rec->q_word == NO_ID) {
    sprintf(outputline, "<none>");
    Output(outputline);
  }
  else
    PrintWord(action_rec->q_word);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nDirection: ");
  Output(outputline);
  if (action_rec->direction == NO_ID) {
    sprintf(outputline, "<none>");
    Output(outputline);
  }
  else {
    sprintf(outputline, "%d\n", action_rec->direction);
    Output(outputline);
  }

  sprintf(outputline, "\nSubject: %d ", action_rec->subject);
  Output(outputline);
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->subject))
    /* Not a special id. */
    PrintId(action_rec->subject);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nSpecifier: ");
  Output(outputline);
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->specifier))
    /* Not a special id. */
    PrintId(action_rec->specifier);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nPrepositions:\n");
  Output(outputline);
  sprintf(outputline, " %d prepositions\n ", action_rec->prepositions.nr_of_prepositions);
  Output(outputline);
  for (i=0; i<action_rec->prepositions.nr_of_prepositions; i++) {
    PrintWord(action_rec->prepositions.preposition[i]);
    sprintf(outputline, " ");
    Output(outputline);
  }
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nValue: %d\n", action_rec->value);
  Output(outputline);

  sprintf(outputline, "\nOrdinal: %d\n", action_rec->ordinal);
  Output(outputline);

  sprintf(outputline, "Trigger: %d %d\n", action_rec->execute[0], action_rec->execute[1]);
  Output(outputline);
  sprintf(outputline, "\n********************\n");
  Output(outputline);
}

void PrintUsrActionRec(action_rec)
 usrActionRec *action_rec;
{
  int32_t      i = 0;

  sprintf(outputline, "**** User Action Record ****\n");
  Output(outputline);

  sprintf(outputline, "\nAction: ");
  if (action_rec->action == NO_ID) {
    sprintf(outputline, "<none>\n");
    Output(outputline);
  }
  else
    if (action_rec->action == DEFAULT) {
      sprintf(outputline, "DEFAULT");
      Output(outputline);
    }
    else
      PrintWord(action_rec->action);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nActor: ");
  Output(outputline);
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->actor))
    /* Not a special id. */
      PrintId(action_rec->actor);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nAdverbs:\n");
  Output(outputline);
  sprintf(outputline, "  %d adverbs\n ", action_rec->adverbs.nr_of_adverbs);
  Output(outputline);
  for (i=0;i<action_rec->adverbs.nr_of_adverbs;i++) {
    PrintWord(action_rec->adverbs.adverb[i]);
    sprintf(outputline, " ");
    Output(outputline);
  }
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nQ-word: ");
  Output(outputline);
  if (action_rec->q_word == NO_ID) {
    sprintf(outputline, "<none>");
    Output(outputline);
  }
  else
    PrintWord(action_rec->q_word);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nDirection: ");
  Output(outputline);
  if (action_rec->direction == NO_ID) {
    sprintf(outputline, "<none>");
    Output(outputline);
  }
  else {
    sprintf(outputline, "%d\n", action_rec->direction);
    Output(outputline);
  }

  for (i=0; i<MAX_SUBJECTS; i++) {
    sprintf(outputline, "\nSubject: %d ", action_rec->subject[i]);
    Output(outputline);
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->subject[i]))
      /* Not a special id. */
      PrintId(action_rec->subject[i]);
    sprintf(outputline, "\n");
    Output(outputline);
  }

  sprintf(outputline, "\nSpecifier: ");
  Output(outputline);
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->specifier))
    /* Not a special id. */
    PrintId(action_rec->specifier);
  sprintf(outputline, "\n");
  Output(outputline);

  sprintf(outputline, "\nPrepositions:\n");
  Output(outputline);
  sprintf(outputline, " %d prepositions\n ", action_rec->prepositions.nr_of_prepositions);
  Output(outputline);
  for (i=0; i<action_rec->prepositions.nr_of_prepositions; i++) {
    PrintWord(action_rec->prepositions.preposition[i]);
    sprintf(outputline, " ");
    Output(outputline);
  }

  sprintf(outputline, "Value: %d\n ", action_rec->value);
  Output(outputline);

  sprintf(outputline, "Ordinal: %d\n ", action_rec->ordinal);
  Output(outputline);

  sprintf(outputline, "\n********************\n");
  Output(outputline);
}


void PrintParserActionRec(action_rec)
 parserActionRec *action_rec;
{
  int32_t      i = 0;

  while (action_rec != NULL) {
    PrintString("**** Parser Action Record ****\n");

    PrintString("\nAction: ");
    if (action_rec->action == NO_ID) {
      sprintf(outputline, "<none>\n");
      Output(outputline);
    }
    else
      if (action_rec->action == DEFAULT) {
        sprintf(outputline, "DEFAULT");
        Output(outputline);
      }
      else
        PrintWord(action_rec->action);
    sprintf(outputline, "\n");
    Output(outputline);

    sprintf(outputline, "\nActor: ");
    Output(outputline);
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->actor))
      /* Not a special id. */
        PrintId(action_rec->actor);
    sprintf(outputline, "\n");
    Output(outputline);

    sprintf(outputline, "\nAdverbs:\n");
    Output(outputline);
    sprintf(outputline, "  %d adverbs\n ", action_rec->adverbs.nr_of_adverbs);
    Output(outputline);
    for (i=0;i<action_rec->adverbs.nr_of_adverbs;i++) {
      PrintWord(action_rec->adverbs.adverb[i]);
      sprintf(outputline, " ");
      Output(outputline);
    }
    sprintf(outputline, "\n");
    Output(outputline);

    sprintf(outputline, "\nQ-word: ");
    Output(outputline);
    if (action_rec->q_word == NO_ID) {
      sprintf(outputline, "<none>");
      Output(outputline);
    }
    else
      PrintWord(action_rec->q_word);
    sprintf(outputline, "\n");
    Output(outputline);

    sprintf(outputline, "\nDirection: ");
    Output(outputline);
    if (action_rec->direction == NO_ID) {
      sprintf(outputline, "<none>");
      Output(outputline);
    }
    else {
      sprintf(outputline, "%d\n", action_rec->direction);
      Output(outputline);
    }

    sprintf(outputline, "\nSubject: ");
    Output(outputline);
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->subject))
      /* Not a special id. */
      PrintId(action_rec->subject);
    else {
      sprintf(outputline, "special id: %d", action_rec->subject);
      Output(outputline);
    }
    sprintf(outputline, "\n");
    Output(outputline);

    sprintf(outputline, "\nSpecifier: ");
    Output(outputline);
    /* Test for special ids. */
    if (!Test4SpecId(action_rec->specifier))
      /* Not a special id. */
      PrintId(action_rec->specifier);
    sprintf(outputline, "\n");
    Output(outputline);

    sprintf(outputline, "\nPrepositions:\n");
    Output(outputline);
    sprintf(outputline, " %d prepositions\n ", action_rec->prepositions.nr_of_prepositions);
    Output(outputline);
    for (i=0; i<action_rec->prepositions.nr_of_prepositions; i++) {
      PrintWord(action_rec->prepositions.preposition[i]);
      sprintf(outputline, " ");
      Output(outputline);
    }

    sprintf(outputline, "\nValue: %d\n ", action_rec->value);
    Output(outputline);

    sprintf(outputline, "Ordinal: %d\n ", action_rec->ordinal);
    Output(outputline);

    sprintf(outputline, "Score: %d\n ", action_rec->score);
    Output(outputline);

    sprintf(outputline, "\n********************\n");
    Output(outputline);

    action_rec = action_rec->next;
  }
}

void    PrintHits(hits, scope, target)
 match   *hits;
 int32_t scope;
 int32_t target;
{
  int i = 0;

  PrintString("\n********* Hits *********\n\n");
  PrintString("Hits for ");
  switch (scope) {
    case ALL_LOCS:
      PrintString("All locations, ");
      break;
    case ACTOR_ONLY:
      PrintString("Actor only, ");
      break;
    case CURR_LOC_ONLY:
      PrintString("Current location, ");
      break;
    default:
      PrintString("<unknown scope>, ");
      break;
  }

  switch (target) {
    case ACTOR:
      PrintString("Actor.\n");
      break;
    case SUBJECT:
      PrintString("Subject.\n");
      break;
    case SPECIFIER:
      PrintString("Specifier.\n");
      break;
    default:
      PrintString("<unknown target>\n");
      break;
  }

  for (i=0; i<hits->nr_of_hits; i++) {
    PrintId(hits->matched_objs[i]);
    PrintString("\n");
  }

  PrintString("\n");
  PrintString("\n************************\n\n");
}

void PrintAllTimers(void)
{
  int32_t i = 0;

  for (i=0; i<nr_of_timers; i++)
    PrintTimer(timers+i);
  sprintf(outputline, "\n\n");
  Output(outputline);
}

void PrintTimer(t)
 timerInfo *t;
{
  sprintf(outputline, "**********Timer info**********\n\n");
  Output(outputline);

  sprintf(outputline, "value: %d\n", t->value);
  Output(outputline);
  sprintf(outputline, "step: %d\n", t->step);
  Output(outputline);
  sprintf(outputline, "interval: %d\n", t->interval);
  Output(outputline);
  sprintf(outputline, "update: %d\n", t->update);
  Output(outputline);
  sprintf(outputline, "direction: %d\n", t->direction);
  Output(outputline);
  sprintf(outputline, "state: %d\n", t->state);
  Output(outputline);
  sprintf(outputline, "trigger_at: %d\n", t->trigger_at);
  Output(outputline);
  sprintf(outputline, "trigger_spec: %d\n", t->trigger_spec);
  Output(outputline);
  sprintf(outputline, "owner execute: %d\n", t->execute[0]);
  Output(outputline);
  sprintf(outputline, "id execute: %d\n", t->execute[1]);
  Output(outputline);

  sprintf(outputline, "\n******************************\n");
  Output(outputline);
}

void PrintSpanTree(tree)
 spanTree *tree;
{
  int i = 0;

  PrintString("****** Spanning Tree ******\n\n");

  for (i=0; i<nr_of_locs; i++) {
    PrintString("Location: ");
    PrintId(i+FIRST_LOCATION_ID);
    PrintString(":\n from_loc: ");
    PrintId(tree[i].from_loc);
    PrintString("\n from_dir: ");
    PrintId(tree[i].from_dir);
    PrintString("\n level: ");
    PrintNumber(tree[i].level);
    PrintString("\n\n");
  }
  PrintString("***************************\n\n");
}

void PrintRoute(route)
 int32_t *route;
{
  int nr_of_hops = 0;
  int i = 0;

  PrintString("****** Route ******\n\n");
  /* find starting location */
  while (route[nr_of_hops] != NO_ID)
    nr_of_hops++;

     for (i=nr_of_hops-1; i>=0; i--) {
       PrintId(route[i]);
      PrintString(", ");
     }
     PrintString("\n");
  PrintString("*******************\n\n");
}
