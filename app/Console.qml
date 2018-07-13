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
import QtQuick.Controls 2.2

import QtQuick.Controls 1.4 as C1
import QtQuick.Controls.Styles 1.4 as C1
import Material 0.3 as M
import QtQml 2.2

import com.voidware.brahman 1.0


FocusScope
{
    id: consoleinput

    property alias text: consolearea.text

    property int wordbarHeight

    property int normalTHeight
    property int maxTHeight

    property int margin: 8*M.Units.dp
    property int fSize

    clip: true

    property color bgColor: theme.backgroundShade
    property int wbh: textinput.activeFocus ? wordbarHeight : 0


    Column
    {
        spacing: 0
        width: parent.width

        WordBar 
        {
            id: wordbar
            width: parent.width
            height: wordbarHeight
            onSelected: textinput.text = wordsModel.addSuggestion(textinput.text, word)
            visible: textinput.activeFocus
        }

        M.TextField
        {
            id: textinput
            width: parent.width
            height: Math.min(consoleinput.height - wbh, maxTHeight)

            placeholderText: "type here"
            focus: true

            fontSize: height*6/10
            showBorder: false
            inputMethodHints: Qt.ImhNoPredictiveText

            onTextChanged: wordsModel.seedText(text)

            Keys.onTabPressed: text = wordsModel.addBestSuggestion(text)

            onAccepted:
            {
                var t = text
                text = "";
                qconsole.appendText(t)
            }
        }
  
        ScrollView
        {
            width: parent.width
            height: consoleinput.height - textinput.height - wbh
            clip: true
            
            TextArea
            {
                id: consolearea

                leftPadding: margin
                rightPadding: margin
                topPadding: margin
                bottomPadding: margin

                readOnly: true
                color: M.Theme.textColor

                font.pixelSize: consoleinput.fSize
                font.family: M.Device.isMac() ? "Arial" : sansRegular.name
                
                text: qconsole.text
                background: Rectangle
                {
                    color: QControl.colorContrast(bgColor, 0.05);
                }
                cursorPosition: length

                MouseArea
                {
                    anchors.fill: parent
                    onClicked:
                    {
                        var t = qconsole.lineAt(consolearea.positionAt(mouseX, mouseY));
                        if (t.length > 0) textinput.text = t;
                    }
                }
            }
        }
    }
}
