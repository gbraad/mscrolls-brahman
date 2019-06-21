
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018, 2019 Marnix van den Bos.             */
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "keyword.h"
#include "typedefs.h"
#include "json.h"
#include "IFI.h"
#include "input.h"

#include "ifiglue.h"

/*************************/
/* Function declarations */
/*************************/

void    GetAddtlInput(char*, char*);
int32_t ProcessInput(char*);

/************************/
/* Function definitions */
/************************/

void GetAddtlInput(char *addtl_input, char *prompt)
 /* addtl_input must have size INPUT_LINE_LEN */
{
  /* this function is used when we need extra  */
  /* input from the user to complete a command */
  /* e.g. 'x key (when there are more)' or     */
  /* 'quit'. If we would use ProcessInput()    */
  /* the interpreter would try to interpret it */
  /* as a new command.                         */
  /* NOTE: for missing subject or specifier we */
  /* do not use this function, because in this */
  /* situation we just return to letsplay()    */
  /* with partially completed action records   */
  /* and new parse syntax                      */

  char      *ifi_get_result = NULL;
  char      *json_string = NULL;
  int       index           = 0;
  int32_t   IFI_request  = IFI_NO_IFI;
  kvPair    kv;

  PrintString("\n", 0);
  PrintString(prompt, 0);
  Output();

  /* init the kvPair struct */
  kv.key                 = NULL;
  kv.value.type          = 0;
  kv.value.textstring    = NULL; /* not: '\0' */
  kv.value.int_number    = 0;
  kv.value.float_number  = 0;

  /* read the json string */
  ifi_get_result = (char*) ifi_getRequest();

  /* copy ifi_get_result as soon as we get it, because  */
  /* ifi_getRequest() will use the same address for all */
  /* calls. So if we don't copy, the result will be     */
  /* overwritten when another functions calls           */
  /* ifi_getRequest                                     */

  json_string = AddToString(json_string, ifi_get_result);

  /* check for valid json */
  if (!ValidateJson(json_string)) {
    /* do not set to NULL */
    addtl_input[0] = '\0';
    json_string = ResetString(json_string);
    return;
  }

  /* we have a valid json here */
  /* extract the KV-pair       */

  while (json_string[index] != '\0') {
    /* read the next key-value pair */
    if (!GetNextKVpair(json_string, &index, &kv)) {
      /* ready */
      json_string         = ResetString(json_string);
      kv.value.textstring = ResetString(kv.value.textstring);
      kv.key              = ResetString(kv.key);
      return;
    }

    IFI_request = CheckIFI(kv.key);

    /* must decide here what we do if not an IFI-request       */
    /* for now, send error msg and stop processing this string */
    if (IFI_request == IFI_NO_IFI) {
      SendIFIerror("UNKNOWN IFI REQUEST: ", kv.key);
    }

    /* test for IFI_REQ_COMMAND */
    if (IFI_request == IFI_REQ_COMMAND) {
      strncpy(addtl_input, kv.value.textstring, INPUT_LINE_LEN);
      addtl_input[INPUT_LINE_LEN-1] = '\0'; /* just make sure */
    }
    else {
      /* we don't care about the return value, here */
      XeqIFIrequest(IFI_request, &(kv.value));
    }
  }  /* while */
  json_string         = ResetString(json_string);
  kv.value.textstring = ResetString(kv.value.textstring);
  kv.key              = ResetString(kv.key);
}


int32_t ProcessInput(char *prompt)
{
  char *ifi_get_result = NULL;
  char  *json_string   = NULL;
  int32_t result       = OK;

  char line_buf[INPUT_LINE_LEN];

  /* Input is either read from the */
  /* IFI interface or from a file  */

  /* clear line_buf */
  line_buf[0] = '\0';

  if (testmode) {
    fgets(line_buf, INPUT_LINE_LEN, testfile);
      if (!feof(testfile)) {
      /* overwrite \n character with '\0' character */
      /* if we leave \n in place, looking up the    */
      /* word will fail.                            */

      /* line_buf must be terminated with */
      /* a '\0' for this to work.         */
      line_buf[strlen(line_buf)-1] = '\0';

      PrintString(line_buf, 0);
      PrintString("\n", 0);
      Output();

      /* now, wrap the line in a json string  */
      /* will always fit, because line_buf is */
      /* INPUT_LINE_LEN length at the max     */

      json_string = AddToString(json_string, "{\"command\":\"");
      json_string = AddToString(json_string, line_buf);
      json_string = AddToString(json_string, "\"}");

    }
    else {
      /* we reached the end of the testfile*/
      PrintError(43, NULL, NULL); /* not an error */
      /* turn off testmode */
      testmode = 0;
      /* do not display the prompt, IFI will */
      /* clear json string */
      free(json_string);
      json_string = NULL;
    }
  }
  /* We do not use else here, because in case we read an */
  /* eof in testmode, we must ask the user for input to  */
  /* prevent an empty string to be returned, which will  */
  /* cause an 'I do not understand...' error.            */

  if (!testmode) {
    ifi_get_result = (char*) ifi_getRequest();
    if (ifi_get_result == NULL) {
      /* front-end wants to quit */
      return(QUIT);
    }

    /* copy ifi_get_result as soon as we get it, because  */
    /* ifi_getRequest() will use the same address for all */
    /* calls. So if we don't copy, the result will be     */
    /* overwritten when another functions calls           */
    /* ifi_getRequest                                     */

    json_string = AddToString(json_string, ifi_get_result);
  }

  /* ok, now we have a json_string, either from   */
  /* the front end, or from file input (testmode) */

  /* check for a valid json */
  if (ValidateJson(json_string)) {
    /* we have a valid json string */
    if ( (result = ProcessJson(json_string, line_buf)) == OK) {
      /* ifiGetrequest() does not return a     */
      /*<CR> at the end of the input string.   */

      if (transcript) {
        if (fprintf(transcriptfile, "%s", line_buf) < 0) {
          PrintError(44, NULL, NULL);
          /* not a severe error, continue but turn off transcript mode */
          transcript = 0;
        }
        /* add <CR>. */
        /* we should check fprintf() again for errors, but we don't */
        /* the last one went ok, so assume this one will as well.   */
        fprintf(transcriptfile, "\n");
      }
    }
  }  /* ValidateJson() */
  else {
    /* it's an invalid json string */
    /* DO NOTHING OR SEND AN ERROR JSON BACKT TO FE? */
    /* stay in the loop, we need a user command */
  }

  free(json_string);
  return(result);
}
