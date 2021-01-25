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


#include <QGuiApplication>

#include <qqmlengine.h>
#include <qqmlcontext.h>
#include <qqml.h>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickview.h>
#include <QResource>
#include <QIcon>
#include <QQuickStyle>
#include <QThread>
#include <QCursor>

#include "qdefs.h"
#include "qlog.h"
#include "qcontrol.h"
#include "qprefs.h"
#include "qtranscript.h"
#include "qconsole.h"
#include "qmapbox.h"
#include "qitems.h"
#include "qpeople.h"
#include "qmatcols.h"
#include "qwords.h"
#include "os.h"
#include "qshape.h"
#include "qml-material/src/plugin.h" // from qml-material
#include "qml-material/src/core/device.h"
#include "game.h"
#include "brahmanapp.h"
#include "opt.h"
#include "soundplayer.h"

#ifdef USE_SPINE
#include "spineqml.h"
#endif

#ifdef USE_ITCH
#include "updateritch.h"
#endif

void systemPause()
{
    QSoundPlayer::pauseAllPlayers();
}

void systemResume()
{
    QSoundPlayer::resumeAllPlayers();
}

int main(int argc, char *_argv[])
{
    Logged initLog;
    
    // create our control object first and handle options before the UI
    QControl* qc = QControl::theControl();
    qc->setupLogging();

    qc->_engineName = DEFAULT_ENGINE_NAME;

#if defined(Q_OS_ANDROID) && !defined(NDEBUG)
    // debug version force logging for android 
    qc->setLogLevel(3);
#endif

#ifdef DEFAULT_STORY_NAME
    qc->_storyfile = DEFAULT_STORY_NAME;
#else

    // take the story name from the program name.
    // remove any path and suffix
    
    const char* p = _argv[0];
    const char* q = strrchr(p, '/');
    if (!q) q = strrchr(p, '\\');
    if (q) ++q;
    else q = p;
    qc->_storyfile = changeSuffix(q, 0); // remove any suffix
#endif


    char** argv = Opt::copyArgs(argc, _argv);
    qc->handleOptions(argc, argv);
    Logged::_logLevel = qc->getLogLevel(); // from API DLL
    Logged::_stream.consume(qc->getLogStream()); // same poo
    
    // NB: this must happen before the UI
    if (qc->_options._hiDpi)
        BrahmanApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // create the UI
    BrahmanApplication app(argc, argv);

    app.setApplicationName("Brahman");

    // breaks standard path??
    //app.setOrganizationName("brahman");

    app.setOrganizationDomain("strandgames.com");

    if (!createUniqueInstance(qc->_storyfile.c_str()))
    {
        // program already running
        Opt::deleteCopyArgs(argv);
        return 0;
    }

    // default material unless overridden
    QString style = QQuickStyle::name();
    if (style.isEmpty()) QQuickStyle::setStyle("Material");
    
    qc->_app = &app;

    QPixmap handcursor;
    int hotx, hoty;
    if (qc->makeHandCursor(handcursor, hotx, hoty))
    {
        BrahmanApplication::setOverrideCursor(QCursor(handcursor, hotx, hoty));
    }

#ifndef Q_OS_ANDROID
    // under android qdebug already goes to the monitor
    qInstallMessageHandler(QLog::logMessage);
#endif

    QREGISTER_CONTROL;
    QREGISTER_PREFS;
    QREGISTER_TRANSCRIPT;
    QREGISTER_CONSOLE;
    QREGISTER_MAPBOX;
    QREGISTER_SHAPE;
    QREGISTER_SOUNDPLAYER;

#ifdef USE_SPINE
    SpineRegisterTypes();  // from spineqml.cpp
#endif    

    // load IF engine as plugin
    if (!qc->loadEngine())
    {
        LOG("Fatal Error, failed to load engine: ", qc->_engineName);
        Opt::deleteCopyArgs(argv);
        return 0;
    }

    // init updater after engine
#ifdef USE_ITCH
    qc->_updater = new UpdaterItch(qc);
#endif

    // second part of control initialisation. must be done before using
    // the controller, but after we process options.
    // must be after the application object
    qc->startUp(Device::theDevice()->isMobile());

    QItemsModel itemsModel(qc);
    QPeopleModel peopleModel(qc);
    QMatColModel matColModel(qc);
    QWordsModel wordsModel(qc);

    qc->beginGame();
    
    // register qml-material
    MaterialPlugin qmlMaterial;
    qmlMaterial.registerTypes("Material");
    
    QQmlApplicationEngine engine;
    engine.addImageProvider("anim", qc->createAnimProvier());

    if (qc->_options._useAccurateDPI)
    {
        // do we have an accurate version?
        int dpi = qc->getScreenDPI(); 
        Device::theDevice()->truedpi(dpi); // might be zero
    }
    
    double dp = qc->_prefs->getDouble(PREFS_DP_SCALE, DEFAULT_DP_SCALE);
    Device::theDevice()->dpScale(dp);

    QQmlContext* ctxt = engine.rootContext();
    ctxt->setContextProperty("itemsModel",  &itemsModel);
    ctxt->setContextProperty("peopleModel",  &peopleModel);
    ctxt->setContextProperty("matColModel",  &matColModel);
    ctxt->setContextProperty("wordsModel",  &wordsModel);
    engine.addImportPath(":/.");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    qc->_qmlengine = &engine;

    int res = app.exec();
    destroyUniqueInstance();
    Opt::deleteCopyArgs(argv);
    return res;
}
