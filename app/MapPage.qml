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
import QtQuick.Layouts 1.3

import com.voidware.brahman 1.0

Page
{
    id: mappage

    title: app.title + " Map"

    property int zoomboxHeight: 32*Units.dp
    readonly property real boxSizeNormal: 10*Units.dp
    property string backImage: qmapbox.backimage
    property string mapTitle: qmapbox.mapTitle
    backgroundColor: theme.backgroundShade

    function selected()
    {
        mapbox.update()
    }

    actions: [
    Action
    {
        iconName: "action/settings"
        name: "Settings"
        hoverAnimation: true
        onTriggered: mainpage.showSettings()
    }
    ]

    Item
    {
        id: maparea
        width: parent.width
        height: parent.height - zoomboxHeight

        Image
        {
            anchors.fill: parent
            source: backImage
            visible: backImage.length > 0
            z: 0
        }

        Text
        {
            // optional map title behind boxes 
            width: parent.width
            
            text:  mapTitle
            color: Theme.subTextColor
            visible: mapTitle.length > 0
            
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 64*Units.dp
            font.family: Theme.fontFamily
            z: 1
        }

        Flickable
        {
            id: mapflick
            width: Math.min(parent.width - Units.dp*8, contentWidth)
            height: Math.min(parent.height - Units.dp*8, contentHeight)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            z: 2
            
            contentWidth: mapbox.width
            contentHeight: mapbox.height
            clip: true

            MapBox 
            {
                id: mapbox; sc: QControl.prefs.mapBoxSize*Units.dp
            }
        }
    }

    RowLayout
    {
        height: zoomboxHeight
        spacing: 0
        anchors
        {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            leftMargin: 16*Units.dp
            rightMargin: 16*Units.dp
        }

        Slider
        {
            id: zoomslider
            Layout.fillWidth: true
            maximumValue: boxSizeNormal*2
            minimumValue: boxSizeNormal/3
            stepSize: (maximumValue - minimumValue)/10
            value: QControl.prefs.mapBoxSize*Units.dp
            
            property bool ready: false
            onValueChanged: if (ready && !mapflick.moving) QControl.prefs.mapBoxSize = value/Units.dp
            Component.onCompleted: ready = true
        }

        Label
        {
            Layout.preferredWidth: Units.dp*50
            horizontalAlignment: Text.AlignRight
            id: zoomlabel
            style: "body1"
            text: "Zoom"
            color: Theme.subTextColor
        }
    }
}
