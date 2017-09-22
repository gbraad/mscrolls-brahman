
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

int32_t Test4SpecId(int32_t);
void    PrintActionRec(actionRec*);
void    PrintWTable(void);

/************************/
/* Function definitions */
/************************/


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


void PrintActionrec(action_rec)
 actionRec *action_rec;
{
  int32_t      i = 0;

  printf("**** Action Record ****\n");
printf("In PrintActionrec aangekomen\n");
  printf("\nAction: ");
  if (action_rec->action == NO_ID)
    printf("<none>\n");
  else
    printf("%d", action_rec->action);
  printf("\n");

  printf("\nActor: ");
  /* Test for special ids. */
  if (!Test4SpecId(action_rec->actor))
    /* Not a special id. */
    printf("%d", action_rec->actor);
  printf("\n");

  printf("\nAdverbs:\n");
  printf("  %d adverbs\n ", action_rec->adverbs.nr_of_adverbs);
  for (i=0;i<action_rec->adverbs.nr_of_adverbs;i++) {
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

  printf("\nDirection: %d", action_rec->direction);
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
  for (i=0; i<action_rec->prepositions.nr_of_prepositions; i++) {
    printf("%d", action_rec->prepositions.preposition[i]);
    printf(" ");
  }
  printf("\n");

  printf("\n********************\n");
}

void PrintWTable()
{
  int32_t i,j = 0;

  printf("*** Word table ***\n");

  for (i=0; i<nr_of_words; i++) {
    printf("\n%d: %s | ", word_table[i].id, word_table[i].word);
    for (j=0; j<MAX_TYPES; j++) {
      printf("%d ", word_table[i].types[j]);
    }
  }
}
