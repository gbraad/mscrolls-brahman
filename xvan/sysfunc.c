
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
#include <stdlib.h>

#ifndef __APPLE__
#include <malloc.h>   /* _alloca() */
#endif

#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "sysfunc.h"
#include "defs.h"

/*************************/
/* Function declarations */
/*************************/

int32_t IsDirection(int32_t);
void    ConvSpecId(int32_t*, int32_t*);
int32_t GetPar(int32_t*, int32_t*, int32_t*, char**, int32_t**);
int32_t GetLvaluePar(int32_t*, int32_t*, int32_t*, char**, int32_t**);
int32_t GetActionRecPar(usrActionRec*, int32_t**);
int32_t GetAttributeInfo(int32_t, int32_t, attrInfo**, int32_t*);
void    SkipPar(int32_t**);
void    SkipFun(int32_t**);
void    SetBitVal(int32_t*, int32_t, int32_t);
int32_t TestBitVal(int32_t, int32_t);
void    ProcLFlagVal(int32_t, int32_t);
int32_t TestCFlag(int32_t, int32_t);
int32_t TestLFlag(int32_t);
int32_t ContList(int32_t, int32_t*, int32_t);
int32_t BuildCList(int32_t, int32_t*, int32_t*);
void    BuildSyncList(int32_t, int32_t*, int32_t*, int32_t);
void    SetAll(int32_t, int32_t);
int32_t TestAll(int32_t, int32_t);
int32_t TestOne(int32_t, int32_t);
int32_t Exit(int32_t, usrActionRec*, int32_t);
int32_t Entrance(int32_t, usrActionRec*, int32_t);
int32_t Contents(int32_t, usrActionRec*, int32_t);
int32_t Move(int32_t, int32_t);
int32_t Owns(int32_t, int32_t, int32_t);
int32_t Shuffle(int32_t);
int32_t InitSpanTree(spanTree**, int32_t**);
int32_t SpanTree(int32_t, int32_t, int32_t, spanTree*);
int32_t IsLit(int32_t);
int32_t CountObjects(int32_t, int32_t, int32_t, int32_t);
int32_t Synchronize(int32_t, int32_t, int32_t, int32_t, int32_t, usrActionRec*, int32_t);
int32_t HasLight(int32_t);
int32_t UpTree(int32_t);
int32_t CanSee(int32_t, int32_t);

void    Push(int32_t);
int32_t Pop(void);

void    And(void);
void    Or(void);
void    Not(void);

/************************/
/* Function definitions */
/************************/

int32_t IsDirection(id)
 int32_t  id;
{
  wordTable *wt = word_table;
  int32_t i = 0;
  int32_t j = 0;

  /* Find id. */
  while (i<nr_of_words && wt[i].id != id)
    i++;

  if (i == nr_of_words) {
    /* Internal error; this shouldn't happen. */
    PrintError(50, &((resultStruct) {VALUE,id}),NULL);
  }
  else {
    while (j < MAX_TYPES) {
      if (wt[i].types[j] == DIRECTIONS)
        return(OK);
      if (wt[i].types[j] == NO_TYPE)
        return(ERROR);
      j++;
    } /* while */
  } /* else */
  return(ERROR);
}


void ConvSpecId(id, type)
 int32_t *id, *type;
{
  /* If type is NUMBER, id can have one of the   */
  /* wildcard values but must not be converted.  */
  /* For example, NUMBER 6710 will be treated as */
  /* as ACTION wildcard if we do not check for   */
  /* NUMBER type.                                */

  if (*type != NUMBER) {
    switch (*id) {
      case ACTION:
        *id   = action;
        *type = VERB;
        break;

      case PREPOS:
        *id   = prepos;
        *type = PREPOSITIONS;
        break;

      case DIR:
        *id = direction;
        *type = DIRECTIONS;
        break;

      case THIS:
        *id   = active_entity;
        *type = IsLocId(*id) ? LOC_ID : OBJ_ID;
        break;

      case CURR_LOC:
        *id   = curr_loc;
        *type = LOC_ID;
        break;

      case ACTOR:
        *id   = actor;
        *type = IsLocId(*id) ? LOC_ID : OBJ_ID;
        break;

      case SUBJECT:
        *id   = subject;
        *type = IsLocId(*id) ? LOC_ID : OBJ_ID;
        break;

      case SPECIFIER:
        *id   = specifier;
        *type = IsLocId(*id) ? LOC_ID : OBJ_ID;
        break;

      case VALUE:
        *id   = value;
        *type = NUMBER;
        break;

      case ORDINAL:
        *id   = ordinal;
        *type = NUMBER;

      default:
        /* do nothing */;
    } /* switch */
  } /* if */
}


int32_t GetPar(owner, par, type, str, trigger)
 int32_t *owner;
 int32_t *par;
 int32_t *type;
 char    **str;      /* Needed to return address of string par. */
 int32_t **trigger;
{
  /* This routine expects a parameter at the start of *trigger.   */
  /* It fills out the parameter plus it's owner (if any). In case */
  /* of a function, the function is executed and its result is    */
  /* considered to be a parameter. The type of the parameter is   */
  /* returned in the type par. The parameter is terminated by an  */
  /* END_OF_PAR keyword.                                          */

  /* Parameter syntax: loc/obj descr/flag/trigger EOP  */
  /*                   loc/obj attr [attr attr ..] EOP */
  /*                   <wildcard> EOP                  */

  int32_t  ratio;
  int32_t  i;
  attrInfo *attributes;
  int32_t  index;
  int32_t  next;

  next = NextOpcode(trigger);

  /* Check for VALUE or ORDINAL parameter (in case of a */
  /* VALUE or ORDINAL par, ConvSpecId() will return the */
  /* actual value, which may fall within another id     */
  /* range. This will cause an error later on.          */

  if (next == VALUE || next == ORDINAL) {
    /* Get value parameter. */
    *par = next;
    ConvSpecId(par, type);  /* returns actual value */
    *owner = NO_ID;

    /* Skip EOP */
    NextOpcode(trigger);
    return(OK);
  }
  else
    /* convert the wildcards to their actual value */
    ConvSpecId(&next, type);

  /* Ok, not a VALUE parameter. */
  if (next == END_OF_PAR)
    return(OK);

  else if (IsTimerId(next)) {
    *owner = NO_ID;
    *par   = next;
    *type  = TIM_ID;

    /* Skip EOP */
    NextOpcode(trigger);
    return(OK);
  }
  else if (IsLocId(next) || IsObjId(next)) {
    *owner = NO_ID;
    *par   = next;

    *type = IsLocId(next) ? LOC_ID : OBJ_ID;
    return(GetPar(owner, par, type, str, trigger));
  }
  else if (IsCDescrId(next) || IsLDescrId(next) || IsCFlagId(next) ||
           IsLFlagId(next) || IsCTriggId(next) || IsLTriggId(next)) {
    *owner = *par; /* Always preceded by owner. */
    *par   = next;
    *type = IsCDescrId(next) ? COMMON_DESCRS : (IsLDescrId(next) ? DESCR_ID : (IsCFlagId(next) ? COMMON_FLAGS : IsLFlagId(next) ? FLAG_ID :
            (IsCTriggId(next) ? COMMON_TRIGGERS : TRIGG_ID)));

    /* Skip EOP */
    NextOpcode(trigger);
    return(NO_TYPE);     /* IS THIS A CORRECT RETURN VALUE ??? */
  }

  /* In case an attribute parameter is encountered, GetPar() will return the value   */
  /* and type of the content of the attribute (it will not return the attribute id). */
  /* In case the attribute id must be returned (e.g. for function SetAttribute()),   */
  /* the function GetLvaluePar() should be used.                                  */

  else if (IsCAttrId(next) || IsLAttrId(next)) {
    *owner = *par;
    if (IsLAttrId(next)) {
      attributes = local_attrs;
      index      = next-FIRST_LOCAL_ATTR_ID;
    }
    else {
      if (IsLocId(*owner)) {
        attributes = c_loc_attrs;
        index      = ((*owner)-FIRST_LOCATION_ID)*nr_of_cattrs+
                     next-FIRST_COMMON_ATTR_ID;
      }
      else {
        attributes = c_obj_attrs;
        index      = ((*owner)-FIRST_OBJECT_ID)*nr_of_cattrs+
                     next-FIRST_COMMON_ATTR_ID;
      }
    }

    *owner = attributes[index].value_owner;  /* 10march2017 */
    *par  = attributes[index].value;
    *type = attributes[index].type;
    ConvSpecId(par, type);

    return(GetPar(owner, par, type, str, trigger));
  }
  else if (IsIntAct(next)) {
    /* following types apply:                      */
    /* rnd()        : NUMBER                       */
    /* distance()   : NUMBER                       */
    /* count()      : NUMBER                       */
    /* synchronize(): NUMBER                       */
    /* firstdir()   : DIRECTIONS                   */
    /* dest()       : LOC_ID                       */
    /* owner()      : LOC_ID, OBJ_ID, NONE or QUIT */

    /* First execute the internal action.     */
    /* Testfunctions cannot be parameters, so */
    /* set action_rec to NULL and             */
    /* subject_index to -1                    */
    *owner = NO_ID;
    *par   = XeqIntAct(next, trigger, NULL, -1);

    /* Do not test for QUIT here. If rnd()    */
    /* returns the numerical value for        */
    /* QUIT, we have a problem.               */

    /* now determine the type */
    switch (next) {
      case RAND:
        *type = NUMBER;
        break;

      case DISTANCE:
        *type = NUMBER;
        break;

      case COUNT:
        *type = NUMBER;
        break;

      case SYNCHRONIZE:
        *type = NUMBER;
        break;

      case FIRSTDIR:
        if (*par == NONE)
          *type = NO_TYPE;
        else
          if (*par == QUIT)
            return(ERROR);
          else
            *type = DIRECTIONS;
        break;

      case DEST:
        if (*par == NONE)
          *type = NO_TYPE;
        else
          if (*par == QUIT)
            return(ERROR);
          else
            *type = LOC_ID;
        break;

      case OWNER:
        /* possible returns for owner are */
        /* location, object or NONE       */
        if (*par == NONE)
          *type = NO_TYPE;
        else
          if (*par == QUIT)
            return(ERROR);
          else
            *type = IsLocId(*par) ? LOC_ID : OBJ_ID;
        break;

      default:
        /* all other internal actions */
        *type = NO_TYPE;
        break;
    }
    return(GetPar(owner, par, type, str, trigger));
  }

  /* if a word is used in the par list it will be    */
  /* preceded by a WORD_ID or DIRECTIONS keyword.    */
  /* If a special id was used, it will be converted  */
  /* to an id, but there will be no WORD_ID or       */
  /* DIRECTIONS keyword to check for.                */
  /* So we must both check for WORD_ID and for       */
  /* IsWordId().                                     */

  else if (IsWordId(next)) {
    /* if a special id was used, the actual value    */
    /* returned by ConvSpecId() will not be preceded */
    /* by a keyword.                                 */
    *owner = NO_ID;
    *par   = next;

    /* get the correct type */
    if (IsDirection(next))
      *type = DIRECTIONS;
    else
      if (IsVerbId(next))
        *type = VERB;
      else
        *type = WORD_ID;
    /* Skip EOP */
    NextOpcode(trigger);

    return(OK);
  }
  else if (next == DIRECTIONS || next == WORD_ID) {
    *owner = NO_ID;
    *par   = NextOpcode(trigger);
    *type  = (next == DIRECTIONS ? DIRECTIONS : WORD_ID);

    /* skip EOP */
    NextOpcode(trigger);
    return(OK);
  }

  else if (next == NUMBER) {
    *owner = NO_ID;
    /* Read the value. */
    *par  = NextOpcode(trigger);
    *type = NUMBER;

    /* Skip EOP */
    NextOpcode(trigger);
    return(OK);
  }

  else if (next == -1) {
    ratio = sizeof(char *)/sizeof(int32_t);  /* Must be > 0 */
    for (i=0; i<ratio; i++)
      ((int32_t *) str)[i] = *((*trigger)++);
    *owner = NO_ID;
    *type  = STRING;

    /* Skip EOP */
    NextOpcode(trigger);

    return(OK);
  }
  /* next code added on January 6th 2004. There were problems   */
  /* with verb PROLOGUES in case the user only typed a verb     */
  /* (e.g. "move". The PROLOGUE would test for the SUBJECT and  */
  /* the GetPar() function would return an error, as NO_ID      */
  /* wasn´t recognized as a valid parameter. Code has been      */
  /* changed to return a NONE value in case a NO_ID parameter   */
  /* is read. Functions must be changed accordingly, to produce */
  /* the correct result.                                        */
  /* 23Nov2015 - NONE added to the test because NONE no longer  */
  /* is included in the IsLocId() and IsObjId() functions.      */
  else if (next == NO_ID || next == NONE) {
	  *owner = NO_ID;
	  *par   = NONE;
      /* type remains the same */

	  /* Skip rest of parameter (in case the NO_ID was the owner */
	  /* of a parameter).                                         */
      while (NextOpcode(trigger) != END_OF_PAR)
        /* do nothing */ ;
  }
  else {
    PrintError(62, &((resultStruct) {VALUE,next}), "GetPar()");
    return(ERROR);
  }
  return(OK);
}


int32_t GetLvaluePar(owner, par, type, str, trigger)
 int32_t *owner;
 int32_t *par;
 int32_t *type;
 char    **str;      /* Needed to return address of string par. */
 int32_t **trigger;
{
  /* This routine expects an attribute parameter at the start of *trigger. */
  /* It returns the attribute id plus it's owner (if any). If the actual   */
  /* value of the attribute (it's content) is needed, function GetPar()    */
  /* should be used.                                                       */

  /* Parameter syntax: loc/obj attribute EOP */

  int32_t next;

  next = NextOpcode(trigger);

  if (IsIntAct(next)) {
    /* Testfunctions cannot be parameters, so */
    /* set action_rec to NULL and             */
    /* subject_index to -1                    */
    switch (next) {
      case OWNER:
        next = XeqIntAct(next, trigger, NULL, -1);
        /* error in return value are caught by the last else statement of this function. */
        break;
      default:
        PrintError(63, NULL, NULL);
        return(ERROR);
    }
  }

  /* check for location or object as owner */
  ConvSpecId(&next, type);

  if (IsLocId(next) || IsObjId(next)) {
    *owner = NO_ID;
    *par   = next;
    *type  = IsLocId(next) ? LOC_ID : OBJ_ID;

    return(GetLvaluePar(owner, par, type, str, trigger));
  }
  else {
    if (IsCAttrId(next)) {
          *owner = *par;
          *par   = next;
          *type  = COMMON_ATTRS;
    }
    else {
      if (IsLAttrId(next)) {
        *owner = *par;
        *par   = next;
        *type  = ATTRIBUTES;
      }
      else {
        if (IsTimerId(next)) {
          *owner = NO_ID;
          *par   = next;
          *type  = TIM_ID;
        }
        else {
          PrintError(64, NULL, NULL);
          return(ERROR);
        } /* else */
      } /* else */
    } /* else */
  } /* else */
  /* skip EOP */
  NextOpcode (trigger);
  return(OK);
}


int32_t GetActionRecPar(action_rec, trigger)
 usrActionRec *action_rec;
 int32_t      **trigger;
{
  int i = 0;

  /* This function expects the members of a   */
  /* parser action record at the start of     */
  /* trigger. It will read all of the members */
  /* and store them in the action_rec par     */

  action_rec->actor   = NextOpcode(trigger);
  action_rec->action1 = NextOpcode(trigger);
  action_rec->action2 = NextOpcode(trigger);

  action_rec->adverbs.nr_of_adverbs = NextOpcode(trigger);

  for (i=0; i<MAX_PARSE_ADVERBS; i++) {
    action_rec->adverbs.adverb[i] = NextOpcode(trigger);
  }

  action_rec->q_word     = NextOpcode(trigger);
  action_rec->direction  = NextOpcode(trigger);
  action_rec->subject[0] = NextOpcode(trigger);
  action_rec->specifier  = NextOpcode(trigger);

  action_rec->prepositions.nr_of_prepositions = NextOpcode(trigger);

  for (i=0; i<MAX_PARSE_PREPOS; i++) {
    action_rec->prepositions.preposition[i] = NextOpcode(trigger);
  }

  action_rec->value      = NextOpcode(trigger);
  action_rec->ordinal    = NextOpcode(trigger);

  /* We still have the trigger[0] and trigger[1] members  */
  /* We chose to let the compiler write them as NO_ID and */
  /* not skip them. Maybe we need in in a future version. */

  /* for now, skip the trigger codes */
  NextOpcode(trigger);
  NextOpcode(trigger);

  /* we should be at the end of the action rec  */
  /* members now, so the next opcode must be an */
  /* END_OF_PARAMETER                           */

  if ((i=NextOpcode(trigger)) != END_OF_PAR) {
    /* abusing i because we still got it */
    PrintError(95, &((resultStruct) {VALUE,i}), NULL);
    return(QUIT);
  }

  /* complete the action record */
  action_rec->next  = NULL;

  return(OK);
}


int32_t GetAttributeInfo(id, owner, attributes, attribute_index)
 int32_t  id;
 int32_t  owner;
 attrInfo **attributes;
 int32_t  *attribute_index;
{
  /* This function searches the attrInfo for the attribute identified by */
  /* id and owner parameters. It returna a pointer to the right attrInfo */
  /* array ( c_loc_attrs, c_obj_attrs or local_attrs) and the index in   */
  /* array for the attribute.                                            */
  /* We need this function in case we do not need an attribute parameter */
  /* itself, but its contents (type and value).                          */

  if (IsCAttrId(id)) {
    if (IsLocId(owner)) {
      *attributes      = c_loc_attrs;
      *attribute_index = (owner-FIRST_LOCATION_ID)*nr_of_cattrs+id-FIRST_COMMON_ATTR_ID;
    }
    else {
      *attributes      = c_obj_attrs;
      *attribute_index = (owner-FIRST_OBJECT_ID)*nr_of_cattrs+id-FIRST_COMMON_ATTR_ID;
    }
  }
  else if (IsLAttrId(id)) {
    /* Local attribute. */
    *attributes      = local_attrs;
    *attribute_index = id-FIRST_LOCAL_ATTR_ID;
  }
  else {
    /* error */
    PrintError(65, &((resultStruct) {VALUE,id}), NULL);
    return(ERROR);
  }
  return(OK);
}


void SkipPar(trigger)
 int32_t **trigger;
{
  /* This routine expects a parameter at the start of *trigger. */
  do {
    if (IsIntAct(**trigger)) {
      /* Skip opcode. */
      NextOpcode(trigger);
      SkipFun(trigger);
    }
    else if (**trigger == NUMBER)
      /* Skip value. */
      (*trigger)++;
    else if (**trigger == -1)
      /* Skip string address. */
      *trigger += 2;
  }
  while ( *((*trigger)++) != END_OF_PAR);
}


void SkipFun(trigger)
 int32_t **trigger;
{
  /* Opcode has already been read by the caller. */

  int32_t i;
  int32_t nr_of_pars;

  /* Read the number of parameters. */

  nr_of_pars  = NextOpcode(trigger);

  for (i=0; i<nr_of_pars; i++)
    SkipPar(trigger);

}


int32_t TestBitVal(word, bit_pos)
 int32_t word;
 int32_t bit_pos;
{
  /* Mind the parenthesis. Operator priority is tricky here. */
  return( ( (word & (1 << bit_pos)) == 0 ? 0 : 1));
}

int32_t ContList(id, list, scope)
 int32_t id;
 int32_t list[];
 int32_t scope;
{
  /* Fills list id and with all object ids that are contained */
  /* in id. In case list is not completely filled, a NO_ID    */
  /* terminates the list.                                     */

  int32_t index = 0;
  int     i     = 0;

  if (scope == ALL_LOCS) {
    for (i=0; i<nr_of_locs; i++) {
      if (i != id-FIRST_LOCATION_ID) {
        /* we will do id last */
        if (!BuildCList(i+FIRST_LOCATION_ID, list, &index))
          return(ERROR);
      }
    }
    /* now do id */
    if (!BuildCList(id, list, &index))
      return(ERROR);
  }
  else
    if (!BuildCList(id, list, &index))
      return(ERROR);

  if (index != nr_of_locs+nr_of_objs)
    /* list[index] was next position to use */
    list[index] = NO_ID;

  return(OK);
}


int32_t BuildCList(id, list, index)
 int32_t id;
 int32_t list[];  /* Must have length nr_of_locs+nr_of_objs. */
 int32_t *index;
{
  /* Fills list with all objects that are (either directly or */
  /* indirectly) contained in id.                             */

  int32_t      i = 0;
  contData *cont;

  list[(*index)++] = id;

  /* the last is to be used in case of scope ALL_LOCS will be   */
  /* nr_of_locs+nr_of_objs-1, but in that case, index will have */
  /* value nr_of_locs+nr_of_objs after the increment. Therefore */
  /* we must test on nr_of_locs+nr_of_objs+1                    */
  if (*index == nr_of_locs+nr_of_objs+1) {
    PrintError(66, NULL, NULL);
    return(ERROR);
  }

  if (IsLocId(id))
    cont = &(loc_dir[id-FIRST_LOCATION_ID].contained_objs);
  else
    /* It must be an object id. */
    cont = &(obj_dir[id-FIRST_OBJECT_ID].contained_objs);

  for (i=0; i<cont->nr_of_objects; i++)
    if (BuildCList(cont->object_ids[i], list, index) == ERROR)
      return(ERROR);
  return(OK);
}


void SetBitVal(word, bit_pos, value)
 int32_t *word;
 int32_t bit_pos;
 int32_t value;
{
  switch (value) {
    case 0:
      *word = *word & (~(1 << bit_pos));
      break;
    case 1:
      *word = *word | (1 << bit_pos);
      break;
    default:
      PrintError(67, NULL, NULL);
      break;
  }
}


void ProcCFlagVal(owner, id, value)
 int32_t owner;
 int32_t id;
 int32_t value;
{
  int32_t word_offset = 0;
  int32_t bit_offset  = 0;
  int32_t first_id    = 0;
  int32_t *flags      = NULL;

  if (IsLocId(owner)) {
    first_id = FIRST_LOCATION_ID;
    flags = com_loc_flags;
  }
  else {
    first_id = FIRST_OBJECT_ID;
    flags = com_obj_flags;
  }

  word_offset = ((id-FIRST_COMMON_FLAG_ID) +
                 ((owner-first_id)*nr_of_cflags)
                ) / WORD_LEN;
  bit_offset  = ((id-FIRST_COMMON_FLAG_ID) +
                 ((owner-first_id)*nr_of_cflags)
                ) % WORD_LEN;

  SetBitVal(flags+word_offset, bit_offset, value);
}


void ProcLFlagVal(id, value)
 int32_t id;
 int32_t value;
{
  /* Id is used as an offset to address within flags. */
  int32_t word_offset = (id - FIRST_LOCAL_FLAG_ID) / WORD_LEN;
  int32_t bit_offset  = (id - FIRST_LOCAL_FLAG_ID) % WORD_LEN;

  SetBitVal(local_flags+word_offset, bit_offset, value);
}


int32_t TestCFlag(owner, id)
 int32_t owner;
 int32_t id;
{
  int32_t word_offset = 0;
  int32_t bit_offset  = 0;
  int32_t first_id    = 0;
  int32_t *flags      = NULL;

  if (IsLocId(owner)) {
    first_id = FIRST_LOCATION_ID;
    flags = com_loc_flags;
  }
  else {
    first_id = FIRST_OBJECT_ID;
    flags = com_obj_flags;
  }

  /* Common flags for one loc/obj are grouped together. */
  word_offset = ((id-FIRST_COMMON_FLAG_ID) +
                 ((owner-first_id)*nr_of_cflags)
                ) / WORD_LEN;
  bit_offset  = ((id-FIRST_COMMON_FLAG_ID) +
                 ((owner-first_id)*nr_of_cflags)
                ) % WORD_LEN;

  return(TestBitVal(*(flags+word_offset), bit_offset));
}


int32_t TestLFlag(id)
 int32_t id;
{
  /* Id is used as an offset to address within flags. */
  int32_t word_offset = (id - FIRST_LOCAL_FLAG_ID) / WORD_LEN;
  int32_t bit_offset  = (id - FIRST_LOCAL_FLAG_ID) % WORD_LEN;

  return(TestBitVal(*(local_flags+word_offset), bit_offset));
}


void SetAll(id, flag_id)
 int32_t id;
 int32_t flag_id;
{
  /* This function sets flag <flag_id> for all */
  /* objects that are contained in id. Flag is */
  /* NOT set for id itself.                    */
  /* Flag_id must be a common flag.            */

  contData *objs;
  int32_t  i;

  if (IsLocId(id))
    objs = &(loc_dir[id-FIRST_LOCATION_ID].contained_objs);
  else
    /* Its an object id. */
    objs = &(obj_dir[id-FIRST_OBJECT_ID].contained_objs);

  for (i=0; i<objs->nr_of_objects; i++) {
    /* Set flag for contained object. */
    ProcCFlagVal(objs->object_ids[i], flag_id, 1);

    /* Set flag for all its contained objects. */
    SetAll(objs->object_ids[i], flag_id);
  }
}


int32_t TestAll(id, flag_id)
 int32_t id;
 int32_t flag_id;
{
  /* This function returns OK if flag <flag_id> is  */
  /* set for all objects that are contained in id.  */
  /* Id itself is NOT tested.                       */
  /* Flag_id must be a common flag.                 */

  contData *objs;
  int32_t  i;

  if (IsLocId(id))
    objs = &(loc_dir[id-FIRST_LOCATION_ID].contained_objs);
  else
    /* Its an object id. */
    objs = &(obj_dir[id-FIRST_OBJECT_ID].contained_objs);

  for (i=0; i<objs->nr_of_objects; i++) {
    /* Test contained object. */
    if (!TestCFlag(objs->object_ids[i], flag_id))
      return(ERROR);
    /* Test its contained objects. */
    if (!TestAll(objs->object_ids[i], flag_id))
      return(ERROR);
  }
  return(OK);
}


int32_t TestOne(id, flag_id)
 int32_t id;
 int32_t flag_id;
{
  /* This function returns OK if flag <flag_id> is  */
  /* set for at least one of the objects that are   */
  /* contained in id.                               */
  /* Id itself is NOT tested.                       */
  /* Flag_id must be a common flag.                 */

  contData *objs;
  int32_t      i;

  if (IsLocId(id))
    objs = &(loc_dir[id-FIRST_LOCATION_ID].contained_objs);
  else
    /* Its an object id. */
    objs = &(obj_dir[id-FIRST_OBJECT_ID].contained_objs);

  for (i=0; i<objs->nr_of_objects; i++) {
    /* Test contained object. */
    if (TestCFlag(objs->object_ids[i], flag_id))
      return(OK);
    /* Test its contained objects. */
    if (TestOne(objs->object_ids[i], flag_id))
      return(OK);
  }
  return(ERROR);
}


int32_t Exit(par, action_rec, subject_index)
 int32_t      par;
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  /* Executes the t_exit triggers for par and all its */
  /* contained objects.                               */

  int32_t next_result = AGREE;
  int32_t result      = AGREE; /* Not NO_MATCH, since we have no */
  int32_t i           = 0;     /* default t_exit.                */
  int32_t* list = _alloca((nr_of_locs+nr_of_objs)*sizeof(int32_t));

  /* Create the containment list for par. */
  /* par is placed at start of list.      */
  ContList(par, list, CURR_LOC);

  par = list[i];

  while (par != NO_ID && i < (nr_of_locs+nr_of_objs) && result != DISAGREE && result != QUIT) {
    next_result = XeqTrigger(par, EXITT, action_rec, subject_index);
    if (next_result != AGREE && next_result != NO_MATCH)
      result = next_result;
    par = list[++i];
  }

  return(result);
}


int32_t Entrance(par, action_rec, subject_index)
 int32_t      par;
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  /* Executes the t_entrance triggers for par and all its */
  /* contained objects.                                   */

  int32_t next_result = AGREE;
  int32_t result      = AGREE; /* Not NO_MATCH, since we have no */
  int32_t i           = 0;     /* default t_entrance.            */
  int32_t* list = _alloca((nr_of_locs+nr_of_objs)*sizeof(int32_t));

  /* Create the containment list for par. */
  /* par is placed at start of list.      */
  ContList(par, list, CURR_LOC);

  par = list[i];

  while (par != NO_ID && i < nr_of_locs+nr_of_objs && result != DISAGREE && result != QUIT) {
    next_result = XeqTrigger(par, ENTR, action_rec, subject_index);
    if (next_result != AGREE && next_result != NO_MATCH)
      result = next_result;
    par = list[++i];
  }

  return(result);
}


int32_t Contents(par, action_rec, subject_index)
 int32_t      par;
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  /* Same as Entrance(), except that this routine doesn't */
  /* execute t_entrance for par itself                    */

  int32_t  result;
  int32_t  end_result = AGREE;
  int32_t  i          = 0;
  contData objs;           /* DIT OOK MET POINTER PROBEREN!!!. */

  if (IsLocId(par))
    objs = loc_dir[par-FIRST_LOCATION_ID].contained_objs;
  else
    /* Its an object */
    objs = obj_dir[par-FIRST_OBJECT_ID].contained_objs;

  while (end_result == AGREE && i<objs.nr_of_objects) {
    /* Now we must call Entrance() and NOT Contents(). */
    /* Otherwise, no t_entrance will be executed.      */

    if ((result = Entrance(objs.object_ids[i], action_rec, subject_index)) != AGREE)
      end_result = result;
    i++;
  }

  return(end_result);
}


int32_t Move(src, dest)
 int32_t src;
 int32_t dest;
{
  /* src must be moved into dest.                                */
  /* src always is an object. Dest can either be an object or    */
  /* a location.                                                 */

  /* Move is also used by the interpreter itself (to move the    */
  /* player around. Therefore the following code is not included */
  /* in the XeqMove() routine (When the interpreter calls Move() */
  /* there is no trigger involved.                               */

  int32_t  container;        /* Location or object that contains src. */
  contData *contained_objs = NULL;
  int32_t  i               = 0;
  int32_t  j               = 0;

  if (IsLocId(dest))
    contained_objs = &(loc_dir[dest-FIRST_LOCATION_ID].contained_objs);
  else
    /* It is an object. */
    contained_objs = &(obj_dir[dest-FIRST_OBJECT_ID].contained_objs);

  /* First, move src to destination. */

  /* Check if there is room in dest. */
  if (contained_objs->nr_of_objects >= MAX_CONTAINED_OBJECTS) {
    PrintError(68, NULL, NULL);
    return(ERROR);
  }

  /* Ok, there is enough room. */
  contained_objs->object_ids[contained_objs->nr_of_objects++] = src;

  /* Now remove src from the obj/loc it is contained in. */
  /* src will always be an object.                       */

  container = obj_dir[src-FIRST_OBJECT_ID].held_by;

  if (IsLocId(container))
    contained_objs = &(loc_dir[container-FIRST_LOCATION_ID].contained_objs);
  else
    /* It is an object. */
    contained_objs = &(obj_dir[container-FIRST_OBJECT_ID].contained_objs);

  while (i<contained_objs->nr_of_objects &&
         contained_objs->object_ids[i] != src)
    i++;

  if (i == contained_objs->nr_of_objects) {
    PrintError(69, NULL, NULL);
    return(ERROR);
  }

  for (j=i; j<contained_objs->nr_of_objects-1; j++)
    contained_objs->object_ids[j] = contained_objs->object_ids[j+1];
  contained_objs->object_ids[j] = NO_ID;

  (contained_objs->nr_of_objects)--;

  obj_dir[src-FIRST_OBJECT_ID].held_by = dest;

  return(OK);
}


int32_t Owns(owner, obj, depth)
 int32_t owner;
 int32_t obj;
 int32_t depth;
{
  /* It is easier to test if obj is held by owner, since */
  /* an object can only be held by one other obj/loc.    */

  /* Returns true if obj is contained in owner.   */
  /* Depth indicates the max containment depth.   */
  /* Zero or negative depth indicates to search   */
  /* all the way up the tree till location level. */

  /* In case of zero or negative initial depth, this   */
  /* loop will terminate when we reach location level. */

  /* First, test whether obj is a location. */
  if (IsLocId(obj))
    /* Locations cannot be contained in something. */
    return(ERROR);

  do {
    if (obj_dir[obj-FIRST_OBJECT_ID].held_by == owner)
      /* Found it. */
      return(OK);
    else {
      obj = obj_dir[obj-FIRST_OBJECT_ID].held_by;
      if (IsLocId(obj))
        /* We're at location level; not found. */
        return(ERROR);
    }
    depth--;
  } while (depth != 0);

  return(ERROR);
}


int32_t Shuffle(id)
 int32_t id;
{
  contData *contained_objs;                      /* pointer to objects to be reshuffled */
  int32_t  displacements[MAX_CONTAINED_OBJECTS]; /* array with random displacements     */
  int32_t  *objs = NULL;
  int      i    = 0;
  int      len  = 0;
  int      disp = 0;
  int32_t  temp = NO_ID;

  if (IsLocId(id)) {
    contained_objs = &loc_dir[id-FIRST_LOCATION_ID].contained_objs;
  }
  else {
    if (IsObjId(id)) {
      contained_objs = &obj_dir[id-FIRST_OBJECT_ID].contained_objs;
    }
    else {
      PrintError(70, NULL, NULL);
      return(ERROR);
    }
  }

  len = contained_objs->nr_of_objects;
  if (len < 3 )
    return(OK);

  /* calculate random displacements                      */
  /* displacement may not be equal to position in object */
  /* array to make sure all objects are repositioned     */
  for (i=0; i<len ; i++) {
    do {
      disp = rand();
      disp = disp%(len);
    }
    while (disp == i);
    displacements[i] = disp;
  }

  /* now shuffle the contained objects */
  /* algorithm: object[i] is switched with object[displacement[i]] */
  objs = contained_objs->object_ids;

  for (i=0; i<len; i++) {
    temp                   = objs[displacements[i]];
    objs[displacements[i]] = objs[i];
    objs[i]                = temp;
  }

  return(OK);
}


int32_t HasLight(id)
 int32_t id;
{
  /* returns OK if one of ids contained    */
  /* objects has the flag f_lit set.       */
  /* Id itself will not be checked, unless */
  /* it is a location.                     */

  /* IMPORTANT: f_lit set means that an object emits light,    */
  /* even if it is opaque. Thus a closed box with f_lit set    */
  /* is a light source to its surroundings, but not to its     */
  /* contained objects. If a closed box must also shine light  */
  /* on its contents, it must contain a light source.          */
  /* This does not count for locations. A location with f_lit  */
  /* set is considered to be lit.                              */

  contData *objs;
  int32_t  i;

  /* HIDDEN objects can still have light. */
  /*  if (TestCFlag(id, HIDDEN)) */
  /*    Hidden loc/obj, skip it and all its contained objects. */
  /*    return(ERROR); */

  if (IsLocId(id)) {
    if (TestCFlag(id, LIT))
      /* Location is lit; see also `IMPORTANT' remark. */
      return(OK);
    objs = &(loc_dir[id-FIRST_LOCATION_ID].contained_objs);
  }
  else
    /* Its an object. */
    objs = &(obj_dir[id-FIRST_OBJECT_ID].contained_objs);

  for (i=0; i<objs->nr_of_objects; i++) {
    if (TestCFlag(objs->object_ids[i], LIT))
      return(OK);
    else if (!TestCFlag(objs->object_ids[i], OPAQUE))
      if (HasLight(objs->object_ids[i]))
        return(OK);
  }

  return(ERROR);
}


int32_t SpanTree(from_loc, to_loc, nr_processed, tree)
 int32_t  from_loc;
 int32_t  to_loc;
 int32_t  nr_processed; /* Number of processed locations. */
 spanTree *tree;
{
  int32_t i;
  int32_t tree_index;
  int32_t exit_index;

  if (from_loc == to_loc)
    return(OK);

  if (nr_processed == nr_of_locs)

    /* There is no path. */
    return(ERROR);

  for (i=0; i<nr_of_directions; i++) {
    exit_index = (from_loc-FIRST_LOCATION_ID)*nr_of_directions+i;
    if (exit_data[exit_index] != NO_ID) {
      tree_index = exit_data[exit_index]-FIRST_LOCATION_ID;
      if (tree[tree_index].level != 1) {
        tree[tree_index].from_loc = from_loc;
        tree[tree_index].from_dir = i+first_direction_id;

        /* Next statement is needed to prevent looping in */
        /* case a path is not traversed in one time, but  */
        /* recalculated after some moves. Also increases  */
        /* performance.                                   */
        if (exit_data[exit_index] == to_loc)
          /* Found a path. */
          return(OK);
      }
    }
  }

  for (i=0; i<nr_of_directions; i++) {
    exit_index = (from_loc-FIRST_LOCATION_ID)*nr_of_directions+i;
    if (exit_data[exit_index] != NO_ID) {
      tree_index = exit_data[exit_index]-FIRST_LOCATION_ID;
      if (tree[tree_index].level != 1) {
        tree_index = exit_data[exit_index]-FIRST_LOCATION_ID;
        tree[tree_index].level = 1;
        nr_processed++;
        if (SpanTree(exit_data[exit_index], to_loc, nr_processed,
                                                         tree) == OK)
          return(OK);
      }
    }
  }
  return(ERROR);
}


int32_t IsLit(id)
 int32_t id;
{
  /* Returns OK if id or one of its containing objects (or the */
  /* current location) is within reach of a light source.      */
  /* Basically, what this routine does is going up in the      */
  /* containment tree as much as possible, and then call       */
  /* HasLight() to check down all contained objects for a      */
  /* light source.                                             */

  int32_t owner;

  if (IsObjId(id)) {
    owner = obj_dir[id-FIRST_OBJECT_ID].held_by;
    if (TestCFlag(owner, OPAQUE))
      return(HasLight(owner));
    else
      return(IsLit(owner));
  }
  else
    /* Its a location. */
    return(HasLight(id));
}


int32_t CountObjects(id, flag, flag_val, level)
 int32_t id;
 int32_t flag;
 int32_t flag_val;
 int32_t level;
{
  contData *source  = NULL;;
  int      i        = 0;
  int32_t  count    = 0;

  /* returns the number of contained objects in   */
  /* id with flag set to flag_val and             */

  /* level indicates the containment depth.       */
  /* level 0 means all levels.                    */

  if (IsObjId(id))
    source = &(obj_dir[id-FIRST_OBJECT_ID].contained_objs);
  else
    source = &(loc_dir[id-FIRST_LOCATION_ID].contained_objs);

  for (i=0; i<source->nr_of_objects; i++) {
    if (TestCFlag(source->object_ids[i], flag) == flag_val) {
      count++;
    }
    if (level != 1) {
      count += CountObjects(source->object_ids[i], flag, flag_val, level-1);
    }
  }
  return(count);
}


int32_t Synchronize(id, trigger_id, flag, flag_val, level, action_rec, subject_index)
 int32_t      id;
 int32_t      trigger_id;
 int32_t      flag;
 int32_t      flag_val;
 int32_t      level;
 usrActionRec *action_rec;
 int32_t      subject_index;
{
  int      i        = 0;
  int32_t  count    = 0;
  int32_t  cont     = 1;
  int32_t  index    = 0;
  int32_t  result   = NO_MATCH;
  int32_t* list     = _alloca((nr_of_objs + 1)*sizeof(int32_t));

  /* Calls trigger_id for each object that is    */
  /* contained in id with flag set to flag_val.  */

  /* level indicates the search depth.           */

  /* Returns the number of objects that executed */
  /* the trigger.                                */

  /* create an array of objects that must be considered */
  /* we do not use BuildClist() here, because that      */
  /* either goes one level deep or includes all objects */
  /* and locations                                      */

  BuildSyncList(id, list, &index, level);

  /* if no contained objects, index will be 0  */
  /* if contained objects, index will point to */
  /* the next free place in list[]             */

  while (i<index && cont) {
    if (TestCFlag(list[i], flag) == flag_val) {
      /* execute trigger_id */
      /* returns either: AGREE, DISAGREE, GET_XXX, QUIT or NO_MATCH. */
      result = XeqTrigger(list[i], trigger_id, action_rec, subject_index);
      switch (result) {
        case AGREE:
          count++;
          break;
        case DISAGREE:
        case QUIT:
          count++;
          cont = 0;
          break;
        case NO_MATCH:
          /* do not update the count */
          break;
        default:
          PrintError(71, &((resultStruct) {VALUE,result}), NULL);
          break;
      }
    }
    i++;
  }
  return(count);
}


void BuildSyncList(id, list, index, level)
 int32_t id;
 int32_t list[];  /* Must have length nr_of_objs+1. */
 int32_t *index;
 int32_t level;
{
  contData *source  = NULL;;
  int      i        = 0;

  if (IsObjId(id))
    source = &(obj_dir[id-FIRST_OBJECT_ID].contained_objs);
  else
    source = &(loc_dir[id-FIRST_LOCATION_ID].contained_objs);

  for (i=0; i<source->nr_of_objects; i++) {
    list[(*index)++] = source->object_ids[i];
    if (level != 1) {
      BuildSyncList(source->object_ids[i], list, index, level-1);
    }
  }
 return;
}


int32_t UpTree(id)
 int32_t id;
{
  /* This routines travels the containment tree upwards, until */
  /* either curr_loc or an opaque object has been encountered. */

  int32_t owner;
  /* We have to start with owner and not with id, because if id */
  /* is opaque itself, we must continue.                        */

  if (IsObjId(id)) {
    owner = obj_dir[id-FIRST_OBJECT_ID].held_by;
    if (TestCFlag(owner, OPAQUE))
      /* Cannot see through this object */
      return(owner);
    else
      return(UpTree(owner));
  }
  else
    /* Its curr_loc. */
    return(id);
}


int32_t CanSee(viewer, target)
 int32_t viewer;
 int32_t target;
{
  /* This function checks if both viewer and target are lit.    */
  /* If so, it uses both as a starting point to travel up the   */
  /* containment tree until either curr_loc or an opaque object */
  /* has been encountered. If both end at the same position,    */
  /* they can see each other.                                   */

  if (TestCFlag(target, HIDDEN))
    return(ERROR);

  if (!(IsLit(viewer) && IsLit(target)))
    /* Both must be lit. */
    return(ERROR);

  if (UpTree(viewer) == UpTree(target))
    /* They can see each other.    */
    return(OK);
  else
    /* They cannot see each other. */
    /* Both must be lit. */
    return(ERROR);
}


void Push(value)
 int32_t value;
{
  if (sp == STACK_SIZE) {
    /* Stack is full. */
    /* From here, things will probably get pretty bad. */
    /* STACK OVERFLOW */
    PrintError(72, NULL, NULL);
  }
  else
    stack[sp++] = value;
}


int32_t Pop()
{
  if (sp == 0) {
    /* Stack is empty. */
    /* STACK UNDERFLOW */
    PrintError(73, NULL, NULL);
    /* return anything you want here, things are fucked up badly. */
    return(-1);
  }
  else
    return(stack[--sp]);
}


void And()
{
  /* This is a boolean AND. */

  Push(Pop() & Pop());
}


void Or()
{
  /* This is a boolean OR. */
  Push(Pop() | Pop());
}


void Not()
{
  Push(!Pop());
}
