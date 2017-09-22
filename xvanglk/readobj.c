
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>     /* malloc(), free() */
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "readobj.h"

/*************************/
/* function declarations */
/*************************/

void       InitObj(objectInfo*);
objectInfo *ReadObject(int64_t);


/************************/
/* Function definitions */
/************************/

void InitObj(obj)
 objectInfo *obj;
{
  obj->descriptions = NULL;
  obj->action_recs  = NULL;
  obj->triggers     = NULL;
}


objectInfo *ReadObject(offset)
 int64_t offset;
{
  objectInfo    *obj;
  descrInfo     *last_descr      = NULL;
  descrInfo     *descr           = NULL;
  compActionRec *last_action_rec = NULL;
  compActionRec *action_rec      = NULL;
  triggerInfo   *last_trigg      = NULL;
  triggerInfo   *trigger         = NULL;
  int32_t code;

  /* Create space on heap */
  if ((obj = (objectInfo *) malloc(sizeof(objectInfo))) == NULL) {
    sprintf(outputline, "ReadObject(): error in malloc.\n");
    Output(outputline);
    return(NULL);
  }

  /* Fill struct with default values. */
  InitObj(obj);

  /* Set filepointer to start of object */
  if (fseek(datafile, offset, 0) == -1) {    /* 0 means ???? */
    sprintf(outputline, "ReadObject(): wrong offset.\n\n");
    Output(outputline);
    return(NULL);
  }

  if (!GetNextCode32(&code))
    return(NULL);

  while (1) {
    switch (code) {
      case ERROR:
        return(NULL);
      case ACTION_REC:
        /* Read the action_rec and trigger to execute.     */
        /* action_rec will be malloced by ReadActionRec(). */
        if ((action_rec = ReadActionRec(&code)) == NULL)
          return(NULL);

        if (last_action_rec == NULL)
          /* First action_rec for this object.             */
          obj->action_recs = action_rec;
        else
          last_action_rec->next = action_rec;
        last_action_rec = action_rec;

        /* Next code is returned by ReadActionRec(). */
        break;
     case END_OBJ:
        return(obj);
      default:
        /* Check for description/trigger id. */
        if (IsDescrId(code)) {
          /* It's a description id. */
          if ((descr = ReadDescr(&code)) == NULL)
            return(NULL);

          if (last_descr == NULL)
            /* First description for this object */
            obj->descriptions = descr;
          else
            last_descr->next = descr;
          last_descr = descr;

          /* Next code is returned by ReadDescr(). */
        }
        else if (IsTriggId(code)) {
          /* It's a trigger id. */
          if ((trigger = ReadTrigger(&code)) == NULL)
            return(NULL);
          if (last_trigg == NULL)
            /* First trigger for this object */
            obj->triggers = trigger;
          else
            last_trigg->next = trigger;
          last_trigg = trigger;

          /* Next code is returned by ReadTrigger(). This  */
          /* will either be an END_OBJ or TRIGG_ID code.   */
        }
        else {
          sprintf(outputline, "ReadObject(): FATAL ERROR, unexpected code: %d\n", code);
          Output(outputline);
          return(ERROR);
        }

        break;
    } /* switch */
  } /* while */
}

