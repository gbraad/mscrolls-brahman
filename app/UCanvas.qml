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
 *  Copyright (c) Strand Games 2020.
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
import QtGraphicalEffects 1.12
import QtQuick.Controls 1.4 as C1
import QtQuick.Layouts 1.12 as C1
import Material 0.3 as M

import com.voidware.brahman 1.0
import Spine 1.0

Item
{
    id: uc

    //property string currentImage
    property font gameFont: QControl.prefs.gameFont
    //property alias choiceActive: choicebox.active
    property bool choiceActive: true
    property bool showText: false
    property bool useAnim: false
    property bool picValid: false

    QConsole { id: qconsole }

    function linkClicked(link)
    {
        if (link.startsWith("http://") || link.startsWith("https://")) Qt.openUrlExternally(link);
        else QControl.evalClickCommand(link)
        //textconsole.forceActiveFocus();
    }

    function choiceAccepted(cmd)
    {
        qtranscript.clear();
        if (picValid) uc.state = "";  // dismiss text after a choice
        QControl.evalJSON(cmd)
    }

    function suffix(name)
    {
        return name.substring(name.lastIndexOf('.')).toLowerCase();
    }

    function prefix(name)
    {
        return name.substring(0, name.lastIndexOf('.'));
    }

    states: 
    [
       State {
           name: "text" 
           PropertyChanges
           {
               target: uc;
               showText: true 
           }

           PropertyChanges
           {
               target: textarea
               visible: true
               opacity: 1.0
           }

           PropertyChanges
           {
               target: picarea
               opacity: 0.25
           }
       }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; duration: 250 }
    }

    C1.SplitView
    {
        id: splitview
        orientation: Qt.Vertical
        anchors.fill: parent

        Item
        { 
            width: parent.width
            C1.Layout.fillHeight: true
        
            Rectangle
            {
                id: picarea
                width: parent.width
                height: parent.height - choicebox.height
                color: QControl.colorContrast(theme.backgroundShade, 0.05)

                property string currentImageJSON: QControl.currentImageJSON
                property real imgBrightness: 0.0
                property real imgContrast: 0.0
                property real imgSaturation: 0.0
                property real imgLightness: 0.0
                property real imgGamma: 1.0

                onCurrentImageJSONChanged:
                {
                    console.log("UCanvas: current image JSON ", currentImageJSON);
                    var js = JSON.parse(currentImageJSON);
                    var rname
                    var iname = ""
                    var atlas
                    var jname = js["name"]
                    
                    if (jname != "last.json")
                    {
                        rname = QControl.resolveAsset(jname)
                        picValid = rname.length > 0
                        useAnim = suffix(rname) == ".json"
                        if (useAnim) atlas = QControl.resolveAsset(js["atlas"])
                    }
                    
                    if (picValid)
                    {
                        var v;
                        v = js["brightness"]
                        imgBrightness = v ? v : 0.0
                        v = js["contrast"]
                        imgContrast = v ? v : 0.0
                        v = js["saturation"]
                        imgSaturation = v ? v : 0.0
                        v = js["lightness"];
                        imgLightness = v ? v : 0.0
                        v = js["gamma"]
                        imgGamma = v ? v : 1.0
                        
                        if (useAnim)
                        {
                            // animation
                            if (rname) skel.skeletonDataFile = rname
                            if (atlas) skel.atlasFile = atlas

                            var anim = js["play"]
                            if (anim)
                            {
                                v = js["loop"]
                                var loop = v ? v != "false" : false
                                v = js["track"]
                                var tr = v ? v : 0
                                v = js["delay"]
                                if (v != undefined) // can be zero
                                {
                                    //console.log("append",anim,"track",tr,"loop",loop);
                                    skel.addAnimation(tr, anim, loop, v)
                                }
                                else
                                {
                                    //console.log("animate",anim,"track",tr,"loop",loop);
                                    skel.setAnimation(tr, anim, loop)
                                }
                                v = js["run"]
                                if (v) skel.setTrackDataTerm(tr, v)
                            }
                            v = js["show"]
                            if (v) skel.setAttachment(v, v);
                            v = js["hide"]
                            if (v) skel.setAttachment(v, ""); // clear
                            v  = js["put"]
                            if (v != undefined)
                            {
                                var at = js["at"]
                                if (at) skel.setAttachment(at, v ? v : "")
                            }
                        }
                        else iname = rname;
                    }
                    pic.source = iname
                }

                Image
                {
                    id: pic
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    verticalAlignment: Image.AlignTop
                    visible: false
                    mipmap: true
                }

                SkeletonAnimation
                {
                    id: skel
                    anchors.fill: parent
                    visible: false

                    //onStart: console.log("skeleton onStart trackIndex: "+ trackIndex);
                    //onEnd: console.log("skeleton onEnd", trackIndex, "term", termname);
                    onComplete: 
                    {
                        //console.log("skeleton onComplete", trackIndex, "loopCount", loopCount, "term", termname);
                        if (termname)
                        {
                            QControl.evalSubcommand(termname);
                        }
                    }
                    
                    //onEvent: console.log("skeleton onEvent trackIndex: "+ trackIndex + ", name:" +event.data.name + ", stringValue:" +event.stringValue);
                }
                
                BrightnessContrast
                {
                    id: effect1
                    anchors.fill: pic 
                    source: useAnim ? skel : pic
                    brightness: picarea.imgBrightness
                    contrast: picarea.imgContrast
                    visible: false
                }

                HueSaturation {
                    id: effect2
                    anchors.fill: effect1
                    source: effect1
                    saturation: picarea.imgSaturation
                    lightness: picarea.imgLightness
                    visible: false
                }

                GammaAdjust {
                    id: effect3
                    anchors.fill: effect2
                    source: effect2
                    gamma: picarea.imgGamma
                    visible: !showText
                }

                FastBlur 
                {
                    anchors.fill: effect3
                    source: effect3
                    radius: 32*M.Units.dp
                    transparentBorder: true
                    visible: showText
                }
            }

            Text
            {
                id: textarea
                z:1
                opacity: 0
                clip: true

                width: parent.width
                height: parent.height - choicebox.height
                
                onWidthChanged: qtranscript.setWidth(width);

                verticalAlignment: TextEdit.AlignVCenter

                wrapMode: TextArea.Wrap
                padding: 64*M.Units.dp
                color: M.Theme.textColor

                font: gameFont
                lineHeight: 1.1
                
                text: qtranscript.textHTML
                textFormat: TextEdit.RichText

                onTextChanged: uc.state = "text"

                MouseArea
                {
                    anchors.fill: parent
                    onClicked: 
                    {
                        var lk = textarea.linkAt(mouse.x, mouse.y);
                        qtranscript.clear();
                        if (lk !== "") linkClicked(lk);
                        else
                        {
                            var v = choicebox.singletonChoice();
                            if (picValid) uc.state = "";  // dismiss text
                            if (v) choiceAccepted(v)

                            // pass on dismiss event as empty string
                            //QControl.evalClickCommand(lk) 
                        }
                    }
                }
            }

            ChoiceBox 
            {
                id: choicebox
                width: parent.width
                height: Math.min(idealHeight, uc.height/3)
                z: 2
                
                anchors.bottom: parent.bottom
                onAccepted: choiceAccepted(cmd);
            }
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
            C1.Layout.minimumHeight: normalTHeight
            C1.Layout.maximumHeight: splitview.height*2/3

            onVisibleChanged: if (visible && !M.Device.isIOS()) forceActiveFocus();
            onWbhChanged:
            {
                if (wbh)
                {
                    var m = normalTHeight + wordbarHeight
                    if (height < m) C1.Layout.minimumHeight = m
                    else
                    {
                        C1.Layout.minimumHeight = m
                        height += wordbarHeight
                    }
                }
                else
                {
                    C1.Layout.minimumHeight = normalTHeight
                    height -= wordbarHeight
                }
            }
            
            visible: app.enableTextInput && choicebox.uiTextAccepted
            
            Component.onCompleted:
            {
                if (app.enableTextInput)
                mainpage.addBubble(textconsole, "Drag to resize text input", 16)
            }
        }
    }
}


