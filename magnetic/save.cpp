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

#include <iostream>
#include <algorithm>
#include <QDir>
#include <QStringList>
#include "save.h"
#include "strutils.h"
#include "qdefs.h"
#include "logged.h"
#include "fd.h"

using namespace std;

// OLD NAME
#define AUTOSAVE_NAME_OLD "autosave.sav"

void UndoState::setup(const string& path)
{
    _path = path; // path prefix for auto save game

    QDir d(QSTR(_path));

    QStringList nameFilter;
    nameFilter << AUTOSAVE_PREFIX ".*";

    std::vector<int> suffs;

    bool oldauto = false;
    
    QStringList fnames = d.entryList(nameFilter, QDir::Files);
    for (int i = 0; i < fnames.size(); ++i)
    {
        string fi = STRQ(fnames.at(i));
        
        // legacy save name
        if (equalsIgnoreCase(fi, AUTOSAVE_NAME_OLD)) oldauto = true;
        
        string si = suffixOf(fi);
        const char* sip = si.c_str() + 1; // skip "."
        if (isDigits(sip)) suffs.push_back(atoi(sip));
    }

    size_t n = suffs.size();
    if (n)
    {
        std::sort(suffs.begin(), suffs.end());
        
        _last = suffs[n-1];
        _first = _last;
        _current  = _last;
        
        // adjust first to be least contiguous value below last
        int i = n-1;
        while (i > 0)
        {
            if (suffs[--i] == _first - 1) _first--;
            else break;
        }
    }

    if (oldauto)
    {
        LOG1("undo, renaming legacy autosave ", AUTOSAVE_NAME_OLD);
        if (FD::rename(makeSavePath(AUTOSAVE_NAME_OLD).c_str(),
                       makeSavePath(autosaveName(_last + 1)).c_str()))
            advanceLast();
    }
}

string UndoState::autosaveName(int n) const
{
    if (!n) n = _current;
    
    char buf[64];
    sprintf(buf, AUTOSAVE_PREFIX ".%d", n);
    return buf;
}

bool UndoState::_removeUndo(int n)
{
    string p = makeSavePath(autosaveName(n));
    bool res = FD::remove(p.c_str());

    if (res)
    {
        LOG4("undo, removed old save ", p);
    }
    else
        LOG("undo, unable to delete old save file ", p);

    return res;
}

