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
import QtQuick.Dialogs 1.2 as C1
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

    readonly property int dialogWidth: app.width/2
    readonly property int dialogHeight: app.height/2

    function enterCommand()
    {
        var t = textinput.text
        textinput.text = "";
        if (!qconsole.evalCommand(t)) quitDialog.show();
    }

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
	    
	    // Having EnterKeyReturn breaks some Android devices
	    // So in theory this test will work, but does iOS actually need the EnterKeyReturn
	    // or is it just a cosmetic display thing.
            // EnterKey.type: M.Device.isIOS() ? Qt.EnterKeyReturn : Qt.EnterKeyDefault

            fontSize: height*6/10
            showBorder: false
            inputMethodHints: Qt.ImhNoPredictiveText

            onTextChanged: wordsModel.seedText(text)

            Keys.onTabPressed: text = wordsModel.addBestSuggestion(text)

            // When not handled by `onReturnPressed`
            // handle here, but not for IOS as it closes the window
            onAccepted: if (!M.Device.isIOS()) enterCommand();
            
            Keys.onReturnPressed:
            {
                // all systems including IOS
                event.accepted = true;
                enterCommand();
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

    Dialog 
    {
        id: quitDialog
        backgroundColor: app.theme.dialogColor
        
        title: "Game Over"
        negativeButtonText: ""
        positiveButtonText: "OK"
        
        width: dialogWidth
        height: dialogHeight

        text: "The game has ended and the application will now terminate"
        onAccepted: Qt.quit()
    }
}
