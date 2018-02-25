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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "random.h"
#include "aesctr.h"
#include "rc4.h"

#define USE_ALGO_AES    0
#define USE_ALGO_RC4    1

extern "C"
{
#include "sha1.h"
}

#define VERSION  0

static AESCTR aes;
static RC4 rc4;
static int debug;
static unsigned int algo;

static const char* algoName[] =
{
    "AES256",
    "RC4"
};

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

void bitcopy(unsigned char* d, unsigned int d0,
             unsigned char* s, unsigned int s0,
             unsigned int sz)
{
    // copy sz bits from s, bit offset `s0` to `d`, bit offset `d0`
    // NB: sizes are in bits.
    
    d += d0>>3; d0 &= 7;
    s += s0>>3; s0 &= 7;

    unsigned int r;

    while (sz)
    {
        r = sz;
        if (8 - s0 < r) r = 8 - s0;
        if (8 - d0 < r) r = 8 - d0;
        assert(r > 0 && r <= 8);

        int m = (1<<r)-1;
        int t = (*s >> s0) & m;
        *d = ((*d) & ~(m << d0)) | (t << d0);
        s0 += r;
        if (s0 >= 8) { ++s; s0 -= 8; }
        d0 += r;
        if (d0 >= 8) { ++d; d0 -= 8; }
        sz -= r;
    }
}

bool solveHash(uint32_t* key, unsigned char* hash)
{
    uint32_t i = 0;

    for (;;)
    {
        unsigned char thash[20];
        key[0] = i;
        
        SHA1_CTX ctx;
        sha1_init(&ctx);
        sha1_update(&ctx, (unsigned char*)key, 8);
        sha1_final(&ctx, thash);
        
        if (!memcmp(thash, hash, 20)) return true; // found!
        
        //if (i && !(i & 0xfffff)) printf("scanning %d...\n", i);

        if (!++i) break; // wrapped
    }
    return false;
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

        int version = 0;

        if (!ok)
            printf("Files does not contain embedded data\n");
        else
        {
            // get version
            version = getc(in);
            
            if (version > VERSION)
            {
                printf("file contains a newer version than this program!\n");
                ok = false;
            }
        }

        unsigned char keyhash[20];
        unsigned char hash[20];
        unsigned char* key;
        unsigned int keysize; // bits
        unsigned int keybytes;
        
        if (ok)
        {
            if (version != VERSION)
                printf("embedded data version %d, current %d\n",
                       version, VERSION);
            
            algo = getc(in);
            unsigned int work = getc(in);

            assert(algo <= USE_ALGO_RC4);
            
            printf("Work factor %d, cipher %s\n", work, algoName[algo]);

            if (algo == USE_ALGO_AES)
            {
                // read IV
                for (int i = 0; i < AES_BLOCK_SIZE; ++i) aes._iv[i] = getc(in);

                key = aes._key;
                keysize = aes._keysize;
            }
            else if (algo == USE_ALGO_RC4)
            {
                key = rc4.SBox;
                keysize = 2048;
            }

            keybytes = keysize/8;
            
            // read a hash of the key for verification
            for (int i = 0; i < sizeof(keyhash); ++i) keyhash[i] = getc(in);

            // number of hashes of `work` bit key segments
            int m = keysize/work;

            // will build up the key as we decode hashes
            memset(key, 0, keybytes);

            if (!debug) printf("Progress  0.0%%");

            uint32_t k[2];
            k[0] = 0;
            for (int i = 0; i < m; ++i)
            {
                k[1] = k[0];
                
                for (int j = 0; j < sizeof(hash); ++j) hash[j] = getc(in);

                ok = solveHash(k, hash);
                if (!ok)
                {
                    printf("failure to solve hash. FAILED!\n");
                    break;
                }
                
                if (debug) printf("K(%d)=%08X\n", i, k[0]);
                
                bitcopy(key, i*work, (unsigned char*)k, 0, work);

                if (debug)
                {
                    printf("Progress %4.1f%%\n", i*100.0/m);
                }
                else
                {
                    printf("\b\b\b\b\b%4.1f%%", i*100.0/m);
                    fflush(stdout);
                }
            }
            
            if (!debug) printf("\n");
        }

        if (ok)
        {
            if (debug)
            {
                for (int i = 0; i < keybytes; ++i) printf("%02X,", key[i]);
                printf("\n");
            }
            
            // verify solved key
            SHA1_CTX ctx;
            sha1_init(&ctx);
            sha1_update(&ctx, key, keybytes);
            sha1_final(&ctx, hash);
            if (memcmp(hash, keyhash, sizeof(keyhash)))
            {
                printf("Reconstructed key not correct. FAILED!\n");
                ok = false;
            }
        }

        if (ok)
        {
            // now we have the correct key
            aes.start();
            
            int mused = ftell(in) - pos;

            // remaining data
            sz -= mused;
        }
        else
        {
            // rewind and reject
            fseek(in, pos, 0);
            return false;
        }
    }

    while (sz--)
    {
        int c = getc(in);
        if (algo == USE_ALGO_AES) c = c ^ aes.next();
        else if (algo == USE_ALGO_RC4) c = c ^ rc4.next();
        putc(c, out);
    }

    return true;
}

void insertDat(FILE* out, FILE* dat, int work)
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
    putc(algo, out);    // which cipher
    putc(work, out);    // work factor

    printf("encoding work factor %d, cipher %s\n", work, algoName[algo]);

    bool v;
    unsigned char* key;
    unsigned int keysize; // bits
    unsigned int keybytes;
    
    if (algo == USE_ALGO_AES)
    {
        // generate a random IV
        // this will encrypt the actual data

        key = aes._key;
        keysize = aes._keysize;
        v = randombytes_sysrandom_buf(aes._iv, AES_BLOCK_SIZE);
        assert(v);
        
        // emit IV
        for (int i = 0; i < AES_BLOCK_SIZE; ++i) putc(aes._iv[i], out);

    }
    else if (algo == USE_ALGO_RC4)
    {
        key = rc4.SBox;
        keysize = 2048;
    }

    keybytes = keysize/8;

    // make random key
    v = randombytes_sysrandom_buf(key, keybytes);
    assert(v);

    int m = keysize/work; // number of key segments, each of `work` bits
    
    uint32_t hashin[2];
    unsigned char hashout[20];
    hashin[0] = 0;

    // any remainder bits of key not included in `m` segments, clear
    bitcopy(key, m*work, (unsigned char*)hashin, 0, keysize - m*work);

    if (debug)
    {
        for (int i = 0; i < keybytes; ++i) printf("%02X,", key[i]);
        printf("\n");
    }
    
    // emit a hash of the key so that the decoder can verify
    SHA1_CTX ctx;
    sha1_init(&ctx);
    sha1_update(&ctx, key, keybytes);
    sha1_final(&ctx, hashout);
    for (int i = 0; i < sizeof(hashout); ++i) putc(hashout[i], out);

    aes.start();

    for (int i = 0; i < m; ++i)
    {
        hashin[1] = hashin[0];
        hashin[0] = 0;
        
        // copy `work` bits from the key ready for hash
        bitcopy((unsigned char*)hashin, 0, key, i*work, work);

        if (debug) printf("K(%d)=%08X\n", i, hashin[0]);

        // 20 byte hash of this key segment + last key segment
        sha1_init(&ctx);
        sha1_update(&ctx, (unsigned char*)hashin, sizeof(hashin));
        sha1_final(&ctx, hashout);

        // emit hash
        for (int j = 0; j < sizeof(hashout); ++j) putc(hashout[j], out);
    }

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
            int a = buf[i];
            if (algo == USE_ALGO_AES) a = a ^ aes.next();
            else if (algo == USE_ALGO_RC4) a = a ^ rc4.next();
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

void scan(FILE* in, FILE* out, FILE* dat, int work)
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
                if (debug) printf("SOI\n");
                break;
            case 0xE0:
                if (debug) printf("APP0\n");
                insdata = true;
                break;
            case 0xE1: if (debug) printf("APP1\n"); break;
            case 0xE2: if (debug) printf("APP2\n"); break;
            case 0xE3: if (debug) printf("APP3\n"); break;
            case 0xE4: if (debug) printf("APP4\n"); break;
            case 0xDA:
                if (debug) printf("SOS\n");
                break;
            case 0xD9: if (debug) printf("EOI\n"); break;
            case 0xDB: if (debug) printf("DQT\n"); break;
            case 0xC4: if (debug) printf("DHT\n"); break;
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
                if (debug) printf("%02X\n", c);
            }

            if (insdata && !donedata)
            {
                if (dat && out) insertDat(out, dat, work);
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

    char* infile = 0;
    char* outfile = 0;
    char* dfile = 0;

    int work = 20;
    
    algo = USE_ALGO_AES; // default

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (!strcmp(argv[i], "-w") && i < argc-1)
            {
                work = atoi(argv[++i]);
                if (work < 20 || work > 32)
                {
                    printf("work factor 20 <= w <= 32\n");
                    return 0;
                }
            }
            else if (!strcmp(argv[i], "-rc4"))
            {
                algo = USE_ALGO_RC4;
            }
            else if (!strcmp(argv[i], "-d"))
                debug = 1;
            else
            {
                printf("unrecognised option '%s'\n", argv[i]);
                infile = 0; // show usage
                break;
            }
        }
        else
        {
            if (!infile) infile = argv[i];
            else if (!outfile) outfile = argv[i];
            else if (!dfile) dfile = argv[i];
        }
    }

    if (!infile)
    {
        printf("Usage %s in-file [-d] [-rc4] [-w workfactor] [out-file [data-file]]\n\n", argv[0]);

        printf("to put a file into a JPEG:\n");
        printf("%s template-jpg output.jpg file.dat\n\n", *argv);
        printf("to extract from a jpeg:\n");
        printf("%s source.jpg file.dat\n", *argv);
        return 0;
    }

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
        
        scan(in, out, dat, work);
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
