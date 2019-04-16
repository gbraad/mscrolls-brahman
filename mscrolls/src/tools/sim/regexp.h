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

#pragma once

#define	CBRA	2
#define	CCHR	4
#define	CDOT	8
#define	CCL	12
#define	CDOL	20
#define	CCEOF	22
#define	CKET	24
#define	CBACK	36

#define	STAR	01
#define	RNGE	03

#define	NBRA	9

#define	PLACE(c)	ep[c >> 3] |= bittab[c & 07]
#define	ISTHERE(c)	(ep[c >> 3] & bittab[c & 07])

char	*braslist[NBRA];
char	*braelist[NBRA];
int	nbra, ebra;
char	*loc1, *loc2, *locs;
int	sed;
int	nodelim;

int	circf;
int	low;
int	size;

char	bittab[] = {
    1,
    2,
    4,
    8,
    16,
    32,
    64,
    128
};

static inline void getrnge(char* str)
{
    low = *str++ & 0377;
    size = ((*str & 0377) == 255) ? 20000 : (*str & 0377) - low;
}

static inline int ecmp(char* a, char* b, int count)
{
    while(count--)
    {
        if(*a++ != *b++)
            return(0);
    }
    return(1);
}

char* compile(char* instring, char* ep, char* endbuf, int seof)
{
    INIT	/* Dependent declarations and initializations */
    int c;
    int eof = seof;
    char *lastep = instring;
    int cclcnt;
    char bracket[NBRA], *bracketp;
    int closed;
    char neg;
    int lc;
    int i, cflg;

    lastep = 0;
    if((c = GETC()) == eof || c == '\n')
    {
        if(c == '\n') {
            UNGETC(c);
            nodelim = 1;
        }
        if(*ep == 0 && !sed)
            ERROR(41);
        RETURN(ep);
    }
    bracketp = bracket;
    circf = closed = nbra = ebra = 0;
    if(c == '^')
        circf++;
    else
        UNGETC(c);
    while(1) {
        if(ep >= endbuf)
            ERROR(50);
        c = GETC();
        if(c != '*' && ((c != '\\') || (PEEKC() != '{')))
            lastep = ep;
        if(c == eof) {
            *ep++ = CCEOF;
            RETURN(ep);
        }
        switch(c) {

        case '?':
            *ep++ = CDOT;
            continue;

        case '\n':
            if(!sed) {
                UNGETC(c);
                *ep++ = CCEOF;
                nodelim = 1;
                RETURN(ep);
            }
            else ERROR(36);
        case '*':
            if(lastep==0 || *lastep==CBRA || *lastep==CKET)
                goto defchar;
            *lastep |= STAR;
            continue;

        case '$':
            if(PEEKC() != eof && PEEKC() != '\n')
                goto defchar;
            *ep++ = CDOL;
            continue;

        case '[':
            if(&ep[17] >= endbuf)
                ERROR(50);

            *ep++ = CCL;
            lc = 0;
            for(i = 0; i < 16; i++)
                ep[i] = 0;

            neg = 0;
            if((c = GETC()) == '^') {
                neg = 1;
                c = GETC();
            }

            do {
                if(c == '\0' || c == '\n')
                    ERROR(49);
                if(c == '-' && lc != 0) {
                    if((c = GETC()) == ']') {
                        PLACE('-');
                        break;
                    }
                    while(lc < c) {
                        PLACE(lc);
                        lc++;
                    }
                }
                if(c == '\\') {
                    switch(c = GETC()) {
                    case 'n':
                        c = '\n';
                        break;
                    }
                }
                lc = c;
                PLACE(c);
            } while((c = GETC()) != ']');
            if(neg) {
                for(cclcnt = 0; cclcnt < 16; cclcnt++)
                    ep[cclcnt] ^= -1;
                ep[0] &= 0376;
            }

            ep += 16;

            continue;

        case '\\':
            switch(c = GETC()) {

            case '(':
                if(nbra >= NBRA)
                    ERROR(43);
                *bracketp++ = nbra;
                *ep++ = CBRA;
                *ep++ = nbra++;
                continue;

            case ')':
                if(bracketp <= bracket || ++ebra != nbra)
                    ERROR(42);
                *ep++ = CKET;
                *ep++ = *--bracketp;
                closed++;
                continue;

            case '{':
                if(lastep == (char *) (0))
                    goto defchar;
                *lastep |= RNGE;
                cflg = 0;
            nlim:
                c = GETC();
                i = 0;
                do {
                    if('0' <= c && c <= '9')
                        i = 10 * i + c - '0';
                    else
                        ERROR(16);
                } while(((c = GETC()) != '\\') && (c != ','));
                if(i >= 255)
                    ERROR(11);
                *ep++ = i;
                if(c == ',') {
                    if(cflg++)
                        ERROR(44);
                    if((c = GETC()) == '\\')
                        *ep++ = (char)255;
                    else {
                        UNGETC(c);
                        goto nlim;
                        /* get 2'nd number */
                    }
                }
                if(GETC() != '}')
                    ERROR(45);
                if(!cflg)	/* one number */
                    *ep++ = i;
                else if((ep[-1] & 0377) < (ep[-2] & 0377))
                    ERROR(46);
                continue;

            case '\n':
                ERROR(36);

            case 'n':
                c = '\n';
                goto defchar;

            default:
                if(c >= '1' && c <= '9') {
                    if((c -= '1') >= closed)
                        ERROR(25);
                    *ep++ = CBACK;
                    *ep++ = c;
                    continue;
                }
            }
            /* Drop through to default to use \ to turn off special chars */

        defchar:
        default:
            lastep = ep;
            *ep++ = CCHR;
            *ep++ = c;
        }
    }
}

int advance(char* lp, char* ep)
{
    char *curlp;
    char c;
    char *bbeg;
    int ct;

    while(1)
    {
        switch(*ep++)
        {

        case CCHR:
            if(*ep++ == *lp++)
                continue;
            return(0);

        case CDOT:
            if(*lp++)
                continue;
            return(0);

        case CDOL:
            if(*lp==0)
                continue;
            return(0);

        case CCEOF:
            loc2 = lp;
            return(1);

        case CCL:
            c = *lp++ & 0177;
            if(ISTHERE(c)) {
                ep += 16;
                continue;
            }
            return(0);
        case CBRA:
            braslist[(int)*ep++] = lp;
            continue;

        case CKET:
            braelist[(int)*ep++] = lp;
            continue;

        case CCHR|RNGE:
            c = *ep++;
            getrnge(ep);
            while(low--)
                if(*lp++ != c)
                    return(0);
            curlp = lp;
            while(size--) 
                if(*lp++ != c)
                    break;
            if(size < 0)
                lp++;
            ep += 2;
            goto star;

        case CDOT|RNGE:
            getrnge(ep);
            while(low--)
                if(*lp++ == '\0')
                    return(0);
            curlp = lp;
            while(size--)
                if(*lp++ == '\0')
                    break;
            if(size < 0)
                lp++;
            ep += 2;
            goto star;

        case CCL|RNGE:
            getrnge(ep + 16);
            while(low--) {
                c = *lp++ & 0177;
                if(!ISTHERE(c))
                    return(0);
            }
            curlp = lp;
            while(size--) {
                c = *lp++ & 0177;
                if(!ISTHERE(c))
                    break;
            }
            if(size < 0)
                lp++;
            ep += 18;		/* 16 + 2 */
            goto star;

        case CBACK:
            bbeg = braslist[(int)*ep];
            ct = braelist[(int)*ep++] - bbeg;

            if(ecmp(bbeg, lp, ct)) {
                lp += ct;
                continue;
            }
            return(0);

        case CBACK|STAR:
            bbeg = braslist[(int)*ep];
            ct = braelist[(int)*ep++] - bbeg;
            curlp = lp;
            while(ecmp(bbeg, lp, ct))
                lp += ct;

            while(lp >= curlp) {
                if(advance(lp, ep))	return(1);
                lp -= ct;
            }
            return(0);


        case CDOT|STAR:
            curlp = lp;
            while(*lp++);
            goto star;

        case CCHR|STAR:
            curlp = lp;
            while(*lp++ == *ep);
            ep++;
            goto star;

        case CCL|STAR:
            curlp = lp;
            do {
                c = *lp++ & 0177;
            } while(ISTHERE(c));
            ep += 16;
            goto star;

        star:
            do {
                if(--lp == locs)
                    break;
                if(advance(lp, ep))
                    return(1);
            } while(lp > curlp);
            return(0);

        }
    }
}

int estep(char* p1, char* p2)
{
    int c;

    if(circf)
    {
        loc1 = p1;
        return(advance(p1, p2));
    }
    /* fast check for first character */
    if(*p2==CCHR)
    {
        c = p2[1];
        do {
            if(*p1 != c)
                continue;
            if(advance(p1, p2))
            {
                loc1 = p1;
                return(1);
            }
        } while(*p1++);
        return(0);
    }
    
    /* regular algorithm */
    do {
        if(advance(p1, p2))
        {
            loc1 = p1;
            return(1);
        }
    } while(*p1++);
    return(0);
}


