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
    id: scaleselector
    title: "Select Scale"

    property int maxWidth: 400
    property int maxHeight: 400
    
    property alias currentScale: spinbox.value
    backgroundColor: app.theme.dialogColor

    // enough space for text left of spinbox
    // otherwise squash text 
    property bool seeText: width >= 330*Units.dp

    Item
    {
        id: content
        width: maxWidth
        height: maxHeight

        ListItem.Subtitled
        {
            id: scaleitem
            width: parent.width
            text: seeText ? "Scale factor" : "Factor"
            subText: seeText ? "20% - 200%" : "20-200%"
            selected: true // stand out
            rowspacing: 0
            rightMargin: 0
            leftMargin: (seeText ? 16 : 8)*Units.dp
            secondaryItem: SpinBox
            {
                id: spinbox
                height: parent.height
                width: Math.min(180*Units.dp, content.width)
                editable: true
                from: 20
                to: 200
                stepSize: 10
                font.family: Theme.fontFamily
                font.pixelSize: Units.dp*20
            }
        }
    }
}
