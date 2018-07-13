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

#include <QQmlApplicationEngine>
#include <QObject>
#include <QString>
#include <qqml.h>
#include <QDebug>
#include <QPluginLoader>
#include <QColor>
#include <assert.h>

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QtAndroid>
#include <QAndroidJniObject>
#endif

#ifdef Q_OS_IOS
#include "iosshare.h"
#endif

#include "qdefs.h"
#include "qprefs.h"
#include "control.h"
#include "logged.h"
#include "qlog.h"
#include "strutils.h"
#include "updater.h"
#include "growbuf.h"
#include "colour.h"
#include "matcol.h"
#include "qimgprov.h"

#define QREGISTER_CONTROL  QREGISTER_SINGLE(QControl)

class QControl : public QObject, public Control
{
    Q_OBJECT
    Q_PROPERTY (QPrefs* prefs READ prefs CONSTANT)
    Q_PROPERTY (QString currentImageJSON READ currentImageJSON NOTIFY currentImageJSONChanged)
    Q_PROPERTY (QString currentMetaJSON READ currentMetaJSON NOTIFY currentMetaJSONChanged)
    Q_PROPERTY (QString currentMessage READ currentMessage NOTIFY currentMessageChanged)
    Q_PROPERTY (bool lowMargins READ lowMargins CONSTANT)
    Q_PROPERTY(QString titleText READ titleText NOTIFY titleTextChanged);

    typedef Control parentT;

public:

    QCoreApplication*           _app;
    QQmlEngine*                 _qmlengine;
    QPluginLoader*              _pluginLoader;

    Updater*                    _updater;

    // per user data
    QString                     _qdataDir;

    // per app data (eg where the exe lives)
    QString                     _qconfigDir;

    // manual specification from command line
    QString                     _dataDirOverride;
    QString                     _configDirOverride;
    QString                     _pluginDirOverride;

    // from sim mode
    QString                     _topBarImageName;
    int                         _topBarHeight = 0;

    AnimImageProvider*          _animProvider = 0;

    static QControl* theControl()
    {
        if (!_theControl) _theControl = new QControl();
        return _theControl;
    }

    QControl() { _init(); }
    
    ~QControl()
    {
        if (_pluginLoader)
        {
            _pluginLoader->unload();
            delete _pluginLoader;
        }

        if (_updater) delete _updater;
        
        _theControl = 0;
    }

    void setupLogging()
    {
#if defined(Q_OS_ANDROID)
        setupAndroidLog();
#endif
    }

    // initialise the control object
    void startUp(bool isMobile);
    void beginGame();
    bool loadEngine();
    bool prepareConfigAssets(const QString& prefix, const QString& configDir);
    bool prepareDataAssets(const QString& prefix, const QString& dataDir);
                                         
    QPrefs* prefs()
    {
        if (!_qprefs)
        {
            _qprefs = _make_unique<QPrefs>(this, _prefs);
            OWN(_qprefs.get()); // make sure QML doesn't free it
        }
        return _qprefs.get();
    }

    Q_INVOKABLE QString currentVersion() const
    {
        return QSTR(Control::currentVersion());
    }

    void updateResult()
    {
        if (_updater)
        {
            const Updater::UpdateInfo& info = _updater->_info;
            LOG4("Update available: ", info._updateAvailable);
            LOG4("Update ok: ", info._ok);
            LOG4("Update msg: ", info._msg);

            if (!info._ok)
            {
                LOG1("Update: ", info._msg);
            }
            else
            {
                if (info._updateAvailable) currentMessage(info._msg);
            }
        }
    }

    Q_INVOKABLE void checkForUpdates()
    {
        // manual check for updates (only if started)
        if (_updater) _updater->check(std::bind(&QControl::updateResult, this));
    }

    Q_INVOKABLE void evalCommand(const QString& cmd)
    {
        Control::evalCommand(STRQ(cmd));
    }

    Q_INVOKABLE bool evalUseXwithY(const QString& x, const QString& y)
    {
        return Control::evalUseXwithY(STRQ(x), STRQ(y));
    }

    Q_INVOKABLE bool menuDrop(const QString& x)
    {
        // called when we drag from menu.
        return Control::menuDrop(STRQ(x));
    }

    Q_INVOKABLE bool menuClick(const QString& x)
    {
        // clicked on sidebar item
        return Control::menuClick(STRQ(x));
    }

    Q_INVOKABLE QString itemIcon(const QString& qid)
    {
        return QSTR(parentT::itemIcon(STRQ(qid)));
    }

#define _GAME_STR(_name, _tag)          \
    string _game ## _name() const       \
    {                                   \
        string s;                       \
        if (_productInfoJSON)           \
            s = JSON::StringValue((JSON&)*_productInfoJSON, BRA_PRO_ ## _tag).toString();  \
        return s;                       \
    }                                   \
    Q_INVOKABLE QString game ## _name() const { return QSTR(_game ## _name()); }

#define _GAME_STR_D(_name, _tag, _def)   \
    string _game ## _name() const       \
    {                                   \
        string s;                       \
        if (_productInfoJSON)           \
            s = JSON::StringValue((JSON&)*_productInfoJSON, BRA_PRO_ ## _tag).toString();  \
        if (s.empty()) s = _def;        \
            return s;                   \
    }                                   \
    Q_INVOKABLE QString game ## _name() const { return QSTR(_game ## _name()); }

#define _GAME_INT_D(_name, _tag, _def)          \
    Q_INVOKABLE int game ## _name() const       \
    {                                           \
        int v = _def;                           \
        if (_productInfoJSON)                   \
            v = JSON::IntValue((JSON&)*_productInfoJSON, BRA_PRO_ ## _tag); \
        return v;                               \
    }

#define _GAME_BOOL_D(_name, _tag, _def)          \
    Q_INVOKABLE bool game ## _name() const       \
    {                                           \
        bool v = _def;                           \
        if (_productInfoJSON)                   \
            v = JSON::BoolValue((JSON&)*_productInfoJSON, BRA_PRO_ ## _tag); \
        return v;                               \
    }
    
    _GAME_STR_D(Title, TITLE, "Game");
    _GAME_STR_D(Author, AUTHOR, "Strand Games");
    _GAME_STR(Credits, CREDITS);
    _GAME_STR(MarketAndroid, MARKET_ANDROID);
    _GAME_STR(MarketIOS, MARKET_IOS);
    _GAME_STR_D(CoverTextColor, COVERTEXT_COL, "black");
    _GAME_STR_D(CoverTextFont, COVERTEXT_FNT, "Arial");
    _GAME_STR(CoverEffect, COVER_EFFECT);
    _GAME_STR(CoverImage, COVER_IMAGE);
    _GAME_STR_D(ThemePrimeColor, THM_PRIME_COL, "#ff5722");
    _GAME_STR_D(ThemeAccentColor, THM_ACCENT_COL, "red");
    _GAME_STR_D(ThemeTabColor, THM_TAB_COL, "white");
    _GAME_STR_D(ThemeContrastColor, THM_CONTR_COL, "#3f51b5");

    _GAME_INT_D(CoverTextWeight, COVERTEXT_W, 0);

    _GAME_BOOL_D(EnableSidebar, UI_SIDEBAR, false);
    _GAME_BOOL_D(EnableTextbox, UI_TEXTBOX, false);
    _GAME_BOOL_D(EnableCompass, UI_COMPASS, false);

    _GAME_BOOL_D(EnableSaveLoad, UI_SAVELOAD, false);
    _GAME_BOOL_D(EnableUpdate, UI_UPDATE, false);
    _GAME_BOOL_D(EnableClassic, UI_CLASSIC, false);
    _GAME_BOOL_D(EnableUndoRedo, UI_CLASSIC, false);

    Q_INVOKABLE QString gameCreditsHTML() const
    {
        string c = _gameCredits();
        string s = _transcript->styleHTML();
        s += c;
        return QSTR(s);
    }

    static void _fontSpec(GrowString& gs, const string& fname, int fs, int fw)
    {
        if (!fname.empty())
            gs.appendf("font-family:'%s';", fname.c_str());
        if (fs)
            gs.appendf("font-size:%dpt;", fs);
        if (fw)
            gs.appendf("font-weight:%d;", fw);
    }

    static void fontSpec(GrowString& gs, const char* styleclass,
                         const string& fname, int fs, int fw)
    {
        if (!styleclass)
        {
            gs.append("<span style=\"");
            _fontSpec(gs, fname, fs, fw);
        }
        else gs.appendf("<span class=\"%s", styleclass);
        gs.append("\">");
    }

    Q_INVOKABLE QString resolveAsset(const QString& path)
    {
        return QSTR(Control::resolveAsset(STRQ(path)));
    }

    Q_INVOKABLE QString formatCoverText(const QString& qfname,
                                        int fpixsize1,
                                        int fpixsize2,
                                        int fw,
                                        const QString& organisation,
                                        const QString& title,
                                        const QString& author) const
    {
        // take first word as this is the font family
        string fname = STRQ(qfname); 

        if (fw)
        {
            // when a weight is supplied, reduce name to font family
            // XX NB: appears that font-weight doesn't work in Qt
            fname = firstWordOf(fname);
        }

        //LOG3("cover text font ", fname << " weight " << fw);

        // dont let font sizes go mad
        if (fpixsize1 < 1) fpixsize1 = 1;
        if (fpixsize2 < 1) fpixsize2 = 1;

        int fsize1 = QPrefs::pixelToPointSize(fpixsize1);
        int fsize2 = QPrefs::pixelToPointSize(fpixsize2);

        GrowString gs;

        gs.append("<style type=\"text/css\">");
        gs.append("\n.line1 { ");
        _fontSpec(gs, fname, fsize1, fw);
        gs.append("\n}\n");
        gs.append("\n.line2 { ");
        _fontSpec(gs, fname, fsize2/3, fw);
        gs.append("\n}\n");
        gs.append("\n.line3 { ");
        _fontSpec(gs, fname, fsize2, fw);
        gs.append("\n}\n");
        gs.append("\n.line4 { ");
        _fontSpec(gs, fname, fsize2/2, fw);
        gs.append("\n}</style>\n");

        fontSpec(gs, "line1", fname, fsize1, fw);
        gs.append("<p align=\"center\">");
        gs.append(STRQ(organisation)); // Strand Games
        gs.append("</p></span>");
        
        fontSpec(gs, "line2", fname, fsize2/3, fw);
        gs.append("<p align=\"center\">");
        gs.append("Presents");
        gs.append("</p></span>");

        fontSpec(gs, "line3", fname, fsize2, fw);
        gs.append("<p align=\"center\">");
        gs.append(STRQ(title));
        gs.append("</p></span>");

        fontSpec(gs, "line4", fname, fsize2/2, fw);
        gs.append("<p align=\"center\">");
        gs.append(STRQ(author));
        gs.append("</p></span>");

        gs.add(0);
        
        // LOG3("covertext: ", gs.start());
        
        return QSTRC(gs.start());
    }

    AnimImageProvider* createAnimProvier()
    {
        _animProvider = new AnimImageProvider;
        return _animProvider;
    }
    
    Q_INVOKABLE bool providerAnimated() const
    {
        assert(_animProvider);
        return _animProvider->animated();
    }
    
    Q_INVOKABLE QString tailWords(const QString& s) const
    {
        return QSTR(selectWords(STRQ(s), 1, -1));
    }

    void handleOptions(int argc, char** argv) override;
    
    void _init()
    {
        _app = 0;
        _pluginLoader = 0;
        _updater = 0;
    }

    void setLogLevel(int level) override
    {
        // set here since there is a different value per DLL
        Logged initLog;        

        if (level >= 0 && level < 100)
        {
            Logged::_logLevel = level;
            
            // pass on
            Control::setLogLevel(level);
        }
    }

    Q_INVOKABLE int getScreenDPI() const { return Control::getScreenDPI(); }

#define DEF_JSON(_name)        \
    QString _name ## JSON() const { return QSTR(_ ## _name ## JSON); }  \
    void _name ## JSON(const string& js)                                \
    {                                                                   \
        if (_ ## _name ## JSON != js)                                   \
        {                                                               \
            _ ## _name ## JSON = js;                                    \
            emit _name ## JSONChanged();                                \
        }                                                               \
    }

    DEF_JSON(currentImage);
    DEF_JSON(currentMeta);

    // called from transcripti
    void imageChanged(const string& js) override { currentImageJSON(js); }

    // from ifiMetaResponse
    void metaChanged(const string& js) override { currentMetaJSON(js); }

    QString currentMessage() const { return QSTR(_currentMessage); }
    void currentMessage(const string& text) { currentMessage(text.c_str()); }

    void currentMessage(const char* text, const char* url = 0)
    {
        string m;

        if (url)
        {
            // make a HTML link from the string and url
            m = "<a href=\"";
            m += url;
            m += "\">";
            m += text;
            m += "</a>";
        }
        else
        {
            m = text;
        }

        if (_currentMessage != m)
        {
            _currentMessage = m;
            LOG3("current message, ", _currentMessage);
            emit currentMessageChanged();
        }
    }

    Q_INVOKABLE QString undoredo(bool undo)
    {
        return QSTR(Control::undoredo(undo)); 
    }

    Q_INVOKABLE QString getMusicFile() const
    {
        // return non-empty string if exists
        string mf;
        if (!_musicFile.empty())
            mf = string("file:///") + _musicFile; // turn url
        
        return QSTR(mf);
    }

    Q_INVOKABLE bool saveGame(const QString& filename)
    {
        return Control::saveGame(STRQ(filename));
    }

    Q_INVOKABLE QStringList loadGameFiles();

    Q_INVOKABLE bool loadGame(const QString& filename)
    {
        return Control::loadGame(STRQ(filename));
    }

    Q_INVOKABLE void restartGame() { Control::restartGame(); }

    Q_INVOKABLE QColor colorContrast(const QColor qc, double a) const
    {
        // NB: this doesnt have any alpha
        Colour c(qc.rgba());
        //LOG3("contrast col from, ", c.toString());
        c = c.contrasting(a);
        //LOG3("contrast col to, ", c.toString());
        return QColor(c._argb);
    }

    Q_INVOKABLE QColor colorAlpha(const QColor qc, double a) const
    {
        QColor col(qc);
        col.setAlphaF(a);
        return col;
    }

    Q_INVOKABLE QString colorMaterial(const QString& name, int shade) const
    {
        // NB: this doesnt have any alpha
        // return empty string is `name` is not material.
        
        Colour c = MatCol::get(STRQC(name), shade);
        return c ? QSTR(c.toString()) : QString();
    }

    Q_INVOKABLE QString colorContrastMaterial(const QString& name) const
    {
        // find the name of the material contrast colour.
        return QSTR(MatCol::getContrastName(STRQC(name)));
    }

    Q_INVOKABLE QColor colorMaterialShade(const QColor& qc, int shade) const
    {
        Colour c(qc.rgba());
        //LOG3("colorMaterialShade ", c << " -> " << MatCol::get(c,shade));
        return MatCol::get(c, shade)._argb;
    }

    Q_INVOKABLE bool deleteSaveGame(const QString& filename)
    {
        return parentT::deleteSaveGame(STRQ(filename));
    }

    Q_INVOKABLE int appUIWidthOR(int v) const
    {
        return _uiOptionWidth > 0 ? _uiOptionWidth : v;
    }

    Q_INVOKABLE int appUIHeightOR(int v) const
    {
        return _uiOptionHeight > 0 ? _uiOptionHeight : v;
    }

    Q_INVOKABLE int appUIFontSize() const
    {
        // also affected by -sim option
        return _uiOptionFontSize;
    }

    Q_INVOKABLE QString topBarImageName() const
    {
        QString name;
        if (!_topBarImageName.isEmpty())
            name = QString("assets/") + _topBarImageName + ".png";

        return name;
    }

    Q_INVOKABLE int topBarHeight() const { return _topBarHeight; }
    Q_INVOKABLE bool simMode() const { return _simMode; }
    Q_INVOKABLE bool noFrame() const { return _uiNoFrame; }

    bool lowMargins() const;

    Q_INVOKABLE int devicePixelRatio() const
    {
        int ratio = 1; // if we don't know
        QGuiApplication* gapp = dynamic_cast<QGuiApplication*>(_app);
        if (gapp)
        {
            QScreen* s = gapp->primaryScreen();
            if (s) ratio = s->devicePixelRatio();
        }
        return ratio;
    }

    Q_INVOKABLE void share(const QString& s) const
    {
        string s1 = STRQ(_qdataDir + '/' + s);
        LOG3("share ", s1);
        
#ifdef Q_OS_ANDROID
        extern void _share(const char*);
        _share(s1.c_str());
#endif
#ifdef Q_OS_IOS
        QUrl fileUrl = QUrl::fromLocalFile(QString::fromStdString(s1));
        IosShare* pIosShare = new IosShare();
        pIosShare->share(fileUrl);
#endif
    }

    void setSimOptions(const char* sim);
    bool makeHandCursor(QPixmap&, int& hotx, int& hoty);

    QString titleText() const
    {
        return QSTR(Control::titleText());
    }

    bool titleText(const string& t) override
    {
        bool r = Control::titleText(t);
        if (r)
            emit titleTextChanged();
        return r;
    }

    Q_INVOKABLE bool compassDirection(unsigned int dir)
    {
        return Control::compassDirection(dir);
    }

public slots:

    void uiInitialised()
    {
        //qDebug() << "main window initialised";
        if (_updater) _updater->start();
    }

signals:

    void currentImageJSONChanged();
    void currentMetaJSONChanged();
    void currentMessageChanged();
    void titleTextChanged();

public:
    
    static QControl* _theControl;
    
    // cached
    std::unique_ptr<QPrefs>     _qprefs;
    string                      _currentMessage;
};

