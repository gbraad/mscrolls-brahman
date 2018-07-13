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
import QtQml 2.2

import Material 0.3
import com.voidware.brahman 1.0

TabbedPage
{
    id: mainpage

    ListModel
    {
        id: pages
        ListElement { name: "Game"; file: "GamePage.qml"  }
        ListElement { name: "Map"; file: "MapPage.qml" }
    }

    QMapBox { id: qmapbox }

    Repeater
    {
        model: pages
        delegate: mytab
    }

    function showSettings()
    {
        pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
    }

    function finishOperation()
    {
        // generally stop things
        Qt.inputMethod.hide() // close any keyboard on mobiles
    }

    property var bubbleList: []

    Bubble { id: bub }
    
    Connections 
    {
        target: bub
        onClosed: showLater(500)
    }

    Timer 
    {
        id: timer
        onTriggered: showBubble()
    }

    function showLater(t)
    {
        timer.interval = t
        timer.start()
    }
    
    function addBubble(hostarea, txt, level)
    {
        if (QControl.prefs.setTutorialLevel(level))
        {
            //console.log("addbubble; ", txt)
            bubbleList.push({host:hostarea, text:txt, level:level})
            showLater(QControl.simMode() ? 1000 : 3000);
        }
    }

    function showBubble()
    {
        for (var i = 0; i < bubbleList.length; ++i)
        {
            var b = bubbleList[i]
            bubbleList.splice(i, 1)
            if (!b.host.nohelp)
            {
                bub.hostArea = b.host
                bub.text = b.text
                bub.show()
                break
            }
            else QControl.prefs.resetTutorialLevel(b.level)
        }
    }

    actionBar
    {
        integratedTabBar: app.width > app.height && QControl.lowMargins
    }

    Component
    {
        id: mytab
        Tab
        {
            title: model.name
            source: model.file
        }
    }
    
    actions: selectedTab.item.actions
    backAction: selectedTab.item.backAction
    title: selectedTab.item.title
    onSelectedTabIndexChanged:
    {
        // NB: selectedTab not yet updated here
        finishOperation();
        tabs.getTab(selectedTabIndex).item.selected();
    }

    Component.onCompleted:
    {
        onClicked: 
        if (QControl.prefs.updateEnabled) QControl.checkForUpdates()
    }
}
