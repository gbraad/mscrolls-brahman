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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"
#include "data.h"
#include "utils.h"
#include "os.h"

#define LIBDIR	"../../tools/lib/fred23jr/"
#define COMPAT_LIBDIR	"../../tools/lib/fred23jr/compat/"

extern char *command;
extern int warnings, forceupdate;	/* command line flags	*/
extern char id[];
extern char locname[];
extern char fixedname[];
extern char slname[];
extern char *pvaluestring;
extern char *fredname, *thetime;
extern FILE *fdata, *fadjequ, *fwtable5, *fwtable6;
extern int eltformat;

static char *libdir[]={
	COMPAT_LIBDIR,
	LIBDIR
	};

int word_no=0;
char word_str[MAXNWORDS][MAXIDLEN];
int word_value[MAXNWORDS];

int word_val(char* s)
{
    int i;

    for (i=0; i<word_no; i++)
        if (!strcmp(word_str[i], s)) break;
    if (i == word_no) return 0;
    return word_value[i];
}

int from_word(char* s, char** p)
{
    int i=1;

    for (; *p; p++, i++) if (!strcmp(s, *p)) return i;
    return 0;
}

void cantopen(char* name)
{
    fprintf(stderr, "%s: can't open \"%s\"\n", command, name);
    exit(-1);
}

char *ucaseof(char* s)
{
    char *p, *q;
    static char temp[30];

    p = s;
    q = temp;
    while (*p) {
        *q++ = islower(*p) ? toupper(*p) : *p;
        p++;
    }
    *q = '\0';
    return temp;
}

void output_data(struct data* dp, FILE* fp)
{
    int i;
    char *p;

    p = (char *)dp;
    fprintf(fdata, "\tDC.W\t");
    for (i=0; i<3; i++) {
        putc('$', fp);
        print_hexb(*p++, fp);
        print_hexb(*p++, fp);
        putc(',', fp);
    }
    putc('$', fp);
    print_hexb(*p++, fp);
    print_hexb(*p++, fp);
    if (slname[0]) fprintf(fp,"+RN%s",ucaseof(slname));
    fprintf(fdata, ",%s,", locname);
    p += 2;
    putc('$', fp);
    print_hexb(*p++, fp);
    print_hexb(*p++, fp);
    fprintf(fdata, ",N%s", ucaseof(fixedname));
    if (dp->fixed) fprintf(fdata, "+$8000");
    p += 2;
    fprintf(fp, "\t; %s\n", id);
}

void output_npc_data(struct npc* np, FILE* fp)
{
    char *p;

    p = (char *)np;
    fputs("\tDC.B\t$", fp);
    print_hexb(*p++, fp);
    fputs(",$", fp);
    print_hexb(*p++, fp);
    fputs(",$", fp);
    print_hexb(*p++, fp);
    fputs(",$", fp);
    print_hexb(*p++, fp);
    fputs("\n", fp);
}

void print_hexb(int n, FILE* fp)
{
    static char hexdigits[] = "0123456789ABCDEF";
    putc(hexdigits[(n>>4)&15], fp);
    putc(hexdigits[n&15], fp);
}

void do_adj(char* s)
{
    int i;
    char *p;
    char temp[30];
    static char adj_list[500][30];
    static int adj_no=0;

    for (i=0; i<adj_no; i++)
        if (!strcmp(s, adj_list[i])) return;	/* already there	*/
    strcpy(adj_list[adj_no++], s);
    do_wtable(s, fwtable5);
    for (p=temp; *s; s++, p++)	/* fix so u don't get '-'s in labels	*/
        if ((*s == '-') || (*s == ' ') || (*s == '.') || (*s == '\'') || (*s==':')) *p = '_';
        else *p = *s;
    *p = 0;
    fprintf(fadjequ, "\tADJDEF\t%s\n", temp);
}

void vdo_wtable(char* s, VFILE* fp)
{
    int lastch, len;
    char *ptr;

    Vprintf(fp, "\tDC.B\t'");
    len = strlen(s);
    lastch = s[len-1];
    s[len-1] = '\0';

    ptr = s;
    while (*ptr != '\0')
    {
        if (*ptr=='.') *ptr=' ';
        else if (*ptr==':') *ptr='.';	/* grr */
        ptr++;
    }

    Vprintf(fp,"%s",s);

    s[len-1] = lastch;
    Vprintf(fp,"',$%02x+$80\n",lastch);
}

void do_wtable(char* s, FILE* fp)
{
    int lastch, len;
    char *ptr;

    fputs("\tDC.B\t'", fp);
    len = strlen(s);

    ptr = s;
    while (*ptr != '\0')
    {
        if (*ptr=='.') *ptr=' ';
        else if (*ptr==':') *ptr='.';	/* grr */
        ptr++;
    }

    lastch = s[len-1];
    s[len-1]= '\0';	
    fputs(s, fp);

    s[len-1] = lastch;
    fputs("',$", fp);
    print_hexb(lastch, fp);
    fputs("+$80\n", fp);
}

static void checkupdate(char* name, char* ending)
{
    char oldname[FNAME_SZ], newname[FNAME_SZ];
    FILE *fp;
    int c, checksum, place;
    int n;

    strcat(strcpy(oldname, name), ending);
    strcat(strcat(strcpy(newname, "tmp"), name), ending);
    
    fp = fopen(newname, "r+b");
    if (!fp) cantopen(newname);
    
    checksum = 0;
    place=1;
    
    while ((c = getc(fp)) != EOF)
    {
        if (c)
        {
            checksum *= c;
            checksum = ((checksum & 0xffff)<<16)+((checksum & 0xffff0000)>>16);
        } else checksum++;
        checksum += place++;
    }
    
    fprintf(fp, "* produced on %s\n* checksum = %08x\n",thetime,checksum);
    fclose(fp);
    
    fp = fopen(oldname, "rb");
    if (fp)
    {
        fseek(fp, -9, 2);	/* checksum is 9 chars from EOF	*/
        fscanf(fp, "%08x", &n);
        fclose(fp);
    }

    /* checksums different ? */
    if (forceupdate || !fp || n != checksum)
    {
        copy_file_to(newname, oldname); // copy to oldname
        if (warnings) printf("wrote '%s'\n", oldname);
    }
    else
        if (warnings) printf("Warning %s not updated\n", oldname);
    
    deleteFile(newname);
}

FILE *openasm(char* name)
{
    /* file containing the hdr portion of the file	*/
    char hdrfile[_PATH_MAX];

    /* file name	*/
    char fname[FNAME_SZ];
    
    FILE *fp;
    strcat(strcat(strcpy(hdrfile, libdir[eltformat]), name), ".hdr");
    strcat(strcat(strcpy(fname, "tmp"), name), ".asm");
    fp = fopen(fname, "wb");
    if (!fp) cantopen(fname);
    
    fputs(
          "***************************************************************************\n",
          fp);
    fputs("*\n", fp);
    fputs("*\n", fp);
    fprintf(fp, "*\tfred23jr output from file '%s'", fredname);
    fputs("*\n", fp);
    fputs("*\n", fp);
    fputs(
          "***************************************************************************\n",
          fp);
    fputs("\n", fp);

    // copy in lib header
    copy_file(hdrfile, fp);
    return fp;
}

VFILE *vopenasm(char* name)
{
    /* file containing the hdr portion of the file	*/
    char hdrfile[_PATH_MAX];
    char fname[FNAME_SZ];		/* file name	*/
    VFILE *fp;
    strcat(strcat(strcpy(hdrfile, libdir[eltformat]), name), ".hdr");
    strcat(strcat(strcpy(fname, "tmp"), name), ".asm");
    fp = vopen (fname, "wb");
    if (!fp) cantopen(fname);
    Vprintf(fp, "***************************************************************************\n");
    Vprintf(fp,"*\n");
    Vprintf(fp,"*\n");
    Vprintf(fp, "*\tfred23jr output from file '%s'", fredname);
    Vprintf(fp,"*\n");
    Vprintf(fp,"*\n");
    Vprintf(fp, "***************************************************************************\n");
    Vprintf(fp,"\n");
    vcopy_file(hdrfile, fp);
    return fp;
}

VFILE *vopeni(char* name)
{
    static char *ending[]={
        ".asm",
        ".i"
    };
    /* file containing the hdr portion of the file	*/
    char hdrfile[_PATH_MAX];
    char fname[FNAME_SZ];		/* file name	*/
    VFILE *fp;
    strcat(strcat(strcpy(hdrfile, libdir[eltformat]), name), ".hdr");
    strcat(strcat(strcpy(fname, "tmp"), name), ending[eltformat]); /* eltformat =0 or 1 */
    fp = vopen (fname, "wb");
    if (!fp) cantopen(fname);
    Vprintf(fp, "***************************************************************************\n");
    Vprintf(fp,"*\n");
    Vprintf(fp,"*\n");
    Vprintf(fp, "*\tfred23jr output from file '%s'", fredname);
    Vprintf(fp,"*\n");
    Vprintf(fp,"*\n");
    Vprintf(fp, "***************************************************************************\n");
    Vprintf(fp,"\n");
    vcopy_file(hdrfile, fp);
    return fp;
}

FILE *openi(char* name)
{
    static char *ending[]={
        ".asm",
        ".i"
    };

    /* file containing the hdr portion of the file	*/
    char hdrfile[_PATH_MAX];
    char fname[FNAME_SZ];		/* file name	*/
    FILE *fp;
    strcat(strcat(strcpy(hdrfile, libdir[eltformat]), name), ".hdr");
    strcat(strcat(strcpy(fname, "tmp"), name), ending[eltformat]); /* eltformat =0 or 1 */
    fp = fopen (fname, "wb");
    if (!fp) cantopen(fname);
    fputs(
          "***************************************************************************\n",
          fp);
    fputs("*\n", fp);
    fputs("*\n", fp);
    fprintf(fp, "*\tfred23jr output from file '%s'", fredname);
    fputs("*\n", fp);
    fputs("*\n", fp);
    fputs(
          "***************************************************************************\n",
          fp);
    fputs("\n", fp);
    copy_file(hdrfile, fp);
    return fp;
}

void vcloseasm(VFILE* fp, char* name, char* ending)
{
    static char *end=".asm";
    char ftrfile[_PATH_MAX];
    strcat(strcat(strcpy(ftrfile, libdir[eltformat]), name), ".ftr");
    vcopy_file(ftrfile, fp);
    vclose(fp);
    if (eltformat) checkupdate(name,ending);
    else checkupdate(name,end);
}

void closeasm(FILE* fp, char* name, char* ending)
{
    static char *end=".asm";
    char ftrfile[_PATH_MAX];
    strcat(strcat(strcpy(ftrfile, libdir[eltformat]), name), ".ftr");

    // write in footer
    copy_file(ftrfile, fp);
    fclose(fp);
    
    if (eltformat) checkupdate(name,ending);
    else checkupdate(name,end);
}

void vcopy_file(char* from, VFILE* to)
{
    int n,i;
    FILE *fp;
    char buf[512];
    if ((fp = fopen(from, "rb")) == NULL) cantopen(from);
    do {
        n = fread(buf, 1, 512, fp);
        for (i=0;i<n;i++) vputc(buf[i],to);
    } while (n == 512);
    fclose(fp);
}

void copy_file_to(char* from, char* name)
{
    FILE* fout = fopen(name, "wb");
    if (!fout) cantopen(name);
    copy_file(from, fout);
    fclose(fout);
}

void copy_file(char* from, FILE* to)
{
    // copy contents of `from` into `to`
    int n;
    FILE *fp;
    char buf[512];
    if ((fp = fopen(from, "rb")) == NULL) cantopen(from);
    do {
        n = fread(buf, 1, 512, fp);
        fwrite(buf, 1, n, to);
    } while (n == 512);
    fclose(fp);
}

void putvalue(char* string)
{
    char c;
    while ((c = *string++)!=0) *pvaluestring++ = c;
}
	
