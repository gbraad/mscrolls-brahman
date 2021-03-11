
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
#include <stdlib.h>   /* malloc() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "trnslate.h"

/*************************/
/* Function declarations */
/*************************/

int32_t      StringToNum(char*, int32_t*);
void         InitUsrActionrec(usrActionRec*);
int32_t      FilledOut(extendedSysDescr*);
int32_t      CountSubjectsInParsedInput(parsedInput*);
int32_t      HasType(int32_t, int32_t);
int32_t      ConvertDynamicDSys(char*, extendedSysDescr*);
int32_t      FlipNoun(sysDescr*);
int32_t      SplitSubject(parsedInput*, int32_t);
int32_t      SplitSubjectAndSpecifier(parsedInput*, int32_t);
int32_t      CompareNounsAndAdjectives(sysDescr, int32_t, sysDescr);  /* @!@ */
resultStruct MatchSysDescr(extendedSysDescr, int32_t, extendedSysDescr);   /* @!@ */
void         SwapSysDescr(extendedSysDescr*, extendedSysDescr*);
resultStruct HasMatchingSysDescr(int32_t, extendedSysDescr, int32_t);   /* @!@ */
int32_t      Search(int32_t, extendedSysDescr*, int32_t, int32_t, int32_t, match*);  /* @!@ */
int32_t      ArrangeVisible(match*);
void         MoreInfo(extendedSysDescr*, match*, char*);
int32_t      SearchHits(extendedSysDescr*, int32_t, int32_t, int32_t*, match*, char*);
int32_t      Find(extendedSysDescr*, int32_t, int32_t, char*, match*);  /* @!@ */
void         PrintNotFound(extendedSysDescr*);
resultStruct Translate(parsedInput*, int32_t, usrActionRec*, char*);

/************************/
/* Function definitions */
/************************/

int32_t StringToNum(char *source, int32_t *num)
{
  int32_t i     = 0;
  int32_t start = 0;  /* start value for index. */
  int32_t sign  = 1;  /* For sign of num.       */

  switch (source[0]) {
    case '-':
      sign  = -1;
      start = 1;
      break;
    case '+':
      start = 1;
      break;
    default:
      /* Do nothing. */
      break;
  }

  /* reset num */
  *num = 0;

  for (i=start;(source[i]>='0') && (source[i]<='9');i++)
    *num = 10*(*num)+source[i]-'0';

  if (source[i] != '\0')
    return(ERROR);

  /* Make right sign. */
  *num *= sign;

  return(OK);
}


void InitUsrActionRec(usrActionRec *usr_action_rec)
{
  int32_t i = 0;

  /* Default actor is the player. */

  usr_action_rec->actor                           = PLAYER;
  usr_action_rec->action1                         = NO_ID;
  usr_action_rec->action2                         = NO_ID;
  usr_action_rec->adverbs.nr_of_adverbs           = 0;
  usr_action_rec->q_word                          = NO_ID;
  usr_action_rec->direction                       = NO_ID;

  for (i=0; i<MAX_SUBJECTS; i++) {   /* @!@ */
    usr_action_rec->subject[i]         = NO_ID;
    usr_action_rec->plural_subjects[i] = NO_ID;
  }

  usr_action_rec->specifier                       = NO_ID;
  usr_action_rec->prepositions.nr_of_prepositions = 0;
  usr_action_rec->value                           = 0;
  usr_action_rec->ordinal                         = 0;
  usr_action_rec->next                            = NULL;
}


int32_t CountSubjectsInParsedInput(parsedInput *parsed_input)
{
  int count = 0;

  while (count<MAX_SUBJECTS && FilledOut(&(parsed_input->subject[count]))) {
    count++;
  }

  return(count);
}


int32_t FilledOut(extendedSysDescr *descr)
{
  /* This routine checks for noun value NO_ID and zero */
  /* adjectives in descr->part1. If so, then descr is  */
  /* unused. Article is not checked                    */

  if (((descr->part1).noun == NO_ID) && ((descr->part1).nr_of_adjectives == 0))
    /* Not filled out. */
    return(ERROR);
  else
    return(OK);
}


int32_t HasType(int32_t word_id, int32_t type)
{
  int i     = 0;
  int j     = 0;
  int found = 0;

  /* Find word_id. */
  while (i<nr_of_words && word_table[i].id != word_id) {
    i++;
  }

  if (i == nr_of_words) {
    /* word not found */
    return(ERROR);
  }

  /* check if word has the requested type */
  while (j<MAX_TYPES && !found) {
    if (word_table[i].types[j] == type) {
      found = 1;
    }
    j++;
  }

  return(found);
}


int32_t ConvertDynamicDSys(char *dyn_text, extendedSysDescr *descr)
{
  char    *descr_text;
  int32_t result = OK;
  int32_t old_article;
  int32_t old_capital;
  int     len;

  /* this functions converts a dynamic system description text */
  /* string to an extendedSysDescr struct                      */
  /* we can also just print descr->dynamic, but by converting  */
  /* to a struct we can handle the article correctly           */

  /* initialize everything but ->dynamic from *descr */
  descr->part1.article          = NO_ID;
  descr->part1.nr_of_adjectives = 0;
  descr->part1.noun             = NO_ID;
  descr->connect_prepos         = NO_ID;
  descr->part2.article          = NO_ID;
  descr->part2.nr_of_adjectives = 0;
  descr->part2.noun             = NO_ID;

  /* flush the outputline */
  Output();

  /* convert dynamic d_sys to text        */
  /* we abuse PrintString here to convert */
  /* a string with parameters, so we must */
  /* save some control values             */

  old_article    = article;
  old_capital    = capital;
  article        = 0;

  /* prevent \n in printed dyn_text     */
  /* (will cause an unknown word error  */
  /* from ParseDSys()                   */
  /* if this becomes an issue with very */
  /* long dynamic descriptions, make a  */
  /* dedicated PrintString() to convert */
  /* dynamic system descriptions        */

  PrintString(dyn_text, 0);

  capital = old_capital;
  article = old_article;

  len = strlen(outputline);
  if (len == 0) {
    /* empty d_sys */
    return(OK);
  }

  if ((descr_text = (char *) malloc((len+1)*sizeof(char))) == NULL) {
    PrintError(15, NULL, "ConvertDynamicDSys()");
    return(ERROR);
  }

  strncpy(descr_text, outputline, len);
  descr_text[len] = '\0';
  outputline = ResetString(outputline);

  /* now parse the system description text */
  result = ParseDSys(descr_text, descr);

  free(descr_text);
  return(result);
}


int32_t FlipNoun(sysDescr *descr)
{
  int i = 0;

  /* if descr has a noun, check if ths noun word also has type */
  /* adjective. If so, add the noun to the adjectives and set  */
  /* the noun field to NO_ID.                                  */

  if (descr->noun == NO_ID) {
    return(ERROR);
  }

  if (HasType(descr->noun, ADJECTIVES)) {
    /* check if there is room for an addtl adjective */
    if (descr->nr_of_adjectives == MAX_PARSE_ADJ) {
      return(ERROR);
    }
    for (i=descr->nr_of_adjectives-1; i>=0; i--) {
      descr->adjectives[i+1] = descr->adjectives[i];
    }
    descr->adjectives[0] = descr->noun;
    descr->nr_of_adjectives++;
    descr->noun          = NO_ID;
  }
  else {
    return(ERROR);
  }

  return(OK);
}


int32_t SplitSubject(parsedInput *p_i, int32_t index)
{
  int32_t i = 0;

  /* This function splits up a subject with a connecting      */
  /* preposition in a subject, a specifier and a preposition. */
  /* E.g. "hit (man with hammer)" becomes "hit (man) with     */
  /* (hammer)".                                               */

  if (p_i->subject[index].connect_prepos == NO_ID) {
    /* There is no connecting preposition */
    return(ERROR);
  }

  /* check if there is room for one more preposition */
  if (p_i->prepositions.nr_of_prepositions == MAX_PARSE_PREPOS-1) {
    /* no room for extra preposition */
    return(ERROR);
  }

  /* move connecting preposition */
  p_i->prepositions.preposition[p_i->prepositions.nr_of_prepositions++] =
              p_i->subject[index].connect_prepos;
  p_i->subject[index].connect_prepos = NO_ID;

  /* move article */
  p_i->specifier.part1.article = p_i->subject[index].part2.article;
  p_i->subject[index].part2.article = NO_ID;

  /* move adjectives */
  for (i=0; i<p_i->subject[index].part2.nr_of_adjectives; i++) {
    p_i->specifier.part1.adjectives[p_i->specifier.part1.nr_of_adjectives++] =
              p_i->subject[index].part2.adjectives[i];
  }
  p_i->subject[index].part2.nr_of_adjectives = 0;

  /* move noun */
  p_i->specifier.part1.noun = p_i->subject[index].part2.noun;
  p_i->subject[index].part2.noun = NO_ID;

  return(OK);
}


int32_t SplitSubjectAndSpecifier(parsedInput *p_i, int32_t index)
{
  int32_t i                      = 0;
  int32_t old_prepos_from_struct = NO_ID;

  /* Splits up subject with a connecting preposition and     */
  /* and specifier in a sentence.                            */
  /* E.g. "put bowl in passage with dripping water" will     */
  /* initially be parsed as:                                 */
  /* "put (bowl in passage) with (dripping water)", but will */
  /* on second try be parsed as:                             */
  /* "put (bowl) in (passage with dripping water)"           */

  /* function requires that:                         */
  /* - subject[index] has a connecting preposition   */
  /* - parsed_input struct has 1 preposition         */
  /* - specifier has no connecting preposition       */

  /* function will: */
  /* - move connecting preposition from subject[i]   */
  /*   to parsed_input struct preposition            */
  /* - move old preposition from parsed_input struct */
  /*   to connecting preposition from specifier      */
  /* - move subject part 2 noun and adjectives to    */
  /*   specifier part 1 noun and adjectives          */
  /* - move specifier part 1 noun and adjectives to  */
  /*   specifier part 2 noun and adjectives          */

  /* check the requirements */
  if (p_i->subject[index].connect_prepos == NO_ID) {
    return(ERROR);
  }

  if (p_i->prepositions.nr_of_prepositions != 1) {
    return(ERROR);
  }

  if (p_i->specifier.connect_prepos != NO_ID) {
    return(ERROR);
  }

  /* backup preposition from parsed_input struct */
  old_prepos_from_struct = p_i->prepositions.preposition[0];

  /* specifier part 2 is empty, so start there.        */
  /* move the adjectives from specifier part1 -> part2 */
  for (i=0; i<p_i->specifier.part1.nr_of_adjectives; i++) {
    p_i->specifier.part2.adjectives[i] = p_i->specifier.part1.adjectives[i];
    p_i->specifier.part1.adjectives[i] = NO_ID;
  }
  p_i->specifier.part2.nr_of_adjectives = p_i->specifier.part1.nr_of_adjectives;
  p_i->specifier.part1.nr_of_adjectives = 0;
  /* move the noun from specifier part1 -> part2 */
  p_i->specifier.part2.noun = p_i->specifier.part1.noun;
  p_i->specifier.part1.noun = NO_ID;

  /* now move subject part 2 noun and adjectives to */
  /* specifier part 1 noun and adjectives           */
  for (i=0; i<p_i->subject[index].part2.nr_of_adjectives; i++) {
    p_i->specifier.part1.adjectives[i] = p_i->subject[index].part2.adjectives[i];
    p_i->subject[index].part2.adjectives[i] = NO_ID;
  }
  p_i->specifier.part1.nr_of_adjectives = p_i->subject[index].part2.nr_of_adjectives;
  p_i->subject[index].part2.nr_of_adjectives = 0;
  /* move the noun from specifier part1 -> part2 */
  p_i->specifier.part1.noun = p_i->subject[index].part2.noun;
  p_i->subject[index].part2.noun = NO_ID;

  /* move connecting preposition from subject[index] */
  p_i->prepositions.preposition[0]   = p_i->subject[index].connect_prepos;
  p_i->subject[index].connect_prepos = NO_ID;


  /* move old preposition from parsed_input struct */
  p_i->specifier.connect_prepos = old_prepos_from_struct;

  return(OK);
}


int32_t CompareNounsAndAdjectives(sysDescr source, int32_t single_id, sysDescr target)  /* @!@ */
{
  /* source comes from user input            */  /* @!@ */
  /* single id is single form of source noun */  /* @!@ */

  /* 01oct2019 if single_id has value NO_ID then the source   */  /* @!@ */
  /* sysDescr is in singular form. If single_id has another   */
  /* value (can be the same value as the source descr noun),  */
  /* then source is in plural form and we must check with the */
  /* single_id and not the source descr noun                  */

  /* why can single_id have the same value as the descr noun? */  /* @!@ */
  /* with a regular plural we will not know the plural word   */
  /* e.g. 'jewels' will not be in the dictionary, but 'jewel' */
  /* will. So we will use the word id for 'jewel' in the      */
  /* sysDescr, but to denote that it is a plural (and thus    */
  /* should not be 'which jewel do you mean'-ed we will enter */
  /* the jewel word id in single_id as well.                  */

  /* irregular plural will be in the word list, as it can     */
  /* also have other types (e.g. 'oxen' and 'oxen barn')      */
  /* here the oxen word-id will be in the sysDescr noun, but  */
  /* we must match with the single_id to match each           */
  /* individual ox.                                           */

  int32_t i     = 0;
  int32_t j     = 0;
  int32_t found = 0;    /* Initially not found. */
  int32_t source_noun      = NO_ID;  /* @!@ */

  /* check for plurality */
  source_noun = (single_id == NO_ID) ? source.noun : single_id;  /* @!@ */

  /* Compare nouns. */

  /* If source_noun is NO_ID, it might be an incomplete system     */
  /* description. Therefore, don't exit yet, but check adjectives. */
  if (source_noun != NO_ID && source_noun != target.noun)
    /* No match */
    return(ERROR);

  /* Could be a match; continue with adjectives.   */
  if (source.nr_of_adjectives > target.nr_of_adjectives)
    /* Source has more adjectives than target; */
    /* this can never be a match.              */
    return(ERROR);

  /* Could still be a match. Check whether all source         */
  /* adjectives are also target adjectives.                   */

  while (i<source.nr_of_adjectives) {
    j = 0;
    found = 0;  /* Not found. */
    while (!found && j < target.nr_of_adjectives)
      found = (source.adjectives[i] == target.adjectives[j++]);
    if (!found)
      /* Target doesn't have this adjective; no match. */
      return(ERROR);

    /* Prevent duplicate adjectives. */
    target.adjectives[j-1] = NO_ID;

    /* Continue with the next source adjective. */
    i++;
  } /* while */

  /* we have a match here. */
  return(OK);
}


resultStruct MatchSysDescr(extendedSysDescr source, int32_t single_id, extendedSysDescr target)   /* @!@ */
 /* source is grabbed from user input */
 /* target is in compiled story       */
{
  /* This function matches two sysdescr structs. The match succeeds */
  /* if source is a `subset' of target. For example, the sources    */
  /* `oil' and `oil lamp' match with target `ancient oil lamp', but */
  /* not vice versa.                                                */

  /* Source and target are passed as value parameters, since target */
  /* will be changed by this function (adjectives that have been    */
  /* matched will be set to NO_ID, so the interpreter will choke on */
  /* double adjectives. Descriptions like `large large lamp' will   */
  /* not be accepted. If they must also be matched, erase the       */
  /* `target.adjectives[j] = NO_ID;' code from this function and    */
  /* remove the if (source.nr_of_... > target.nr_of... ; test.      */

  /* In this function it is assumed that the target sysDescr will   */
  /* always have a complete description. I.e. if the target actor   */
  /* noun is absent, there will also be no adjectives. Therefore,   */
  /* make sure that in the story sourcefile all d_sys descriptions  */
  /* are complete with a noun. If not, the corresponding location   */
  /* or object will never give a match.                             */
  /* In case the above assumption is not wanted, add the check for  */
  /* target.nr_of_adjectives == 0 to the check for empty target.    */

  /* 28nov2016: changed this function to handle extended system     */
  /* descriptions, i.e. <part1> <connecting preposition> <part2>    */
  /* Some initial thoughts:                                         */
  /* - if source has a part 2 and target hasn't -> no match.        */
  /* - if source has no part 2 and target has   -> continue.        */
  /* - For matching part 2:                                         */
  /*   - connecting prepositions must be identical.                 */
  /*   - source adjectives must be subset of target adjectives.     */

  /* 29nov2016: moved part of code to CompareNounsAndAdjectives()   */

  /* 19mar2019: added the flipnoun() function. In some cases where  */
  /* additional info was asked and an entered single adjective was  */
  /* initially parsed as a noun, there would not be a match which   */
  /* would result in redefining the search set. With scope all_locs */
  /* this could lead to strange situations (new things in scope,    */
  /* based on only the adjective).                                  */

  /* 14may2019: function will now also handle dynamic system        */
  /* descriptions.                                                  */

  /* check if target is an expanded system description */
  if (target.dynamic != NULL) {
    /* dynamic system description */
    if (!ConvertDynamicDSys(target.dynamic, &target)) {
      return( (resultStruct) {ERROR, NONE, ERROR} );  /* @!@ */
    }
  }

  /* ok, now we have a 'normal' d_sys */

  /* Check for empty target. */
  if (target.part1.noun == NO_ID)
    return( (resultStruct) {ERROR, NONE, ERROR} );  /* @!@ */

  /* Check for empty source. */
  if (source.part1.noun == NO_ID && source.part1.nr_of_adjectives == 0)
    return( (resultStruct) {ERROR, NONE, ERROR} );  /* @!@ */

  /* Check for different part 2 */
  if (source.connect_prepos != target.connect_prepos && source.connect_prepos != NO_ID)
    return( (resultStruct) {ERROR, NONE, ERROR} );  /* @!@ */

  /* Compare part 1 nouns and adjectives */
  if (!CompareNounsAndAdjectives(source.part1, single_id, target.part1)) {  /* @!@ */

    /* 01oct2019: now that we have plural, only flip the noun if     */  /* @!@ */
    /* single_id = NO_ID or part1 noun and single_id are different.  */
    /* If they are the same, part 1 noun must be considered plural   */
    /* and thus may only have type NOUN. e.g. input 'jewels' will    */
    /* lead to part1 noun 'jewel' but this jewel may not trigger     */
    /* 'jewel box'. where input 'oxen' will lead to part1 noun 'ox', */
    /* but may also trigger 'oxen barn'                              */

    if (single_id == NO_ID || single_id != source.part1.noun) {  /* @!@ */
      if (!FlipNoun(&source.part1)) {
        return( (resultStruct) {ERROR, NONE, ERROR} );  /* @!@ */
      }
      else {
        if (!CompareNounsAndAdjectives(source.part1, single_id, target.part1)) {  /* @!@ */
          return( (resultStruct) {ERROR, NONE, ERROR} );  /* @!@ */
        }
      }
    }
    else {
      /* not a match for part 1, so not a match at all */
      return( (resultStruct) {ERROR, NONE, ERROR} );  /* @!@ */
    }
  }

  /* Compare part 2 nouns and adjectives */
  /* with part 2 we allow no plural, so pass NO_ID for single_id par */
  if (!CompareNounsAndAdjectives(source.part2, NO_ID, target.part2)) {  /* @!@ */
    if (!FlipNoun(&source.part2)) {
      return( (resultStruct) {ERROR, NONE, ERROR} );  /* @!@ */
    }
    else {
      if (!CompareNounsAndAdjectives(source.part2, single_id, target.part2)) {  /* @!@ */
        return( (resultStruct) {ERROR, NONE, ERROR} );  /* @!@ */
      }
    }
  }

  /* We made it to here; this definitely is a match!          */

  return( (resultStruct) {OK, NONE, OK} );  /* @!@ */
}


void SwapSysDescr(extendedSysDescr *sd1, extendedSysDescr *sd2)
{
  extendedSysDescr sd_temp;
  int32_t          i = 0;

  /*****************************/
  /* NOT TESTED IN VERSION 2.1 */
  /*****************************/

  /* copy sd1 to sdtemp */
  sd_temp.dynamic                 = sd1->dynamic;
  sd_temp.part1.article = (sd1->part1).article;
  sd_temp.part1.nr_of_adjectives = (sd1->part1).nr_of_adjectives;
  for (i=0; i<MAX_PARSE_ADJ-1; i++)
    (sd_temp.part1).adjectives[i] = (sd1->part1).adjectives[i];
  sd_temp.part1.noun = (sd1->part1).noun;

  sd_temp.connect_prepos = sd1->connect_prepos;

  sd_temp.part2.article = (sd1->part2).article;
  sd_temp.part2.nr_of_adjectives = (sd1->part2).nr_of_adjectives;

  for (i=0; i<MAX_PARSE_ADJ-1; i++)
    sd_temp.part2.adjectives[i] = (sd1->part2).adjectives[i];

  sd_temp.part2.noun = (sd1->part2).noun;

  /* copy sd2 to sd1 */
  sd1->dynamic                  = sd2->dynamic;
  (sd1->part1).article = (sd2->part1).article;
  (sd1->part1).nr_of_adjectives = (sd2->part1).nr_of_adjectives;
  for (i=0; i<MAX_PARSE_ADJ-1; i++)
    (sd1->part1).adjectives[i] = (sd2->part1).adjectives[i];
  (sd1->part1).noun = (sd2->part1).noun;

  sd1->connect_prepos = sd2->connect_prepos;

  (sd1->part2).article = (sd2->part2).article;
  (sd1->part2).nr_of_adjectives = (sd2->part2).nr_of_adjectives;

  for (i=0; i<MAX_PARSE_ADJ-1; i++)
    (sd1->part2).adjectives[i] = (sd2->part2).adjectives[i];

  (sd1->part2).noun = (sd2->part2).noun;

  /* copy sd_temp to sd2 */
   sd2->dynamic                 = sd_temp.dynamic;
  (sd2->part1).article = sd_temp.part1.article;
  (sd2->part1).nr_of_adjectives = sd_temp.part1.nr_of_adjectives;

  for (i=0; i<MAX_PARSE_ADJ-1; i++)
    (sd2->part1).adjectives[i] = sd_temp.part1.adjectives[i];

  (sd2->part1).noun = sd_temp.part1.noun;

  sd2->connect_prepos = sd_temp.connect_prepos;

  (sd2->part2).article = sd_temp.part2.article;
  (sd2->part2).nr_of_adjectives = sd_temp.part2.nr_of_adjectives;

  for (i=0; i<MAX_PARSE_ADJ-1; i++)
    (sd2->part2).adjectives[i] = sd_temp.part2.adjectives[i];

  (sd2->part2).noun = sd_temp.part2.noun;
}


resultStruct HasMatchingSysDescr(int32_t id, extendedSysDescr descr, int32_t single_id)   /* @!@ */
{
  /* A location or object can have more than one System Description. */
  /* In this function we determine of any one of these matches with  */
  /* the 'descr' parameter. If we have a match, we do not check the  */
  /* remaining system descriptions.                                  */

  /* When referring to d_sys in the story file, the first d_sys will */
  /* always be selected. So, printcr(d_sys) will print the first     */
  /* System Description in the sysDescr array. When there is a match */
  /* on another System Description than the first, the matching      */
  /* System Description will be swapped with the first one. This to  */
  /* ensure that in the interpreter's reply, the same descriptive    */
  /* text will be used as the player did to identify the obj/loc.    */
  /* EDIT: we commented out swapping because with multiple hits,     */
  /* identical descriptions (always the first) would be printed for  */
  /* the user to select from.                                        */

  int32_t          i     = 0;
  int32_t          match = 0;
  int32_t          id_nr_of_dsys;
  extendedSysDescr *id_descrs;

  if (IsLocId(id)) {
    id_nr_of_dsys = loc_dir[id-FIRST_LOCATION_ID].nr_of_dsys;
    id_descrs     = loc_dir[id-FIRST_LOCATION_ID].descr;
  }
  else {
    /* Its an object id. */
    id_nr_of_dsys = obj_dir[id-FIRST_OBJECT_ID].nr_of_dsys;
    id_descrs     = obj_dir[id-FIRST_OBJECT_ID].descr;
  }

  /* search through system descriptions */
  while (i<id_nr_of_dsys && !match)
    if (MatchSysDescr(descr, single_id, id_descrs[i++]).tag == OK)  /* @!@ */
      match = 1;

  if (match) {
    /* Swap the matching System Description with the first one. */
    /* commented out because it messed up solving ambiguities   */
    /* (which key do you mean? The key or the key?)             */
    if (i != 0 && TestCFlag(id, SWAP_SYS_DESCR)) {
      SwapSysDescr(&(id_descrs[0]), &(id_descrs[i-1]));
    }
    return( (resultStruct) {OK, NONE, OK} );  /* @!@ */
  }
  else
    return( (resultStruct) {ERROR, NONE, ERROR} );  /* @!@ */
}


int32_t Search(int32_t id, extendedSysDescr *descr, int32_t single_id, int32_t visible, int32_t depth, match *hits)  /* @!@ */
 /* id tells which loc/obj to search through.        */
 /* single_id is single form of descr                */  /* @!@ */
 /* visible = 0 means scope is ALL_LOCS.             */
 /* depth tells the containment level of the search. */
 /* depth -1 means unlimited depth.                  */
 /* match holds Object ids that match descr.         */
{
  /* This function first determines whether the `id' matches     */
  /* with descr. Next it searches objects that are contained in  */
  /* `id' for contained objects that match descr.                */
  /* The visible-parameter tells whether the loc/obj must be     */
  /* visible to the player. In case of an ALL_LOCS scope in the  */
  /* Find() function, visible must be set to 0. With the other   */
  /* scopes, visible should be 1.                                */
  /* Objects with f_bypass set are also checked, regardless of   */
  /* their visibility (we need this, for example, to turn on a   */
  /* lamp carried by the player, in the dark.                    */
  /* The number of objects that is searched is indicated by      */
  /* depth. Ids of matched objects are returned in the hits par  */
  /* If there are more than MAX_HITS matching objects, OVERFLOW  */
  /* will be returned.                                           */
  /* June 11th 2016: except when scope is ALL_LOCS because the   */
  /* number of hits in that case can be decreased by filtering   */
  /* the objects visible to the player, after which we will      */
  /* check for MAX_HITS.                                         */

  /* October 19th 2015. This function has been adapted, because  */
  /* locations and objects may now have more than 1 system       */
  /* description (d_sys). The set of system descriptions for a   */
  /* location or object is searched until there is a hit. Only   */
  /* one matching d_sys is required to give a hit. After a hit,  */
  /* the remaining system descriptions are not further searched. */

  int32_t  i;
  contData *objs;

  if (IsLocId(id))
    objs = &(loc_dir[id - FIRST_LOCATION_ID].contained_objs);
  else
    /* Its an object id. */
    objs = &(obj_dir[id - FIRST_OBJECT_ID].contained_objs);

  /* Check if id has a d_sys that matches *descr.            */
  /* Checked locs/objs must either be visible, or must have  */
  /* f_bypass set, or visibility must be set to 0 by caller. */

  /* 15apr2016. In case visibility is 0 (e.g. for scope      */
  /* all_locs), we must also consider f_hidden. If           */
  /* visibility = 0 and f_hidden = 1, the loc/obj must be    */
  /* excluded anyway.                                        */

  if (TestCFlag(id, BYPASS) || CanSee(actor, id) || (!visible && !TestCFlag(id, HIDDEN))) {  /* @!@ */
    /* id_descr can have multiple System Descriptions */
    if (HasMatchingSysDescr(id, *descr, single_id).tag == OK) {     /* @!@ */
      /* 07dec2016: changed '>=' to '>' because  */
      /* of autoincrement in else branche below. */
      if (hits->nr_of_hits > MAX_HITS && visible != 0) {
        /* visible = 0 means scope ALL_LOCS. Here we will */
        /* allow more than MAX_HITS hits because we will  */
        /* limit it to visible objects later on.          */
        /* Too many hits. */
        return(OVERFLOW);
      }
      else {
        hits->matched_objs[hits->nr_of_hits++] = id;
      }
    }
  }

  if (depth == 0)
    /* Reached max containment level. */
    return(OK);

  /* Now, find all objects that match descr.             */
  /* Scan through all objects that are contained in loc. */
  for (i=0; i<objs->nr_of_objects; i++)
    if (Search(objs->object_ids[i], descr, single_id, visible, --depth, hits) == OVERFLOW)  /* @!@ */
      /* Too many hits. */
      return(OVERFLOW);
  /* Ready */
  return(OK);
}


int32_t ArrangeVisible(match *hits)
{
  int i       = 0;
  int j       = 0;
  int visible = 0;
  int temp    = 0;
  int cont_i  = 1;
  int cont_j  = 1;

  while (i<hits->nr_of_hits && cont_i) {
    if (!CanSee(PLAYER, hits->matched_objs[i])) {
      /* found an invisible matched object */
      /* swap with one that is visible     */
      /* find the next visible object, if  */
      /* there is one.                     */
      j      = i+1;
      cont_j = 1;
      while (j<hits->nr_of_hits && cont_j) {
        if (CanSee(PLAYER, hits->matched_objs[j])) {
          cont_j = 0;
        }
        else
          j++;
      }
      if (!cont_j) {
        /* found a visible match to swap places with */
        visible++;
        temp = hits->matched_objs[i];
        hits->matched_objs[i] = hits->matched_objs[j];
        hits->matched_objs[j] = temp;
      }
      else
        /*  didn't find a visible object */
        cont_i = 0;
    }
    else
      visible++;
    i++;
  }
  return(visible);
}


void MoreInfo(extendedSysDescr *descr, match *hits, char *line_buf)
{
  switch(story_info.story_language) {
    case NL:
      NL_MoreInfo(descr, hits, line_buf);
      break;
    case ENG:
      ENG_MoreInfo(descr, hits, line_buf);
      break;
    default:
      /* unknown language, use English */
      ENG_MoreInfo(descr, hits, line_buf);
      break;
  }
}


int32_t SearchHits(extendedSysDescr *descr, int32_t single_id, int32_t scope, int32_t *id, match *search_set, char *line_buf)  /* @!@ */
 /* descr is description that must be matched.            */
 /* scope is for in case we need to refresh search set    */
 /* Id is of matching loc/obj (if only one).              */
 /* search_set is set of objs locs to compare with descr. */
{
  /* This function is similar to Find(), in the way that it tries   */
  /* to find an object or location whose description matches descr. */
  /* It differs in the way that it searches only locations and      */
  /* objects in the search_set and that there will be no overflows. */

  match    hits;   /* 19dec07 forgot to initialize, program crashed  */
  int32_t  result;
  int32_t  i = 0;

  /* Malloc space for hits->matched_objs. Will never need more than */
  /* than there are in the search_set variable.                     */
  if ((hits.matched_objs = (int32_t *) malloc((search_set->nr_of_hits)*sizeof(int32_t))) == NULL) {
    PrintError(15, NULL, "8()");
    return(ERROR);
  }
  /* added 19dec07 - start */
  hits.nr_of_hits = 0;
  /* added 19dec07 - end */

  for (i=0; i<search_set->nr_of_hits; i++) {
      if (HasMatchingSysDescr(search_set->matched_objs[i], *descr, single_id).tag == OK)    /* @!@ */
        hits.matched_objs[hits.nr_of_hits++] =
                                         search_set->matched_objs[i];
  } /* for */

  switch (hits.nr_of_hits) {
    case 0:
      free(hits.matched_objs);
      return(NO_MATCH);
    case 1:
      *id = hits.matched_objs[0];
      free(hits.matched_objs);
      return(OK);
    default:
      MoreInfo(descr, &hits, line_buf);

      /* Reset descr. */
      (descr->part1).article          = NO_ID;
      (descr->part1).nr_of_adjectives = 0;
      (descr->part1).noun             = NO_ID;
      descr->connect_prepos           = NO_ID;
      (descr->part2).article          = NO_ID;
      (descr->part2).nr_of_adjectives = 0;
      (descr->part2).noun             = NO_ID;

      switch (ParseDSys(line_buf, descr)) {
        case OK:
          /* line_buf[0] = '\0'; */ /* moved down below */
          /* Try again with new descr. */
          result = SearchHits(descr, single_id, scope, id, &hits, line_buf);  /* @!@ */
          if (result == NO_MATCH) {
            /* they did enter a valid d_sys, so recreate the search set */
            /* free matched_objs because Find() will malloc new space   */
            free(hits.matched_objs);
            Find(descr, single_id, scope, line_buf, &hits);  /* @!@ */
            result = SearchHits(descr, single_id, scope, id, &hits, line_buf);
          }
          free(hits.matched_objs);
          /* next if-clause added on 23mar19. If the player types a new */
          /* command that also has valid object syntax, it will result  */
          /* in a 'you don't see that here' error. E.g 'open chest' is  */
          /* new command, but also can be an open chest. So, now we     */
          /* catch NO_MATCH and return NEXT_SENTENCE.                   */
          if (result == NO_MATCH) {
            return(NEXT_SENTENCE);
          }
          line_buf[0] = '\0';
          return(result);
        case UNKNOWN_WORD:
          /* User entered an unknown word.                     */
          free(hits.matched_objs);
          return(UNKNOWN_WORD);
        case ERROR:
          /* The user entered his next sentence, which is      */
          /* contained in line_buf.                            */
          free(hits.matched_objs);
          return(NEXT_SENTENCE);
        default:
          /* We should never get here, bad ParseDsys() return value */
          PrintError(75, NULL, NULL);

          /* Reset line_buf, so calling function can continue. */
          line_buf[0] = '\0';
          free(hits.matched_objs);
          return(ERROR);
      } /* switch */
  } /* switch */
  /* ready. */
}


int32_t Find(extendedSysDescr *descr, int32_t single_id, int32_t scope, char *line_buf, match *hits)  /* @!@ */

  /* IN CASE OF A SUSPECTED MEMORY LEAK: CHECK THE */
  /* MALLOCs AND FREEs FOR hits->matched_objs      */

{
  int32_t i     = 0;
  int32_t ready = 0;

  /* malloc space for hits. Theoretically - with scope ALL_LOCS - */
  /* each object and location may be a hit, therefore malloc      */
  /* nr_of_locations + nr_of_objects places for hits.             */
  if ((hits->matched_objs = (int32_t *) malloc((nr_of_locs+nr_of_objs)*sizeof(int32_t))) == NULL) {
    PrintError(15, NULL, "Find()");
    return(ERROR);
  }

  hits->nr_of_hits = 0;

  while (!ready) {
    /* Determine boundaries of the search. */
    switch (scope) {
      case ALL_LOCS:
        /* Jan 19th 2004: Changed "i<nr_of_locs+FIRST_LOCATION_ID-1" */
        /* to "i<nr_of_locs+FIRST_LOCATION_ID"                       */
        for (i=FIRST_LOCATION_ID; i<nr_of_locs+FIRST_LOCATION_ID; i++)
        /* 0 means locs/objs need not be visible to the player. */
        /* -1 means unlimited search depth.                     */

          if (Search(i, descr, single_id, 0, -1, hits) == OVERFLOW) {  /* @!@ */
            free(hits->matched_objs);
            return(OVERFLOW);
          }
        break;
      case CURR_LOC_ONLY:
        /* 1 means locs/objs must be visible to the player. */
        /* -1 means unlimited search depth.                 */
        if (Search(curr_loc, descr, single_id, 1, -1, hits) == OVERFLOW) {  /* @!@ */
          free(hits->matched_objs);
          return(OVERFLOW);
        }
        break;
      case ACTOR_ONLY:
        /* -1 means unlimited search depth.                          */
        /* 1 means locs/objs must be visible to the player.          */
        /* Check if there is light first, since SearchObj() doesn't. */
        if (Search(actor, descr, single_id, 1, -1, hits) == OVERFLOW) {  /* @!@ */
          free(hits->matched_objs);
          return(OVERFLOW);
        }
        break;
      default:
        PrintError(76, NULL, NULL);
        free(hits->matched_objs);
        return(ERROR);
    } /* switch */

    if (hits->nr_of_hits == 0 && descr->connect_prepos == NO_ID && !ready) {
      if (!FlipNoun(&(descr->part1))) {
        /* do we really need this? FlipNoun() is also in MatchSysDescr() already */  /* @!@ */
        ready = 1;
      }
    }
    else {
      ready = 1;
    }
  }  /* while */  /* @!@ */

  return(OK);
}


void PrintNotFound(extendedSysDescr *descr)
{
  switch(story_info.story_language) {
    case NL:
      NL_PrintNotFound(descr);
      break;
    case ENG:
      ENG_PrintNotFound(descr);
      break;
    default:
      /* unknown language, use English */
      ENG_PrintNotFound(descr);
      break;
  }
}


resultStruct Translate(parsedInput *parsed_input, int32_t index, usrActionRec *usr_action_rec, char *line_buf)
 /* index tells which subject to use from parsed_input */
{
  int          i           = 0;
  int32_t      try_to_find = 0;
  int32_t      tries       = 3;  /* actor, subject and specifier */
  int32_t      winners     = 0;
  int          unbound     = 0;  /* to denote a possible unbound subjet */  /* @!@ */
  resultStruct result      = {OK, NONE, OK};
  match        actor_hits;
  match        subject_hits;
  match        specifier_hits;

  /* xxx_hits.matched_objs will be maloced by the Find() function */
  /* this will only occur if xxx is filled out                    */
  actor_hits.nr_of_hits       = 0;
  actor_hits.matched_objs     = NULL;
  subject_hits.nr_of_hits     = 0;
  subject_hits.matched_objs   = NULL;
  specifier_hits.nr_of_hits   = 0;
  specifier_hits.matched_objs = NULL;

  /* When we begin, the actor is the player. Only he can have */
  /* entered the command line that must be parsed.            */
  actor = PLAYER;

  /* if xxx_hits.number_of_hits equals -1, means not filled out */

  /* find possible hits for actor */
  if (FilledOut(&(parsed_input->actor))) {
    switch(Find(&(parsed_input->actor), NO_ID, parsed_input->scope, line_buf, &actor_hits)) {  /* @!@ */
      case ERROR:
        result.tag = ERROR;
        free(actor_hits.matched_objs);
        return(result);
        break;
      case OVERFLOW:
        result.tag = OVERFLOW;
        free(actor_hits.matched_objs);
        return(result);
        break;
      case OK:
        if (actor_hits.nr_of_hits == 0) {
          result.tag = ACTOR_ERROR;
          free(actor_hits.matched_objs);
          return(result);
        }
        if (actor_hits.nr_of_hits == 1) {
          /* we found the actor */
          usr_action_rec->actor = actor_hits.matched_objs[0];
          /* one less try because actor is already determined */
          tries--;
        }
        break;
    } /* switch */
  } /* if */
  else {
    /* no actor in user input */
    /* this means the actor is the player */
    /* we don't know how the author named the player object   */
    /* in the story file, so we can only fill it in here with */
    /* the PLAYER keyword (mapping has been done by now).     */
    tries--;
    actor_hits.nr_of_hits = 1;
    actor_hits.matched_objs = (int32_t*) malloc(sizeof(int32_t));
    actor_hits.matched_objs[0] = PLAYER;
  }

  /* find possible hits for subject */
  /* There may be multiple subjects (i.e. 'take lamp, knife'), */  /* @!@ */
  /* but after we introduced parse rules, iterating through    */
  /* subjects was moved to LetsPlay() because of possible      */
  /* parse dependencies between subject and specifier.         */

  /* We do NOT set the subject global var here.                */
  /* This is done by Letsplay().                               */

  /* 30sep2019 subject can now also be plural                  */

  if (FilledOut(&(parsed_input->subject[index]))) {
    do {
      try_to_find = 0; /* will be set to 1 if we have 0 hits and can split subject  */

      switch(Find(&(parsed_input->subject[index]), parsed_input->single[index],
                  parsed_input->scope, line_buf, &subject_hits)) {  /* @!@ */
        case ERROR:
          result.tag = ERROR;
          /* load value with index of subject that caused the error */
          result.value = index;
          free(subject_hits.matched_objs);
          return(result);
          break;
        case OVERFLOW:
          result.tag = OVERFLOW;
          free(subject_hits.matched_objs);
          return(result);
          break;
        case OK:
          /* continue */
          if (subject_hits.nr_of_hits == 0) {
            /* see if we can split up subject */
            /* example: put bowl in passage   */
            if (parsed_input->subject[index].connect_prepos != NO_ID) {
              if (!FilledOut(&(parsed_input->specifier))) {
                /* transform second part of subject to specifier */
                if (SplitSubject(parsed_input, index))
                   /* try again to match subject */
                   try_to_find = 1;
              }
              else {
                /* see if we can move around some words between     */
                /* subject and specifier                            */
                /* example: put bowl in passage with dripping water */
                if ( (parsed_input->prepositions.nr_of_prepositions == 1) &&
                     (parsed_input->specifier.connect_prepos == NO_ID) ) {
                  if (SplitSubjectAndSpecifier(parsed_input, index))
                    try_to_find = 1;
                }
              }
            }
            if (!try_to_find) {
              /* this is not going to work, 0 hits remain          */
              /* it may be an unbound subject, e.g. "hit the road" */  /* @!@ */
              result.tag   = SUBJECT_ERROR;
              result.value = index;
              unbound      = 1;  /* @!@ */
              tries--; /* @!@ */
              /*free(subject_hits.matched_objs);*/  /* @!@ */
              /*return(result);*/  /* @!@ */
            }
          } /* if */
          else {
            if (subject_hits.nr_of_hits == 1) {
              /* we found the subject */
              usr_action_rec->subject[index] = subject_hits.matched_objs[0];

              /* one less try because subject is already determined */
              tries--;
            }
          }
          break;
      } /* switch */
    } while (try_to_find);
  } /* if */
  else {
    /* no subject in user input */
    subject_hits.nr_of_hits = -1;
    tries--;
  }

  /* find possible hits for specifier */
  if (FilledOut(&(parsed_input->specifier))) {
    switch(Find(&(parsed_input->specifier), NO_ID, parsed_input->scope, line_buf, &specifier_hits)) {  /* @!@ */
      case ERROR:
        result.tag = ERROR;
        free(specifier_hits.matched_objs);
        return(result);
        break;
      case OVERFLOW:
        result.tag = OVERFLOW;
        free(specifier_hits.matched_objs);
        return(result);
        break;
      case OK:
        if (specifier_hits.nr_of_hits == 0) {
          result.tag = SPECIFIER_ERROR;
          free(specifier_hits.matched_objs);
          return(result);
        }
        if (specifier_hits.nr_of_hits == 1) {
          /* we found the specifier */
          usr_action_rec->specifier = specifier_hits.matched_objs[0];

          /* one less try because specifier is already determined */
          tries--;
        }
        break;
      default:
        PrintError(77, NULL, NULL);
        /*free(specifier_hits.matched_objs);*/
    } /* switch */
  } /* if */
  else {
    tries--;
    specifier_hits.nr_of_hits = -1;
  }

  /******************************************************************/  /* @!@ */
  /* At this point we have all possible hits for actor, subject and */
  /* specifier. If all matches are unique, then tries will be 0.    */
  /******************************************************************/

  while (tries != 0) {
    winners = ApplyParserRules(parsed_input, &actor_hits, &subject_hits, &specifier_hits);

    /* There will always be 1 or more winners, 0 winners is not possible, */
    /* because there is always at least one action record.                */

    if (winners == 1) {
      usr_action_rec->actor            = (actor_hits.nr_of_hits == 1 ? actor_hits.matched_objs[0] : PLAYER);
      usr_action_rec->subject[index]   = (subject_hits.nr_of_hits == 1 ? subject_hits.matched_objs[0] : NO_ID);
      usr_action_rec->specifier        = (specifier_hits.nr_of_hits == 1 ? specifier_hits.matched_objs[0] : NO_ID);
      tries = 0;
    }
    else {
      /* two or more winners, try to narrow down by asking to specify */
      /* one of actor, subject or specifier that had more than 1 hit. */

      /* try actor */
      if (actor_hits.nr_of_hits > 1) {
        switch(SearchHits(&(parsed_input->actor), NO_ID, parsed_input->scope, &(usr_action_rec->actor),
                          &actor_hits, line_buf)) {
          case NO_MATCH:
            free(actor_hits.matched_objs);
            result.tag = ACTOR_ERROR;
            return(result);
            break;
          case OK:
            /* There was exactly 1 match.   */
            /* Update actor_hits in case we */
            /* need to resolve more objects */
            actor_hits.nr_of_hits = 1;
            actor_hits.matched_objs[0] = usr_action_rec->actor;
            break;
          case UNKNOWN_WORD:
            /* unknown word in user input */
            free(actor_hits.matched_objs);
            result.tag = UNKNOWN_WORD;
            return(result);
            break;
          case ERROR:
            free(actor_hits.matched_objs);
            result.tag = ERROR;
            return(result);
            break;
          case NEXT_SENTENCE:
            free(actor_hits.matched_objs);
            result.tag = NEXT_SENTENCE;
            return(result);
            break;
        } /* switch SearchHits() */
        tries--;
      } /* if */
      else {
        /* try subject */
        if (subject_hits.nr_of_hits > 1) {
          /* 02oct2019: if it was a plural, do not SearchHits(). All  */
          /* hits remaining after the disambiguation rules are valid. */  /* @!@ */
          if (parsed_input->single[index] == NO_ID) {
            /* not a plural */
            switch(SearchHits(&(parsed_input->subject[index]), parsed_input->single[index],
                              parsed_input->scope, &(usr_action_rec->subject[index]), &subject_hits, line_buf)) {
              case NO_MATCH:
                free(subject_hits.matched_objs);
                result.tag = SUBJECT_ERROR;
                return(result);
                break;
              case OK:
                /* There was exactly 1 match.     */
                /* Update subject_hits in case we */
                /* need to resolve more objects   */
                subject_hits.nr_of_hits = 1;
                subject_hits.matched_objs[0] = usr_action_rec->subject[index];
                break;
              case UNKNOWN_WORD:
                /* unknown word in user input */
                free(actor_hits.matched_objs);
                result.tag = UNKNOWN_WORD;
                return(result);
                break;
              case ERROR:
                free(subject_hits.matched_objs);
                result.tag = ERROR;
                return(result);
                break;
              case NEXT_SENTENCE:
                free(subject_hits.matched_objs);
                result.tag = NEXT_SENTENCE;
                return(result);
                break;
            } /* switch SearchHits() */
            tries--;
          } /* if */
          else {
            /* it was a plural. All ids in subject_hits are */
            /* valid and must be returned to caller         */

            /* first, indicate that the subject is plural   */
            usr_action_rec->subject[index] = PLURAL;

            /* next, copy the hits */
            for (i=0; i<subject_hits.nr_of_hits; i++) {
              usr_action_rec->plural_subjects[i] = subject_hits.matched_objs[i];
            }
            tries--;
          }
        }
        else {
          /* try specifier */
          if (specifier_hits.nr_of_hits > 1) {
            switch(SearchHits(&(parsed_input->specifier), NO_ID, parsed_input->scope,
                              &(usr_action_rec->specifier), &specifier_hits, line_buf)) {
              case NO_MATCH:
                free(specifier_hits.matched_objs);
                result.tag = SPECIFIER_ERROR;
                return(result);
                break;
              case OK:
                /* There was exactly 1 match.      */
                /* Update specifier_hits in case   */
                /* we need to resolve more objects */
                specifier_hits.nr_of_hits = 1;
                specifier_hits.matched_objs[0] = usr_action_rec->specifier;
                break;
              case UNKNOWN_WORD:
                /* unknown word in user input */
                free(actor_hits.matched_objs);
                result.tag = UNKNOWN_WORD;
                return(result);
                break;
              case ERROR:
                /* unknown word in user input */
                free(specifier_hits.matched_objs);
                result.tag = ERROR;
                return(result);
                break;
              case NEXT_SENTENCE:
                free(specifier_hits.matched_objs);
                result.tag = NEXT_SENTENCE;
                return(result);
                break;
            } /* switch SearchHits() */
            tries--;
          } /* if */
        } /* else */
      } /* else */
    } /* else */
  } /* while */

  actor     = usr_action_rec->actor;
  subject   = usr_action_rec->subject[index];
  specifier = usr_action_rec->specifier;

  /* Copy rest of parsed_input into usr_action_rec, */
  usr_action_rec->action1      = parsed_input->action1;
  usr_action_rec->action2      = parsed_input->action2;
  usr_action_rec->adverbs      = parsed_input->adverbs;
  usr_action_rec->direction    = parsed_input->direction;
  usr_action_rec->q_word       = parsed_input->q_word;
  usr_action_rec->prepositions = parsed_input->prepositions;
  usr_action_rec->value        = parsed_input->value;
  usr_action_rec->ordinal      = parsed_input->ordinal;

  action    = usr_action_rec->action1;
  direction = usr_action_rec->direction;
  prepos    = (parsed_input->prepositions.nr_of_prepositions == 0?
               NO_ID : parsed_input->prepositions.preposition[0]);
  value     = usr_action_rec->value;
  ordinal   = usr_action_rec->ordinal;

  free(actor_hits.matched_objs);
  free(subject_hits.matched_objs);
  free(specifier_hits.matched_objs);

  result.tag = (unbound == 1 ? SUBJECT_ERROR : OK);  /* @!@ */
  return(result);
}
