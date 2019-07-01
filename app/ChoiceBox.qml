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
//import QtGraphicalEffects 1.0
import QtQml 2.2

import Material 0.3 

import com.voidware.brahman 1.0

FocusScope
{
    id: choicearea
    property int lineHeight: 48*Units.dp
    
    property int headerHeight: headerText.length > 0 ? lineHeight : 0
    property string ifiChoiceJSON: QControl.ifiChoiceJSON
    property string headerText
    property int hmargin: 16*Units.dp

    visible: ifiChoiceJSON.length > 0

    ListModel { id: jchoicemodel }

    height: jchoicemodel.count*lineHeight + headerHeight

    onVisibleChanged: if (visible) choices.forceActiveFocus()
    

    function updateJSONModel(js) 
    {
        // [{choiceobj}...]
        // {text:"heading",choice:[{choiceobj}...]}
        // choiceobj = {"text":"whatever","chosen":"text","enabled":true}

        jchoicemodel.clear();
        headerText = ""
        if (js.length > 0)
        {
            var obj = JSON.parse(js);
            
            if (!Array.isArray(obj))
            {
                // choice object
                var t = obj["text"];
                if (t != null && t.length > 0) headerText = t;

                obj = obj["choice"]
            }

            if (Array.isArray(obj))
            {
                for (var c in obj) jchoicemodel.append(obj[c]);
            }
        }

    }

    onIfiChoiceJSONChanged: updateJSONModel(ifiChoiceJSON)

    function acceptChoice(t)
    {
        QControl.ifiChoiceJSON = "";
        QControl.evalJSON(t)
    }

    Rectangle
    {
        anchors.fill: parent
        color: theme.backgroundShade
        
        ChoiceText
        {
            id: header
            text: headerText
            height: headerHeight
            visible: headerHeight > 0
        }

        ListView
        {
            id: choices
            width: parent.wdith
            height: choicearea.height - headerHeight
            anchors.top: header.bottom
            
            model: jchoicemodel
            delegate: ChoiceText
            {
                height: lineHeight
                width: choicearea.width

                text: model.text
                chosen: model.chosen || null
                color: ListView.isCurrentItem ? Theme.primaryColor : Theme.textColor  
                onAccept: acceptChoice(choice)
            }
            
            
            highlight: Rectangle { color: "#e0e0e0"; radius: 5 }

            //keyNavigationWraps: true
            focus: true
            Keys.onReturnPressed:  acceptChoice(currentItem.response)
        }
    }
}

