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
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0
import Material 0.3 as M

import com.voidware.brahman 1.0

Item
{
    id: item
    
    property int thumbSize: 15*M.Units.dp
    property int dheight: 400
    property int dwidth: 400

    property int dypos: 0

    property alias content: contentarea.data
    property alias dcontent: dcontent.data
    property alias dcolor: dcontent.color
    property alias dopacity: dcontent.opacity
    property alias thumbbar: rthumb
    property bool isopen: false

    // darea will have x set back to width on change 
    // which will hide the drawer, effectively closing it.
    onWidthChanged: isopen = false

    Item
    {
        id: contentarea
        width: parent.width 
        height: parent.height
    }

    NumberAnimation
    {
        id: anim_open
        target: darea
        property: "x"
        duration: 500
        easing.type: Easing.OutQuad
    }

    function open()
    {
        if (!isopen)
        {
            isopen = true
            anim_open.stop()
            anim_open.to = item.width - dwidth
            anim_open.start()
        }
    }

    function close()
    {
        if (isopen)
        {
            isopen = false
            anim_open.stop()
            anim_open.to = item.width
            anim_open.start()
        }
    }

    Rectangle
    {
        // activation touch bar
        id: rthumb
        height: dheight
        width: thumbSize
        anchors.right: parent.right
        y: item.dypos
        z: 1
        color: M.Theme.isDark ? "#30FFFFFF" : "#20000000"
        visible: !isopen && app.enableCompass

        MouseArea
        {
            anchors.fill: parent
            drag.target: darea
            drag.axis: Drag.XAxis
            drag.minimumX: item.width - dwidth
            drag.maximumX: item.width
            onReleased: open();
        }
    }

    Item
    {
        id: darea
        height: dheight
        width: dwidth
        z: 2
        x: parent.width
        y: item.dypos

        property bool dragActive: dragm.drag.active
        onDragActiveChanged: if (!dragActive) close();

        MouseArea
        {
            id: dragm
            anchors.fill: parent
            drag.target: darea
            drag.axis: Drag.XAxis
            drag.minimumX: item.width - dwidth
            drag.maximumX: item.width
            drag.filterChildren: true // handle drag
            
            Rectangle
            {
                // background panel for rose
                id: dcontent
                anchors.fill: parent
                color: M.Theme.isDark ? "#20FFFFFF" : "#10000000"
            }
        }
    }
}


