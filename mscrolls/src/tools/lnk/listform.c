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
#include <stdlib.h>
#include "hunks.h"

static int getlong(FILE* fp)
{
    int a,b,c,d;
    a=getc(fp);
    b=getc(fp);
    c=getc(fp);
    d=getc(fp);
    if (feof(fp)) return EOF;
    return (a<<24)+(b<<16)+(c<<8)+d;
}

static int getstring(char* s, FILE* fp)
{
    int i, len=getlong(fp);
    for (i=len*4; i; i--)
        *s++ = getc(fp);
    *s=0;
    return len;
}

static int getstring2(int len, char* s, FILE* fp)
{
    int i;
    for (i=len*4; i; i--)
        *s++ = getc(fp);
    *s=0;
    return len;
}

int main(int argc, char** argv)
{
    int i, n, type;
    int f,l;
    FILE *fp;
    FILE *fq;
    char s[256];
    char *codefile;
    char *datafile;

    if (argc == 1 || argc > 4) {
        fprintf(stderr,"Usage: lf objfile [codefile] [datafile]\n");
        exit(-1);
    }
    if (argc > 2)
        codefile = argv[2];
    else
        codefile = NULL;
    if (argc == 4)
        datafile = argv[3];
    else
        datafile = NULL;
    fp = fopen(argv[1], "rb");
    if (!fp) {
        perror(argv[1]);
        exit(-1);
    }
    while ((n = getlong(fp)) != EOF)
        switch (n) {	/* hunk type */
        case hunk_unit :
            getstring(s, fp);
            printf("hunk unit:%s\n", s);
            break;
        case hunk_name :
            getstring(s, fp);
            printf("hunk name:%s\n", s);
            break;
        case hunk_code :
            printf("hunk code, length %d lwords\n", i=getlong(fp));
            if (codefile) {
                printf("copying code to %s\n", codefile);
                fq = fopen(codefile, "wb");
                if (!fq) {
                    fprintf(stderr,"can't open %s\n", codefile);
                    goto fred;
                } else {
                    for (i*=4; i; i--)
                        putc(getc(fp), fq);
                    fclose(fq);
                }
            } else
            fred:			for (i*=4; i; i--)
                    getc(fp);
            break;
        case hunk_code_word :
            printf("word aligned code, %d words\n", i=getlong(fp));
            for (i=2*i; i; i--) getc(fp);
            break;
        case hunk_data :
            printf("hunk data, length %d lwords\n", i=getlong(fp));
            if (datafile) {
                printf("copying data to %s\n", datafile);
                fq = fopen(datafile, "wb");
                if (!fq) {
                    fprintf(stderr,"can't open %s\n", datafile);
                    goto bill;
                } else {
                    for (i*=4; i; i--)
                        putc(getc(fp), fq);
                    fclose(fq);
                }
            } else
            bill:			for (i*=4; i; i--)
                    getc(fp);
            break;
        case hunk_bss :
            printf("hunk bss, size %d lwords\n", getlong(fp));
            break;
        case hunk_reloc32 :
            printf("hunk reloc 32 absolute :\n");
            while ((i=getlong(fp)) != 0)
            {
                n=getlong(fp);
                printf("\t%d lwords for hunk %d :\n", i, n);
                for (; i; i--)
                    printf("\t\t%08x\n", getlong(fp));
            }
            break;
        case hunk_reloc16An :
            printf("hunk reloc 16 An relative :\n");
            while ((i=getlong(fp)) != 0)
            {
                n=getlong(fp);
                printf("\t%d lwords for hunk %d\n", i, n);
                for (; i; i--) getlong(fp);
            }
            break;
        case hunk_reloc16 :
            printf("hunk reloc 16 pc relative :\n");
            while ((i=getlong(fp)) != 0)
            {
                n=getlong(fp);
                printf("\t%d lwords for hunk %d\n", i, n);
                for (; i; i--) getlong(fp);
            }
            break;
        case hunk_reloc8 :
            printf("hunk reloc 8 pc relative :\n");
            while ((i=getlong(fp)) != 0)
            {
                n=getlong(fp);
                printf("\t%d lwords for hunk %d\n", i, n);
                for (; i; i--) getlong(fp);
            }
            break;
        case hunk_ext :
            printf("hunk ext :\n");
            while ((i=getlong(fp)) != 0)
            {
                type = (i>>24)&0xff;
                i &= 0xffffff;
                getstring2(i, s, fp);
                switch (type)
                {
                case ext_symb :
                    printf("\tsymbol %s\n", s);
                    break;
                case ext_def :
                    printf("\trelocatable definition %s=%08x\n",
                           s, getlong(fp));
                    break;
                case ext_abs :
                    printf("\tabsolute definition %s=%08x\n",
                           s, getlong(fp));
                    break;
                case ext_res :
                    printf("\trelocatable library definition %s=%08x\n",
                           s, getlong(fp));
                    break;
                case ext_common :
                    printf("\tcommon reference to %s size %d, %d refs\n",
                           s, getlong(fp), i=getlong(fp));
                    for (; i; i--) getlong(fp);
                    break;
                case ext_ref32 :
                    printf("\t32 bit reference to %s, %d refs\n",
                           s, i=getlong(fp));
                    for (; i; i--) getlong(fp);
                    break;
                case ext_ref16 :
                    printf("\t16 bit reference to %s, %d refs\n",
                           s, i=getlong(fp));
                    for (; i; i--) getlong(fp);
                    break;
                case ext_ref16An :
                    printf("\t16 bit reference to %s(An), %d refs\n",
                           s, i=getlong(fp));
                    for (; i; i--) getlong(fp);
                    break;
                case ext_ref8 :
                    printf("\t8 bit reference to %s, %d refs\n",
                           s, i=getlong(fp));
                    for (; i; i--) getlong(fp);
                    break;
                }
            }
            break;
        case hunk_symbol :
            printf("hunk symbol :\n");
            while (getstring(s, fp))
                printf("\t%-20s=%08x\n", s, getlong(fp));
            break;
        case hunk_end :
            printf("hunk end\n");
            break;
        case hunk_header :
            printf("hunk header :\n");
            printf("\tname list :\n");
            while (getstring(s, fp))
                printf("\t\t%s\n", s);
            printf("\ttable size %08x\n", getlong(fp));
            printf("\tfirst hunk %d\n", f=getlong(fp));
            printf("\tlast hunk %d\n", l=getlong(fp));
            printf("\thunk sizes :\n");
            for (i=f; i<=l; i++)
                printf("\t\t%08x\n", getlong(fp));
            break;
        default :
            printf("bad hunk %d, %x\n", n, n);
        }
    exit(0);
}

