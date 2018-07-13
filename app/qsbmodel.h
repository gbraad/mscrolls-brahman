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

#include <assert.h>
#include "qdefs.h"
#include "itemsmodel.h"
#include "control.h"
#include "logged.h"

class QSidebarModel : public QAbstractListModel, public APIType::Notifier
{
    //Q_OBJECT

protected:

    typedef std::string string;
    typedef QAbstractListModel parentT;
    typedef ItemsModel::Entry Entry;

    Control*            _control;
    ItemsModel*         _model;

public:

    enum CatRoles
    {
        NameRole = Qt::UserRole + 1,
        IDRole = Qt::UserRole + 2,
        IconRole = Qt::UserRole + 3,
    };

    QSidebarModel(Control* control) : QAbstractListModel(0) 
    {
        _control = control;
        _model = _control->_itemsModel.get();
        
        assert(_model);
        _model->addNotifier(this);
    }

    virtual ~QSidebarModel()
    {
        assert(_model);
        _model->removeNotfier(this);
    }

    virtual int size() const = 0;
    virtual const Entry& getEntry(int i) const = 0;
				
    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        int n = size();
        //LOG3("items model size, ", n);
        return n;
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (index.isValid())
        {
            int ix = index.row();
            //LOG3("Request for item ", ix);
            
            if (ix < size())
            {
                const ItemsModel::Entry& ii = getEntry(ix);

                switch (role)
                {
                case NameRole:
                    return QSTR(ii._name);
                    break;
                case IDRole:
                    return QSTR(ii._id);
                    break;
                case IconRole:
                    {
                        string icon;

                        // if we have a specific icon, resolve it
                        if (!ii._icon.empty())
                            icon = _control->resolveAsset(ii._icon);
                        
                        return QSTR(icon);   
                    }
                    break;
                default:
                    LOG3("Request for item ", ix << " unknown role " << role);
                    break;
                }
            }
        }
        return QVariant();
    }

    void changed() override
    {
        LOG4("UI, ", "item model changed");
        beginResetModel(); 
        endResetModel();
    }

protected:

    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[IDRole] = "itemid";
        roles[IconRole] = "icon";
        return roles;
    }
};

