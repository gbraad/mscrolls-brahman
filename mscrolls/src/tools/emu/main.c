/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2016-2018 Voidware Ltd.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code as
 * defined in and that are subject to the Voidware Public Source Licence version
 * 1.0 (the 'Licence'). You may not use this file except in compliance with the
 * Licence or with expressly written permission from Voidware.  Please obtain a
 * copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
 * it before using this file.
 *
 * THIS IS NOT FREE SOFTWARE.
 * 
 * The Original Code and all software distributed under the Licence are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
 * OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
 * WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Please see the Licence for the specific language governing rights and 
 * limitations under the Licence.
 *
 * contact@strandgames.com
 *
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It is being released under a Public Source Licence for historical reference
 * and educational purposes.
 */

#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "emu.h"
#include "symbols.h"
#include "errors.h"
#include "version.h"
#include "load.h"
#include "get_text.h"
#include "emulate.h"
#include "display.h"

extern int yyparse();

FILE *exec_file=NULL, *log_file=NULL, *longlogfile=NULL;
int moreflag=0, morecount=0;
int warnings=0, verbose=0, debug=0, quick=0;
int oldFline=0;
int noclones=0;	/* force no clones even if SETNOUNS wants them */
int value;	/* value returned from expr parser */
jmp_buf command;

void interrupt(int);
void catchsig(int);

int main(int argc, char** argv)
{
    int i;
    char *p;

    for (i=1; i<argc; i++)
        if (argv[i][0] == '-')
            for (p=argv[i]+1; *p; p++)
                switch (*p) {
                case 'w' :
                    warnings=1;
                    break;
                case 'v' :
                    verbose=1;
                    break;
                case 'd' :
                    debug=1;
                    break;
                case 'q' :
                    quick=1;
                    break;
                case 'o' :
                    oldFline=1;
                    break;
                case 'c' :
                    noclones=1;
                    break;
                case 'V' :
                    Version();
                }
        else
            break;
    argc -= i-1;
    argv += i-1;

    load(argc>1 ? argv[1] : "prog", !quick);
    get_text(argc>2 ? argv[2] : "simtext");
    Areg[7] = 128000;
    PC = 0;
    init_backend();		/* let the back end initialise itself */
    for (;;) {
        switch (setjmp(command)) {
        case 0 :
        case E_SYM :
            break;	/* no error */
        case E_INTR :
            printf("interrupt!\n");
            break;
        case E_ILL :
            displayregs();
            break;
        }
        if ((int)signal(SIGINT, interrupt)==-1) {	/* catch ^C's */
            perror("signal");
            exit(-1);
        }
        yyparse();	/* parse & execute commands */
    }
}

void interrupt(int s)
{
    longjmp(command, E_INTR);
}

void catchsig(int sig)
{
#if defined(VAX) || defined(C)
    extern long PC, oldPC;
#endif

#ifdef BSD42
    extern char *sys_siglist[];
    printf("%s\n", sys_siglist[sig]);
#endif
        
#ifdef VAX
    /* note - this assumes prog was running when signal happened */
    PC = (int)m68mem_end - oldPC - 1;
#endif
        
#ifdef C
# ifdef BIG_ENDIAN
    PC = oldPC - (long)memory;
# else
    PC = (long)memory_end - oldPC - 1;
# endif
#endif
    signal(SIGINT, interrupt);	/* paranoia */
    longjmp(command,1);
}
