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
#include "emu.h"
#include "charinout.h"

#define TRUE	1
#define FALSE	0

static int lastchar=0;
static int quoteflag=0;

extern FILE *log_file, *exec_file, *longlogfile;
extern int moreflag, morecount;
int troff;
int tex;
extern int inscount;

char ttstring[]="%\n\\bf ";		/* string to go into courier */
char rmstring[]="%\n\\rm ";		/* string to go into roman */
char lbstring[]="{\\par\\noindent}";	/* line break string */

void charout(int c, int channel, int plural)
{
    static int capital=FALSE;		/* Doug tells me this only done at comp */
    static int punct = FALSE;
    static char wordbuf[80];
    static int charcount=0;
    static int screenpos=0;
    //static int oldx=0;
    //static int oldy=0;
    
    int i;	
    
    c=c&0x7F;			/* belt and braces part 102 */
    
    if (c==0x7F)
	{
	    capital=TRUE;
	    return;			/* only function */
	}
    
    if (c==0)			/* flush */
	{
	    screenpos=0;
	    return;
	}
    
    /* bits that may apply to top line stuff one day 
       
       if ((channel==0) && ((oldx!=0) || (oldy!=0)))
       {
       move(oldy,oldx);
       oldx=0;
       oldy=0;
       }
       
       if (channel!=0)
       {
       if ((oldx==0) && (oldy==0))
       {
       getyx(stdscr,oldy,oldx);
       move(0,0);
       }
       if (c==9)
       {
       clrtoeol();
       move(0,68);
       }
       else if (c!=10) addch(c);
       return;
       }
       */
    
    if (channel!=0) return;
    
    if (c=='^') c=10;	/* hmmm what do u call an LF that wants to cross the road ? */
    if (c=='@')
	{
	    if (plural) c=0;
	    else c='s';
	}
    
    /* fod case if necessary */
    
    if (capital==TRUE)
	{
	    if ((c>0x60) && (c<0x7C)) 
		{
		    c=c-0x20;
		    capital=FALSE;
		}
	    else
		if (((c>0x40) && (c<0x5C)) || (c=='\'') || (c=='"') ) capital = FALSE;
	}
    
    if ((c=='~') || (c=='!') || (c=='.') || (c=='?') || (c==10)) capital=TRUE;
    
    if (c==13) c=10;
    
    if ( ((c==10) || (c==' ')) && (c==lastchar) ) c=0;
    
    if ( (c=='~') && (lastchar!=10) )
	{
	    charout(10,channel,plural);
	    c=10;
	}
    if ( (c=='~') && (lastchar==10) ) c=10;
    
    if (c=='_') c=' ';
    
    if (c==0) return;
    
    if ((lastchar==',') || (lastchar=='.') ||
	(lastchar=='!') || (lastchar=='?')) punct=TRUE;
    else punct=FALSE;
    if (punct && (islower(c) || isupper(c)))
	charout(32,channel,plural);
    
    if (lastchar=='|') 			/* lose |a etc */
	{
	    lastchar=c;
	    return;
	}
    lastchar=c;
    //	if (c=='|') return;			/* lose |a etc */
    
    wordbuf[charcount++]=c;
    if ((c==' ') ||
	(c==8) ||
	(c==10) ||
	(charcount>78)) /* hmmm time to flush? */
	{
	    if (charcount+screenpos>78)
		{
		    if (wordbuf[0] != '\n')
			putchar('\n');
		    if (exec_file == NULL) {
			if (moreflag) morecount++;
			if (morecount==20)
			    {
				int c;
				morecount=0;
				printf("<MORE>\n");
				while ((c=getchar())!=10) ;
			    }
		    }
		    if (longlogfile != NULL) putc('\n',longlogfile);
		    screenpos=0;
		    lastchar=10;
		}
	    for (i=0;i<charcount;i++) 
		{
		    putchar(wordbuf[i]);
		    lastchar=wordbuf[i];
		    if (longlogfile != NULL) putinlonglog(wordbuf[i]);
		    screenpos++;
		    if (wordbuf[i]==10) {
			screenpos=0;
			lastchar=10;
			if (exec_file == NULL) {
			    if (moreflag) morecount++;
			    if (morecount==20) {
				int c;
				morecount=0;
				printf("<MORE>\n");
				while ((c=getchar())!=10) ;
			    }
			}
		    }
		    if (wordbuf[i]==8) {
			screenpos=screenpos-2;
			if (longlogfile) fseek(longlogfile, -2L, 1);
		    }
		}
	    charcount=0;

	    if (longlogfile && tex)
		if (c == 10) {
		    if (screenpos != 0) putc('\n', longlogfile);
		    fprintf(longlogfile, lbstring); /* TeX end of para command */
		}

	    if (longlogfile && troff)
		if (c == 10) {
		    if (screenpos != 0) putc('\n', longlogfile);
		    fprintf(longlogfile, ".br\n"); /* troff line break command */
		}
	}
}

static int read_char();

int charin()
{
    char c;
    static int command_start=1;
    
    morecount=0;
    c = read_char();
    if (log_file!=NULL) putc(c,log_file);
    if (longlogfile!=NULL) {
	if (tex) {
		quoteflag=0;			/* so next will be `` (open) */
	    if (command_start) {
		command_start=0;
		fprintf(longlogfile, ttstring);	/* \tt => 'typewriter font' */
	    }
	    if (c == 10)
			{
			fprintf(longlogfile, rmstring); /* roman */
			quoteflag=0;			/* so next will be `` (open) */
			}
	}

	if (troff) {
	    if (command_start) {
		command_start=0;
		fprintf(longlogfile, "\\f7");
	    }
	    if (c == 10) fprintf(longlogfile, "\\f1");
	}
	putc(c,longlogfile);
    }
    fflush(stdout);
    if (c==9) {
	printf("\nInstruction Count = $%08x\n", inscount);
	c=' ';
    }
    if (c == ';') {
	while ((c=read_char()) != '\n') {
	    if (log_file!=NULL) putc(c,log_file);
	    if (longlogfile!=NULL) putc(c,longlogfile);
	}
	if (log_file!=NULL) putc(c,log_file);
	if (longlogfile!=NULL) {
	    if (tex) {
		fprintf(longlogfile, rmstring);
		command_start = 1;
	    }
	    if (troff) {
		fprintf(longlogfile, "\\f1");
		command_start = 1;
	    }
	    putc(c,longlogfile);
	}
	if (longlogfile && tex)
	    fprintf(longlogfile, lbstring);
	if (longlogfile && troff)
	    fprintf(longlogfile, ".br\n");
    }
    
    lastchar=10;		/* so charout loses first lf */
    if (c==10) {
	charout(0,0,0); 	/* tell charout where cursor is */
	if (longlogfile && tex) fprintf(longlogfile, lbstring);
	if (longlogfile && troff) fprintf(longlogfile, ".br\n");
	command_start = 1;
    }
    return c;
}

static int read_char()
{
    int c;
    
    if (exec_file==NULL) {
	c=getchar();
	if (c == -1) {		/* EOF i.e. ^D pressed ? */
	    c = 4;
	    clearerr(stdin);
	}
    } else {
	if ((c=getc(exec_file))==EOF) {
	    fclose(exec_file);
	    exec_file=NULL;
	    printf("\n\nEnd of exec file!\n\n");
	    c=getchar();
	} else {
	    putchar(c);
	}
    }
    return c;
}

void putinlonglog(int c)		/* filter for tex & troff */
{
	if (tex)
		{
		switch (c) {
			case '>':
			case '<':
				fprintf(longlogfile, "$%c$",c);
				break;
			case '&':
			case '%':
			case '#':
			case '$':
				fprintf(longlogfile, "\\%c",c);
				break;
			case '"':
				if (quoteflag) fprintf(longlogfile, "''");
				else fprintf(longlogfile, "``");
				quoteflag = !quoteflag;
				break;
			default:
				putc(c,longlogfile);
				break;
			}
		}
	else putc(c,longlogfile);
}

void getline_ev()
{
	unsigned char c, buffer[256];
	unsigned char *p;
	int i, j;

	i = 0;
	while ((c=charin())!=10) buffer[i++] = c;

	p = memory_end - Areg[1];	/* 'tis a char pointer */
	for (j=0; j<i; j++) *p-- = buffer[j];

	*p=0;
	Areg[1] += i+1;
	Dreg[1] = 0;	/* ADV_EV_GETLINE */
}
