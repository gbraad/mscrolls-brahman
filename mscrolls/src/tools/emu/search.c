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

#include "emu.h"

extern char *wtable;

/*	A6 points at string to identify (inbuff)
	A1 points at ADJBUF
	A2 points at start of (empty) matchbuf
	A0 ADJ.TBL
	A3 TBL.WRD	(pos of begining of nouns' section sometimes)
	A2 points at MATCHBUF
	A5 points to synonyms

	D6 is the section no.

	On exit:
		D0 is TYPE
		D1 is ADJFLG
		A5 needs to be maintained for God knows what reason
		A6 is A5+1 
		A2 points at the word AFTER the last one put into MATCHBUF

*/

search()
{

/* cos 0x20 & 0x5F == 0 !! */
#define SearchSPACE	0

	short *matchbuf,*newmatchbuf,*oldmatchbuf,*thirdmatchbuf,*adjbuffer;
	register short word,section;
	unsigned char *A6,*A5,type0,type1;
	register A3;
	unsigned char *adjtable;
	register int count,adjflag,currentnoun,no_matched;
	unsigned char c1,c2;
	int longest_match=0,islong;		/* flags for in_the_boat */
	int oneislong=0;

	u_char *syntable=(u_char *)(wtable + Areg[5]);

	Areg[5]=Areg[6];		/* hmmm A5  wants to be A6 */

	A6=(unsigned char *)(m68mem_end-Areg[6]);		/* string to match */
	adjtable=(unsigned char *)(m68mem_end-Areg[0]);
	adjbuffer=(short *)(m68mem_end-1 - Areg[1]);
	matchbuf=(short *)(m68mem_end-1 - Areg[2]);
	newmatchbuf=matchbuf;
	oldmatchbuf=matchbuf;
	thirdmatchbuf=matchbuf;	/* 3rd pass to fod non-long */
	A3=Areg[3];		/* word table */

	type0=0;
	type1=0;
	adjflag=0;
	currentnoun=0;	/* to keep track of where in the adj list we are */

	word=0;
	section=(short)Dreg[6];
	count=0;	/* no. of chars matched */
	A5=A6;		/* start of word to match */
	islong=0;
	do {
		c1= wtable[A3++];
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
			c2=(*A6--) &0x5F;
			if ((c2==SearchSPACE) && (c1=='_'))
				{
				c2='_';
				islong=0x8000;	/* flag so as we know a 'long match' found */
				}
			if (c2==SearchSPACE) c2 = 0xFF;
			if ( ( ((c1>0x80) && ((c1&0x5F)==c2) ) || (count>99)) 
						&& ((*A6==0)||(*A6==' ')||(*A6=='\'')) )
				{
				if (*A6=='\'') 
					{
					type0=0x02;
					type1= *(A6-1);
					}
				*matchbuf-- = section+islong;
				*matchbuf-- = word;
				count=0;
				if ( (A5-A6) > longest_match ) longest_match= A5-A6;
				A6=A5;
				islong=0;
				word++;
				while (c1<0x80) c1= wtable[A3++];	/*  find end of word in WTABLE
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
						while ((wtable[A3++] & 0x80)==0);
						word++;
						}
					}
				}
			}
		}
	while (wtable[A3] != 0x81);

/* now resolve the adjectives for any section 6 word (nouns) */
/* yea, so adjectives don't work for synonyms... */

	while (matchbuf!=oldmatchbuf)
		{
		adjflag=0;
		section = *oldmatchbuf--;
		islong = section & 0x8000;
		section = section & 0x7fff;
		word = *oldmatchbuf--;
		if (section==11)
			{
			int w;
			w = (syntable[2*word]<<8) +
			     syntable[2*word+1];
			section=w & 0x1F;
			word=(w>>5) & 0x07FF;
			}
		if (section!=6)
			{
			*newmatchbuf-- = section+islong;
			if (islong!=0) oneislong=1;
			*newmatchbuf-- = word;
			no_matched++;
			}
		else
			{
			register unsigned char *adjlist;

			if (*adjbuffer==0)
				{
				*newmatchbuf-- = section+islong;
				if (islong!=0) oneislong=1;
				*newmatchbuf-- = word;
				no_matched++;
				}
			else
				{
				register given;
				unsigned char adj;
				short *myadjbuffer;
				unsigned char *myadjtable;
				myadjtable=adjtable;
				myadjbuffer=adjbuffer;
				for (currentnoun=0;currentnoun<word;currentnoun++) while (*myadjtable-- != 0);

				while ((given = *myadjbuffer--)!=0)
					{
					adjlist=myadjtable;
					for (;;)
						{
						if ((adj = *adjlist--)==0) 
							{
							adjflag=1;
							break;
							}
						if ((adj-3) == (unsigned char)given) break;
						}
					}
				if (adjflag==0)
					{
					*newmatchbuf-- = section+islong;
					if (islong!=0) oneislong=1;
					*newmatchbuf-- = word;
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
			section= *thirdmatchbuf--;
			word= *thirdmatchbuf--;
			if (section & 0x8000)
				{
				*matchbuf-- = section & 0x7FFF;
				*matchbuf-- = word;
				no_matched++;
				}
			}
		newmatchbuf = matchbuf;
		A6=A5-longest_match;
		}
	else A6=A5-1;

/*	if (no_matched!=0) adjflag=0; */

	Areg[6]=m68mem_end - (u_char *)A6;
	Areg[2]=m68mem_end-1 - (u_char *)newmatchbuf;
	*(short *)(Dreg)=(type0<<8)+type1;	/* set up TYPE return */
	*(short *)(Dreg+1)=adjflag;
}

search_old_wtable()
{

/* cos 0x20 & 0x5F == 0 !! */
#define SearchSPACE	0

	short *matchbuf,*newmatchbuf,*oldmatchbuf,*thirdmatchbuf,*adjbuffer;
	register short word,section;
	unsigned char *A6,*A5,*A3,type0,type1;
	unsigned char *adjtable;
	register int count,adjflag,currentnoun,no_matched;
	unsigned char c1,c2;
	int longest_match=0,islong;		/* flags for in_the_boat */
	int oneislong=0;

	short *syntable=(short *)(m68mem_end-1 - Areg[5]);

	Areg[5]=Areg[6];		/* hmmm A5  wants to be A6 */

	A6=(unsigned char *)(m68mem_end-Areg[6]);		/* string to match */
	adjtable=(unsigned char *)(m68mem_end-Areg[0]);
	adjbuffer=(short *)(m68mem_end-1 - Areg[1]);
	matchbuf=(short *)(m68mem_end-1 - Areg[2]);
	newmatchbuf=matchbuf;
	oldmatchbuf=matchbuf;
	thirdmatchbuf=matchbuf;	/* 3rd pass to fod non-long */
	A3=(unsigned char *)(m68mem_end-Areg[3]);		/* word table */

	type0=0;
	type1=0;
	adjflag=0;
	currentnoun=0;	/* to keep track of where in the adj list we are */

	word=0;
	section=(short)Dreg[6];
	count=0;	/* no. of chars matched */
	A5=A6;		/* start of word to match */
	islong=0;
	do {
		c1= *A3--;
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
			c2=(*A6--) &0x5F;
			if ((c2==SearchSPACE) && (c1=='_'))
				{
				c2='_';
				islong=0x8000;	/* flag so as we know a 'long match' found */
				}
			if (c2==SearchSPACE) c2 = 0xFF;
			if ( ( ((c1>0x80) && ((c1&0x5F)==c2) ) || (count>99)) 
						&& ((*A6==0)||(*A6==' ')||(*A6=='\'')) )
				{
				if (*A6=='\'') 
					{
					type0=0x02;
					type1= *(A6-1);
					}
				*matchbuf-- = section+islong;
				*matchbuf-- = word;
				count=0;
				if ( (A5-A6) > longest_match ) longest_match= A5-A6;
				A6=A5;
				islong=0;
				word++;
				while (c1<0x80) c1= *A3--;	/*  find end of word in WTABLE
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
						while ((*A3-- & 0x80)==0);
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
		section = *oldmatchbuf--;
		islong = section & 0x8000;
		section = section & 0x7fff;
		word = *oldmatchbuf--;
		if (section==11)
			{
			section=syntable[-word] & 0x1F;
			word=(syntable[-word]>>5) & 0x07FF;
			}
		if (section!=6)
			{
			*newmatchbuf-- = section+islong;
			if (islong!=0) oneislong=1;
			*newmatchbuf-- = word;
			no_matched++;
			}
		else
			{
			register unsigned char *adjlist;

			if (*adjbuffer==0)
				{
				*newmatchbuf-- = section+islong;
				if (islong!=0) oneislong=1;
				*newmatchbuf-- = word;
				no_matched++;
				}
			else
				{
				register given;
				unsigned char adj;
				short *myadjbuffer;
				unsigned char *myadjtable;
				myadjtable=adjtable;
				myadjbuffer=adjbuffer;
				for (currentnoun=0;currentnoun<word;currentnoun++) while (*myadjtable-- != 0);

				while ((given = *myadjbuffer--)!=0)
					{
					adjlist=myadjtable;
					for (;;)
						{
						if ((adj = *adjlist--)==0) 
							{
							adjflag=1;
							break;
							}
						if ((adj-3) == (unsigned char)given) break;
						}
					}
				if (adjflag==0)
					{
					*newmatchbuf-- = section+islong;
					if (islong!=0) oneislong=1;
					*newmatchbuf-- = word;
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
			section= *thirdmatchbuf--;
			word= *thirdmatchbuf--;
			if (section & 0x8000)
				{
				*matchbuf-- = section & 0x7FFF;
				*matchbuf-- = word;
				no_matched++;
				}
			}
		newmatchbuf = matchbuf;
		A6=A5-longest_match;
		}
	else A6=A5-1;

/*	if (no_matched!=0) adjflag=0; */

	Areg[6]=m68mem_end - (u_char *)A6;
	Areg[2]=m68mem_end-1 - (u_char *)newmatchbuf;
	*(short *)(Dreg)=(type0<<8)+type1;	/* set up TYPE return */
	*(short *)(Dreg+1)=adjflag;
}
