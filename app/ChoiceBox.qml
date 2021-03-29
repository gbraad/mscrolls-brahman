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
import QtQml 2.2

import Material 0.3 

import com.voidware.brahman 1.0

FocusScope
{
    id: choicearea
    property int lineHeight: 48*Units.dp
    
    property string ifiChoiceJSON: QControl.ifiChoiceJSON
    property var jchoicemodel
    property bool active: false
    property int cmargin: 8*Units.dp
    property int idealHeight: jchoicemodel ? jchoicemodel.count*lineHeight + header.aheight + cmargin*2 : 0
    property bool uiTextAccepted: true

    visible: active // ifiChoiceJSON.length > 0

    signal accepted(string cmd)

    Component
    {
        id: jchoice
        ListModel 
        {
            dynamicRoles: true
        }
    }

    onIfiChoiceJSONChanged: updateJSONModel(ifiChoiceJSON)
    onVisibleChanged: if (visible) choices.forceActiveFocus()

    function singletonChoice()
    {
        var v
        if (jchoicemodel && jchoicemodel.count == 1 && uiTextAccepted == false)
        {
            v = choices.currentItem.chosen
        }
        return v
    }

    function updateJSONModel(js) 
    {
        // [{choiceobj}...]
        // {text:"heading",ui_textinput:true,choice:[{choiceobj}...]}
        // choiceobj = {"text":"whatever","chosen":"text"...}

        //console.log("update choice JSON", js)

        header.setText(null)
        active = false
        jchoicemodel = null // drop
        if (js.length > 0)
        {
            jchoicemodel = jchoice.createObject()
            var obj = JSON.parse(js);
            
            if (!Array.isArray(obj))
            {
                // choice object
                header.setText(obj["text"]);

                var v = obj["ui_textinput"];
                if (v)
                {
                    //console.log("choice text accepted")
                    uiTextAccepted = true
                }
                else
                {
                    //console.log("choice text NOT accepted")
                    uiTextAccepted = false
                }
                
                obj = obj["choice"]
            }

            if (Array.isArray(obj))
            {
                for (var c in obj) jchoicemodel.append(obj[c])
                choicearea.active = true
            }
        }
        choices.model = jchoicemodel
    }


    function acceptChoice(t)
    {
        QControl.ifiChoiceJSON = "";
        choicearea.active = false
        accepted(t)
    }

    Rectangle
    {
        id: cpane
        anchors.fill: parent
        anchors.margins: cmargin
        color: theme.backgroundShade
        
        ChoiceText
        {
            id: header
            visible: aheight > 0
            color: theme.backgroundShade

            gradient: Gradient {
                GradientStop { position: 0.0; color: header.color }
                GradientStop { position: 0.3; color: Theme.primaryColor }
                GradientStop { position: 0.7; color: Theme.primaryColor }
                GradientStop { position: 1.0; color: header.color }
            }
        }

        ListView
        {
            id: choices
            width: parent.width
            height: parent.height - header.aheight
            anchors.top: header.bottom
            clip: true
            
            delegate: ChoiceText
            {
                height: lineHeight
                width: cpane.width
                selected: ListView.isCurrentItem
                onAccept: acceptChoice(choice)

                Component.onCompleted:
                {
                    setText(model.text)
                    setChosen(model.chosen)
                }
            }
            
            
            highlight: Rectangle { color: "#e0e0e0"; radius: 5 }

            //keyNavigationWraps: true // doesnt work
            focus: true
            Keys.onReturnPressed:  acceptChoice(currentItem.chosen)
        }
    }
}

