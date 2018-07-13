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
import QtQuick.Controls 2.2
import Material.ListItems 0.1 as ListItem
import QtQml 2.2

import com.voidware.brahman 1.0

Dialog 
{
    id: fontselector
    title: "Select Font"

    property int maxWidth: 400
    property int maxHeight: 400
    
    property var currentFont: Qt.application.font
    property var font
    property var fonts: Qt.fontFamilies()
    backgroundColor: app.theme.dialogColor

    onCurrentFontChanged: font = Qt.font(currentFont) // copy
    allowFlick: false

    function update()
    {
        var ix = fonts.indexOf(font.family)
        if (ix >= 0)
        {
            fontlist.currentIndex = ix
            fontlist.positionViewAtIndex(ix, ListView.Visible)
        }
    }

    Item
    {
        id: content
        width: maxWidth
        height: maxHeight

        Item
        {
            id: sizeselector
            width: parent.width
            height: Units.dp*64

            ListItem.Standard
            {
                anchors.fill: parent
                text: "SIZE"
                selected: true // stand out
                rowspacing: 8*Units.dp
                secondaryItem: SpinBox
                {
                    height: parent.height
                    width: Math.min(200*Units.dp, sizeselector.width*2/3)
                    value: fontselector.font.pointSize
                    editable: true
                    from: 6
                    to: 72
                    font.family: Theme.fontFamily
                    font.pixelSize: Units.dp*20
                    onValueChanged: fontselector.font.pointSize = value
                }
            }
        }
        
        ListView 
        {
            id: fontlist
            anchors
            {
                top: sizeselector.bottom
                topMargin: Units.dp*16
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }
            
            model: fonts
            clip: true
            snapMode: ListView.SnapToItem

            //highlightFollowsCurrentItem: true
            //highlightMoveVelocity: Units.dp*800
            //highlight: Rectangle { color: "lightsteelblue"; radius: 5 }

            focus: true

            delegate: ListItem.Simple
            {
                width: fontlist.width
                text: modelData
                label.font.family: modelData

                // XX workaround for text crash bug in Qt5.8
                label.renderType: Text.NativeRendering
                
                onClicked:
                {
                    fontlist.currentIndex = index
                    font.family = text
                }
                selected: index == fontlist.currentIndex
            }

            Component.onCompleted: fontselector.update();
        }
    }
}
