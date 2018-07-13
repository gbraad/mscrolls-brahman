/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2017.
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
 *  contact@voidware.com
 */

#pragma once

#include "fdbuf.h"
#include "lz.h"

#define WRITEZ_VERSION 0

bool writez(FDBuf& fb, const unsigned char* buf, size_t amt, size_t& nwrote)
{
    nwrote = 0;

    Compress cp;
    cp.compress(buf, amt);

    uint version = WRITEZ_VERSION;
    uint usize = amt;
    uint csize = cp._outsz;

    uint startpos = fb._pos;

    // write the compress header
    // high bit set the first byte (with the version) so that it also
    // marks the data as binary. In cases where we can have either a
    // compressed version or plain text, this first byte will act as
    // as a marker for the compressed format.
    fb.putc(version + 0x80);
    fb.putint(usize);
    fb.putint(csize);

    // size of the above data
    uint headSize = fb._pos - startpos;
    
    // now write compressed data
    bool res = fb.write(cp._outbuf, csize, nwrote) && nwrote == csize;
    if (res) nwrote += headSize;

    return res;
}

bool writez(FD& fd, const unsigned char* buf, size_t amt, size_t& nwrote)
{
    FDBuf fb(fd);
    return writez(fb, buf, amt, nwrote);
}

inline bool writez(FD& fd, const unsigned char* buf, size_t amt)
{
    size_t nw;
    return writez(fd, buf, amt, nw);
}

unsigned char* readz(FDBuf& fb, size_t* fsize = 0)
{
    uint startpos = fb._pos;

    // read header
    uint version = fb.getc();

    if (version == (uint)-1 || version < 0x80)
    {
        // not binary. seek back to before the header and return fail
        // the caller could use this to fallback to reading a text version
        LOG3("readz, text file?", "");
        fb._fd->seek(startpos);
        return 0; // fail
    }

    version &= 0x7f; 

    if (version > WRITEZ_VERSION)
    {
        LOG1("readz newer version", version);
        return 0; // fail
    }
    
    uint usize = fb.getint();
    uint csize = fb.getint();
    //uint headSize = fb._pos - startpos;

    bool res = csize != (uint)-1;
    unsigned char* data = 0;

    if (res)
    {    
        //LOG3("ZHeader; compress size: ", csize << ", uncompress size:" << usize);
        uchar* buf = new unsigned char[csize];
        size_t nread;
        res = fb.read(buf, csize, nread) && csize == nread;
            
        if (res)
        {
            Compress cp;
            cp.decompress(buf, csize, usize);
            data = cp.donate(fsize);
        }
        delete [] buf;
    }
    return data;
}

unsigned char* readz(FD& fd, size_t* fsize = 0)
{
    FDBuf fb(fd);
    return readz(fb, fsize);
}
