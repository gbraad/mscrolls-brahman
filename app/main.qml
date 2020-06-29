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


import QtQuick 2.7
import QtQuick.Controls 1.4
import Material 0.3
import QtQuick.Window 2.2
import QtMultimedia 5.8


import com.voidware.brahman 1.0

ApplicationWindow
{
    id: app

    // the desktop width/height are chosen with enough difference so
    // the main window does not flip between portrait and landscape on startup
    width: QControl.appUIWidthOR(Device.isMobile ? Screen.desktopAvailableWidth : Math.min(1100*Units.dpNow(), Screen.desktopAvailableWidth * 0.50))
    height: QControl.appUIHeightOR(Device.isMobile ? Screen.desktopAvailableHeight : Math.min(900*Units.dpNow(), Screen.desktopAvailableHeight * 0.90))
    
    title: QControl.gameTitle()
    visible: true

    property string author: QControl.gameAuthor();
    property string organisation: "Strand Games"
    property string currentMetaJSON: QControl.currentMetaJSON
    property bool enableSidebar: QControl.gameEnableSidebar()
    property bool enableTextInput: QControl.gameEnableTextbox()
    property bool enableCompass: QControl.gameEnableCompass()
    property string coverImage: QControl.gameCoverImage()
    property string coverEffect: QControl.gameCoverEffect()
    property color coverTextColor: QControl.gameCoverTextColor()
    property int coverTextWeight: QControl.gameCoverTextWeight()
    property string coverTextFont: QControl.gameCoverTextFont()
    property string gameCredits: QControl.gameCreditsHTML()
    property bool enableSaveLoad: QControl.gameEnableSaveLoad()
    property bool enableClassic: QControl.gameEnableClassic()
    property string privacyPolicy: QControl.gamePrivacyPolicy()

    // whether save & load can happen within a choice
    property bool enableSaveLoadChoice: false
    property bool canSaveLoad: true
    
    // classic or enabled through IFI meta
    property bool enableRestart: QControl.gameEnableClassic()

    property string soundJSON: QControl.soundJSON
    property int force: QControl.force
    property real soundVol: QControl.prefs.soundVol/100

    readonly property int dialogWidth: width*3/4
    readonly property int dialogHeight: height*8/10
    
    function setThemeCols(matname)
    {
        var c = QControl.colorMaterial(matname, 500);
        if (c)
        {
            app.theme.primaryColor = c
            var cc = QControl.colorContrastMaterial(matname);
            app.theme.contrastColor = QControl.colorMaterial(cc, Theme.isDark ? -200 : -700)
            //console.log("theme primary mat col", matname)
            //console.log("theme contrast mat col", cc)
        }
        else
        {
            // try using directly
            app.theme.primaryColor = matname

            // XX this isn't an opposing colour!
            var cc = QControl.colorContrast(app.theme.primaryColor, 0.5)
            app.theme.contrastColor = cc
            
            //console.log("theme primary col", c)
            //console.log("theme contrast col", cc)
        }
    }

    onCurrentMetaJSONChanged:
    {
        //console.log("current meta JSON ", currentMetaJSON)
        var js = JSON.parse(currentMetaJSON)
        var v;
        v = js["title"]
        if (v) title = v
        v = js["author"]
        if (v) author = v
        v = js["organisation"]
        if (v) organisation = v
        v = js["privacy"]
        if (v) privacyPolicy = v
        v = js["ui_textinput"]
        if (v) enableTextInput = true
        v = js["ui_sidebar"]
        if (v) enableSidebar = true
        v = js["ui_compass"]
        if (v) enableCompass = true
        v = js["credits"]
        if (v) gameCredits = v
        v = js["covertext"]
        if (v && typeof v == 'object')
        {
            var t = v["color"]
            if (t) coverTextColor = t
            t = v["weight"]
            if (t) coverTextWeight = t
            t = v["font"]
            if (t) coverTextFont = t
        }
        v = js["primary_color"]
        if (v) setThemeCols(v)
        v = js["saveload"]
        if (v) enableSaveLoad = true
        v = js["ui_restart"]
        if (v) enableRestart = true
        v = js["saveloadchoice"]
        if (v) saveloadchoice = true
        coverImage = QControl.resolveAsset(js["backimage"])
    }
    

    flags: QControl.noFrame() ? Qt.FramelessWindowHint | Qt.BypassWindowManagerHint : Qt.Window
    
    topBarImage: Qt.resolvedUrl(QControl.topBarImageName())
    topBarHeight: QControl.topBarHeight()

    FontLoad { id: sansRegular; source: "fonts/Roboto-Regular.ttf" }

    // XX light versions of Kanit do not work on the mac
    // unless we ONLY load that version
    // still here in 5.9.6
    FontLoad { id: sansK; source: !Device.isMac() ? "fonts/Kanit-Regular.ttf": "" }
    FontLoad { id: sansKLight; source: !Device.isMac() ? "fonts/Kanit-Light.ttf" : "" }
    FontLoad { id: sansKThin; source: "fonts/Kanit-Thin.ttf" }

    FontLoad { id: ptri; source: "fonts/PT_Serif-Web-Italic.ttf" }
    FontLoad { id: ptr; source: "fonts/PT_Serif-Web-Regular.ttf" }

    FontLoad { id: mw; source: "fonts/Merriweather-Light.ttf" }
    FontLoad { id: mwi; source: "fonts/Merriweather-LightItalic.ttf" }
    //FontLoad { id: mwr; source: "fonts/Merriweather-Regular.ttf" }
    //FontLoad { id: mwri; source: "fonts/Merriweather-Italic.ttf" }

    Component
    {
        id: retroFonts
        Item
        {
        FontLoad { source: "fonts/PxPlus_AmstradPC1512.ttf" }
        FontLoad { source: "fonts/PxPlus_IBM_CGA.ttf" }
        FontLoad { source: "fonts/PxPlus_IBM_EGA9.ttf" }
        FontLoad { source: "fonts/PxPlus_IBM_VGA9.ttf" }
        FontLoad { source: "fonts/AtariST8x16SystemFont.ttf" }
        }
    }

    Loader 
    {
        // only load retro fonts when classic mode is supported
        active: app.enableClassic
        asynchronous: true
        sourceComponent: retroFonts
    }
    
    theme
    {
        // gameThemePrimeColor & gameThemeContrastColor are the shade 500 colors
        // the theme contrast colour is converted into the actual contrast
        // colour depending on the light/dark mode.
        primaryColor: QControl.prefs.gamePrimaryColorOR(QControl.gameThemePrimeColor())
        
        contrastColor: QControl.prefs.gameContrastColorOR(QControl.colorMaterialShade(QControl.gameThemeContrastColor(), Theme.isDark ? -200 : -700))

        accentColor: QControl.colorMaterialShade(theme.primaryColor, 
          Theme.isDark ? 300 : 700)

        // hardly used. only for the little line underneath current tab
        //tabHighlightColor: QControl.gameThemeTabColor();
        tabHighlightColor: "white" 


        // XX should not be necessary
        // some reason Roboto renders not weight 400 on Mac ??
        // this is still here on 5.9.6
        fontFamily: Device.isMac() ? "Arial" : sansRegular.name

        backgroundColor: QControl.prefs.gameBackgroundColorOR("white");
        backgroundShade: QControl.colorContrast(theme.backgroundColor, 0.03);
        dialogColor: Theme.isDark ? 
        QControl.colorContrast(theme.backgroundColor, 0.2) : theme.backgroundColor
    }

    QSoundPlayer
    {
        // this is to play ogg ambience and loops
        id: soundplayer1
    }
            
    function setSoundVol(v)
    {
        soundplayer1.setVolume(0, v)
        soundplayer1.setVolume(1, v)
    }

    onSoundVolChanged:
    {
        //console.log("sound voume", soundVol)
        setSoundVol(soundVol)
    }            

    function playTitleMusic()
    {
        if (!QControl.prefs.musicEnabled) return;        
        var mf = QControl.resolveAsset(QControl.getMusicFile())
        if (mf.length > 0)
        {
            setSoundVol(1.0)  // always play max
            playSound(mf, 0);
        }
    }

    function stopTitleMusic()
    {
        stopSound(0)
    }            

    function playSound(f, ch)
    {
        if (!QControl.prefs.musicEnabled) return;

        //console.log("playSound", f);
        soundplayer1.loops = -1
        soundplayer1.fade = 1000
        soundplayer1.play(ch, f)
    }

    function stopSound(ch)
    {
        soundplayer1.stop(ch)
    }          
           
    function pauseSound()
    {
        //console.log("pauseSound");
        soundplayer1.pause()
    }

    function resumeSound()
    {
        if (!QControl.prefs.musicEnabled) return;
        //console.log("resumeSound");
        soundplayer1.resume()
    }

    function updateSound()
    {
        //console.log("sound JSON ", soundJSON)
        var js = JSON.parse(soundJSON)
        var ch = js["channel"]
        if (ch == undefined) ch = 0
        var d = js["duration"]
        if (d !== undefined)
        {
            if (d == 0)
            {
                stopSound(ch)
            }
            else
            {
                var v = QControl.resolveAsset(js["name"])
                if (v.length > 0)
                {
                    setSoundVol(soundVol)
                    playSound(v, ch)
                }
            }
        }
        else stopSound(ch)
    }

    onSoundJSONChanged: updateSound();

    // force the update of sound from json.
    onForceChanged: updateSound();

    Loader
    {
        // loader to allow spash screen while we load
        anchors.fill: parent
        sourceComponent: CoverPage {}
        onLoaded: app.initialPage = item
    }

   // onClosing: {console.log("ApplicationWindow closing"); close.accepted = false; }

    QtObject 
    {
        id: fontobj
        property font afont: Qt.font({
            family: "Merriweather",
            weight: Font.Light
        })
    }

    Component.onCompleted:
    {
        if (!Device.isMobile)
        {
            x = (Screen.desktopAvailableWidth-width)/2
            y = (Screen.desktopAvailableHeight-height)/2
        }

        // ui option provides a point size
        var fs = QControl.appUIFontSize();
        if (fs)
        {
            // fs is point size
            QControl.prefs.setGameFontDefault(fontobj.afont, fs*Units.dp, false);
        }
        else
        {
            // fs is pixel size
            fs = 16;
            if (Device.isLargeMobile) fs = 20 // tablet+
            QControl.prefs.setGameFontDefault(fontobj.afont, fs*Units.dp, true)
        }

        // signal that we've started up
        QControl.uiInitialised()
        playTitleMusic()
    }
}
