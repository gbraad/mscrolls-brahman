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


#include <QObject>
#include <QString>
#include <QDebug>
#include <QColor>

#include <string>

#include "qdefs.h"
#include "qcontrol.h"
#include "transcript.h"
#include "braschema.h"
#include "logged.h"
#include "mdfilter.h"
#include "qaccess.h"

#define QREGISTER_TRANSCRIPT  QREGISTER(QTranscript)

class QTranscript:
    public QObject,
    public Transcript::Notifier
{
public:

    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged);
    Q_PROPERTY(QString textHTML READ textHTML NOTIFY textChanged);
    Q_PROPERTY(QString customControl READ customControl WRITE setCustomControl NOTIFY customControlChanged);
    Q_PROPERTY(QColor linkColor READ linkColor WRITE setLinkColor NOTIFY textChanged);
    Q_PROPERTY(QColor cechoColor READ cechoColor WRITE setCEchoColor NOTIFY textChanged);    

    typedef std::string string;

public:

    QTranscript() 
    {
        // share the transcript with the api controller
        _transcript = QControl::theControl()->_transcript;
        _transcript->_notifier = this;
        if (_transcript->notifyPending()) addedText(0, _transcript->text());
    }
   
    ~QTranscript()
    {
        _transcript->_notifier = 0;
        delete _qac;
    }

    QString text() const
    {
        return QSTR(_transcript->text());
    }

    QString textHTML() const
    {
        return QSTR(_transcript->textHTML());
    }

    QString customControl() const
    {
        return QSTR(_transcript->customJSON());
    }

    void setCustomControl(const QString&)
    {
        // ignore string and reset
        _transcript->resetCustomJSON();
        customControlChanged();
    }

    QColor linkColor() const 
    {
        QColor col(_transcript->_linkColor);
        return col;
    }

    void setLinkColor(const QColor& col) 
    {
        uint c = col.rgba();
        if (c != _transcript->_linkColor)
        {
            _transcript->_linkColor = c;
            emit textChanged();
        }
    }

    QColor cechoColor() const 
    {
        QColor col(_transcript->_consoleEchoColor);
        return col;
    }

    void setCEchoColor(const QColor& col) 
    {
        uint c = col.rgba();
        if (c != _transcript->_consoleEchoColor)
        {
            _transcript->_consoleEchoColor = c;
            emit textChanged();
        }
    }

    // from notifier
    void changed(int what) override
    {
#if 0
        if (what == BRA_SEGMENT_TITLE)
        {
            // title bar changed
            emit titleTextChanged();
        }
#endif

        if (what == BRA_SEGMENT_JSON)
        {
            emit customControlChanged();
        }
        else
        {
            // body of transcript changed
            emit textChanged();
        }
    }

    void addTTS(char c)
    {
        if (strchr(">#*_", c)) c = 0;  // dump markup chars
        if (c) _tts += c;
    }

    void addedText(int what, const string& s) override
    {
        // notification when new text is added.
        if (!_qac)
        {
            // create on demand
            _qac = new QAccess();
        }
        
        if (*_qac)
        {
            MDFilter f;
            f._pos = s.c_str();
            
            f.skipSpace();
            for (;;)
            {
                const char* p = f._pos;
                if (!*p) break;
                if (f.skipMarkup())
                {
                    ++p; // skip '['
                    while (*p && *p != ']') addTTS(*p++);
                }
                else if (*p == '{')  // dump {foo:bar} syntax
                {
                    while (*p)
                    {
                        if (*p == '}')
                        {
                            f._pos = ++p;
                            break;
                        }
                        ++p;
                    }
                }
                else
                {
                    addTTS(*f._pos);
                    f.advance();
                }
            }
            
            LOG4("TTS text: '", _tts << "'");
            _qac->speak(_tts);
            _tts.clear();
        }
        
    }

    Q_INVOKABLE void setWidth(int w)
    {
        if (w != _transcript->width())
        {
            _transcript->setWidth(w);
            //emit textChanged();  
        }
    }

    Q_INVOKABLE void clear()
    {
        // this does not emit a textchanged
        _transcript->clear();
    }

    Transcript::Ref     _transcript;
    QAccess*            _qac = 0;
    string              _tts;

signals:

    void textChanged();
    void customControlChanged();
};




