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
 *  Copyright (©) Voidware 2016-2017.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License (LGPL) as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 *  for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  contact@voidware.com
 *
 */

#include <stdio.h>
#include <ctype.h>
#include "msg.h"
#include "strutils.h"
#include "puzzle.h"

// so that C functions can get it
static Messages* theMsg;

bool Messages::start(const char* filename)
{
    int res = false;
    if (filename)
    {
        string fname;
        
        fname = changeSuffix(filename, ".text");
        res = readFromTextFile(fname.c_str());

        if (!res)
        {
            fname = changeSuffix(filename, ".msg");
            res = readFromMsgFile(fname.c_str());
        }

        // for C-linkage
        if (res) theMsg = this;

    }
    return res;
}

void Messages::emitMsg(int n, const char* s, outfn fn)
{
    if (_hook)
    {
        const char* m = (_hook)(n);  // IFMagnetic hooks our messages
        if (m)
        {
            // allow return to change message, using %s to embed original
            while (*m)
            {
                if (*m != '%') (*fn)(*m++);
                else
                {
                    char c = *++m;
                    if (c == 's')
                    {
                        while (*s) (*fn)(*s++);
                    }
                    else (*fn)(c);
                    ++m;
                }
            }
            return;
        }
    }
    
    while (*s) (*fn)(*s++);
}

bool Messages::readFromTextFile(const char* filename)
{
    // format of message file:
    // one line per message
    bool res = false;

    // open binary to be the same whether DOS or unix
    FILE* fp = fopen(filename, "rb");
    if (fp)
    {
        int i = 0;
        res = true;

        _line = 1;

        string s;
        int c;
        do
        {
            c = _getc(fp);
            if (c == '\n' || c == EOF)
            {
                // completed a line
                Msg m;
                m._index = i;
                m._text = s;
                ++i;
                _messages.insert(m);
                s.clear();
            }
            else s += c;
            
        } while (c != EOF);

        LOG3("read ", i << " messages from " << filename);
        fclose(fp);
    }
    else
    {
        LOG1("Can't open message file '", filename << "'");
    }
    return res;
}

bool Messages::readFromMsgFile(const char* filename)
{
    // format of message file:
    // <number><spaces or comma>"text"  ignore rest of line
    bool res = false;

    // open binary to be the same whether DOS or unix
    FILE* fp = fopen(filename, "rb");
    if (fp)
    {
        int i = 0;
        res = true;

        _line = 1;
        int cc = 0;
        
        for (;;)
        {
            if (readNumber(fp, i) && i >= 0)
            {
                int c;
                c = getc(fp);
                if (u_isspace(c) || c == ',')
                {
                    std::string text;
                    if (readString(fp, text) && text.size() > 0)
                    {
                        Msg m;
                        m._index = i;
                        m._text = text;

                        _messages.insert(m);
                        ++cc;
                    }
                    else LOG("Expected string, line ", _line);
                }
                else LOG("Expected comma or space, line ", _line);

                // skip to end of line
                nextLine(fp);
            }
            else
            {
                if (_getc(fp) == EOF) break;
                else 
                {
                    LOG("Expected number line, ", _line);
                    nextLine(fp);
                }
            }
        }

        LOG3("read ", cc << " messages from " << filename);
        fclose(fp);
    }
    else
    {
        LOG1("Can't open message file '", filename << "'");
    }
    return res;
}

// C linkage
int msg_write_string(int n, outfn fn)
{
    // NB: game thread
    Messages* ms = theMsg;
    if (ms)
    {
        const Messages::Msg* m = ms->find(n);
        if (m)
        {
            ms->emitMsg(n, m->_text.c_str(), fn);
            return 1;  // handled
        }
    }
    return 0;
}
