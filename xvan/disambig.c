
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
#include <stdlib.h>  /* malloc() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "disambig.h"

/************************/
/*        Globals       */
/************************/

int32_t disambig_score = 0;

/*************************/
/* Function declarations */
/*************************/

void            FillDefaultValues(parsedInput*, parserActionRec*);
parserActionRec *CreateParserRecs(parsedInput*, match*, match*, match*);
void            FreeParserRecs(parserActionRec*);
int32_t         AlreadyHas(match*, int32_t);
void            RebuildActorHits(parserActionRec*, match*, int32_t);
void            RebuildSubjectHits(parserActionRec*, match*, int32_t);
void            RebuildSpecifierHits(parserActionRec*, match*, int32_t);
int32_t         *FindParserRules(parserActionRec*);
int32_t         XeqParserRules(parserActionRec*,int32_t*, int32_t*);
int32_t         ApplyParserRules(parsedInput*, match*, match*, match*);

/************************/
/* Function definitions */
/************************/

void FillDefaultValues(source, target)
 parsedInput     *source;
 parserActionRec *target;
{
  int i = 0;

  /* set actor, subject, specifier to default */
  target->actor     = PLAYER;  /* there always is an actor */
  target->subject   = NO_ID;
  target->specifier = NO_ID;

  /* copy fixed values form compActionRec */
  /* to parserActionRec                   */

  target->action1 = source->action1;
  target->action2 = source->action2;
  target->adverbs.nr_of_adverbs = source->adverbs.nr_of_adverbs;

  for (i=0; i<source->adverbs.nr_of_adverbs; i++) {
    target->adverbs.adverb[i] = source->adverbs.adverb[i];
  }

  target->q_word = source->q_word;
  target->direction = source->direction;
  target->prepositions.nr_of_prepositions = source->prepositions.nr_of_prepositions;

  for (i=0; i<source->prepositions.nr_of_prepositions; i++) {
    target->prepositions.preposition[i] = source->prepositions.preposition[i];
  }

  target->value = source->value;
  target->ordinal = source->ordinal;

  /* set score in target */
  target->score = DEFAULT_PARSER_SCORE;
}


void FreeParserRecs(start)
 parserActionRec *start;
{
  if (!start) {
    FreeParserRecs(start->next);
    free(start);
  }
}


parserActionRec *CreateParserRecs(p_i, actor_hits, subject_hits, specifier_hits)
 parsedInput *p_i;
 match       *actor_hits;
 match       *subject_hits;
 match       *specifier_hits;
{
  int i = 0;
  int j = 0;
  int k = 0;
  int l = 0;

  int total_recs      = 1;
  int actor_freq      = 0;
  int subject_freq    = 0;
  int specifier_freq  = 0;
  int actor_count     = 0;
  int subject_count   = 0;
  int specifier_count = 0;

  parserActionRec *start   = NULL;
  parserActionRec *current = NULL;

  if ((start = (parserActionRec*) malloc(sizeof(parserActionRec))) == NULL) {
    PrintError(15, NULL, "ApplyParserRules()");
    return(NULL);
  }

  /* fill fixed values in parser action record */
  FillDefaultValues(p_i, start);

  start->next = NULL;
  current     = start;

  /* calculate total action records to be created, taking into account 0 hits */
  total_recs = total_recs * (actor_hits->nr_of_hits < 1 ? 1 : actor_hits->nr_of_hits);
  total_recs = total_recs * (subject_hits->nr_of_hits < 1 ? 1 : subject_hits->nr_of_hits);
  total_recs = total_recs * (specifier_hits->nr_of_hits < 1 ? 1 : specifier_hits->nr_of_hits);

  if (actor_hits->nr_of_hits*subject_hits->nr_of_hits*specifier_hits->nr_of_hits > MAX_AMBIGUITY) {
    free(start);
    return(NULL);
  }

  /* The following code can be written more compact but it's all new and we */
  /* want to fully understand the concept. Optimization we can do later.    */

  /* create empty set of user action records             */
  /* we already created start, but we must also create a */
  /* record for no_such_thing, su use i rather than i-1  */
  for (i=0; i<total_recs; i++) {
    if ((current->next = (parserActionRec*) malloc(sizeof(parserActionRec))) == NULL) {
      PrintError(15, NULL, "CreateParserRecs()");
      FreeParserRecs(start);
      return(NULL);
    }
    current = current->next;
    FillDefaultValues(p_i, current);
  }
  current->next = NULL;

  /* Now fill the no_such_thing record. no_such_thing is */
  /* to prevent situations like 'drop cube' when there   */
  /* are only cubes that the player is not holding.      */
  /* if either actor, subject or specifier has 1 hit,    */
  /* update the no_such_thing action record with this    */
  /* value.                                              */

  current->actor     = (actor_hits->nr_of_hits == 1 ?
                        actor_hits->matched_objs[0] : NO_SUCH_THING);
  current->subject   = (subject_hits->nr_of_hits == 1 ?
                        subject_hits->matched_objs[0] :NO_SUCH_THING);
  current->specifier = (specifier_hits->nr_of_hits == 1 ?
                        specifier_hits->matched_objs[0] : NO_SUCH_THING);
  /* give a score of 1 above the default value */
  current->score     = DEFAULT_PARSER_SCORE+1;

  /* correct frequencies for -1 and 0 */
  actor_freq     = (actor_hits->nr_of_hits < 1 ? 1 : actor_hits->nr_of_hits);
  subject_freq   = (subject_hits->nr_of_hits < 1 ? 1 : subject_hits->nr_of_hits);
  specifier_freq = (specifier_hits->nr_of_hits < 1 ? 1 : specifier_hits->nr_of_hits);

  actor_count     = total_recs/actor_freq;
  subject_count   = actor_count/subject_freq;
  specifier_count = subject_count/specifier_freq;

  /* now fill the action records */

  /* actor */
  current = start;
  for (i=0; i<actor_hits->nr_of_hits; i++) {
    for (j=0; j<actor_count; j++) {
      current->actor = actor_hits->matched_objs[i];
      current = current->next;
    }
  }

  /* subject */
  current = start;
  for (k=0; k<actor_freq; k++) {
    for (i=0; i<subject_hits->nr_of_hits; i++) {
      for (j=0; j<subject_count; j++) {
        current->subject = subject_hits->matched_objs[i];
        current = current->next;
      }
    }
  }

  /* specifier */
  current = start;
  for (k=0; k<actor_freq; k++) {
    for (i=0; i<subject_freq; i++) {
      for (j=0; j<specifier_hits->nr_of_hits; j++) {
        for (l=0; l<specifier_count; l++) {
          current->specifier = specifier_hits->matched_objs[j];
          current = current->next;
        }
      }
    }
  }

  return(start);
}


int32_t CompareActorSubjectSpecifier(verb, parser)
 int32_t verb;
 int32_t parser;
{
  /* Note: here can be wildcards (ACTOR, SUBJECT, */
  /* SPECIFIER) in verb, but not in parser.       */

  /* Example of output */

/*           |        parser         */
/*           | NO_ID o_test1 o_test2 */
/*   verb    | --------------------- */
/*  NO_ID    |  V      X       X     */
/*  wildcard |  X      V       V     */
/*  o_test1  |  X      V       V     */
/*  o_test2  |  X      V       V     */
/*            ---------------------  */

  if ( (verb != NO_ID && parser != NO_ID) || (verb == parser) )
    return(OK);
  else
    return(ERROR);
}


int32_t MatchParserRec(verb, parser)
 compActionRec   *verb;
 parserActionRec *parser; /* looking for this one */
{
  int32_t i     = 0;
  int32_t j     = 0;
  int32_t found = 0; /* Initially not found. */

  /* This function compares the verb action record with the  */
  /* parser action record. It does not compare actor,        */
  /* subject and specifier values, other than NO_ID (which   */
  /* indicates absence in the input chain record.            */

  if (!CompareActorSubjectSpecifier(verb->actor, parser->actor))
    return(ERROR);

  if (verb->action1 != parser->action1 &&
      (verb->action1 != ACTION || parser->action1 == NO_ID))
    return(ERROR);

  if (verb->q_word != parser->q_word)
    return(ERROR);

  if (verb->direction != parser->direction &&
      (verb->direction != DIR || parser->direction == NO_ID))
    return(ERROR);

  if (!CompareActorSubjectSpecifier(verb->subject, parser->subject))
    return(ERROR);

  if (!CompareActorSubjectSpecifier(verb->specifier, parser->specifier))
    return(ERROR);

  /* Test values. */
  if (verb->value != parser->value && verb->value != VALUE)
    return(ERROR);

  if (verb->ordinal != parser->ordinal && verb->ordinal != ORDINAL)
    return(ERROR);

  /* Test adverbs. */
  /* No need to test for doubles here, like in MatchSysDescr() in */
  /* module trnslate.c. We want a perfect match here (subsets are */
  /* rejected), so doubles will automatically be rejected.        */

  i     = 0;
  j     = 0;
  found = 0;

  if ((verb->adverbs).nr_of_adverbs == (parser->adverbs).nr_of_adverbs) {
    while (i<(verb->adverbs).nr_of_adverbs) {
      while (!found && j<(parser->adverbs).nr_of_adverbs) {
        if ((verb->adverbs).adverb[i] == (parser->adverbs).adverb[j])
          found = 1;
        else
          j++;
      } /* while */
      if (j == (parser->adverbs).nr_of_adverbs)
        /* Not found. */
        return(ERROR);

      /* Reset j, found for next round. */
      j     = 0;
      found = 0;
      i++;
    } /* while */
  } /* if */
  else
    return(ERROR);

  /* Test prepositions. */
  i     = 0;
  j     = 0;
  found = 0;

  if ((verb->prepositions).nr_of_prepositions ==
      (parser->prepositions).nr_of_prepositions) {
    while (i<(verb->prepositions).nr_of_prepositions) {
      while (!found && j<(parser->prepositions).nr_of_prepositions) {
        if (verb->prepositions.preposition[i] == PREPOS ||
            (verb->prepositions).preposition[i] ==
            (parser->prepositions).preposition[j])
          found = 1;
        else
          j++;
      } /* while */
      if (j == (parser->prepositions).nr_of_prepositions)
        /* Not found. */
        return(ERROR);

      /* Reset j, found for next round. */
      j     = 0;
      found = 0;
      i++;
    } /* while */
  } /* if */
  else
    return(ERROR);

  /* OK, it's a match. */
  return(OK);
}


int32_t AlreadyHas(hits, item)
 match   *hits;
 int32_t item;
{
  int i = 0;

  for (i=0; i<hits->nr_of_hits; i++) {
    if (hits->matched_objs[i] == item)
      return(OK);
  }
  return(ERROR);
}


void RebuildActorHits(parser_recs, actor_hits, max_score)
 parserActionRec *parser_recs;
 match           *actor_hits;
 int32_t         max_score;
{
  parserActionRec *current = parser_recs;

  if (actor_hits->nr_of_hits >= 0) {
    /* reset number of hits */
    actor_hits->nr_of_hits = 0;

    while (current != NULL) {
      if (current->score == max_score) {
        /* check for NO_ID because we don't want these in the hits */
        if (current->actor != NO_ID) {
          /* check if we already have this hit in the list */
          if (!AlreadyHas(actor_hits, current->actor)) {
            actor_hits->matched_objs[actor_hits->nr_of_hits++] = current->actor;
          }
        }
      }
      current = current->next;
    }
  }
}


void RebuildSubjectHits(parser_recs, subject_hits, max_score)
 parserActionRec *parser_recs;
 match           *subject_hits;
 int32_t         max_score;
{
  parserActionRec *current = parser_recs;

  if (subject_hits->nr_of_hits >= 0) {
    /* reset number of hits */
    subject_hits->nr_of_hits = 0;

    while (current != NULL) {
      if (current->score == max_score) {
        /* check for NO_ID because we don't want these in the hits */
        if (current->subject != NO_ID) {
          /* check if we already have this hit in the list */
          if (!AlreadyHas(subject_hits, current->subject)) {
            subject_hits->matched_objs[subject_hits->nr_of_hits++] = current->subject;
          }
        }
      }
      current = current->next;
    }
  }
}


void RebuildSpecifierHits(parser_recs, specifier_hits, max_score)
 parserActionRec *parser_recs;
 match           *specifier_hits;
 int32_t         max_score;
{
  parserActionRec *current = parser_recs;

  if (specifier_hits->nr_of_hits >= 0) {
    /* reset number of hits */
    specifier_hits->nr_of_hits = 0;

    while (current != NULL) {
      if (current->score == max_score) {
        /* check for NO_ID because we don't want these in the hits */
        if (current->specifier != NO_ID) {
          /* check if we already have this hit in the list */
          if (!AlreadyHas(specifier_hits, current->specifier)) {
            specifier_hits->matched_objs[specifier_hits->nr_of_hits++] = current->specifier;
          }
        }
      }
      current = current->next;
    }
  }
}


int32_t *FindParserRules(parser_recs)
 parserActionRec *parser_recs;
{
  verbInfo      *verb  = NULL;
  int32_t       cont   = 1;
  int32_t       *result = NULL; /* no malloc needed */
  compActionRec *arec  = NULL;

  /* check to see if the verb we need is already in memory */
  if (!InMem(parser_recs->action1))
    switch (Load(parser_recs->action1)) {
      case ERROR:
        PrintError(42, NULL, NULL);
        return(NULL);
      case NO_MATCH: /* printf("Load gives no_match\n"); */
        return(NULL);
      default: /* printf("Load gives ok\n"); */
        /* OK */
        break;
    } /* switch */

  verb = verbs[(parser_recs->action1)-FIRST_VERB_ID];

  /* Try to find a matching action record within verbInfo. */
  /* we need only to match the first parser action record, */
  /* because they are all of the same structure, only the  */
  /* values of actor, subject and specifier will vary.     */

  /*                 |                                          */
  /*        verbInfo | --> compActionRec --> CompActionRec .... */
  /*                 |                                          */
  /*                 | --> integer array (trigger code)         */
  /*                 |                                          */
  /*                 | --> integer array (parser rules)         */
  /*        ----------                                          */
  /*            |                                               */
  /*            V                                               */
  /*        ----------                                          */
  /*        verbInfo |                                          */

  while (cont && verb != NULL) {
    arec = verb->action_rec;
    while (cont && arec != NULL) {
      if (arec->action1 == DEFAULT || MatchParserRec(arec, parser_recs)) {
        result = verb->parser_rules;
        cont = 0;
      } /* if */
      arec = arec->next;
    } /* while */
    verb = verb->next;
  } /* while */

  return(result);
}


int32_t XeqParserRules(parser_recs, rules, max_score)
 parserActionRec *parser_recs;
 int32_t         *rules;
 int32_t         *max_score;
{
  int32_t winners   = 1;

  /* This routine executes the parser rules for the             */
  /* parser_recs action. Purpose of this function is to narrow  */
  /* down the number of hits before we start asking the player  */
  /* what he means.                                             */

  if (rules == NULL) {
    /* there are no parser rules */
    /* everybody is a winner     */

    /* count the parser_recs     */
    while ((parser_recs = parser_recs->next) != NULL)
      winners++;
  }
  else {
    /* update score for each parser rec */
    /* parser_recs var cannot be NULL   */
    do {
      /* load all special ids with info from the action record */
      /* ACTION is always action1 (the main verb)              */
      action    = parser_recs->action1;
      actor     = parser_recs->actor;
      subject   = parser_recs->subject;
      specifier = parser_recs->specifier;

      /* no need for action_rec and subject_index here */

      /* reset disambig_score var */
      disambig_score = 0;
      Execute(rules, NULL, -1, NULL);
      parser_recs->score += disambig_score;

      /* update max_score and winners */
      if (parser_recs->score > *max_score) {
        *max_score = parser_recs->score;
        winners = 1;
      }
      else {
        if (parser_recs->score == *max_score) {
          winners++;
        }
      }

      parser_recs = parser_recs->next;
    } while (parser_recs != NULL);

    return(winners);
  }

  /* Ready */
  return(winners);
}


int32_t ApplyParserRules(p_i, actor_hits, subject_hits, specifier_hits)
 parsedInput *p_i;
 match       *actor_hits;
 match       *subject_hits;
 match       *specifier_hits;
{
  parserActionRec *parser_recs  = NULL;
  int32_t         *parser_rules = NULL; /* is actually a trigger */
  int32_t         max_score = 0;
  int32_t         winners = 0;

  /* create linked list with parser records for all possible */
  /* actor, subject, specifier hit combinations              */
  if ((parser_recs = CreateParserRecs(p_i, actor_hits, subject_hits,
                                      specifier_hits)) == NULL) {
    /* return too many hits so caller will continue */
    return(MAX_AMBIGUITY+1);
  }

  /* look up the parser rules */
  parser_rules = FindParserRules(parser_recs);

  /* execute the parser rules for all parser records */
  /* in the linked list and hope for a winner        */
  winners = XeqParserRules(parser_recs, parser_rules, &max_score);

  /* now rebuild the hits, in case caller wants to further process */
  RebuildActorHits(parser_recs, actor_hits, max_score);
  RebuildSubjectHits(parser_recs, subject_hits, max_score);
  RebuildSpecifierHits(parser_recs, specifier_hits, max_score);

  FreeParserRecs(parser_recs);
  return(winners);
}
