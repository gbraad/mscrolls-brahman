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

/* deal with all file things portably */

#pragma once

#include <string>
#include <vector>

#ifdef _MSC_VER
#include <io.h>
#include <direct.h> // _mkdir
#include "direntwin.h"

#pragma warning (disable: 4996)  // open deprecated
#define lseek64 _lseeki64
#define mkdir _mkdir

typedef int mode_t;

static const mode_t S_ISUID      = 0x08000000;           ///< does nothing
static const mode_t S_ISGID      = 0x04000000;           ///< does nothing
static const mode_t S_ISVTX      = 0x02000000;           ///< does nothing
static const mode_t S_IRUSR      = mode_t(_S_IREAD);     ///< read by user
static const mode_t S_IWUSR      = mode_t(_S_IWRITE);    ///< write by user
static const mode_t S_IXUSR      = 0x00400000;           ///< does nothing

static const mode_t S_IRGRP      = mode_t(_S_IREAD);     ///< read by *USER*
static const mode_t S_IWGRP      = mode_t(_S_IWRITE);    ///< write by *USER*
static const mode_t S_IXGRP      = 0x00080000;           ///< does nothing
static const mode_t S_IROTH      = mode_t(_S_IREAD);     ///< read by *USER*
static const mode_t S_IWOTH      = mode_t(_S_IWRITE);    ///< write by *USER*
static const mode_t S_IXOTH      = 0x00010000;           ///< does nothing

// missing
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)

#else

// arrange off_f to be 64 bits
#define _FILE_OFFSET_BITS 64

#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#define lseek64 lseek

#endif

#ifdef __CYGWIN__
// 64bit might not work under cygwin
//#undef lseek64
//#define lseek64 lseek
#endif


#include        <stdio.h>
#include        <sys/stat.h>  // open modes
#include        <fcntl.h>
#include        "types.h"
#include        "winerr.h" // dummy for non windows

struct FDBase
{
    static const int fd_read = O_RDONLY;
    static const int fd_write = O_WRONLY;
    static const int fd_wr = O_RDWR;
    static const int fd_create = O_CREAT;
    static const int fd_trunc = O_TRUNC;
    static const int fd_append = O_APPEND;
    static const int fd_new = fd_wr | fd_create | fd_trunc;

    typedef int64 Pos;

    int                 _fd;

    FDBase() { _init(); }
    
    virtual ~FDBase() {}

    bool isOpen() const { return _fd >= 0; }

    // interface
    virtual void close() = 0;
    virtual bool read(unsigned char* buf, size_t amt, size_t& nread) = 0;
    virtual bool write(const unsigned char* buf, size_t amt, size_t& nwrote)  = 0;

protected:

    void _init() { _fd = -1; }
};

struct FD: public FDBase
{
    Pos                 _pos;

    FD() { _init(); }

    ~FD() { close(); }

    operator bool() const { return isOpen(); }

    // compliance
    bool open(const char* path, int flags = fd_read) 
    {
        close();

        mode_t mode = S_IRUSR | S_IWUSR;
 
#ifdef _WIN32
        /* even though the data files are text, we must open in binary
         * so that the logical offsets match the physical offsets.
         */
        flags |= O_BINARY;
#else
        // these are present for mingw but not always, so leave them out.
        mode |= S_IRGRP | S_IROTH | S_IWGRP | S_IWOTH;
#endif

        _fd = ::open(path, flags, mode); 
        bool res = isOpen();
        if (!res) WinError();
        return res;
    }

    static bool exists(const char* path)
    {
        // exists 
        struct stat sbuf;
        return stat(path, &sbuf) == 0;
    }

    static bool exists(const std::string& path) { return exists(path.c_str()); }
    bool open(const std::string& path, int flags = fd_read)
    { return open(path.c_str(), flags); }
    
    static bool existsFile(const char* path)
    {
        // exists and is a file
        struct stat sbuf;
        return stat(path, &sbuf) == 0 && S_ISREG(sbuf.st_mode);
    }

    static bool existsFile(const std::string& path)
    { return existsFile(path.c_str()); }

    static bool existsDir(const char* path)
    {
        // exists and is a directory
        struct stat sbuf;
        return stat(path, &sbuf) == 0 && S_ISDIR(sbuf.st_mode);
    }

    static bool touch(const char* path)
    {
        int fd = ::open(path, fd_wr | fd_create, 0666);
        if (fd >= 0) ::close(fd);
        return fd >= 0;
    }

    static time_t mtime(const char* path)
    {
        time_t t = 0; 
        struct stat sbuf;
        if (stat(path, &sbuf) == 0)
        {
            t = sbuf.st_mtime;
        }
        return t;
    }

    static bool mkdir(const char* path)
    {
#ifdef _WIN32
        return ::mkdir(path) == 0;
#else
        return ::mkdir(path,S_IRWXU | S_IRWXG | S_IRWXO) == 0;
#endif
    }

    static bool mkdirIf(const char* path)
    {
        // if not already there, create it
        return existsDir(path) || mkdir(path);
    }

    static bool remove(const char* path)
    {
        return !unlink(path);
    }

    static bool rename(const char* oldname, const char* newname)
    {
        return ::rename(oldname, newname) == 0;
    }

    static bool renameBak(const char* name)
    {
        char* bak = new char[strlen(name) + 2];
        strcat(strcpy(bak, name), "~");

        // if already, remove it.
        remove(bak);

        bool res = rename(name, bak);
        delete [] bak;
        return res;
    }

    static bool getDirectory(const char* path, std::vector<std::string>& files)
    {
        bool res = false;
        
        DIR* dp = opendir(path);
        res = dp != 0;
        if (res)
        {
            struct dirent *ep;
            while ((ep = readdir(dp)) != 0) files.push_back(ep->d_name);
        }
        return res;
    }

    // compliance
    void close() override
    {
        if (isOpen()) 
        {
            ::close(_fd); 
            _init();
            FDBase::_init();
        }
    }

    Pos size() const
    {
        Pos sz = 0;
        if (isOpen())
        {
            sz = lseek64(_fd, 0, SEEK_END);
            if (sz == -1 || lseek64(_fd, _pos, SEEK_SET) == -1)
            {
                // error
                sz = 0;
            }
        }
        return sz;
    }

    bool seek(Pos pos)
    {
        bool res = isOpen();
        if (res && pos != _pos)
            _pos = lseek64(_fd, pos, SEEK_SET);

        return res;
    }

    bool truncate()
    {
        // at current _pos
        bool res = isOpen();
        if (res)
        {
#ifdef _WIN32
            res = _chsize(_fd, _pos) == 0;
#else
            // posix
            res = ftruncate(_fd, _pos) == 0;
#endif
        }
        return res;
    }

    // compliance
    bool read(unsigned char* buf, size_t amt, size_t& nread) override
    {
        // true if no error, nread returns bytes actualty read.
        // nread can be zero.
        nread = 0;
        bool res = isOpen();
        if (res)
        {
            nread = ::read(_fd, buf, amt);
            res = nread != (size_t)-1;
            if (res) _pos += nread;
        }
        return res;
    }

    bool read(unsigned char* buf, size_t amt)
    {
        // return if no error and read amt bytes
        size_t nr;
        return read(buf, amt, nr) && nr == amt;
    }

    // compliance
    bool write(const unsigned char* buf, size_t amt, size_t& nwrote) override
    {
        nwrote = 0;
        if (!amt) return true;
        
        bool res = isOpen();
        if (res)
        {
            nwrote = ::write(_fd, buf, amt);
            res = nwrote == amt;
            if (nwrote != (size_t)-1) _pos += nwrote;
        }
        return res;
    }

    bool write(const unsigned char* buf, size_t amt)
    {
        size_t nw;
        return write(buf, amt, nw);
    }

    unsigned char* readAll(Pos* fsize = 0, bool removeDOS = false)
    {
        // removeDOS true removes all '\r' from buffer
        
        unsigned char* data = 0;
        
        _pos = 0;
        Pos sz = size(); // 0 if closed
        if (sz)
        {
            data = new unsigned char[sz + 1];
            if (data)
            {
                if (read(data, sz))
                {
                    if (removeDOS)
                    {
                        unsigned char* p = data;
                        unsigned char* q = p;
                        for (Pos s = sz; s; --s)
                        {
                            if (*p != '\r') *q++ = *p;
                            else --sz;
                            ++p;
                        }
                    }
                    data[sz] = 0; 
                }
                else
                {
                    // problem with read
                    delete data;
                    data = 0;
                    sz = 0;
                }
            }
        }

        if (fsize) *fsize = sz;

        return data;
    }

    bool writeAll(const unsigned char* data, size_t sz = 0)
    {
        // NB: suitable only for text files, unless size given
        if (!sz) sz = strlen((const char*)data);
        return seek(0) && write(data, sz) && truncate();
    }
    
private:

    Pos _tell() const
    {
        Pos p = 0;
        if (isOpen())
            p = lseek64(_fd, 0, SEEK_CUR);

        return p;
    }

    void _init()
    {
        _pos = 0;
    }
};

