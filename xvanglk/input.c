
/* Copyright 2016, 2017 Marnix van den Bos */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "glk.h"
#include "keyword.h"
#include "input.h"

char *GetInput(char*, char*);
void GetGlkInput(char*);
void GetGlkChar(char*);


 char *GetInput(line_buf, prompt)
  char *line_buf;
  char *prompt;
{
  /* Input is either read from the */
  /* commandline or from a file    */

  sprintf(outputline, "\n\n%s", prompt);
  Output(outputline);

  if (testmode) {
    fgets(line_buf, INPUT_LINE_LEN, testfile);
      if (!feof(testfile)) {
      /* overwrite \n character with '\0' character */
      /* if we leave \n in place, looking up the    */
      /* word will fail.                            */

      line_buf[strlen(line_buf)-1] = '\0';
      Output(line_buf);
      Output("\n");
    }
    else {
      /* we reached the end of the testfile*/
      sprintf(outputline, "%s\n", "*** end of testfile ***");
      Output(outputline);
      /* turn off testmode */
      testmode = 0;
      /* display the prompt */
      sprintf(outputline, "\n\n%s", prompt);
      Output(outputline);
    }
  }
  /* We do not use else here, because in case we read an */
  /* eof in testmode, we must ask the user for input to  */
  /* prevent an empty string to be returned, which will  */
  /* cause an 'I do not understand...' error.            */
  if (!testmode) {
    /* gets(line_buf); */
    GetGlkInput(line_buf);
    if (transcript) {
      if (fprintf(transcriptfile, "%s", line_buf) < 0) {
        printf("Output(): error writing to transcript file.\n");
        /* not a severe error, continue but turn off transcript mode */
        transcript = 0;
      }
      /* add <CR>. */
      /* we should check fprintf() again for errors, but we don't */
      /* the last one went ok, so assume this one will as well.   */
      fprintf(transcriptfile, "\n");
    }
  }
  return(line_buf);
}

void GetGlkInput(line_buf)
 char *line_buf;
{
  int     gotline;
  int     len;
  event_t ev;

  glk_request_line_event(mainwin, line_buf, INPUT_LINE_LEN, 0);
  gotline = 0;

  while (!gotline) {
    glk_select(&ev);
    switch (ev.type) {
      case evtype_LineInput:
        if (ev.win == mainwin) {
          len = ev.val1;
          line_buf[len] = '\0';
          gotline = 1;
        }
      break;
      case evtype_Arrange:
        /* Do nothing */
      break;
    }
  }
}

void GetGlkChar(ch)
 char *ch;
{
  int gotchar;
  event_t ev;

  glk_request_char_event(mainwin);
  gotchar = 0;
  while (!gotchar) {
    glk_select(&ev);
    switch (ev.type) {
      case evtype_CharInput:
        if (ev.win == mainwin) {
          *ch = ev.val1;
          gotchar = 1;
        }
      break;
      case evtype_Arrange:
        /* do nothing */
      break;
    }
  }
}
