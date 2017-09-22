
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>         /* malloc(), free() */
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "prsetrig.h"


/*************************/
/* function declarations */
/*************************/

int32_t  StringToNum(char*, int32_t*);
void     PrintSource(int32_t*, int32_t);
int32_t  ParseLeftBanana(int32_t*, int32_t*);
int32_t  ParseRightBanana(int32_t*, int32_t*);
int32_t  ParseBoolOp(int32_t*, int32_t*, int32_t);
int32_t  ParseCTrigs(char**, int32_t*, FILE**);
int32_t  ParseFlags(char**, int32_t*, int32_t, FILE**);               /* both for common and local flags      */
int32_t  ParseAttributes(char**, int32_t*, int32_t, FILE**);          /* both for common and local attributes */
int32_t  ParseIfThenElse(char**, int32_t*, int32_t, int32_t, FILE**); /* to allow nested if statements        */
int32_t  ParseIf(char**, int32_t*, int32_t, int32_t, FILE**);
int32_t  ParseThenElse(char**, int32_t*, int32_t, int32_t, FILE**);
int32_t  ParseTriggers(char**, int32_t*, int32_t, FILE**);
int32_t  ParseCondString(int32_t*, int32_t, int32_t);

void PrintSource(source, len)
 int32_t *source;
 int32_t len;
{
  int32_t i;
  printf("***Printing source***\n");
  printf("len is %d\n", len);
  for (i=0;i<=len-1;i++)
    printf("%d ", source[i]);
  printf("\n*********************\n");
}

int32_t StringToNum(source, num)
 char *source;
 int32_t  *num;
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

int32_t ParseLeftBanana(target, index)
 int32_t *target;
 int32_t *index;
{
  if (*index == MAX_COND_LEN) {
    ErrHdr();
    printf("\nLine %d: max condition length exceeded.\n", line_num);
    return (ERROR);
  }
  target[(*index)++] = LEFT_PAR;
  return (OK);
}

int32_t ParseRightBanana(target, index)
  int32_t *target;
  int32_t *index;
{
  if (*index == MAX_COND_LEN) {
    ErrHdr();
    printf("\nLine %d: max condition length exceeded.\n", line_num);
    return (ERROR);
  }
  target[(*index)++] = RIGHT_PAR;
  return (OK);
}

int32_t ParseBoolOp(target, index, op_code)
 int32_t *target;
 int32_t *index;
 int32_t op_code;
{
  if (*index == MAX_COND_LEN) {
    ErrHdr();
    printf("\nLine %d: max condition length exceeded.\n", line_num);
    return (ERROR);
  }
  target[(*index)++] = op_code;
  return (OK);
}

int32_t ParseCTrigs(word, keyword, source)
 char    **word;
 int32_t *keyword;
 FILE    **source;
{
  int32_t state = 5; /* added one state prior to 1. */
  int32_t fun_buf[MAX_COND_LEN];
  int32_t index = 0;
  int32_t id;        /* Not used, needed for GetTriggerId(). */

  /* *word will be COMMON_TRIGGERS. */
  free(*word);
  *word = GetNextWord(keyword, NO_ID, 0, source);

  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source)) {
          break;
        }
        ErrHdr();
        printf("ParseCTrigs(): unexpected end of inputfile\n");
        return(ERROR);
      case ERROR:
        return(ERROR);
      case TRIGG_ID:
        switch (state) {
          case 5:
            /* Ok, there are common triggers. Handle the */
            /* table stuff.                              */
            /* Unfortunately we cannot hide the datafile */
            /* and dir_data here.                        */
            dir_data.common_trigg_offset = ftell(datafile);
            if (!StoreKeyword(COMMON_TRIGGERS)) {
              ErrHdr();
              printf("\nError writing COMMON_TRIGGERS keyword.\n");
              return(ERROR);
            }
            state = 1;
            break;
          case 1: ;
          case 2: ;
          case 3: ;
          case 4: ;
          case 6: ;
            state = 2;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected trigger id: %s\n", line_num, *word);
            return(ERROR);
        }
        if (!GetTriggerId((*word)+2, &id, COMMON_TRIGGERS, 1))
          /* 1 means it is defined here */
          return(ERROR);

        /* store the trigger id */
         if (!StoreId(id))
          return(ERROR);

        free(*word);
        *word = GetNextWord(keyword, NO_ID, 0, source);
        break;
      case LOC_ID:
        switch (state) {
          case 1:
          case 2:
          case 4:
          case 6:
            state = 6;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: Unexpected location identifier\n", line_num);
            return(ERROR);
        }
        if (!ParseAssignment(word, keyword, fun_buf, &index, THIS, source))
          return(ERROR);

        /* ParseAssignment() returns with index at the first unused position in the */
        /* string. Storefun() expects the length of the string, so therefore index  */
        /* will suffice since it starts at 0.                                       */
        if (!StoreFun(fun_buf, index))
          return(ERROR);
        index = 0;
        /* next word and keyword have already been read by ParseAssignment() */
        break;
      case OBJ_ID:
        switch (state) {
          case 1:
          case 2:
          case 4:
          case 6:
            state = 6;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: Unexpected object identifier\n", line_num);
            return(ERROR);
        }
        if (!ParseAssignment(word, keyword, fun_buf, &index, THIS, source))
          return(ERROR);

        if (!StoreFun(fun_buf, index))
          return(ERROR);
        index = 0;
        /* next word and keyword have already been read by ParseAssignment() */
        break;
      case ATTR_ID:
        switch (state) {
          case 1:
          case 2:
          case 4:
          case 6:
          case 7:
            state = 6;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: Unexpected attribute identifier\n", line_num);
            return(ERROR);
        }
        if (!ParseAssignment(word, keyword, fun_buf, &index, THIS, source))
          return(ERROR);

        if (!StoreFun(fun_buf, index))
          return(ERROR);
        index = 0;
        /* next word and keyword have already been read by ParseAssignment() */
        break;
      case DOT:
        switch(state) {
          case 4:
            state = 7;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected '.' keyword %d\n", line_num, state);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, NO_ID, 0, source);
        break;
      case TIM_ID:
        switch (state) {
          case 1:
          case 2:
          case 4:
          case 6:
            state = 6;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: Unexpected timer\n", line_num);
            return(ERROR);
        }
        if (!ParseAssignment(word, keyword, fun_buf, &index, THIS, source))
          return(ERROR);

        /* ParseAssignment() returns with index at the first unused position in the */
        /* string. Storefun() expects the length of the string, so therefore index  */
        /* will suffice since it starts at 0.                                       */
        if (!StoreFun(fun_buf, index))
          return(ERROR);
        index = 0;
        /* next word and keyword have already been read by ParseAssignment() */
        break;

      case OWNER:           /* It's an assignment with     */
        switch (state) {    /* syntax owner(obj).attribute */
          case 1:
          case 4:
          case 6:
            state = 6;
            if (!ParseAssignment(word, keyword, fun_buf, &index, THIS, source))
              return(ERROR);

            if (!StoreFun(fun_buf, index))
              return(ERROR);
            index = 0;
            /* next word and keyword have already been read by ParseAssignment() */
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected owner() function %s\n", line_num, *word);
            return(ERROR);
          }
        break;
      case IF:
        switch (state) {
          case 1: ;
          case 2: ;
          case 4:
          case 6:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: Unexpected IF keyword\n", line_num);
            return(ERROR);
        }
        /* Owner is the loc/obj which executes the common trigger. */
        if (!ParseIfThenElse(word, keyword, THIS, 0, source))
          return(ERROR);
        /* next word is returned by ParseIfThenElse() */
        break;
      case ENDIF:
        switch (state) {
          case 3:
            state = 4;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: Unexpected ENDIF keyword.\n", line_num);
            return(ERROR);
        }
        if (!StoreKeyword(ENDIF))
          return(ERROR);
        free(*word);
        *word = GetNextWord(keyword, NO_ID, 0, source);
        break;
      case NO_KEYWORD:
        ErrHdr();
        printf("\nLine %d: unexpected word: %s\n", line_num, *word);
        return(ERROR);
      default:
        /* For explanation about GetAttrId(), */
        /* see ParseIf().                    */
        if (IsIntAct(*keyword)) {
          switch (state) {
            case 1: ;
            case 2:   /* same as 4  */
            case 4:
            case 6:
              state = 4;
              break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected internal action: %s\n",
                      line_num, *word);
              return(ERROR);
          }
          if (!ParseFun(fun_buf, &index, *keyword, THIS, source))
            return(ERROR);

          if (!StoreFun(fun_buf, index))
            return(ERROR);
          index = 0;  /* reset index for the next function */
          free(*word);
          *word = GetNextWord(keyword, NO_ID, 0, source);
        } /* if */
        else
          /* 2 sep 2017. Used to be able to exit from any   */
          /* state. Changed this to states 1 thru 4 because */
          /* this is in the STD.                            */
          switch (state) {
            case 1: ;
            case 2: ;
            case 3: ;
            case 4:
               return(OK);
            default:
              printf("\nError");
              return(ERROR);
          }
      break; /* needed in case of an internal action */
    } /* switch */
  } /* while */
}

int32_t ParseFlags(word, keyword, owner_id, source)
 char    **word;
 int32_t *keyword;
 int32_t owner_id; /* needed to look up or store the flag id */
 FILE    **source;
{
  int32_t state = 1;
  char    name[MAX_ID_LENGTH + 1]; /* Needed to remember flag name. */
  int32_t id;                      /* Needed to convert flag ids.   */
  int32_t n;                       /* Needed to convert numstrings. */

  free(*word);
  *word = GetNextWord(keyword, owner_id, 0, source);
  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source)) {
          break;
        }
        ErrHdr();
        printf("ParseFlags(): unexpected end of inputfile\n");
        return(ERROR);
      case ERROR:
        return(ERROR);
      case FLAG_ID:
        switch (state) {
          case 1: ; /* same as 4 */
          case 4:
            state = 2;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected flag id: %s\n", line_num, *word);
            return(ERROR);
        }
        /* Remember flag name for later use.                   */
        /* We must have the flag's initial value before we can */
        /* call GetFlagId().                                   */
        /* *word already has been truncated (if necessary) in  */
        /* module keyword.c                                    */

        strncpy(name, *word, MAX_ID_LENGTH);

        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;
      case EQUAL_SIGN:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected =\n", line_num);
            return(ERROR);
        }
        /* skip the = */
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;
      case NO_KEYWORD:
        if (StringToNum(*word, &n)) {
          switch (state) {
            case 3:
              state = 4;
              break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected value: %d\n", line_num, n);
              return(ERROR);
          } /* switch */
          if (n!=1 && n!=0) {
            ErrHdr();
            printf("\nLine %d: flag value must be either 0 or 1\n", line_num);
            return(ERROR);
          }
          /* Ok, we have the value. Now get the flag's id. */
          /* The flag's name is in the name var.           */
          if (!GetFlagId(name+2, &id, n, owner_id, 1))
            /* 1 means flag is defined here */
            return(ERROR);

          free(*word);
          *word = GetNextWord(keyword, owner_id, 0, source);
        } /* if */
        else {
          ErrHdr();
          printf("\nLine %d: unexpected word: %s\n", line_num, *word);
          return(ERROR);
        }
        break;
      default:
        switch (state) {
          case 1: ; /* same as 4 */
          case 4:
            return(OK);  /* ok to exit */
          default:
            ErrHdr();
            printf("\nLine %d: unexpected word: %s\n", line_num, *word);
            return(ERROR);
        }
    } /* switch */
  } /* while */
}

int32_t ParseAttributes(word, keyword, owner_id, source)
 char    **word;
 int32_t *keyword;
 int32_t owner_id; /* needed to look up or store the attribute id */
 FILE    **source;
{

  /* Function ParseAttributes() added on July 2nd. It replaces  */
  /* ParseCAttributes() and ParseLAttributes(). Revised because */
  /* attributes are now prefixed with r_                        */

  /* 08mar2017: added descriptions as possible value */

  int32_t  state       = 1;
  char     name[MAX_ID_LENGTH + 1]; /* Needed to remember attribute name. */
  int32_t  id;                      /* Needed to convert attribute ids.   */
  int32_t  type        = NO_TYPE;
  int32_t  value       = NONE;
  int32_t  descr_owner = NO_ID; /* in case of descr id as initial value */

  free(*word);
  *word = GetNextWord(keyword, owner_id, 0, source);
  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source)) {
          break;
        }
        ErrHdr();
        printf("ParseAttributes(): unexpected end of inputfile\n");
        return(ERROR);

      case ERROR:
        return(ERROR);

      case ATTR_ID:
        switch (state) {
          case 1: ; /* same as 4 */
          case 4: ;
            state = 2;
            break;
          case 5:
            state = 2;
            /* We came from a previous definition with */
            /* syntax: r_attr = loc_id/obj_id.         */
            /* Store the previous attribute whose name */
            /* still is in the 'name' variable.        */

            /* Now store the attribute with the        */
            /* location or object id (= descr_owner)   */
            /* id as its value                         */ /* 10march2017 */
            if (!GetAttrId(name+2, owner_id, &id, type, descr_owner, NO_ID, 1)) /* 10march2017 */
              /* 1 means attribute is defined here */
              return(ERROR);
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected attribute id: %s\n", line_num, *word);
            return(ERROR);
        }
        /* Remember attribute name for later use.                   */
        /* We must have the attribute's initial value before we can */
        /* call GetAttrId().                                        */
        /* *word already has been truncated (if necessary) in       */
        /* module keyword.c                                         */

        strncpy(name, *word, MAX_ID_LENGTH);

        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;

     case EQUAL_SIGN:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected =\n", line_num);
            return(ERROR);
        }
        /* skip the = */
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;

      case LOC_ID:
        switch (state) {
          case 3:
            state = 5;
            type = LOC_ID;      /* need this to distinguish from a value */

            /* Now get the location identifier */
            if (!GetLocationId((*word)+2, &value, 0, 0))
              /* 0 means location is not defined here, only referenced */
              return(ERROR);

            /* we must not store the location id yet, because it */
            /* may be the owner of a description that follows.   */
            descr_owner = value;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected location Identifier\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;

      case OBJ_ID:
        switch (state) {
          case 3:
            state = 5;
            type = OBJ_ID;      /* need this to distinguish from a value */

            /* Now get the object identifier */
            if (!GetObjectId((*word)+2, &value, 0, 0))
              /* 0 means object is not defined here, only referenced */
              return(ERROR);

            /* we must not store the object id yet, because it */
            /* may be the owner of a description that follows. */
            descr_owner = value;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected object identifier\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;

      case THIS:
        /* value 'this' in storyfile will at runtime evaluate */
        /* to a location or object id                         */
        switch (state) {
          case 3:
            state = 4;
            type  = NO_TYPE;
            value = THIS;
            /* Now store the attribute with NONE as its value */
            if (!GetAttrId(name+2, owner_id, &id, type, value, NO_ID, 1)) /* 10march2017 */
              /* 1 means attribute is defined here */
              return(ERROR);
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected NONE keyword\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;

      case NONE:
        /* attribute has no value yet, value 'none' in story file */
        switch (state) {
          case 3:
            state = 4;
            type  = NO_TYPE;
            value = NONE;
            /* Now store the attribute with NONE as its value */
            if (!GetAttrId(name+2, owner_id, &id, type, value, NO_ID, 1)) /* 10march2017 */
              /* 1 means attribute is defined here */
              return(ERROR);
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected NONE keyword\n", line_num);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;

     case DOT:
        /* this is a <loc/obj>.d_xxx initialization */
        switch (state) {
          case 5:
            state = 6;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected '.'\n", line_num);
            return(ERROR);
        }
        /* skip the . */
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;

      case DESCR_ID:
        switch (state) {
          case 3:
            state = 4;
            descr_owner = owner_id;
            break;
          case 6:
            state = 4;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected description identifier\n", line_num);
            return(ERROR);
        }
        type = DESCR_ID;      /* need this to distinguish from a value */
        /* Now get the object identifier */
        if (!GetDescrId((*word)+2, &value, descr_owner, 0))
          /* 0 means object is not defined here, only referenced */
          return(ERROR);

        /* Now store the attribute with the description id as its value    */
        /* and the descr_owner as its owner (descr_owner is also stored in */ /* 10march2017 */
        /* the attribute information struct.                               */
        if (!GetAttrId(name+2, owner_id, &id, type, value, descr_owner, 1)) /* 10march2017 */
          /* 1 means attribute is defined here */
          return(ERROR);

        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break;

      default:
        switch (state) {
          case 1: ;
          case 4:
            return(OK);  /* ok to exit */
            break;
          case 5:
            /* we were expecting a dot but must exit instead    */
            /* we read a loc or obj and saved is as descr_owner */
            /* but since there was no dot coming, we must now   */
            /* store the loc/obj as the attribute value         */

            /* Now store the attribute with the */
            /* descr_owner as its value         */
            if (!GetAttrId(name+2, owner_id, &id, type, descr_owner, NO_ID, 1))
              /* 1 means attribute is defined here */
              return(ERROR);

            /* exit now */
            return(OK);
            break;
          default:
            /* do nothing */ ;
        }

        /* It could be a value */
        if (StringToNum(*word, &value)) {
          switch (state) {
            case 3:
              state = 4;
              type = NUMBER;
              break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected value: %d\n", line_num, value);
              return(ERROR);
          } /* switch */
        } /* if */
        else {
          /* It could be a direction. GetNextWord() will not have returned a    */
          /* DIRECTION keyword as we don't know beforehand which directions the */
          /* story will have. We therefore have to check for a direction word.  */
          if (IsDirection(*word, &value)) {
            switch (state) {
              case 3:
                state = 4;
                type = DIRECTIONS;
                break;
              default:
                ErrHdr();
                printf("\nLine %d: unexpected direction: %s\n", line_num, *word);
                return(ERROR);
            } /* switch */
          } /* if */
          else {
            /* It could be a word from the vocabulary                                */
            /* Make sure this is done after checking for DIRECTION (is also a word). */
            /* LookUpId() returns the word id for existing words, otherwise NO_ID    */
            /* CAREFUL: for numberstrings it returns the integer value               */
            /* Have to change this in the future, no issue here because we already   */
            /* checked for a numstring above.                                        */
            value = LookUpId(*word);

            if (value != NO_ID) {
              switch (state) {
                case 3:
                  state = 4;
                  type = WORD_ID;
                  break;
              default:
                ErrHdr();
                printf("\nLine %d: illegal word as attribute value, must be word from vocabulary: %s\n", line_num, *word);
                return(ERROR);
              } /* switch */
            } /* if */
            else {
              /* It was neither a value nor a direction, nor a word */
              ErrHdr();
              printf("\nLine %d: unexpected word: %s\n", line_num, *word);
              return(ERROR);
            } /* else */
          } /* else */
        } /* else */

        /* Ok, it was a direction, a value or a word. Now get the attribute's id. */
        /* The attributes's name is in the name variable                          */

        if (!GetAttrId(name+2, owner_id, &id, type, value, NO_ID, 1)) /* 10march2017 */
          /* 1 means attribute is defined here */
          return(ERROR);

        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break; /* default */
    } /* switch */
  } /* while */
}

int32_t ParseCondString(source, len, leading_not)
 int32_t *source;
 int32_t len;                /* length of source              */
 int32_t leading_not;
{
  int32_t cont = 1;          /* continue                      */
  int32_t level = 0;
  int32_t index = 0;
  int32_t store_fun = 1;     /* assume it's just one function */
  int32_t *left_part;
  int32_t left_index = 0;
  int32_t result;

  if (len <= 0)
    return(OK);
  left_part  =  (int32_t *) malloc(len*sizeof(int32_t));

  while ( (index < len) && cont) {
    switch (source[index]) {
      case LEFT_PAR: if ((level != 0) || leading_not)
                       left_part[left_index++] = LEFT_PAR;
      				 index++;
      				 level++;
      				 if (level == 2)
      				   store_fun = 0; /* not just one function */
      				 break;
      case RIGHT_PAR: if ((level != 1) || leading_not)
                        /* skip the last right par */
                        left_part[left_index++] = RIGHT_PAR;
      				  index++;
      				  level--;
      				  break;
      case AND:		  if (level == 0) {
       				    cont = 0;
       				    StoreKeyword(AND);
       				    /* do not increase index */
       				  }
       				  else {
       				    ParseBoolOp(left_part, &left_index, AND);
       				    index++;
       				    store_fun = 0;
       				  }
       				  break;
      case OR:		  if (level == 0) {
       				    cont = 0;
       				    StoreKeyword(OR);
       				    /* do not increase index */
       				  }
       				  else {
       				    ParseBoolOp(left_part, &left_index, OR);
       				    index++;
       				    store_fun = 0;
       				  }
       				  break;
      case NOT:		  if (leading_not && (level == 0)) {
                        /* must store the leading NOT */
                        StoreKeyword(NOT);
                        index+=2;        /* skip 'NOT('   */
                        len--;           /* skip last ')' */
                        leading_not = 0; /* reset flag    */
                      }
                      else {
                        ParseBoolOp(left_part, &left_index, NOT);
                        if (level == 0) {
                          leading_not = 1;
                          /* don't try to store a */
                          /* function here        */
                          store_fun = 0;
                        }
       				    index++;
       				  }
       				  break;

      default:		  /* it is a function */
      				  while ((left_part[left_index++] =
      				          source[index++]) != RIGHT_PAR)
      				    /* do nothing */;
    } /* switch */
  } /* while */

  if (store_fun) {
    if (!StoreFun(left_part, left_index))
      return(ERROR);
    left_index = -1;
  }

  if ((result = ParseCondString(left_part, left_index, leading_not))) {
    index++;
    result = ParseCondString(&(source[index]), len-index, 0);
  }
  free(left_part);
  return (result);
}


int32_t ParseIf(word, keyword, owner_id, disambig, source)
 char    **word;   /* output parameter */
 int32_t *keyword; /* output parameter */
 int32_t owner_id; /* location or object that owns the if-statement */
 int32_t disambig; /* tells whether we are parsing disambig code    */
 FILE    **source;
{
  int32_t state = 1;
  int32_t cont = 1;
  int32_t leading_not = 0;   /* assume no leading NOT */
  int32_t left_part[MAX_COND_LEN];
  int32_t right_part[MAX_COND_LEN];
  int32_t level = 0;
  int32_t left_index = 0;
  int32_t right_index = 0;

  free(*word);
  *word = GetNextWord(keyword, owner_id, 0, source);
  while (cont) {
    switch (*keyword) {
      case EOF:
        ErrHdr();
        printf("ParseIf(): unexpected EOF\n");
        return(ERROR);
      case ERROR:
        return(ERROR);
      case THEN:
        switch (state) {
          case 3:  /* remains the same */
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected THEN\n", line_num);
            return (ERROR);
        }
        cont = 0;  /* ready */
    	break;
      case LEFT_PAR:
        switch (state) {
          case 1:
            break;
          case 2:
            state = 1;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected left parenthesis\n", line_num);
            return (ERROR);
        };
        if ((level !=0) || leading_not) {
          /* if we have parsed a 'NOT', we do need to */
          /* parse the level 0 left parentheses       */
          if (!(ParseLeftBanana(left_part, &left_index)))
      	    return (ERROR);
      	}
      	level++;
      	free(*word);
    	*word = GetNextWord(keyword, owner_id, 0, source);
    	break;
      case RIGHT_PAR:
        switch (state) {
          case 3:    /* remains the same */
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected right parenthesis\n", line_num);
            return (ERROR);
        };
        if ((level != 1) || leading_not) {
          /* if we have parsed a 'NOT', we do need to */
          /* parse the level 1 right parentheses       */
          if (!(ParseRightBanana(left_part, &left_index)))
      	    return (ERROR);
      	}
      	level--;
      	if (level < 0) {
    	  ErrHdr();
          printf("\nLine %d: negative parentheses level\n", line_num);
    	  return (ERROR);
    	}
    	free(*word);
    	*word = GetNextWord(keyword, owner_id, 0, source);
    	break;
      case AND: ; /* same as OR */
      case OR:
        switch (state) {
          case 3:
            state = 1;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected AND/OR\n", line_num);
            return (ERROR);
        };
        if (level == 0) {
      	  StoreKeyword(*keyword);
    	  cont = 0;  /* stop */
    	}
    	else {
    	  if(!(ParseBoolOp(left_part, &left_index, *keyword)))
    	    return (ERROR);
    	  free(*word);
    	  *word = GetNextWord(keyword, owner_id, 0, source);
    	}
    	break;
      case NOT:
        switch (state) {
          case 1:
            state = 2;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected NOT\n", line_num);
            return (ERROR);
        }
        /* Conditions like NOT(A and B) OR C are tricky, since */
        /* we cannot parse the NOT before the OR.              */

        if(!(ParseBoolOp(left_part, &left_index, *keyword)))
    	  return (ERROR);
    	if (level == 0)
    	  /* As we are filling left_part, this must */
    	  /* be a leading NOT.                      */
    	  leading_not = 1;
    	free(*word);
    	*word = GetNextWord(keyword, owner_id, 0, source);
    	break;
      default:
        /* We cannot use IsAttrId() here, since we don't have    */
        /* the attribute id yet (GetNextWord() doesn't recognize */
        /* attributes, since they don't have a prefix).          */
        /* We reuse keyword for GetAttrId() here.                */
        /* !!!!!!!!Conditional OR is required here!!!!!!         */

        if (!(IsTestFun(*keyword))) {
      	  ErrHdr();
          printf("\nLine %d: unexpected word: %s\n", line_num, *word);
      	  return (ERROR);
      	}

      	switch (state) {
      	  case 1:
      	    state = 3;
      	    break;
      	  default:
      	    ErrHdr();
            printf("\nLine %d: unexpected testfunction\n", line_num);
      	    return (ERROR);
      	}
      	if (!(ParseFun(left_part, &left_index, *keyword, owner_id, source)))
      	  return (ERROR);

      	free(*word);
    	*word = GetNextWord(keyword, owner_id, 0, source);
    	break;
    } /* switch */
  } /* while */

  if (level != 0) {
    ErrHdr();
    printf("\nLine %d: Error in parenthesis level\n", line_num);
    return (ERROR);
  }

  if (*keyword != THEN) {
    /* we've just stored an operator */
    free(*word);
    *word = GetNextWord(keyword, owner_id, 0, source);
    cont = 1;
    while (cont) {
      switch (*keyword) {
        case EOF:
          ErrHdr();
          printf("ParseIf(): unexpected EOF\n");
          return(ERROR);
        case ERROR:
          return(ERROR);
        case THEN:
          switch (state) {
            case 3:  /* remains the same */
              break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected THEN\n", line_num);
              return (ERROR);
          }
          cont = 0;
    	  break;
        case LEFT_PAR:
          switch (state) {
            case 1:
              break;
            case 2:
              state = 1;
              break;
            default:
              ErrHdr();
              printf("\nLine %d: Unexpected left parenthesis\n", line_num);
              return (ERROR);
          }
          if (!(ParseLeftBanana(right_part, &right_index)))
        	return (ERROR);
          level++;
          free(*word);
          *word = GetNextWord(keyword, owner_id, 0, source);
    	  break;
        case RIGHT_PAR:
          switch (state) {
            case 3:  /* remains the same */
            break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected right parenthesis\n", line_num);
              return (ERROR);
          }
          if (!(ParseRightBanana(right_part, &right_index)))
        	return (ERROR);
          level--;
      	  if (level < 0) {
    		ErrHdr();
            printf("\nLine %d: negative parentheses level\n", line_num);
    		return (ERROR);
    	  }
    	  free(*word);
    	  *word = GetNextWord(keyword, owner_id, 0, source);
    	  break;
        case AND: ; /* same as OR */
        case OR:
          switch (state) {
            case 3:
              state = 1;
              break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected AND/OR\n", line_num);
              return (ERROR);
          };
          if (!(ParseBoolOp(right_part, &right_index, *keyword)))
        	return (ERROR);
          free(*word);
    	  *word = GetNextWord(keyword, owner_id, 0, source);
    	  break;
        case NOT:
          switch (state) {
            case 1:
              state = 2;
              break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected NOT\n", line_num);
              return (ERROR);
          }
          if (!(ParseBoolOp(right_part, &right_index, *keyword)))
        	return (ERROR);
          free(*word);
    	  *word = GetNextWord(keyword, owner_id, 0, source);
    	  break;
        default:
          /* For explanation about GetAttrId(), */
          /* see code for left_part.           */
          if (!(IsTestFun(*keyword))) {
      	    ErrHdr();
            printf("\nLine %d: unexpected word: %s\n", line_num, *word);
      		return (ERROR);
      	  }
      	  switch (state) {
      	    case 1:
      	      state = 3;
      	      break;
      	    default:
      	      ErrHdr();
              printf("\nLine %d: unexpected testfunction\n", line_num);
      	      return (ERROR);
      	  }
          if (!(ParseFun(right_part, &right_index, *keyword, owner_id, source)))
        	return (ERROR);

          free(*word);
       	  *word = GetNextWord(keyword, owner_id, 0, source);
    	  break;
      } /* switch */
    } /* while */
  } /* if */

  if (level != 0) {
    ErrHdr();
    printf("\nLine %d: Error in parenthesis level\n", line_num);
    return (ERROR);
  }

  if (!(ParseCondString(left_part, left_index, leading_not)))
    return (ERROR);
  else
    return (ParseCondString(right_part, right_index, 0));
}

int32_t ParseThenElse(word, keyword, owner_id, disambig, source)
 char    **word;
 int32_t *keyword;
 int32_t owner_id;  /* location or object that owns */
                    /* the then or else-statement   */
 int32_t disambig;  /* tells whether we are parsing disambig code */
 FILE    **source;
{
  int32_t state = 1;
  int32_t fun_buf[MAX_COND_LEN];
  int32_t index = 0;

  free(*word);
  *word = GetNextWord(keyword, owner_id, 0, source);

  while (1) {
    switch (*keyword) {
      case ERROR:
        return(ERROR);
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source)) {
          break;
        }
        ErrHdr();
        printf("ParseThenElse(): unexpected end of inputfile\n");
        return(ERROR);
      case IF:          /* we have a nested if-statement here */
        switch (state) {
          case 1: ; /* same as 2 */
          case 2: ;
          case 4:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected IF\n", line_num);
            return(ERROR);
        }
        if (!ParseIfThenElse(word, keyword, owner_id, disambig, source))
          return(ERROR);
        /* next word is returned by ParseIfThenElse() */
        break; /* IF */
      /* Code to prevent ELSE from being defined as an attribute.         */
      /* now no longer necessary, because attributes are prefixed with r_ */
      case ELSE:
        switch(state) {
          case 2: ;  /* 8 jul 2015, why change the state if we do a return() right after? */
          case 3:
          case 4:
            return(OK);
          default:
            ErrHdr();
            printf("\nLine %d: unexpected ELSE keyword.\n", line_num);
            return(ERROR);
        }
        /* No break needed here. */
        /* end of code. */
      case ENDIF:
        switch (state) {
          case 2:  /* pass ENDIF on to calling function */
                   /* do not store it, the caller will  */
          case 4:       /* june 1 2016, moved up from next case statement  */
            return(OK);
          case 3:
/*          case 4: */ /* june 1 2016, moved to previous case statement */
            state = 2;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected ENDIF keyword\n", line_num);
            return(ERROR);
        }
        if (!StoreKeyword(ENDIF))
          return(ERROR);
        free(*word);
        *word = GetNextWord(keyword, owner_id, 0, source);
        break; /* ENDIF */
      case LOC_ID:
        /* it's an assignment */
        switch (state) {
          case 1:
          case 2:
          case 4:
            state = 4;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected location identifier %s\n",
                    line_num, *word);
            return(ERROR);
          }
          if (!ParseAssignment(word, keyword, fun_buf, &index, owner_id, source))
            return(ERROR);

          if (!StoreFun(fun_buf, index))
            return(ERROR);
          index = 0;
          /* next word and keyword have already been read by ParseAssignment() */
        break; /* LOC_ID */
      case OBJ_ID:
        /* it's an assignment */
        switch (state) {
          case 1: ;
          case 2: ;
          case 4:
            state = 4;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected object identifier %s\n",
                    line_num, *word);
            return(ERROR);
          }
          if (!ParseAssignment(word, keyword, fun_buf, &index, owner_id, source))
            return(ERROR);

          if (!StoreFun(fun_buf, index))
            return(ERROR);
          index = 0;
          /* next word and keyword have already been read by ParseAssignment() */
        break; /* OBJ_ID */
      case TIM_ID:
        /* it's an assignment */
        switch (state) {
          case 1:
          case 2:
          case 4:
            state = 4;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected timer identifier %s\n",
                    line_num, *word);
            return(ERROR);
          }
          if (!ParseAssignment(word, keyword, fun_buf, &index, owner_id, source))
            return(ERROR);

          if (!StoreFun(fun_buf, index))
            return(ERROR);
          index = 0;
          /* next word and keyword have already been read by ParseAssignment() */
        break; /* TIM_ID */
     case OWNER:
        /* it's an assignment with syntax owner(obj).attribute */
        switch (state) {
          case 1:
          case 2:
          case 4:
            state = 4;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected owner() function.\n", line_num);
            return(ERROR);
          }
          if (!ParseAssignment(word, keyword, fun_buf, &index, owner_id, source))
            return(ERROR);

          if (!StoreFun(fun_buf, index))
            return(ERROR);
          index = 0;
          /* next word and keyword have already been read by ParseAssignment() */
        break; /* OWNER */
      case ATTR_ID:
        /* it's an assignment */
        switch (state) {
          case 1:
          case 2:
          case 4:
          case 5:
            state = 4;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected attribute identifier %s\n",
                    line_num, *word);
            return(ERROR);
          }
          if (!ParseAssignment(word, keyword, fun_buf, &index, owner_id, source))
            return(ERROR);

          if (!StoreFun(fun_buf, index))
            return(ERROR);
          index = 0;
          /* next word and keyword have already been read by ParseAssignment() */
        break; /* ATTR_ID */
      case DOT:
        switch (state) {
          case 2:
            state = 5;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected '.' keyword.\n", line_num);
            return(ERROR);
          }

          free(*word);
          *word = GetNextWord(keyword, owner_id, 0, source);
        break; /* DOT */
      default:
        /* For explanation of GetAttrId(), */
        /* see ParseIf() function.        */
        if (IsIntAct(*keyword)) {
          switch (state) {
            case 1:
            case 2:
            case 4:
              state = 2;
              break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected internal action: %s\n",
                      line_num, *word);
              return(ERROR);
          }
          /* check for disambiguation mode */
          if (disambig && *keyword != SCORE) {
            ErrHdr();
            printf("\nLine %d: In disambiguation section, only score() function may be used.\n", line_num);
            return(ERROR);
          }

          if (!ParseFun(fun_buf, &index, *keyword, owner_id, source))
            return(ERROR);

          if (!StoreFun(fun_buf, index))
            return(ERROR);
          index = 0;  /* reset index for the next function */
          free(*word);
          *word = GetNextWord(keyword, owner_id, 0, source);
        } /* if */
        else {
          switch (state) {
            case 2: ; /* same as 3 */
            case 3:
            case 4:
              return(OK); /* ok to exit here */
            default:
              ErrHdr();
              printf("\nLine %d: unexpected word %s\n", line_num, *word);
              return(ERROR);
          }
        }
        break;  /* in case of an internal action */
    } /* switch */
  } /* while */
}

int32_t ParseIfThenElse(word, keyword, owner_id, disambig, source)
 char    **word;   /* *word will be "IF" */
 int32_t *keyword;
 int32_t owner_id; /* needed to look up and store trigger ids    */
 int32_t disambig; /* tells whether we are parsing disambig code */
 FILE    **source;
{
  int32_t state = 1;

  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source)) {
          break;
        }
        ErrHdr();
        printf("ParseIfThenElse(): unexpected end of inputfile\n");
        return(ERROR);
      case ERROR:
        return(ERROR);
      case IF:
        switch (state) {
          case 1:
            state = 2;
            break;
          default:
            ErrHdr();
            printf("\nLine: %d: unexpected IF\n", line_num);
            return(ERROR);
        }
        if (!StoreKeyword(IF))
          return(ERROR);

        if (!ParseIf(word, keyword, owner_id, disambig, source))
          return(ERROR);
        /* next word and keyword will be returned by ParseIf() */
        break;
      case THEN:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected THEN keyword\n", line_num);
            return(ERROR);
        }
        StoreKeyword(*keyword);

        if (!ParseThenElse(word, keyword, owner_id, disambig, source))
          return(ERROR);
        /* next word is returned by ParseThenElse() */
        break;
      case ELSE:
        switch (state) {
          case 3:
            state = 4;
            break;
          case 4:
            return(OK);  /* else belongs to another if-then    */
                         /* we have a nested if-statement here */
          default:
            ErrHdr();
            printf("\nLine %d: unexpected ELSE keyword\n", line_num);
            return(ERROR);
        }
        if (!StoreKeyword(ELSE))
          return(ERROR);

        if (!ParseThenElse(word, keyword, owner_id, disambig, source))
          return(ERROR);
        /* next word is returned by ParseThenElse() */
        break;
      default:
        switch (state) {
          case 3: ; /* same as 4 */
          case 4:
            return(OK);  /* ok to exit */
          default:
            ErrHdr();
            printf("\nLine %d: unexpected word: %s\n", line_num, *word);
            return(ERROR);
        }
    } /* switch */
  } /* while */
}

int32_t ParseTriggers(word, keyword, owner_id, source)
 char    **word;
 int32_t *keyword;
 int32_t owner_id; /* needed to look up and store trigger ids */
 FILE    **source;
{
  int32_t   state = 1;
  int32_t   fun_buf[MAX_COND_LEN];
  int32_t   index = 0;
  int32_t   id;
  actionRec action_rec;
  int32_t   owned_by = owner_id;
    /* needed to hold the owner from a trigger that appears in a */
    /* <string> -> [l_word/o_word] <t_word> sentence             */
    /* initial value is owner_id, in case [...] is absent        */

  free(*word);
  *word = GetNextWord(keyword, owned_by, 0, source);
  while (1) {
    switch (*keyword) {
      case EOF:
        /* check for EOF of an inserted file */
        if (ChangeInputFile(word, keyword, NO_ID, 0, source)) {
          break;
        }
        ErrHdr();
        printf("ParseTrigger(): unexpected end of inputfile\n");
        return(ERROR);
      case ERROR:
        return(ERROR);
      case STRING:
        switch (state) {
          case 1: ; /* same as 5 */
          case 5:
            state = 2;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected string: %s\n", line_num, *word);
            return(ERROR);
        }
        /* Initialize the action_rec. */
        InitActionRec(&action_rec);

        /* Parse the sentence.        */
        if (!ParseSentence(*word, &action_rec))
          return(ERROR);

        /* action_rec will be stored after the trigger id */
        /* has been read.                                 */

        free(*word);
        *word = GetNextWord(keyword, owned_by, 0, source);
        break;
      case ARROW:
        switch (state) {
          case 2:
            state = 3;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected ARROW keyword\n", line_num);
            return(ERROR);
        }
        /* do nothing, just skip the arrow */
        free(*word);
        *word = GetNextWord(keyword, owned_by, 0, source);
        break;
      case OBJ_ID:         /* OBJ_ID owns the trigger specified */
        switch (state) {   /* before the ARROW                  */
          case 3:
            state = 4;
            if (!GetObjectId((*word)+2, &owned_by, 0, 0))
              return(ERROR);
            /* owned_by will be used in the TRIGG_ID case */
            free(*word);
            *word = GetNextWord(keyword, owned_by, 0, source);
            break;
          case 6:
          case 7:
          case 9:
            state = 9;
            if (!ParseAssignment(word, keyword, fun_buf, &index, owner_id, source))
              return(ERROR);

            if (!StoreFun(fun_buf, index))
              return(ERROR);
            index = 0;
            /* next word and keyword have already been read by ParseAssignment() */
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected object id %s\n", line_num, *word);
            return(ERROR);
          }
        break;
      case LOC_ID:          /* LOC_ID owns the trigger specified */
        switch (state) {    /* before the ARROW                  */
          case 3:
            state = 4;
            if (!GetLocationId((*word)+2, &owned_by, 0, 0))
              return(ERROR);
            /* owned_by will be used in the TRIGG_ID case */
            free(*word);
            *word = GetNextWord(keyword, owned_by, 0, source);
            break;
          case 6:
          case 7:
          case 9:
            state = 9;
            if (!ParseAssignment(word, keyword, fun_buf, &index, owner_id, source))
              return(ERROR);

            if (!StoreFun(fun_buf, index))
              return(ERROR);
            index = 0;
            /* next word and keyword have already been read by ParseAssignment() */
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected location id %s\n", line_num, *word);
            return(ERROR);
          }
        break;
      case TIM_ID:
        switch (state) {
          case 6:
          case 7:
          case 9:
            state = 9;
            if (!ParseAssignment(word, keyword, fun_buf, &index, owner_id, source))
              return(ERROR);

            if (!StoreFun(fun_buf, index))
              return(ERROR);
            index = 0;
            /* next word and keyword have already been read by ParseAssignment() */
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected timer id %s\n", line_num, *word);
            return(ERROR);
          }
        break;
      case OWNER:           /* It's an assignment with     */
        switch (state) {    /* syntax owner(obj).attribute */
/*          case 3:*/
/*            state = 4;*/
/*            free(*word);*/
/*            *word = GetNextWord(keyword, owner_id, 0, source);*/
/*            break;*/
          case 6:
          case 7:
          case 9:
            state = 9;
            if (!ParseAssignment(word, keyword, fun_buf, &index, owner_id, source))
              return(ERROR);

            if (!StoreFun(fun_buf, index))
              return(ERROR);
            index = 0;
            /* next word and keyword have already been read by ParseAssignment() */
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected owner() function %s\n", line_num, *word);
            return(ERROR);
          }
        break;
      case ATTR_ID:
        switch (state) {
          case 6:
          case 7:
          case 9:
          case 10:
            state = 9;
            if (!ParseAssignment(word, keyword, fun_buf, &index, owner_id, source))
              return(ERROR);

            if (!StoreFun(fun_buf, index))
              return(ERROR);
            index = 0;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected attribute id %s\n", line_num, *word);
            return(ERROR);
          }
        /* next word and keyword have already been read by ParseAssignment() */
        break;
      case DOT:
        switch(state) {
          case 4:
            state = 11;
            break;
          case 7:
            state = 10;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected '.' keyword %d\n", line_num, state);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owned_by, 0, source);
        break;
      case TRIGG_ID:
        switch (state) {
          case 1:  ; /* same as 5  */
          case 5:  ; /* same as 7  */
          case 7:  ; /* same as 8  */
          case 8:
          case 9:
            state = 6;
            /* For this trigger, the owner is owner_id, */
            /* since it is a trigger defined here.      */
            if (!GetTriggerId((*word)+2, &id, owner_id, 1))
              return(ERROR);

            /* store the trigger id */
            if (!StoreId(id))
              return(ERROR);
            break;
          case 3: ; /* same as 11 */
          case 11:
            state = 5;
            /* for this trigger, the owner is owned_by, since     */
            /* it is a trigger defined elsewhere; it is a trigger */
            /* from a <string> -> [l_word/o_word] <t_word> line   */
            if (!GetTriggerId((*word)+2, &id, owned_by, 0))
              return(ERROR);

            /* Put the owner and trigger id in the action_rec.    */
            /* In case of a local trigger we don't have to store  */
            /* the owned_by, but since it must be stored for      */
            /* common triggers, we'll store it anyway.            */
            action_rec.execute[0] = owned_by;
            action_rec.execute[1] = id;

            /* Store the action_rec. */
            if (!StoreActionRec(&action_rec))
              return(ERROR);

            /* reset owned_by */
            owned_by = owner_id;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected trigger id: %s\n", line_num, *word);
            return(ERROR);
        }
        free(*word);
        *word = GetNextWord(keyword, owned_by, 0, source);
        break;
      case IF:
        switch (state) {
          case 6:  ; /* same as 7  */
          case 7:
          case 9:
            state = 8;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected IF keyword\n", line_num);
            return(ERROR);
        }
        if (!ParseIfThenElse(word, keyword, owner_id, 0, source))
          return(ERROR);
        /* next word is returned by ParseIfThenElse() */
        break;
      case ENDIF:
        switch (state) {
          case 8:
            state = 7;
            break;
          default:
            ErrHdr();
            printf("\nLine %d: unexpected ENDIF\n", line_num);
            return(ERROR);
        }
        if (!StoreKeyword(ENDIF))
          return(ERROR);

        free(*word);
        *word = GetNextWord(keyword, owned_by, 0, source);
        break;
      default:
        /* For explanation of GetAttrId(), */
        /* see ParseIf() function.        */
        if (IsIntAct(*keyword)) {
          switch (state) {
            case 6:  ; /* same as 7  */
            case 7:
            case 9:
              state = 7;
              break;
            default:
              ErrHdr();
              printf("\nLine %d: unexpected internal action: %s\n", line_num, *word);
              return(ERROR);
          }
          if (!ParseFun(fun_buf, &index, *keyword, owner_id, source))
            return(ERROR);

          if (!StoreFun(fun_buf, index))
            return(ERROR);
          index = 0;  /* reset index for the next function */
          free(*word);
          *word = GetNextWord(keyword, owned_by, 0, source);
        } /* if */
        else {
          switch (state) {
            case 1:  ; /* same as 5  */
            case 5:  ; /* same as 7  */
            case 7:  ; /* same as    */
            case 8:  ;
            case 9:    /* added on nov 30 2015 */
              return(OK);  /* ok to exit here */
            default:
              ErrHdr();
              printf("\nLine %d: unexpected word: %s\n", line_num, *word);
              return(ERROR);
          }
        } /* else */
      break; /* needed in case of an internal action */
    } /* switch */
  } /* while */
}

