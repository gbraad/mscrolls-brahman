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
 *  Copyright (c) Strand Games 2021.
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


#include <QObject>
#include <QString>
#include <QLibrary>
#include <QDebug>

#include <string>
#include "qdefs.h"
#include "logged.h"

#define QREGISTER_QACCESS  QREGISTER(QAccess);

#define TAG_ACCESS      "Access, "

class QAccess: public QObject
{
public:

    Q_OBJECT

    typedef std::string string;
    typedef std::wstring wstring;

    typedef int error_status_t;
    typedef error_status_t _cdecl nvda0Fn();
    typedef error_status_t _cdecl nvdaStringFn(const wchar_t*);

    nvda0Fn* nvdaController_testIfRunning = 0;
    nvdaStringFn* nvdaController_speakText = 0;
    bool ttsMode = false;
    error_status_t _error = 0;

public:    

    QAccess()
    {
        _init();
    }

    operator bool() const { return ttsMode; }
    
    Q_INVOKABLE void speak(const QString& s)
    {
        if (ttsMode)
        {
            //qDebug() << "speak " << s << "\n";
            speak(s.toStdWString());
        }
    }

    void speak(const std::string& s)
    {
        std::wstring ws( s.length(), L' ' );
        std::copy(s.begin(), s.end(), ws.begin() );
        speak(ws);
    }

    void speak(const std::wstring& ws)
    {
        if (ttsMode)
        {
            assert(nvdaController_speakText);
            nvdaController_speakText(ws.c_str());
        }
    }

private:

    void _init()
    {
        LOG2(TAG_ACCESS, "Trying to load NVDA");

        QLibrary nvda("nvdaControllerClient64");
        if (nvda.load())
        {
            LOG1(TAG_ACCESS, "loaded NVDA");

            nvdaController_testIfRunning = (nvda0Fn*)nvda.resolve("nvdaController_testIfRunning");
            nvdaController_speakText = (nvdaStringFn*)nvda.resolve("nvdaController_speakText");

            if (nvdaController_testIfRunning && nvdaController_speakText)
            {
                error_status_t v = (*nvdaController_testIfRunning)();
                if (!v)
                {
                    LOG1(TAG_ACCESS, "NVDA is running");
                    ttsMode = true;
                }
                else
                {
                    LOG1(TAG_ACCESS "NVDA is not running ", v);
                }
            }
            else
            {
                LOG1(TAG_ACCESS, "NVDA can't locate NVDA functions");
                _error = 1; 
            }
        }
        else
        {
            LOG1(TAG_ACCESS, "FAILED to load NVDA");
        }
    }
};

    
