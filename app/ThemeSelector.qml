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
import QtQuick.Controls 1.4 as C1
import Material.ListItems 0.1 as ListItem
import QtQml 2.2

import com.voidware.brahman 1.0

Dialog 
{
    id: themeDialog
    
    width: selector.idealWidth
    
    allowFlick: false
    backgroundColor: app.theme.dialogColor
    centerTitle: true
    
    title: "Select Theme"
    positiveButtonText: "OK"
    negativeButtonText: "Cancel"

    property string oldPrimaryColor
    property string oldContrastColor
    property string oldBackgroundColor

    function setThemeType(dark)
    {
        if (dark)
        {
            app.theme.backgroundColor = "black"
            app.theme.contrastColor = QControl.colorMaterialShade(app.theme.contrastColor, -200)
        }
        else
        {
            app.theme.backgroundColor = "white"
            app.theme.contrastColor = QControl.colorMaterialShade(app.theme.contrastColor, -700)
        }
    }

    onOpened:
    {
        oldPrimaryColor = app.theme.primaryColor
        oldContrastColor = app.theme.contrastColor
        oldBackgroundColor = app.theme.backgroundColor
    }

    onRejected:
    {
        app.theme.primaryColor = oldPrimaryColor
        app.theme.contrastColor = oldContrastColor
        app.theme.backgroundColor = oldBackgroundColor
    }

    onAccepted:
    {
        QControl.prefs.gameBackgroundColor = app.theme.backgroundColor
        QControl.prefs.gamePrimaryColor = app.theme.primaryColor
        QControl.prefs.gameContrastColor = app.theme.contrastColor
    }

    Item
    {
        id: selector

        width: parent.width
        height: themeDialog.height -  themeDialog.vmargin

        property int textWidth: 120*Units.dp
        property int swatchWidth: 120*Units.dp
        property int spacing: 16*Units.dp

        readonly property int idealWidth: textWidth + swatchWidth + spacing + themeDialog.contentMargins*2

        Row
        {
            id: typeRow
            height: 64*Units.dp
            anchors.horizontalCenter: parent.horizontalCenter
            
            C1.ExclusiveGroup { id: themeType }
            RadioButton 
            {
                text: "Light"
                checked: Theme.isDark == false
                exclusiveGroup: themeType
                onClicked: setThemeType(false)
            }
            RadioButton 
            {
                text: "Dark"
                checked: Theme.isDark == true
                exclusiveGroup: themeType
                onClicked: setThemeType(true)
            }
        }
        
        ListView 
        {
            id: themelist
            
            anchors
            {
                top: typeRow.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }


            snapMode: ListView.SnapToItem
            clip: true

            model: matColModel
            focus: true

            delegate: ListItem.BaseListItem
            {
                id: item
                width: themelist.width
                showDivider: true
                height: 48 * Units.dp

                onClicked:
                {
                    themelist.currentIndex = index
                    app.theme.primaryColor = model.primaryColor
                    app.theme.contrastColor = QControl.colorMaterial(model.contrastName, Theme.isDark ? -200 : -700)
                }
                selected: index == themelist.currentIndex

                Row
                {
                    height: parent.height
                    spacing: selector.spacing

                    Text
                    {
                        id: rowlabel
                        width: selector.textWidth
                        height: parent.height
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        color: Theme.textColor
                        font.pixelSize: 20*Units.dp
                        font.family: Theme.fontFamily
                        text: model.name
                    }

                    Rectangle
                    {
                        anchors.verticalCenter: parent.verticalCenter
                        height: parent.height*2/3
                        width: selector.swatchWidth
                        color: model.color
                        border.width: 1
                        border.color: Theme.dividerColor

                        Rectangle
                        {
                            y:1
                            x:parent.width - width - 1
                            height: parent.height-2
                            width: 20*Units.dp
                            color: QControl.colorMaterial(model.contrastName, 500)
                        }
                    }
                }
            }
        }
    }
}
