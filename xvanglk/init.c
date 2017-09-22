
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <stdlib.h>   /* malloc() */
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "glk.h"
#include "keyword.h"
#include "typedefs.h"
#include "init.h"

/***********/
/* Globals */
/***********/


winid_t mainwin   = NULL;          /* Glk story windows */
winid_t statuswin = NULL;          /* Glk status window */

FILE      *datafile       = NULL;  /* storyfile             */
FILE      *transcriptfile = NULL;  /* to store transcript   */
FILE      *testfile       = NULL;  /* to read testinput     */
FILE      *debugfile      = NULL;  /* to store debug dump   */

storyInfo story_info;
int32_t   *stack;     /* Needed for executing triggers. */
int32_t   sp = 0;     /* stackpointer                   */

char      *outputline;


/*************************/
/* function declarations */
/*************************/

int32_t CompilerVersionOK(void);
int32_t ExitProgram(void);


/************************/
/* Function definitions */
/************************/

int32_t CompilerVersionOK()
{
  if (!ReadStoryInfo(&story_info))
    return(ERROR);

  if (strcmp(story_info.compiler_version, "2.3") != 0) {
    sprintf(outputline, "\n\nThis interpreter version 2.3 needs compiler version 2.2 stories.\n");
    Output(outputline);
    sprintf(outputline, "\nThis story was compiled with XVAN compiler version %s.\n\n\n",
            story_info.compiler_version);
    Output(outputline);
    return(ERROR);
  }

  /* correct compiler version */
  return(OK);
}

int32_t ExitProgram()
{
  char ch;
  /* we need to exit, but don't close the console window immediately */

  /* close transcript file */
  if (transcript) {
    transcript = 0;
    fclose(transcriptfile);
  }

  sprintf(outputline, "\n*** hit a key ***\n");
  Output(outputline);
  GetGlkChar(&ch);

  /* Free all Mallocs */
  CleanUp();

  return(OK);
}

/************************/
/*         main()       */
/************************/


/* glk_change int32_t main() */
void glk_main()
{
  /* first create the windows */
  if (!CreateGlkWindows()) {
    /* creating windows failed, we cannot print an error msg */
    return;
  }

  /* init random number generator */
  srand((unsigned) time(NULL));

  /* malloc space for outputline */
  /* This must be the firsts malloc() because we need */
  /* outputline to print messages.                    */
  if ((outputline = (char *) malloc(OUTPUT_LINE_LEN*sizeof(char))) == NULL) {
    PrintString("main(): error in malloc for outputline.\n");
    Output(outputline);
    ExitProgram();
    return;
  }

  /* initialize story_info struct */
  strncpy(story_info.title, "<no title>", MAX_TITLE_LEN);
  strncpy(story_info.version, "<no version>", MAX_VERSION_LEN);
  strncpy(story_info.compiler_version, "<no version>", MAX_COMPILER_LEN);

  /* malloc space for stack            */
  if ((stack = (int32_t *) malloc(STACK_SIZE*sizeof(int32_t))) == NULL) {
    sprintf(outputline, "main(): error in malloc for stack.\n");
    Output(outputline);
    ExitProgram();
    return;
  }

  /* Open the datafile */
  if ((datafile = fopen(DATAFILE, "rb")) == NULL) {
    sprintf(outputline, "Error opening %s.\n", DATAFILE);
    Output(outputline);
    ExitProgram();
    return;
  }

  /* First, check the compiler version in the story_info struct */
  if (!CompilerVersionOK()) {
    ExitProgram();
    return;
  }

  /* Read the directories from the datafile */
  if (!InitDirs()) {
    ExitProgram();
    return;
  }


  /* Read the max possible number of verb default codes.  */
  /* Can only do so AFTER the verb dir has been read.     */
  if (!InitVerbs()) {
    ExitProgram();
    return;
  }


  /* Read the max possible number of locations. */
  /* Can only do so AFTER the location dir has been read. */
  if (!InitLocations()) {
    ExitProgram();
    return;
  }


  /* Read the max possible number of objects. */
  /* Can only do so AFTER the object dir has been read. */
  if (!InitObjects()) {
    ExitProgram();
    return;
  }


  /* Read attributes.                    */
  /* Can only do this AFTER InitDirs(), */
  /* InitLocations() and InitObjects(). */
  if (!InitAttributes()) {
    ExitProgram();
    return;
  }

/* PrintObjectDirectory(); */
  /* OK, we're all set. Let's roll! */
  LetsPlay();
  ExitProgram();
  return;
}
