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
import QtQuick.Controls 2.2 as C2
import QtQuick.Controls.Material 2.1 as C2

import Material 0.3
import Material.ListItems 0.1 as ListItem
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2 as C1
import QtQml 2.2

import com.voidware.brahman 1.0

Page
{
    id: settingspage
    
    title: "Settings"

    readonly property int dialogWidth: width*3/4
    readonly property int dialogHeight: height*8/10
    
    readonly property string marketAndroid: QControl.gameMarketAndroid()
    readonly property string marketIOS: QControl.gameMarketIOS()
    
    Flickable
    {
        anchors.fill: parent
        contentHeight: Math.max(content.implicitHeight, height)
        
        Column 
        {
            id: content
            anchors.fill: parent

            ListItem.Standard
            {
                text: "GAME"
                showDivider: true
                textColor: Theme.accentColor
            }

            ListItem.Subtitled
            {
                text: "Save"
                subText: "Save game to file"
                onClicked:
                {
                    saveselectorLoader.asynchronous = false
                    saveselectorLoader.item.open()
                }
                visible: app.enableSaveLoad
            }

            ListItem.Subtitled
            {
                text: "Load"
                subText: "Load game from file"
                onClicked:
                {
                    loadselectorLoader.asynchronous = false
                    loadselectorLoader.item.open()
                }
                visible: app.enableSaveLoad
            }

            ListItem.Subtitled
            {
                text: "Modern Mode"
                subText: "Enable Modern Gameplay Style"
                secondaryItem: Switch
                {
                    id: modernSwitch
                    anchors.verticalCenter: parent.verticalCenter
                    checked: QControl.prefs.modernEnabled
                    onCheckedChanged:
                    {
                        if (QControl.prefs.modernEnabled != checked)
                        {
                            QControl.prefs.modernEnabled = checked
                            if (!checked) modernEnableDialog.show()
                        }
                    }
                }
                onClicked: modernSwitch.checked = !modernSwitch.checked
                visible: app.enableClassic
            }

            ListItem.Subtitled
            {
                text: "Restart Game"
                subText: "Lose all progress and start over"
                interactive: false
                secondaryItem: MButton
                {
                    backgroundColor: app.theme.accentColor
                    elevation: 2
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height/2
                    width: Math.max(Math.min(content.width/2, Units.dp*80), implicitWidth)
                    text: "Reset"
                    onClicked: resetConfirm.show()
                }
                visible: app.enableRestart
            }

            ListItem.Subtitled
            {
                text: "Show Commands"
                subText: "Show commands in main text window"
                secondaryItem: Switch
                {
                    id: consoleechoSwitch
                    anchors.verticalCenter: parent.verticalCenter
                    checked: QControl.prefs.consoleechoEnabled
                    onCheckedChanged: QControl.prefs.consoleechoEnabled = checked
                }
                onClicked: consoleechoSwitch.checked = !consoleechoSwitch.checked
                visible: app.enableTextInput
            }


            ///////////////////

            ListItem.Standard
            {
                text: "USER INTERFACE"
                showDivider: true
                textColor: Theme.accentColor
            }

            ListItem.Subtitled
            {
                text: "Font"
                subText: "Select font for game window"
                onClicked: 
                {
                    fontDialogLoader.asynchronous = false
                    fontDialogLoader.item.open()
                }
            }

            ListItem.Subtitled
            {
                text: "Image Scaling"
                subText: "Resize images to fit"
                secondaryItem: Switch
                {
                    id: imagescaleSwitch
                    anchors.verticalCenter: parent.verticalCenter
                    checked: QControl.prefs.imagescaleEnabled
                    onCheckedChanged: QControl.prefs.imagescaleEnabled = checked
                }
                onClicked: imagescaleSwitch.checked = !imagescaleSwitch.checked
            }


            ListItem.Subtitled
            {
                text: "Image Pixel Scaling"
                subText: "Apply pixel scaling algorithms"
                secondaryItem: Switch
                {
                    id: imagepixscaleSwitch
                    anchors.verticalCenter: parent.verticalCenter
                    checked: QControl.prefs.imagepixscaleEnabled
                    onCheckedChanged: QControl.prefs.imagepixscaleEnabled = checked
                }
                onClicked: imagepixscaleSwitch.checked = !imagepixscaleSwitch.checked
                visible: app.enableClassic
            }

            ListItem.Subtitled
            {
                text: "Image Adjustment"
                subText: "Apply image colour adjustments"
                secondaryItem: Switch
                {
                    id: imageadjSwitch
                    anchors.verticalCenter: parent.verticalCenter
                    checked: QControl.prefs.imageadjEnabled
                    onCheckedChanged: QControl.prefs.imageadjEnabled = checked
                }
                onClicked: imageadjSwitch.checked = !imageadjSwitch.checked
                visible: app.enableClassic
            }

            ListItem.Subtitled
            {
                text: "Compass Moves"
                subText: "Compass moves with image pull-down"
                secondaryItem: Switch
                {
                    id: compassmoveSwitch
                    anchors.verticalCenter: parent.verticalCenter
                    checked: QControl.prefs.compassmoveEnabled
                    onCheckedChanged: QControl.prefs.compassmoveEnabled = checked
                }
                onClicked: compassmoveSwitch.checked = !compassmoveSwitch.checked
                visible: app.enableCompass
            }

            ListItem.Subtitled
            {
                text: "Change Theme"
                subText: "Select color theme for App"
                interactive: false
                secondaryItem: MButton
                {
                    backgroundColor: app.theme.accentColor
                    elevation: 2
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height/2
                    width: Math.max(Math.min(content.width/2, Units.dp*80), implicitWidth)
                    text: "Select"
                    onClicked: 
                    {
                        themeDialogLoader.asynchronous = false
                        themeDialogLoader.item.open()
                    }
                }
            }

            ListItem.Subtitled
            {
                text: "Global Scale"
                subText: "Resize Everything"
                /*clipContent: false // allow bubble to overlap
                secondaryItem:  Slider
                {
                    property bool ready: false
                    
                    tickmarksEnabled: false
                    numericValueLabel: true
                    stepSize: 0.2
                    minimumValue: 0.4
                    maximumValue: 2.0
                    knobLabel: value.toFixed(1)
                    value: QControl.prefs.dpScale
                    onValueChanged: if (ready) QControl.prefs.dpScale = value
                    Component.onCompleted: ready = true
                }
                */
                onClicked:
                {
                    scaleDialogLoader.asynchronous = false
                    scaleDialogLoader.item.open()
                }
            }

            ///////////////////

            ListItem.Standard
            {
                text: "MISC"
                showDivider: true
                textColor: Theme.accentColor
            }

            ListItem.Subtitled
            {
                text: "Music"
                subText: "Enable Music"
                secondaryItem: Switch
                {
                    id: musicSwitch
                    anchors.verticalCenter: parent.verticalCenter
                    checked: QControl.prefs.musicEnabled
                    onCheckedChanged: QControl.prefs.musicEnabled = checked
                }
                onClicked: musicSwitch.checked = !musicSwitch.checked
            }
            
            ListItem.Subtitled
            {
                text: "Update"
                subText: "Enable Check For Updates"
                visible: !Device.isMobile && QControl.gameEnableUpdate()
                secondaryItem: Switch
                {
                    id: updateSwitch
                    anchors.verticalCenter: parent.verticalCenter
                    checked: QControl.prefs.updateEnabled
                    onCheckedChanged: QControl.prefs.updateEnabled = checked
                }
                onClicked: updateSwitch.checked = !updateSwitch.checked
            }

            ListItem.Subtitled
            {
                text: "Tutorial"
                subText: "Restart on screen help"
                interactive: false
                secondaryItem: MButton
                {
                    backgroundColor: app.theme.accentColor
                    elevation: 2
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height/2
                    width: Math.max(Math.min(content.width/2, Units.dp*80), implicitWidth)
                    text: "Reset"
                    onClicked: QControl.prefs.resetTutorialLevel(0);
                }
            }

            ListItem.Subtitled
            {
                text: "Credits"
                subText: "People behind this project"
                onClicked: creditsDialog.open()
            }

            ListItem.Subtitled
            {
                text: "Rate Game"
                subText: "Open App Store"
                visible: Device.isAndroid() && marketAndroid.length > 0
                onClicked: Qt.openUrlExternally(marketAndroid)
            }

            ListItem.Subtitled
            {
                text: "Rate Game"
                subText: "Open App Store"
                visible: Device.isIOS() && marketIOS > 0
                onClicked: Qt.openUrlExternally(marketIOS)
            }

            ListItem.Subtitled
            {
                text: "About"
                subText: 
                {
                    var sdpi = QControl.getScreenDPI()
                    var sdpi_s = sdpi > 0 ? "/" + sdpi : ""
                    var dr = QControl.devicePixelRatio()
                    var dr_s = dr > 1 ? "(" + dr + ")" : ""
                    return "Version " + QControl.currentVersion() + ", DPI: " + Units.dpi + sdpi_s + " " + Screen.width + "x" + Screen.height + dr_s + " scale: " + Units.dp.toFixed(1) + " " + Device.name + ", touch: " + Device.hasTouchScreen;
                }
                onClicked: if (QControl.gameEnableUpdate()) QControl.checkForUpdates()
            }

            Messenger
            {
                text: QControl.currentMessage
                color: "transparent"
            }
        }
    }

    Snackbar { id: settingsSnackbar }

    Dialog 
    {
        id: modernEnableDialog
        title: "Classic Mode"
        negativeButtonText: ""
        width: settingspage.dialogWidth
        backgroundColor: app.theme.dialogColor
        
        text: "By disabling Modern Mode, you have selected Classic mode, where some of the new helpful features are disabled, and you're on your own!"
    }

    Dialog 
    {
        id: creditsDialog
        negativeButtonText: ""
        width: dialogWidth
        height: dialogHeight
        backgroundColor: app.theme.dialogColor

        onOpened: app.playMusic()
        onClosed: app.stopMusic()

        Column
        {
            spacing: 0
            width: parent.width
            
            Item
            {
                // header spacer 
                width: parent.width
                height: creditsDialog.contentMargins
            }
            
            Text
            {
                width: parent.width
                wrapMode: Text.Wrap
                color: Theme.textColor
                text: app.gameCredits
                textFormat: Text.RichText
                font.family: Theme.fontFamily
                font.pixelSize: Units.dp*14
                onLinkActivated: Qt.openUrlExternally(link);
            }
        }
    }

    Dialog 
    {
        id: resetConfirm
        backgroundColor: app.theme.dialogColor
        title: "Reset Game"
        centerTitle: true
        text: "Are You Sure?"
        positiveButtonText: "Yes"
        negativeButtonText: "Cancel"
        onAccepted: QControl.restartGame()
    }

    Loader
    {
        id: themeDialogLoader
        asynchronous: true

        sourceComponent: ThemeSelector
        {
            height: dialogHeight
        }
    }

    Loader
    {
        id: saveselectorLoader
        asynchronous: true

        sourceComponent: SaveLoadSelector 
        {
            maxWidth: dialogWidth
            maxHeight: dialogHeight
            onAccepted: settingsSnackbar.open(QControl.saveGame(filename) ? "OK" : "Save FAILED!")
        }
    }

    Loader
    {
        id: loadselectorLoader
        asynchronous: true

        sourceComponent: SaveLoadSelector
        {
            maxWidth: dialogWidth
            maxHeight: dialogHeight
            saveMode: false
            onAccepted: settingsSnackbar.open(QControl.loadGame(filename) ? "OK" : "Load Failed")
        }
    }

    Loader
    {
        id: fontDialogLoader
        asynchronous: true

        // our version
        sourceComponent: FontSelector
        {
            maxWidth: dialogWidth
            maxHeight: dialogHeight
            currentFont: QControl.prefs.gameFont            
            onAccepted: QControl.prefs.gameFont = font
        }
    }

    Loader
    {
        id: scaleDialogLoader
        asynchronous: true

        sourceComponent: ScaleSelector
        {
            maxWidth: dialogWidth
            maxHeight: 0.2*dialogHeight
            currentScale: QControl.prefs.dpScale*100
            onAccepted: QControl.prefs.dpScale = currentScale/100
        }
    }
}
