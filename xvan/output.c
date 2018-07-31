
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
#include <stdlib.h>   /* malloc()  */

#ifndef __APPLE__
#include <malloc.h>   /* _alloca   */
#endif

#include <string.h>
#include <ctype.h>    /* toupper() */
#include <stdint.h>

#include "defs.h"
#include "keyword.h"
#include "typedefs.h"
#include "output.h"
#include "json.h"

#define CR (char) 10


/*************************/
/* Function declarations */
/*************************/

void    Output(char*, int);
void    BuildOutputJson(char*, char*);
void    PrintString(char*, int);
void    PrintWord(int32_t, int);
void    PrintExtendedSysDescr(extendedSysDescr*, int);
void    PrintSysDescr(sysDescr*, int);
void    PrintArticle(sysDescr*, int32_t, int);
void    PrintId(int32_t, int);
void    PrintNumber(int32_t, int);
int32_t IsSpecialChar(char);
int32_t IsVowel(char);
int32_t Power(int32_t, int32_t);

/* 03Feb2018: with function try() we allow to */
/* mute output, so nothing will be printed.   */
/* When mute is on, we must not change the    */
/* char_count var because we don't print      */
/* Therefore, each function in this module    */
/* that manipulates char_count, has an early  */
/* exit when muted == 1                       */


void Output(line, use_json)
 char *line;
 int  use_json;
{
  char *json_text = _alloca((strlen(line)+JSON_TEXT_OVERHEAD + 1)*sizeof(char));

  if (muted == 1 && !use_json) {
    return;
  }

  /* check if it is a json message from the story to the front-end */
  if (use_json) {
    /* we must add line to the json message parameter */
    /* no test for NULL resul, we will always return directly */
    json_msg_from_story = AddToString(json_msg_from_story, line);
    return;
  }

  if (debug_mode) {
    /* debug info will not be written to the transcript file */
    if (fprintf(debugfile, "%s", line) < 0) {
      PrintError(47, NULL, NULL);
      /* not a severe error, continue but turn off debug mode */
      debug_mode = 0;
    }
  }
  else {
    BuildOutputJson(line, json_text);
    ifi_emitResponse(json_text);
    /*printf("%s", line);*/
    if (transcript) {
      if (fprintf(transcriptfile, "%s", line) < 0) {
        PrintError(44, NULL, NULL);
        /* not a severe error, continue but turn off transcript mode */
        transcript = 0;
      }
    }
  }
}


void BuildOutputJson(line, json_line)
 char *line;
 char *json_line;
{
  /* this function wraps the text in a json string               */
  /* example: "drop lamp" will wrapped to {"text" : "drop lamp"} */

  /* build the output json */
  strcpy(json_line, "{\"text\":\"");
  strcat(json_line, line);
  strcat(json_line, "\"}");

  return;
}


void PrintString(str, use_json)
 char *str;
 int  use_json;
{
  /* Syntax: <string> or                            */
  /*         [string]<-1><parameter>[string]        */

  /* Printstring takes into account the output line */
  /* length (OUTPUT_LINE_LEN) and makes sure that   */
  /* a word does not get cut in half thru wrapping  */

  int32_t i    = 0;
  int32_t id1  = NO_ID;               /* Needed in case str contains an id. */
  int32_t id2  = NO_ID;
  int32_t type = NO_TYPE;             /* dummy for ConvSpecId()             */
  int     len = strlen(str)+2;        /* hugh told +2 and not +1 */
  char    *word_buffer = _alloca(len*sizeof(char));
                                      /* printing it                        */
  int32_t j = 0;                      /* index for word_buffer              */
  int32_t cont = 1;
  int32_t owner = NO_ID;
  char    *descr = NULL;              /* to retrieve a description          */

  attrInfo *attributes = NULL;        /* only need the pointer */
  int32_t  attribute_index = 0;

  /* outputline is global and otherwise is not freed properly, so */
  /* use a local buffer                                           */
  char *o_line = (char *) _alloca(len*sizeof(char));

  if (muted == 1 && use_json == 0)
    return;

  word_buffer[0] = '\0';

  /* Concatenate single chars to a word and then check if it still fits on  */
  /* the current line. If not, start at the next line. ALWAYS stop at a -1. */
  while (cont) {
    if (str[i] == '\0')
      cont = 0;
    if (str[i] != (char)-1) {
      word_buffer[j++] = (capital == 1 ? (char) toupper((int32_t) str[i]) : str[i]);

      /* check to see if capital variable must be set or cleared */
      if (str[i] == '.' || str[i] == '?' || str[i] == '!')
        /* after a '.', '?' or '!' a new sentence starts */
        capital = 1;
      else if (str[i] != ' ' && str[i] != '\0')
        /* a space may come after a dot; to be on the safe side, never change the */
        /* value of capital after a white space or a NULL character.              */
        capital = 0;

      if (IsSpecialChar(str[i])) {
        /* if we have a white space and a pending article, the white space */
        /* must not be processed to prevent double spaces when the object  */
        /* or location does not have an article associated with it.        */
        if (str[i] == SPACE && article != 0) {
          /* skip the space that was already added to str[] */
          j--;
        }

        /* check if the whole word fits on the remainder of the line       */
        word_buffer[j] = '\0';
        sprintf(o_line, "%s", word_buffer);
        Output(o_line, use_json);
        j = 0;
      }
    i++;
    }
    else { /* (char) -1 */
      /* An id is coming next.                                   */
      /* Id syntax: -1 TAG MSB LSB (4 bytes/chars total length). */
      /* -1 indicates a parameter is coming.                     */
      /* TAG left nibble indicates whether MSB should be read as */
      /* a 0 (nibble is 1) or not (nibble is 2).                 */
      /* TAG right nibble indicates same as above for LSB.       */
      /* MSB is MSB value of parameter id.                       */
      /* LSB is LSB value of parameter id.                       */

      /* We must cast to unsigned char because we do not want to */
      /* use negative numbers in out math. If we change the      */
      /* type of str to unsigned, the check for -1 will fail.    */

      /* 22 jan 2017: we now also allow description ids and      */
      /* attribute ids. But these must have an owner. Owner is   */
      /* a location, an object or THIS keyword,                  */
      /* After a location id, an object id or a THIS keyword     */
      /* there will always follow a second id. In case this id   */
      /* ia a description or an attribute, the preceding id is   */
      /* its owner. If the second id is NO_ID, then the          */
      /* preceding id was a single occurrence and must be printed */

      id1  = ( (int32_t) (str[i+1] & 0xF0) ==  0x10? 0: (int32_t) (unsigned char) (str[i+2])*256);
      id1 += ( (int32_t) (str[i+1] & 0x0F) ==  0x01? 0: (int32_t) (unsigned char) str[i+3]);

      /* Skip the -1, the tag and the 2 bytes for the id.*/
      i += 4;

      /* Convert in case it is a special id. */
      ConvSpecId(&id1, &type);

      /* 02march2017: we must first check for a number */
      /* if we don't and it is a number with a value that    */
      /* is equal to a location id, object is, timer id, etc */
      /* things will go wrong.                               */
      if (type == NUMBER) {
        PrintNumber(id1, use_json);
      }
      else {
        /* not a number */
        if (IsLocId(id1) || IsObjId(id1)) {
          /* Another parameter is coming */
          /* loc/obj/THIS description    */
          /* loc/obj/THIS attribute      */
          /* loc/obj/THIS NO_ID          */

          id2  = ( (int32_t) (str[i+1] & 0xF0) ==  0x10? 0: (int32_t) (unsigned char) (str[i+2])*256);
          id2 += ( (int32_t) (str[i+1] & 0x0F) ==  0x01? 0: (int32_t) (unsigned char) str[i+3]);

          /* Skip the -1, the tag and the 2 bytes for the id.*/
          i += 4;
        }

        if (id1 == ARTICLE_A || id1 == ARTICLE_THE) {
          /* we must remember to print an article if the next */
          /* word is an object or location id                 */
          article = id1;
        }

        if (IsTimerId(id1)) {
          PrintId(id1, use_json);
          article = 0;
        }

        if (IsLocId(id1) || IsObjId(id1)) {
          if (id2 == NO_ID) {
            PrintId(id1, use_json);
            article = 0;
          }
          else {
            /* id2 must be a description or an attribute */
            /* now, id1 is the owner                     */
            /* Now set owner and id1 so we can use the   */
            /* same code for a single descr or attr      */
            owner = id1;
            id1   = id2;
          }
        }

        if (IsCAttrId(id1) || IsLAttrId(id1)) {
          if (!GetAttributeInfo(id1, owner, &attributes, &attribute_index)) {
            PrintError(48, NULL, NULL);
            return;
          }
          else {
            switch (attributes[attribute_index].type) {
              case ATTR_ID:
                PrintError(49, NULL, NULL);
                break;
              case COMMON_DESCRS: ;  /* common description */
              case DESCR_ID:         /* local description */
                id1 = attributes[attribute_index].value;
                /* Owner is the owner of the attribute, but */
                /* since the value is a description id we   */
                /* must also retrieve the description owner */
                /* from the attributes struct.              */
                owner = attributes[attribute_index].value_owner;
                break;
              case NUMBER:
                PrintNumber(attributes[attribute_index].value, use_json);
                break;
              default:
                PrintId(attributes[attribute_index].value, use_json);
                article = 0;
                break;
            }
          }
        }
        if (IsCDescrId(id1) || IsLDescrId(id1)) {
          /* 05feb2017: forgot to test for NULL pointer. Crashed. */
          if ((descr = GetDescr(owner, id1)) != NULL) {
            PrintString(descr, use_json);
          }
        }
        if (IsWordId(id1)) {
          PrintWord(id1, use_json);
        }
      } /* else for NUMBER test */
    }
  } /* while */
}


void PrintWord(id, use_json)
 int32_t id;
 int     use_json;
{
  wordTable *wt = word_table;
  int32_t i = 0;

  /* in case of NONE we must print a white space,  */
  /* except when we are at the beginning of a line */

  /* Find id. */
  while (i<nr_of_words && wt[i].id != id)
    i++;

  if (i == nr_of_words) {
    /* Internal error; this shouldn't happen. */
    PrintError(50, &((resultStruct) {VALUE, id}), NULL);
  }
  else
    PrintString(wt[i].print_word, use_json);
}


void PrintExtendedSysDescr(extended_sys_descr, use_json)
 extendedSysDescr *extended_sys_descr;
 int              use_json;
{
  PrintSysDescr(&(extended_sys_descr->part1), use_json);

  if (extended_sys_descr->connect_prepos != NO_ID) {
    PrintString(" ", use_json);
    PrintWord(extended_sys_descr->connect_prepos, use_json);
    PrintString(" ", use_json);

    /* PrintSysDescr() will not print the article for  */
    /* the second part of extended system descriptions */
    /* Because variable 'article' will be 0.           */
    if ((extended_sys_descr->part2).article != NO_ID) {
      PrintWord((extended_sys_descr->part2).article, use_json);
      PrintString(" ", use_json);
    }
    PrintSysDescr(&(extended_sys_descr->part2), use_json);
  }
  return;
}


void PrintSysDescr(descr, use_json)
 sysDescr *descr;
 int      use_json;
{
  int32_t i = 0;

  /* Caller must print the leading space, if needed.           */
  /* Since we have a recursive syntax check, the adjectives    */
  /* have been inserted backwards (last one first). Therefore, */
  /* we will print the adjectives in reverse order.            */

  /* print the article (or not) */
  PrintArticle(descr, article, use_json);

  for (i=descr->nr_of_adjectives-1; i>=0; i--) {
    PrintWord(descr->adjectives[i], use_json);

    /* clear article, otherwise the white space */
    /* will not be printed by PrintString()     */
    article = 0;
    PrintString(" ", use_json);
  }

  if (descr->noun != NO_ID)
    PrintWord(descr->noun, use_json);
}


void PrintArticle(descr, type, use_json)
 sysDescr *descr;
 int32_t  type;
 int      use_json;
{
  switch (story_info.story_language) {
    case NL:
      NL_PrintArticle(descr, type, use_json);
      break;
    case ENG:
      ENG_PrintArticle(descr, type, use_json);
      break;
    default:
      /* unknown language, use English */
      ENG_PrintArticle(descr, type, use_json);
      break;
  }
}


void PrintId(id, use_json)
 int32_t id;
 int   use_json;
{
  int32_t offset;
  int32_t type             = NO_TYPE;
  int32_t num              = 0;
  int32_t remember_capital = 0;

  /* Remember whether it's a number. */
  if (id == VALUE || id == ORDINAL)
    num = 1;

  /* Convert in case it is a special id. */
  ConvSpecId(&id, &type);

  if (num)
    PrintNumber(id, use_json);
  else if (IsLocId(id)) {
    offset = id - FIRST_LOCATION_ID;
    if (loc_dir[offset].nr_of_dsys != 0) {
      if (!story_info.autolink && !use_json) {
        /* remember the capital status */
        remember_capital = capital;
        PrintString("[", 0);
        capital = remember_capital;
      }
      PrintExtendedSysDescr(&(loc_dir[offset].descr[0]), use_json);
      if (!story_info.autolink && !use_json) {
        PrintString("]", 0);
        PrintString("(use ", 0);
        PrintExtendedSysDescr(&(loc_dir[offset].descr[0]), use_json);
        PrintString(")", 0);
      }
    }
    /* else do nothing */
  }
  else if (IsObjId(id)) {
    offset = id - FIRST_OBJECT_ID;
    if (obj_dir[offset].nr_of_dsys != 0) {
      if (!story_info.autolink && !use_json) {
        /* remember the capital status */
        remember_capital = capital;
        PrintString("[", 0);
        capital = remember_capital;
      }
      PrintExtendedSysDescr(&(obj_dir[offset].descr[0]), use_json);
      if (!story_info.autolink && !use_json) {
        PrintString("]", 0);
        PrintString("(use ", 0);
        PrintExtendedSysDescr(&(obj_dir[offset].descr[0]), use_json);
        PrintString(")", 0);
      }
    }
    /* else do nothing */
  }
  else if (IsTimerId(id)) {
    offset = id - FIRST_TIMER_ID;
    sprintf(outputline, "%d", timers[offset].value);
    Output(outputline, use_json);
  }
  else
    /* Assume it's a word id. */
    PrintWord(id, use_json);
}


void PrintNumber(number, use_json)
 int32_t number;
 int     use_json;
{
  sprintf(outputline, "%d", number);
  Output(outputline, use_json);
}

int32_t IsSpecialChar(ch)
 char ch;
{
  if ( (((int32_t) ch>=97) && ((int32_t) ch<=122)) ||   /* a..z */
       (((int32_t) ch>=65) && ((int32_t) ch<=90))  ||   /* A..Z */
       (((int32_t) ch>=48) && ((int32_t) ch<=57))  ||   /* 0..9 */
       (ch == '_') )                          /* for identifiers */
    return(0);
  else
    return(1);
}


int32_t IsVowel(ch)
 char ch;
{
  switch (ch) {
    case 'a':
    case 'A':
    case 'e':
    case 'E':
    case 'i':
    case 'I':
    case 'o':
    case 'O':
    case 'u':
    case 'U':
    case 'y':
    case 'Y':
      return(OK);
      break;
    default:
      return(ERROR);
  }
}


int32_t Power(i, j)
 int32_t i;
 int32_t j;
{
  /* Calculates i raised to the power of j. */
  /* Works only for j>0.                    */

  if (j == 0)
    return(1);
  else
    return(i*Power(i, j-1));
}
