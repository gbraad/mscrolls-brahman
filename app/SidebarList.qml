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
import Material.ListItems 0.1 as ListItem

import com.voidware.brahman 1.0


Item
{
    function help()
    {
        nohelp(false)
        mainpage.addBubble(thingstitle,
        "Drag items onto <font color=\"blue\"><u>text</u></font> to apply,<br/>or drag into blank area to drop", 4)    

        mainpage.addBubble(peopletitle,
        "Drag people onto <font color=\"blue\"><u>text</u></font> to ask about", 8)
    }

    function nohelp(v)
    {
        thingstitle.nohelp = v
        peopletitle.nohelp = v
    }
    
    implicitHeight: content.implicitHeight

    /*
    // this puts a color gradient on the sidebar/nav 
    LinearGradient
    {
        anchors.fill: parent
        start: Qt.point(0,0)
        end: Qt.point(parent.width,parent.height)

        gradient: Gradient
        {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 1.0; color: "#dae5ff" }
        }
    }
    */

    Column
    {
        id: content
        anchors.fill: parent

        ListItem.Simple
        {
            property bool nohelp: false
            id: thingstitle
            text: "YOUR THINGS"
            showDivider: true
            textColor: Theme.accentColor
            interactive: false
        }
        
        Repeater
        {
            model: itemsModel
            SidebarItem {}
        }

        ListItem.Simple
        {
            id: peopletitle
            property bool nohelp: false
            text: "PEOPLE"
            showDivider: true
            textColor: Theme.accentColor
            interactive: false
        }

        Repeater
        {
            model: peopleModel
            SidebarItem {}
        }
    }
}
