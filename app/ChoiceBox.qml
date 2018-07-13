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
import QtQuick.Controls 2.2 as C2
import QtGraphicalEffects 1.0
import QtQml 2.2

import Material 0.3 
import Material.ListItems 0.1 as ListItem

import com.voidware.brahman 1.0

Rectangle
{
    id: choicebox
    height: choices.height
    color: theme.backgroundShade
    visible: customJSON.length > 0

    ListModel { id: jchoicemodel }
                    
    property alias count: jchoicemodel.count
    property string customJSON: qtranscript.customControl
    property string pageButtonText: ""

    function updateJSONModel(js) 
    {
        jchoicemodel.clear();
        pageButtonText = ""
        if (js.length > 0)
        {
            var obj = JSON.parse(js);
            var choices = obj["choices"];
            if (choices != null)
                for ( var c in choices ) jchoicemodel.append(choices[c]);
            else
            {
                var page = obj["page"];
                if (page != null && page.length > 0) pageButtonText = page
            }
        }
    }

    onCustomJSONChanged:
    {
        //console.log("custom json ", customJSON)
        updateJSONModel(customJSON);
    }
    
    Column
    {
        id: choices
        width: parent.width

        Item
        {
            width: parent.width
            height: pagebutton.height
            visible: pageButtonText.length > 0
            
            C2.Button
            {
                id: pagebutton
                width: parent.width
                height: Units.dp*64
                text: pageButtonText
                font.family: sansRegular.name 
                font.pixelSize: height*0.5
                onClicked:
                {
                    qtranscript.customControl = "";
                    QControl.evalCommand("page");
                    
                }
            }

            ColorOverlay 
            {
                anchors.fill: pagebutton
                source: pagebutton
                color: Qt.rgba(Theme.primaryColor.r, 
                               Theme.primaryColor.g,
                               Theme.primaryColor.b,
                               0.5)
            }
        }

        Repeater
        {
            model: jchoicemodel
            ListItem.Standard
            {
                width: parent.width;
                text: model.text
                iconName: enabled ? "toggle/radio_button_checked" : "toggle/radio_button_unchecked" 
                enabled: !model.disabled 

                onClicked:
                {
                    var a = model.label
                    qtranscript.customControl = "";
                    QControl.evalCommand(a)
                }
            }
        }
    }
}

