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
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import com.voidware.brahman 1.0

Page
{
    id: gamepage
    title: app.title

    backAction: navDrawer.action

    function selected() {}

    property string droplink
    property string currentImage
    property bool picValid

    onCurrentImageChanged:
    {
        pic.play(currentImage)
        if (picValid) gamearea.bump();
        else gamearea.unbump(); 
    }

    function setDroplink(s)
    {
        droplink = s
        //if (droplink.length > 0) console.log("drag to ", droplink)
    }

    actions: [
    Action
    {
        iconName: "other/download"
        name: "Save"
        hoverAnimation: true
        visible: !Device.isMobile && app.enableSaveLoad
        onTriggered:
        {
            saveselectorLoader.asynchronous = false
            saveselectorLoader.item.open()
        }

    },
    Action
    {
        iconName: "other/upload"
        name: "Restore"
        hoverAnimation: true
        visible: !Device.isMobile && app.enableSaveLoad
        onTriggered:
        {
            loadselectorLoader.asynchronous = false
            loadselectorLoader.item.open()
        }
    },
    Action
    {
        iconName: "navigation/refresh"
        name: "Refresh"
        hoverAnimation: true
        onTriggered: QControl.refreshCommand()
    },
    Action
    {
        iconName: "content/undo"
        name: "Undo"
        onTriggered: snackbar.open(QControl.undoredo(true))
        visible: QControl.gameEnableUndoRedo()
    },
    Action
    {
        iconName: "maps/my_location"
        name: "Compass"
        hoverAnimation: true
        onTriggered: game.dtoggle()
        visible: app.enableCompass
    },
    Action
    {
        iconName: "content/redo"
        name: "Redo"
        onTriggered: snackbar.open(QControl.undoredo(false))
        visible: QControl.gameEnableUndoRedo()
    },
    Action
    {
        iconName: "action/settings"
        name: "Settings"
        hoverAnimation: true
        onTriggered: mainpage.showSettings()
    }
    /*
    Action
    {
        iconName: "action/help"
        name: "Help"
        hoverAnimation: true
        //onTriggered: mainpage.showSettings()
    },
    */
    ]

    function close()
    {
        navDrawer.close();
    }

    NavigationDrawer 
    {
        id: navDrawer
        drawerName: "Inventory"

        navenabled: gamepage.width < Units.dp*800 && app.enableSidebar

        onOpened: sidebarlist.help()
        onClosed: sidebarlist.nohelp(true)

        Flickable 
        {
            id: navFlick
            width: parent.width
            height: parent.height - msger.height
            contentHeight: Math.max(sidebarlist.implicitHeight, height)
            
            MouseArea 
            {
                anchors.fill: parent
                z:-1
                onPressed: close()
            }

            SidebarList 
            {
                id: sidebarlist 
                anchors.fill: parent
            }
        }

        Messenger
        {
            id: msger
            anchors.top: navFlick.bottom
            text: QControl.currentMessage
        }
        
        Scrollbar { flickableItem: navFlick }
    }

    Item
    {
        anchors.fill: parent
            
        // use the sidebar instead of the navdrawer when enough space
        GameSidebar 
        {
            id: sidebar
            expanded: !navDrawer.navenabled && app.enableSidebar
            width: 220 * Units.dp
            z: 1
            clipContent: false

            onExpandedChanged: 
            {
                if (expanded) sidelist.help(); 
                else sidelist.nohelp(true);
            }
            
            flicker.contentHeight: Math.max(sidelist.implicitHeight, height)

            SidebarList 
            {
                id: sidelist
                anchors.fill: parent
                Component.onCompleted: if (sidebar.expanded) sidelist.help();
            }
        }

        Roller
        {
            id: gamearea
            anchors
            {
                left: sidebar.right
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }

            thumbColor: Theme.primaryColor

            DropArea
            {
                id: droparea
                anchors.fill: parent

                onPositionChanged:
                setDroplink(QControl.tailWords(game.textarea.linkAt(drag.x, drag.y + game.contenty - gamearea.thumbHeight)))
            }
            
            content: Game
            {
                id: game 
                anchors.fill: parent
                drawerypos: QControl.prefs.compassmoveEnabled ? gamearea.rollHeightShown + Units.dp*16 : 0
            }
            
            thumbcontent: RowLayout
            {
                anchors
                {
                    fill: parent
                    leftMargin: Units.dp*16
                    rightMargin: Units.dp*16
                }
                
                Text
                {
                    // host the game status bar here. show the score etc.
                    readonly property int fsize: Units.dp*16
                    height: parent.height
                    font.pixelSize: fsize
                    font.family: Theme.fontFamily
                    text: QControl.titleText
                    color: Theme.isDarkColor(gamearea.thumbColor) ? "white" : "black"
                    Layout.alignment: Qt.AlignRight
                }
            }

            rollcontentheight: picValid ? picarea.height : 0

            rollcontent: Rectangle
            {
                id: picarea

                width: parent.width
                height: pic.aspect * width + pic.hadj*2

                anchors.bottom: parent.bottom
                color: QControl.colorContrast(theme.backgroundShade, 0.05)

                property string currentImageJSON: QControl.currentImageJSON
                property real imgBrightness: 0.0
                property real imgContrast: 0.0
                property real imgSaturation: 0.0
                property real imgLightness: 0.0
                property real imgGamma: 1.0

                onCurrentImageJSONChanged:
                {
                    //console.log("current image JSON ", currentImageJSON);
                    var js = JSON.parse(currentImageJSON);
                    var pname = js["name"]
                    var iname = ""
                    if (pname)
                    {
                        iname = QControl.resolveAsset(pname)
                        var v;
                        v = js["brightness"]
                        imgBrightness = v ? v : 0.0
                        v = js["contrast"]
                        imgContrast = v ? v : 0.0
                        v = js["saturation"]
                        imgSaturation = v ? v : 0.0
                        v = js["lightness"];
                        imgLightness = v ? v : 0.0
                        v = js["gamma"]
                        imgGamma = v ? v : 1.0
                    }
                    
                    picValid = iname.length > 0
                    currentImage = iname
                }

                AnimatedImage2
                {
                    id: pic

                    // do not use pixel blending unless new scaling selected
                    smooth: QControl.prefs.imagepixscaleEnabled

                    property int m1: (QControl.lowMargins ? 6 : 16)*Units.dp
                    property int m: scaleMode ? Math.max(4*Units.dp, m1*gamearea.rollHeightShown/sourceSize.height) : m1
                    property real aspect: sourceSize.width ? sourceSize.height/sourceSize.width : 0
                    property int hadj: Math.max(m1*(1 - aspect), 0)

                    property bool scaleMode: QControl.prefs.imagescaleEnabled

                    width: picValid ? parent.width - m*2 : 0
                    height: picValid ? (scaleMode ? Math.max(gamearea.rollHeightShown - m*2, 0): parent.height) : 0

                    anchors
                    {
                        bottom: parent.bottom
                        bottomMargin: scaleMode ? m : 0
                        horizontalCenter: parent.horizontalCenter
                    }

                    fillMode: Image.PreserveAspectFit
                    
                    // these make drawings blurred!
                    //mipmap: true
                    //antialiasing: true

                    visible: false
                    paused: gamearea.rollHeightShown <= 0 || !gamepage.visible
                }

                BrightnessContrast
                {
                    id: effect1
                    anchors.fill: pic
                    source: pic
                    brightness: QControl.prefs.imageadjEnabled ? picarea.imgBrightness : 0.0
                    contrast: QControl.prefs.imageadjEnabled ? picarea.imgContrast : 0.0
                    visible: false
                }

                HueSaturation {
                    id: effect2
                    anchors.fill: effect1
                    source: effect1
                    saturation: QControl.prefs.imageadjEnabled ? picarea.imgSaturation : 0.0
                    lightness: QControl.prefs.imageadjEnabled ? picarea.imgLightness : 0.0
                    visible: false
                }

                GammaAdjust {
                    id: effect3
                    anchors.fill: effect2
                    source: effect2
                    gamma:  QControl.prefs.imageadjEnabled ? picarea.imgGamma : 1.0;

                    visible: picValid
                    layer.enabled: Theme.isDark == false
                    layer.effect: DropShadow
                    {
                        transparentBorder: true
                        verticalOffset: 4*Units.dp
                        horizontalOffset: 4*Units.dp
                        radius: 6*Units.dp
                        samples: 1+radius*2
                        color: "#80000000"

                    }
                }
            }
        }
    }

    Loader
    {
        id: saveselectorLoader
        asynchronous: true

        sourceComponent: SaveLoadSelector 
        {
            maxWidth: app.dialogWidth
            maxHeight: app.dialogHeight
            onAccepted: snackbar.open(QControl.saveGame(filename) ? "OK" : "Save FAILED!")
        }
    }

    Loader
    {
        id: loadselectorLoader
        asynchronous: true

        sourceComponent: SaveLoadSelector
        {
            maxWidth: app.dialogWidth
            maxHeight: app.dialogHeight
            saveMode: false
            onAccepted: snackbar.open(QControl.loadGame(filename) ? "OK" : "Load Failed")
        }
    }

    QTranscript
    {
        id: qtranscript 
        linkColor: app.theme.contrastColor
        cechoColor: QControl.colorContrast(Theme.textColor, 0.2)
    }
    Snackbar { id: snackbar }
}
