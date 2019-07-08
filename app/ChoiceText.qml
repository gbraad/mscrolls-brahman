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


import QtQuick 2.12
//import QtQuick.Controls 2.2

import Material 0.3 

Item
{
    id: ctext
    property string chosen
    property color textColor: Theme.textColor
    property bool selected: false
    property alias text: label.text
    property int normalSize: 16*Units.dp
    
    property int aheight: text.length > 0 ? 48*Units.dp : 0

    height: aheight
    width: parent.width
    
    signal accept(string choice)

    function setText(t)
    {
        if (t == null)
        {
            label.text = null

            label.font.bold = false
            label.font.italic = false
            label.font.pixelSize = normalSize
            label.font.family = Theme.fontFamily
            textColor = Theme.textColor
        }
        else
        {
            if (typeof t === 'object')
            {
                setText(t["text"])
                
                var c = t["color"]
                if (c) textColor = c

                c = t["bold"]
                if (c) label.font.bold = true
                
                c = t["italic"]
                if (c) label.font.italic = true

                c = t["size"]
                if (c) label.font.pixelSize = c*Units.dp
                
                c = t["family"]
                if (c) label.font.family = c
            }
            else
            {
                label.text = t
            }
        }
    }

    function setChosen(t)
    {
        if (t != null) chosen = t
        else chosen = label.text
    }
    
    Text
    {
        id: label
        
        height: parent.height
        leftPadding: 16*Units.dp
        rightPadding: 16*Units.dp
        
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        color: selected ? Theme.primaryColor : textColor
        font.pixelSize: normalSize
        font.family: Theme.fontFamily
    }
    
    MouseArea
    {
        anchors.fill: parent
        onClicked: accept(chosen)
    }
}
