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
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "dis.h"
#include "kem.h"
#include "hunks.h"
#include "dis1.h"
#include "dis2.h"
#include "dis3.h"
#include "get_ea.h"
#include "read_sym.h"
#include "aline.h"
#include "display.h"
#include "symbols.h"

extern char *PC,*memory;

int more_count = 0;
int moreflag = TRUE;


#define MAXMADSIZE 400000
char Madness[MAXMADSIZE];       /* to buffer prog file */

char RestartGame[33000];
char *begincode;

int AmigaDOS;       /* TRUE if first long word = hunk_header */
int InstructionCount=0;
FILE *exec_file=NULL;
FILE *log_file=NULL;

int trace_val;      /* offset into array of stack_trace structures */
#define MAXSTACKTRACE 1000
struct stack_trace trace_info[MAXSTACKTRACE];

int verbose=FALSE;
int debug=FALSE;
int hexoption = FALSE;
int soption = FALSE;
int skip = FALSE;
int maggot_rolls = TRUE;
char *textname,*longlogname;
FILE *longlogfile=NULL;
int infile;
char *infile_pnt;
int  pc = 0;
int instrwords = 0;
char string[40];
#define MAXCODESIZE 200000
unsigned char code[MAXCODESIZE];    /* was short array */

int pass;
jmp_buf env;
char inputbuf[256], *inputp;
int addr,breakpoint[8];
int n_text,n_data,n_sym,n_bss;

#define TRACESIZE 102400

int traceplace=0;
int pctrace[TRACESIZE];

extern struct registers registers;

static int fastreadword() 
{
    int word, w2;

    word = code[pc];
    word = word << 8;
    w2 = code[pc+1];
    word = word + w2;
    pc += 2;
    return wordext(word);
}

int readword() 
{
    int i, word, w2;

    word = code[pc];
    word = word << 8;
    w2 = code[pc+1];
    word = word + w2;
    if (pass == 2)
    {
        if (instrwords == 0)
        {
            for (i=0; i<n_symbols; i++)
                if (pc == sym_table[i].value) break;
            if (i<n_symbols) printf("%-8s ", sym_table[i].name);
            else printf("         ");
        }
    }
    instrwords++;
    pc += 2;
    return wordext(word);
}

int readlong()
{
    return (readword() << 16) + (readword() & 0xffff);
}

char *addr_str(int ad)
{
    char* s = sym_from_ad(ad, 0);
    return s ? s : "unknown";
}

void dprint(const char* form, ...)
{
    va_list args;
    va_start(args, form);

    vsprintf(string + strlen(string), form, args);
    va_end(args);
}

int opsize(int op, int a)
{
    switch (op)
    {
    case i_ori:
    case i_andi:
    case i_subi:
    case i_addi:
    case i_eori:
    case i_cmpi:
    case i_negx:
    case i_clr:
    case i_neg:
    case i_not:
    case i_TEST:
    case i_tst:
    case i_addq:
    case i_subq:
    case i_or:
    case i_sub:
    case i_subx:
    case i_cmp:
    case i_cmpm:
    case i_eor:
    case i_and:
    case i_add:
    case i_addx:
    case i_dshift: return bits(a, 7, 6);

    case i_move: switch (bits(a, 13, 12)) {
        case 1: return 0;
        case 2: return 2;
        case 3: return 1;
        }
    case i_movea: return bits(a, 13, 12) == 3 ? 1 : 2;

    case i_adda:
    case i_suba:
    case i_cmpa: return bits(a, 8, 6) == 3 ? 1 : 2;

    case i_mmtreg:
    case i_mmfreg:
    case i_movep: return bits(a, 6, 6) == 0 ? 1 : 2;

    case i_dbit:
    case i_sbit:
    case i_scc:
        return 0;

    default: return 1;
    }
}

const char* condcode(int op, int a)
{
    switch (bits(a, 11, 8))
    {
    case  4: return "CC";
    case  5: return "CS";
    case  7: return "EQ";
    case 12: return "GE";
    case 14: return "GT";
    case  2: return "HI";
    case 15: return "LE";
    case  3: return "LS";
    case 13: return "LT";
    case 11: return "MI";
    case  6: return "NE";
    case 10: return "PL";
    case  8: return "VC";
    case  9: return "VS";
    case  0: return op == i_bcc ? "RA" : "T ";
    case  1: return op == i_bcc ? "SR" : "F ";
    }
    return "";
}

void writeop(char* op, int n)
{
    int s=0;
    while (op[s] != 0) s++;
    dprint (op);
    if (n == 0 || n == 4) dprint(".B");
    if (n == 1 || n == 5) dprint(".W");
    if (n == 2 || n == 6) dprint(".L");
    if (n >= 0) s = s + 2;
    for (; s<=8; s++) dprint(" ");
}

static int nextchar()
{
    if (!*inputp) return 0;
    return *inputp++;
}

int getaddress()
{
    int i=0;
    char s[80];
    char c;
    while ((c = nextchar()) == ' ') ;
    while (c && (c != ',') && (c != ' ') && (c!=')'))
    {
        s[i++]=c;
        c = nextchar();
    }
    while (c == ')') c = nextchar();
    s[i] = '\0';
    if (s[0] == '\0') return -1;
    return calcaddress(s);
}

static int hexval(int c)
{
    if (isdigit(c)) return c-'0';
    if (c>='A' && c<='F') return c-'A'+10;
    if (c>='a' && c<='f') return c-'a'+10;
    return 0;
}

static int gethexad(char* s)
{
    int a=0, c;
    while ((c = *s++) != '\0')
    {
        a *= 16;
        a += hexval(c);
    }
    return a;
}

int calcaddress(char* s)
{
    int i=0;
    if (s[0]=='#')
    {
        if (s[1] == 'p' && s[2] == 'c') return PC - memory;
        
        i=s[2]-'0';
        if ((i<0) || (i>7)) 
        {
            printf("Bad register\n");
            return -1;
        }
        if ((s[1]=='A') || (s[1]=='a'))
            return reverse_l(registers.A[i].L);
        if ((s[1]=='D') || (s[1]=='d'))
            return reverse_l(registers.D[i].L);
        printf("Bad register\n");
        return -1;
    }
    if (s[0]=='(')  /* indirection - lots of nasties to avoid bad addr's.  */
    {
        int temp;
        temp=calcaddress(s+1);
        if (temp==-1) return -1;
        if ((temp<0) || (temp>MAXCODESIZE)) return -1;
        temp= reverse_l(*(int *)(memory+temp));
        if ((temp<0) || (temp>MAXCODESIZE)) return -1;
        return temp;
    }
    if (!isdigit(s[0])) /* look up in symbol table  */
    {
        int ad = 0;
        
        if (s[0]=='.')  /* to suss labels starting with 0-9 (local) */
        {
            ad = sym_to_ad(s+1);
        }
        else ad = sym_to_ad(s);

        if (!ad)
        {
            printf("Unrecognised label \"%s\"\n", s);
            return -1;
        }
        return ad;
    }
    return gethexad(s);
}


#if 0
interrupt(sig)
{
    switch (sig) {
    case SIGINT:
        longjmp(env, 1);
    case SIGSEGV:
        longjmp(env,2);
    case SIGBUS:
        longjmp(env,3);
    }
}
#endif

static void printlabel(int addr)  
{
    char* s = sym_from_ad(addr, 0);
    if (s) printf("%s (%08x)",s,addr);
    else printf("%08x",addr);
}

static void plabel(int addr)        /* prints label (value) lomg word */
{
    printlabel(addr);
    if (addr<MAXCODESIZE) printf(": %08x\n",reverse_l(*(int *)(memory+addr)));
    else putchar('\n');
}

/* put info on stack at a BSR/JSR/CALL to allow us to look where we've been */

void push_stack_trace(char* newpc, int sp)
{
    if (trace_val>MAXSTACKTRACE)
        printf("\n\nSTACK TRACE overflow!!\n\n");
    else
    {
        trace_info[trace_val].pc=newpc-memory;
        trace_info[trace_val].sp=sp;
        trace_val++;
    }
}

/* take info off again when an RTS/RET happens */

void pop_stack_trace(int sp)
{
    int i;
    if (sp==trace_info[trace_val].sp)
        trace_val--;
    else
        for (i=0;i<trace_val;i++)
            if (trace_info[i].sp <= sp) 
            {
                trace_val=i;
                break;
            }
}

static void saveproc(int addr)
{
    pctrace[traceplace++]=addr;
    if (traceplace==TRACESIZE) traceplace=0;
}

int mgetb_infile()
{
    return *infile_pnt++ & 0xff;
}

/* this gets a long word! */
int mgetw_infile()
{
    int result;
    result = mgetb_infile() << 24;
    result = result + (mgetb_infile() << 16);
    result = result + (mgetb_infile() << 8);
    result = result + mgetb_infile();
    return result;
}

int simopen(const char* filename, int flags)
{
    
#ifdef _WIN32
    flags |= O_BINARY;
#endif

    return open(filename, flags);
}

static void doread_infile()
{
    int n;

    /* Madness = (char *)malloc(MAXMADSIZE);
       if (Madness == NULL) 
       {
       printf("WAAA!!!! Unable to malloc mad amount of memory!!!\n");
       exit (-1);
       } */
    n=read (infile, Madness, MAXMADSIZE);
    close(infile);
    if (debug) printf("Read %08x bytes...\n",n);
    infile_pnt = Madness;
}

int main(int argc, char** argv)
{
    int i, word, op, c, end, oldpc;
    char *p;

    if (argc < 2)
    {
        fprintf (stderr, "Usage: sim [-v] prog text logfile\n");
        return 1;
    }

    if ((!strcmp("-v",argv[1])) || (!strcmp("-d",argv[1])) )
    {
        if (!strcmp("-d",argv[1])) debug=TRUE;
        verbose=TRUE;
        if ((infile = simopen(argv[2], 0)) == -1)
        {
            fprintf (stderr, "Can't open %s\n", argv[2]);
            return -1;
        }
        doread_infile();
        if (argc==4) textname = argv[3];
        else textname = "simtext";
        if (argc==5)
        {
            textname = argv[3];
            longlogname = argv[4];
            if ((longlogfile = fopen(longlogname,"rb")) != NULL)
            {
                longlogfile = NULL;
                fprintf(stderr,"Waaaa log file %s exists already!\n",longlogname);
            }
            if ((longlogfile = fopen(longlogname,"wb")) == NULL)
                fprintf(stderr,"Waaaa I can't open log file %s\n",longlogname);
        }
    }
    else
    {
        verbose=FALSE;
        if ((infile = simopen(argv[1], 0)) == -1)
        {
            fprintf (stderr, "Can't open %s\n", argv[1]);
            return -1;
        }
        doread_infile();
        if (argc==3) textname=argv[2];
        else textname = "simtext";
        if (argc==4)
        {
            textname = argv[2];
            longlogname = argv[3];
            if ((longlogfile = fopen(longlogname,"rb")) != NULL)
            {
                longlogfile = NULL;
                fprintf(stderr,"Waaaa log file %s exists already!\n",longlogname);
            }
            if ((longlogfile = fopen(longlogname,"wb")) == NULL)
                fprintf(stderr,"Waaaa I can't open log file %s\n",longlogname);
        }
    }

    word=mgetb_infile()<<8;
    word=word+mgetb_infile();
    if (debug) printf ("1st = %04x\n",word);
    if (word==0x601A) AmigaDOS=FALSE;
    else 
    {
        word=(mgetb_infile()<<8);
        word=word+mgetb_infile();
        if (debug) printf ("2st (hunk_header) = %04x\n",word);
        if (word==hunk_header) AmigaDOS=TRUE;
        else 
        {
            fprintf(stderr, "bad file format\n");
            return -1;
        }
    }

    if (AmigaDOS==FALSE)
    {
        n_text = mgetw_infile();
        n_data = mgetw_infile();
        n_bss = mgetw_infile();
        n_sym = mgetw_infile();
        mgetw_infile(); mgetw_infile();
        mgetb_infile(); mgetb_infile();
        /* read in text and data segments   */
        for (i=0; i<n_text+n_data; i++) code[i]=mgetb_infile();

        read_sym_tab(n_sym);
    }
    else
    {
        /* format of a very simple AmigaDOS load file:

           hunk_header L
           0 L
           1 L (n hunks, 2=>BSS, but none present)
           0 L
           0 L
           size/4
           hunk_code
           size of code/4 (also size_of_code == size)
           the code (size_of_code longwords)

           hunk_reloc32
           [n, hunk#, then n longwords of reloc 32] repeats until n == 0

           hunk_symbol

           [sym_len/4 sym_len bytes (sym name) value.L ] repeat until sym_len == 0
           hunk_end

        */

        int n,loop;

        n_data=0;   /* frig cos we don't use the data segment */    

        while ((n=mgetw_infile())!=0)
            for (loop=0;loop<n;loop++) mgetw_infile();    /* miss library names to load */
        if ((n=mgetw_infile())>2)
        {
            fprintf(stderr,"Bad file format (too many hunks)\n");
            return -1;
        }
        mgetw_infile();     /* first and last hunks in table... */
        mgetw_infile();
        n_text=4*mgetw_infile();
        if (n==2) n_bss=4*mgetw_infile();
        else n_bss=0;
        if (verbose) printf("code size = 0x%08x (%d)\n"
                            "BSS size = 0x%08x\n",n_text, n_text,
                            n_bss);
        else printf("Size = %08x (%d dec = %dk)\n",n_text,n_text,n_text/1024);
        if ((n=mgetw_infile())!=hunk_code)  
        {
            fprintf(stderr,"Bad file format (no code hunk)\n");
            return -1;
        }
        if ((n=4*mgetw_infile())!=n_text)
        {
            fprintf(stderr,"Bad file format (code size mismatch)\n");
            return -1;
        }
    
        for (n=0;n<n_text;n++) code[n]=mgetb_infile();

        n=mgetw_infile();
        if (n != hunk_reloc32) printf("No relocation bits\n");
        else
        {
            int xyzzy,loop;
            while ((xyzzy=mgetw_infile())!=0)
            {
                if (mgetw_infile()==0)
                {
                    if (verbose) printf("Ignoring hunk 0 reloc32 of %04x records\n",xyzzy);
                    for (loop=0;loop<xyzzy;loop++) mgetw_infile();
                }
                else
                {
                    int loop,offset,newval,*pnt;
                    if (verbose) printf("Relocating hunk 1 reloc32 of %04x records\n",xyzzy);
                    for (loop=0;loop<xyzzy;loop++)
                    {
                        offset=mgetw_infile();
                        if (verbose) printf("%08x\n",offset);
                        pnt=(int *)&code[offset];
                        newval=reverse_l(*pnt)+n_text;
                        *pnt=reverse_l(newval);
                    }
                }
            }
        }

        if (debug) printf("Symbols...\n");
        read_sym_tab_AmigaDOS();
    }

    order_symbols();

    if (n_text<1000)
    {
        pc=0; pass=1;
        if (debug) printf("Creating local labels ...\n");
        do  {
            string[0] = 0;
            instrwords = 0;
            word = readword();
            op = readinstr (word);
            writeinstr (op, word);
        } while (pc<(n_text+n_data));
    }

    if (debug) printf("Loading text...\n");

    get_text(textname);

    if (debug) printf("Restart game...\n");

    begincode = (char *)code;
    memcpy(RestartGame, code, 32000);

    pass=2;
    pc=0;
    PC=pc+memory;
    registers.A[7].L=reverse_l(pc+128000); // 0x1f400
    //signal(SIGINT, interrupt);
    //signal(SIGBUS, interrupt);
    //signal(SIGSEGV, interrupt);
    for (;;) {
        switch(setjmp(env)) {
        case 0 : break;
        case 1 :
            printf("Interrupted\n");
            break;
        case 2 : 
            printf("Segmentation fault!\n");
            break;
        case 3 : 
            printf("Bus error!\n");
            break;
        }
        printf("."); fflush(stdout);
        p = inputbuf;
        while((c = getchar()) != '\n' && c != EOF) *p++=c;
        *p='\0';
        if (c == EOF) return 0;
        inputp = inputbuf;

        switch(nextchar())
        {
        case 'l' :
            {
                int loop;
                end = getaddress();
                if (end>0) pc=end;
                end = -1;
                for (loop=0;loop<20;loop++)
                {
                    string[0] = 0;
                    instrwords = 0;
                    oldpc = pc;
                    word = readword();
                    op = readinstr (word);
                    writeinstr (op, word);
                    printf ("%-35s ", string);
                    for (i=oldpc; i<pc; i+=2)
                        printf("%04X ", code[i]*256 + code[i+1]);
                    for (i=instrwords; i<5; i++) printf ("     ");
                    for (i=oldpc; i<pc; i++) {
                        c = code[i]&0x7f;
                        if ((c>=0 && c<=31) || c==0x7f) putchar('.');
                        else putchar(c);
                    }
                    putchar('\n');
                    if (pc>MAXCODESIZE) break;
                }
            }
            break;

        case 'z' :
            {
                int loop;
                end = getaddress();
                if (end>0) pc=end;
                end = -1;
                for (loop=0;loop<20;loop++)
                {
                    string[0] = 0;
                    instrwords = 0;
                    oldpc = pc;
                    word = readword();
                    op = readinstr (word);
                    writeinstr (op, word);
                    printf ("%-35s ", string);
                    putchar('\n');
                    if (pc>MAXCODESIZE) break;
                }
            }
            break;

        case 'x' :
            {
                int c;
                c=nextchar();
                if ((c==0) || (c==10) || (c==' '))
                {
                    string[0] = 0;
                    instrwords = 0;
                    oldpc = pc;
                    pc=PC-memory;
                    word = readword();
                    op = readinstr (word);
                    writeinstr (op, word);
                    printf ("%-35s ", string);
                    for (i=oldpc; i<pc; i+=2)
                        printf("%04X ", code[i]*256 + code[i+1]);
                    for (i=instrwords; i<5; i++) printf ("     ");
                    for (i=oldpc; i<pc; i++) {
                        c = code[i]&0x7f;
                        if ((c>=0 && c<=31) || c==0x7f) putchar('.');
                        else putchar(c);
                    }
                    putchar('\n');
                    pc=oldpc;
                    displayregs();
                }
                else
                {
                    int i;
                    switch (c) {
                    case 'A':
                    case 'a':
                        i=nextchar()-'0';
                        if ((i<0) || (i>7))
                        {
                            printf("Bad register\n");
                            break;
                        }
                        else registers.A[i].L=reverse_l(getaddress());
                        break;

                    case 'D':
                    case 'd':
                        i=nextchar()-'0';
                        if ((i<0) || (i>7))
                        {
                            printf("Bad register\n");
                            break;
                        }
                        else registers.D[i].L=reverse_l(getaddress());
                        break;


                    case 'p':
                    case 'P':
                        c=nextchar();
                        if ((c=='c') || (c!='C'))
                        {
                            if ((i=getaddress()&0xFFFFFFFE)<MAXCODESIZE)
                            {
                                pc=i;
                                PC=i+memory;
                            }
                        }
                        else printf("Use xpc, xa0-7 or xd0-7\n");
                        break;

                    default:
                        printf("Use xpc, xa0-7 or xd0-7\n");
                        break;
                    }
                }
            }
            break;

        case 'P':
            {
                int oldpc,loop;
                char *oldPC;
                oldpc=pc;
                oldPC=PC;

                for (loop=traceplace;loop>0;loop--)
                {
                    pc=pctrace[loop];
                    string[0] = 0;
                    instrwords = 0;
                    PC=pc+memory;
                    word = readword();
                    op = readinstr (word);
                    writeinstr (op, word);
                    printf ("%-35s ", string);
                    printf ("%08x",pctrace[loop]);
                    putchar('\n');
                }
                pc=oldpc;
                PC=oldPC;
            }
            break;

        case 'y':
            read_sym_tab_AmigaDOS();
            order_symbols();
            break;

        case 's' :
            pc=getaddress();
            if (pc>0) PC=pc+memory;
            pc=PC-memory;

            word = readword();
            op = readinstr (word);

            PC=PC+2;                /* cos this PC not used to suss instruction */
            emulate(op,word);
            pc=PC-memory;           /* in case of jumps */
            displayregs();

            string[0] = 0;
            instrwords = 0;
            oldpc = pc;
            word = readword();
            op = readinstr (word);
            writeinstr (op, word);
            printf ("%-35s ", string);
            for (i=oldpc; i<pc; i+=2)
                printf("%04X ", code[i]*256 + code[i+1]);
            for (i=instrwords; i<5; i++) printf ("     ");
            for (i=oldpc; i<pc; i++) {
                c = code[i]&0x7f;
                if ((c>=0 && c<=31) || c==0x7f) putchar('.');
                else putchar(c);
            }
            putchar('\n');
            pc=PC-memory;           /* in case of jumps */
            break;

        case 'f' :
            {
                u_long oldA7;

                pc=getaddress();
                if (pc>0) PC=pc+memory;
                pc=PC-memory;
                end = getaddress();
                if (end != -1 && pc>end) break;
                oldA7=registers.A[7].L;     
                /* calc addr of end of inst. by fake disassemble */
                pass=1;
                word = readword();
                op = readinstr (word);
                writeinstr (op, word);
                pass=2;
                oldpc=pc;               /* where to stop */
                pc=PC-memory;
                do {
                    word = readword();
                    op = readinstr (word);
                    PC=PC+2;    /* cos this PC not used to suss instruction */
                    emulate(op,word);
                    pc=PC-memory;
                }
                while ((registers.A[7].L!=oldA7) && (pc!=oldpc));
                displayregs();

                string[0] = 0;
                instrwords = 0;
                oldpc = pc;
                word = readword();
                op = readinstr (word);
                writeinstr (op, word);
                printf ("%-35s ", string);
                for (i=oldpc; i<pc; i+=2)
                    printf("%04X ", code[i]*256 + code[i+1]);
                for (i=instrwords; i<5; i++) printf ("     ");
                for (i=oldpc; i<pc; i++) {
                    c = code[i]&0x7f;
                    if ((c>=0 && c<=31) || c==0x7f) putchar('.');
                    else putchar(c);
                }
                putchar('\n');
                pc=PC-memory;
            }
            break;

        case 't' :
            pc=getaddress();
            if (pc>0) PC=pc+memory;
            pc=PC-memory;
            end = getaddress();
            do {
                word = readword();
                op = readinstr (word);
    
                PC=PC+2;        /* cos this PC not used to suss instruction */
                emulate(op,word);
                pc=PC-memory;       /* in case of jumps */
                displayregs();

                string[0] = 0;
                instrwords = 0;
                oldpc = pc;
                word = fastreadword();
                op = readinstr (word);
                writeinstr (op, word);
                printf ("%-35s ", string);
                for (i=oldpc; i<pc; i+=2)
                    printf("%04X ", code[i]*256 + code[i+1]);
                for (i=instrwords; i<5; i++) printf ("     ");
                for (i=oldpc; i<pc; i++) {
                    c = code[i]&0x7f;
                    if ((c>=0 && c<=31) || c==0x7f) putchar('.');
                    else putchar(c);
                }
                putchar('\n');
                if (end != -1 && pc>end) break;
                pc=PC-memory;
            } while (pc<(n_text+n_data));
            break;
        case 'c' :
            pc=PC-memory;
            do {
                word = fastreadword();
                op = readinstr (word);
                PC=PC+2;                /* cos this PC not used to suss instruction */
                emulate(op,word);
                pc=PC-memory;           /* in case of jumps */
                for (i=0;i<8;i++)
                    if (pc==breakpoint[i]) 
                    {
                        printf("\nBreakpoint at ");
                        plabel(pc);
                        displayregs();
                        pc=MAXCODESIZE;
                    }
            } while (pc<MAXCODESIZE);
            pc=PC-memory;
            break;

            /* faster version of c */

        case 'C' :
            pc=PC-memory;
            for (;;)
            {
                word = fastreadword();
                op = readinstr (word);
                PC=PC+2;                /* cos this PC not used to suss instruction */
                emulate(op,word);
                pc=PC-memory;           /* in case of jumps */
            }
            break;

        case 'F' :
            pc=PC-memory;
            traceplace=0;
            saveproc(pc);
            do {
                word = fastreadword();
                op = readinstr (word);
                PC=PC+2;                /* cos this PC not used to suss instruction */
                emulate(op,word);
                pc=PC-memory;           /* in case of jumps */
                saveproc(pc);
                for (i=0;i<8;i++)
                    if (pc==breakpoint[i]) 
                    {
                        printf("\nBreakpoint at ");
                        plabel(pc);
                        displayregs();
                        pc=MAXCODESIZE;
                    }
            } while (pc<MAXCODESIZE);
            pc=PC-memory;
            break;

        case 'g' :
            pc = getaddress();
            if (pc<0) pc=0;
            PC=memory+pc;
            end = getaddress();
            do {
                word = fastreadword();
                op = readinstr (word);
                if ((end != -1)  && (pc>end)) break;
                PC=PC+2;                /* cos this PC not used to suss instruction */
                emulate(op,word);
                pc=PC-memory;           /* in case of jumps */
                for (i=0;i<8;i++)
                    if (pc==breakpoint[i]) 
                    {
                        printf("\nBreakpoint at ");
                        plabel(pc);
                        displayregs();
                        pc=MAXCODESIZE;
                    }
            } while (pc<MAXCODESIZE);
            pc=PC-memory;
            break;

            /* faster version of g */

        case 'G' :
            pc = getaddress();
            if (pc<0) pc=0;
            PC=memory+pc;
            for (;;)
            {
                word = fastreadword();
                op = readinstr (word);
                PC=PC+2;                /* cos this PC not used to suss instruction */
                emulate(op,word);
                pc=PC-memory;           /* in case of jumps */
            }
            break;

        case 'd':
            {
                int loop;

                end=getaddress();
                if (end>0) addr=end;
                end = getaddress();
                if (end<0) end=addr+16;
                for (loop=addr;loop<end;loop=loop+16)
                {
                    printf("%08X: ",loop);
                    for (i=loop;i<loop+16;i=i+2) printf("%02X%02X ",code[i],code[i+1]);
                    printf ("  *");
                    for (i=loop;i<loop+16;i++) 
                        if ((code[i] & 0x7F)<32) putchar('.');
                        else putchar(code[i]);
                    printf("*\n");
                }
                addr=loop;
            }
            break;

        case '-':
            addr=addr-32;
            pc=pc-32;
            printf("PC = %08X Addr = %08X\n",pc,addr);
            break;

        case 'b':
            {
                int i,c;
                c=nextchar();
                if (c=='?')
                {
                    for (i=0;i<8;i++) 
                        if (breakpoint[i]>0) 
                        {
                            printf("%d @ ",i);
                            plabel(breakpoint[i]);
                        }
                }
                else
                {
                    i=c&0x07;
                    breakpoint[i]=getaddress();
                    if (breakpoint[i]>0) 
                    {
                        printf("Breakpoint %d at ",i);
                        plabel(breakpoint[i]);
                    }
                    else printf("Breakpoint %d cleared\n",i);
                }
            }
            break;

        case '?':
            {
                int addr;
                addr=getaddress();
                if (addr==-1)
                {
                    printf("g: go\nt: trace\ns: step\nf: follow\n");
                    printf("G: go faster\nC: continue faster\n");
                    printf("F: continue & trace pc\n");
                    printf("x: examine regs & list instruction at PC\n");
                    printf("xpc/xAn/xDn - modify PC/An/Dn\n");
                    printf("n: examine regs & list next instruction\n");
                    printf("l: list\nd: dump as HEX and ASCII\n");
                    printf("bn: set/clear breakpoint n\n");
                    printf("-: decrement PC (a bit)\n");
                    printf("c: continue\n");
                    printf("XE <filename> - use file as input\n");
                    printf("LG <filename> - log input to file\n");
                    printf("L! close log file\n");
                    printf("W! <filename> - save object only in file\n");
                    printf("p: print stack trace\n");
                    printf("mb/w/l: modify byte/word/long\n");
                    printf("!<string> - shell out\n");
                    printf("y: load symbol table\n");
                    printf("$: dump symbol table\n");
                    printf("q: quit\n");
                }
                else plabel(addr);
            }
            break;

        case 'M':
            if (moreflag) { moreflag=FALSE; printf("More OFF\n"); }
            else  { moreflag=TRUE; printf("More ON\n"); }
            break;

        case 'm':
            {
                char *addr;
                int temp;
                char c;
                c=nextchar();
                if ((temp=getaddress())!= -1)
                {
                    addr=temp+memory;
                    switch (c&0x5F) {
                    case 'B':
                        while ((temp=getaddress())!= -1)
                            *addr++ =(char)temp&0xFF;
                        break;
                    case 'W':
                        while ((temp=getaddress())!= -1)
                        {
                            *addr++ =(char)(temp>>8)&0xFF;
                            *addr++ =(char)temp&0xFF;
                        }
                        break;
                    case 'L':
                        while ((temp=getaddress())!= -1)
                        {
                            *addr++ =(char)(temp>>24)&0xFF;
                            *addr++ =(char)(temp>>16)&0xFF;
                            *addr++ =(char)(temp>>8)&0xFF;
                            *addr++ =(char)temp&0xFF;
                        }
                        break;
                    case ('"' & 0x5F):
                        {
                            char c;
                            while ((c=nextchar())!='\0')
                                *addr++ = c;
                        }
                        break;
                    }
                }
            }
            break;
            
        case 'p' :
            {
                int i;
                printf("Stack trace at %d\n",trace_val);
                for (i=0;i<trace_val;i++)
                    plabel(trace_info[i].pc);
                printf("PC = %08X\n",PC-memory);
            }
            break;
        
        case 'X':
            {
                int c,i;
                char filename[100];

                for (i=0; (c=nextchar())!='\0'; i++) filename[i]=c;

                filename[i+1]=0;
                if ((filename[0]!='E') || (filename[1]!=' '))
                {
                    printf("Use XE <filename> to exec a script\n");
                    break;
                }

                for (i=2;filename[i]==' ';i++) ;

                if ((exec_file=fopen(&filename[i],"rb"))==NULL)
                    printf("Unable to open %s\n",&filename[i]);
            }   
            break;

        case 'L':
            {
                int c,i;
                char filename[100];

                for (i=0; (c=nextchar())!='\0'; i++) filename[i]=c;

                filename[i+1]=0;
                if ((filename[0]!='G') || (filename[1]!=' '))
                {
                    if (log_file==NULL)
                    {
                        printf("Use LG <filename> to log to a script file\n");
                        break;
                    }
                    else
                    {
                        printf("Logging terminated.\n");
                        fclose(log_file);
                        log_file=NULL;
                        break;
                    }
                }

                for (i=2;filename[i]==' ';i++) ;

                if ((log_file=fopen(&filename[i],"wb"))==NULL)
                    printf("Unable to open %s\n",&filename[i]);
            }   
            break;

        case 'W':
            {
                int c,i;
                char filename[100];
                FILE *fp;

                for (i=0; (c=nextchar())!='\0'; i++) filename[i]=c;

                filename[i+1]=0;
                if ((filename[0]!='!') || (filename[1]!=' '))
                {
                    printf("Use W! <filename> to save object\n");
                    break;
                }

                for (i=2;filename[i]==' ';i++) ;

                if ((fp=fopen(&filename[i],"wb"))==NULL)
                {
                    printf("Unable to open %s!\n",&filename[i]);
                    break;
                }
                if (fwrite(code,1,n_text+n_data,fp)!=n_text+n_data)
                    printf("Error writing %s!\n",&filename[i]);
                fclose(fp);
            }   
            break;

        case '!':
            {
                int c,i=0;
                char command[256];
                while ((c=nextchar())!=0) command[i++]=c;
                command[i]=0;
                if (i!=0) system(command);
                else printf("Use !<string> to shell out\n");
            }
            break;

        case '$':
            dump_symbols();
            break;
        case 'q' :
            return 0;

        case '\0' :
            break;

        default :
            printf("Not understood %c\n", c);
        }
    }
    return 0;
}
