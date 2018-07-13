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
import QtQml 2.2

import com.voidware.brahman 1.0

Item
{
    id: item
    width: parent.width
    height: listitem.height

    property int xpos
    property int ypos

    property int hotx
    property int hoty

    ParallelAnimation
    {
        id: back_anim
        NumberAnimation
        {
            target: item
            property: "x"
            to: xpos
            easing.type: Easing.OutBounce
            easing.amplitude: 1
        }

        NumberAnimation
        {
            target: item
            property: "y"
            to: ypos
            easing.type: Easing.OutBack
            easing.overshoot: 1
        }
    }

    Drag.active: dragarea.drag.active
    Drag.hotSpot: Qt.point(hotx, hoty)

    MouseArea 
    {
        id: dragarea
        anchors.fill: parent
        drag.target: item
        drag.threshold: 20*Units.dp
        drag.filterChildren: true // handle drag
        
        onPressed:
        {
            xpos = item.x
            ypos = item.y
            hotx = Device.isMobile ? item.width/2 : mouseX
            hoty = Device.isMobile ? item.height/2 : mouseY
        }
        
        onReleased:
        {
            listitem.state = "" // revert listitem.text

            var dlink = gamepage.droplink
            setDroplink("");

            if (item.Drag.target == droparea)
            {
                if (dlink.length > 0 &&
                QControl.evalUseXwithY("item" + listitem.itemid, dlink)) ;
                else
                QControl.menuDrop(listitem.itemid)
            }
            item.Drag.cancel()
            back_anim.start()
        }

        ListItem.Standard
        {
            id: listitem
            text: model.name
            showDivider: true
            dividerInset: Units.dp*16
            
            property string itemid: model.itemid
            property string iconp: model.icon

            // use any direct icon if given
            iconSource: iconp ? iconp : QControl.itemIcon(itemid)
            iconColor: Theme.isDark ? Theme.textColor : Theme.primaryColor

            states: [
            State
            {
                // highlight when over a possible drop target
                when: dragarea.drag.active && gamepage.droplink.length > 0
                PropertyChanges
                {
                    target: listitem
                    tintColor: QControl.colorAlpha(app.theme.accentColor, 0.7)
                    text: gamepage.droplink
                    textColor: Theme.isDarkColor(tintColor) ? "white" : "black"
                }                        
            },
            State
            {
                // highlight when dragging
                when: dragarea.drag.active
                PropertyChanges
                {
                    target: listitem
                    tintColor: Theme.isDark ? "#60FFFFFF" : "#60000000"
                    textColor: Theme.textColor
                }
                StateChangeScript { script: close(); }
            },
            State
            {
                // reset when not dragging
                when: !dragarea.drag.active
                PropertyChanges
                {
                    target: listitem
                    textColor: Theme.textColor
                }
                StateChangeScript { script: close(); }
            }
            ]

            onClicked: 
            {
                close()
                QControl.menuClick(itemid)
            }
        }
    }
}

