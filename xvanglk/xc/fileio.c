
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>  /* malloc(), realloc(), free() */
#include <string.h>
#include <stdint.h>
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

int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**);
char     *Concat(char*,char*);
int32_t  IsSpecialChar(char);
char     *HandleSpecialChar(char*, FILE*);
void     SkipSpacesPlusCR(char*, FILE*);
void     SkipComment(char*, FILE*);
void     IdToChar(int32_t, char*);
char     *ReadParInString(int32_t, char*, char*, FILE*);
char     *ReadString(char*, int32_t, int16_t*, int32_t, FILE*);
char     *ReadWord(char*, FILE*);
char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**);
int32_t  StoreOffsets(void);
int32_t  StoreKeyword(int32_t);
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

/***********************/
/* file input routines */
/***********************/

int32_t ChangeInputFile(word, keyword, owner, voc_pass1, source)
 char     **word;
 int32_t  *keyword;
 int32_t  owner;     /* in case we encounter a parameter in a string */
 int32_t  voc_pass1; /* tells whether we have to read (and parse) strings or skip them */
 FILE     **source;
{
  static FILE    *old_source   = NULL;
  static int32_t old_line_num  = 0;
  static char    old_filename[MAX_ID_LENGTH+1] = "";

  /* We got here because the compiler encountered an INSERT or EOF keyword */

  /* There are 4 possibilities:                                                  */
  /* 1. keyword = EOF and old_source = NULL => end of inputfile and no higher    */
  /*    level file available. Return ERROR and calling function must handle EOF. */
  /* 2. keyword = EOF and old_source != NULL => end of inputfile and there is a  */
  /*    higher level file available. Change inputfile, call GetNextWord() and    */
  /*    return OK.                                                               */
  /*    call GetNextWord().                                                      */
  /* 3. keyword = INSERT and old_source = NULL => we are in the top level file   */
  /*    and must process the inserted file. Read the filename, open the file and */
  /*    call GetNextWord(). Return OK.                                           */
  /* 4. keyword = INSERT and old_source != NULL => we are already processing an  */
  /*    inserted file. As we only allow one level of insertion, this is an       */
  /*    error. print error msg and return ERROR.                                 */

  switch (*keyword) {
    case EOF:
      if (old_source == NULL) {
        /* end of all files */
        return(ERROR);
      }
      else {
        *source = old_source;
        old_source = NULL;
/*        total_lines += line_num; */
        line_num = old_line_num;
        old_line_num = 0;
        strncpy(current_filename, old_filename, MAX_ID_LENGTH+1);
        old_filename[0] = '\0';

        /* read word from new inputfile */
        free(*word);
        *word = GetNextWord(keyword, owner, voc_pass1, source);
        return(OK);
      }
      break;
    case INSERT:
      if (old_source == NULL) {
        /* now read the filename */
        free(*word);
        *word = GetNextWord(keyword, owner, voc_pass1, source);

        if (*keyword != STRING) {
          ErrHdr();
          printf("filename (with quotes) expected after insert keyword.\n");
          return(ERROR);
        }
        strncpy(old_filename, current_filename, MAX_ID_LENGTH+1);
        strncpy(current_filename, *word, MAX_ID_LENGTH+1);

        /* save the current file pointer */
        old_source   = *source;
        old_line_num = line_num;
        line_num     = 1;

        if ((*source = fopen(*word, "r")) == NULL) {
          ErrHdr();
          printf("\nError opening file: %s\n", *word);
          return(ERROR);
        }

        /* read word from new inputfile */
        free(*word);
        *word = GetNextWord(keyword, owner, voc_pass1, source);
        return(OK);
      }
      else {
        ErrHdr();
        printf("Insert statement found in inserted file. Insert statements may only be one level deep.\n");
        return(ERROR);
      }
      break;
    default:
      /* we should never get here */
      printf("\nInternal error: ChangeInputFile() called with unexpected keyword value.\n");
      printf("\nDon't know what is going to happen now as we shouldd never have gotten here.\n");
      return(ERROR);
      break;
  }
}


char *Concat(str1,str2)
 char *str1,*str2;
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


int32_t IsSpecialChar(ch)
 char ch;
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


void SkipSpacesPlusCR(ch, source)
 char *ch;
 FILE *source;
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
        *ch = fgetc(source);
        state = 0;
        break;
      case CR:
        line_num++;
        *ch = fgetc(source);
        state = 0;
        break;
      case '/':
        switch (state) {
          case 0:
            state = 1;
            *ch = fgetc(source);
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

void SkipComment(ch, source)
 char *ch;
 FILE *source;
{
  while ((*ch != CR) && (!feof(source)))
    *ch = fgetc(source);

  /* in case *ch now contains the CR, load it with the */
  /* next character from source                        */
  if (!feof(source)) {
    /* it contains the CR */
    line_num++;
    *ch = fgetc(source);
  }
}

void IdToChar(id, str)
 int32_t  id;
 char *str;
{
  /* Length of str is expected to be 5. */

  /* Syntax of str[] is: */
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

char *ReadParInString(owner, r_s, ch, source)
 int32_t owner;  /* the parameter's owner */
 char    *r_s;
 char    *ch;   /* *ch = '[' */
 FILE    *source;
{
  int32_t state = 1;
  char    *par;
  char    par2[MAX_ID_LENGTH+2] = "%";
  char    str[5];       /* used to store parameters in string */
  int32_t kw;
  int32_t id;           /* used for holding timer/loc/obj id  */
  int     cont = 1;

  par = GetNextWord(&kw, owner, 0, &source);

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
            printf("\nLine %d: unexpected location parameter in string: %s\n", line_num, par);
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
            printf("\nLine %d: unexpected object parameter in string: %s\n", line_num, par);
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
            printf("\nLine %d: unexpected timer parameter in string: %s\n", line_num, par);
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
            printf("\nLine %d: unexpected description parameter in string: %s\n", line_num, par);
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
            printf("\nLine %d: unexpected attribute parameter in string: %s\n", line_num, par);
            free(par);
            return(NULL);
        }
        if (!GetAttrId(par+2, owner, &id, NO_TYPE, NONE, NO_ID, 0)) { /* skip "r_" */ /* 10march2017 */
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
            printf("\nLine %d: illegal THIS parameter in string: %s\n", line_num, par);
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
            printf("\nLine %d: unexpected '.' in string parameter.\n", line_num);
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
            printf("\nLine %d: unexpected wildcard parameter in string: %s\n", line_num, par);
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
              printf("File: %s, Line %d: error: missing ']' symbol in parameter.\n", current_filename, line_num);
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
              /* we have a single occurence of THIS */
              /* add a second id, because the interpreter expects 2 */
              IdToChar(NO_ID, str);
              r_s = Concat(r_s, str);
              break;
            case 5:
              break;
            case 6:
              /* we have a single occurence of a location or object id */
              /* add a second id, because the interpreter expects    2 */
              IdToChar(NO_ID, str);
              r_s = Concat(r_s, str);
              break;
            case 7:
              break;
            case 8:
              break;
            default:
              printf("\nLine %d: unexpected ']'  in string parameter.\n", line_num);
              free(par);
              return(NULL);
          }
          cont = 0;
        }
        else {
          /* they started with a '[' but no parameter followed */\
          printf("File: %s, Line %d: error: parameter expected after '[' symbol.\n", current_filename, line_num);
          free(par);
          return(NULL);
        }
    } /* switch */
    if (cont) {
      if ( (par = GetNextWord(&kw, owner, 0, &source)) == NULL)
        return(NULL);
    }
  } /* while */
  free(par);
  return(r_s);
}

char *ReadString(ch, owner, error, voc_pass1, source)
 char    *ch;
 int32_t owner;   /* in case we encounter a parameter */
 int16_t *error;
 int32_t voc_pass1;  /* tells whether we are in Pass1Voc() */
 FILE    *source;
{
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
        *ch = fgetc(source);
      }
    }
    else {
      if (*ch == '[' && !voc_pass1) {
        if ((result_string = ReadParInString(owner, result_string, ch, source)) == NULL)
          return(NULL);
        /* ReadParInString() doesn't return a new ch var. */
        }
      else {
        if (*ch == CR)
          line_num++;
        chs[0] = *ch;         /* needed for Concat */
        result_string = Concat(result_string, chs);
      }
      *ch = fgetc(source);
    }
  } /* while */

  if (feof(source)) {
    ErrHdr();
    printf("Unexpected end of file in string.\n");
    *error = 1;
    return(NULL);
  }
  else
    return (result_string);
}

char *HandleSpecialChar(ch, source)
 char *ch;
 FILE *source;
{
  char ch1;
  char *word      = malloc(2*sizeof(char));
  char *next_word = NULL;

  word[0] = *ch;
  word[1] = '\0';

  /* "-" can be either used in an arrow keyword (->), */
  /* or as part of a negative value.                  */
  if (*ch == '-') {
    if ((ch1 = fgetc(source)) == '>') {
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

char *ReadWord(ch, source)
 char *ch;
 FILE *source;
{
  /* Returns everything from *ch up to the next special_char. */

  char *result_string = malloc(sizeof(char));
  char chs[2];         /* used to convert *ch to a string */

  *result_string = '\0';
  chs[1] = '\0';       /* terminate chs */

  do {
    chs[0] = *ch;      /* needed for Concat */
    result_string = Concat(result_string, chs);
    *ch = fgetc(source);
  } while (!(IsSpecialChar(*ch)));

  ungetc(*ch, source);   /* read one char too many */
  return(result_string);
}

char *GetNextWord(keyword, owner, voc_pass1, source)
 int32_t  *keyword;
 int32_t  owner;     /* in case we encounter a parameter in a string */
 int32_t  voc_pass1; /* tells whether we have to read (and parse) strings or skip them */
 FILE     **source;
{
  /* Returns the next word in source. A word is bounded         */
  /* either by SPACE, CR, ", EOF or a specialchar. SPACE and    */
  /* specialchar at the end of a word are put back to the file. */
  /* The returned string is malloc'ed. It should be free'ed by  */
  /* the calling function when it is no longer used.            */

  char    ch;
  char    *word;
  int16_t error = 0;

  ch = fgetc(*source); /* read the first character */

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
    ch = fgetc(*source); /* skip the STRING_MARKER */
    if ( (word = ReadString(&ch, owner, &error, voc_pass1, *source)) == NULL)
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
      if (!ChangeInputFile(&word, keyword, owner, voc_pass1, source))
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

int32_t StoreOffsets()
{
  if (fseek(datafile, 0, 0) == -1) {
    ErrHdr();
    printf("StoreOffsets(): error in fseek.\n");
    return(ERROR);
  }

  /* store the directory data struct */

  if (!StoreInt64(dir_data.story_info_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing story info offset.\n");
    return(ERROR);
  }

  if (!StoreInt64(dir_data.word_data_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing word data offset.\n");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.loc_dir_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing location directory offset.\n");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.obj_dir_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing object directory offset.\n");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.common_descr_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing common descriptions offset.\n");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.common_trigg_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing common triggers offset.\n");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.exit_data_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing exit data offset.\n");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.flag_data_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing flag data offset.\n");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.trigg_owners_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing trigger owners offset.\n");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.descr_owners_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing description owners offset.\n");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.counter_data_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing timer data offset.\n");
    return(ERROR);
  }
  if (!StoreInt64(dir_data.attribute_data_offset)) {
    ErrHdr();
    printf("StoreOffsets(): error writing attribute data offset.\n");
    return(ERROR);
  }

  return(OK);
}

int32_t StoreKeyword(keyword)
 int32_t keyword;
{
  if (!StoreInt32(keyword))
    return(ERROR);

  return(OK);
}

int32_t StoreInt16(n)
 int16_t n;
{
  /* convert to big endian */
  n = htons(n);

  if (fwrite((void *) &n, sizeof(int16_t), 1, datafile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);

  return(OK);
}

int32_t StoreInt32(n)
 int32_t n;
{
  /* convert to big endian */
  n = htonl(n);

  if (fwrite((void *) &n, sizeof(int32_t), 1, datafile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);

  return(OK);
}

int32_t StoreInt64(n)
 int64_t n;
{
  /* convert to big endian */
  n = htonll(n);

  if (fwrite((void *) &n, sizeof(int64_t), 1, datafile) != 1)
    /* error message will be printed by calling function */
    return(ERROR);

  return(OK);
}

int32_t StoreId(id)
 int32_t id;
{
  if (!StoreInt32(id)) {
    ErrHdr();
    printf("StoreId(): line %d: error writing id.\n", line_num);
    return(ERROR);
  }
  return(OK);
}

int32_t StoreString(str)
 char *str;
{
  uint32_t len = strlen(str)+1; /* include the '\0' character */

  /* store the string length */
  if (!StoreInt32(len)) {
    ErrHdr();
    printf("StoreString(): line %d: error writing string length\n", line_num);
    return(ERROR);
  }
  /* store the string */
  if (fwrite((void *) str, sizeof(char), len, datafile) != len) {
    ErrHdr();
    printf("StoreString(): line %d: error writing string: %s\n", line_num, str);
    return(ERROR);
  }
  return(OK);
}


int32_t StoreExtendedSysDescr(extended_sys_descr)
 extendedSysDescr *extended_sys_descr;
{
  if (!StoreSysDescr(&(extended_sys_descr->part1))) {
    return(ERROR);
  }
  else {
    if (!StoreInt32(extended_sys_descr->connect_prepos)) {
      ErrHdr();
      printf("StoreExtendedSysDescr(): error writing preposition.\n");
      return(ERROR);
    }
    if (!StoreSysDescr(&(extended_sys_descr->part2))) {
      return(ERROR);
    }
  }
  return(OK);
}


int32_t StoreSysDescr(sys_descr)
 sysDescr *sys_descr;
{
  int i=0;

  if (!StoreInt32(sys_descr->article)) {
    ErrHdr();
    printf("StoreSysDescr(): write error.\n");
    return(ERROR);
  }
  if (!StoreInt32(sys_descr->nr_of_adjectives)) {
    ErrHdr();
    printf("StoreSysDescr(): write error.\n");
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_ADJ; i++) {
    if (!StoreInt32(sys_descr->adjectives[i])) {
      ErrHdr();
      printf("StoreSysDescr(): write error.\n");
      return(ERROR);
    }
  }
  if (!StoreInt32(sys_descr->noun)) {
    ErrHdr();
    printf("StoreSysDescr(): write error.\n");
    return(ERROR);
  }
  return(OK);
}

int32_t StoreContData(cont_data)
 contData *cont_data;
{
  int i=0;

  if (!StoreInt32(cont_data->nr_of_objects)) {
    ErrHdr();
    printf("StoreContData(): write error.\n");
    return(ERROR);
  }
  for (i=0; i<MAX_CONTAINED_OBJECTS; i++) {
    if (!StoreInt32(cont_data->object_ids[i])) {
      ErrHdr();
      printf("StoreContData(): write error.\n");
      return(ERROR);
    }
  }
return(OK);
}

int32_t StoreAdverbInfo(adverbs)
 adverbInfo *adverbs;
{
  int i=0;

  if (!StoreInt32(adverbs->nr_of_adverbs)) {
    ErrHdr();
    printf("StoreAdverbInfo(): Error writing number of adverbs.\n");
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_ADVERBS; i++) {
    if (!StoreInt32(adverbs->adverb[i])) {
      ErrHdr();
      printf("StoreAdverbInfo(): Error writing adverb.\n");
      return(ERROR);
    }
  }

  return(OK);
}

int32_t StorePreposInfo(prepositions)
 preposInfo *prepositions;
{
  int i=0;

  if (!StoreInt32(prepositions->nr_of_prepositions)) {
    ErrHdr();
    printf("StorePreposInfo(): Error writing number of prepositions.\n");
    return(ERROR);
  }
  for (i=0; i<MAX_PARSE_PREPOS; i++) {
    if (!StoreInt32(prepositions->preposition[i])) {
      ErrHdr();
      printf("StorePreposInfo(): Error writing preposition.\n");
      return(ERROR);
    }
  }

  return(OK);
}

int32_t StoreActionRec(action_rec)
 actionRec *action_rec;
{
  if (!StoreKeyword(ACTION_REC))
    return(ERROR);

  /* store the action_rec */
  if (!StoreInt32(action_rec->actor)) {
    ErrHdr();
    printf("StoreActionRec(): Error writing actor.\n");
    return(ERROR);
  }

  if (!StoreInt32(action_rec->action)) {
    ErrHdr();
    printf("StoreActionRec(): Error writing action.\n");
    return(ERROR);
  }
  if (!StoreAdverbInfo(&action_rec->adverbs))
    return(ERROR);

  if (!StoreInt32(action_rec->q_word)) {
    ErrHdr();
    printf("StoreActionRec(): Error writing question word.\n");
    return(ERROR);
  }
  if (!StoreInt32(action_rec->direction)) {
    ErrHdr();
    printf("StoreActionRec(): Error writing direction.\n");
    return(ERROR);
  }
  if (!StoreInt32(action_rec->subject)) {
    ErrHdr();
    printf("StoreActionRec(): Error writing subject.\n");
    return(ERROR);
  }
  if (!StoreInt32(action_rec->specifier)) {
    ErrHdr();
    printf("StoreActionRec(): Error writing specifier.\n");
    return(ERROR);
  }
  if (!StorePreposInfo(&action_rec->prepositions))
    return(ERROR);

  if (!StoreInt32(action_rec->value)) {
    ErrHdr();
    printf("StoreActionRec(): Error writing value.\n");
    return(ERROR);
  }
  if (!StoreInt32(action_rec->ordinal)) {
    ErrHdr();
    printf("StoreActionRec(): Error writing ordinal number.\n");
    return(ERROR);
  }
  if (!StoreInt32(action_rec->execute[0])) {
    ErrHdr();
    printf("StoreActionRec(): Error writing trigger.\n");
    return(ERROR);
  }
  if (!StoreInt32(action_rec->execute[1])) {
    ErrHdr();
    printf("StoreActionRec(): Error writing trigger.\n");
    return(ERROR);
  }

  return(OK);
}

int32_t StoreStoryInfo(info)
 storyInfo *info;
{
  int64_t offset;

  offset = ftell(datafile);
  dir_data.story_info_offset = offset;

  /* Store the storyInfo struct. */
  if (fwrite((void *) info->title, MAX_TITLE_LEN, 1, datafile) != 1) {
    ErrHdr();
    printf("StoreStoryInfo(): title write error\n");
    return(ERROR);
  }

  if (fwrite((void *) info->version, MAX_VERSION_LEN, 1, datafile) != 1) {
    ErrHdr();
    printf("StoreStoryInfo(): version write error\n");
    return(ERROR);
  }

  if (fwrite((void *) info->compiler_version, MAX_COMPILER_LEN, 1, datafile) != 1) {
    ErrHdr();
    printf("StoreStoryInfo(): compiler versions write error\n");
    return(ERROR);
  }
  return(OK);
}

int32_t StoreWordTable()
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
    printf("StoreWordTable(): error writing nr_of_words.\n");
    return(ERROR);
  }

  /* store the word list */
  for (i=0; i<nr_of_words; i++) {
    if (fwrite((void *) &(word_table[i].word), sizeof(char),
               MAX_WORD_LEN+1, datafile) != MAX_WORD_LEN+1) {
      ErrHdr();
      printf("StoreWordTable(): error writing word.\n");
      return(ERROR);
    }
    if (fwrite((void *) &(word_table[i].print_word), sizeof(char),
               MAX_WORD_LEN+1, datafile) != MAX_WORD_LEN+1) {
      ErrHdr();
      printf("StoreWordTable(): error writing printword.\n");
      return(ERROR);
    }
    if (!StoreInt32(word_table[i].id)) {
      ErrHdr();
      printf("StoreWordTable(): error writing word id.\n");
      return(ERROR);
    }
    for (j=0; j<MAX_TYPES; j++) {
      if (!StoreInt32(word_table[i].types[j])) {
        ErrHdr();
        printf("StoreWordTable(): error writing word type.\n");
        return(ERROR);
      }
    }
  }

  return(OK);
}


int32_t StoreTimerInfo()
{
  timerInfo *info = md_start;
  int64_t offset;

  if (nr_of_timers == 0) {
    printf("Warning: no timers/counters defined.\n");
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
    printf("StoreTimerInfo(): error writing nr of timers\n");
    return(ERROR);
  }

  /* now store the chain */
  while (info != NULL) {
    if (!StoreInt32(info->id)) {
      ErrHdr();
      printf("StoreTimerInfo(): error writing id.\n");
      return(ERROR);
    }

    if (!StoreInt32(info->value)) {
      ErrHdr();
      printf("StoreTimerInfo(): error writing value.\n");
      return(ERROR);
    }
    if (!StoreInt32(info->step)) {
      ErrHdr();
      printf("StoreTimerInfo(): error writing step.\n");
      return(ERROR);
    }
    if (!StoreInt32(info->interval)) {
      ErrHdr();
      printf("StoreTimerInfo(): error writing interval.\n");
      return(ERROR);
    }
    if (!StoreInt32(info->update)) {
      ErrHdr();
      printf("StoreTimerInfo(): error writing update.\n");
      return(ERROR);
    }
    if (!StoreInt16(info->direction)) {
      ErrHdr();
      printf("StoreTimerInfo(): error writing direction.\n");
      return(ERROR);
    }
    if (!StoreInt16(info->state)) {
      ErrHdr();
      printf("StoreTimerInfo(): error writing state.\n");
      return(ERROR);
    }
    if (!StoreInt32(info->trigger_at)) {
      ErrHdr();
      printf("StoreTimerInfo(): error writing threshold.\n");
      return(ERROR);
    }
    if (!StoreInt32(info->trigger_spec)) {
      ErrHdr();
      printf("StoreTimerInfo(): error writing trigger_spec.\n");
      return(ERROR);
    }
    if (!StoreInt32(info->execute[0])) {
      ErrHdr();
      printf("StoreTimerInfo(): error writing trigger.\n");
      return(ERROR);
    }
    if (!StoreInt32(info->execute[1])) {
      ErrHdr();
      printf("StoreTimerInfo(): error writing trigger.\n");
      return(ERROR);
    }

    info = info->next;
  } /* while */

  return(OK);
}


int32_t StoreFun(source, len)
 int32_t *source;
 int32_t len;      /* length of source */
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

int32_t StoreParList(source, len)
 int32_t *source; /* Format: nr_of_pars ( par1par2par3..parn ) */
 int32_t len;     /* length of source                          */
{
  int32_t  i;
  int      index=0;
  uint32_t str_len = -1;

  /* Store the number of parameters. */
  if (!StoreInt32(source[0])) {
    ErrHdr();
    printf("\nLine: %d: par list nr_of_pars write error\n", line_num);
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
            printf("\nLine %d: parameter list write error\n", line_num);
           return(ERROR);
          }
        }

        i += 2+str_len;
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

int32_t StoreExits()
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
    printf("StoreExits(): error writing nr_of_directions.\n");
    return(ERROR);
  }

  /* Store the first direction id. */
  if (!StoreInt32(first_direction_id)) {
    ErrHdr();
    printf("StoreExits(): error writing first_direction_id.\n");
    return(ERROR);
  }

  /* store the exit table */
  for (i=0; i<size; i++) {
    if (!StoreInt32(exit_table[i])) {
      printf("StoreExits(): write error.\n");
      return(ERROR);
    }
  }

  return(OK);
}

int32_t StoreFlags()
{

  int64_t     offset;
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

  int32_t i = 0;             /* index. */

  /* Malloc() space for common location flags. */
  if ((com_loc_flags = (int32_t *) malloc(com_loc_len*sizeof(int32_t))) == NULL) {
    ErrHdr();
    printf("StoreFlags(): error in malloc for common location flags.\n");
    return(ERROR);
  }

  /* Malloc() space for common object flags. */
  if ((com_obj_flags = (int32_t *) malloc(com_obj_len*sizeof(int32_t))) == NULL) {
    ErrHdr();
    printf("StoreFlags(): error in malloc for common object flags.\n");
    return(ERROR);
  }

  /* Malloc() space for local flags. */
  if ((local_flags = (int32_t *) malloc(local_len*sizeof(int32_t))) == NULL) {
    ErrHdr();
    printf("StoreFlags(): error in malloc for local flags.\n");
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

  /* First store the common location flags.               */
  /* Tell the interpreter the number of the common flags. */
  if (!StoreInt32(nr_of_cflags)) {
    ErrHdr();
    printf("StoreFlags(): error writing nr_of_cflags.\n");
    return(ERROR);
  }

    if (fwrite((void *) com_loc_flags, sizeof(int32_t), com_loc_len,
                                          datafile) != com_loc_len) {
    ErrHdr();
    printf("StoreFlags(): error writing common location flags.\n");
    return(ERROR);
  }

  /* Now store the common object flag bits.             */
  if (fwrite((void *) com_obj_flags, sizeof(int32_t), com_obj_len,
                                          datafile) != com_obj_len) {
    ErrHdr();
    printf("StoreFlags(): error writing common object flags.\n");
    return(ERROR);
  }

  /* Next store the local flags.                         */
  /* Tell the interpreter the length of the flag string. */
  if (!StoreInt32(local_len)) {
    ErrHdr();
    printf("StoreFlags(): error writing local_len.\n");
    return(ERROR);
  }

  if (fwrite((void *) local_flags, sizeof(int32_t), local_len,
                                         datafile) != local_len) {
    ErrHdr();
    printf("StoreFlags(): error writing local flags.\n");
    return(ERROR);
  }
  free(com_loc_flags);
  free(com_obj_flags);
  free(local_flags);

  return(OK);
}

int32_t StoreAttributes()
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

  int32_t i = 0;             /* index. */

  /* Malloc() space for common location attributes. */
  if ((com_loc_attrs = (attrInfo *) malloc(com_loc_len*sizeof(attrInfo))) == NULL) {
    ErrHdr();
    printf("StoreAttributes(): error in malloc for common location attributes.\n");
    return(ERROR);
  }

  /* Malloc() space for common object attributes. */
  if ((com_obj_attrs = (attrInfo *) malloc(com_obj_len*sizeof(attrInfo))) == NULL) {
    ErrHdr();
    printf("StoreAttributes(): error in malloc for common object attributes.\n");
    return(ERROR);
  }

  /* Malloc() space for local attributes. */
  if ((local_attrs = (attrInfo *) malloc(nr_of_lattrs*sizeof(attrInfo))) == NULL) {
    ErrHdr();
    printf("StoreAttributes(): error in malloc for local attributes.\n");
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

  /* Ok, we've filled the attribute arrays.           */
  /* now store the attribute arrays */
  /* get offset */
  offset = ftell(datafile);
  dir_data.attribute_data_offset = offset;

  /* Tell the interpreter the attribute are coming up next. */
  if (!StoreKeyword(ATTRIBUTES))
    return(ERROR);

  /* First store the common location attributes.               */
  /* Tell the interpreter the number of the common attributes. */
  if (!StoreInt32(nr_of_cattrs)) {
    ErrHdr();
    printf("StoreAttributes: error writing nr_of_cattrs\n");
    return(ERROR);
  }
  for (i=0; i<com_loc_len; i++) {
    if (!StoreInt32(com_loc_attrs[i].type)) {
      ErrHdr();
      printf("StoreAttributes: error writing common location attribute (type).\n");
      return(ERROR);
    }
    if (!StoreInt32(com_loc_attrs[i].value)) {
      ErrHdr();
      printf("StoreAttributes: error writing common location attribute (value).\n");
      return(ERROR);
    }
    if (!StoreInt32(com_loc_attrs[i].owner)) {
      ErrHdr();
      printf("StoreAttributes: error writing common location attribute (value owner).\n");
      return(ERROR);
    }
  }

  /* Now store the common object attributes. */
  for (i=0; i<com_obj_len; i++) {
    if (!StoreInt32(com_obj_attrs[i].type)) {
      ErrHdr();
      printf("StoreAttributes: error writing common object attribute (type).\n");
      return(ERROR);
    }
    if (!StoreInt32(com_obj_attrs[i].value)) {
      ErrHdr();
      printf("StoreAttributes: error writing common object attribute (value).\n");
      return(ERROR);
    }
    if (!StoreInt32(com_obj_attrs[i].owner)) {
      ErrHdr();
      printf("StoreAttributes: error writing common object attribute (value owner).\n");
      return(ERROR);
    }
  }

  /* Next store the local attributes.                     */
  /* Tell the interpreter the number of local attributes. */
  if (!StoreInt32(nr_of_lattrs)) {
    ErrHdr();
    printf("StoreAttributes(): error writing nr_of_lattrs.\n");
    return(ERROR);
  }

  for (i=0; i<nr_of_lattrs; i++) {
    if (!StoreInt32(local_attrs[i].type)) {
      ErrHdr();
      printf("StoreAttributes: error writing local attribute (type).\n");
      return(ERROR);
    }
    if (!StoreInt32(local_attrs[i].value)) {
      ErrHdr();
      printf("StoreAttributes: error writing local atttribute (value).\n");
      return(ERROR);
    }
    if (!StoreInt32(local_attrs[i].owner)) {
      ErrHdr();
      printf("StoreAttributes: error writing local atttribute (value owner).\n");
      return(ERROR);
    }
  }

  free(com_loc_attrs);
  free(com_obj_attrs);
  free(local_attrs);
  return(OK);
}

int32_t StoreVerbDir()

{ int32_t i;
  int32_t size = nr_of_verbs - nr_of_syn_verbs;

  if (!StoreKeyword(VERB))
    return(ERROR);

  /* store the number of directory entries. This number may differ */
  /* from the number of verbs because of possible synonyms. See    */
  /* comment with function InitVerbDir() in module tables.c.       */

  if (!StoreInt32(size)) {
    ErrHdr();
    printf("StoreVerbDir(): error writing size of verb_dir %d.\n", size);
    return(ERROR);
  }

  /* store the verb directory */
  for (i=0; i<size; i++) {
    if (!StoreInt32(verb_dir[i].scope)) {
      printf("\nError writing verb directory\n");
      return(ERROR);
    }
    if (!StoreInt64(verb_dir[i].offset)) {
      printf("\nError writing verb directory\n");
      return(ERROR);
    }
  }

  return(OK);
}

int32_t StoreLocDir()
{
  int64_t      offset;  /* directory's offset in outputfile */
  locationData *lp = loc_table;
  dirInfo      *loc_dir;
  int          i,j;

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
    ErrHdr();
    printf("StoreLocDir(): error in malloc().\n");
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
    ErrHdr();
    printf("StoreLocDir(): error writing number of locations.\n");
    free(loc_dir);
    return(ERROR);
  }

  /* Store the location directory. */
  for (i=0; i<nr_of_locations; i++) {
    if (!StoreInt32(loc_dir[i].nr_of_dsys)) {
      ErrHdr();
      printf("StoreLocDir(): write error.\n");
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
      ErrHdr();
      printf("StoreLocDir(): write error.\n");
      free(loc_dir);
      return(ERROR);
    }
    if (!StoreInt64(loc_dir[i].offset)) {
      ErrHdr();
      printf("StoreLocDir(): write error.\n");
      free(loc_dir);
      return(ERROR);
    }
  }
  free(loc_dir);
  return(OK);
}

int32_t StoreObjDir()
{
  int64_t      offset;  /* directory's offset in outputfile */
  objectData   *op = obj_table;
  dirInfo      *obj_dir;
  int32_t      i,j;

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
    ErrHdr();
    printf("StoreobjDir(): error in malloc().\n");
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
    ErrHdr();
    printf("StoreObjDir(): error writing number of objects.\n");
    free(obj_dir);
    return(ERROR);
  }

  /* store the object directory */
  for (i=0; i<nr_of_objects; i++) {
    if (!StoreInt32(obj_dir[i].nr_of_dsys)) {
      ErrHdr();
      printf("StoreObjDir(): write error.\n");
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
      ErrHdr();
      printf("StoreObjDir(): write error.\n");
      free(obj_dir);
      return(ERROR);
    }
    if (!StoreInt64(obj_dir[i].offset)) {
      ErrHdr();
      printf("StoreObjDir(): write error.\n");
      free(obj_dir);
      return(ERROR);
    }
  }

  free(obj_dir);
  return(OK);
}

int32_t StoreTriggOwners()
{
  triggerData *tp = trigg_table;
  int32_t     *trigg_owners;   /* The trigg_owners array is used   */
                               /* only in this function. Therefore */
                               /* it is not defined in module      */
                               /* tables.c                         */
  int64_t     offset;
  int         i=0;

  /* get the offset */
  offset = ftell(datafile);
  dir_data.trigg_owners_offset = offset;

  /* Malloc() space for trigg_owners */
  if ((trigg_owners = (int32_t *) malloc(nr_of_ltrigs*sizeof(int32_t))) == NULL) {
    ErrHdr();
    printf("StoreTriggOwners(): error in malloc for trigg_owners\n");
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
  /* the interpreter doesn't need to know  this.                 */
  if (!StoreInt32(nr_of_ltrigs)) {
    ErrHdr();
    printf("StoreTriggOwners(): error writing number of local triggers.\n");
    free(trigg_owners);
    return(ERROR);
  }

  /* Write the trigg_owners to file */
  for (i=0; i<nr_of_ltrigs; i++) {
    if (!StoreInt32(trigg_owners[i])) {
    ErrHdr();
    printf("StoreTriggOwners(): error writing trigger owner.\n");
    free(trigg_owners);
    return(ERROR);
    }
  }

  free(trigg_owners);
  return(OK);
}

int32_t StoreDescrOwners()
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
    ErrHdr();
    printf("StoredescrOwners(): error in malloc for descr_owners\n");
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
    ErrHdr();
    printf("StoreDescrOwners(): error writing number of descriptions.\n");
    free(descr_owners);
    return(ERROR);
  }

  /* Write the descr_owners to file */
  for (i=0; i<nr_of_ldescrs; i++) {
    if (!StoreInt32(descr_owners[i])) {
      ErrHdr();
      printf("StoreDescrOwners(): error writing description owner.\n");
      free(descr_owners);
      return(ERROR);
    }
  }

  free(descr_owners);
  return(OK);
}



int32_t StoreDirs()
{
  if (!StoreWordTable())  /* maps words to ids en types */
    return(ERROR);

  if (!StoreVerbDir())    /* tells where to find the default code */
    return(ERROR);        /* for verbs                            */

  if (!StoreLocDir())    /* store the location directory */
    return(ERROR);

  if (!StoreObjDir())    /* store the object directory   */
    return(ERROR);

  if (!StoreTriggOwners())     /* triggers are stored together    */
    return(ERROR);             /* with their owner's code         */

  if (!StoreDescrOwners())  /* desriptions are stored together */
    return(ERROR);          /* with their owner's code          */

  if (!StoreExits())     /* Map information. */
    return(ERROR);

  if (!StoreFlags())     /* Default flag values. */
    return(ERROR);

  if (!StoreAttributes()) /* Default attribute values. */
    return(ERROR);

  if (!StoreTimerInfo()) /* timer info */
    return(ERROR);

  /* FOLLOWING COMMENTED OUT ON JAN12 2015 */
  /* Force program to flush buffer; bug in TC 1.0 ???? */
  /*fwrite((void *) "h", 1, 1, datafile);    ???? */

  if (!StoreOffsets())    /* contains directory offsets       */
    return(ERROR);

  return(OK);
}


/**************************************/
/* Miscellaneous routines, needed by  */
/* StoreFlags() and StoreAttributes(). */
/**************************************/

void InitCFlags(flags, size, id, value)
 int32_t *flags;
 int32_t size;
 int32_t id;
 int32_t value;
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

void ProcCFlagVal(flags, owner, id, value)
 int32_t *flags;
 int32_t owner;
 int32_t id;
 int32_t value;
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


void ProcLFlagVal(flags, id, value)
 int32_t *flags;
 int32_t id;
 int32_t value;
{
  /* Id is used as an offset to address within flags. */
  int32_t word_offset = (id - FIRST_LOCAL_FLAG_ID) / WORD_LEN;
  int32_t bit_offset  = (id - FIRST_LOCAL_FLAG_ID) % WORD_LEN;

  SetBitVal(flags+word_offset, bit_offset, value);
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
      ErrHdr();
      printf("SetBitVal(): error in value, must be 1 or 0. %d\n", value);
      break;
  }
}

void InitCAttrs(attrs, size, id, type, value, value_owner)  /*10march2017 */
 attrInfo *attrs;
 int32_t  size;
 int32_t  id;
 int32_t  type;
 int32_t  value;
 int32_t  value_owner; /* for descriptions */  /*10march2017 */
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

void ProcCAttrVal(attrs, owner, id, type, value, value_owner)  /*10march2017 */
 attrInfo *attrs;
 int32_t  owner;
 int32_t  id;
 int32_t  type;
 int32_t  value;
 int32_t  value_owner; /* for descriptions */ /*10march2017 */
{
  int32_t index    = 0;
  int32_t first_id = 0;

  first_id = (IsLocId(owner)? FIRST_LOCATION_ID : FIRST_OBJECT_ID);

  index = (id-FIRST_COMMON_ATTR_ID) + (owner-first_id)*nr_of_cattrs;

  attrs[index].type  = type;
  attrs[index].value = value;
  attrs[index].owner = value_owner; /*10march2017 */
}

void ProcLAttrVal(attrs, id, type, value, value_owner)  /*10march2017 */
 attrInfo *attrs;
 int32_t  id;
 int32_t  type;
 int32_t  value;
 int32_t  value_owner; /*10march2017 */
{
  /* Id is used as an offset to address within attrs. */
  int32_t index = id - FIRST_LOCAL_ATTR_ID;

  attrs[index].type  = type;
  attrs[index].value = value;
  attrs[index].owner = value_owner; /*10march2017 */
}

