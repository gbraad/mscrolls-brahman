
/* Copyright 2016, 2017 Marnix van den Bos */

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

char     current_filename[MAX_ID_LENGTH+1];
FILE     *source_file;
FILE     *datafile;
int32_t  line_num = 1;
int32_t  total_lines = 0;  /* to keep track of number of lines. Since */
                           /* there is more than one file we reset    */
                           /* line_num                                */


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
  int ok = 1;

  /* the command 'compiler' itself is stored in argv[0] */

  input[0]  = '\0';
  output[0] = '\0';

  while (i<nr_args) {
    if (strcmp(args[i], "-i") == 0) {
      if (nr_args > i+1) {
        /* inputfile was specified */
        strncpy(input, args[++i], MAX_ID_LENGTH);
      }
    }
    else
      if (strcmp(args[i], "-o") == 0) {
        if (nr_args > i+1) {
          /* outputfile was specified */
          strncpy(output, args[++i], MAX_ID_LENGTH);
        }
      }
      else {
        if (strcmp(args[i], "-d") == 0) {
          /* set debug compile mode */
          /* implement this later   */
        }
        else {
          printf("Unknown commandline option: %s\n\n", args[i]);
          ok = 0;
        }
      }
    ++i;
  }

  if (!ok)
  {
      printf("Usage: %s [-i <inputfile>] [-o <outputfile>] [-d <level>]\n",
             args[0]);
      return ERROR;
  }

  /* now check whether filenames are filled */
  if (strlen(input) == 0) {
    printf("\n\nInputfile: ");
    scanf("%19s", input);
    input[MAX_ID_LENGTH] = '\0';
    printf("\n");
  }

  if (strlen(output) == 0) {
    printf("\n\nOutputfile: ");
    scanf("%19s", output);
    output[MAX_ID_LENGTH] = '\0';
    printf("\n");
  }

  /* check if filenames are the same */
  if (strcmp(input, output) == 0) {
    printf("\n\nInputfile and outputfile must have different names.");
    return(ERROR);
  }
  else
    return(OK);
}


int32_t PreDefs(void)
{
  /* Descriptions: d_sys will be used to hold the system       */
  /* description of a location or an object. The system        */
  /* description is used by the interpreter to refer to a      */
  /* location/object.                                          */
  /* Attributes: prepos is used to store a preposition, to be  */
  /* used for containment.                                     */
  /* Objects: make sure the player gets the first object id,   */
  /* which is PLAYER.                                          */
  /* Objects: make sure the no_such_thing object gets the      */
  /* second object id, which is NO_SUCH_THING.                 */
  /* Flags: f_lit will be used by the interpreter to indicate  */
  /* that locations and objects are lit (i.e. they are a       */
  /* lightsource). f_opaque indicates that an object or        */
  /* location is opaque. In the story code, the flags can be   */
  /* treated as any other flags (testflag, setflag). f_lit     */
  /* must always be the first common flag;  f_opaque the       */
  /* second. If this code is changed, make sure f_lit and      */
  /* f_opaque are always defined first.                        */
  /* Triggers: t_entrance is triggered when the player enters  */
  /* a location or an object. t_exit is triggered when the     */
  /* player leaves a location or an object. entrance and exit  */
  /* triggers are triggered for all objects that are contained */
  /* in the location/object the player enters/leaves.          */
  /* Make sure the order in which t_entrance and t_exit are    */
  /* defined is not changed and that these are the first       */
  /* common trigger definitions.                               */

  int32_t id = NO_ID;

  if (!GetDescrId("sys", &id, COMMON_DESCRS, 1))
    return(ERROR);

/* r_preposition must have default value in, but we have not yet parsed */
/* the vocabulary yet. Therefore, we will now make a reference and      */
/* define it with default value "in" after parsing the vocabulary.      */

  /* Make sure r_preposition gets value FIRST_COMMON_ATTR_ID, because   */
  /* the interpreter will use it in the move() function.                */
  if (!GetAttrId("preposition", COMMON_ATTRS, &id, WORD_ID, NONE, NO_ID, 1)) /* 10march2017 */
    return(ERROR);

  if (!GetObjectId("player", &id, 0, -1))
    return(ERROR);

  /* predefine no_such_thing */
  if (!GetObjectId("nst", &id, 0, -1))
    return(ERROR);

  if (!GetFlagId("lit", &id, 0, COMMON_FLAGS, 1))
    return(ERROR);

  if (!GetFlagId("opaque", &id, 0, COMMON_FLAGS, 1))
    return(ERROR);

  if (!GetFlagId("bypass", &id, 0, COMMON_FLAGS, 1))
    return(ERROR);

  if (!GetFlagId("hidden", &id, 0, COMMON_FLAGS, 1))
    return(ERROR);

  if (!GetFlagId("swap", &id, 0, COMMON_FLAGS, 1))
    return(ERROR);

  if (!GetTriggerId("entrance", &id, COMMON_TRIGGERS, 1))
    return(ERROR);

  if (!GetTriggerId("exit", &id, COMMON_TRIGGERS, 1))
    return(ERROR);

  if (!GetTriggerId("default", &id, COMMON_TRIGGERS, 1))
    return(ERROR);

  return(OK);
}

int32_t main(argc, argv)
 int  argc;
 char **argv;
{
  char      storyfile[MAX_ID_LENGTH+1]  = "";
  char      outputfile[MAX_ID_LENGTH+1] = "";
  int32_t   err = 0;       /* No errors yet. */
  storyInfo story_info;

  printf("                     *****************************************\n");
  printf("                                  XVAN COMPILER\n");
  printf("                                   version 2.3\n");
  printf("                       Copyright 2016, 2017 Marnix van den Bos\n");
  printf("                     *****************************************\n\n");



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
  strncpy(current_filename, storyfile, MAX_ID_LENGTH+1);
  if ((source_file = fopen(storyfile, "r")) == NULL) {
    ErrHdr();
    printf("\nError opening file: %s\n", storyfile);
    ExitMsg();
    return(ERROR);
  }

  /* Reset the sourcefile */
  rewind(source_file);
  line_num = 1;

  /* read the story info so we can open the vocabulary file */
  if (!ReadStoryInfo(&story_info, storyfile)) {
    ErrHdr();
    printf("\nError reading story information.\n");
    ExitMsg();
    return(ERROR);
  }
  /* add compiler version to story info */
  strncpy(story_info.compiler_version, "2.3", MAX_COMPILER_LEN);

  printf("\n%s %s\n\n", story_info.title, story_info.version);
  /* open the output file */
  /* strncpy(current_filename, outputfile, MAX_ID_LENGTH); */
  if ((datafile = fopen(outputfile, "wb")) == NULL) {
    ErrHdr();
    printf("\nError opening file: %s\n", outputfile);
    ExitMsg();
    return(ERROR);
  }

  if (!SetUpTables()) {
    ErrHdr();
    printf("main(): error setting up tables\n");
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
    ErrHdr();
    printf("\nError writing story info.\n");
    ExitMsg();
    return(ERROR);
  }

  if (!ParseStory(source_file))
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

  if (err)
    printf("\nCompilation aborted...\n");
/*  else */
/*    printf("%d lines compiled.\n\n", total_lines); */

  ExitMsg();
  return(OK);
}
