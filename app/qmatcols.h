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
#include <QColor>

#include <assert.h>
#include "qdefs.h"
#include "control.h"
#include "logged.h"
#include "matcol.h"

class QMatColModel : public QAbstractListModel
{
    Q_OBJECT

    typedef std::string string;
    typedef QAbstractListModel parentT;

public:

    enum CatRoles
    {
        NameRole = Qt::UserRole + 1,
        ColRole = Qt::UserRole + 2,
        PrimaryColRole = Qt::UserRole + 3,
        ContrastNameRole = Qt::UserRole + 4,
    };

    QMatColModel(Control* control) : QAbstractListModel(0) {}

    int size() const
    {
        static int _size;
        if (!_size) _size = MatCol::baseColourCount();
        return _size;
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        int n = size();
        //LOG3("matcol model size, ", n);
        return n;
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (index.isValid())
        {
            int ix = index.row();

            MatCol::iterator it = MatCol::begin();            
            for (int i = 0; i < ix; ++i) it = MatCol::next(it);
            
            switch (role)
            {
            case NameRole:
                return QSTR(MatCol::atName(it));
                break;
            case ColRole:
                return QColor(MatCol::atColour(it)._argb);
                break;
            case PrimaryColRole:
                {
                    // return the colour to use as primary
                    // this is the same as `ColRole` except some colours
                    // need adjusting such as black and white. 
                    // these can't be used as real primaries
                    Colour c = MatCol::atColour(it);
                    if (c._argb == Colour::black) c = c.contrasting(0.2);
                    if (c._argb == Colour::white) c = c.contrasting(0.05);

                    return QColor(c._argb);
                }
                break;
            case ContrastNameRole:
                return QSTR(MatCol::getContrastName(MatCol::atName(it)));
                break;
            }
        }
        return QVariant();
    }

protected:

    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[ColRole] = "color";
        roles[PrimaryColRole] = "primaryColor";
        roles[ContrastNameRole] = "contrastName";
        return roles;
    }
};

