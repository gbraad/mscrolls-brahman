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
import Material 0.3 as QM

import com.voidware.brahman 1.0

Item
{
    property int thumbHeight: 30*QM.Units.dp
    id: item

    property alias content: contentarea.data
    property alias rollcontent: rollarea.data
    property alias thumbcontent: rollthumb.data
    property alias thumbColor: rollthumb.color
    
    property int rollcontentheight: 0

    property int yPos: height - thumbHeight

    // height of drawer open 
    property int rollHeightShown: yPos - flick.contentY

    Timer
    {
        id: delayer
        interval: 500
        onTriggered: bump()
    }

    function bump()
    {
        if (visible)
        {
            if (height == 0)
            {
                // the first picture change comes here, but we are not
                // ready. why? who knows. something to do with QML not
                // being set up all at once. 
                // anyhow, if this is the case, we wait a short time then try again
                delayer.running = true
            }
            else if (rollHeightShown < 2*thumbHeight) bump_anim.start();
        }
    }

    function unbump()
    {
        if (visible) back_anim.start()
    }

    Flickable
    {
        id: flick
        width: parent.width
        height: (moving || ok) ? item.height : item.height - contentY

        contentWidth: item.width
        contentHeight: item.height
        bottomMargin: (moving || ok) ? yPos : 0
        contentY: yPos
        z:1

        property bool ok
        
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds

        topMargin: rollcontentheight ? Math.min(rollcontentheight - yPos, 0) : thumbHeight - yPos;

        onMovingChanged: ok = false

        Item
        {
            id: roll
            width: item.width
            height: item.height

            Item
            {
                // placement for the roller content
                id: rollarea
                width: parent.width
                height: yPos
            }

            Rectangle
            {
                id: rollthumb
                width: parent.width
                height: thumbHeight
                y: yPos
                color: "red"

                Component.onCompleted: 
                mainpage.addBubble(rollthumb, "Roll down to reveal picture", 2)
            }

            NumberAnimation
            {
                id: back_anim
                target: flick
                property: "contentY"
                to: yPos
                easing.type: Easing.OutBounce
                easing.amplitude: 0.5
                duration: 500
            }

            NumberAnimation
            {
                id: open_anim
                target: flick
                property: "contentY"
                to: -flick.topMargin
                easing.type: Easing.OutBounce
                easing.amplitude: 0.5
                duration: 500
            }

            SequentialAnimation
            {
                id: bump_anim
                readonly property int bumpval: QControl.prefs.imagescaleEnabled ? thumbHeight*4 : thumbHeight*2
                
                NumberAnimation
                {
                    target: flick
                    property: "contentY"
                    to: yPos - bump_anim.bumpval
                    duration: 500
                }

                NumberAnimation
                {
                    // delay
                    target: flick
                    property: "contentY"
                    to: yPos - bump_anim.bumpval
                    duration: 500
                }

                NumberAnimation
                {
                    target: flick
                    property: "contentY"
                    to: yPos
                    duration: 500
                }
            }

            MouseArea
            {
                // click on thumb
                anchors.fill: parent
                onPressed: flick.ok = true
                onClicked:
                {
                    flick.ok = false
                    if (flick.contentY == yPos) open_anim.start();
                    else back_anim.start();
                }
            }

            DropShadow
            {
                anchors.fill: rollthumb
                verticalOffset: 4*QM.Units.dp
                radius: 6*QM.Units.dp
                samples: 1+radius*2
                color: "#60000000"
                source: rollthumb
            }
        }
    }

    Item
    {
        // game text goes here
        id: contentarea
        width: parent.width
        height: yPos
        y: thumbHeight
    }
}


