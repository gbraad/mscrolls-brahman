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
import QtQuick.Controls 2.1
import Material.ListItems 0.1 as ListItem
import QtQml 2.2

import com.voidware.brahman 1.0

Dialog 
{
    id: saveloadselector
    property bool saveMode: true

    property alias filename: entrybox.text
    
    title: saveMode ? "Save Game" : "Load Game"

    property int maxWidth: 400
    property int maxHeight: 400
    backgroundColor: app.theme.dialogColor
    
    function update()
    {
        filelist.model = QControl.loadGameFiles()
    }

    onOpened:
    {
        update()
        entrybox.text = ""
    }

    Item
    {
        id: content
        width: maxWidth
        height: maxHeight

        TextField
        {
            id: entrybox
            width: parent.width
            placeholderText: "File name"
            fontSize: Units.dp*20
            focus: true
        }

        ListView 
        {
            id: filelist
            anchors
            {
                top: entrybox.bottom
                topMargin: Units.dp*16
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }
            
            clip: true
            snapMode: ListView.SnapToItem

            delegate: ListItem.Standard
            {
                id: item
                width: filelist.width
                text: modelData
                itemLabel.font.pixelSize: Units.dp*20
                onClicked: entrybox.text = text
                showDivider: true

                secondaryItem: Row
                {
                    height: parent.height
                    spacing: 32*Units.dp
                    
                    Icon
                    {
                        size: item.height/2
                        name: "social/share"
                        anchors.verticalCenter: parent.verticalCenter
                        visible: Device.isMobile

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: QControl.share(item.text);
                        }
                    }

                    Icon
                    {
                        size: item.height/2
                        name: "action/delete"
                        anchors.verticalCenter: parent.verticalCenter

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked:
                            {
                                deleteConfirm.filename = item.text
                                deleteConfirm.show();
                            }
                        }
                    }
                }
            }
            Component.onCompleted: update()
        }
    }

    Dialog 
    {
        property string filename
        
        id: deleteConfirm
        title: "Delete " + filename
        text: "Are You Sure?"
        positiveButtonText: "Yes"
        negativeButtonText: "Cancel"
        onAccepted: 
        {
            QControl.deleteSaveGame(filename)
            saveloadselector.update()
        }
    }
}
