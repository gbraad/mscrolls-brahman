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

    // does the text move with the image or go behind it
    property bool textFollowScrollMode: QControl.prefs.textmoveEnabled
    
    // valid for GameRoller and UCanvas
    property bool choiceActive: gamearea.item.choiceActive

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
        enabled: app.canSaveLoad
        hoverAnimation: true
        visible: !Device.isMobile && app.enableSaveLoad
        onTriggered: saveSelector.open();
    },
    Action
    {
        iconName: "other/upload"
        name: "Restore"
        enabled: app.canSaveLoad
        hoverAnimation: true
        visible: !Device.isMobile && app.enableSaveLoad
        onTriggered: loadSelector.open()
    },
    Action
    {
        iconName: "navigation/refresh"
        name: "Refresh"
        hoverAnimation: true
        enabled: !choiceActive
        onTriggered: QControl.refreshCommand()
    },
    Action
    {
        iconName: "content/undo"
        name: "Undo"
        enabled: !choiceActive
        onTriggered: snackbar.open(QControl.undoredo(true))
        visible: QControl.gameEnableUndoRedo()
    },
    Action
    {
        iconName: "maps/my_location"
        name: "Compass"
        hoverAnimation: true
        onTriggered: gamearea.item.agame.dtoggle() // invalid for uicanvas
        visible: app.enableCompass
    },
    Action
    {
        iconName: "content/redo"
        name: "Redo"
        enabled: !choiceActive
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

        Loader
        {
            // load one or the other to prevent both being active 
            // even when invisible
            id: gamearea
            anchors
            {
                left: sidebar.right
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            source: app.enableUCanvas ? "UCanvas.qml" : "GameRoller.qml"
        }
    }

    SaveLoadSelector
    {
        id: saveSelector
        maxWidth: app.dialogWidth
        maxHeight: app.dialogHeight
        onAccepted: snackbar.open(QControl.saveGame(filename) ? "OK" : "Save FAILED!")
    }

    SaveLoadSelector
    {
        id: loadSelector
        maxWidth: app.dialogWidth
        maxHeight: app.dialogHeight
        saveMode: false
        onAccepted: snackbar.open(QControl.loadGame(filename) ? "OK" : "Load Failed")
    }

    QTranscript
    {
        id: qtranscript 
        linkColor: app.theme.contrastColor
        cechoColor: QControl.colorContrast(Theme.textColor, 0.2)
    }
    Snackbar { id: snackbar }
}
