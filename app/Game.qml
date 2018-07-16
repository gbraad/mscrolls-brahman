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
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 1.4 as C1
import QtQuick.Controls.Styles 1.4 as C1

import QtQuick.Layouts 1.0
import Material 0.3 as M

import com.voidware.brahman 1.0

    
C1.SplitView
{
    id: splitview
    orientation: Qt.Vertical

    property alias textarea: area
    property alias contenty: textflick.contentY
    property alias drawerypos: rdrawer.dypos

    readonly property int compassSize: 200*M.Units.dp

    property font gameFont: QControl.prefs.gameFont

    function dtoggle()
    {
        if (rdrawer.isopen) rdrawer.close()
        else rdrawer.open()
    }

    RDrawer
    {
        id: rdrawer
        width: parent.width
        Layout.fillHeight: true

        dwidth: Math.min(Math.min(parent.width*2/3, parent.height*2/3), compassSize)
        dheight: dwidth
        dopacity: 0.7
        
        content: Rectangle 
        {
            id: transcript
            anchors.fill: parent
            onWidthChanged: qtranscript.setWidth(width);
            color: theme.backgroundColor

            Flickable 
            {
                id: textflick
                anchors
                {
                    fill: parent
                    bottomMargin: M.Device.hasTouchScreen ? Math.max(gameFont.pixelSize*1.5,20*M.Units.dp) : 10*M.Units.dp

                }
                flickableDirection: Flickable.VerticalFlick
                contentHeight: Math.max(flickarea.height, height)
                clip: true

                function ensureVisible(r)
                {
                    if (contentY >= r.y) contentY = r.y;
                    else if (contentY+height < r.y+r.height)
                    contentY = r.y+r.height-height;
                }

                onContentHeightChanged: 
                if (choicebox.visible) contentY = contentHeight - height;

                Column
                {
                    id: flickarea
                    width: parent.width
                    
                    TextEdit
                    {
                        id: area
                        wrapMode: TextArea.Wrap
                        textMargin: (QControl.lowMargins ? 8 : 16)*M.Units.dp
                        color: M.Theme.textColor
                        font: gameFont
                        width: parent.width
                        height: Math.max(implicitHeight, textflick.height - choicebox.height)

                        text: qtranscript.textHTML
                        textFormat: TextEdit.RichText
                        readOnly: true

                        // clicked on active text
                        onLinkActivated:
                        {
                            if (link.startsWith("http://")) Qt.openUrlExternally(link);
                            else QControl.evalClickCommand(link)
                        }

                        cursorPosition: length
                        onCursorRectangleChanged:
                        textflick.ensureVisible(cursorRectangle)

                        // this is to allow text selection in the game
                        // window on desktop right mouse button
                        // you can then C-c to copy etc.
                        MouseArea
                        {
                            id: rightbutton
                            anchors.fill: parent
                            acceptedButtons: Qt.RightButton 

                            property int start
                            
                            onPressed: 
                            {
                                area.deselect()
                                start = area.positionAt(mouseX, mouseY)
                                textflick.interactive = false
                            }
                            onPositionChanged: area.select(start, area.positionAt(mouseX, mouseY))
                            onReleased: textflick.interactive = true
                        }
                    }
                    
                    ChoiceBox 
                    {
                        id: choicebox
                        width: parent.width
                    }
                }

                Keys.onPressed:
                {
                    if (event.key == Qt.Key_Up || event.key == Qt.Key_PageUp)
                        scrollbar.decrease();
                    if (event.key == Qt.Key_Down || event.key == Qt.Key_PageDown)
                        scrollbar.increase();
                }

                ScrollBar.vertical: ScrollBar
                {
                    id: scrollbar 
                    stepSize: textflick.contentHeight ?
                    textflick.height * 0.9/textflick.contentHeight : 0
                }
            }
        }

        dcontent: Compass
        {
            id: compass
            anchors.fill: parent
            fgColor: app.theme.contrastColor
        }

        Component.onCompleted:
        if (app.enableCompass)
        mainpage.addBubble(rdrawer.thumbbar, "Tap to show compass", 1)
    }

    Console
    {
        id: textconsole
        width: parent.width
        fSize: gameFont.pixelSize

        normalTHeight: M.Device.isMobile ? fSize*3 : fSize*2
        wordbarHeight: normalTHeight
        maxTHeight: Math.min(M.Device.isMobile ? fSize*4 : fSize*3, M.Units.dp*200)

        // initial
        Layout.minimumHeight: normalTHeight
        Layout.maximumHeight: splitview.height*2/3

        onWbhChanged:
        {
            if (wbh)
            {
                var m = normalTHeight + wordbarHeight
                if (height < m) Layout.minimumHeight = m
                else
                {
                    Layout.minimumHeight = m
                    height += wordbarHeight
                }
            }
            else
            {
                Layout.minimumHeight = normalTHeight
                height -= wordbarHeight
            }
        }
        
        visible: app.enableTextInput

        Component.onCompleted:
        {
            if (app.enableTextInput)
            mainpage.addBubble(textconsole, "Drag to resize text input", 16)
        }
    }

    QConsole { id: qconsole }
}
