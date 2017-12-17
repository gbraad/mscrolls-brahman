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

#include "puzzle.h"
#include "logged.h"
#include "ifmagnetic.h"

// IFmagnetic dependent functions (see puzzle.h)

void PuzzleManager::text(const char* s)
{
    // emit text to game
    assert(_host);

    //LOG3("puzzle text: '", s << "'");
    while (*s) _host->emitchar(*s++);
}

bool PuzzleManager::action(const char* s)
{
    // game perform action
    assert(_host);

    LOG3("puzzle action: '", s << "'");
    bool res = _host->evalCommand(s, 0, true); // echo
    if (res)
    {
        // sync and flush
        _host->emitScene();
    }
    return res;
}

bool PuzzleManager::goroom(int n)
{
    LOG3("goroom ", n);

    bool res = false;
    IItem cr = IItem::currentRoom();

    // we prefer to go NESW rather than diagonal where possible
    // as it looks nicer
    static const int dirTable[] = 
    {
        IItem::dir_n,
        IItem::dir_e,
        IItem::dir_s,
        IItem::dir_w,
        IItem::dir_ne,
        IItem::dir_se,
        IItem::dir_sw,
        IItem::dir_nw,
    };

    int d;
    int ti = 0;
    for (;;)
    {
        if (ti == DIM(dirTable))
        {
            // after the table do any other directions possible
            if (d < IItem::dir_count) ++d;
            else break; // all done
        }
        else d = dirTable[ti++];
        
        IItem di = cr.getExit(d);
        if (di.roomNumber() == (uint)n)
        {
            res = action(IItem::dirName(d)); // go NW etc.
            if (res)
            {
                // check we are in the expected room
                if (get_current_room() != n)
                {
                    //LOG3("goroom, failed to go ", IItem::dirName(d) << " to " << n);
                    res = false;
                }
            }
            break;
        }
    }

    return res;
}



