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
    property alias color: txt.color
    property alias text: txt.text
    property string chosen
    property string response: chosen ? chosen : text

    height: 48*Units.dp
    width: parent.width

    signal accept(string choice)
    
    Text
    {
        id: txt
        
        height: parent.height
        leftPadding: 16*Units.dp
        rightPadding: 16*Units.dp
        
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        color: Theme.textColor
        font.pixelSize: 16*Units.dp
        font.family: Theme.fontFamily
    }
    
    MouseArea
    {
        anchors.fill: parent
        onClicked: accept(response)
    }
}
