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
import Material 0.3

import com.voidware.brahman 1.0

Rectangle
{
    id: wordbar
    
    color: "grey"  // color behind bar

    property color bgCol: theme.backgroundShade

    signal selected(string word)

    Flickable
    {
        anchors.fill: parent
        clip: true
        
        contentWidth: bar.width
        contentHeight: bar.height
        
        flickableDirection: Flickable.HorizontalFlick
        
        Rectangle
        {
            id: bar
            width: Math.max(wordrow.width, wordbar.width)
            height: wordbar.height
            color: bgCol

            property int wordBorderW: Units.dp

            Row
            {
                id: wordrow
                height: parent.height
                spacing: -bar.wordBorderW
                Repeater
                {
                    model: wordsModel
                    Rectangle
                    {
                        y:-bar.wordBorderW
                        height: parent.height + bar.wordBorderW*2
                        width: word.implicitWidth
                        color: "transparent"
                        border.width: bar.wordBorderW
                        border.color: Theme.dividerColor

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: wordbar.selected(word.text)
                        }
                        
                        Text
                        {
                            id: word
                            height: parent.height
                            text: model.name
                            font.pixelSize: height * 0.66
                            font.family: Theme.fontFamily
                            color: QControl.colorContrast(Theme.textColor, 0.2)
                            leftPadding: 8*Units.dp
                            rightPadding: 8*Units.dp
                            verticalAlignment: Text.AlignVCenter
                            //horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }
    }
}
