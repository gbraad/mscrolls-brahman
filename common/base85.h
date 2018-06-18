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

#include <string>
#include <memory>
#include <assert.h>

struct Base85
{
    typedef std::string string;
    typedef std::unique_ptr<char> Ptr;
    typedef std::unique_ptr<unsigned char> DPtr;

    static Ptr encode(const unsigned char* buf, int amt)
    {
        char* dbuf = 0;
        if (amt)
        {
            // calculate deist buffer size +1 for zero terminator
            
            int dsize = (amt/4)*5;
            int f = amt & 3;
            if (f) dsize += f + 1;
            ++dsize;
            
            dbuf = new char[dsize];
            _encode_asc85(dbuf, dsize, buf, amt);
            assert(dbuf[dsize-1] == 0);
        }
        return Ptr(dbuf);
    }

    static DPtr decode(const char* buf, int& amt)
    {
        int n = strlen(buf);
        unsigned char* dbuf = 0;

        if (n)
        {
            int t = n/5;
            int dsize = t*4;
            t = n - t*5;
            if (t) dsize += t - 1;

            dbuf = new unsigned char[dsize];

            amt = _decode_asc85(dbuf, dsize, buf);
            assert(amt == dsize);
        }

        return DPtr(dbuf);
    }

    static string encodeToString(const unsigned char* buf, int amt)
    {
        return encode(buf, amt).get();
    }

    static DPtr decodeString(const string& s, int& amt)
    {
        return decode(s.c_str(), amt);
    }

private:

    /* Functions taken from Ascii85 RFC 1924
     * from https://github.com/woolstar/test/tree/master/encode
     */

    static char ascii85(int i)
    {
        static const char _ascii85[]= "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~" ;
        return _ascii85[i];
    }

    static unsigned int dascii85(int i)
    {
        static const unsigned char _dascii85[128]= {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0, 62, 0, 63, 64, 65, 66, 0, 67, 68, 69, 70, 0, 71, 0, 0,
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 72, 73, 74, 75, 76,
            77, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
            25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0, 0, 0, 78, 79,
            80, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
            51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 81, 82, 83, 84, 0 } ;

        return _dascii85[i];
    }

    static const char* _pack_a85( unsigned long lval)
    {
        static char result[6] ;
        char * pfill ;
        int istep ;

        if ( ! lval ) return "." ;

        pfill= result +5 ;
        *( pfill --)= '\0' ;

        for ( istep= 5 ; ( istep -- ) ; ) {
            *( pfill --)= ascii85(lval % 85);
            lval /= 85 ;
        }

        return result ;
    }

    static const char* _pack_a85e( unsigned long lval)	// pack without '.' shortcut
    {
        static char result[6] ;
        char * pfill ;
        int istep ;

        pfill= result +5 ;
        *( pfill --)= '\0' ;

        for ( istep= 5 ; ( istep -- ) ; ) {
            *( pfill --)= ascii85(lval % 85);
            lval /= 85 ;
        }

        return result ;
    }

    
    static void _encode_asc85(char * zbuf, int asz,
                       const unsigned char * asrc, int alen)
    {
        char c, * zfill ;
        const char * p ;
        unsigned long lval ;
        int istep ;

        if ( ! zbuf || ! asrc || ! asz ) return ;	// bad pointer, or no room for even a null terminator
        asz -- ;  // pre-reserve space for null

        for (zfill= zbuf ; (( alen & ~3 ) && ( asz >= 5 )) ; alen -= 4 )
        {
            lval= * (asrc ++) ;  lval <<= 8 ; lval |= * (asrc ++) ;
            lval <<= 8 ;
            lval |= * (asrc ++) ;  lval <<= 8 ; lval |= * (asrc ++) ;

            for ( p= _pack_a85( lval); ( c= *( p ++) ) ; -- asz )
            { *( zfill ++)= c ; }
        }

        if ( alen && ! ( alen & ~3 ) && ( asz >= ( alen +1 )))	// there's a fragment left, and still space
        {
            // pack 1,2 or 3 bytes
            lval= * (asrc ++) * (1L << 24) ;
            switch ( alen )
            {
            case 3:
                lval |= *( asrc ++) << 16 ;
                lval |= *( asrc ++) << 8 ;
                break ;
            case 2:
                lval |= *( asrc ++) << 16 ;
                break ;
            }

            for ( istep= alen +1, alen= 0, p= _pack_a85e( lval) ; ( istep -- && ( *(zfill ++)= *(p ++) )) ; -- asz ) { }
        }

        if ( alen ) {  // src data left over
            if ( ! asz && ( zfill > zbuf ) ) { -- zfill ; }	// back up one to fit the error marker
            *( zfill ++)= '/' ;
        }

        * zfill= '\0' ;
    }

    static unsigned long _unpack_a85x( const char * astr, int * alen )	// pack with fragment tracking
    {
        long lval ;
        int istep, itrim ;

        for (istep= 5, lval= 0; ( * astr && istep ) ; istep -- )
        { lval *= 85, lval += dascii85(0x7f & * ( astr ++)) ; }
        if ( alen ) { * alen= ( 5 - istep ) ; }
        if ( istep ) {
            // trim out for short messages
            for ( itrim= 0 ; ( istep -- ) ; itrim += 8 ) { lval *= 85 ;  lval += 84 ; }
        }

        return lval ;
    }

    static int _decode_asc85(unsigned char * zdest, int asz, const char * asrc)
    {
        int istep, ival ;
        unsigned long lval ;
        unsigned char * zfill= zdest ;

        if ( ! zdest || ! asz || ! asrc ) return -1 ;

        while ( asz && * asrc )
        {
            if ( '.' == * asrc ) {
                asrc ++ ;
                for ( istep= 4 ; ( istep -- && asz ) ; -- asz ) { *(zfill ++)= 0 ; }
                continue ;
            }

            lval= _unpack_a85x( asrc, &ival ) ;
            asrc += ival ;
            if ( ival < 2 ) return -1 ;
            for ( istep= 32, ival -- ; ( istep && ival && asz ) ; -- ival, -- asz ) { istep -= 8 ;  *(zfill ++)= 0xff & ( lval >> istep ) ; }
        }

        return ( zfill - zdest ) ;
    }
    
};
