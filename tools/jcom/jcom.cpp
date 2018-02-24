/*
 *
 *
 *   ______              _
 *   | ___ \            | |
 *   | |_/ / _ __  __ _ | |__   _ __ ___    __ _  _ __
 *   | ___ \| '__|/ _` || '_ \ | '_ ` _ \  / _` || '_ \
 *   | |_/ /| |  | (_| || | | || | | | | || (_| || | | |
 *   \____/ |_|   \__,_||_| |_||_| |_| |_| \__,_||_| |_|
 *
 *
 *  "The creative principle which lies realized in the whole world"
 *
 *  Copyright (c) Strand Games 2018.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 * 
 *  contact@strandgames.com
 */
 

#include <stdio.h>
#include <assert.h>
#include "random.h"
#include "rc4.h"

#define VERSION  0

static RC4 rc4;

int getLen(FILE* in)
{
    int a = getc(in);
    int b = getc(in);
    return b + (a<<8);
}

void putLen(int v, FILE* out)
{
    int a = v >> 8;
    int b = v & 0xff;
    putc(a, out);
    putc(b, out);
}

bool extractDat(FILE* in, FILE* out, size_t sz, int cc)
{
    // extract block# `cc` of  `sz` bytes

    if (!cc)
    {
        // this is the prologue
        int pos = ftell(in);

        // check magic
        bool ok = getc(in) == 'J';
        ok = ok && (getc(in) == 'C');
        ok = ok && (getc(in) == 'O');
        ok = ok && (getc(in) == 'M');

        if (!ok)
            printf("Files does not contain embedded data\n");
        else
        {
            // get version
            int v = getc(in);
            
            if (v > VERSION)
            {
                printf("file contains a newer version than this program!\n");
                ok = false;
            }
            else
            {
                printf("embedded data version %d, current %d\n", v, VERSION);

                // read key
                char key[256];
                for (int i = 0; i < sizeof(key); ++i) key[i] = getc(in);
                memcpy(rc4.SBox, key, sizeof(key));

                int mused = ftell(in) - pos;

                // remaining data
                sz -= mused;
            }
        }

        if (!ok)
        {
            // rewind and reject
            fseek(in, pos, 0);
            return false;
        }
    }

    while (sz--)
    {
        int c = getc(in) ^ rc4.next();
        putc(c, out);
    }

    return true;
}

void insertDat(FILE* out, FILE* dat)
{
    char buf[0xffff-2]; // max COM payload
    
    // insert prologue
    
    putc(0xff, out);
    putc(0xfe, out); // com

    int pos = ftell(out);
    putLen(0, out); // dummy

    putc('J', out);
    putc('C', out);
    putc('O', out);
    putc('M', out);

    putc(VERSION, out);

    char key[256];
    bool v = randombytes_sysrandom_buf(key, sizeof(key));
    assert(v);

    for (int i = 0; i < sizeof(key); ++i) putc(key[i], out);
    memcpy(rc4.SBox, key, sizeof(key));

    int mused = ftell(out) - pos;
    int msize = 0xffff - mused;
    int cc = 0;

    size_t n;
    
    for (;;)
    {
        n = fread(buf, 1, msize, dat);
        if (!n) break; // done
        
        if (cc)
        {
            putc(0xff, out);
            putc(0xfe, out); // com
            putLen(n + 2, out);
        }
        else
        {
            // go back and correct first block size
            int here = ftell(out);
            int v = fseek(out, pos, 0);
            assert(!v);
            assert(n + mused <= 0xffff);
            putLen(n + mused, out);
            
            v = fseek(out, here, 0);
            assert(!v);
        }

        for (int i = 0; i < n; ++i)
        {
            int a = buf[i] ^ rc4.next();
            putc(a, out);
        }

        ++cc;
        printf("(%d) wrote APP %d bytes\n", cc, n + mused);

        if (n < msize) break; 

        // subsequent blocks max size
        mused = 2;
        msize = 0xffff - mused;
    }
}

void scan(FILE* in, FILE* out, FILE* dat)
{
    int c;
    bool esc = false;
    bool insdata = false;
    bool donedata = false;
    int ec = 0;
    
    while ((c = getc(in)) != EOF)
    {
        if (esc)
        {
            esc = false;

            switch (c)
            {
            case 0:
                break; // ignore
            case 0xD8:
                printf("SOI\n");
                break;
            case 0xE0:
                printf("APP0\n");
                insdata = true;
                break;
            case 0xE1: printf("APP1\n"); break;
            case 0xE2: printf("APP2\n"); break;
            case 0xE3: printf("APP3\n"); break;
            case 0xE4: printf("APP4\n"); break;
            case 0xDA:
                printf("SOS\n");
                break;
            case 0xD9: printf("EOI\n"); break;
            case 0xDB: printf("DQT\n"); break;
            case 0xC4: printf("DHT\n"); break;
            case 0xFE:
                {
                    int l = getLen(in);
                    printf("COM size %d\n", l);
                    assert(l >= 2);
                    l -= 2;
                    
                    bool v = false;
                    
                    if (out && !dat && ec >= 0)
                    {
                        v = extractDat(in, out, l, ec++);
                        if (!v)
                        {
                            // reject for all subsequent
                            ec = -1;
                        }
                    }

                    if (!v)
                    {
                        // skip comment
                        while (l--) getc(in);
                    }
                    continue;
                }
                break;
            default:
                printf("%02X\n", c);
            }

            if (insdata && !donedata)
            {
                if (dat && out) insertDat(out, dat);
                donedata = true;
            }

            if (dat && out)
            {
                putc(0xff, out);
                putc(c, out);
            }
        }
        else
        {
            if (c == 0xff) esc = true;
            else
            {
                if (dat && out) putc(c, out);
            }
        }
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage %s in-file [out-file [data-file]]\n\n", argv[0]);

        printf("to put a file into a JPEG:\n");
        printf("%s template-jpg output.jpg file.dat\n\n", *argv);
        printf("to extract from a jpeg:\n");
        printf("%s source.jpg file.dat\n", *argv);
        return 0;
    }
    
    char* infile = argv[1];
    char* outfile = 0;
    char* dfile = 0;

    if (argc > 2) outfile = argv[2];
    if (argc > 3) dfile = argv[3];
    
    FILE* in = fopen(infile, "rb");
    FILE* out = 0;
    FILE* dat = 0;

    if (in)
    {
        if (outfile)
        {
            out = fopen(outfile, "wb");
            if (!out)
            {
                printf("can't open output '%s'\n", outfile);
                fclose(in);
                return -1;
            }
        }

        if (dfile)
        {
            dat = fopen(dfile, "rb");
            if (!dat)
            {
                printf("Can't open data file '%s'\n", dfile);
                fclose(out);
                fclose(in);
                return -1;
            }
        }
        
        scan(in, out, dat);
        fclose(in);

        if (dat) fclose(dat);
        if (out) fclose(out);
    }
    else
    {
        printf("can't open input file '%s'\n", infile);
    }
    return 0;
}
