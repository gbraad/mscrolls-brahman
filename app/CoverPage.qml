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
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import Material 0.3
import QtQml 2.2

import com.voidware.brahman 1.0

Page
{
    title: app.title

    // shadow
    property int xw: 4*Units.dp
    property int xh: 4*Units.dp

    Image
    {
        z:1
        anchors.fill: parent
        source: app.coverImage
        visible: coverImage.length > 0
    }

    Item
    {
        id: titlearea
        anchors.fill: parent;
        z: 3

        Text
        {
            id: titleMeasure
            text: app.title
            font.family: tarea.coverFont
            font.pixelSize: tarea.th
            visible: false
        }

        TextArea
        {
            id: tarea
            property bool portrait: parent.height >= parent.width
            property int th: portrait ? width/6 : width/8
            property int hmargin: Units.dp*64
            property string coverFont: app.coverTextFont

            property real s: titleMeasure.implicitWidth > 0 ? Math.min(titleMeasure.implicitWidth, tarea.width - hmargin)/titleMeasure.implicitWidth : 0;
            
            anchors
            {
                fill: parent
                topMargin: portrait ? Units.dp*64 : Units.dp*8
            }

            textFormat: TextEdit.RichText
            text: th ? QControl.formatCoverText(coverFont,
                                        th*.80,
                                        s*th,
                                        app.coverTextWeight,
                                        app.organisation,
                                        app.title,
                                        app.author) : ""
            color: app.coverTextColor
            readOnly: true
            renderType: Device.isMac() ? Text.NativeRendering : Text.QtRendering
            
            background: Item {}

            MouseArea
            {
                anchors.fill: parent
                onClicked: 
                {
                    enabled = false; // prevent a second click!
                    app.stopMusic()
                    pageStack.replace(Qt.resolvedUrl("MainPage.qml"))
                    QControl.coverPageClosed()
                }
            }
        }
    }

    DropShadow
    {
        z: 3
        anchors.fill: titlearea
        verticalOffset: xh
        horizontalOffset: xw
        radius: 6*Units.dp
        samples: 1+radius*2
        color: "#60000000"
        source: titlearea
        cached: true
        //visible: QControl.gameCoverTextColor() == "black"
    }
    
    Loader
    {
        z: 2
        anchors.fill: parent;
        active: app.coverEffect.length > 0
        source: Qt.resolvedUrl("shaders/" + app.coverEffect + ".qml");
    }
}

