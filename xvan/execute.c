
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
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "execute.h"

/*************************/
/* Function declarations */
/*************************/

int32_t      NextOpcode(int32_t**);
char         *GetDescr(int32_t, int32_t);
int32_t      GetTrigger(int32_t, int32_t, int32_t**);
int32_t      EvalCond(int32_t**, int32_t*, usrActionRec*, int32_t, int32_t*);
int32_t      SkipCond(int32_t**);
resultStruct Ite(int32_t**, int32_t, int32_t*, usrActionRec*, int32_t, int32_t*);
int32_t      MatchActionRec(compActionRec*, usrActionRec*, int32_t);
int32_t      ReplaceItObjects(usrActionRec*);
resultStruct XeqActionRec(usrActionRec*, int32_t*, int32_t);
resultStruct XeqPrologue(int32_t);
resultStruct XeqEpilogue(int32_t);
resultStruct XeqVerbDefault(usrActionRec*, int32_t);
resultStruct Execute(int32_t*, usrActionRec*, int32_t, int32_t*);
resultStruct XeqTrigger(int32_t, int32_t, usrActionRec*, int32_t);
int32_t      IsWordId(int32_t);
int32_t      IsVerbId(int32_t);
int32_t      IsLocId(int32_t);
int32_t      IsObjId(int32_t);
int32_t      IsCDescrId(int32_t);
int32_t      IsLDescrId(int32_t);
int32_t      IsCFlagId(int32_t);
int32_t      IsLFlagId(int32_t);
int32_t      IsDescrId(int32_t);
int32_t      IsCTriggId(int32_t);
int32_t      IsLTriggId(int32_t);
int32_t      IsTriggId(int32_t);
int32_t      IsTimerId(int32_t);
int32_t      IsTestFun(int32_t);
int32_t      IsIntAct(int32_t);
int32_t      IsCAttrId(int32_t);
int32_t      IsLAttrId(int32_t);

/************************/
/* Function definitions */
/************************/

int32_t NextOpcode(int **trigger)
 /* Caller expects the remainder of the trigger */
 /* to be returned in this parameter.           */
{
  return(*((*trigger)++));
}


char *GetDescr(int32_t owner, int32_t id)
{
  descrInfo   *dp;

  /* Check owner; must either be location or object id.  */
  if (!(IsLocId(owner) || IsObjId(owner))) {
    PrintError(7, &((resultStruct) {VALUE, owner}), NULL);
    return(NULL);
  }

  /* Find out whether the owner is already in memory.    */
  if (!InMem(owner))
    if (!Load(owner))
      return(NULL);

  /* OK, owner is in memory; now let's find the description. */
  if (IsLocId(owner))
    dp = locations[owner - FIRST_LOCATION_ID]->descriptions;
  else
    dp = objects[owner - FIRST_OBJECT_ID]->descriptions;

  /* Order for next AND is important. */
  while (dp != NULL && dp->id != id)
    dp = dp->next;

  if (dp == NULL) {
    /* Either undefined common description (ok),  */
    /* or local trigger error. In both cases,     */
    /* return NULL pointer.                       */
    if (IsLDescrId(id)) {
      PrintError(8, NULL, NULL);
    }
    return(NULL);
  }

  /* Found it. */
  return(dp->text);
}


int32_t GetTrigger(int32_t owner, int32_t id, int32_t *triggers[])
{
  triggerInfo *tp;
  int32_t     index;

  /* Common triggers that have default code, are overruled */
  /* if the location/object has also code defined for the  */
  /* common trigger.                                       */

  /* We return both the common and the overruling trigger  */
  /* (if any). In case the overruling trigger code returns */
  /* NO_MATCH, the common trigger will be executed by      */
  /* XeqTrigger().                                         */

  /* Init triggers. */
  triggers[0] = NULL;
  triggers[1] = NULL;

  /* Check for common trigger id. */
  if (IsCTriggId(id)) {
    /* It's a common trigger. */
    index = id - FIRST_COMMON_TRIGGER_ID;
    triggers[1] = common_trigs[index];  /* May be NULL. */
  }

  /* Check if code (local trigger or common trigger   */
  /* override) has been defined for the owner.        */

  /* Find out whether the owner is already in memory. */
  if (!InMem(owner))
    if (!Load(owner))
      return(ERROR);

  /* OK, owner is in memory; now let's find the trigger. */
  if (IsLocId(owner))
    tp = locations[owner - FIRST_LOCATION_ID]->triggers;
  else
    tp = objects[owner - FIRST_OBJECT_ID]->triggers;

  /* Order for next AND is important. */
  while (tp != NULL && tp->id != id)
    tp = tp->next;

  if (tp != NULL)
    /* Store the address of the overruling trigger code */
    triggers[0] = tp->code;
  else {
    /* Either undefined common trigger (ok), */
    /* or local trigger error.               */
    if (IsLTriggId(id)) {
      PrintError(9, NULL, NULL);
      return(ERROR);
    }
  }
  return(OK);
}


int32_t EvalCond(int32_t **trigger, int32_t *opcode, usrActionRec *action_rec, int32_t subject_index, int32_t *com_trig)
{
  int32_t eval_arr[EVAL_ARR_SIZE];
  int32_t eval_ptr = 0;         /* Needed to evaluate condition in */
                                /* reverse order.                  */

  /* First opcode of condition is in opcode parameter. */

  /* The conditions are stored in reverse order. For example:     */
  /* IF NOT(T1() AND T2()) is stored as IF NOT AND T1() T2()      */
  /* However it must be evaluated as: T2() T1() AND NOT.          */
  /* Therefore, we use eval_arr to evaluate the condition back-   */
  /* words. We could also have evaluated directly from **trigger, */
  /* but this causes problems finding the end of the condition.   */
  /* Also, we want to hide the **trigger structure in this env.   */

  while (1) {
    switch (*opcode) {

      /********************************/
      /* Cases for boolean operators. */
      /********************************/

      case AND: ;
      case OR : ;
      case NOT: ;
        eval_arr[eval_ptr++] = *opcode;
        break;
      case THEN:
        /* End of the condition. Return the next opcode to the   */
        /* caller, but first use eval_arr to evaluate the result */
        /* of the  condition.                                    */
          while (--eval_ptr >= 0)
            switch (eval_arr[eval_ptr]) {
              case AND:
                And();
                break;
              case OR:
                Or();
                break;
              case NOT:
                Not();
                break;
              default:
                /* This is either a 0 or a 1 (result of testfun). */
                Push(eval_arr[eval_ptr]);
                break;
            }

          /* Read the next opcode, return the result of the */
          /* evaluation and restore the stack.              */
          *opcode = NextOpcode(trigger);
          return(Pop());
      default:
        /* It is either a testfunction, or an attribute. */
        /* XeqTestFun() also does attributes.           */
        if (IsTestFun(*opcode) || IsCAttrId(*opcode) ||
                                  IsLAttrId(*opcode)) {
          eval_arr[eval_ptr++] = XeqTestFun(*opcode, trigger, action_rec, subject_index, com_trig);
          break;
        }
        else {
          PrintError(10, &((resultStruct) {VALUE, *opcode}), "EvalCond()");
          return(ERROR);
        } /* else */
    } /* switch */

    *opcode = NextOpcode(trigger);
  } /* while */
  /* No return here, makes TC happy. */
}


int32_t SkipCond(int32_t **trigger)
{
  int32_t opcode;

  while (1) {
    opcode = NextOpcode(trigger);

    switch (opcode) {
      case AND:
      case OR:
      case NOT:
        break;
      case THEN:
        /* End of the condition. Return the next opcode. */
        return(NextOpcode(trigger));
      default:
        if (IsTestFun(opcode) || IsCAttrId(opcode) || IsLAttrId(opcode))
          SkipFun(trigger);
        else {
          PrintError(10, &((resultStruct) {VALUE, opcode}), "SkipCond()");
          return(ERROR);
        }
        break;
    } /* switch */
  } /* while */

  /* No return here, makes TC happy. */
}


resultStruct Ite(int32_t **trigger, int32_t state, int32_t *opcode, usrActionRec *action_rec, 
                 int32_t subject_index, int32_t *com_trig)
{
  int32_t      new_state  = XEQ;
  resultStruct result     = {CONTINUE, 0};
  int32_t      first_else = 1;               /* The next ELSE will be the first. */

  /* The IF keyword has been read by the caller */

  if (state == XEQ) {
    /* First, evaluate the condition.                         */
    /* In EvalCond(), opcode is given as a parameter, since   */
    /* the function's return value indicates the eval result. */

    /* Read first opcode after the IF keyword.                */
    *opcode = NextOpcode(trigger);
    if (!EvalCond(trigger, opcode, action_rec, subject_index, com_trig))
      state = NO_XEQ;
  } /* if */
  else
    /* Skip the condition, don't evaluate. */
    *opcode = SkipCond(trigger);

  while (1) {
    switch (*opcode) {
      case ERROR: ;
      case END_OF_CODE:
        /* Ok to return CONTINUE here. */
        return(result);
      case IF:
        switch (state) {
          case XEQ:
            new_state = XEQ;
            break;
          case NO_XEQ:
            new_state = SKIP;
            break;
          case SKIP:
            new_state = SKIP;
            break;
          default:
            PrintError(13, NULL, "Ite()");
            return( (resultStruct) {ERROR, 0} );
        } /* switch */

        result = Ite(trigger, new_state, opcode, action_rec, subject_index, com_trig);
        if (result.tag != CONTINUE)
          /* Finished with this trigger. */
          return(result);
        break;
      case ELSE:
        if (first_else) {
          first_else = 0;
          switch (state) {
            case XEQ:
              state = NO_XEQ;
              break;
            case NO_XEQ:
              state = XEQ;
              break;
            case SKIP:
              state = SKIP;
              break;
            default:
              PrintError(13, NULL, "Ite()");
              /* Although there is an error, we will continue. */
              return(result);
          } /* switch */

          *opcode = NextOpcode(trigger);
        } /* if */
        else {
          return( (resultStruct) {CONTINUE, 0} );  /* Don't read new opcode. */}
      break;
      case ENDIF:
        *opcode = NextOpcode(trigger);
        return(result);
      default:
        /* It should be an internal action or an attribute.     */
        if (!(IsIntAct(*opcode) || IsCAttrId(*opcode) ||
                                   IsLAttrId(*opcode))) {
          PrintError(10, &((resultStruct) {VALUE, *opcode}), "Ite()");
          /* Although there is an error, the show must go on. */
          /* We might also return QUIT here.                  */
          return( (resultStruct) {DISAGREE, 0} );
        }

        /* It's an internal action.               */
        switch (state) {
          /* for comments see function Execute()  */
          case XEQ:
            result = XeqIntAct(*opcode, trigger, action_rec, subject_index);
            switch (*opcode) {
              case RAND:        /* returns a number    */
              case DISTANCE:    /* returns a number    */
              case COUNT:       /* returns a number    */
              case SYNCHRONIZE: /* returns a number    */
              case FIRSTDIR:    /* returns a direction */
              case DEST:        /* returns a direction */
                /* do nothing */
                break;
              default:
                if (result.tag != CONTINUE)
                  /* finished with this trigger */
                  return(result);
                break;
            }
            break;
          case NO_XEQ: ; /* same as SKIP */
          case SKIP:
            /* Skip the internal action here.     */
            /* Skipping internal actions doesn't  */
            /* return the next opcode.            */
            SkipFun(trigger);
          break;
        }
        *opcode = NextOpcode(trigger);
        break;
    } /* switch */
  } /* while */

  /* No return here. */
}


int32_t MatchActionRec(compActionRec *source, usrActionRec *target, int32_t index)
 /* source is object or location action_rec.              */
 /* target is user input.                                 */
 /* index tells which subject to compare from user input. */
{
  int32_t i     = 0;
  int32_t j     = 0;
  int32_t found = 0; /* Initially not found. */

  /* This function determines whether the source and target    */
  /* action records are equal. Adverbs and prepositions may be */
  /* in any order.  The `next' pointers are not matched.       */

  /* Source action_rec may contain special ids. These must be  */
  /* mapped to `real' ids before they can be compared.         */

  if (source->actor != target->actor &&
      (source->actor != ACTOR || target->actor == NO_ID))
    return(ERROR);

  if (source->action1 != target->action1 &&
      (source->action1 != ACTION || target->action1 == NO_ID))
    return(ERROR);

  if (source->action2 != target->action2 &&
      (source->action2 != ACTION || target->action2 == NO_ID))
    return(ERROR);

  if (source->q_word != target->q_word)
    return(ERROR);

  if (source->direction != target->direction &&
      (source->direction != DIR || target->direction == NO_ID))
    return(ERROR);

  if (source->subject != target->subject[index] &&
      (source->subject != SUBJECT || target->subject[index] == NO_ID))
    return(ERROR);

  if (source->specifier != target->specifier &&
      (source->specifier != SPECIFIER || target->specifier == NO_ID))
    return(ERROR);

  /* Test values. */
  if (source->value != target->value && source->value != VALUE)
    return(ERROR);

  if (source->ordinal != target->ordinal && source->ordinal != ORDINAL)
    return(ERROR);

  /* Test adverbs. */
  /* No need to test for doubles here, like in MatchSysDescr() in */
  /* module trnslate.c. We want a perfect match here (subsets are */
  /* rejected), so doubles will automatically be rejected.        */

  i     = 0;
  j     = 0;
  found = 0;

  if ((source->adverbs).nr_of_adverbs == (target->adverbs).nr_of_adverbs) {
    while (i<(source->adverbs).nr_of_adverbs) {
      while (!found && j<(target->adverbs).nr_of_adverbs) {
        if ((source->adverbs).adverb[i] == (target->adverbs).adverb[j])
          found = 1;
        else
          j++;
      } /* while */
      if (j == (target->adverbs).nr_of_adverbs)
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

  if ((source->prepositions).nr_of_prepositions ==
      (target->prepositions).nr_of_prepositions) {
    while (i<(source->prepositions).nr_of_prepositions) {
      while (!found && j<(target->prepositions).nr_of_prepositions) {
        if (source->prepositions.preposition[i] == PREPOS ||
            (source->prepositions).preposition[i] ==
            (target->prepositions).preposition[j])
          found = 1;
        else
          j++;
      } /* while */
      if (j == (target->prepositions).nr_of_prepositions)
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

  /* OK, it's a perfect match. */
  return(OK);
}


int32_t ReplaceItObjects(usrActionRec *action_rec)
{
  int32_t  it_value;
  int32_t  index;
  attrInfo *attributes;
  int      i;

  /* this function replaces occurrences of the    */
  /* 'it' object by the actual object or location */

  /* question: should we allow an 'it' value for  */
  /* the actor? No, it is unlikely that someone   */
  /* would enter something as "it, open the door" */

  /* first, retrieve the actual value of 'it' */
  if (!GetAttributeInfo(IT_ATTR, IT, &attributes, &index)) {
    return(ERROR);
  }

  if (attributes[index].value != NONE) {
    /* there is a value for 'it' */
    /* check if 'it' is a location or an object */
    if (!(attributes[index].type == LOC_ID || attributes[index].type == OBJ_ID)) {
      PrintError(97, NULL, NULL);
      return(ERROR);
    }
  }

  it_value = attributes[index].value;

  for (i=0; i<MAX_SUBJECTS; i++) {
    if (action_rec->subject[i] == IT)
      /* if it == none then replace with no_id for getsubject() to */
      /* kick in. No need to adjust nr_of_subjects in letsplay()   */
      /* because this will just count as an ambiguous subject.     */
      action_rec->subject[i] = (it_value == NONE ? NO_ID : it_value);
  }

  if (action_rec->specifier == IT) {
    action_rec->specifier = (it_value == NONE ? NO_ID : it_value);
  }

    return(OK);
}


resultStruct XeqActionRec(usrActionRec *action_rec, int32_t *xeq_list, int32_t subject_index)
{
  int32_t       id;
  compActionRec *arec       = NULL;
  int32_t       found       = 0;
  int32_t       i           = 0;
  resultStruct  result      = {NO_MATCH, 0};
  resultStruct  next_result = {NO_MATCH, 0};

  /* This function offers action_rec to the locations and objects   */
  /* in xeq_list, until one of them returns DISAGREE, QUIT, GET_XXX */
  /* or until the end of the list is reached.                       */

  /* Possible return values for this function are: AGREE, DISAGREE, */
  /* GET_XXX, NO_MATCH and QUIT (MATCH is not a return value).      */
  /* NO_MATCH is returned only if all executed triggers return it.  */

  id = xeq_list[0];
  while (id != NO_ID && i < (nr_of_locs+nr_of_objs) && result.tag != GET_SUBJECT &&
        result.tag != GET_SPECIFIER && result.tag != GET_ANSWER &&
        result.tag != DISAGREE && result.tag != QUIT) {

    /* Try to find a matching action record within id.  */
      if (!InMem(id))
      Load(id);

    if (IsLocId(id))
      /* It's a location.                                */
      /* Set arec to start of location's action records. */
      arec = locations[id-FIRST_LOCATION_ID]->action_recs;

    else if (IsObjId(id))
      /* It's an object.                               */
      /* Set arec to start of object's action records. */
      arec = objects[id-FIRST_OBJECT_ID]->action_recs;

    else {
      PrintError(11, NULL, NULL);
      return( (resultStruct) {DISAGREE, 0} );
    }

    while (arec != NULL && !found) {
      if (MatchActionRec(arec, action_rec, subject_index)) {
        /* Found it. */
        found = 1;

        /* Execute arec's trigger. */
        next_result = XeqTrigger(arec->execute[0], arec->execute[1], action_rec, subject_index);
        if (next_result.tag != NO_MATCH) {
          result.tag   = next_result.tag;
          result.value = next_result.value;
        }
      }
      else
        arec = arec->next;
    }
    /* Following section added on sep 3 2015 for Cloak of Darkness */
    /* In case no matching action record was found for this id,    */
    /* execute the default trigger if it was defined.              */
    if (!found) {
      next_result = XeqTrigger(id, DEFAULT_TRIGG, action_rec, subject_index);
        if (next_result.tag != NO_MATCH) {
          result.tag   = next_result.tag;
          result.value = next_result.value;
        }
    }

    /* Reset found variable for next loop. */
    found = 0;

    id = xeq_list[++i];
  } /* while */

  /* Ready */
  return(result);
}


resultStruct XeqPrologue(int32_t verb_id)
{
  /* This routine executes the verb's prologue.             */
  /* It must be called prior to the XeqActionRec() routine. */

  /* If there is a `PROLOGUE' defined, it will be in the    */
  /* first action_rec of the first verbInfo struct.         */

  /* See also XeqVerbDefault().                             */

  verbInfo *verb;

  if (!InMem(verb_id))
    switch (Load(verb_id)) {
      case ERROR:
        PrintError(12, &((resultStruct) {VALUE, verb_id}), "XeqPrologue()");
        return( (resultStruct) {QUIT, 0} );
      case NO_MATCH:
        return( (resultStruct) {NO_MATCH, 0} );
      default:
        /* OK */
        break;
    } /* switch */

  verb = verbs[verb_id-FIRST_VERB_ID];

  if ((verb->action_rec)->action1 == PROLOGUE)
    /* Execute() needs action_rec, subject_index and    */
    /* com_trig vars for functions runverb() and        */
    /* runcommon(). But as these functions are not      */
    /* allowed in verb code (this is checked by the     */
    /* compiler) we can set action_rec  and com_trig to */
    /* NULL and subject_index to -1 here, because they  */
    /* will not be used.                                */

    return(Execute(verb->code, NULL, -1, NULL));

  return( (resultStruct) {NO_MATCH, 0} );
}


resultStruct XeqEpilogue(int32_t verb_id)
{
  /* This routine executes the verb's epilogue.           */
  /* It must be called after the XeqActionRec() routine.  */

  /* If there is an `EPILOGUE' defined, it will be in the */
  /* first action_rec of either the first or the second   */
  /* verbInfo struct.                                     */

  /* See also XeqVerbDefault() and XeqEpilogue().         */

  verbInfo *verb;
  int32_t  *code;

  if (!InMem(verb_id))
    switch (Load(verb_id)) {
      case ERROR:
        PrintError(12, &((resultStruct) {VALUE, verb_id}), "XeqEpilogue()");
        return( (resultStruct) {QUIT, 0} );
      case NO_MATCH:
        return( (resultStruct) {NO_MATCH, 0} );
      default:
        /* OK */
        break;
    } /* switch */

  verb = verbs[verb_id-FIRST_VERB_ID];

  if ((verb->action_rec)->action1 == EPILOGUE)
    code = verb->code;
  else if (verb->next != NULL &&
           verb->next->action_rec->action1 == EPILOGUE)
    code = verb->next->code;
  else
    /* No epilogue. */
    return( (resultStruct) {NO_MATCH, 0} );

    /* Execute() needs action_rec, subject_index and    */
    /* com_trig vars for functions runverb() and        */
    /* runcommon(). But as these functions are not      */
    /* allowed in verb code (this is checked by the     */
    /* compiler) we can set action_rec  and com_trig to */
    /* NULL and subject_index to -1 here, because they  */
    /* will not be used.                                */

  return(Execute(code, NULL, -1, NULL));
}


resultStruct XeqVerbDefault(usrActionRec *action_rec, int32_t subject_index)
{
  /* This routine executes the verb default code for the        */
  /* action_rec's action. This routine should be called in      */
  /* case XeqActionRec() returns NO_MATCH.                      */

  /* Unlike XeqActionRec(), this routine is not recursive.      */
  /* This is due to the verbInfo datastructure (see below).     */

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

  /* If there is a `DEFAULT' defined, it will be in the         */
  /* last action_rec of the last verbInfo struct.               */
  /* DEFAULT denotes the default default-code (no               */
  /* action_rec match).                                         */

  verbInfo      *verb  = NULL;
  int32_t       cont   = 1;
  resultStruct  result = {NO_MATCH, 0};
  compActionRec *arec  = NULL;

  if (!InMem(action_rec->action1))
    switch (Load(action_rec->action1)) {
      case ERROR:
        PrintError(12, &((resultStruct) {VALUE, action_rec->action1}), "XeqVerbDefault()");
        return( (resultStruct) {QUIT, 0} );
      case NO_MATCH: /* printf("Load geeft no_match\n"); */
        return( (resultStruct) {NO_MATCH, 0} );
      default: /* printf("Load geeft ok\n"); */
        /* OK */
        break;
    } /* switch */

  verb = verbs[(action_rec->action1)-FIRST_VERB_ID];

  /* Try to find a matching action record within verbInfo. */
  while (cont && verb != NULL) {
    arec = verb->action_rec;
    while (cont && arec != NULL) {
      if (arec->action1 == DEFAULT ||
          MatchActionRec(arec, action_rec, subject_index)) {
        result = Execute(verb->code, action_rec, subject_index, NULL);
        cont = 0;
      } /* if */
      arec = arec->next;
    } /* while */
    verb = verb->next;
  } /* while */

  /* Ready */
  return(result);
}


resultStruct Execute(int32_t *trigger, usrActionRec *action_rec, int32_t subject_index, int32_t *com_trig)
{
  /* This function assumes owner is in memory. */

  /* Executing a trigger returns either: AGREE, */
  /* DISAGREE, GET_XXX, QUIT or NO_MATCH.       */

  int32_t      state     = XEQ;
  int32_t      new_state = XEQ;
  int32_t      opcode;
  resultStruct result    = {AGREE, 0}; /* By default, a trigger returns agree. */

  opcode = NextOpcode(&trigger);

  while (1) {
    switch (opcode) {
      case END_OF_CODE:
        return((result.tag == CONTINUE? (resultStruct) {AGREE, 0} : result));
      case IF:
        switch (state) {
          case XEQ:
            new_state = XEQ;
            break;
          case NO_XEQ:
            new_state = SKIP;
            break;
          case SKIP:
            new_state = SKIP;
            break;
          default:
            PrintError(13, NULL, "Execute()");
            return( (resultStruct) {QUIT, 0} );
        } /* switch */

        result = Ite(&trigger, new_state, &opcode, action_rec, subject_index, com_trig);
        if (result.tag != CONTINUE)
          /* Finished with this trigger. */
          return(result);
        break;
      default:
        /* It should be an internal action or a attribute. */
        if (!(IsIntAct(opcode) || IsCAttrId(opcode) ||
                                  IsLAttrId(opcode))) {
          PrintError(12, &((resultStruct) {VALUE,opcode}), "Execute()");
          return( (resultStruct) {QUIT, 0} );
        }

        /* It's an internal action.                     */
        /* DO WE REALLY NEED THIS SWITCH STATEMENT????? */
        /* Yes, is for outside if-then-else loops       */
        switch (state) {
          case XEQ:
            /* Execute the internal action here.  */
            /* XeqIntAct() also does attributes.  */
            /* Executing internal actions doesn't */
            /* return the next opcode.            */

            result = XeqIntAct(opcode, &trigger, action_rec, subject_index);

            /* Following are internal actions that return something */
            /* other than agree/disagree/nomatch or quit (like a    */
            /* number). In case we test the return value against    */
            /* CONTINUE with these internal action as the trigger   */
            /* execution will stop. Hence the switch statement.     */

            /* Most of the following will only be used in           */
            /* assignments or as a function parameter where we will */
            /* not have this issue, but just in case they are all   */
            /* added to the switch statement                        */

            switch (opcode) {
              case RAND:        /* returns a number    */
              case DISTANCE:    /* returns a number    */
              case COUNT:       /* returns a number    */
              case SYNCHRONIZE: /* returns a number    */
              case SCORE:       /* returns a number    */
              case FIRSTDIR:    /* returns a direction */
              case DEST:        /* returns a location  */
                /* do nothing */
                break;
              default:
                if (result.tag != CONTINUE)
                  /* finished with this trigger */
                  return(result);
                break;
            }
            break;
          case NO_XEQ: ; /* same as SKIP */
          case SKIP:
            /* Skip the internal action here.     */
            /* Skipping internal actions doesn't  */
            /* return the next opcode.            */
            SkipFun(&trigger);
          break;
        }
        opcode = NextOpcode(&trigger);
        break;
    } /* switch */
  } /* while */
  /* No return here */
}


resultStruct XeqTrigger(int32_t owner, int32_t id, usrActionRec *action_rec, int32_t subject_index)
{
  int32_t      *triggers[2];                        /* Pointers to start of trigger code. */
  resultStruct result            = {NO_MATCH, 0};
  int32_t      old_active_entity = active_entity;

  if (!GetTrigger(owner, id, triggers))
    return( (resultStruct) {QUIT, 0} );

  /* triggers[0] is override code (defined with loc/obj). */
  /* triggers[1] is common trigger code.                  */

  /* Set current active entity. */
  active_entity = owner;

  /* 08 oct 2017 we now have testfunction runcommon() that  */
  /* can be used from a local trigger to execute the common */
  /* trigger that it overwrites. A bit like nomatch(), only */
  /* runcommon() returns to the local trigger when it       */
  /* finishes, whereas nomatch() doesn't.                   */
  /* So, we need a way to retrieve the common trigger id    */
  /* when executing a local trigger. We do this by passing  */
  /* it with the Execute() function.                        */

  /* First execute the override code (if any). */
  if (triggers[0] != NULL) {
    /* there is override code (local trigger). Pass on  */
    /* the pointer to the common trigger to Execute()   */
    result = Execute(triggers[0], action_rec, subject_index, triggers[1]);
    if (result.tag == NO_MATCH)
      /* Execute the common trigger code, if it exists. */
      if (triggers[1] != NULL)
        /* there is a common trigger. From the common   */
        /* trigger we may not call runcommon(), so we   */
        /* set the com_trig par to NULL. When they do   */
        /* try to call runcommon(), the interpreter     */
        /* will throw a runtime error                   */
        /* Change made august 6 1995. */
        result = Execute(triggers[1], action_rec, subject_index, NULL);
  }
  else
    /* Execute the common trigger code (if any). */
    if (triggers[1] != NULL)
      /* Change made august 6 1995.    */
      /* return(Execute(triggers[1])); */
      result = Execute(triggers[1], action_rec, subject_index, NULL);

  /* Restore the active entity. */
  active_entity = old_active_entity;

  return(result);
}


int32_t IsWordId(int32_t id)
{
  if ((id>=FIRST_VERB_ID) && (id<=LAST_WORD_ID))
    return(OK);

  return(ERROR);
}


int32_t IsVerbId(int32_t id)
{
  if ((id>=FIRST_VERB_ID) && (id<=LAST_VERB_ID))
    return(OK);

  return(ERROR);
}

int32_t IsLocId(int32_t id)
{
  switch (id) {
  /* test for special location ids */
    case CURR_LOC: ;
      return(OK);
    default:
      if ((id>=FIRST_LOCATION_ID) && (id<=LAST_LOCATION_ID))
        return(OK);
      break;
  } /* switch */

  return(ERROR);
}


int32_t IsObjId(int32_t id)
{
  switch (id) {
    /* test for special object ids */
    case ACTOR:
    case SUBJECT:
    case SPECIFIER:
      return(OK);
    default:         /* test for normal object ids */
      if ((id>=FIRST_OBJECT_ID) && (id<=LAST_OBJECT_ID))
        return(OK);
      break;
  }  /* switch */
  return(ERROR);
}


int32_t IsCDescrId(int32_t id)
{
  if ((id>=FIRST_COMMON_DESCR_ID) && (id<=LAST_COMMON_DESCR_ID))
    return(OK);
  return(ERROR);
}


int32_t IsLDescrId(int32_t id)
{
  if ((id>=FIRST_LOCAL_DESCR_ID) && (id<=LAST_LOCAL_DESCR_ID))
    return(OK);
  return(ERROR);
}


int32_t IsDescrId(int32_t id)
{
  return(IsCDescrId(id) || IsLDescrId(id));
}


int32_t IsCFlagId(int32_t id)
{
  if ((id>=FIRST_COMMON_FLAG_ID) && (id<=LAST_COMMON_FLAG_ID))
    return(OK);
  return(ERROR);
}


int32_t IsLFlagId(int32_t id)
{
  if ((id>=FIRST_LOCAL_FLAG_ID) && (id<=LAST_LOCAL_FLAG_ID))
    return(OK);
  return(ERROR);
}


int32_t IsCTriggId(int32_t id)
{
  if ((id>=FIRST_COMMON_TRIGGER_ID) && (id<=LAST_COMMON_TRIGGER_ID))
    return(OK);
  return(ERROR);
}


int32_t IsLTriggId(int32_t id)
{
  if ((id>=FIRST_LOCAL_TRIGGER_ID) && (id<=LAST_LOCAL_TRIGGER_ID))
    return(OK);
  return(ERROR);
}


int32_t IsTriggId(int32_t id)
{
  return(IsCTriggId(id) || IsLTriggId(id));
}


int32_t IsCAttrId(int32_t code)
{
  if ((code >= FIRST_COMMON_ATTR_ID) && (code <= LAST_COMMON_ATTR_ID))
    return (OK);
  else
    return (ERROR);
}


int32_t IsLAttrId(int32_t code)
{
  if ((code >= FIRST_LOCAL_ATTR_ID) && (code <= LAST_LOCAL_ATTR_ID))
    return (OK);
  else
    return (ERROR);
}


int32_t IsTimerId(int32_t id)
{
  if ((id>=FIRST_TIMER_ID) && (id<=LAST_TIMER_ID))
    return(OK);
  return(ERROR);
}


int32_t IsTestFun(int32_t code)
{
  if ((code > LOWER_BOUND_TESTFUN) && (code < UPPER_BOUND_TESTFUN))
    return (OK);
  else
    return (ERROR);
}


int32_t IsIntAct(int32_t code)
{
  if ((code > LOWER_BOUND_INT_ACT) && (code < UPPER_BOUND_INT_ACT))
    return (OK);
  else
    return (ERROR);
}
