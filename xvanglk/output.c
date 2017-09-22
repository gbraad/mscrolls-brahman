
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>   /* malloc()  */
#include <string.h>
#include <ctype.h>    /* toupper() */
#include <stdint.h>

#include "glk.h"
#include "keyword.h"
#include "typedefs.h"
#include "output.h"

#define  CR (char) 10

/* The non-Glk version of the interpreter has a mechanism that   */
/* ensures that words that don't fit on the remainder of the     */
/* current line, get printed on the next line and do not end up  */
/* broken in half (using the char_count variable).               */
/* The Glk version of the interpreter does't need such a         */
/* mechanism, as word wrapping is controlled by hte Glk library. */
/* The non-Glk interpreter's mechanism could be used  for        */
/* grid-windows if we want to control word wrapping in a grid.   */

/*************************/
/* Function declarations */
/*************************/

int      CreateGlkWindows(void);
void     SetGlkStyles(void);
void     Output(char*);
void     PrintString(char*);
void     PrintWord(int32_t);
void     PrintExtendedSysDescr(extendedSysDescr*);
void     PrintSysDescr(sysDescr*);
void     PrintId(int32_t);
void     PrintNumber(int32_t);
int32_t  IsSpecialChar(char);
int32_t  IsVowel(char);
int32_t  Power(int32_t, int32_t);

void SetGlkStyles()
{
  /* set styles text color, background color and bold font size for styles Normal, Alert and Note */
  glk_stylehint_set(wintype_TextBuffer, style_Normal, stylehint_TextColor, 0x00FFFFFF);
  glk_stylehint_set(wintype_TextBuffer, style_Normal, stylehint_BackColor, 0x00000084);

  glk_stylehint_set(wintype_TextBuffer, style_Alert, stylehint_TextColor, 0x00FFFFFF);
  glk_stylehint_set(wintype_TextBuffer, style_Alert, stylehint_BackColor, 0x00000084);

  glk_stylehint_set(wintype_TextBuffer, style_Note, stylehint_TextColor, 0x00FFFFFF);
  glk_stylehint_set(wintype_TextBuffer, style_Note, stylehint_BackColor, 0x00000084);

  glk_stylehint_set(wintype_TextBuffer, style_Alert, stylehint_Size, -1);

  glk_stylehint_set(wintype_TextGrid, style_Normal, stylehint_TextColor, 0x00000084);
  glk_stylehint_set(wintype_TextGrid, style_Normal, stylehint_BackColor, 0x00FFFFFF);

  glk_stylehint_set(wintype_TextGrid, style_Alert, stylehint_TextColor, 0x00000084);
  glk_stylehint_set(wintype_TextGrid, style_Alert, stylehint_BackColor, 0x00FFFFFF);

  glk_stylehint_set(wintype_TextGrid, style_Note, stylehint_TextColor, 0x00000084);
  glk_stylehint_set(wintype_TextGrid, style_Note, stylehint_BackColor, 0x00FFFFFF);

  glk_stylehint_set(wintype_TextGrid, style_Normal, stylehint_Size, -1);
  glk_stylehint_set(wintype_TextGrid, style_Note, stylehint_Size, -1);
  glk_stylehint_set(wintype_TextGrid, style_Alert, stylehint_Size, -2);

  glk_stylehint_set(wintype_TextBuffer, style_Input, stylehint_Weight, 0);

  /* stylehints for user input representation */
  glk_stylehint_set(wintype_TextBuffer, style_Input, stylehint_TextColor, 0x00FFFFFF);
  glk_stylehint_set(wintype_TextBuffer, style_Input, stylehint_BackColor, 0x00000084);
}

int CreateGlkWindows()
{
   /* set styles text color, background color and bold font size for styles Normal, Alert and Note */
  SetGlkStyles();

  /* Open the main window. */
  mainwin = glk_window_open(0, 0, 0, wintype_TextBuffer, 1);
  if (!mainwin) {
    /* failed to open. */
    return(ERROR);
  }

  /* Set the current output stream to print to it. */
  glk_set_window(mainwin);

  /* Open a second window: a text grid, above the main window, three lines
      high. It is possible that this will fail also, but we accept that. */
  statuswin = glk_window_open(mainwin, winmethod_Above | winmethod_Fixed,
      3, wintype_TextGrid, 0);
  if (!statuswin) {
    return(ERROR);
  }

  return(OK);
}

void Output(line)
 char *line;
{
  if (debug_mode) {
    /* debug info will not be written to the transcript file */
    if (fprintf(debugfile, "%s", line) < 0) {
      printf("Output(): error writing to debug file.\n");
      /* not a severe error, continue but turn off debug mode */
      debug_mode = 0;
    }
  }
  else {
    /*printf("%s", line);*/
    glk_put_string(line);
    if (transcript) {
      if (fprintf(transcriptfile, "%s", line) < 0) {
        printf("Output(): error writing to transcript file.\n");
        /* not a severe error, continue but turn off transcript mode */
        transcript = 0;
      }
    }
  }
}

void PrintString(str)
 char *str;
{
  /* Syntax: <string> or                            */
  /*         [string]<-1><parameter>[string]        */

  /* Printstring takes into account the output line */
  /* length (OUTPUT_LINE_LEN) and makes sure that   */
  /* a word does not get cut in half thru wrapping  */

  int32_t i = 0;
  int32_t id1  = NO_ID;               /* Needed in case str contains an id. */
  int32_t id2  = NO_ID;
  int32_t type = NO_TYPE;             /* dummy for ConvSpecId()             */
  char word_buffer[OUTPUT_LINE_LEN];  /* To build up a full word before     */
                                      /* printing it                        */
  int32_t j = 0;                      /* index for word_buffer              */
  int32_t cont = 1;
  int32_t owner = NO_ID;
  char    *descr = NULL;              /* to retrieve a description          */

  attrInfo *attributes = NULL;        /* only need the pointer */
  int32_t  attribute_index = 0;

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
        sprintf(outputline, "%s", word_buffer);
        Output(outputline);
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
        PrintNumber(id1);
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
          PrintId(id1);
          article = 0;
        }

        if (IsLocId(id1) || IsObjId(id1)) {
          if (id2 == NO_ID) {
            PrintId(id1);
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
            PrintString("PrintString: error retrieving attribute info.\n");
            return;
          }
          else {
            switch (attributes[attribute_index].type) {
              case ATTR_ID:
                PrintString("PrintString() error. Attribute par in string may not contain another attribute.\n");
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
                PrintNumber(attributes[attribute_index].value);
                break;
              default:
                PrintId(attributes[attribute_index].value);
                article = 0;
                break;
            }
          }
        }
        if (IsCDescrId(id1) || IsLDescrId(id1)) {
          /* 05feb2017: forgot to test for NULL pointer. Crashed. */
          if ((descr = GetDescr(owner, id1)) != NULL) {
            PrintString(descr);
          }
        }
        if (IsWordId(id1)) {
          PrintWord(id1);
        }
      } /* else for NUMBER test */
    }
  } /* while */
}

void PrintWord(id)
 int32_t id;
{
  wordTable *wt = word_table;
  int32_t       i   = 0;

  /* in case of NONE we must print a white space,  */
  /* except when we are at the beginning of a line */

  /* Find id. */
  while (i<nr_of_words && wt[i].id != id)
    i++;

  if (i == nr_of_words) {
    /* Internal error; this shouldn't happen. */
    sprintf(outputline, "<unknown word id %d>", id);
    Output(outputline);
  }
  else
    PrintString(wt[i].print_word);
}

void PrintExtendedSysDescr(extended_sys_descr)
 extendedSysDescr *extended_sys_descr;
{
  PrintSysDescr(&(extended_sys_descr->part1));

  if (extended_sys_descr->connect_prepos != NO_ID) {
    PrintString(" ");
    PrintWord(extended_sys_descr->connect_prepos);
    PrintString(" ");

    /* PrintSysDescr() will not print the article for  */
    /* the second part of extended system descriptions */
    /* Because variable 'article' will be 0.           */
    if ((extended_sys_descr->part2).article != NO_ID) {
      PrintWord((extended_sys_descr->part2).article);
      PrintString(" ");
    }
    PrintSysDescr(&(extended_sys_descr->part2));
  }
  return;
}


void PrintSysDescr(descr)
 sysDescr *descr;
{
  wordTable *wt = word_table;
  int32_t id;
  int32_t i = 0;

  /* Caller must print the leading space, if needed.           */
  /* Since we have a recursive syntax check, the adjectives    */
  /* have been inserted backwards (last one first). Therefore, */
  /* we will print the adjectives in reverse order.            */

  /* check if we must print a different article than   */
  /* descr->article                                    */

  if (descr->article != NO_ID) {
    /* there is an article, so check if we */
    /* must print something                */
    switch (article) {
      case ARTICLE_THE:
        /* clear article or else the white space will */
        /* not be printed by PrintString()            */
        article = 0;
        PrintString("the ");
        break;
      case ARTICLE_A:
        /* We must print either 'a' or 'an'.             */
        /* Check if the next word starts with a vowel.   */
        /* Next word is either an adjective or the noun. */
        if ((i = descr->nr_of_adjectives-1) == -1)
          /* there are no adjectives, we must have the noun */
          id = descr->noun;
        else
          id = descr->adjectives[i];

        /* Find word with id. */
        while (i<nr_of_words && wt[i].id != id)
          i++;
        if (i == nr_of_words) {
          /* Internal error; this shouldn't happen. */
          sprintf(outputline, "\n<unknown word id %d>\n", id);
          Output(outputline);
        }
        else {
          /* clear article or else the white space will */
          /* not be printed by PrintString()            */
          article = 0;
          if (IsVowel( (wt[i].print_word)[0]))
            PrintString("an ");
          else
            PrintString("a ");
        }
        break;
      default:
        /* There is an article in the system description,  */
        /* but no %a or %the in the string, so don't print */
        /* the article.                                    */
        break;
    } /* switch */
  }

  for (i=descr->nr_of_adjectives-1; i>=0; i--) {
    PrintWord(descr->adjectives[i]);
    /* clear article, otherwise the white space */
    /* will not be printed by PrintString()     */
    article = 0;
    PrintString(" ");
  }

  if (descr->noun != NO_ID)
    PrintWord(descr->noun);
}

void PrintId(id)
 int32_t id;
{
  int32_t offset;
  int32_t type = NO_TYPE;
  int32_t num = 0;

  /* Remember whether it's a number. */
  if (id == VALUE || id == ORDINAL)
    num = 1;

  /* Convert in case it is a special id. */
  ConvSpecId(&id, &type);

  if (num)
    PrintNumber(id);
  else if (IsLocId(id)) {
    offset = id - FIRST_LOCATION_ID;
    if (loc_dir[offset].nr_of_dsys != 0)
      PrintExtendedSysDescr(&(loc_dir[offset].descr[0]));
    /* else do nothing */
  }
  else if (IsObjId(id)) {
    offset = id - FIRST_OBJECT_ID;
    if (obj_dir[offset].nr_of_dsys != 0)
      PrintExtendedSysDescr(&(obj_dir[offset].descr[0]));
    /* else do nothing */
  }
  else if (IsTimerId(id)) {
    offset = id - FIRST_TIMER_ID;
    sprintf(outputline, "%d", timers[offset].value);
    Output(outputline);
  }
  else
    /* Assume it's a word id. */
    PrintWord(id);
}

void PrintNumber(number)
 int32_t number;
{
  sprintf(outputline, "%d", number);
  Output(outputline);
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
