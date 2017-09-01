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

#include        <iostream>
#include        <string>
#include        "types.h"
#include        "fdbuf.h"
#include        "crc32.h"

#define SAVE_VERSION_MAJOR      1
#define SAVE_VERSION_MINOR      2

#define SAVE_TYPE_MEMIMAGE      1
#define SAVE_TYPE_ENGINE        2

// MS2s
#define SAVE_MAGIC              0x4d533273

#define AUTOSAVE_PREFIX "autosave"

struct SaveGameHeader
{
    uint         _magic;
    ushort       _versionMajor;
    ushort       _versionMinor;
    ushort       _size;
    ushort       _type;
    uint         _crc32;

    SaveGameHeader()
    {
        _magic = SAVE_MAGIC;
        _versionMajor = SAVE_VERSION_MAJOR;
        _versionMinor = SAVE_VERSION_MINOR;
        _type = 0;
        _size = 4 + 2*4 + 4;
    }

    bool write(FD& fd)
    {
        FDBuf f(fd);
        return f.putint(_magic)
            && f.putshort(_versionMajor)
            && f.putshort(_versionMinor)
            && f.putshort(_size)
            && f.putshort(_type) 
            && f.putint(_crc32)
            && f.flush();
    }

    bool read(FD& fd)
    {
        FDBuf f(fd);        
        _magic = f.getint();
        _versionMajor = f.getshort();
        _versionMinor = f.getshort();
        _size = f.getshort();
        _type = f.getshort();
        _crc32 = f.getint();
        return _magic == SAVE_MAGIC && !f._eof;
    }

    void calcCrc(const uchar* data, size_t size)
    { _crc32 = crc32(data, size); }

    bool checkCrc(const uchar* data, size_t size) const
    { return crc32(data, size) == _crc32; }
    
    bool versionOver(int m1, int m2) const
    {
        return m1 > _versionMajor || (m1 == _versionMajor && m2 >= _versionMinor);
    }
    
};

struct UndoState
{
    typedef std::string string;
    
    int         _first = 0;
    int         _last = 0;
    int         _current = 0;
    int         _maxSize = 31; // 30 levels of undo + current
    int         _suspendAutosave = 0;

    struct SuspendSave
    {
        UndoState& _host;
        SuspendSave(UndoState& host) : _host(host) { ++_host._suspendAutosave; }
        ~SuspendSave() { _host._suspendAutosave--; }
    };

    friend std::ostream& operator<<(std::ostream& os, const UndoState& u)
    {
        return os << '[' << u._first << ',' << u._current << ',' << u._last << ']';
    }

    string makeSavePath(const string& name) const
    {
        return makeSavePath(name.c_str());
    }      

    string makeSavePath(const char* name) const
    {
        string path = _path;
        if (!path.empty()) path += '/';
        
        path += name;
        return path;
    }

    void setup(const string& path);

    int size() const { return _last ? _last - _first + 1 : 0; }

    // user logical position go the other way
    int position() const { return _last - _current; }
    
    bool canAutoSave() const
    {
        // only save when current is last and not suspended
        return _current == _last && _suspendAutosave == 0;
    }

    string autosaveName(int n) const;

    int advanceLast()
    {
        int id = ++_last;
        if (!_first) _first = _last;
        _current = _last;
        
        while (size() > _maxSize)
        {
            // clean up oldest (ie first)
            if (_removeUndo(_first)) ++_first;
            else break; 
        }
        
        return id;
    }
    
    bool deltaCurrent(int delta)
    {
        int v = _current + delta;
        if (v > _last) v = _last;
        else if (v < _first) v = _first;

        bool res = v != _current;
        _current = v;
        return res;
    }

    void acceptCurrent()
    {
        // accept the current position and delete all further undos
        while (_last > _current) _removeUndo(_last--);
    }

private:

    bool _removeUndo(int n);

    // save game path
    string      _path;
    
};
