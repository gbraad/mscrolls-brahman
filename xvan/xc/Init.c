
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
#include <stdlib.h>    /* malloc(), free() */
#include <string.h>
#include <stdint.h>
#include "keyword.h"
#include "typedefs.h"
#include "init.h"


/***********/
/* globals */
/***********/

int16_t  xvan_language  = ENG;
int16_t  story_language = ENG;

char     current_filename[MAX_FILENAME_LEN+1];
FILE     *source_file;
FILE     *datafile;
int32_t  line_num    = 1;
int32_t  total_lines = 0;  /* to keep track of number of lines. Since */
                           /* there is more than one file we reset    */
                           /* line_num                                */

char entrance_trigger[MAX_ID_LENGTH+1];
char exit_trigger[MAX_ID_LENGTH+1];
char default_trigger[MAX_ID_LENGTH+1];

/*************************/
/* Function declarations */
/*************************/

int32_t ExitMsg(void);
int32_t GetFileNames(int, char**, char*, char*);
int32_t PreDefs(void);
int32_t main(int, char**);


/************************/
/* Function definitions */
/************************/

int32_t ExitMsg()
{
  char ch;

  /* we need to exit, but don't close the console window immediately */

  printf("\n*** hit a key...\n");
  scanf("%c%c", &ch, &ch);
  return(OK);
}


int32_t GetFileNames(nr_args, args, input, output)
 int  nr_args;
 char **args;
 char *input;
 char *output;
{
  int i = 1;

  /* the command 'compiler' itself is stored in argv[0] */

  input[0]  = '\0';
  output[0] = '\0';

  while (i<nr_args) {
    if (strcmp(args[i], "-i") == 0) {
      if (nr_args > i+1) {
        /* inputfile was specified */
        strncpy(input, args[++i], MAX_FILENAME_LEN);
        i++;
      }
    }
    else
      if (strcmp(args[i], "-o") == 0) {
        if (nr_args > i+1) {
          /* outputfile was specified */
          strncpy(output, args[++i], MAX_FILENAME_LEN);
          i++;
        }
      }
      else {
        if (strcmp(args[i], "-d") == 0) {
          /* set debug compile mode */
          /* implement this later   */
          i++;
        }
        else {
          PrintError(4, NULL, args[i]);
        }
      }
  }

  /* now check whether filenames are filled */
  if (strlen(input) == 0) {
    printf("\n\nInputfile: ");
    scanf("%19s", input);
    input[MAX_FILENAME_LEN] = '\0';
    printf("\n");
  }

  if (strlen(output) == 0) {
    printf("\n\nOutputfile: ");
    scanf("%19s", output);
    output[MAX_FILENAME_LEN] = '\0';
    printf("\n");
  }

  /* check if filenames are the same */
  if (strcmp(input, output) == 0) {
    PrintError(5, NULL, NULL);
    return(ERROR);
  }
  else
    return(OK);
}


int32_t PreDefs(void)
{
  switch(xvan_language) {
    case ENG:
      return(ENG_PreDefs());
      break;
    case NL:
      return(NL_PreDefs());
      break;
    default:
      /* we should never get here, use English */
      return(ENG_PreDefs());
  }
}

int32_t main(argc, argv)
 int  argc;
 char **argv;
{
  fileList  **file_list = NULL;
  char      storyfile[MAX_FILENAME_LEN+1]  = "";
  char      outputfile[MAX_FILENAME_LEN+1] = "";
  int32_t   err = 0;       /* No errors yet. */
  storyInfo story_info;

  printf("                     ***********************************************\n");
  printf("                                        XVAN COMPILER\n");
  printf("                                        version 2.3.4\n");
  printf("                       Copyright 2016, 2017, 2018 Marnix van den Bos\n");
  printf("                     ***********************************************\n\n");



  printf("                           max  %d locations\n", LAST_LOCATION_ID-FIRST_LOCATION_ID+1);
  printf("                           max  %d objects\n", LAST_OBJECT_ID-FIRST_OBJECT_ID+1);
  printf("                           max   %d common descriptions\n", LAST_COMMON_DESCR_ID-FIRST_COMMON_DESCR_ID+1);
  printf("                           max %d local descriptions\n", LAST_LOCAL_DESCR_ID-FIRST_LOCAL_DESCR_ID+1);
  printf("                           max  %d common flags\n", LAST_COMMON_FLAG_ID-FIRST_COMMON_FLAG_ID+1);
  printf("                           max  %d local flags\n", LAST_LOCAL_FLAG_ID-FIRST_LOCAL_FLAG_ID+1);
  printf("                           max %d common attributes\n", LAST_COMMON_ATTR_ID-FIRST_COMMON_ATTR_ID+1);
  printf("                           max %d local attributes\n", LAST_LOCAL_ATTR_ID-FIRST_LOCAL_ATTR_ID+1);
  printf("                           max  %d common triggers\n", LAST_COMMON_TRIGGER_ID-FIRST_COMMON_TRIGGER_ID+1);
  printf("                           max  %d local triggers\n", LAST_LOCAL_TRIGGER_ID-FIRST_LOCAL_TRIGGER_ID+1);
  printf("                           max %d words\n", LAST_WORD_ID-FIRST_WORD_ID+1);
  printf("                           max %d verbs\n", LAST_VERB_ID-FIRST_VERB_ID+1);
  printf("                           max  %d timers\n", LAST_TIMER_ID-FIRST_TIMER_ID+1);

  /* get the filenames */
  if (!GetFileNames(argc, argv, storyfile, outputfile)) {
    ExitMsg();
    return(ERROR);
  }
  /* open the storyfile */
  strncpy(current_filename, storyfile, MAX_FILENAME_LEN+1);
  if ((source_file = fopen(storyfile, "rb")) == NULL) {
    /* error opening file */
    ErrHdr();
    PrintError(0, NULL, storyfile);
    ExitMsg();
    return(ERROR);
  }

  /* Reset the sourcefile */
  rewind(source_file);
  line_num = 1;

  /* malloc space for file_list */
  /* can't do this in InitFileList because file_list is a stack var */
  if ( (file_list = (fileList **) malloc(sizeof(fileList*))) == NULL) {
    /* error in malloc() for file_list */
    ErrHdr();
    PrintError(1, NULL, "file_list");
    return(ERROR);
  }

  if (!InitFileList(file_list, storyfile, source_file)) {
    /* error is printed by InitFileList() */
    return(ERROR);
  }

  /* read the story info so we can open the vocabulary file */
  if (!ReadStoryInfo(&story_info, storyfile, file_list)) {
    /* error was already printed by ReadStoryInfo() */
    ExitMsg();
    return(ERROR);
  }

  /* set the languages */
  xvan_language  = story_info.xvan_language;
  story_language = story_info.story_language;

  /* add compiler version to story info */
  strncpy(story_info.compiler_version, "2.3.4", MAX_COMPILER_LEN);

  printf("\n%s %s\n\n", story_info.title, story_info.version);

  /* open the output file */
  /* strncpy(current_filename, outputfile, MAX_ID_LENGTH); */
  if ((datafile = fopen(outputfile, "wb")) == NULL) {
    /* error opening file */
    ErrHdr();
    PrintError(0, NULL, outputfile);
    ExitMsg();
    return(ERROR);
  }

  if (!SetUpTables()) {
    /* error msg is priinted by SetUpTables() */
    ExitMsg();
    return(ERROR);
  }

  /* Initialize the directory offsets. */
  InitOffsets();

  /* store dummy directory data to reserve space at start of file */
  if (!StoreOffsets()) {
    ExitMsg();
    return(ERROR);
  }

  /* Predefine identifiers that must have special */
  /* values (some are #defined in keyword.h)      */
  if (!PreDefs()) {
    ExitMsg();
    return(ERROR);
  }

  /* store the story info in the outputfile */
  if (!StoreStoryInfo(&story_info)) {
    /* error msg is printed by StoreStoryInfo() */
    ExitMsg();
    return(ERROR);
  }

  if (!ParseStory(source_file, file_list))
    err = 1;

  else {
    if (!CheckTables()) /* Are there undefined identifiers?  */
      err = 1;
    else if (!StoreDirs())
      err = 1;
    } /* else */

  /* close the datafiles                   */
  /* storyfiles are closed by ParseStory() */
  fclose(datafile);

  /* release malloc'ed space */
  FreeTables();
  FreeFileList(file_list);

  if (err)
    PrintError(2, NULL, NULL);
  else
    PrintError(3, &((resultStruct){VALUE,total_lines}), NULL);

  ExitMsg();
  return(OK);
}