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

#include <string>

#include "qdefs.h"
#include "qcontrol.h"

#define QREGISTER_CONSOLE  QREGISTER(QConsole)

class QConsole: public QObject, public Console::Notifier
{
public:

    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged);

    typedef std::string string;
    
public:

    QConsole()
    {
        // share the console with the api controller
        _console = QControl::theControl()->_console;
        _console->_notifier = this;
    }

    ~QConsole()
    {
        _console->_notifier = 0;
    }

    QString text() const
    {
        return QSTR(_console->text());
    }

    Q_INVOKABLE void appendText(const QString& text)
    {
        string s = trim(STRQ(text));
        if (!s.empty())
            _console->appendText(s);
    }

    // from notifier
    void changed() override
    {
        emit textChanged();
    }

    Q_INVOKABLE QString lineAt(int pos) const
    {
        return QSTR(_console->lineAt(pos));
    }

    Console::Ref     _console;

signals:

    void textChanged();
};




