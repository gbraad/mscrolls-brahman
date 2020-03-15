
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
/*                                                                      */
/* <marnix@xvan.nl>                                                     */
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
#include <stdlib.h>  /* malloc(), realloc(), free() */
#include <string.h>
#include <stdint.h>
#include <limits.h>       /* PATH_MAX */
#include "keyword.h"
#include "typedefs.h"
#include "fileio.h"

/***********/
/* globals */
/***********/

dirData dir_data;      /* Used by StoreDirInfo. */

/*************************/
/* function declarations */
/*************************/

FILE     *fopen_path(const char*, const char*, const char*);
static   int NextChar(FILE*);
void     ConvertFilePath(char**);
int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**, fileList**);
int32_t  GetPrevFile(char[], FILE**, fileList**);
int32_t  AddToFileList(char*, FILE*, fileList**);
int32_t  CheckForLoop(char*, fileList*);
char     *Concat(char*,char*);
int32_t  IsSpecialChar(char);
char     *HandleSpecialChar(char*, FILE*);
int32_t  ConvertBackSlashChar(char*, FILE*);
void     SkipSpacesPlusCR(char*, FILE*);
void     SkipComment(char*, FILE*);
void     IdToChar(int32_t, char*);
char     *ReadParInString(int32_t, char*, char*, FILE*, fileList**);
char     *ReadString(char*, int32_t, int16_t*, int32_t, FILE*, fileList**);
char     *Read1String(char*, int32_t, int16_t*, int32_t, FILE*, fileList**);
char     *ReadWord(char*, FILE*);
char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**, fileList**);
int32_t  StoreOffsets(void);
int32_t  StoreKeyword(int32_t);
int32_t  StoreInt16(int16_t);
int32_t  StoreInt32(int32_t);
int32_t  StoreInt64(int64_t);
int32_t  StoreId(int32_t);
int32_t  StoreString(char*);
int32_t  StoreExtendedSysDescr(extendedSysDescr*);
int32_t  StoreSysDescr(sysDescr*);
int32_t  StoreContData(contData*);
int32_t  StoreAdverbInfo(adverbInfo*);
int32_t  StorePreposInfo(preposInfo*);
int32_t  StoreActionRec(actionRec*);
int32_t  StoreTimerInfo(void);
int32_t  StoreFun(int32_t*, int32_t);
int32_t  StoreParList(int32_t*, int32_t);
int32_t  StoreExits(void);
int32_t  StoreFlags(void);
int32_t  StoreAttributes(void);
int32_t  StoreStoryInfo(storyInfo*);
int32_t  StoreWordTable(void);
int32_t  StoreVerbDir(void);
int32_t  StoreLocDir(void);
int32_t  StoreObjDir(void);
int32_t  StoreTriggOwners(void);
int32_t  StoreDescrOwners(void);
int32_t  StoreDirs(void);
void     InitCFlags(int32_t*, int32_t, int32_t, int32_t);
void     ProcCFlagVal(int32_t*, int32_t, int32_t, int32_t);
void     ProcLFlagVal(int32_t*, int32_t, int32_t);
void     SetBitVal(int32_t*, int32_t, int32_t);
void     InitCAttrs(attrInfo*, int32_t, int32_t, int32_t, int32_t, int32_t); /*10march2017 */
void     ProcCAttrVal(attrInfo*, int32_t, int32_t, int32_t, int32_t, int32_t); /*10march2017 */
void     ProcLAttrVal(attrInfo*, int32_t, int32_t, int32_t, int32_t); /*10march2017 */

extern void PrintFileList(fileList**);

extern int32_t total_lines;


/************************/
/* function definitions */
/************************/

FILE *fopen_path(const char *file_path, const char *file_name, const char *mode)
{
  /* this function opens a file, taking into account the path to the file */
  /* file_path must end with a path delimiter ('/' or '\')                */
  /* initially developed for macOS                                        */

  char full_name[PATH_MAX + MAX_FILENAME_LEN + 1] = "";

  strcpy(full_name, file_path);
  strncat(full_name, file_name, MAX_FILENAME_LEN);

  return(fopen(full_name, mode));
}


/***********************/
/* file input routines */
/***********************/

static int NextChar(FILE *fp)
{
  /* nextchar() for windows                   */
  /* will read dos, mac and linux text files  */
  /* will return a dos line terminator ('\n') */

  int c;
  int nc;

  c = fgetc(fp);

  if (c == '\n') {
    /* must be linux                    */
    /* '\n' is ok to return for windows */
  }
  else {
    if (c == '\r') {
        // mac or dos
        // look ahead for '\n'
        nc = fgetc(fp);

        if (nc != '\n') {
          /* must be a mac, put back nc */
            ungetc(nc, fp);
        }
        c = '\n';
    }
  }
  return c;
}


void ConvertFilePath(char **path)
{
  int32_t len = strlen(*path);
  int32_t i   = 0;

  /* this routine converts a windows pathname to linux  */
  /* it replaces all occurences of '\' in path with '/' */

  for (i=0; i<len; i++) {
    if ((*path)[i] == '\\')
      (*path)[i] = '/';
  }
}


int32_t ChangeInputFile(char **word, int32_t *keyword, int32_t owner, int32_t voc_pass1, FILE **source, fileList **file_list)
/* owner is needed in case we encounter a parameter in a string */
/* voc_pass1 tells whether we have to read (and parse) strings or skip them */
{
  /* We got here because the compiler encountered an INSERT or EOF keyword */
  switch (*keyword) {
    case EOF:
      if (!GetPrevFile(current_filename, source, file_list)) {
        /* error msg was already printed by GetPrevFile() */
        return(ERROR);
      }
      if (source == NULL) {
        /* this was the top level file, we processed everything */
        return(ERROR);
      }
      /* read word from new input file */
      free(*word);
      *word = GetNextWord(keyword, owner, voc_pass1, source, file_list);
      return(OK);
      break;
    case INSERT:
      /* read the file name */
      free(*word);
      *word = GetNextWord(keyword, owner, voc_pass1, source, file_list);

      if (*keyword != STRING) {
        ErrHdr();
        PrintError(6, NULL, NULL);
        return(ERROR);
      }

      /* check for loop in inserts */
      if (CheckForLoop(*word, *file_list)) {
        ErrHdr();
        PrintError(7, NULL, *word);
        return(ERROR);
      }

      /* open the new file */
      if ((*source = fopen_path(file_path, *word, "rb")) == NULL) {
        /* error may be caused because we are running linux */
        /* windows uses '\' as path delimiter and linux '/' */
        /* Convert path to linux syntax and try again       */
        ConvertFilePath(word);
        if ((*source = fopen_path(file_path, *word, "rb")) == NULL) {
          ErrHdr();
          PrintError(0, NULL, *word);
          return(ERROR);
        }
      }

      /* add file name to file list */
      if (!AddToFileList(*word, *source, file_list)) {
        /* error msg was already printed by AddToFileList() */
        return(ERROR);
      }

      strncpy(current_filename, *word, MAX_FILENAME_LEN+1);

      /* read word from new input file */
      free(*word);
      *word = GetNextWord(keyword, owner, voc_pass1, source, file_list);
      return(OK);
      break;
    default:
      /* we should never get here */
      printf("\nInternal error: ChangeInputFile() called with unexpected keyword value.\n");
      printf("\nDon't know what is going to happen now as we should never have gotten here.\n");
      return(ERROR);
      break;
  }
}


int32_t GetPrevFile(char file_name[], FILE **fp, fileList **file_list)
{
  fileList *runner = *file_list;
  fileList *temp;
  int found        = 0;

  /* Go through the chain of file_list records to */
  /*find the current file                         */
  while ( (runner != NULL) && !found ) {
    if (strcmp(file_name, runner->name) == 0) {
      found = 1;
      /* we got here through an EOF so now */
      /* we know the number of lines for   */
      /* the current file                  */
      if (runner->lines == -1) {
        /* test necessary for > level 1 inserts */
        runner->lines = line_num;
      }
    }
    /* remember the record for the current file in temp */
    temp = runner;
    runner = runner->previous;
  }

  if (!found) {
    /* we have a serious issue, the filename should be there */
    ErrHdr();
    PrintError(8,NULL, file_name);
    return(ERROR);
  }
  else {
    /* we found the current file             */
    /* now check if there is a previous file */
    if (runner != NULL) {
      /* there is a previous file */
       strncpy(file_name, runner->name, MAX_FILENAME_LEN);
      *fp = runner->fp;
      line_num = runner->current_line;

      *file_list = runner;
      fclose(temp->fp); total_lines += temp->lines;
      free(temp);

      return(OK);
    }
    /* there is no previous file, this is the top level */
    return(ERROR);
  }
}


int32_t AddToFileList(char file_name[], FILE *file_pointer, fileList **file_list)
{
  /* at first use, *file_list will be NULL, so initialization */
  /* will be done automatically in this function.             */
  fileList *new_filestruct = NULL;
  fileList *runner = NULL;
  int      found   = 0;

  if ((new_filestruct = (fileList *)malloc(sizeof(fileList))) == NULL) {
    ErrHdr();
    PrintError(9, NULL, file_name);
    return(ERROR);
  }

  /* fill the new file struct */
  strncpy(new_filestruct->name, file_name, MAX_FILENAME_LEN);
  new_filestruct->current_line = 1;
  new_filestruct->lines        = -1;
  new_filestruct->fp           = file_pointer;
  new_filestruct->previous     = *file_list;

  /* store the current line number with the struct for the */
  /* current file. NOTE: *file_list is not necessarily the */
  /* struct for the current file.                          */

  /* find file_list struct for current file */
  runner = *file_list;
  while ( (runner != NULL) && !found ) {
  if (strcmp(current_filename, runner->name) == 0) {
    found = 1;
  }
  else
    runner = runner->previous;
  }

  if (runner == NULL) {
    /* we should never get here */
    ErrHdr();
    printf("Line %d: no file_list struct for %s.", line_num, current_filename);
    return(ERROR);
  }

/*  (*file_list)->current_line = line_num;*/
  runner->current_line = line_num;
  line_num = 1;
  *file_list = new_filestruct;

  return(OK);
}


int32_t CheckForLoop(char file_name[], fileList *file_list)
{
  fileList *runner = file_list;
  int found        = 0;

  /* Go through the chain of file_list records to see if */
  /* file_name was already opened (=inserted) earlier.   */
  /* If so, then we have a loop and must abort.          */

  while ( (runner != NULL) && !found ) {
    if (strcmp(file_name, runner->name) == 0) {
      found = 1;
    }
    runner = runner->previous;
  }

  return(found ? OK : ERROR);
}


char *Concat(char *str1, char *str2)
{
  /* Concatenates str2 to str1. str1 must be malloc'ed, since it */
  /* is free'ed. TC's strcat(str1,str2)  doesn't malloc extra   */
  /* space for str1. */

  char *temp = malloc(strlen(str1)+strlen(str2)+1);
  strcpy(temp,str1);
  strcat(temp,str2);
  free(str1);
  return(temp);
}


int32_t IsSpecialChar(char ch)
{
  if ( (((int32_t) ch>=97) && ((int32_t) ch<=122)) ||   /* a..z                       */
       (((int32_t) ch>=65) && ((int32_t) ch<=90))  ||   /* A..Z                       */
       (((int32_t) ch>=48) && ((int32_t) ch<=57))  ||   /* 0..9                       */
       (ch == '_') ||                                   /* for identifiers            */
       (ch == '%') ||                                   /* for wildcards in functions */
       (ch == '$') )                                    /* for some keywords          */
    return(0);
  else
    return(1);
}


int32_t ConvertBackSlashChar(char *ch, FILE *source)
{
  char chs[2];  /* for error message */

  /* this function converts a \x to its ASCII code */

  /* terminate chs[] */
  chs[1] = '\0';

  if (*ch != '\\') {
    /* do nothing */
    return(OK);
  }

  *ch = NextChar(source);

  switch (*ch) {
    case '\\':
      /* they wanted to print a \ */
      *ch = '\\';
      break;
    case 'n':
      *ch = CR;
      break;
    case 't':
      *ch = TAB;
      break;
    case '"':
      *ch = STRING_MARKER;
      break;
    default:
      chs[0] = *ch;
      ErrHdr();
      PrintError(143, NULL, chs);
      return(ERROR);
  }
  return(OK);
}


void SkipSpacesPlusCR(char *ch, FILE *source)
{
  int state = 0;
  int cont  = 1;

  /* 20sep2017 - the '/' is used to skip the upcoming spaces and */
  /* CR characters, so long text blocks can be broken for better */
  /* readability in the story source text. If the author wants   */
  /* to print a '/', he must type '//'. The SkipSPacesPlusCR()   */
  /* function must check for this.                               */

  while (cont && !feof(source)) {
    switch (*ch) {
      case SPACE:
        *ch = NextChar(source);
        state = 0;
        break;
      case CR:
        line_num++;
        *ch = NextChar(source);
        state = 0;
        break;
      case '/':
        switch (state) {
          case 0:
            state = 1;
            *ch = NextChar(source);
            break;
          case 1:
            /* they entered a second '/' */
            cont = 0;
            /* do not read the next char */
            break;
        }
        break;
      default:
        /* we're done with skipping */
        cont = 0;
        break;
    } /* switch */
  } /* while */
}


void SkipComment(char *ch, FILE *source)
{
  while ((*ch != CR) && (!feof(source)))
    *ch = NextChar(source);

  /* in case *ch now contains the CR, load it with the */
  /* next character from source                        */
  if (!feof(source)) {
    /* it contains the CR */
    line_num++;
    *ch = NextChar(source);
  }
}


void IdToChar(int32_t id, char *str)
{
  /* Length of str is expected to be 5. */

  /* Syntax of str[] is:                                       */
  /* str[0] indicates a parameter is coming.                   */
  /* str[1] left nibble indicates whether str[2] should be     */
  /*        be read as a 0 (nibble is 1) or not (nibble is 2). */
  /* str[1] right nibble indicates same as above for str[3].   */
  /* str[2] MSB value of parameter id.                         */
  /* str[3] LSB value of parameter id.                         */
  /* str[4] indicates end of string.                           */

  /* int16_t dummy_id = id; */
  int16_t val;

  str[0] = (char) -1;    /* indicates a parameter is coming next */
  str[4] = '\0';         /* end of string */

  /* Fill MSB. */

  val = id/256;

  /* val = id/(0xFFFF+1); */
  if (val == 0) {
    str[1] = 0x10;
    str[2] = 1;      /* Any value but 0 is ok. */
  }
  else {
    str[1] = 0x20;
    str[2] = val;
  }

  /* Fill LSB. */

  val = id%256;

  /* val = id%(0xFFFF+1); */
  if (val == 0) {
    str[1] |= 0x01;
    str[3] = 1;     /* Any value but 0 is ok. */
  }
  else {
    str[1] |= 0x02;
    str[3] = val;
  }
}


char *ReadParInString(int32_t owner, char *r_s, char *ch, FILE *source, fileList **file_list)
{
  int32_t state = 1;
  char    *par;
  char    par2[MAX_ID_LENGTH+2] = "%";
  char    str[5];       /* used to store parameters in string */
  int32_t kw;
  int32_t id;           /* used for holding timer/loc/obj id  */
  int     cont = 1;

  par = GetNextWord(&kw, owner, 0, &source, file_list);

  /* we want to allow to enter wildcards (action, this, dir, ...) */
  /* without the '%' sign as a string parameter. So in case we    */
  /* a NO_KEYWORD we check again after putting a '%' at the       */
  /* beginning of the word.                                       */

  if (kw == NO_KEYWORD) {
    /* do not use strcat() here as Linux will crash */
    /* initialize par2 */
    strncpy(par2+1, par, MAX_ID_LENGTH);
    kw = CheckIfKeyword(par2);
  }

  /* For wildcards, we must include the %-sign, because they    */
  /* are always preceded by a % (otherwise wildcard words could */
  /* not be used as normal vocabulary words.                    */

  /* 11jan2016: we also want to allow descriptions and attributes */
  /* as parameters in strings. We now have the following          */
  /* possibilities:                                               */
  /* - location                                                   */
  /* - object                                                     */
  /* - timer                                                      */
  /* - local description     (new)                                */
  /* - common description    (new)                                */
  /* - location.description  (new) (local and common)             */
  /* - object.description    (new) (local and common)             */
  /* - attribute.description (new) (only common)                  */
  /* - THIS.description      (new) (only common)                  */
  /* - local attribute       (new)                                */
  /* - common attribute      (new)                                */
  /* - location.attribute    (new) (local and common)             */
  /* - object.attribute      (new) (local and common)             */
  /* - THIS.attribute        (new) (only common)                  */

  /* A location, object or THIS parameter may be the owner of a   */
  /* description or attribute that follows, but this is not       */
  /* necessary. In order to prevent uncertainty about whether     */
  /* another parameter is coming, location, object and THIS       */
  /* parameters will always be followed by another parameter. In  */
  /* case the location, object or THIS parameter is by itself,    */
  /* the second parameter will have value NO_ID.                  */

  /* Function GiveNextId(), called by ParseSentence() will throw  */
  /* an error in case the id following a location id, object id   */
  /* or THIS parameter is not NO_ID. Because this would mean they */
  /* added a description or attribute, which is not allowed in    */
  /* TRIGGERS or VERB sections.                                   */

  while (cont) {
    switch (kw) {
      case LOC_ID:
        switch (state) {
          case 1:
            state = 6;
            break;
          default:
            ErrHdr();
            PrintError(10, NULL, par);
            free(par);
            return(NULL);
        }
        if (!GetLocationId(par+2, &id, 0, -1)) {  /* skip "l_" */ /* -1 added on June 15th 2015 to denote unused offset */
          free(par);
          return(NULL);
        }

        /* set owner in case a description or attribute follows */
        owner = id;
        IdToChar(id, str);
        r_s = Concat(r_s, str);
        break;
      case OBJ_ID:
        switch (state) {
          case 1:
            state = 6;
            break;
          default:
            ErrHdr();
            PrintError(11, NULL, par);
            free(par);
            return(NULL);
        }
        if (!GetObjectId(par+2, &id, 0, -1)) {  /* skip "o_" */ /* -1 added on June 15th 2015 to denote unused offset */
          free(par);
          return(NULL);
        }

        /* set owner in case a description or attribute follows */
        owner = id;
        IdToChar(id, str);
        r_s = Concat(r_s, str);
        break;
      case TIM_ID:
        switch (state) {
          case 1:
            state = 8;
            break;
          default:
            ErrHdr();
            PrintError(12, NULL, par);
            free(par);
            return(NULL);
        }
        if (!GetTimerId(par+2, &id, 0)) {  /* skip "m_" */
          free(par);
          return(NULL);
        }

        IdToChar(id, str);
        r_s = Concat(r_s, str);
        break;
      case DESCR_ID:
        switch (state) {
          case 1:
            state = 5;
            /* single occurrence of description id */
            /* we must add the owner             */
            IdToChar(owner, str);
            r_s = Concat(r_s, str);
            break;
          case 4:
            state = 7;
            break;
          default:
            ErrHdr();
            PrintError(13, NULL, par);
            free(par);
            return(NULL);
        }
        if (!GetDescrId(par+2, &id, owner, 0)) { /* skip "d_" */
          free(par);
          return(NULL);
        }

        IdToChar(id, str);
        r_s = Concat(r_s, str);
        break;
      case ATTR_ID:
        switch (state) {
          case 1:
            state = 5;
            /* single occurrence of attribute id */
            /* we must add the owner             */
            IdToChar(owner, str);
            r_s = Concat(r_s, str);
            break;
          case 4:
            state = 7;
            break;
          default:
            ErrHdr();
            PrintError(14, NULL, par);
            free(par);
            return(NULL);
        }
        if (!GetAttrId(par+2, 0, owner, &id, NO_TYPE, NONE, NO_ID, 0)) { /* skip "r_" */ /* 10march2017 */
          free(par);
          return(NULL);
        }

        IdToChar(id, str);
        r_s = Concat(r_s, str);
        break;
      case THIS:
        /* %this format. */
        switch (state) {
          case 1:
            state = 3;
            break;
          default:
            ErrHdr();
            PrintError(15, NULL, NULL);
            free(par);
            return(NULL);
        }

        /* set owner in case a description or attribute follows */
        owner = THIS;

        IdToChar(THIS, str);
        r_s = Concat(r_s, str);
        break;
      case DOT:
        switch (state) {
          case 3:
          case 6:
            state = 4;
            break;
          default:
            ErrHdr();
            PrintError(16, NULL, NULL);
            free(par);
            return(NULL);
        }
        break;
      case DIR:
        /* %dir format */
      case ACTION:
        /* %action format */
      case PREPOS:
        /* %prepos format. */
      case VALUE:
        /* %value in string */
      case ORDINAL:
        /* %ord in string */
      case ARTICLE_A:
        /* %a format */
      case ARTICLE_THE:
        /* %the format */
        switch (state) {
          case 1:
            state = 2;
            break;
          default:
            ErrHdr();
            PrintError(17, NULL, par);
            free(par);
            return(NULL);
        }

        IdToChar(kw, str);
        r_s = Concat(r_s, str);
        break;
      default:
        if (strcmp(par, "[") == 0) {
          switch(state) {
            case 1:
              /* they used a '[[', which means they */
              /* want a '[' and not a parameter     */
              r_s = Concat(r_s, "[");
              cont = 0;
              break;
            default:
              ErrHdr();
              PrintError(18, NULL, NULL);
              free(par);
              return(NULL);
              break;
          }
        }
        else if (strcmp(par, "]") == 0) {
          switch (state) {
            case 2:
              break;
            case 3:
              /* we have a single occurrence of THIS */
              /* add a second id, because the interpreter expects 2 */
              IdToChar(NO_ID, str);
              r_s = Concat(r_s, str);
              break;
            case 5:
              break;
            case 6:
              /* we have a single occurrence of a location or object id */
              /* add a second id, because the interpreter expects 2     */
              IdToChar(NO_ID, str);
              r_s = Concat(r_s, str);
              break;
            case 7:
              break;
            case 8:
              break;
            default:
              ErrHdr();
              PrintError(19, NULL, NULL);
              free(par);
              return(NULL);
          }
          cont = 0;
        }
        else {
          /* they started with a '[' but no parameter followed */
          ErrHdr();
          PrintError(20, NULL, NULL);
          free(par);
          return(NULL);
        }
    } /* switch */
    if (cont) {
      if ( (par = GetNextWord(&kw, owner, 0, &source, file_list)) == NULL)
        return(NULL);
    }
  } /* while */
  free(par);
  return(r_s);
}


char *ReadString(char *ch, int32_t owner, int16_t *error, int32_t voc_pass1, FILE *source, fileList **file_list)
/* owner is needed in case we encounter a parameter */
/* voc_pass1 tells whether we are in Pass1Voc()     */
{
  /* 08feb2018: We had issues tracking down missing end quotes in */
  /* strings. Because a string could contain a <cr> there was no  */
  /* obligation to end a line containing a string with a ". If    */
  /* the author would forget the end quote, an error would be     */
  /* thrown after reading the next string's start quote (more     */
  /* precise : after reading the first word from the next string. */
  /* To fix this, a line with a string must either end with a     */
  /* quote or with a '/'. If it ends with a quote and the next    */
  /* line starts with a string again, the 2 strings will be       */
  /* concatenated. If the line ends with a '/', behavior remains  */
  /* unchanged: the next <cr> and spaces will be skipped.         */

  int  cont = 1;
  char *end_result;
  char *result;

  /* read the (first) string */
  if ((end_result = Read1String(ch, owner, error, voc_pass1, source, file_list)) == NULL)
    return(NULL);

  while (cont) {
    *ch = NextChar(source);
    SkipSpacesPlusCR(ch, source);

    if (*ch != STRING_MARKER) {
      /* there was only one string */
      ungetc(*ch, source);
      cont = 0;
    }
    else {
      /* there is another string coming */
      /* immediately after this one     */

      *ch = NextChar(source);
      if ((result = Read1String(ch, owner, error, voc_pass1, source, file_list)) == NULL)
        return(NULL);
      end_result = Concat(end_result, result);
      free(result);
    }
  }  /* while */

  return(end_result);
}


char *Read1String(char *ch, int32_t owner, int16_t *error, int32_t voc_pass1, FILE *source, fileList **file_list)
/* owner is needed in case we encounter a parameter */
/* voc_pass1 tells whether we are in Pass1Voc()     */
{

  /* 08feb2018: This is the original ReadString() function.   */
  /* Because of better tracking missing end quotes in strings */
  /* an encompassing function was created. See function       */
  /* ReadString() above for more details.                     */

  /* Returns everything from *ch up to the first " character. */
  /* The " itself is skipped.                                 */

  char *result_string = malloc( sizeof(char) );
  char chs[2];           /* used to convert *ch to a string  */

  *result_string = '\0';
  chs[1]         = '\0';          /* terminate chs */

  while ( (*ch != STRING_MARKER) && (!feof(source)) ) {
    if (*ch == '/') {
      SkipSpacesPlusCR(ch, source);
      /* SkipSpacesPlusCR() returns a new ch, which might        */
      /* be a string marker. To prevent this marker from         */
      /* being concatenated to the string, we need an ELSE       */
      /* statement to prevent execution of the rest of the loop. */

      /* check if it returned a '/', which means they used '//'  */
      if (*ch == '/') {
        /* add the '/' to the string */
        chs[0] = *ch;         /* needed for Concat */
        result_string = Concat(result_string, chs);
        *ch = NextChar(source);
      }
    }
    else {
      if (*ch == '[' && !voc_pass1) {
        if ((result_string = ReadParInString(owner, result_string, ch, source, file_list)) == NULL)
          return(NULL);
        /* ReadParInString() doesn't return a new ch var. */
        }
      else {
        if (*ch == CR) {
          ErrHdr();
          PrintError(142, NULL, NULL);
          return(NULL);
          /*line_num++;*/
        }
        else {
          if (*ch == '\\') {
            /* check for \n, \t, etc */
             if (!ConvertBackSlashChar(ch, source))
               return(NULL);
          }
        }
        chs[0] = *ch;         /* needed for Concat */
        result_string = Concat(result_string, chs);
      }
      if (!feof(source)) {
        /* ConvertBackSlashChar() may have hit EOF */
        *ch = NextChar(source);
      }
    }
  } /* while */


  if (feof(source)) {
    ErrHdr();
    PrintError(21, NULL, NULL);
    *error = 1;
    return(NULL);
  }
  else
    return (result_string);
}


char *HandleSpecialChar(char *ch, FILE *source)
{
  char ch1;
  char *word      = malloc(2*sizeof(char));
  char *next_word = NULL;

  word[0] = *ch;
  word[1] = '\0';

  /* "-" can be either used in an arrow keyword (->), */
  /* or as part of a negative value.                  */
  if (*ch == '-') {
    if ((ch1 = NextChar(source)) == '>') {
      word = Concat(word, ">");
      *ch = ch1; /* must also return last char read */
    }
    else if (!IsSpecialChar(ch1)) {
      next_word = ReadWord(&ch1, source);
      word = Concat(word, next_word);
      free(next_word);
    }
    else
      /* We have a "-" followed by a special char other than ">". */
      ungetc(ch1, source);  /* Read one char too many */
  }

  return(word);
}


char *ReadWord(char *ch, FILE *source)
{
  /* Returns everything from *ch up to the next special_char. */

  char *result_string = malloc(sizeof(char));
  char chs[2];         /* used to convert *ch to a string */

  *result_string = '\0';
  chs[1] = '\0';       /* terminate chs */

  do {
    chs[0] = *ch;      /* needed for Concat */
    result_string = Concat(result_string, chs);
    *ch = NextChar(source);
  } while (!(IsSpecialChar(*ch)));

  ungetc(*ch, source);   /* read one char too many */
  return(result_string);
}


char *GetNextWord(int32_t *keyword, int32_t owner, int32_t voc_pass1, FILE **source, fileList **file_list)
/* owner is needed in case we encounter a parameter in a string */
/* vocpass1 tells whether we have to read (and parse) strings or skip them */
{
  /* Returns the next word in source. A word is bounded         */
  /* either by SPACE, CR, ", EOF or a specialchar. SPACE and    */
  /* specialchar at the end of a word are put back to the file. */
  /* The returned string is malloc'ed. It should be free'ed by  */
  /* the calling function when it is no longer used.            */

  char    ch;
  char    *word;
  int16_t error = 0;

  ch = NextChar(*source); /* read the first character */

  SkipSpacesPlusCR(&ch, *source);

  while (ch == COMMENT_MARKER) {
    /* need the following code in case               */
    /* ".. # comment  <CR><SPACE> #comment " are used */
    SkipComment(&ch, *source);
    SkipSpacesPlusCR(&ch, *source);
  }

  if (feof(*source)) {
    *keyword = EOF;
    return (NULL);
  }

  if (ch == STRING_MARKER) {
    ch = NextChar(*source); /* skip the STRING_MARKER */
    if ( (word = ReadString(&ch, owner, &error, voc_pass1, *source, file_list)) == NULL)
      error = 1;
    else
      *keyword = STRING;
  }
  else {
    if (IsSpecialChar(ch))
      word = HandleSpecialChar(&ch, *source);
    else
      word = ReadWord(&ch, *source);
    *keyword = CheckIfKeyword(word);

    /* check if we must change inputfiles */
    if (*keyword == INSERT) {
      /* change inputfile */
      if (!ChangeInputFile(&word, keyword, owner, voc_pass1, source, file_list))
        error = 1;
    }
  }

  if (error) {
    *keyword = ERROR;
    return(NULL);
  }

  /* Do not use ch any more from this point */
  return (word);
}


/************************/
/* file output routines */
/************************/

int32_t StoreOffsets(void)
{
  if (fseek(datafile, 0, 0) == -1) {
    ErrHdr();
    PrintError(22, NULL, NULL);
    return(ERROR);
  }

  /* store the directory data struct */

  if (!StoreInt64(dir_data.story_info_offset)) {
    ErrHdr();
    PrintError(23, NULL, "story info.");
    return(ERROR);
  }

  if (!StoreInt64(dir_data.word_data_offset)) {
    ErrHdr();
    PrintError(23, NULL, "word data.");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.loc_dir_offset)) {
    ErrHdr();
    PrintError(23, NULL, "location directory.");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.obj_dir_offset)) {
    ErrHdr();
    PrintError(23, NULL, "object directory offset.");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.common_descr_offset)) {
    ErrHdr();
    PrintError(23, NULL, "common descriptions offset.");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.common_trigg_offset)) {
    ErrHdr();
    PrintError(23, NULL, "common triggers offset.");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.exit_data_offset)) {
    ErrHdr();
    PrintError(23, NULL, "exit data offset.");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.flag_data_offset)) {
    ErrHdr();
    PrintError(23, NULL, "flag data.");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.trigg_owners_offset)) {
    ErrHdr();
    PrintError(23, NULL, "trigger owners.");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.descr_owners_offset)) {
    ErrHdr();
    PrintError(23, NULL, "description owners.");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.counter_data_offset)) {
    ErrHdr();
    PrintError(23, NULL, "timer data.");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.attribute_data_offset)) {
    ErrHdr();
    PrintError(23, NULL, "attribute data");
    return(ERROR);
  }

  return(OK);
}


int32_t StoreKeyword(int32_t keyword)
{
  if (!StoreInt32(keyword))
    return(ERROR);

  return(OK);
}


int32_t StoreInt16(int16_t n)
{
  /* convert to big endian */
  n = xv_htons(n);

  if (fwrite((void *) &n, sizeof(int16_t), 1, datafile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);

  return(OK);
}


int32_t StoreInt32(int32_t n)
{
  /* convert to big endian */
  n = xv_htonl(n);

  if (fwrite((void *) &n, sizeof(int32_t), 1, datafile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);

  return(OK);
}


int32_t StoreInt64(int64_t n)
{
  /* convert to big endian */
  n = xv_htonll(n);

  if (fwrite((void *) &n, sizeof(int64_t), 1, datafile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);

  return(OK);
}


int32_t StoreId(int32_t id)
{
  if (!StoreInt32(id)) {
    ErrHdr();
    PrintError(24, NULL, NULL);
    return(ERROR);
  }
  return(OK);
}


int32_t StoreString(char *str)
{
  uint32_t len = strlen(str)+1; /* include the '\0' character */

  /* store the string length */
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(25, NULL, NULL);
    return(ERROR);
  }
  /* store the string */
  if (fwrite((void *) str, sizeof(char), len, datafile) != len) {
    ErrHdr();
    PrintError(26, NULL, str);
    return(ERROR);
  }
  return(OK);
}


int32_t StoreExtendedSysDescr(extendedSysDescr *extended_sys_descr)
{
  /* first check if it is a dynamic system description */
  if (extended_sys_descr->dynamic != NULL) {
    /* dynamic system description */
    /* store keyword */
    if (!StoreKeyword(DYN_DSYS)) {
      return(ERROR);
    }
    /* store the string */
    if (!StoreString(extended_sys_descr->dynamic)) {
      return(ERROR);
    }
    else {
      return(OK);
    }
  }

  /* not a dynamic system description */
  /* store keyword */
  if (!StoreKeyword(DSYS)) {
    return(ERROR);
  }

  if (!StoreSysDescr(&(extended_sys_descr->part1))) {
    return(ERROR);
  }
  else {
    if (!StoreInt32(extended_sys_descr->connect_prepos)) {
      ErrHdr();
      PrintError(27, NULL, NULL);
      return(ERROR);
    }
    if (!StoreSysDescr(&(extended_sys_descr->part2))) {
      return(ERROR);
    }
  }
  return(OK);
}


int32_t StoreSysDescr(sysDescr *sys_descr)
{
  int i=0;

  if (!StoreInt32(sys_descr->article)) {
    ErrHdr();
    PrintError(28, NULL, NULL);
    return(ERROR);
  }
  if (!StoreInt32(sys_descr->nr_of_adjectives)) {
    ErrHdr();
    PrintError(28, NULL, NULL);
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_ADJ; i++) {
    if (!StoreInt32(sys_descr->adjectives[i])) {
      ErrHdr();
      PrintError(28, NULL, NULL);
      return(ERROR);
    }
  }
  if (!StoreInt32(sys_descr->noun)) {
    ErrHdr();
    PrintError(28, NULL, NULL);
    return(ERROR);
  }
  return(OK);
}


int32_t StoreContData(contData *cont_data)
{
  int i=0;

  if (!StoreInt32(cont_data->nr_of_objects)) {
    ErrHdr();
    PrintError(29, NULL, NULL);
    return(ERROR);
  }
  for (i=0; i<MAX_CONTAINED_OBJECTS; i++) {
    if (!StoreInt32(cont_data->object_ids[i])) {
      ErrHdr();
      PrintError(29, NULL, NULL);
      return(ERROR);
    }
  }
return(OK);
}


int32_t StoreAdverbInfo(adverbInfo *adverbs)
{
  int i=0;

  if (!StoreInt32(adverbs->nr_of_adverbs)) {
    ErrHdr();
    PrintError(30, NULL, NULL);
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_ADVERBS; i++) {
    if (!StoreInt32(adverbs->adverb[i])) {
      ErrHdr();
      PrintError(30, NULL, NULL);
      return(ERROR);
    }
  }

  return(OK);
}


int32_t StorePreposInfo(preposInfo *prepositions)
{
  int i=0;

  if (!StoreInt32(prepositions->nr_of_prepositions)) {
    ErrHdr();
    PrintError(31, NULL, NULL);
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_PREPOS; i++) {
    if (!StoreInt32(prepositions->preposition[i])) {
      ErrHdr();
      PrintError(31, NULL, NULL);
      return(ERROR);
    }
  }

  return(OK);
}


int32_t StoreActionRec(actionRec *action_rec)
{
  if (!StoreKeyword(ACTION_REC))
    return(ERROR);

  /* store the action_rec */
  if (!StoreInt32(action_rec->actor)) {
    ErrHdr();
    PrintError(32, NULL, NULL);
    return(ERROR);
  }

  if (!StoreInt32(action_rec->action1)) {
    ErrHdr();
    PrintError(32, NULL, NULL);
    return(ERROR);
  }

    if (!StoreInt32(action_rec->action2)) {
    ErrHdr();
    PrintError(32, NULL, NULL);
    return(ERROR);
  }

  if (!StoreAdverbInfo(&action_rec->adverbs))
    return(ERROR);

  if (!StoreInt32(action_rec->q_word)) {
    ErrHdr();
    PrintError(32, NULL, NULL);
    return(ERROR);
  }
  if (!StoreInt32(action_rec->direction)) {
    ErrHdr();
    PrintError(32, NULL, NULL);
    return(ERROR);
  }
  if (!StoreInt32(action_rec->subject)) {
    ErrHdr();
    PrintError(32, NULL, NULL);
    return(ERROR);
  }
  if (!StoreInt32(action_rec->specifier)) {
    ErrHdr();
    PrintError(32, NULL, NULL);
    return(ERROR);
  }
  if (!StorePreposInfo(&action_rec->prepositions))
    return(ERROR);

  if (!StoreInt32(action_rec->value)) {
    ErrHdr();
    PrintError(32, NULL, NULL);
    return(ERROR);
  }
  if (!StoreInt32(action_rec->ordinal)) {
    ErrHdr();
    PrintError(32, NULL, NULL);
    return(ERROR);
  }
  if (!StoreInt32(action_rec->execute[0])) {
    ErrHdr();
    PrintError(32, NULL, NULL);
    return(ERROR);
  }
  if (!StoreInt32(action_rec->execute[1])) {
    ErrHdr();
    PrintError(32, NULL, NULL);
    return(ERROR);
  }

  return(OK);
}


int32_t StoreStoryInfo(storyInfo *info)
{
  int64_t offset;
  int32_t len;

  offset = ftell(datafile);
  dir_data.story_info_offset = offset;

  /* Store the storyInfo struct. */

  len = strlen(info->title)+1;  /* include the '\0' */
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->title, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

  len = strlen(info->author)+1;
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->author, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

  len = strlen(info->organization)+1;
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->organization, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

    len = strlen(info->cover_text)+1;
    if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->cover_text, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

  len = strlen(info->credits)+1;
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->credits, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

  len = strlen(info->version)+1;
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->version, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

  len = strlen(info->android_mkt)+1;
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->android_mkt, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

  len = strlen(info->ios_mkt)+1;
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->ios_mkt, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

  len = strlen(info->backimage)+1;
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->backimage, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

  len = strlen(info->effect)+1;
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->effect, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

  len = strlen(info->primary_color)+1;
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->primary_color, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

  if (!StoreInt16(info->ui_sidebar)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }

  if (!StoreInt16(info->ui_textinput)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }

  if (!StoreInt16(info->ui_compass)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }

  if (!StoreInt16(info->autolink)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }

  len = strlen(info->compiler_version)+1;
  if (!StoreInt32(len)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }
  if (len != 0) {
    if (fwrite((void *) info->compiler_version, len, 1, datafile) != 1) {
      ErrHdr();
      PrintError(33, NULL, NULL);
      return(ERROR);
    }
  }

  if (!StoreInt16(info->xvan_language)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }

  if (!StoreInt16(info->story_language)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }

  if (!StoreInt16(info->play_mode)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }

  if (!StoreInt16(debug)) {
    ErrHdr();
    PrintError(33, NULL, NULL);
    return(ERROR);
  }

  return(OK);
}


int32_t StoreWordTable(void)
{
  int i=0;
  int j=0;

  /* get offset */
  dir_data.word_data_offset = ftell(datafile);

  if (!StoreKeyword(VOCABULARY))
    return(ERROR);

  /* store the total number of words */
  if (!StoreInt32(nr_of_words)) {
    ErrHdr();
    PrintError(34, NULL, NULL);
    return(ERROR);
  }

  /* store the word list */
  for (i=0; i<nr_of_words; i++) {
    if (fwrite((void *) &(word_table[i].word), sizeof(char),
               MAX_WORD_LEN+1, datafile) != MAX_WORD_LEN+1) {
      ErrHdr();
      PrintError(34, NULL, NULL);
      return(ERROR);
    }
    if (fwrite((void *) &(word_table[i].print_word), sizeof(char),
               MAX_WORD_LEN+1, datafile) != MAX_WORD_LEN+1) {
      ErrHdr();
      PrintError(34, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(word_table[i].id)) {
      ErrHdr();
      PrintError(34, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(word_table[i].single_id)) {
      ErrHdr();
      PrintError(34, NULL, NULL);
      return(ERROR);
    }
    for (j=0; j<MAX_TYPES; j++) {
      if (!StoreInt32(word_table[i].types[j])) {
        ErrHdr();
        PrintError(34, NULL, NULL);
        return(ERROR);
      }
    }
  }

  return(OK);
}


int32_t StoreTimerInfo(void)
{
  timerInfo *info = md_start;
  int64_t offset;

  debugInfo *timer_dbug = NULL;
  int i = 0;

  if (nr_of_timers == 0) {
    PrintError(35, NULL, NULL);
    dir_data.counter_data_offset = -1;  /* no timers */
    return(OK);
  }

  /* get offset */
  offset = ftell(datafile);
  dir_data.counter_data_offset = offset;

  if (!StoreKeyword(TIMERS))
    return(ERROR);

  /* store the number of structs that follow */
  if (!StoreInt32(nr_of_timers)) {
    ErrHdr();
    PrintError(36, NULL, NULL);
    return(ERROR);
  }

  /* now store the chain */
  while (info != NULL) {
    if (!StoreInt32(info->id)) {
      ErrHdr();
      PrintError(36, NULL, NULL);
      return(ERROR);
    }

    if (!StoreInt32(info->value)) {
      ErrHdr();
      PrintError(36, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(info->step)) {
      ErrHdr();
      PrintError(36, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(info->interval)) {
      ErrHdr();
      PrintError(36, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(info->update)) {
      ErrHdr();
      PrintError(36, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt16(info->direction)) {
      ErrHdr();
      PrintError(36, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt16(info->state)) {
      ErrHdr();
      PrintError(36, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(info->trigger_at)) {
      ErrHdr();
      PrintError(36, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(info->trigger_spec)) {
      ErrHdr();
      PrintError(36, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(info->execute[0])) {
      ErrHdr();
      PrintError(36, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(info->execute[1])) {
      ErrHdr();
      PrintError(36, NULL, NULL);
      return(ERROR);
    }

    info = info->next;
  } /* while */

  /* Check if we must generate debug info */
  if (debug) {

    if (CreateTimerDebugInfo(&timer_dbug)) {

      /* store the debug info */
      if (!StoreKeyword(DEBUG))
        return(ERROR);
      for (i=0; i<nr_of_timers; i++) {
        if (!StoreString(timer_dbug[i].name))
          return(ERROR);
      }
    }
    else {
      /* error */
      return(ERROR);
    }
    free(timer_dbug);
  }

  return(OK);
}


int32_t StoreFun(int32_t *source, int32_t len)
{
  /* Syntax: fun_code nr_of_pars ( par_list ) */

  if (len <= 0) {
    ErrHdr();
    printf("StoreFun(): zero length %d\n", len);
    return(ERROR);
  }

  /* Store the function name. */
  if (!StoreKeyword(source[0]))
    return(ERROR);  /* Error message is printed by StoreKeyword() */

  if (!StoreParList(++source, len-1))  /* skip function code */
    return(ERROR);

  return(OK);
}


int32_t StoreParList(int32_t *source, int32_t len)
/* Format: nr_of_pars ( par1par2par3..parn ) */
{
  int32_t  i;
  int      index=0;
  uint32_t str_len = -1;

  /* Store the number of parameters. */
  if (!StoreInt32(source[0])) {
    ErrHdr();
    PrintError(37, NULL, NULL);
    return(ERROR);
  }

  /* Skip nr_of_pars and '(' */
  i = 2;

  while (i<len) {
    switch (source[i]) {
      case STRING:
        /* Store the string keyword. */
        StoreInt32(STRING);

        /* Store length of string */
        str_len = source[i+1];
        StoreInt32(str_len);

        /* Store the string. */
        for (index=i+2; index<i+2+str_len; index++) {
          if (!StoreInt32(source[index])) {
            ErrHdr();
            PrintError(37, NULL, NULL);
           return(ERROR);
          }
        }

        i += 2+str_len;
        break;
      case ACTION_REC:
        /* Store the action_rec keyword. */
        StoreInt32(ACTION_REC);

        str_len = 11 + MAX_PARSE_ADJ+1 + MAX_PARSE_PREPOS+1;

        /* Store the action record members. */
        for (index=i+1; index<i+1+str_len; index++) {
          if (!StoreInt32(source[index])) {
            ErrHdr();
            PrintError(37, NULL, NULL);
           return(ERROR);
          }
        }

        i += 1+str_len;
        break;
      case NUMBER:
        /* Store the number keyword. */
        StoreInt32(NUMBER);

        /* Store the value. */
        StoreInt32(source[i+1]);
        i += 2;
        break;
      case RIGHT_PAR:
        i++;
        break;
      default:
        if (IsTestFun(source[i]) || IsIntAct(source[i])) {
          /* No recursive call because we don't know the */
          /* length of the function.                     */

          /* Store function name. */
          StoreInt32(source[i]);

          /* Store number of pars. */
          StoreInt32(source[i+1]);

          /* Skip function name, nr of parameters and '(' */
          i += 3;
        }
        else {
          /* Either a normal id or an EOP. */
          StoreInt32(source[i]);
          i++;
        }
        break;
    } /* switch */
  } /* while */

  return(OK);
}


int32_t StoreExits(void)
{
  uint32_t size = nr_of_locations*nr_of_directions;
  int i=0;

  /* get offset */
  dir_data.exit_data_offset = ftell(datafile);

  /* first store an EXITS keyword */
  if (!StoreKeyword(EXITS))
    return(ERROR);

  /* Store the number of directions (nr_of_locations will be */
  /* stored by StoreLocDir()).                               */
  if (!StoreInt32(nr_of_directions)) {
    ErrHdr();
    PrintError(38, NULL, NULL);
    return(ERROR);
  }

  /* Store the first direction id. */
  if (!StoreInt32(first_direction_id)) {
    ErrHdr();
    PrintError(38, NULL, NULL);
    return(ERROR);
  }

  /* store the exit table */
  for (i=0; i<size; i++) {
    if (!StoreInt32(exit_table[i])) {
      ErrHdr();
      PrintError(38, NULL, NULL);
      return(ERROR);
    }
  }

  return(OK);
}


int32_t StoreFlags(void)
{
  int64_t  offset;
  uint32_t com_loc_len = ((nr_of_cflags*nr_of_locations)/WORD_LEN)+1;
  uint32_t com_obj_len = ((nr_of_cflags*nr_of_objects)/WORD_LEN)+1;
  uint32_t local_len   = (nr_of_lflags/WORD_LEN)+1;
  flagData *ft         = flag_table;

  /* The flags are stored in three parts:            */
  /* - common location flags                         */
  /* - common object flags                           */
  /* - local flags                                   */
  /* The number of common location and object flags  */
  /* is the same.                                    */

  /* Each flag occupies one bit. First we must create    */
  /* integer arrays that can hold all flags of one type. */

  int32_t *com_loc_flags;
  int32_t *com_obj_flags;
  int32_t *local_flags;

  /* for debugging */
  debugInfo *com_flag_dbug = NULL;
  debugInfo *loc_flag_dbug = NULL;

  int32_t i = 0;             /* index. */

  /* Malloc() space for common location flags. */
  if ((com_loc_flags = (int32_t *) malloc(com_loc_len*sizeof(int32_t))) == NULL) {
    PrintError(1, NULL, "common location flags");
    return(ERROR);
  }

  /* Malloc() space for common object flags. */
  if ((com_obj_flags = (int32_t *) malloc(com_obj_len*sizeof(int32_t))) == NULL) {
    PrintError(1, NULL, "common object flags");
    return(ERROR);
  }

  /* Malloc() space for local flags. */
  if ((local_flags = (int32_t *) malloc(local_len*sizeof(int32_t))) == NULL) {
    PrintError(1, NULL, "local flags");
    return(ERROR);
  }

  /* Initialize the common location flags. */
  for (i=0; i<com_loc_len; i++)
    com_loc_flags[i] = 0;

  /* Initialize the common object flags.   */
  for (i=0; i<com_obj_len; i++)
    com_obj_flags[i] = 0;

  /* Initialize the local flags.           */
  for (i=0; i<local_len; i++)
    local_flags[i] = 0;

  /* Next, we have to go through the flag_table and put the */
  /* initial values in the appropriate int array.           */
  /* We have to check ft->next rather than ft, since the    */
  /* last struct is a TERMINATOR struct.                    */

  while (ft->next != NULL) {
    if (ft->common) {
      /* It is a common flag. */
      if (ft->owner == COMMON_FLAGS) {
        /* This is the struct with the default value, for all */
        /* locations and objects.                             */
        InitCFlags(com_loc_flags, nr_of_locations,ft->f_id,ft->value);
        InitCFlags(com_obj_flags, nr_of_objects, ft->f_id, ft->value);
      }
      else if (IsLocId(ft->owner)) {
        /* It is a common location flag. */
        ProcCFlagVal(com_loc_flags, ft->owner, ft->f_id, ft->value);
      } /* else */
      else {
        /* It must be an object id. */
        ProcCFlagVal(com_obj_flags, ft->owner, ft->f_id, ft->value);
      } /* else */
    }
    else {
      /* It is a local flag. */
      ProcLFlagVal(local_flags, ft->f_id, ft->value);
    }

    ft = ft->next;
  } /* while */

  /* Ok, we've filled the flag arrays. Now store them. */

  /* get offset */
  offset = ftell(datafile);
  dir_data.flag_data_offset = offset;

  /* tells the interpreter the flags are coming up next */
  if (!StoreKeyword(FLAGS))
    return(ERROR);

  /* store the number of common flags */
  if (!StoreInt32(nr_of_cflags)) {
    PrintError(39, NULL, NULL);
    return(ERROR);
  }
  /* First store the common location flags.               */
  if (fwrite((void *) com_loc_flags, sizeof(int32_t), com_loc_len,
                                          datafile) != com_loc_len) {
    ErrHdr();
    PrintError(39, NULL, NULL);
    return(ERROR);
  }

  /* Now store the common object flag bits.             */
  if (fwrite((void *) com_obj_flags, sizeof(int32_t), com_obj_len,
                                          datafile) != com_obj_len) {
    PrintError(39, NULL, NULL);
    return(ERROR);
  }

  /* Next store the local flags.                         */
  /* Tell the interpreter the length of the flag string. */
  if (!StoreInt32(local_len)) {
    PrintError(39, NULL, NULL);
    return(ERROR);
  }

  if (fwrite((void *) local_flags, sizeof(int32_t), local_len,
                                         datafile) != local_len) {
    PrintError(39, NULL, NULL);
    return(ERROR);
  }

  /* Check if we must generate debug info */
  if (debug) {
    if (CreateFlagDebugInfo(&com_flag_dbug, &loc_flag_dbug)) {
      /* store the debug info */
      if (!StoreKeyword(DEBUG))
        return(ERROR);

      for (i=0; i<nr_of_cflags; i++) {
        if (!StoreString(com_flag_dbug[i].name))
          return(ERROR);
      }
      /* store the number of local flags.    */
      /* we store it here, because it is     */
      /* only needed when we have debug info */
      if (!StoreInt32(nr_of_lflags)) {
        PrintError(39, NULL, NULL);
        return(ERROR);
      }
      for (i=0; i<nr_of_lflags; i++) {
        if (!StoreString(loc_flag_dbug[i].name))
          return(ERROR);
        if (!StoreInt32(loc_flag_dbug[i].owner))
          return(ERROR);
      }
    }
    else {
      /* error */
      return(ERROR);
    }
    free(com_flag_dbug);
    free(loc_flag_dbug);
  }

  free(com_loc_flags);
  free(com_obj_flags);
  free(local_flags);

  return(OK);
}


int32_t StoreAttributes(void)
{
  int64_t   offset;
  int32_t   com_loc_len = nr_of_cattrs*nr_of_locations;
  int32_t   com_obj_len = nr_of_cattrs*nr_of_objects;
  attrData  *rt         = attr_table;

  /* The attributes are stored in three parts: */
  /* - common location attributes              */
  /* - common object attributes                */
  /* - local attributes                        */
  /* The number of common location and object  */
  /* attributes is the same.                   */

  attrInfo *com_loc_attrs;
  attrInfo *com_obj_attrs;
  attrInfo *local_attrs;

  /* for debugging */
  debugInfo *com_attr_dbug = NULL;
  debugInfo *loc_attr_dbug = NULL;

  int32_t i = 0;             /* index. */

  /* Malloc() space for common location attributes. */
  if ((com_loc_attrs = (attrInfo *) malloc(com_loc_len*sizeof(attrInfo))) == NULL) {
    PrintError(1, NULL, "common location attributes");
    return(ERROR);
  }

  /* Malloc() space for common object attributes. */
  if ((com_obj_attrs = (attrInfo *) malloc(com_obj_len*sizeof(attrInfo))) == NULL) {
    PrintError(1, NULL, "common object attributes");
    return(ERROR);
  }

  /* Malloc() space for local attributes. */
  if ((local_attrs = (attrInfo *) malloc(nr_of_lattrs*sizeof(attrInfo))) == NULL) {
    PrintError(1, NULL, "local attributes");
    return(ERROR);
  }

  /* Initialize the common location attributes. */
  for (i=0; i<com_loc_len; i++) {
    com_loc_attrs[i].type  = NO_TYPE;
    com_loc_attrs[i].value = NO_ID;
    com_loc_attrs[i].owner = NO_ID;  /*10march2017 */
  }

  /* Initialize the common object attributes.   */
  for (i=0; i<com_obj_len; i++) {
    com_obj_attrs[i].type  = NO_TYPE;
    com_obj_attrs[i].value = NO_ID;
    com_obj_attrs[i].owner = NO_ID;  /*10march2017 */
  }

  /* Initialize the local attributes.           */
  for (i=0; i<nr_of_lattrs; i++) {
    local_attrs[i].type  = NO_TYPE;
    local_attrs[i].value = NO_ID;
    local_attrs[i].owner = NO_ID;  /*10march2017 */
  }

  /* Next, we have to go through the attr_table and put the */
  /* initial types and values in the appropriate int array. */
  /* We have to check rt->next rather than rt, since the    */
  /* last struct is a TERMINATOR struct.                    */
  while (rt->next != NULL) {
    if (rt->common) {
      /* It is a common attribute. */
      if (rt->owner == COMMON_ATTRS) {
        /* This is the struct with the default value, for all */
        /* locations and objects.                             */
        InitCAttrs(com_loc_attrs, nr_of_locations,rt->r_id,rt->type,rt->value, rt->value_owner); /*10march2017 */
        InitCAttrs(com_obj_attrs, nr_of_objects, rt->r_id, rt->type, rt->value, rt->value_owner); /*10march2017 */
      }
      else if (IsLocId(rt->owner)) {
        /* It is a common location attribute. */
        ProcCAttrVal(com_loc_attrs, rt->owner, rt->r_id, rt->type,rt->value, rt->value_owner); /*10march2017 */
      } /* else */
      else {
        /* It must be an object id. */
        ProcCAttrVal(com_obj_attrs, rt->owner, rt->r_id, rt->type,rt->value, rt->value_owner); /*10march2017 */
      } /* else */
    }
    else {
      /* It is a local attribute. */
      ProcLAttrVal(local_attrs, rt->r_id, rt->type, rt->value, rt->value_owner); /*10march2017 */
    }

    rt = rt->next;
  } /* while */

  /* Ok, we've filled the attribute arrays. */
  /* now store the attribute arrays */
  /* get offset */
  offset = ftell(datafile);
  dir_data.attribute_data_offset = offset;

  /* Tell the interpreter the attribute are coming up next. */
  if (!StoreKeyword(ATTRIBUTES))
    return(ERROR);

  /* Tell the interpreter the number of the common attributes. */
  if (!StoreInt32(nr_of_cattrs)) {
    PrintError(40, NULL, NULL);
    return(ERROR);
  }

  /* First store the common location attributes.               */
  for (i=0; i<com_loc_len; i++) {
    if (!StoreInt32(com_loc_attrs[i].type)) {
      PrintError(40, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(com_loc_attrs[i].value)) {
      PrintError(40, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(com_loc_attrs[i].owner)) {
      PrintError(40, NULL, NULL);
      return(ERROR);
    }
  }

  /* Now store the common object attributes. */
  for (i=0; i<com_obj_len; i++) {
    if (!StoreInt32(com_obj_attrs[i].type)) {
      PrintError(40, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(com_obj_attrs[i].value)) {
      PrintError(40, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(com_obj_attrs[i].owner)) {
      PrintError(40, NULL, NULL);
      return(ERROR);
    }
  }

  /* Next store the local attributes.                     */
  /* Tell the interpreter the number of local attributes. */
  if (!StoreInt32(nr_of_lattrs)) {
    PrintError(40, NULL, NULL);
    return(ERROR);
  }

  for (i=0; i<nr_of_lattrs; i++) {
    if (!StoreInt32(local_attrs[i].type)) {
      PrintError(40, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(local_attrs[i].value)) {
      PrintError(40, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt32(local_attrs[i].owner)) {
      PrintError(40, NULL, NULL);
      return(ERROR);
    }
  }

  /* Check if we must generate debug info */
  if (debug) {
    if (CreateAttrDebugInfo(&com_attr_dbug, &loc_attr_dbug)) {
      /* store the debug info */
      if (!StoreKeyword(DEBUG))
        return(ERROR);

      for (i=0; i<nr_of_cattrs; i++) {
        if (!StoreString(com_attr_dbug[i].name))
          return(ERROR);
      }
      for (i=0; i<nr_of_lattrs; i++) {
        if (!StoreString(loc_attr_dbug[i].name))
          return(ERROR);
        if (!StoreInt32(loc_attr_dbug[i].owner))
          return(ERROR);
      }
    }
    else {
      /* error */
      return(ERROR);
    }
    free(com_attr_dbug);
    free(loc_attr_dbug);
  }

  free(com_loc_attrs);
  free(com_obj_attrs);
  free(local_attrs);
  return(OK);
}


int32_t StoreVerbDir(void)

{ int32_t i;
  int32_t size = nr_of_verbs;

  if (!StoreKeyword(VERB))
    return(ERROR);

  /* 06jan2018: because we now allow verb redefinition, it   */
  /* may occur that a synonym verb will become an autonomous */
  /* verb with its own verbDir entry. Therefore we can no    */
  /* longer substract nr_of_syn_verbs from nr_of_verbs to    */
  /* determine the size of the verbDir.                      */

  if (!StoreInt32(size)) {
    PrintError(41, NULL, NULL);
    return(ERROR);
  }

  /* store the verb directory */
  for (i=0; i<size; i++) {
    if (!StoreInt32(verb_dir[i].scope)) {
      PrintError(41, NULL, NULL);
      return(ERROR);
    }
    if (!StoreInt64(verb_dir[i].offset)) {
      PrintError(41, NULL, NULL);
      return(ERROR);
    }
  }

  return(OK);
}


int32_t StoreLocDir(void)
{
  int64_t      offset;  /* directory's offset in outputfile */
  locationData *lp = loc_table;
  dirInfo      *loc_dir;
  int          i,j;

  /* for debugging */
  debugInfo *loc_dbug = NULL;

  /* loc_dir will be stored in the outputfile for */
  /* later lookups, it is indexed by location id  */

  /* get the offset */
  offset = ftell(datafile);
  dir_data.loc_dir_offset = offset;

  /* there will always be at least one location */
  if (!StoreKeyword(LOC_DIR))
    return(ERROR);

  /* create space on heap for loc_dir */
  if ((loc_dir = (dirInfo *) malloc(nr_of_locations*sizeof(dirInfo))) == NULL) {
    PrintError(1, NULL, "location directory");
    return(ERROR);
  }

  /* last struct will be the terminator struct, which must */
  /* not be stored; therefore test for lp->next != NULL    */
  for (i=0;i<nr_of_locations;i++) {
    /* Copy the next location id's offset and contained    */
    /* objects in loc_dir.                                 */
    loc_dir[(lp->l_id)-FIRST_LOCATION_ID] = lp->dir_info;
    lp = lp->next;
  }

  /* store the number of locations */
  if (!StoreInt32(nr_of_locations)) {
    PrintError(42, NULL, NULL);
    free(loc_dir);
    return(ERROR);
  }

  /* Store the location directory. */
  for (i=0; i<nr_of_locations; i++) {
    if (!StoreInt32(loc_dir[i].nr_of_dsys)) {
      PrintError(42, NULL, NULL);
      free(loc_dir);
      return(ERROR);
    }
    for (j=0; j<MAX_DSYS; j++) {
      if (!StoreExtendedSysDescr(&(loc_dir[i].descr[j]))) {
        free(loc_dir);
        return(ERROR); /* error message was printed by StoreContData() */
      }
    }
    if (!StoreContData(&(loc_dir[i].contained_objs))) {
      free(loc_dir);
      return(ERROR); /* error message was printed by StoreContData() */
    }
    if (!StoreInt32(loc_dir[i].held_by)) {
      PrintError(42, NULL, NULL);
      free(loc_dir);
      return(ERROR);
    }
    if (!StoreInt64(loc_dir[i].offset)) {
      PrintError(42, NULL, NULL);
      free(loc_dir);
      return(ERROR);
    }
  }

  /* Check if we must generate debug info */
  if (debug) {
    if (CreateLocDebugInfo(&loc_dbug)) {
      /* store the debug info */
      if (!StoreKeyword(DEBUG))
        return(ERROR);

      for (i=0; i<nr_of_locations; i++) {
        if (!StoreString(loc_dbug[i].name))
          return(ERROR);
      }
    }
    else {
      /* error */
      return(ERROR);
    }
    free(loc_dbug);
  }
  free(loc_dir);
  return(OK);
}


int32_t StoreObjDir(void)
{
  int64_t      offset;  /* directory's offset in outputfile */
  objectData   *op = obj_table;
  dirInfo      *obj_dir;
  int32_t      i,j;

  /* for debugging */
  debugInfo *obj_dbug = NULL;

  /* obj_dir will be stored in the outputfile for */
  /* later lookups, it is indexed by object id    */

  /* get the offset */
  offset = ftell(datafile);
  dir_data.obj_dir_offset = offset;

  /* there will always be at least one object */
  if (!StoreKeyword(OBJ_DIR))
    return(ERROR);

  /* create space on heap for obj_offsets */
  if ((obj_dir = (dirInfo *) malloc(nr_of_objects*sizeof(dirInfo))) == NULL) {
    PrintError(1, NULL, "object directory");
    return(ERROR);
  }

  /* last struct will be the terminator struct, which must */
  /* not be stored; therefore test for lp->next != NULL    */
  for (i=0;i<nr_of_objects;i++) {
    obj_dir[(op->o_id)-FIRST_OBJECT_ID] = op->dir_info;
    op = op->next;
  }

  /* store the number of objects */
  if (!StoreInt32(nr_of_objects)) {
    PrintError(43, NULL, NULL);
    free(obj_dir);
    return(ERROR);
  }

  /* store the object directory */
  for (i=0; i<nr_of_objects; i++) {
    if (!StoreInt32(obj_dir[i].nr_of_dsys)) {
      PrintError(43, NULL, NULL);
      free(obj_dir);
      return(ERROR);
    }
    for (j=0; j<MAX_DSYS; j++) {
      if (!StoreExtendedSysDescr(&(obj_dir[i].descr[j]))) {
        free(obj_dir);
        return(ERROR); /* error message was printed by StoreSysDescr() */
      }
    }
    if (!StoreContData(&(obj_dir[i].contained_objs))) {
      free(obj_dir);
      return(ERROR); /* error message was printed by StoreExtendedSysDescr() */
    }
    if (!StoreInt32(obj_dir[i].held_by)) {
      PrintError(43, NULL, NULL);
      free(obj_dir);
      return(ERROR);
    }
    if (!StoreInt64(obj_dir[i].offset)) {
      PrintError(43, NULL, NULL);
      free(obj_dir);
      return(ERROR);
    }
  }

  /* Check if we must generate debug info */
  if (debug) {
    if (CreateObjDebugInfo(&obj_dbug)) {
      /* store the debug info */
      if (!StoreKeyword(DEBUG))
        return(ERROR);

      for (i=0; i<nr_of_objects; i++) {
        if (!StoreString(obj_dbug[i].name))
          return(ERROR);
      }
    }
    else {
      /* error */
      return(ERROR);
    }
    free(obj_dbug);
  }

  free(obj_dir);
  return(OK);
}


int32_t StoreTriggOwners(void)
{
  triggerData *tp = trigg_table;
  int32_t     *trigg_owners;   /* The trigg_owners array is used   */
                               /* only in this function. Therefore */
                               /* it is not defined in module      */
                               /* tables.c                         */
  int64_t     offset;
  int         i=0;

  debugInfo *com_trigg_dbug = NULL;
  debugInfo *loc_trigg_dbug = NULL;

  /* get the offset */
  offset = ftell(datafile);
  dir_data.trigg_owners_offset = offset;

  /* Malloc() space for trigg_owners */
  if ((trigg_owners = (int32_t *) malloc(nr_of_ltrigs*sizeof(int32_t))) == NULL) {
    PrintError(1, NULL, "trigg_owners");
    return(ERROR);
  }

  /* For each trigger, store its owner in the trigg_owners array. */
  /* the trigger's ids are used to index the array.               */
  /* Common triggers need not be handled.                         */
  /* Test tp->next here, since last struct contains TERMINATOR    */
  while (tp->next != NULL) {
    if (!tp->common)
      trigg_owners[tp->t_id - FIRST_LOCAL_TRIGGER_ID] = tp->owner;
    tp = tp->next;
  }

  /* Store keyword, so interpreter can check datafile format */
  if (!StoreKeyword(TRIGGERS))
    return(ERROR);

  /* Store the number of local triggers.                         */
  /* There is no need to store the number of common triggers, as */
  /* the interpreter doesn't need to know this.                  */
  if (!StoreInt32(nr_of_ltrigs)) {
    PrintError(44, NULL, NULL);
    free(trigg_owners);
    return(ERROR);
  }

  /* Write the trigg_owners to file */
  for (i=0; i<nr_of_ltrigs; i++) {
    if (!StoreInt32(trigg_owners[i])) {
    PrintError(44, NULL, NULL);
    free(trigg_owners);
    return(ERROR);
    }
  }

  /* Check if we must generate debug info */
  if (debug) {
    if (CreateTriggerDebugInfo(&com_trigg_dbug, &loc_trigg_dbug)) {
      /* store the debug info */
      if (!StoreKeyword(DEBUG))
        return(ERROR);

      /* Tell the interpreter the number of the common triggers. */
      if (!StoreInt32(nr_of_ctrigs)) {
        PrintError(44, NULL, NULL);
        return(ERROR);
      }
      for (i=0; i<nr_of_ctrigs; i++) {
        if (!StoreString(com_trigg_dbug[i].name))
          return(ERROR);
      }
      for (i=0; i<nr_of_ltrigs; i++) {
        if (!StoreString(loc_trigg_dbug[i].name))
          return(ERROR);
        if (!StoreInt32(loc_trigg_dbug[i].owner))
          return(ERROR);
      }
    }
    else {
      /* error */
      return(ERROR);
    }
    free(com_trigg_dbug);
    free(loc_trigg_dbug);
  }

  free(trigg_owners);
  return(OK);
}


int32_t StoreDescrOwners(void)
{
  descrData *dp = descr_table;
  int32_t   *descr_owners;     /* The descr_owners array is used   */
                               /* only in this function. Therefore */
                               /* it is not defined in module      */
                               /* tables.c                         */
  int64_t   offset;
  int i=0;

  /* get the offset */
  offset = ftell(datafile);
  dir_data.descr_owners_offset = offset;

  /* Malloc() space for descr_owners */
  if ((descr_owners = (int32_t *) malloc(nr_of_ldescrs*sizeof(int32_t))) == NULL) {
    PrintError(1, NULL, "descr_owners");
    return(ERROR);
  }

  /* For each description, store its owner in the descr_owners    */
  /* array. The description ids are used to index the array.      */
  /* Test dp->next here, since last struct contains TERMINATOR    */
  /* Common description owners need not be stored, since each     */
  /* location and object has these descriptions.                  */
  while (dp->next != NULL) {
    if (!dp->common)
      descr_owners[dp->d_id - FIRST_LOCAL_DESCR_ID] = dp->owner;
    dp = dp->next;
  }

  /* Store keyword, so interpreter can check datafile format */
  if (!StoreKeyword(DESCRIPTIONS))
    return(ERROR);

  /* Store the number of local descriptions.                      */
  /* There is no need to store the number of common descriptions, */
  /* as the interpreter doesn't need to know this.               */
  if (!StoreInt32(nr_of_ldescrs)) {
    PrintError(45, NULL, NULL);
    free(descr_owners);
    return(ERROR);
  }

  /* Write the descr_owners to file */
  for (i=0; i<nr_of_ldescrs; i++) {
    if (!StoreInt32(descr_owners[i])) {
      PrintError(45, NULL, NULL);
      free(descr_owners);
      return(ERROR);
    }
  }

  free(descr_owners);
  return(OK);
}


int32_t StoreDirs(void)
{
  if (!StoreWordTable())    /* maps words to ids en types */
    return(ERROR);

  if (!StoreVerbDir())      /* tells where to find the default code */
    return(ERROR);          /* for verbs                            */

  if (!StoreLocDir())       /* store the location directory */
    return(ERROR);

  if (!StoreObjDir())       /* store the object directory   */
    return(ERROR);

  if (!StoreTriggOwners())  /* triggers are stored together    */
    return(ERROR);          /* with their owner's code         */

  if (!StoreDescrOwners())  /* desriptions are stored together */
    return(ERROR);          /* with their owner's code          */

  if (!StoreExits())        /* Map information. */
    return(ERROR);

  if (!StoreFlags())        /* Default flag values. */
    return(ERROR);

  if (!StoreAttributes())   /* Default attribute values. */
    return(ERROR);

  if (!StoreTimerInfo())    /* timer info */
    return(ERROR);

  if (!StoreOffsets())      /* contains directory offsets       */
    return(ERROR);

  return(OK);
}


/**************************************/
/* Miscellaneous routines, needed by  */
/* StoreFlags() and StoreAttributes(). */
/**************************************/

void InitCFlags(int32_t *flags, int32_t size, int32_t id, int32_t value)
{
  int32_t i           = 0;
  int32_t word_offset = 0;
  int32_t bit_offset  = 0;

  for (i=0; i<size; i++) {
    word_offset = ((id-FIRST_COMMON_FLAG_ID)+(i*nr_of_cflags))
                                                         / WORD_LEN;
    bit_offset = ((id-FIRST_COMMON_FLAG_ID)+(i*nr_of_cflags))
                                                         % WORD_LEN;

  SetBitVal(flags+word_offset, bit_offset, value);
 }
}


void ProcCFlagVal(int32_t *flags, int32_t owner, int32_t id, int32_t value)
{
  int32_t word_offset = 0;
  int32_t bit_offset  = 0;
  int32_t first_id    = 0;

  first_id = (IsLocId(owner)? FIRST_LOCATION_ID : FIRST_OBJECT_ID);

  word_offset = ((id-FIRST_COMMON_FLAG_ID) +
                 ((owner-first_id)*nr_of_cflags)
                ) / WORD_LEN;
  bit_offset  = ((id-FIRST_COMMON_FLAG_ID) +
                 ((owner-first_id)*nr_of_cflags)
                ) % WORD_LEN;

  SetBitVal(flags+word_offset, bit_offset, value);
}


void ProcLFlagVal(int32_t *flags, int32_t id, int32_t value)
{
  /* Id is used as an offset to address within flags. */
  int32_t word_offset = (id - FIRST_LOCAL_FLAG_ID) / WORD_LEN;
  int32_t bit_offset  = (id - FIRST_LOCAL_FLAG_ID) % WORD_LEN;

  SetBitVal(flags+word_offset, bit_offset, value);
}


void SetBitVal(int32_t *word, int32_t bit_pos, int32_t value)
{
  switch (value) {
    case 0:
      *word = *word & (~(1 << bit_pos));
      break;
    case 1:
      *word = *word | (1 << bit_pos);
      break;
    default:
      ErrHdr();
      PrintError(46, &((resultStruct){VALUE, value}), NULL);
      break;
  }
}


void InitCAttrs(attrInfo *attrs, int32_t size, int32_t id, int32_t type, int32_t value, int32_t value_owner)  /*10march2017 */
/* value_owner is for for descriptions */  /*10march2017 */
{
  int32_t i     = 0;
  int32_t index = 0;

  for (i=0; i<size; i++) {
    index = (id-FIRST_COMMON_ATTR_ID)+(i*nr_of_cattrs);
    attrs[index].type  = type;
    attrs[index].value = value;
    attrs[index].owner = value_owner; /*10march2017 */
 }
}


void ProcCAttrVal(attrInfo *attrs, int32_t owner, int32_t id, int32_t type, int32_t value, int32_t value_owner)  /*10march2017 */
 /* value_owner is for descriptions */ /*10march2017 */
{
  int32_t index    = 0;
  int32_t first_id = 0;

  first_id = (IsLocId(owner)? FIRST_LOCATION_ID : FIRST_OBJECT_ID);

  index = (id-FIRST_COMMON_ATTR_ID) + (owner-first_id)*nr_of_cattrs;

  attrs[index].type  = type;
  attrs[index].value = value;
  attrs[index].owner = value_owner; /*10march2017 */
}


void ProcLAttrVal(attrInfo *attrs, int32_t id, int32_t type, int32_t value, int32_t value_owner)  /*10march2017 */
 /* value_owner is for descriptions */ /*10march2017 */
{
  /* Id is used as an offset to address within attrs. */
  int32_t index = id - FIRST_LOCAL_ATTR_ID;

  attrs[index].type  = type;
  attrs[index].value = value;
  attrs[index].owner = value_owner; /*10march2017 */
}
