
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
#include <stdlib.h>   /* malloc() */
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "init.h"
#include "defs.h"

#include "json.h"
#include "IFI.h"

#include "ifiglue.h"
#include "ifischema.h"


/***********/
/* Globals */
/***********/

FILE      *datafile       = NULL;  /* storyfile             */
FILE      *transcriptfile = NULL;  /* to store transcript   */
FILE      *testfile       = NULL;  /* to read testinput     */
FILE      *debugfile      = NULL;  /* to store debug dump   */

ifiStats  ifi_stats;
storyInfo story_info;
int32_t   *stack;     /* Needed for executing triggers. */
int32_t   sp = 0;     /* stackpointer                   */

char      *outputline;
char      *json_msg_from_story;


/*************************/
/* function declarations */
/*************************/

int32_t CompilerVersionOK(void);
int32_t ExitProgram(void);
void    FileNameFromJson(char*, char*);
void    GetFileName(int, char**, char*, int*);
int32_t main(int, char**);


/************************/
/* Function definitions */
/************************/

int32_t CompilerVersionOK()
{
  if (!ReadStoryInfo(&story_info))
    return(ERROR);

  if (strcmp(story_info.compiler_version, "2.3.4") != 0) {
    PrintError(37, NULL, NULL);
    PrintError(38, NULL, story_info.compiler_version);
    return(ERROR);
  }

  /* correct compiler version */
  return(OK);
}


int32_t ExitProgram()
{
  /* we need to exit, but don't close the console window immediately */

  /* close transcript file */
  if (transcript) {
    transcript = 0;
    fclose(transcriptfile);
  }

  /* Free all Mallocs */
  CleanUp();

  return(OK);
}


void FilenameFromJSON(json_string, filename)
 char *json_string;
 char *filename;
{
  int       index = 0;
  kvPair    kv;

  /* init the kv struct */
  kv.key                 = NULL;
  kv.value.type          = 0;
  kv.value.textstring    = NULL;
  kv.value.int_number    = 0;
  kv.value.float_number  = 0;

  /* make sure MAXIMUM_PATH is at least twice MAX_ID_LENGTH + 3 */
  char config_dir[MAXIMUM_PATH+1] = "";
  char data_dir[MAXIMUM_PATH+1] = "";
  char story[MAXIMUM_PATH+1] = "";

  filename[0] = '\0';

  if (strlen(json_string) == 0) {
    return;
  }

  /* check json_string for valid syntax */
  if (!ValidateJson(json_string)) {
    return;
  }

  /* following keys must be present */
  /* in json_string:                */
  /* story, configdir, datadir      */

  while (GetNextKVpair(json_string, &index, &kv)) {

    /* check if we need this key */
    switch(CheckIFI(kv.key)) {
      case IFI_REQ_CONFIGDIR:
        strncpy(config_dir, kv.value.textstring, MAXIMUM_PATH);
        config_dir[MAXIMUM_PATH] = '\0';  /* just make sure */
        kv.key                   = ResetString(kv.key);
        kv.value.textstring      = ResetString(kv.value.textstring);
        break;

      case IFI_REQ_DATADIR:
        strncpy(data_dir, kv.value.textstring, MAXIMUM_PATH);
        data_dir[MAXIMUM_PATH] = '\0';  /* just make sure */
        kv.key                 = ResetString(kv.key);
        kv.value.textstring    = ResetString(kv.value.textstring);
        break;

      case IFI_REQ_STORY:
        strncpy(story, kv.value.textstring, MAXIMUM_PATH);
        story[MAXIMUM_PATH] = '\0';  /* just make sure */
        kv.key              = ResetString(kv.key);
        kv.value.textstring = ResetString(kv.value.textstring);
        break;

      default:
        /* don't need this one */
        /* no error, just skip */
        kv.key              = ResetString(kv.key);
        kv.value.textstring = ResetString(kv.value.textstring);
        break;
    } /* case */
  } /* while */

  /* now create the filename: "config_dir/story"   */
  /* make sure it all fits in the filename string  */
  if (strlen(config_dir) + strlen(story) +1 > MAXIMUM_PATH + 1) {
    /* it doesn't fit */
    kv.key              = ResetString(kv.key);
    kv.value.textstring = ResetString(kv.value.textstring);
    return;
  }

  if (strlen(story) != 0) {
    if (strlen(config_dir) != 0) {
      strcat(filename, config_dir);

      /* add the '/' */
      strcat(filename, "/");
    }
    strcat(filename, story);
  }

  /* clean up */
  kv.key              = ResetString(kv.key);
  kv.value.textstring = ResetString(kv.value.textstring);

  return;
}


void GetFileName(nr_args, args, filename, json)
 int  nr_args;
 char **args;
 char *filename;
 int* json;
{
  int   i           = 1;
  char *initialJSON = NULL;

  filename[0]    = '\0';

  /* the command 'interpreter' itself is stored in argv[0] */

  while (i<nr_args) {
    if (strcmp(args[i], "-e") == 0) {
      /* we cannot set the json flag here, because  */
      /* flags have not been initialized yet        */
      /* set json-mode so we can set the flag later */
      *json = 1;

      /* option "-e <json_string>" allows some initial JSON to */
      /* be sent right at the start AND signifies JSON mode    */

      if (nr_args > i+1) {
        /* json string was specified (or at least there */
        /* is some other argument right after '-e'      */

        initialJSON = AddToString(initialJSON, args[i+1]);

        /* extract filename from json string  */
        /* will return empty filename in case */
        /* of an error                        */
        FilenameFromJSON(initialJSON, filename);
      }
      /* skip json string */
      i += 2;
    }
    else {
      if (strcmp(args[i], "-i") == 0) {
        if (nr_args > i+1) {
          /* inputfile was specified */
          strncpy(filename, args[++i], MAX_ID_LENGTH);
          i++;
        }
      }
      else {
        PrintError(41, NULL, args[i]);
        i++;
      }
    }
  } /* while */

  if (filename[0] == '\0') {
    /* either no file name was specified or there was */
    /* an error in the initial json string            */
    /* try story.dat as a last resort                 */
    strcpy(filename, "story.dat");
  }
  free(initialJSON);
}


/************************/
/*         main()       */
/************************/

int32_t main(argc, argv)
 int  argc;
 char **argv;
{
  char inputfile[MAXIMUM_PATH];
  int json = 0;

  /* init random number generator */
  srand((unsigned) time(NULL));

  /* malloc space for outputline */
  /* This must be the firsts malloc() because we need */
  /* outputline to print messages.                    */
  if ((outputline = (char *) malloc(OUTPUT_LINE_LEN*sizeof(char))) == NULL) {
    PrintError(15, NULL, "main()");
    ExitProgram();
    return(OK);
  }

  /* initialize ifi_stats struct  */
  ifi_stats.objects  = 0;
  ifi_stats.location = 1;
  ifi_stats.map      = 0;
  ifi_stats.picture  = 0;
  ifi_stats.items    = 0;
  ifi_stats.people   = 0;
  ifi_stats.move     = 0;

  /* initialize story_info struct */
  strcpy(story_info.title, "");
  strcpy(story_info.version, "1.0");
  strcpy(story_info.compiler_version, "2.3.4");
  strcpy(story_info.author, "");
  strcpy(story_info.organization, "");
  strcpy(story_info.cover_text, "");
  strcpy(story_info.android_mkt, "");
  strcpy(story_info.ios_mkt, "");
  strcpy(story_info.backimage, "");
  strcpy(story_info.effect, "");
  strcpy(story_info.primary_color, "");
  story_info.ui_sidebar     = 1;
  story_info.ui_textinput   = 1;
  story_info.ui_compass     = 1;
  story_info.autolink       = 0;
  story_info.xvan_language  = ENG;
  story_info.story_language = ENG;


  /* malloc space for stack            */
  if ((stack = (int32_t *) malloc(STACK_SIZE*sizeof(int32_t))) == NULL) {
    PrintError(15, NULL, "main()");
    ExitProgram();
    return(OK);
  }

  /* Get filename */
  GetFileName(argc, argv, inputfile, &json);

  /* Open the datafile */
  if ((datafile = fopen(inputfile, "rb")) == NULL) {
    PrintError(40, NULL, inputfile);
    ExitProgram();
    return(OK);
  }

  /* First, check the compiler version in the story_info struct */
  if (!CompilerVersionOK()) {
    ExitProgram();
    return(OK);
  }

  /* Read the directories from the datafile */
  if (!InitDirs()) {
    ExitProgram();
    return(OK);
  }

  /* check for json mode */
  if (json == 1) {
    ProcLFlagVal(JSON_MODE, 1);
  }

  /* Read the max possible number of verb default codes.  */
  /* Can only do so AFTER the verb dir has been read.     */
  if (!InitVerbs()) {
    ExitProgram();
    return(OK);
  }

  /* Read the max possible number of locations. */
  /* Can only do so AFTER the location dir has been read. */
  if (!InitLocations()) {
    ExitProgram();
    return(OK);
  }

  /* Read the max possible number of objects. */
  /* Can only do so AFTER the object dir has been read. */
  if (!InitObjects()) {
    ExitProgram();
    return(OK);
  }

  /* Read attributes.                    */
  /* Can only do this AFTER InitDirs(), */
  /* InitLocations() and InitObjects(). */
  if (!InitAttributes()) {
    ExitProgram();
    return(OK);
  }

  /* OK, we're all set. Let's roll! */
  LetsPlay();
  ExitProgram();
  return(OK);
}
