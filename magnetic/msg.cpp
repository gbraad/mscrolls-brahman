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
 *  Copyright (?) Voidware 2016-2017.
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

#if 0
// old version
void Messages::emitStoprn(const char* msg, outfn fn)
{
    int obj = get_stopron();
    if (obj)
    {
        //bool notrailingspace = (obj & 0x4000);
        obj &= 0x3fff;
        IItem ii(find_item(obj));
        if (ii)
        {
            string tn = ii.tnWord();
            emitChars(tn.c_str(), fn);
        }
        else
        {
            LOG1("MS, failed to decode msg obj ", obj << " for " << msg);
        }
    }
    else
    {
        LOG1("MS, warning missing object for @ in ;", msg);
    }
}
#endif    


struct SPContinuation
{
    SPContinuation*     _next;
    const char*         _continuation;
    int                 _instoprn;    
};

static SPContinuation* SPStack;

void Messages::_emitChars(const char* s, outfn fn)
{
    char last = 0;
    
    for (;;)
    {
        char c = *s++;
        if (!c) break;

        if (c == '@' && last == ' ')
        {
            /* when we encounter " @" we must print;
             * "THE" ITEM
             * where item is in STOPRON
             * then continue the message.
             * 
             * What we do is store any message continuation
             * set a message countdown to 2 
             * call emu to execute a virtual JSR P.TN
             *
             * countdown of 2 will cause "the" to be handled normally
             * then, when the next message is emitted, it will 
             * trigger the continuation to be printed FIRST.
             */
            if (emu_emit_stopron())
            {
                if (*s)
                {
                    SPContinuation* spc = new SPContinuation;
                    memset(spc, 0, sizeof(SPContinuation));

                    spc->_instoprn = 2;
                    spc->_continuation = s;
                    
                    spc->_next = SPStack;
                    SPStack = spc;
                }
                return;
            }
        }
        else (*fn)(c);

        last = c;
    }
}

void Messages::emitChars(const char* msg, outfn fn)
{
    if (SPStack)
    {
        // wait for the countdown to hit zero
        // then, if there is a continuation, emit it.
        SPContinuation* spc = SPStack;
        assert(spc->_instoprn);
        if (!--spc->_instoprn)
        {
            const char* s = spc->_continuation;
            spc->_continuation = msg;
            msg = s;
        }
    }

    _emitChars(msg, fn);

    while (SPStack && !SPStack->_instoprn)
    {
        SPContinuation* spc = SPStack;
        const char* s = spc->_continuation;
        
        SPStack = spc->_next;
        delete spc;
        
        _emitChars(s, fn);
    }
}

void Messages::emitMsg(int n, const char* s, outfn fn)
{
    if (_hook)
    {
        string altm = (_hook)(n, s);  // IFMagnetic hooks our messages
        if (!altm.empty())
        {
            // allow return to change message, using %s to embed original
            const char* m = altm.c_str();
            while (*m)
            {
                if (*m != '%') (*fn)(*m++);
                else
                {
                    char c = *++m;
                    if (c == 's') emitChars(s, fn);
                    else (*fn)(c);
                    ++m;
                }
            }
            return;
        }
    }

    emitChars(s, fn);
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
