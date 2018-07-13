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

#include <QAbstractListModel>
#include <QDebug>

#include <vector>
#include <string>
#include <assert.h>
#include "qdefs.h"
#include "control.h"
#include "logged.h"
#include "strutils.h"
#include "utils.h"

class QWordsModel : public QAbstractListModel
{
    Q_OBJECT

    typedef std::string string;
    typedef QAbstractListModel parentT;

    
    Control::Words      _words;
    Control*            _control;
    
public:

    enum CatRoles
    {
        NameRole = Qt::UserRole + 1,
    };

    QWordsModel(Control* control) : QAbstractListModel(0)
    {
        _control = control;
    }

    int size() const
    {
        return (int)_words.size();
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        int n = size();
        //LOG3("qword model size, ", n);
        return n;
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (index.isValid())
        {
            int ix = index.row();

            if (ix < size())
            {
                switch (role)
                {
                case NameRole:
                    //LOG3("wordmodel, request for index ", ix);
                    return QSTR(_words[ix]);
                    break;
                }
            }
        }
        return QVariant();
    }

    Q_INVOKABLE void seedText(const QString& t)
    {
        assert(_control);
        _control->suggestCompletion(STRQC(t), _words);
        changed();
    }

    Q_INVOKABLE void forceSuggestion(const QString& t)
    {
        _words.insert(_words.begin(), STRQ(t));
        changed();
    }

    Q_INVOKABLE QString addSuggestion(const QString& base,
                                      const QString& word)
    {
        // replace the last word with the suggested one
        return QSTR(_addSuggestion(STRQ(base), STRQ(word)));
    }

    Q_INVOKABLE QString addBestSuggestion(const QString& base)
    {
        if (_words.size())
            return QSTR(_addSuggestion(STRQ(base), _words[0]));

        return base; // unchanged
    }

    void changed() 
    {
        //LOG3("words model changed, ", _words.size());
        beginResetModel(); 
        endResetModel();
    }

protected:

    string _addSuggestion(const string& base,
                          const string& word)
    {
        // replace the last word with the suggested one
        Strings line;
        split(line, base);

        size_t n = line.size();
        if (n)
        {
            line[n-1] = word;

            // also append a space for convenience
            return  unsplit(line) + ' ';
        }
        return base; // unchanged
    }


    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        return roles;
    }
};

