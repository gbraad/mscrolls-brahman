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
 *  contact@strandgames.com
 *
 */


#pragma once

#include <QQuickImageProvider>
#include <assert.h>

#include "apngreader.h"
#include "strutils.h"
#include "qdefs.h"
#include "logged.h"


class AnimImageProvider : public QQuickImageProvider
{
public:

    typedef std::string string;

    ApngReader*  _reader = 0;
    uint         _index = 0;
    uint         _offset = 0;
    string       _filename;

    AnimImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}
    ~AnimImageProvider() { drop(); }

    void drop()
    {
        delete _reader;
        _reader = 0;
    }

    void reset()
    {
        drop();
        _index = 0;
        _offset = 0;
    }
    
    bool animated() const
    {
        return _reader && _reader->isAnimated();
    }

    QImage requestImage(const QString &name, QSize *size,
                        const QSize &requestedSize)
    {
        // anim/ + count/name

        string reqName = STRQ(name);
        
        char buf[32];
        const char* p = reqName.c_str();
        char* q = buf;
        while (u_isdigit(*p)) *q++ = *p++;
        *q = 0;

        if (*p == '/') ++p;
        if (startsWithIgnoreCase(p, "file:///")) p += 8;

        reqName = p;

        if (startsWithIgnoreCase(reqName, "qrc:///"))
        {
            // qrc:/// -> :/
            reqName = string(":") + reqName.substr(5, string::npos);
        }

        uint reqIndex = atoi(buf);

        //LOG3("animimageprovider request for ", reqName << " index " << reqIndex)
        bool ok = true;
        
        if (_filename != reqName)
        {
            // changed names?
            LOG3("animimageprovider, reset for ", reqName);
            reset();
            _filename = reqName;
        }

again:
        
        // logical index
        _index = reqIndex - _offset;

        if (!_index)
        {
            drop();
            
            _reader = new ApngReader();
            
            if (!_reader->init(_filename))
            {
                LOG1("animimageprovider, init failed ", _filename);
                ok = false;
            }
        }
        else
        {
            if (_reader && _reader->valid())
            {
                uint nf = _reader->frames();
                if (nf < 2 || _index >= nf-1)
                {
                    //LOG3("last frame in ", _filename);
                    
                    _offset = reqIndex;
                    goto again;
                }
            }
            else
            {
                LOG1("animimageprovider, expect index 0 first! ", _filename);
                ok = false;
            }
        }

        if (ok) 
        {
            if (!_reader->readFrame(_index))
            {
                LOG1("animimageprovider, readframe failed ", _index << " " << _filename);
                ok = false;
            }
        }

        if (ok)
        {
            //LOG3("APNG read frame ", _index);

            int w = _reader->_lastImg.width();
            int h = _reader->_lastImg.height();

            if (size) *size = QSize(w, h);
            
            if (requestedSize.isValid())
            {
                int rw = requestedSize.width();
                int rh = requestedSize.height();

                if (rw != w || rh != h)
                {
                    // need to scale, but makes it worse!
                    //LOG3("animImageProvider, scaling image ", _filename << "(" << _index << ") " << w << "x" << h << " to " << rw << "x" << rh);
                    //return _reader->_lastImg.scaled(rw,rh);
                }
            }

            return _reader->_lastImg;
        }
         
        return QImage(); // null
    }

};
