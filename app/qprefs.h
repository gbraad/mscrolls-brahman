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
#include <QFont>
#include <QFontInfo>
#include <QDebug>
#include <assert.h>

#include <algorithm>
#include <future>

#include "qdefs.h"
#include "prefs.h"
#include "control.h"
#include "braschema.h"
#include "var.h"
#include "logged.h"

#include "qml-material/src/core/device.h"

#define QREGISTER_PREFS  QREGISTER_TYPE(QPrefs)

#define DPREF_BOOL(_name, _setname, _def)       \
bool _name ## Enabled() const                   \
{                                               \
    return _prefs->getInt(PREFS_ ## _def ## _ENABLED, DEFAULT_ ## _def ## _ENABLE) != 0; \
}       \
void set ## _setname ## Enabled(bool v)                           \
{                                                               \
    if (v != _name ## Enabled())                                \
    {                                                           \
        qDebug() << "prefs " #_name " enabled" << v;            \
        _prefs->set(PREFS_ ## _def ## _ENABLED, (int)v);        \
        _control->updatePrefDependencies();                     \
        emit _name ## EnabledChanged();                         \
    }                                                           \
}

#define DPREF_STRING(_name, _def)               \
string game ## _name ## String() const          \
{                                               \
    return _prefs->getString(PREFS_GAME_ ## _def, DEFAULT_GAME_ ## _def); \
}       \
void setGame ## _name ## String(const string& v)                    \
{                                                               \
    if (v != game ## _name ## String())                         \
    {                                                           \
        LOG3("game " #_name " set to ", v);                     \
        _prefs->set(PREFS_GAME_ ## _def, v);                     \
        emit game ## _name ## Changed();                        \
    }                                                           \
}

#define DPREF_QSTRING(_name, _def)                                          \
DPREF_STRING(_name, _def)                                                   \
QString game ## _name() const { return QSTR(game ## _name ## String()); }   \
void setGame ## _name(const QString& s)                                     \
{ setGame ## _name ## String(STRQ(s)); }


class QPrefs: public QObject
{
public:

    typedef std::string string;

    Q_OBJECT
    Q_PROPERTY(bool updateEnabled READ updateEnabled WRITE setUpdateEnabled NOTIFY updateEnabledChanged);
    Q_PROPERTY(bool musicEnabled READ musicEnabled WRITE setMusicEnabled NOTIFY musicEnabledChanged);
    Q_PROPERTY(bool modernEnabled READ modernEnabled WRITE setModernEnabled NOTIFY modernEnabledChanged);
    Q_PROPERTY(qreal dpScale READ dpScale WRITE setDpScale NOTIFY dpScaleChanged);
    Q_PROPERTY(QFont gameFont READ gameFont WRITE setGameFont NOTIFY gameFontChanged);

    Q_PROPERTY(qreal mapBoxSize READ mapBoxSize WRITE setMapBoxSize NOTIFY mapBoxSizeChanged);

    Q_PROPERTY(bool imageadjEnabled READ imageadjEnabled WRITE setImageadjEnabled NOTIFY imageadjEnabledChanged);

    Q_PROPERTY(bool imagepixscaleEnabled READ imagepixscaleEnabled WRITE setImagepixscaleEnabled NOTIFY imagepixscaleEnabledChanged);

    Q_PROPERTY(bool imagescaleEnabled READ imagescaleEnabled WRITE setImagescaleEnabled NOTIFY imagescaleEnabledChanged);

    Q_PROPERTY(bool compassmoveEnabled READ compassmoveEnabled WRITE setCompassmoveEnabled NOTIFY compassmoveEnabledChanged);

    Q_PROPERTY(QString gameBackgroundColor READ gameBackgroundColor WRITE setGameBackgroundColor NOTIFY gameBackgroundColorChanged);

    Q_PROPERTY(QString gamePrimaryColor READ gamePrimaryColor WRITE setGamePrimaryColor NOTIFY gamePrimaryColorChanged);

    Q_PROPERTY(QString gameContrastColor READ gameContrastColor WRITE setGameContrastColor NOTIFY gameContrastColorChanged);

    Q_PROPERTY(bool consoleechoEnabled READ consoleechoEnabled WRITE setConsoleechoEnabled NOTIFY consoleechoEnabledChanged);

    Q_PROPERTY(bool textmoveEnabled READ textmoveEnabled WRITE setTextmoveEnabled NOTIFY textmoveEnabledChanged);


    
public:

    Control*            _control;
    Prefs::Ref          _prefs;
    bool                _isMobile;

    QPrefs() { _control = 0; } // dummy
    QPrefs(Control* c, const Prefs::Ref& u) : _control(c), _prefs(u)
    {
        assert(_prefs);
        _isMobile = false;
    }

    DPREF_BOOL(update, Update, UPDATE);
    DPREF_BOOL(music, Music, MUSIC);
    DPREF_BOOL(imageadj, Imageadj, IMAGEADJ);
    DPREF_BOOL(imagescale, Imagescale, IMAGESCALE);
    DPREF_BOOL(compassmove, Compassmove, COMPASSMOVE);
    DPREF_BOOL(consoleecho, Consoleecho, CONSOLEECHO);
    DPREF_BOOL(textmove, Textmove, TEXTMOVE);

    bool modernEnabled() const
    { return _prefs->getInt(PREFS_MODERN_ENABLED, DEFAULT_MODERN_ENABLE) != 0; }

    void setModernEnabled(bool v)
    {
        if (v != modernEnabled())
        {
            qDebug() << "prefs modern enabled" << v;
            _prefs->set(PREFS_MODERN_ENABLED, (int)v);

            assert(_control);
            _control->sendOptionToEngine(BRA_OPT_MODERN, (int)v);
            
            emit modernEnabledChanged();
        }
    }
    
    qreal dpScale() const
    {
        assert(_prefs);
        return  _prefs->getDouble(PREFS_DP_SCALE, DEFAULT_DP_SCALE);
    }

    void setDpScale(qreal v)
    {
        assert(_prefs);
        
        if (v < 0.5) v = 0.5;
        else if (v > 5) v = 5;
        if (v != dpScale())
        {
            _prefs->set(PREFS_DP_SCALE, (double)v);

            // pass to the device
            Device::theDevice()->dpScale(v);
            
            emit dpScaleChanged();
        }
    }

    static int pixelToPointSize(double pixSize)
    {
        // pass in a pixel size.
        // here we calculate the equivalent point size

        int psz = floor(pixSize + 0.5); // round to whole pixel

        QFont m;
        m.setPixelSize(psz);
        
        QFontInfo fi(m);
        int pt = fi.pointSize();

        //LOG3("pixel size ", psz << " = " << pt << " points");

        return pt;
    }

    /*
    static int pointSizeToPixel(double pointSize)
    {
        QFont m;
        m.setPointSize(pointSize);
        
        QFontInfo fi(m);
        int pt = fi.pixelSize();
        return pt;
    }
    */

    DPREF_STRING(Font, FONT);
    DPREF_QSTRING(BackgroundColor, BACKGROUND_COLOR);
    DPREF_QSTRING(PrimaryColor, PRIMARY_COLOR);
    DPREF_QSTRING(ContrastColor, CONTRAST_COLOR);

    Q_INVOKABLE QString gameBackgroundColorOR(const QString& def) const
    {
        string s = gameBackgroundColorString();
        if (s.empty()) s = STRQ(def);
        return QSTR(s);
    }

    Q_INVOKABLE QString gamePrimaryColorOR(const QString& def) const
    {
        string s = gamePrimaryColorString();
        if (s.empty()) s = STRQ(def);
        return QSTR(s);
    }

    Q_INVOKABLE QString gameContrastColorOR(const QString& def) const
    {
        string s = gameContrastColorString();
        if (s.empty()) s = STRQ(def);
        return QSTR(s);
    }

    QFont gameFont() const
    {
        string fn = gameFontString();
        QFont font;
        if (!fn.empty())
        {
            font.fromString(QSTR(fn));

            double fontPoints = _control->_uiOptionFontSize;
            if (fontPoints > 0)
            {
                // only have fakeDP in sim mode, this adjusts
                // so it will agree
                double r =  Device::theDevice()->_fakeDp;
                if (r > 0)
                {
                    fontPoints *= r;
                    fontPoints *= _control->_uiOptionSimFontSizeMultiplier;
                }

                // command overrides setting, also for -sim
                LOG2("override font size to ", fontPoints);
                font.setPointSize(fontPoints);
            }
            
        }
        return font;
    }

    void setGameFont(const QFont& f) 
    {
        setGameFontString(STRQ(f.toString()));
    }

    qreal mapBoxSize() const
    {
        assert(_prefs);
        return  _prefs->getDouble(PREFS_MAPBOXSIZE, DEFAULT_MAPBOXSIZE);
    }

    void setMapBoxSize(qreal v)
    {
        assert(_prefs);
        if (v != mapBoxSize())
        {
            _prefs->set(PREFS_MAPBOXSIZE, (double)v);
            emit mapBoxSizeChanged();
        }
    }

    Q_INVOKABLE void setGameFontDefault(const QFont& f,
                                        qreal size, bool isPixels)
    {
        // pass in a pixel size.
        // here we calculate the equivalent point size and set it on
        // the given font. then that is saved as default
        //
        // we want the font to be saved by point size so that it works
        // in the font selector - which is always points and not pixels.
        //
        // but why not just pass in a font with point size?
        // that ought to work, but we know that systems go not use the
        // correct DPI, instead we've gone to the trouble of making
        // `Units.dp` correct, so this should be used to calculate size

        if (gameFontString().empty())
        {
            QFont m(f);
            int pointSize = isPixels ? pixelToPointSize(size) : size;
            m.setPointSize(pointSize);
            setGameFont(m);
        }
    }

    Q_INVOKABLE int tutorialLevel() const
    {
        return  _prefs->getInt(PREFS_TUTORIAL_LEVEL, DEFAULT_TUTORIAL_LEVEL);
    }

    Q_INVOKABLE bool setTutorialLevel(int v)
    {
        // OR in tutorial level
        
        int tv = tutorialLevel();
        
        v |= tv;
        
        if (v != tv)
        {
            _prefs->set(PREFS_TUTORIAL_LEVEL, v);
            return true;
        }

        return false;
    }

    Q_INVOKABLE void resetTutorialLevel(int v)
    {
        // if non-zero, reset tutorial level
        // if zero, reset all

        int tv = tutorialLevel();
        
        if (v) v = tv & ~v;

        if (v != tv)
            _prefs->set(PREFS_TUTORIAL_LEVEL, v);
    }

    std::future<void> _notifyResult;
    bool              _nofifyResultWaiting = false;

    void _pixelScaleOptionNotifyEngine(bool v)
    {
        // pass to back-end
        _control->sendOptionToEngine(BRA_OPT_PIXELSCALE, var(v));
    }

    bool imagepixscaleEnabled() const
    { return _prefs->getInt(PREFS_IMAGEPIXSCALE_ENABLED, DEFAULT_IMAGEPIXSCALE_ENABLE) != 0; }

    void setImagepixscaleEnabled(bool v)
    {
        if (v != imagepixscaleEnabled())
        {
            qDebug() << "prefs imagepixscale enabled" << v;
            _prefs->set(PREFS_IMAGEPIXSCALE_ENABLED, (int)v);

            if (_nofifyResultWaiting)
            {
                _notifyResult.get(); // will block if still running
            }

            _nofifyResultWaiting = true;

            _notifyResult = std::async(std::launch::async, &QPrefs::_pixelScaleOptionNotifyEngine, this, v);

            emit imagepixscaleEnabledChanged();
        }
    }

signals:

    void updateEnabledChanged();
    void musicEnabledChanged();
    void modernEnabledChanged();
    void dpScaleChanged();
    void gameFontChanged();
    void mapBoxSizeChanged();
    void imageadjEnabledChanged();
    void imagepixscaleEnabledChanged();
    void imagescaleEnabledChanged();
    void compassmoveEnabledChanged();
    void gameBackgroundColorChanged();
    void gamePrimaryColorChanged();
    void gameContrastColorChanged();
    void consoleechoEnabledChanged();
    void textmoveEnabledChanged();
};

#undef DPREF_BOOL
#undef DPREF_STRING
