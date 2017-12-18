/**
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
 *
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "defs.h"
#include "data.h"
#include "utils.h"
#include "in_word.h"
#include "errors.h"
#include "change.h"

extern char *command;
extern int lineno;
extern int type;
extern char id[];
extern char locname[];
extern char fixedname[];
extern char slname[];
extern char pseudos[][MAXIDLEN];
extern char verbtext[][MAXVERBLEN];
extern int verbnum;

extern char *valid_sections[];
extern FILE *fwtable11, *fsyns;

int clonenum=0, clonedescnum=0;
FILE *fwtable4, *fverbequ;

static char* fod_hyphens(char* s)
{
    static char verbname[128];
    char *p=verbname;
    while (*s) {
	if (*s!='-') *p++ = *s;
	s++;
    }
    *p=0;
    return verbname;
}

void do_synonym()
{
    char s[128],synonym[128],*p,section[128],word[128],c;
    int type;

    in_word(s);
    if (!ISQUOTE(s)) expected("\"");

    p=synonym;
    while ((c = in_char()) != '"') *p++ = c;
    *p = '\0';

    in_word(s);
    if (!ISOPENB(s)) expected("{");

    in_word(word);			/* can't verify this - could be anything */
    in_word(section);

    if (!(type = from_word(section, valid_sections))) invalid_prop(section);

    do_wtable(synonym, fwtable11);
    switch(type) {
    case NOUN:
        fprintf(fsyns, "\tSYNMACRO\tN%s-1,SECT.NOUN\n", ucaseof(word));
        printf("Warning - strange synonym of a noun! %s/%s\n",synonym,word);
        break;
    case ADJ:
        fprintf(fsyns, "\tSYNMACRO\tADJ_%s-4,SECT.ADJ\n",word);
        break;
    case VERB:
        fprintf(fsyns, "\tSYNMACRO\tVN%s,SECT.VERB\n",ucaseof(word));
        break;
    default:	
        fprintf(fsyns, "\tSYNMACRO\t%s,SECT.%s\n",word,
                ucaseof(section));
    }
    in_word(s);
}

void do_verbs()
{
    char s[MAXVERBLEN];

    in_word(s);
    if (!ISOPENB(s)) expected("{");

    while (strcmp(s,"}"))
    {
        in_word(s);
        strcpy(verbtext[verbnum++],fod_hyphens(s));
        if (verbnum > MAXVERBS) fprintf(stderr,"Waaa - too many verbs!!\n");
    } 
}

void write_verbs()
{
    int i;

    if (verbnum == 0) 
    {
        fprintf(stderr,"Warning! - No verbs defined!!\n");
        return;
    }

    fverbequ = openi("verbequ");
    fwtable4 = openi("wtable4");

    for (i=0;i<verbnum-1;i++)
    {
        fprintf(fverbequ,"VN%s\tEQU\t%d\n",ucaseof(verbtext[i]),i);
        do_wtable(verbtext[i], fwtable4);
    }

    closeasm(fverbequ,"verbequ",".i");
    closeasm(fwtable4,"wtable4",".i");
}


/* format of a clone line:

   clone freddy john "big freddy"

   where 'clone is the type, freddy is the new identifier of the clone, john is
   the identifier of the object being cloned. Between the quotes is as normal for
   a noun, ie adjectives and base word.

   also

   clonedesc freddy john "big freddy"

*/

extern char clonedescid_strings[MAXNITEMS/4][MAXIDLEN];
extern char cloneid_strings[MAXNITEMS/4][MAXIDLEN];

void do_clone()
{
    char s[128],cloneid[128],nounid[128],*p,*q,c;
    int i;
    extern VFILE *vcloneadj, *vclonewtable6, *vcloneequ, *vclonedata;
    extern VFILE *vclonedescadj, *vclonedescwtable6, *vclonedescequ, *vclonedescdata;

    in_word(cloneid);	/* like 'freddy' */
    in_word(nounid);	/* like 'john' */
    in_word(s);
    if (!ISQUOTE(s)) expected("\"");
    p = s;
    while ((c = in_char()) != '"') *p++ = c;
    *p = '\0';
    p = s;
    i = 0;	/* using this as a flag	*/
    if (type==CLONE) {
        Vprintf(vcloneadj, "* %s\n\tADJ\t", cloneid);
        strcpy(cloneid_strings[clonenum], ucaseof(cloneid));
    }
    else {
        Vprintf(vclonedescadj, "* %s\n\tADJ\t", cloneid);
        strcpy(clonedescid_strings[clonedescnum], ucaseof(cloneid));
    }
    while (q = strchr(p, ' '))
    {
        char tempstring[100],*ts;
        *q++ = '\0';
        if (i)
            if (type==CLONE)
                vputc(',', vcloneadj);
            else
                vputc(',', vclonedescadj);
        else i++;
        strcpy(tempstring,p);
        ts=tempstring;
	/* loose '-' in adj's identifiers */
        while (*ts)
        {
            if ((*ts == '-') || (*ts == ' ') ||
                (*ts == '\'') || (*ts == '.')) *ts = '_';
            ts++;
        }

        if (type==CLONE)
            Vprintf(vcloneadj, "%s", tempstring);
        else
            Vprintf(vclonedescadj, "%s", tempstring);
        do_adj(p);	/* make a new adjective if ness	*/
        p = q;
    }
    if (type==CLONE) {
        vputc('\n', vcloneadj);
        vdo_wtable(p, vclonewtable6); /* do word table for this noun */
        Vprintf(vcloneequ, "N%s\tEQU\tMAXCLONEDESC+%d\n", ucaseof(cloneid),
                clonenum++);
        Vprintf(vclonedata, "\tDC.W\tN%s\n",ucaseof(nounid));
        change_clone_props(cloneid);
    } else {
        vputc('\n', vclonedescadj);
        vdo_wtable(p, vclonedescwtable6); /* do word table for this noun */
        Vprintf(vclonedescequ, "N%s\tEQU\tNNOUNS+%d\n", ucaseof(cloneid),
                clonedescnum++);
        Vprintf(vclonedescdata, "\tDC.W\tN%s\n",ucaseof(nounid));
        change_clone_props(cloneid);
    }

    /* fudge id into word table as a synonym */

    {
        char tempstring[128];
        strcat(strcpy(tempstring, "#"),id);
        fprintf(fwtable11," IFNE YCHEAT&VAX\n");
        do_wtable(tempstring, fwtable11);
        fprintf(fwtable11," ENDC\n");
        fprintf(fsyns, "\tSYNMACRO2\tN%s-1,SECT.NOUN\n", ucaseof(cloneid));
    }
}
