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


#include <qqmlengine.h>
#include <QStandardPaths>
#include <QFile>
#include <QFileDevice>
#include <QDir>
#include <QSvgRenderer>
#include <QPainter>
#include <QImage>
#include "qcontrol.h"
#include "cutils.h"
#include "ifengine.h"
#include "strutils.h"
#include "game.h"
#include "qml-material/src/core/device.h"

#ifndef CONFIG_ASSET_LIST
#error you need to define CONFIG_ASSET_LIST in yourgame.h
#endif

DECLARE_IFENGINE

#ifdef Q_OS_IOS
#ifdef STATIC_CHOICE
Q_IMPORT_PLUGIN(IFChoice)
#elif defined(STATIC_BRAHMAN)
Q_IMPORT_PLUGIN(IFBrahman)
#else
Q_IMPORT_PLUGIN(IFMagnetic)
#endif
#endif

QControl* QControl::_theControl;

QObject *QControl_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return QControl::theControl();
}

bool QControl::prepareConfigAssets(const QString& prefix,
                                   const QString& configDir)
{
    // unpack assets
    // for android `prefix` is "assets:/"

    
    bool res = true;
    
    static const char* assetList[] =
    {
        CONFIG_ASSET_LIST
    };   

    if (!configDir.isEmpty())
    {
        QDir dpath(configDir);
        for (int i = 0; i < ASIZE(assetList); ++i)
        {
            QString fname = prefix;
            fname += QString(assetList[i]);

            QFile fi(fname);
            if (fi.exists())
            {
                QString dname = dpath.filePath(assetList[i]); 
                QFile di(dname);
            
                // if not already present, we copy
                bool copy = !di.exists();

                if (!copy)
                {
                    // copy if size changed.
                    // NB: this doesn't properly check for change, but good enough
                    copy = fi.size() != di.size();
                    if (copy)
                    {
                        // remove old target before copy
                        copy = di.remove();
                        if (!copy)
                        {
                            qDebug() << "failed to remove existing asset " << dname;
                            res = false;
                        }
                    }
                }
            
                if (copy)
                {
                    qDebug() << "unpacking asset " << fname;

                    if (fi.copy(dname)) // NB: fail if exists
                        qDebug() << "copied " << fname << " -> " << dname;
                    else
                    {
                        qDebug() << "FAILED " << fname << " -> " << dname;
                        res = false;
                    }
                }
            }
            else
            {
                // quietly ignore listed assets not present in package
                // allows a generic list of assets which might not be present

                //qDebug() << "asset, " << fname << " does not exist.";
                //res = false;
            }
        }
    }
    return res;
}

bool QControl::prepareDataAssets(const QString& prefix, const QString& dataDir)
{
    // unpack assets
    // for android `prefix` is "assets:/"

    bool res = true;
    
    static const char* dataList[] =
    {
    };   

    if (!dataDir.isEmpty())
    {
        QDir dpath(dataDir);
        for (int i = 0; i < ASIZE(dataList); ++i)
        {
            QString fname = prefix;
            fname += QString(dataList[i]);

            qDebug() << "unpacking data asset " << fname;
            QFile fi(fname);
            if (fi.exists())
            {
                QString dname = dpath.filePath(dataList[i]); 
                QFile di(dname);
            
                // if not already present, we copy
                bool copy = !di.exists();

                if (copy)
                {
                    if (fi.copy(dname))
                    {
                        qDebug() << "copied " << fname << " -> " << dname;

                        // adjust permissions
                        if (di.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner |
                                              QFileDevice::ReadUser | QFileDevice::WriteUser))
                            qDebug() << "Adjusted Permissions of " << dname;
                        else
                        {
                            qDebug() << "setPermissions FAILED " << dname;
                            res = false;
                        }
                    }
                    else
                    {
                        qDebug() << "FAILED " << fname << " -> " << dname;
                        res = false;
                    }
                }
            }
            else
            {
                qDebug() << "asset, " << fname << " does not exist.";
                res = false;
            }
        }
    }
    return res;
}


static QString getOptionPath(const char* path)
{
    QString dir = QSTRC(path);
    QDir d(dir);

    if (!d.exists())
    {
        LOG1("specified directory does not exist ", path);
        dir.clear();
    }
    else
    {
        // make path absolute
        dir = d.absolutePath();
    }
    return dir;
}

struct SimOpt
{
    const char* _name;
    int         _w;
    int         _h;
    float       _dp;
    const char* _barImage;

    // override the bar image height, if nonzero
    // normally, the bar will be the height of the image specified (eg 60)
    // but for scaled down modes, need to specify here
    int         _barHeight;

    int         _fontSize;
};

void QControl::setSimOptions(const char* sim)
{
    static const SimOpt SimTab[] = 
    {
        // rendered pixels
        //{ "iphone8p", 1125, 2436, 3, "iphone8p_bar", 0, 12 }, // h60

        { "iphone7p", 1242, 2208, 3, "iphone7p_bar", 0, 12 }, // h60
        { "iphone7pl", 2208, 1242, 3, "iphone7pl_bar", 0, 12 }, //h60
        
        // actual pixels (for vids) x/1.15 scale
        { "iphone7pvid", 1080, 1920, 2.6, "iphone7p_bar", 52, 12 },
        { "iphone7plvid", 1920, 1080, 2.6, "iphone7pl_bar", 52, 12 },
        

        { "ipad12", 2048, 2732, 2, "ipad12_bar", 0, 15 }, //h40
        { "ipad12l", 2732, 2048, 2, "ipad12l_bar", 0, 15 }, // h40
        
        { "ipad12vid", 1200, 1600, 1.15, "ipad12_bar", 23, 15},
        { "ipad12lvid", 1600, 1200, 1.15, "ipad12l_bar", 23, 15 },
    };

    for (int i = 0; i < ASIZE(SimTab); ++i)
    {
        const SimOpt* si = SimTab + i;
        if (!u_stricmp(sim, si->_name))
        {
            _simMode = true;
            
            if (!strncmp(si->_name, "ipad", 4))
            {
                // ipads count as large mobile.
                _simLargeMobile = true;
            }

            _uiOptionWidth = si->_w;
            _uiOptionHeight = si->_h;
            _uiOptionFontSize = si->_fontSize;
            _topBarImageName = si->_barImage;
            _topBarHeight = si->_barHeight;
            Device::_fakeDp = si->_dp;
            break;
        }
    }
    
    if (!_simMode)
    {
        LOG1("Sim device ", sim << " not found\n");
    }
}

void QControl::handleOptions(int argc, char** argv)
{
    Control::handleOptions(argc, argv);
    
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (!u_stricmp(argv[i], "-datadir") && i < argc-1)
            {
                // manually specify the data dir, if valid
                _dataDirOverride = getOptionPath(argv[++i]);
            }
            else if (!u_stricmp(argv[i], "-configdir") && i < argc-1)
                _configDirOverride = getOptionPath(argv[++i]);
            else if (!u_stricmp(argv[i], "-plugindir") && i < argc-1)
                _pluginDirOverride = getOptionPath(argv[++i]);
            else if (!u_stricmp(argv[i], "-sim") && i < argc-1)
                setSimOptions(argv[++i]);
            else
            {
                LOG("unknown option: ", argv[i]);
            }
        }
    }
}

static QString makePath(const QString& path, const QString file)
{
    int sz = path.size();
    if (!sz) return file;

    QString fp = path;
    if (fp.at(sz-1) != '/') fp += '/';
    fp += file;
    return fp;
}

void QControl::startUp(bool isMobile)
{
    if (!_dataDirOverride.isEmpty())
        _qdataDir = _dataDirOverride;
    else
    {
        // the standard writable location for windows is AppData/Roaming
        _qdataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

#if !defined(Q_OS_ANDROID)  // maybe also ios??
        // desktops store user data in a game subdir

        QDir ddir(_qdataDir);

        // the subdir name, ensure no spaces
        string gamesubdir = replaceAll(_storyfile, " ", "_");

        // make game dir if not already
        QString gdir(gamesubdir.c_str());
        if (!ddir.exists(gdir)) ddir.mkpath(gdir);
        ddir.cd(gdir);

        // the local user data is per game
        _qdataDir = ddir.path();
#endif
    }

    // ensure path exists
    QDir(_qdataDir).mkpath(".");
    
#if defined(Q_OS_OSX)
    // configs will be the "assets" directory underneath "Resources"
    // the first will be the writable location (above), take the last one
    QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);    
    if (dirs.size() > 1)
    {
#ifdef LOGGING
        for (int i = 0; i < dirs.size(); ++i)
            LOG3("OSX standard paths: ", STRQ(dirs[i]));
#endif
        QString d = dirs.at(dirs.size()-1);

        // XX Qt5.9 bug workaround
        d.replace(".appContents", ".app/Contents");

        size_t sz = d.size();
        if (sz > 0)
            _qconfigDir = makePath(d, "assets");
    }
#endif // OSX

    if (_qconfigDir.isEmpty())
    {
        assert(_app);

        // game assets live in a subdir of the exe
        _qconfigDir = makePath(_app->applicationDirPath(), "assets");
    }

    if (!_configDirOverride.isEmpty())
    {
        // override prevails in all cases, if given
        _qconfigDir = _configDirOverride; 
    }

#ifdef Q_OS_ANDROID
    // android always uses the same dir as for data
    _qconfigDir = _qdataDir;
#endif

    //remove html spaces
    _qconfigDir.replace("%20"," ");

    LOG1("data dir, ", STRQC(_qdataDir));
    LOG1("config dir, ", STRQC(_qconfigDir));

#ifdef Q_OS_ANDROID
    prepareConfigAssets(QString("assets:/"), _qconfigDir);
    prepareDataAssets(QString("assets:/"), _qdataDir);
#else
    // some files part of the package need to be unpacked
    //prepareDataAssets(_qconfigDir + '/', _qdataDir);
#endif

    int userSpace = 1; // XX

    Control::startUp(STRQ(_qdataDir).c_str(),
                     STRQ(_qconfigDir).c_str(),
                     userSpace);

    // pass on whether we're on mobile to prefs
    // NB: must happen after contron::startup so api prefs already initialised
    prefs()->_isMobile = isMobile;
}

void QControl::beginGame()
{
    Control::beginGame();

    // initialise info from the backend
    getProductInfo();
}

bool QControl::loadEngine()
{
    bool res = false;
    if (_app && !_pluginLoader && _engineName.size() > 0)
    {
#ifdef Q_OS_IOS
        QObject *plugin = QPluginLoader::staticInstances().first();
        if (plugin)
        {
            IFEngineInterface* be =
                qobject_cast<IFEngineInterface*>(plugin);
            if (be)
            {
                parentT::loadEngine(be);
                LOG("engine loaded, magnetic static","");
                res = true;
            }
            else
            {
                LOG1("plugin not loaded, magnetic static","");

                // clean up
                delete _pluginLoader; _pluginLoader = 0;
            }
        }
#else
        QString pdir = _pluginDirOverride;
        if (pdir.length() == 0) pdir = _app->applicationDirPath();
        
        QDir pluginsDir(pdir);
        LOG("scanning for plugin in ", STRQ(pdir));

        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            string fname(STRQ(fileName));

            if (startsWithIgnoreCase(fname, "lib"))
            {
                // libbrahman.dll, libbrahman.so is allowed too
                fname.erase(0, 3); // remove "lib"
            }

            if (startsWithIgnoreCase(fname, _engineName.c_str()))
            {
                LOG1("loading plugin ", pluginsDir.absoluteFilePath(fileName).toLatin1().data());
  
                _pluginLoader = new QPluginLoader(pluginsDir.absoluteFilePath(fileName));
                QObject *plugin = _pluginLoader->instance();
                if (plugin)
                {
                    IFEngineInterface* be = 
                        qobject_cast<IFEngineInterface*>(plugin);
                    if (be)
                    {
                        bool v = parentT::loadEngine(be);
                        if (v)
                        {
                            LOG("engine loaded, ", fname);
                            res = true;
                            break;
                        }
                    }
                    else
                    {
                        LOG1("plugin not loaded, ", pluginsDir.absoluteFilePath(fileName).toLatin1().data());

                        // clean up
                        delete _pluginLoader; _pluginLoader = 0;
                    }
                }
            }
        }
#endif
    }

    if (!res)
    {
        LOG4("QControl, ", "loading IFI");
        
        // fallback to IFI
        res = parentT::loadIFI();
    }
    
    return res;
}

QStringList QControl::loadGameFiles()
{
    // get a list of load game candidates
    QDir qd(_qdataDir);
    qd.setFilter(QDir::Files);

    QStringList filters;
    filters << "*.sav";
    qd.setNameFilters(filters);

    return qd.entryList();
}

bool QControl::lowMargins() const
{
    bool r = false;

    // NOTE:
    // low margins might not apply for large mobiles (eg pads),
    // but actually they _do_ look better with more space.
    // desktops still look worse.
    //
    // so, for now, we allow large mobile to have slim margins

    if (_simMode)
    {
        // except for large mobile in sim mode (eg ipad)
        //r = !_simLargeMobile;
        r = true;
    }
    else
    {
        Device* d = Device::theDevice();
        if (d->isMobile())
        {
            // except for large mobiles
            //r = !d->isLargeMobile();
            r = true;
        }
    }
    return r;
}

bool QControl::makeHandCursor(QPixmap& pm, int& hotx, int& hoty)
{
    bool r = _simMode;

    if (r)
    {
        const char* iconpath = ":/icons/other/handtap.svg";
        QString qiconpath(iconpath);

        // based on size 1024
        hotx = 412;
        hoty = 200;

        int size = 128;

        QFile f(qiconpath);
        if (!f.exists())
        {
            LOG1("Cannot location icon ", iconpath);
            r = false;
        }
        else
        {
            QSvgRenderer renderer(qiconpath);
            QImage image(size, size, QImage::Format_ARGB32);
            image.fill(0);

            QPainter painter(&image);
            renderer.render(&painter);

            hotx = (hotx * size)/1024;
            hoty = (hoty * size)/1024;

            pm.convertFromImage(image);  
        }
    }
    return r;
}
