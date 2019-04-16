/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2017 Voidware Ltd.  All Rights Reserved.
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
 * It is being released under a Public Source Licence for posterity and
 * historical reference.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "kem.h"
#include "dis.h"
#include "dis1.h"
#include "aline.h"
#include "get_ea.h"

extern int more_count;
extern int moreflag;

extern struct registers registers;
extern struct flags flags;

extern char *begincode,RestartGame[];

static char *NounDataStart; /* vax pointer to start of noun data */
static u_int NounData68k;  /* vax order 68k addr of noun data */
//static unsigned char *Section24;    /* vax pointer to scenary noun texts */
//static short *Location24;   /* vax pointer to locations of scenary nouns */
//static int Word24=0;        /* word# of next section 24 word to be built */
//static int Pos24=0;         /* offset in to Section24 of next word */

extern FILE *exec_file,*log_file;
extern FILE *longlogfile;
extern int longlogflag;

extern char *PC,*memory;
extern int pc;
char *MessageCode;

#define MAX_MESSAGES 2000
#define MAX_TEXT 1024*512

char Text[MAX_TEXT];          /* adventure text */
char *Index[MAX_MESSAGES];          /* pointers to each message */

static char lastchar=0;

void get_text(char* textname)
{
    FILE *fp;
    int c,i;
    char *pnt;

    fp = fopen(textname,"rb");
    
    if (fp==NULL) 
    {
        printf("Unable to find %s!!\n",textname);
        return;
    }

    pnt = &Text[0];
    i=0;
    Index[i]=pnt;

    while ((c=getc(fp))!=EOF)
    {
        if (c==10) 
        {
            *pnt++ = 0;
            if (i >= MAX_MESSAGES)
            {
                printf("FATAL: too many messages\n");
                break;
            }
            Index[++i]=pnt;
        }
        else
        {
            if (pnt - Text >= MAX_TEXT)
            {
                printf("FATAL: too much text!\n");
                break;
            }
            *pnt++ = c;
        }
    }
    fclose(fp);

    if (verbose) printf("total messages %d (0x%x)\n", i, i);
}

static void charout(char c,int channel,int plural)
{
    static int capital=FALSE;       /* Doug tells me this only done at comp */
    static int punct = FALSE;
    static char wordbuf[80];
    static int charcount=0;
    static int screenpos=0;

    int i; 

    c=c&0x7F;           /* belt and braces part 102 */

    if (c==0x7F)
    {
        capital=TRUE;
        return;         /* only function */
    }
        
    if (c==0)           /* flush */
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

    if (c=='^') c=10;   /* hmmm what do u call an LF that wants to cross the road ? */
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
    if (punct && (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))))
        charout(32,channel,plural);

    if (lastchar=='|')          /* lose |a etc */
    {
        lastchar=c;
        return;
    }
    lastchar=c;
    //  if (c=='|') return;         /* lose |a etc */

    wordbuf[charcount++]=c;
    if ((c==' ') || (c==8) || (c==10) || (charcount>79)) /* hmmm time to flush? */
    {
        if (charcount+screenpos>79)
        {
            putchar('\n');
            if (exec_file == NULL) 
            {
                if (moreflag) more_count++;
                if (more_count==20)
                {
                    int c;
                    more_count=0;
                    printf("<MORE>\n");
                    while ((c=getchar())!=10) ;
                }
            }
            if (longlogfile != NULL) putc('\n',longlogfile);
            screenpos=0;
        }
        for (i=0;i<charcount;i++) 
        {
            putchar(wordbuf[i]);
            if (longlogfile != NULL) putc(wordbuf[i],longlogfile);
            screenpos++;
            if (wordbuf[i]==10)
            {
                screenpos=0;
                if (exec_file == NULL) 
                {
                    if (moreflag) more_count++;
                    if (more_count==20)
                    {
                        int c;
                        more_count=0;
                        printf("<MORE>\n");
                        while ((c=getchar())!=10) ;
                    }
                }
            }
            if (wordbuf[i]==8) screenpos=screenpos-2;
        }
        charcount=0;
    }
}


void do_TEXT()       /* wants a message no. in D0, if C set then half done */
{
    char *pointer,c;
    int message, plural;
    
    static char *oldpointer;

    plural=registers.D[2].chars.c4;
    message=reverse(registers.D[0].shorts.s2);
    if (flags.C==TRUE) pointer=oldpointer;
    else pointer=Index[message];

    //fprintf(stderr, "[[[ printing message %d:\n", message);

    if (!pointer)
    {
        printf("BAD message: number %d (0x%x) does not exist!\n",
               message, message);
        return;
    }
     
    while ((c = *pointer++)!=0) 
    {
        if (!((c=='@') && (*(pointer-2)==' '))) charout(c,0,plural);
        else
        {
            oldpointer=pointer;
            set_C;
            return;
        }
        /*
          if (c=='|')
          {
          int i,count=0;
          unsigned char temp;
          char *p=pointer+1;

          Location24[Word24++]=reverse(message+1);

          while ((c = *p++)!=' ')
          {
          if ((temp=Section24[Pos24])>0x80) 
          {
          count++;
          Section24[Pos24++]=c;
          }
          else Section24[Pos24++]=c;
          }
          Section24[Pos24-1]=Section24[Pos24-1] | 0x80;
          if (temp<0x80) 
          {
          int temppos=Pos24;
          while (Section24[temppos]<0x80) Section24[temppos++]=0;
          Section24[temppos]=0xFF;
          }
          for (i=Word24;i<190;i++) Location24[i]=Location24[i+count];
          }
        */
    }
    clear_C;

    //fprintf(stderr, "]]] printed\n");
}

void do_SETNOUNS()   /* set start of noun data pointer up (called only once) */
{
    srand(time(0));

    // A0 = noundata
    // A6 = clonedata
    // D6 = maxnoun
    // A5 = spacetable
    // D7 = minspaceentry
    // A3 = messagecode

    // A1 = save start
    // D0 = save_len

    MessageCode = vaxof68k(registers.A[3].L);   /* where to go for MSG xy */
    NounData68k = reverse_l(registers.A[0].L);
    NounDataStart = vaxof68k(registers.A[0].L); /* noundata pointer */

    //Section24=(unsigned char *)vaxof68k(registers.A[1].L);  /* wtable section24 */
    //Location24=(short *)vaxof68k(registers.A[2].L);     /* locations of these */
}


/*
  GETDATA -   entry D0.W = noun#
  exit  A0.L = pointer to noundata

*/

void do_GETDATA()
{
    int nounno,nounpointer;
    nounno=0x3FFF & (reverse(registers.D[0].shorts.s2));
    nounpointer=(14*nounno)+NounData68k;
    registers.A[0].L=reverse_l(nounpointer);
}

/*
  WORDSCAN -  finds the Nth word in the dictionary
  words being hi-bit terminated strings (today)

  entry   D2.W = no. of words to scan
  A1.L = points at dictionary

  exit    D2.W = 0
  A1.L = points at end of correct word

*/

void do_WORDSCAN()
{
    int count;
    unsigned char *pos;

    count=reverse(registers.D[2].shorts.s2);
    pos=(unsigned char *)vaxof68k(registers.A[1].L);

    do 
        if ((*pos++)&0x80) count--;
    while (count>0);

    registers.D[2].shorts.s2=0;
    registers.A[1].L=vaxto68k((char*)pos);
}

/* 
   NOUNSCAN -  like WORDSCAN but scans adjective list simultaneously
   adjective list is zero terminated bytes

   Entry:  A0.L    points at word list
   A1.L    points at adjective list
   D0.W    no. of words to skip

   Exit:   A0.L    pointer to end of word
   A1.L    pointer to end of its adjective list
   D0.W    now zero

*/

void do_NOUNSCAN()
{
    int count;
    unsigned char *A0,*A1;

    count=reverse(registers.D[0].shorts.s2);
    A0=(unsigned char *)vaxof68k(registers.A[0].L);
    A1=(unsigned char *)vaxof68k(registers.A[1].L);

    do {
        if ((*A0++)&0x80)   /* find end of word */
        {
            count--;
            for (;(*A1++)!=0;); /* find end of adj. list */
        }
    }
    while (count>0);

    registers.D[0].shorts.s2=0;
    registers.A[0].L=vaxto68k((char*)A0);
    registers.A[1].L=vaxto68k((char*)A1);
}

/* 
   GETNOUNS -  scans noundata for nouns with given attributes

   Entry:  D5.B == 0=> bytewise else wordwise
   D2.B/W      value to match
   D3.W        no. of last noun matched
   D4.W        no. of nouns in table (NOT no. left to do - total!)
   A0.L        pointer to first noun to compare

   Exit:   A0.L        Points to NEXT byte/word to compare
   D3.W        no. of noun that matched
   carry       SET=> match found, else not

*/

void do_GETNOUNS()
{
    int D2,D3,D4;
    char *A0;

    A0=vaxof68k(registers.A[0].L);
    D3=reverse(registers.D[3].shorts.s2);
    D4=reverse(registers.D[4].shorts.s2);

    clear_C;

    if (registers.D[5].chars.c4==0x00) /* bytewise */
    {
        D2=registers.D[2].chars.c4;
        do {
            D3++;
            if (D2 == *A0)
            {
                set_C;
                break;
            }
            A0=A0+14;
        }
        while (D4>D3);
    }
    else
    {
        D2=registers.D[2].shorts.s2;
        do {
            D3++;
            if (D2 == *(u_short *)A0)
            {
                set_C;
                break;
            }
            A0=A0+14;
        }
        while (D4>D3);
    }
    registers.A[0].L=vaxto68k(A0+14);
    registers.D[3].shorts.s2=reverse(D3);
}

void do_CHAROUT()
{
    char* A7;
    A7 = vaxof68k(registers.A[7].L);

    // push PC
    u_int* p = (u_int*)A7 - 1;
    *p = vaxto68k(PC);
    registers.A[7].L = vaxto68k((char*)p);

    // jump to MessageCode (in message.asm)
    PC = MessageCode;

    /*
      char c;
      c=(registers.D[1].chars.c4)&0x7F;
      putchar(c);
      fflush(stdout);
    */
}

void do_CHAROUT2()
{
    int channel,plural;
    char c;

    c=registers.D[1].chars.c4;
    channel=registers.D[3].chars.c4;
    plural=registers.D[2].chars.c4;

    charout(c,channel,plural);

}


/* save/load params A0-> filename A1->start of data D1.W = length */

void do_LOADFILE()
{
    int fd,size;
    char *name=vaxof68k(registers.A[0].L);

    registers.D[7].L = 0;

    if ((fd=simopen(name, 0)) == -1)
    {
        printf("\nUnable to open %s!\n",name);
        registers.D[7].L = -1;
        return;
    }
    else
    {
        size=reverse(registers.D[1].shorts.s2);
        if (size!=read(fd,vaxof68k(registers.A[1].L),size))
            printf("\nHmmmm - iffy size\n");
        close(fd);
    }
}

void do_SAVEFILE()
{
    int fd,size;
    char *name=vaxof68k(registers.A[0].L);

    if ((fd=creat(name, 0644)) == -1)
    {
        printf("\nUnable to open %s!\n",name);
        registers.D[7].L = -1;
        return;
    }
    else
    {
        size=reverse(registers.D[1].shorts.s2);
        if (size!=write(fd,vaxof68k(registers.A[1].L),size))
            printf("\nHmmmm - iffy size\n");
        close(fd);
    }
}

void do_RANDOM1()
{
    registers.D[0].chars.c4=(unsigned char)rand();
}

void do_RANDOM2()
{
    int rnd,param,result;

    param=reverse_l(registers.D[1].L)&0xFF;
    rnd=rand() & 0xFFFF;
    result=((param*rnd)>>16)&0xFF;

    registers.D[1].L=reverse_l(result);

}

void do_CHARIN()
{
    char c;

    more_count=0;
    fflush(stdout);
    if (exec_file==NULL)
    {
        c=getchar();
        if (log_file!=NULL) putc(c,log_file);
        if (longlogfile!=NULL) putc(c,longlogfile);
    }
    else
    {
        if ((c=getc(exec_file))==EOF)
        {
            fclose(exec_file);
            exec_file=NULL;
            printf("\n\nEnd of exec file!\n\n");
            c=getchar();
        }
        else 
        {
            if (c=='#')
            {
                do {
                    putchar(c);
                    c=getc(exec_file);
                }
                while ((c!=EOF) && (c!='\n'));
                putchar('\n');
            }
            else putchar(c);
        }
    }
    if (c==9) c=' ';
    if ((c==';') && (exec_file==NULL))
    {
        while ((c=getchar()) != '\n')
        {
            if (log_file!=NULL) putc(c,log_file);
            if (longlogfile!=NULL) putc(c,longlogfile);
        }
        if (log_file!=NULL) putc(c,log_file);
        if (longlogfile!=NULL) putc(c,longlogfile);
    }
            
    registers.D[1].L=0;
    registers.D[1].chars.c4=c;
    lastchar=10;                    /* so charout loses first lf */
    if (c==10) charout(0,0,0);  /* tell charout where cursor is */

}

void do_QCARRIED()
{
    int noun,finito,i;
    unsigned char *pnt;
    unsigned short loc;
    
    noun=0x7FFF & (reverse(registers.D[0].shorts.s2));


    for (i=noun;i>0;i--)
    {
        set_Z;
        finito=FALSE;
        noun=i;
        do {
            pnt=(unsigned char *)((14*noun)+NounDataStart);
            if (*(pnt+6) & 0x80) 
            {
                clear_Z;            /* out */
                finito=TRUE;
            }
            else
            {
                if ((*(pnt+5)&0x01) != 0)       /* pseudo */
                {
                    clear_Z;
                    finito=TRUE;
                }
                else
                {
                    loc = (u_short)((*(pnt+8))<<8) | (u_short)(*(pnt+9));
                    if (loc==noun) 
                    {
                        clear_Z;
                        break;
                    }
                    if (*(pnt+6) == 0)
                    {
                        if (loc!=0) clear_Z;    /* non pointed, loc != 0 ie not on inv */
                        finito=TRUE;
                    }
                    else
                    {
                        if ((*(pnt+6)&0xCC) != 0)   /* not on invent is D0 player ? */
                        {
                            if (noun!=0) clear_Z;
                            finito=TRUE;
                        }
                        else
                        {
                            noun=loc;   /* pointed noun, next one up... */
                        }
                    }
                }
            }
        }
        while (finito!=TRUE);
        if (flags.Z==TRUE) break;
    }
    if (i==0) i=1;
    registers.D[0].shorts.s2=reverse(i);
}


/*  A6 points at string to identify (inbuff)
    A1 points at ADJBUF
    A2 points at start of (empty) matchbuf
    A0 ADJ.TBL
    A3 TBL.WRD  (pos of begining of nouns' section sometimes)
    A2 points at MATCHBUF

    D6 is the section no.

    On exit:
    D0 is TYPE
    D1 is ADJFLG
    A5 needs to be maintained for God knows what reason
    A6 is A5+1 
    A2 points at the word AFTER the last one put into MATCHBUF

*/


void do_SEARCH()
{

    /* cos 0x20 & 0x5F == 0 !! */
#define SearchSPACE 0

    short *matchbuf,*newmatchbuf,*oldmatchbuf,*thirdmatchbuf,*adjbuffer;
    short word,section;
    unsigned char *A6,*A3,*A5,type0,type1;
    unsigned char *adjtable;
    int count,adjflag,currentnoun,no_matched=0;
    unsigned char c1,c2;
    int longest_match=0,islong;     /* flags for in_the_boat */
    int oneislong=0;

    short *syntable=(short *)vaxof68k(registers.A[5].L);

    registers.A[5].L=registers.A[6].L;      /* hmmm A5  wants to be A6 */

    A6=(unsigned char *)vaxof68k(registers.A[6].L);       /* string to match */
    adjtable=(unsigned char *)vaxof68k(registers.A[0].L);
    adjbuffer=(short *)vaxof68k(registers.A[1].L);
    matchbuf=(short *)vaxof68k(registers.A[2].L);
    newmatchbuf=matchbuf;
    oldmatchbuf=matchbuf;
    thirdmatchbuf=matchbuf; /* 3rd pass to fod non-long */
    A3=(unsigned char *)vaxof68k(registers.A[3].L);       /* word table */

    type0=0;
    type1=0;
    adjflag=0;
    currentnoun=0;  /* to keep track of where in the adj list we are */

    word=0;
    section=reverse(registers.D[6].shorts.s2);
    count=0;    /* no. of chars matched */
    A5=A6;      /* start of word to match */
    islong=0;
    do {
        c1= *A3++;
        if (c1==0x81) break;
        if (c1==0x82) 
        {
            word=0;
            section++;
            count=0;
            A6=A5;
            islong=0;
        }
        else
        {
            c2=(*A6++) &0x5F;
            if ((c2==SearchSPACE) && (c1=='_'))
            {
                c2='_';
                islong=0x8000;  /* flag so as we know a 'long match' found */
            }
            if ( ( ((c1>0x80) && ((c1&0x5F)==c2) ) || (count>99)) 
                 && ((*A6==0)||(*A6==' ')||(*A6=='\'')) )
            {
                if (*A6=='\'') 
                {
                    type0=0x02;
                    type1= *(A6+1);
                }
                *matchbuf++ = reverse(section+islong);
                *matchbuf++ = reverse(word);
                count=0;
                if ( (A6-A5) > longest_match ) longest_match= A6-A5;
                A6=A5;
                islong=0;
                word++;
                while (c1<0x80) c1= *A3++;  /*  find end of word in WTABLE
                                                in case we didn't hit end */
            }
            else
            {
                if (c1>0x80) { word++; count=0; A6=A5; islong=0; }
                else
                {
                    c1=c1 & 0x5F;
                    if (c1==c2)
                        count++;
                    else
                    {
                        count=0;
                        A6=A5;
                        islong=0;
                        while (((*A3++)&0x80)==0);
                        word++;
                    }
                }
            }
        }
    }
    while (*A3 != 0x81);

    /* now resolve the adjectives for any section 6 word (nouns) */
    /* yea, so adjectives don't work for synonyms... */

    while (matchbuf!=oldmatchbuf)
    {
        adjflag=0;
        section = reverse(*oldmatchbuf++);
        islong = section & 0x8000;
        section = section & 0x7fff;
        word = reverse(*oldmatchbuf++);
        if (section==11)
        {
            section=reverse(syntable[word]) & 0x1F;
            word=(reverse(syntable[word])>>5) & 0x07FF;
        }
        if (section!=6)
        {
            *newmatchbuf++ = reverse(section+islong);
            if (islong!=0) oneislong=1;
            *newmatchbuf++ = reverse(word);
            no_matched++;
        }
        else
        {
            unsigned char *adjlist;

            if (*adjbuffer==0)
            {
                *newmatchbuf++ = reverse(section+islong);
                if (islong!=0) oneislong=1;
                *newmatchbuf++ = reverse(word);
                no_matched++;
            }
            else
            {
                int given;
                unsigned char adj;
                short *myadjbuffer;
                unsigned char *myadjtable;
                myadjtable=adjtable;
                myadjbuffer=adjbuffer;
                for (currentnoun=0;currentnoun<word;currentnoun++) while (*myadjtable++ != 0);

                while ((given = reverse(*myadjbuffer++))!=0)
                {
                    adjlist=myadjtable;
                    for (;;)
                    {
                        if ((adj = *adjlist++)==0) 
                        {
                            adjflag=1;
                            break;
                        }
                        if ((adj-3) == (unsigned char)given) break;
                    }
                }
                if (adjflag==0)
                {
                    *newmatchbuf++ = reverse(section+islong);
                    if (islong!=0) oneislong=1;
                    *newmatchbuf++ = reverse(word);
                    no_matched++;
                }
            }
        }
    }

    /* if there were any 'long' matches (ie. with '_' in them) then kill the
       others */

    if (oneislong!=0)
    {
        no_matched=0;
        matchbuf=thirdmatchbuf;
        while (newmatchbuf!=thirdmatchbuf)
        {
            section=reverse(*thirdmatchbuf++);
            word=reverse(*thirdmatchbuf++);
            if (section & 0x8000)
            {
                *matchbuf++ = reverse(section & 0x7FFF);
                *matchbuf++ = reverse(word);
                no_matched++;
            }
        }
        newmatchbuf = matchbuf;
        A6=A5+longest_match;
    }
    else A6=A5+1;

    /*  if (no_matched!=0) adjflag=0; */

    registers.A[6].L=vaxto68k((char*)A6);
    registers.A[2].L=vaxto68k((char*)newmatchbuf);
    registers.D[0].chars.c3=type0;  /* set up TYPE return */
    registers.D[0].chars.c4=type1;
    registers.D[1].shorts.s2=reverse(adjflag);
}

/*
  GETTIED -   finds the next lowest noun which has (12(A0) & 0x3FFF)
  or == D4!
                
  On entry -  D2.W first noun to look at (NOT decremented first)
  D4.W special case - actually the noun that we are trying to
  suss what's tied to it

  On exit -   D2.W noun no. of tied noun (may be equal to D4)
  CARRY - set => noun was found, else no tied nouns left
*/

void do_GETTIED()
{
    int nounno,D4;
    char *nounpointer;

    clear_C;
    nounno=0x7FFF & (reverse(registers.D[2].shorts.s2));
    D4=0x7FFF & (reverse(registers.D[4].shorts.s2));
    nounpointer=(14*nounno)+NounDataStart+12;
    
    while (nounno!=0)
    {
        if ( ((*nounpointer)&0x3F) || (*(nounpointer+1)) || (nounno==D4) )
        {
            set_C;
            break;
        }
        nounno--;
        nounpointer=nounpointer-14;
    }
    if (nounno==0) nounno=1;        /* don't return 0 */
    
    registers.D[2].shorts.s2=reverse(nounno);
}

/* 
   FINDZERO -  finds the Nth zero in a list

   On entry    A1.L points at start of list
   D0.W is the no. of zeros (-1) to find.

   On exit     A1.L points at byte after Nth zero
   D0.W = -1 (optional)
*/

void do_FINDZERO()
{
    char *A1;
    int D0;

    D0=reverse(registers.D[0].shorts.s2);
    A1=vaxof68k(registers.A[1].L);

    while (D0-- != 0) while (*A1++ != 0) ;

    registers.D[0].shorts.s2 = -1;
    registers.A[1].L=vaxto68k(A1);

}

void do_GRAPHICS()
{
    int D0,D1;
    D0=reverse(registers.D[0].shorts.s2);
    D1=reverse(registers.D[1].shorts.s2);
    printf("\nEeek a graphics trap!!\nr0 = %04x\nr1 = %04x\n",D0,D1);

}

void do_PRINTER()
{
    printf("Eeek a printer trap!!\n");
}

void do_RESTART()
{
    PC = memory;
    pc = 0;
    registers.A[7].L = reverse_l(reverse_l(registers.A[7].L)+4);
    memcpy(begincode, RestartGame, 32000);
}

void do_QUIT()
{
    printf("Eeek a QUIT trap!!\n");
}

void do_MAKE_DIFF()
{
    //printf("trap MAKE_DIFF\n");
}

void do_UNDO_DIFF()
{
    //printf("trap UNDO_DIFF\n");    
}
