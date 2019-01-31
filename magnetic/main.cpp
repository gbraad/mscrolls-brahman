/****************************************************************************\
 *
 * Magnetic - Magnetic Scrolls Interpreter.
 *
 * Written by Niclas Karlsson <nkarlsso@abo.fi>,
 *            David Kinder <davidk.kinder@virgin.net>,
 *            Stefan Meier <Stefan.Meier@if-legends.org> and
 *            Paul David Doherty <pdd@if-legends.org>
 *
 * Copyright (C) 1997-2008  Niclas Karlsson
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 *
 *     Simple ANSI interface main.c
 *
\****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "msg.h"

extern "C"
{
#include "defs.h"
};



//#define WIDTH 78
#define WIDTH 0 // disable wrap


extern int handle_test_commands(const char* cmd);


type8 buffer[80], xpos = 0, bufpos = 0, log_on = 0, ms_gfx_enabled;
FILE *logfile1 = 0, *logfile2 = 0;
char filename[256];

#ifdef STANDALONE
type8 ms_load_file(type8s *name, type8 *ptr, type16 size, unsigned int pos)
{
    FILE *fh;
    char* realname;

    if (name)
        realname = (char*)name;
    else
    {
        do
        {
            printf("Filename: ");
        }
        while (!fgets(filename, sizeof(filename), stdin));
        realname = filename;
    }
    if (!(fh=fopen(realname,"rb")))
        return 1;
    if (fread(ptr,1,size,fh) != size)
        return 1;
    fclose(fh);
    return 0;
}

type8 ms_save_file(type8s *name, type8 *ptr, type16 size, unsigned int pos)
{    

    FILE *fh;
    char* realname;

    if (name)
        realname = (char*)name;
    else
    {
        do
        {
            printf("Filename: ");
        }
        while (!fgets(filename, sizeof(filename), stdin));
        realname = filename;
    }
    if (!(fh = fopen(realname,"wb")))
        return 1;
    if (fwrite(ptr,1,size,fh) != size)
        return 1;
    fclose(fh);
    return 0;
}
#endif

void script_write(type8 c)
{
    if (log_on == 2 && fputc(c,logfile1) == EOF)
    {
        printf("[Problem with script file - closing]\n");
        fclose(logfile1);
        log_on = 0;
    }
}

void transcript_write(type8 c)
{
    if (logfile2 && c == 0x08 && ftell(logfile2) > 0)
        fseek(logfile2,-1,SEEK_CUR);
    else if (logfile2 && fputc(c,logfile2) == EOF)
    {
        printf("[Problem with transcript file - closing]\n");
        fclose(logfile2);
        logfile2 = 0;
    }
}

#ifdef STANDALONE
void ms_statuschar(type8 c)
{
    static type8 x=0;

    if (c == 0x09)
    {
        while (x + 11 < WIDTH)
        {
            putchar(0x20);
            x++;
        }
        return;
    }
    if (c == 0x0a)
    {
        x = 0;
        putchar(0x0a);
        return;
    }
    printf("\x1b[32m%c\x1b[31m",c);
    x++;
}

void ms_flush(void)
{
    type8 j;

    if (!bufpos)
        return;
    if (WIDTH > 0 && xpos + bufpos > WIDTH)
    {
        putchar(0x0a);
        transcript_write(0x0a);
        xpos = 0;
    }
    for (j = 0; j < bufpos; j++)
    {
        if (buffer[j] == 0x0a)
            xpos = 0;
        if (buffer[j] == 0x08)
            xpos -= 2;
        putchar(buffer[j]);
        transcript_write(buffer[j]);
        xpos++;
    }
    bufpos = 0;
}

void ms_putchar(type8 c)
{
    /*
      if (c == 0x08)
      {
      if (bufpos > 0)
      bufpos--;
      return;
      }
    */
    buffer[bufpos++] = c;
    if ((c == 0x20) || (c == 0x0a) || (bufpos >= 80))
        ms_flush();
}

static void _print_word(Word* w)
{
    printf("(%d) %s\n", w->index, w->name);
}

static void _print_item(Item* item)
{
    size_t i;
    printf("(%d)", item->word.index);
    
    for (i = 0; i < ITEM_MAX_ADJ; ++i)
    {
        int a = item->adj[i];
        if (!a) break;
        if (a <= adjectiveCount)
            printf(" %s", adjectives[a-1].name);
        else
            printf(" (adj %d unknown)", a);
    }

    printf(" %s\n", item->word.name);
}

static void _dump_dict()
{
    size_t i;

    printf("%d adjectives:\n", adjectiveCount);
    for (i = 0; i < adjectiveCount; ++i)
    {
        _print_word(adjectives + i);
    }
    
    printf("%d items:\n", itemCount);
    for (i = 0; i < itemCount; ++i)
    {
        _print_item(items + i);
    }
}

type8 ms_getchar(type8 trans)
{
    static char buf[256];
    static type16 pos=0;
    int c;
    type8 i;

    if (!pos)
    {
        /* Read new line? */
        i = 0;
        while (1)
        {
            if (log_on == 1)
            {
                /* Reading from logfile */
                if ((c = fgetc(logfile1)) == EOF)
                {
                    /* End of log? - turn off */
                    log_on = 0;
                    fclose(logfile1);
                    c = getchar();
                }
                else printf("%c",c); /* print the char as well */
            }
            else
            {
                c = getchar();
            }
            if (c == '#' && !i && trans)
            {
                /* Interpreter command? */
                if (log_on == 1)
                {
                    while ((c = fgetc(logfile1)) != '\n' && c != '\r' && c != EOF && i < 255)
                    {
                        buf[i++] = c;
                        printf("%c",c);
                    }
                }
                else
                {
                    while ((c = getchar()) != '\n' && c != '\r' && c != EOF && i < 255)
                    {
                        buf[i++] = c;
                        printf("%c",c);
                    }
                }
                buf[i] = 0;
                c = '\n'; /* => Prints new prompt */
                printf("\n");
                i = 0;
                if (!strcmp(buf,"logoff") && log_on == 2)
                {
                    printf("[Closing script file]\n");
                    log_on = 0;
                    fclose(logfile1);
                }
                else if (!strcmp(buf,"undo"))
                    c = 0;
                else if (!strcmp(buf,"dict"))
                {
                    _dump_dict();
                }
                else if (!handle_test_commands(buf))
                    printf("[Nothing done]\n");
            }
            //}
            script_write((type8)c);
            if (c != '\n' && c != '\r')
                transcript_write((type8)c);
            if (c == '\n' || c == '\r' || c == EOF || i == 255)
                break;
            buf[i++] = c;
            if (!c)
                break;
        }
        buf[i] = '\n';
    }
    if ((c = buf[pos++]) == '\n' || !c || c == '\r'){
        pos = 0;
        buf[0]=0x00;
    }
    return (type8)c;
}

void ms_showpic(type32 c, type32 picAddr, type8 mode, type8 ver, float* profile)
{
    /* Insert your favourite picture viewing code here
       mode: 0 gfx off, 1 gfx on (thumbnails), 2 gfx on (normal) */

    printf("Display picture [%d]\n",c);
    if (ver == 2)
        printf("Display picture at [%04X]\n",picAddr);  

    /* Small bitmap retrieving example */

    /*
      {
      type16 w, h, pal[16];
      type8 *raw = 0, i;

      raw = ms_extract(c,&w,&h,pal,0);
      printf("\n\nExtract: [%d] %dx%d",c,w,h);
      for (i = 0; i < 16; i++)
      printf(", %3.3x",pal[i]);
      printf("\n");
      printf("Bitmap at: %8.8x\n",raw);
      }
    */
}
#endif // STANDALONE


void ms_fatal(type8s *txt)
{
    fputs("\nFatal error: ",stderr);
    fputs((char*)txt,stderr);
    fputs("\n",stderr);
    ms_status();
    exit(1);
}

type8 ms_showhints(struct ms_hint * hints)
{
    return 0;
}

#ifdef STANDALONE

void ms_playmusic(type8 * midi_data, type32 length, type16 tempo)
{
}


static char msgbuf[4096];
static char* mbp;

static void emit_char(type8 c);

static void emit_string(const char* s)
{
    while (*s) *mbp++ = *s++;
}

static void emit_char(type8 c)
{
    if (c == 0x0a) emit_string("\\n");
    else if (c == '"') emit_string("\\\"");
    else *mbp++ = c;
}

static void emit_message_table(int n)
{
    for (int i = 0; i < n; ++i)
    {
        mbp = msgbuf;
        mbp += sprintf(mbp, "%d, \"", i);
        if (decode_string(i, emit_char))
        {
            sprintf(mbp, "\"\n");
            printf("%s", msgbuf);
        }
        else
        {
            fprintf(stderr, "Skipping %d: '%s'\n", i, msgbuf);
        }
    }
}

int main(int argc, char **argv)
{
    char *gamename = 0, *gfxname = 0, *hintname = 0;
    type8 running;
    type32 dlimit, slimit;
    int i;

    int decode_msg = 0;

    if (sizeof(type8) != 1 || sizeof(type16) != 2 || sizeof(type32) != 4)
    {
        fprintf(stderr,
                "You have incorrect typesizes, please edit the typedefs and recompile\n"
                "or proceed on your own risk...\n");
        exit(1);
    }
    dlimit = slimit = 0xffffffff;
    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (!strcmp(argv[i], "-makemsg") && i < argc-1)
            {
                // number of messages to decode
                decode_msg = atoi(argv[++i]);
                continue;
            }
            
            switch (tolower(argv[i][1]))
            {
            case 'd':
                if (strlen(argv[i]) > 2)
                    dlimit = atoi(&argv[i][2]);
                else
                    dlimit = 0;
                break;
            case 's':
                if (strlen(argv[i])>2)
                    slimit = atoi(&argv[i][2]);
                else
                    slimit = 655360;
                break;
            case 't':
                if (!(logfile2 = fopen(&argv[i][2],"w")))
                    printf("Failed to open \"%s\" for writing.\n",&argv[i][2]);
                break; 
            case 'r':
                if (logfile1 = fopen(&argv[i][2],"r"))
                    log_on = 1;
                else
                    printf("Failed to open \"%s\" for reading.\n",&argv[i][2]);
                break;
            case 'w':
                if (logfile1 = fopen(&argv[i][2],"w"))
                    log_on = 2;
                else
                    printf("Failed to open \"%s\" for writing.\n",&argv[i][2]);
                break;
            default:
                printf("Unknown option -%c, ignoring.\n",argv[i][1]);
                break;
            }
        }
        else if (!gamename)
            gamename = argv[i];
        else if (!gfxname)
            gfxname = argv[i];
        else if (!hintname)
            hintname = argv[i];
    }
    if (!gamename)
    {
        printf("Magnetic 2.3 - a Magnetic Scrolls interpreter\n\n");
        printf("Usage: %s [options] game [gfxfile] [hintfile]\n\n"
               "Where the options are:\n"
               " -dn    activate register dump (after n instructions)\n"
               " -rname read script file\n"
               " -sn    safety mode, exits automatically (after n instructions)\n"
               " -tname write transcript file\n"
               " -wname write script file\n\n"
               "The interpreter commands are:\n"
               " #undo   undo - don't use it near are_you_sure prompts\n"
               " #logoff turn off script writing\n\n",argv[0]);
        exit(1);
    }

    Messages msg;
    if (msg.start(gamename))
    {
        printf("read external message file\n");
    }
    else
    {
        printf("did not read external message file\n");
    }

    if (!(ms_gfx_enabled = ms_init((type8s*)gamename,(type8s*)gfxname,(type8s*)hintname,0)))
    {
        printf("Couldn't start up game \"%s\".\n",gamename);
        exit(1);
    }
    ms_gfx_enabled--;

    if (decode_msg > 0)
    {
        emit_message_table(decode_msg);
    }
    else
    {
        running = 1;
        while ((ms_count() < slimit) && running)
        {
            if (ms_count() >= dlimit)
                ms_status();
            running = ms_rungame();
        }
        if (ms_count() == slimit)
        {
            printf("\n\nSafety limit (%d) reached.\n", (int)slimit);
            ms_status();
        }
    }
    ms_freemem();
    if (log_on)
        fclose(logfile1);
    if (logfile2)
        fclose(logfile2);
    //printf("\nExiting.\n");
    return 0;
}

void game_state_notify(int movecount) {}
void update_game_save_area(unsigned char* ptr, size_t size) {}
void ms_undo_signal() {}
void update_game_save_area(unsigned char* ptr, size_t size, unsigned int addr) {}

#endif // STANDALONE


