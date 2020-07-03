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
import Material 0.3 as M

import com.voidware.brahman 1.0
import Spine 1.0

Item
{
    id: uc

    //property string currentImage
    property font gameFont: QControl.prefs.gameFont
    property alias choiceActive: choicebox.active
    property bool showText: false
    property bool useAnim: false

    function linkClicked(link)
    {
        if (link.startsWith("http://") || link.startsWith("https://")) Qt.openUrlExternally(link);
        else if (!choiceActive) QControl.evalClickCommand(link)
        //textconsole.forceActiveFocus();
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
            var pname = js["name"]
            var iname = ""
            if (pname)
            {
                var rname = QControl.resolveAsset(pname)
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
                
                useAnim = suffix(pname) == ".json"

                if (useAnim)
                {
                    // animation
                    var atlas = js["atlas"]
                    if (atlas)
                    {
                        skel.skeletonDataFile = rname
                        skel.atlasFile = QControl.resolveAsset(atlas);

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
            visible: false
        }

        SkeletonAnimation
        {
            id: skel
            anchors.fill: parent
            visible: false

            //onStart: console.log("skeleton onStart trackIndex: "+ trackIndex);
            //onEnd: console.log("skeleton onEnd trackIndex: "+ trackIndex);
            //onComplete: console.log("skeleton onComplete trackIndex: "+ trackIndex + ", loopCount:" +loopCount);
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

        width: parent.width
        height: parent.height - choicebox.height
        
        onWidthChanged: qtranscript.setWidth(width);

        verticalAlignment: TextEdit.AlignVCenter

        wrapMode: TextArea.Wrap
        padding: 64*M.Units.dp

        color: Theme.textColor

        font: gameFont
        //fontSizeMode : Text.Fit 
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
                //console.log("############ text click", lk);
                qtranscript.clear();
                if (lk !== "") linkClicked(lk);
                else uc.state = "";  // dismiss text
            }
        }
    }

    ChoiceBox 
    {
        id: choicebox
        width: parent.width
        height: Math.min(idealHeight, uc.height/3)
        z: 2
        anchors
        {
            bottom: parent.bottom
        }
    }
    
}


