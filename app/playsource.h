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

#include <math.h>

#include <QIODevice>
#include <QObject>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QFile>
#include "strutils.h"
#include "logged.h"
#include "qdefs.h"

class PlayerSource : public QIODevice
{
    Q_OBJECT

public:

    typedef std::string string;

    PlayerSource(QObject* parent, QAudioOutput* output = 0) : QIODevice(parent)
    {
        _audioOutput = output;
    }

    // interface
    virtual ~PlayerSource() 
    {
        delete _audioOutput;
    }
    virtual bool start(QIODevice* push) = 0;
    virtual void stop() = 0;
    virtual void loops(int l) {}
    virtual void fade(int ms) {}
    virtual qint64 readData(char *data, qint64 maxlen) { return 0; }
    virtual qint64 writeData(const char *data, qint64 len) { return 0; }
    virtual qint64 bytesAvailable() const  = 0;
    virtual bool playing() const = 0;

    bool setSource(const string& filename)
    {
        bool r = false;

        // fix url formatted file path
        if (startsWithIgnoreCase(filename, "file:///"))
        {
            _sourceFile = filename.substr(8); // file prefix
        }
        else if (startsWithIgnoreCase(filename, "qrc:///"))
        {
            // for some reason qrc:/// doesnt work but :/ does ?
            _sourceFile = ":/" + filename.substr(7); // resource prefix
        }
        else
        {
            _sourceFile = filename;
        }

        if (!_sourceFile.empty())
        {
            _source.setFileName(QSTR(_sourceFile));
            r = _source.open(QIODevice::ReadOnly); 
            if (!r)
            {
                LOG1("PlayerSource, can't open '", _sourceFile << "'");
            }
            else
            {
                LOG4("PlayerSource, opened ", _sourceFile);
            }

        }
        return r;
    }

protected:

    QAudioOutput*   _audioOutput = 0;
    string          _sourceFile;
    QFile           _source;

};


