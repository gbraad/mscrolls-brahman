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


#include <QtGlobal>
#include <QtGui/QFileOpenEvent>
#include <QQuickWindow>
#include <QQmlComponent>

#include "brahmanapp.h"
#include "qcontrol.h"

BrahmanApplication::BrahmanApplication(int &argc, char **argv)
    : QGuiApplication(argc, argv)
{
#if defined(Q_OS_OSX) || defined(Q_OS_IOS)
    /* TODO for OSX
    this->connect(this, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            QControl::_theControl, SLOT(applicationStateChanged(Qt::ApplicationState)));
    */
#endif
}

#if defined(Q_OS_OSX) || defined(Q_OS_IOS)

bool BrahmanApplication::event(QEvent* event)
{
    //qDebug() << event->type();
    switch (event->type())
    {
        case QEvent::ApplicationActivate:
        {
            // TODO
            // if window is minimized
            // show
            QGuiApplication::focusWindow();
            return true;
        }
        case QEvent::FileOpen:
        {
            QString filePath = static_cast<QFileOpenEvent *>(event)->file();
            QString fileName = filePath.section("/", -1, -1);
            QControl* qc = QControl::theControl();
            QFile::copy(filePath,qc->_qdataDir + "/" + fileName);
            QQmlApplicationEngine *engine = static_cast<QQmlApplicationEngine *>(qc->_qmlengine);
            QQuickWindow *itm = qobject_cast<QQuickWindow*>(engine->rootObjects().value(0));
            QQmlComponent component(engine, QUrl(QStringLiteral("qrc:/ImportMessage.qml")),itm);
            QObject *childItem = qobject_cast<QObject*>(component.create());
            return true;
        }
        default:
            break;
    }
    return QGuiApplication::event(event);
}
#endif
