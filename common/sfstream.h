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

#pragma once

#include <string.h>
#include "sstream.h"

struct StdFStream: public StdStream
{
    typedef std::string string;

    // Constructors
    StdFStream()
    {
        _fp = 0;
        _also = 0;
    }
    
    ~StdFStream() { close(); }

    const char* filename() const override { return _filename.c_str(); }
    bool ok() const override { return _fp != 0; }
    
    bool open(const char* filename, const char* mode = "w")
    {
        close();
        _filename = filename;
        _fp = fopen(filename, mode);
        return _fp != 0;
    }

    void close()
    {
        if (_fp)
        {
            fclose(_fp);
            _fp = 0;
        }
    }

    void setAlso(std::ostream* os)
    {
        _also = os;
    }


    bool _emit(StdStreamBuf* buf) override
    {
        bool r = false;
        if (_fp)
        {
            size_t nw = buf->size();
            r = (fwrite(*buf, 1, buf->size(), _fp) == nw) && !fflush(_fp);
            
            if (!r)
            {
                // failed, signal error
                close();
            }
        }
        
        if (_also)
        {
            _also->write(*buf, buf->size());
            _also->flush();
        }
        
        return r;
    }

protected:


    string                                      _filename;
    FILE*                                       _fp;

    // optional addition output (eg cout)
    std::ostream*                               _also;

};

