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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "emuc.h"
#include "aline.h"
#include "charinout.h"
#include "load.h"
#include "utils.h"
#include "get_text.h"

#define TRUE	1
#define FALSE	0

E_ad noundata_E_ad;
C_ad noundata_C_ad;
UWORD *messagecode;
C_ad clonedata;
int maxnoun;
int minspaceentry;
int spacetable;
C_ad spacetable_C_ad;

extern void (*aline_table[])();

void ALINE(int ins)
{
    void CHARIN();
    if (ins & 255)
        (*aline_table[255 - (ins & 255)])();
    else
        CHARIN();
}


#ifndef STANDALONE
/* wants a message no. in D0, if C set then half done */
void TEXT()
{
    char *pointer,c;
    int message,plural,channel;
    static char *oldpointer;

    plural=Dreg[2].ub;
    channel=Dreg[3].ub;
    message=Dreg[0].uw;
    if (SR & C_bit) pointer=oldpointer;
    else pointer=textindexes[message];
	 
    while ((c = *pointer++)!=0) {
        if (!((c==' ') && (*(pointer)=='@')))
            charout(c,channel,plural);
        else {
            oldpointer=pointer+2;
            SR |= C_bit;
            return;
        }
    }
    SR &= ~C_bit;
}
#endif

#ifdef MULTIFRONT
extern char *save_start;
extern char *stack_start;
extern int save_len, stack_len;
#endif

void SETNOUNS()
{
#ifdef BSD42
#ifdef STANDALONE
    srandom(getpid());
#else
    srandom(0);		/* makes debugging easier */
#endif
#else
#ifdef STANDALONE
    srand(getpid()>>4);
#else
    srand(0);
#endif
#endif

    messagecode = (UWORD *)E_to_C(SIZE_word, Areg[3].l);	/* where to go for MSG xy */
    noundata_E_ad = Areg[0].l;
    noundata_C_ad = E_to_C(SIZE_byte, Areg[0].l);
    clonedata = E_to_C(SIZE_word, Areg[6].l);
    maxnoun = Dreg[6].l;
    spacetable = Areg[5].l;
    spacetable_C_ad = E_to_C(SIZE_word, spacetable);
    minspaceentry = Dreg[7].l;

#ifdef MULTIFRONT
    save_start = E_to_C(SIZE_byte, Areg[1].l);	/* start of save game bit */
    save_len = Dref[0].l;
    stack_start = E_to_C(SIZE_byte, Areg[7].l - 1024 */
                         stack_len = 1024;
#endif
                         
                         }


        /*
          GETDATA -	entry D0.W = noun#
          exit  A0.L = pointer to noundata

        */

void GETDATA()
        {
            int nounno;
            nounno = 0x3FFF & Dreg[0].uw;
            if (maxnoun!=0 && nounno>maxnoun)
		nounno=read_uw(clonedata ADD 2*(nounno-maxnoun-1));
            Areg[0].l = (14*nounno)+noundata_E_ad;
        }

    /*
      WORDSCAN -	finds the Nth word in the dictionary
      words being hi-bit terminated strings (today)

      entry	D2.W = no. of words to scan
      A1.L = points at dictionary

      exit	D2.W = 0
      A1.L = points at end of correct word

    */

    void WORDSCAN()
    {
        int count;
        C_ad pos;

	count=Dreg[2].uw & 0x3fff;
	if (wtable_loaded)
            pos = W_to_C(Areg[1].l);
	else
            pos = E_to_C(SIZE_byte, Areg[1].l);

	do 
            if (read_ub(INC(pos)) & 0x80) count--;
	while (count>0);

	Dreg[2].uw=0;
	if (wtable_loaded)
            Areg[1].l = C_to_W(pos);
	else
            Areg[1].l = C_to_E(SIZE_byte, pos);
    }

    /* 
       NOUNSCAN -	like WORDSCAN but scans adjective list simultaneously
       adjective list is zero terminated bytes

       Entry:	A0.L	points at word list
       A1.L	points at adjective list
       D0.W	no. of words to skip

       Exit:	A0.L	pointer to end of word
       A1.L	pointer to end of its adjective list
       D0.W	now zero

    */

    void NOUNSCAN()
    {
        int count;
        C_ad A0, A1;

	count = Dreg[0].uw & 0x3fff;
	if (wtable_loaded)
            A0 = W_to_C(Areg[0].l);
	else
            A0 = E_to_C(SIZE_byte, Areg[0].l);
	A1 = E_to_C(SIZE_byte, Areg[1].l);

	do {
            if (read_ub(INC(A0)) & 0x80) {	/* find end of word */
                count--;
                while (read_ub(INC(A1)) != 0) ; /* find end of adj. list */
            }
	} while (count>0);

	Dreg[0].uw = 0;
	if (wtable_loaded)
            Areg[0].l = C_to_W(A0);
	else
            Areg[0].l = C_to_E(SIZE_byte, A0);
	Areg[1].l = C_to_E(SIZE_byte, A1);
    }

    /* 
       GETNOUNS -	scans noundata for nouns with given attributes

       Entry:	D5.B == 0=> bytewise else wordwise
       D2.B/W		value to match
       D3.W		no. of last noun matched
       D4.W		no. of nouns in table (NOT no. left to do - total!)
       A0.L		pointer to first noun to compare

       Exit:	A0.L		Points to NEXT byte/word to compare
       D3.W		no. of noun that matched
       carry		SET=> match found, else not

    */

    void GETNOUNS()
    {
        int D2, D3, D4;
        C_ad A0;

	D3 = Dreg[3].uw;
	D4 = Dreg[4].uw;

	SR &= ~C_bit;

	if (Dreg[5].ub == 0) {	/* bytewise */
            A0 = E_to_C(SIZE_byte, Areg[0].l);
            D2 = Dreg[2].ub;
            do {
                D3++;
                if (D2 == read_ub(A0)) {
                    SR |= C_bit;
                    break;
                }
                A0 = A0 ADD 14;
            } while (D4 > D3);
            Areg[0].l = C_to_E(SIZE_byte, A0 ADD 14);
	} else {
            A0 = E_to_C(SIZE_word, Areg[0].l);
            D2 = Dreg[2].uw;
            do {
                D3++;
                if (D2 == (read_uw(A0) & 0x3FFF)) {
                    SR |= C_bit;
                    break;
                }
                A0 = A0 ADD 14;
            } while (D4>D3);
            Areg[0].l = C_to_E(SIZE_word, A0 ADD 14);
	}
	Dreg[3].uw = D3;
    }

    void CHAROUT()
    {
	charout(Dreg[1].ub,Dreg[3].ub,Dreg[2].ub);
    }

    /* save/load params A0-> filename A1->start of data D1.W = length */

    void SAVEFILE()
    {
	int size;
        FILE *fp;
	char name[256];
        char *p;
        C_ad q;

	Dreg[7].l = 0;
	/* copy the name into the buffer */
	p = name;
	q = E_to_C(SIZE_byte, Areg[0].l);
	for (; *q; INC(q)) *p++ = read_ub(q);
	*p = 0;

	if ((fp=fopen(name, "w")) == NULL) {
            perror(name);
            Dreg[7].l = -1;
            return;
	}
	size = Dreg[1].uw;
	q = E_to_C(SIZE_byte, Areg[1].l);
	while (size--) putc(read_ub(INC(q)), fp);
	fclose(fp);
    }

    void LOADFILE()
    {
	int size;
        FILE *fp;
	char name[256];
        char *p;
        C_ad q;

	Dreg[7].l = 0;
	/* copy the name into the buffer */
	p = name;
	q = E_to_C(SIZE_byte, Areg[0].l);
	for (; *q; INC(q)) *p++ = read_ub(q);
	*p = 0;

	if ((fp=fopen(name, "r")) == NULL) {
            perror(name);
            Dreg[7].l = -1;
            return;
	}
	size = Dreg[1].uw;
	q = E_to_C(SIZE_byte, Areg[1].l);
	while (size--) write_ub(getc(fp), INC(q));
	fclose(fp);
    }

    void RANDOM1()
    {
#ifdef BSD42
	Dreg[0].ub = random();
#else
	Dreg[0].ub = rand();
#endif
    }


    void RANDOM2()
    {
        int rnd,param,result;

	param = Dreg[1].ub;
#ifdef BSD42
	rnd=random() & 0xFFFF;
#else
	rnd=rand() & 0xFFFF;
#endif
	result=((param*rnd)>>16)&0xFF;
	Dreg[1].l = result;
    }

    void CHARIN()
    {
	Dreg[1].l = 0;
	Dreg[1].ub = charin();
    }

    void QCARRIED()
    {
        int noun,finito,i;
        C_ad ptr;
        UWORD loc;
	
	noun = 0x7FFF & Dreg[0].uw;
	for (i=noun;i>0;i--) {
            SR |= Z_bit;
            finito=FALSE;
            noun=i;
            do {
                ptr = noundata_C_ad ADD noun*14;
                if (read_ub(ptr ADD 6) & 0x80) {
                    SR &= ~Z_bit;			/* out */
                    finito=TRUE;
                } else  if ((read_ub(ptr ADD 5) & 1) != 0) {		/* pseudo */
                    SR &= ~Z_bit;
                    finito=TRUE;
                } else {
#ifdef BIG_ENDIAN
                    loc = read_uw(ptr + 8);
#else
                    loc = read_uw(ptr - 9);	/* adjust to a word ptr */
#endif
                    if (loc == noun) {
                        SR &= ~Z_bit;
                        break;
                    }
                    if (read_ub(ptr ADD 6) == 0) {
                        if (loc!=0) SR &= ~Z_bit;	/* simply in a room (not inv) */
                        finito=TRUE;
                    } else if ((read_ub(ptr ADD 6) & 0xCC) != 0) {
                        /* not on inventory unless noun is player */
                        if (noun != 0) SR &= ~Z_bit;
                        finito=TRUE;
                    } else {
                        noun=loc;	/* next one up... */
                    }
                }
            } while (!finito);
            if (SR & Z_bit) break;
	}
	if (i==0) i=1;
	Dreg[0].uw = i;
    }


    /*	A6 points at string to identify (inbuff)
	A1 points at ADJBUF
	A2 points at start of (empty) matchbuf
	A0 ADJ.TBL
	A3 TBL.WRD	(pos of begining of nouns' section sometimes)
	A2 points at MATCHBUF

	D6 is the section no.

	On exit:
        D0 is TYPE
        D1 is ADJFLG
        A5 needs to be maintained for God knows what reason
        A6 is A5+1 
        A2 points at the word AFTER the last one put into MATCHBUF

    */

    void SEARCH()
    {
	C_ad matchbuf, newmatchbuf, oldmatchbuf, thirdmatchbuf, adjbuffer;
	C_ad syntable;
	C_ad A6, A3, A5;
	C_ad adjtable;
	UBYTE type0,type1;
	UBYTE c1, c2;
	int t;
	 int count,adjflag,currentnoun,no_matched;
	 int word, section;
	int longest_match=0, islong;		/* flags for in_the_boat */
	int oneislong=0;

	if (wtable_loaded) {
            syntable = W_to_C(Areg[5].l);
#ifndef BIG_ENDIAN
            syntable--;
#endif
            A3 = W_to_C(Areg[3].l);		/* word table */
	} else {
            syntable = E_to_C(SIZE_word, Areg[5].l);
            A3 = E_to_C(SIZE_byte, Areg[3].l);
	}

	Areg[5].l=Areg[6].l;		/* hmmm A5  wants to be A6 */

	A6=E_to_C(SIZE_byte,Areg[6].l);		/* string to match */
	adjtable=E_to_C(SIZE_byte, Areg[0].l);
	adjbuffer=E_to_C(SIZE_word, Areg[1].l);
	matchbuf=E_to_C(SIZE_word, Areg[2].l);
	newmatchbuf=matchbuf;
	oldmatchbuf=matchbuf;
	thirdmatchbuf=matchbuf;	/* 3rd pass to fod non-long */

	type0=0;
	type1=0;
	adjflag=0;
	currentnoun=0;	/* to keep track of where in the adj list we are */

	word=0;
	section=Dreg[6].uw;
	count=0;	/* no. of chars matched */
	A5=A6;		/* start of word to match */
	islong=0;
	do {
            c1= read_ub(INC(A3));
            if (c1==0x81) break;
            if (c1==0x82) {
                word=0;
                section++;
                count=0;
                A6=A5;
                islong=0;
            } else {
                c2=read_ub(INC(A6)) & 0x5F;
                if (c2==0 && c1=='_') {	/* note: ' ' & 0x5f = 0 */
                    c2='_';
                    islong=0x8000;	/* flag so as we know a 'long match' found */
                }
                if (c2==0) c2 = 0xFF;
                t = read_ub(A6);
                if ( ( (c1>0x80 && (c1 & 0x5F)==c2 ) || count>99 ) &&
                     (t == 0 || t == ' ' || t == '\'' ) ) {
                    if (t == '\'') {
                        type0 = 0x02;
                        type1 = read_ub(A6 ADD 1);
                    }
                    write_uw(section+islong, matchbuf);
                    matchbuf = matchbuf ADD 2;
                    write_uw(word, matchbuf);
                    matchbuf = matchbuf ADD 2;
                    count=0;
                    if ( DIFF(A6,A5) > longest_match )
                        longest_match= DIFF(A6,A5);
                    A6=A5;
                    islong=0;
                    word++;
                    while (c1<0x80) c1= read_ub(INC(A3));	/* find end of word */
                } else {
                    if (c1>0x80) { word++; count=0; A6=A5; islong=0; }
                    else {
                        c1=c1 & 0x5F;
                        if (c1==c2)
                            count++;
                        else {
                            count=0;
                            A6=A5;
                            islong=0;
                            while (!(read_ub(INC(A3)) & 0x80)) ;
                            word++;
                        }
                    }
                }
            }
	} while (read_ub(A3) != 0x81);

        /* now resolve the adjectives for any section 6 word (nouns) */
        /* yea, so adjectives don't work for synonyms... */

	while (matchbuf!=oldmatchbuf) {
            adjflag=0;
            section = read_uw(oldmatchbuf);
            oldmatchbuf = oldmatchbuf ADD 2;
            islong = section & 0x8000;
            section = section & 0x7fff;
            word = read_uw(oldmatchbuf);
            oldmatchbuf = oldmatchbuf ADD 2;
            if (section==11) {
                section = read_uw(syntable ADD 2*word) & 0x1f;
                word = read_uw(syntable ADD 2*word)>>5 & 0x7ff;
            }
            if (section!=6) {
                write_uw(section + islong, newmatchbuf);
                newmatchbuf = newmatchbuf ADD 2;
                if (islong!=0) oneislong=1;
                write_uw(word, newmatchbuf);
                newmatchbuf = newmatchbuf ADD 2;
                no_matched++;
            } else {
                C_ad adjlist;

                if (read_uw(adjbuffer) == 0) {
                    write_uw(section + islong, newmatchbuf);
                    newmatchbuf = newmatchbuf ADD 2;
                    if (islong!=0) oneislong=1;
                    write_uw(word, newmatchbuf);
                    newmatchbuf = newmatchbuf ADD 2;
                    no_matched++;
                } else {
                    UWORD given;
                    UBYTE adj;
                    C_ad myadjbuffer;
                    C_ad myadjtable;

                    myadjtable=adjtable;
                    myadjbuffer=adjbuffer;
                    for (currentnoun=0; currentnoun<word; currentnoun++)
                        while (read_ub(INC(myadjtable)) != 0) ;

                    while ((given = read_uw(myadjbuffer)) != 0) {
                        myadjbuffer = myadjbuffer ADD 2;
                        adjlist=myadjtable;
                        for (;;) {
                            if ((adj = read_ub(INC(adjlist))) == 0) {
                                adjflag=1;
                                break;
                            }
                            if (adj-3 == given) break;
                        }
                    }
                    if (adjflag==0) {
                        write_uw(section + islong, newmatchbuf);
                        newmatchbuf = newmatchbuf ADD 2;
                        if (islong!=0) oneislong=1;
                        write_uw(word, newmatchbuf);
                        newmatchbuf = newmatchbuf ADD 2;
                        no_matched++;
                    }
                }
            }
	}

        /* if there were any 'long' matches (ie. with '_' in them) then kill the
           others */

	if (oneislong) {
            no_matched=0;
            matchbuf=thirdmatchbuf;
            while (newmatchbuf!=thirdmatchbuf) {
                section=read_uw(thirdmatchbuf);
                thirdmatchbuf = thirdmatchbuf ADD 2;
                word=read_uw(thirdmatchbuf);
                thirdmatchbuf = thirdmatchbuf ADD 2;
                if (section & 0x8000) {
                    write_uw(section & 0x7fff, matchbuf);
                    matchbuf = matchbuf ADD 2;
                    write_uw(word, matchbuf);
                    matchbuf = matchbuf ADD 2;
                    no_matched++;
                }
            }
            newmatchbuf = matchbuf;
            A6=A5 ADD longest_match;
	} else A6=A5 ADD 1;

	Areg[6].l=C_to_E(SIZE_byte, A6);
	Areg[2].l=C_to_E(SIZE_word, newmatchbuf);
	Dreg[0].uw = (type0<<8) | type1;	/* set up TYPE return */
	Dreg[1].uw=adjflag;
    }

    /*
      GETTIED -	finds the next lowest noun which has (12(A0) & 0x3FFF)
      or == D4!
				
      On entry -	D2.W first noun to look at (NOT decremented first)
      D4.W special case - actually the noun that we are trying to
      suss what's tied to it

      On exit -	D2.W noun no. of tied noun (may be equal to D4)
      CARRY - set => noun was found, else no tied nouns left
    */

    void GETTIED()
    {
	int nounno, D4;
	 C_ad nounpointer;

	SR &= ~C_bit;
	nounno = 0x7FFF & Dreg[2].uw;
	D4 = 0x7FFF & Dreg[4].uw;
	nounpointer = noundata_C_ad ADD 12 ADD 14*nounno;

	while (nounno!=0) {
            if (read_ub(nounpointer) & 0x3F ||
                read_ub(nounpointer ADD 1) || nounno == D4) {
                SR |= C_bit;
                break;
            }
            nounno--;
            nounpointer=nounpointer SUB 14;
	}
	if (nounno==0) nounno=1;		/* don't return 0 */
	
	Dreg[2].uw=nounno;
    }

    /* 
       FINDZERO -	finds the Nth zero in a list

       On entry	A1.L points at start of list
       D0.W is the no. of zeros (-1) to find.

       On exit 	A1.L points at byte after Nth zero
       D0.W = -1 (optional)
    */

    void FINDZERO()
    {
	C_ad A1;
	int D0;

	D0 = Dreg[0].uw;
	A1 = E_to_C(SIZE_byte, Areg[1].l);

	while (D0-- != 0) while (read_ub(INC(A1)) != 0) ;

	Dreg[0].w = -1;
	Areg[1].l = C_to_E(SIZE_byte, A1);
    }

#ifndef STANDALONE

    void GRAPHICS()
    {
	printf("\nGraphics trap : D0 = %08x, D1 = %08x, D2 = %08x\n",
               (int)Dreg[0].l, (int)Dreg[1].l, (int)Dreg[2].l);
    }

    void PRINTER()
    {
	printf("\nPrinter trap\n");
    }

    void RESTART()
    {
	int  i;

	load(0, 0);	/* reload all but the symbols */
	for (i=0;i<8;i++) {
            Areg[i].l=0;
            Dreg[i].l=0;
	}
	Areg[7].l=0x1f400;
    }

    void QUIT()
    {
	printf("QUIT trap!!\n");
    }

#endif

    void GETWTAB()
    {
	if (!wtable_loaded) eltham_error("No wordtable loaded!");
	else
            Dreg[1].ub = read_ub(W_to_C(Areg[1].l));
    }

    void PUTWTAB()
    {
	if (!wtable_loaded) eltham_error("No wordtable loaded!");
	else
            write_b(Dreg[1].ub, W_to_C(Areg[1].l));
    }
	
    void PRINTWORD()
    {
	C_ad ptr;
	int channel=0;
	int i=0;

	if (!wtable_loaded) eltham_error("No wordtable loaded!");
	else {
            ptr = W_to_C(Areg[1].l);
            channel = Dreg[1].ub;
            do {
                charout(read_ub(ptr),channel,0);
                i++;
            } while (!(read_ub(INC(ptr)) & 0x80));
            Areg[1].l += i;
	}
    }

    void COPYWORD()
    {
	C_ad src, dest;
	int i=0;

	if (!wtable_loaded) eltham_error("No wordtable loaded!");
	else {
            src = W_to_C(Areg[1].l);
            dest = E_to_C(SIZE_byte, Areg[0].l);
            do {
                write_ub(read_ub(src), dest);
                INC(dest);
                i++;
            } while (!(read_ub(INC(src)) & 0x80));
            Areg[0].l += i;
            Areg[1].l += i;
	}
    }

    void SETNE()
    {
	SR &= ~Z_bit;
    }

    void SETEQ()
    {
	SR |= Z_bit;
    }

    void SETNE_RET()
    {
	SR &= ~Z_bit;
	PC = (UWORD *)E_to_C(SIZE_word, pull_l());
    }

    void SETEQ_RET()
    {
	SR |= Z_bit;
	PC = (UWORD *)E_to_C(SIZE_word, pull_l());
    }

    void POPSP_RET()
    {
	Areg[7].l += 4;
	PC = (UWORD *)E_to_C(SIZE_word, pull_l());
    }

    void FLINE(int ins)
    {
	C_ad entry;
	if (minspaceentry && (ins & 0x7ff) > minspaceentry) {
            if (!(ins & 0x800)) {
                push_l(C_to_E(SIZE_word, PC));
            }
            entry = spacetable_C_ad ADD 2*(0x7ff - (ins & 0x7ff));
            PC = (UWORD *)(entry ADD read_w(entry));
	} else {
            push_l(C_to_E(SIZE_word, PC));
            PC = messagecode;
	}
    }

    void PROMPT_EV()
    {
        printf("prompt ev trap\n");
    }

    void UNDO_DIFF()
    {
        printf("undo diff trap\n");
    }

    void MAKE_DIFF()
    {
        printf("make diff trap\n");
    }

    void NOTIFY()
    {
        printf("notify trap\n");
    }

    /* this is the replacement for CHARIN - the game says to the front end:

       "Hey guys, anything doin'?"

       and the front end says: "yea here's an event (in D1), oh and by the way
       theres a buffer of shit at (A1) (you didn't set A1 - tut tut)!"
    */

    void GET_NEXT_EV()
    {
        getline_ev();
    }

    
