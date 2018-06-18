/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2016-2017.
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

#ifdef _MSC_VER

#include <iostream>
#include <windows.h>

struct WinError
{
    WinError()
    {
        _error = GetLastError();
        _handle();
    }

    WinError(int e)
    {
        _error = e; 
        _handle();
    }

private:

    void _handle()
    {
        _format();
#ifdef _DEBUG
        OutputDebugString(buf);
#endif
        std::cerr << "Windows Error: " << buf << std::endl;
        
    }

    void _format()
    {
        DWORD buflen = sizeof(buf)/sizeof(wchar_t);

        *buf = 0;

        DWORD n = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                                0,
                                _error,
                                0,
                                buf, buflen,
                                0);

        if (!n)
        {
            /* see if we have some likely modules */
            HMODULE m1 = GetModuleHandle("WININET.DLL");
            if (m1)
            {
                n = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE,
                                  m1,
                                  _error,
                                  0,
                                  buf, buflen,
                                  0);
            }
        }
    }

    char                        buf[256];
    unsigned int                _error;
};
#else

// dummy
struct WinError {};

#endif 


