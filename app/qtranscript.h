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

#define QREGISTER_TRANSCRIPT  QREGISTER(QTranscript)

class QTranscript: public QObject, public Transcript::Notifier
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
    }
   
    ~QTranscript()
    {
        _transcript->_notifier = 0;
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

    Q_INVOKABLE void setWidth(int w)
    {
        if (w != _transcript->width())
        {
            _transcript->setWidth(w);
            //emit textChanged();  
        }
    }

    Transcript::Ref     _transcript;

signals:

    void textChanged();
    void customControlChanged();
};




