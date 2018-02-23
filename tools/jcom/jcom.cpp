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

void insertDat(FILE* out, FILE* dat)
{
    int msize = 0xffff - 2;
    char* buf = new char[msize];

    size_t n;
    do
    {
        n = fread(buf, 1, msize, dat);
        if (n > 0)
        {
            putc(0xff, out);
            putc(0xfe, out); // com
            putLen(n + 2, out);

            for (int i = 0; i < n; ++i)
            {
                putc(buf[i], out);
            }

            printf("wrote APP %d bytes\n", n);
        }
    } while (n == msize);

    delete [] buf;
}

void scan(FILE* in, FILE* out, FILE* dat)
{
    int c;
    bool esc = false;
    bool insdata = false;
    bool donedata = false;
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
                    assert(l >= 2);
                    l -= 2;
                    printf("COM size %d\n", l);

                    while (l--)
                    {
                        int c = getc(in);
                        if (out && !dat)
                        {
                            putc(c, out);
                        }
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
