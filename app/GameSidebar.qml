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
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.voidware.brahman 1.0

View
{
    backgroundColor: theme.backgroundColor

    anchors
    {
        left: mode === "left" ? parent.left : undefined
        right: mode === "right" ? parent.right : undefined
        top: parent.top
        bottom: parent.bottom
        leftMargin: expanded ? 0 : -width
        rightMargin: expanded ? 0 : -width
    }

    width: 250 * Units.dp

    property bool expanded: true
    property string mode: "left" // or "right"
    property color borderColor: Theme.dividerColor
    
    property alias flicker: flickable

    default property alias contents: contents.data

    Behavior on anchors.leftMargin {
        NumberAnimation { duration: 200 }
    }

    Behavior on anchors.rightMargin {
        NumberAnimation { duration: 200 }
    }

    Rectangle 
    {
        color: borderColor
        width: 1

        anchors 
        {
            top: parent.top
            bottom: parent.bottom
            right: mode === "left" ? parent.right : undefined
            left: mode === "right" ? parent.left : undefined
        }
    }

    Item
    {
        anchors
        {
            fill: parent
            rightMargin: mode === "left" ? 1 : 0
            leftMargin: mode === "right" ? 1 : 0
        }

        Flickable 
        {
            id: flickable

            width: parent.width
            height: parent.height - sbmsger.height
            contentHeight: Math.max(contents.height, height)
            
            Item 
            {
                id: contents
                width: flickable.width
                height: childrenRect.height
            }
        }

        Messenger
        {
            id: sbmsger
            anchors.top: flickable.bottom
            text: QControl.currentMessage
        }

        Scrollbar { flickableItem: flickable }
    }
}
