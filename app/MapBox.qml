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
import QtQuick.Controls 1.4
import Material 0.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import com.voidware.brahman 1.0

Item
{
    id: map

    property int xw: 4*Units.dp
    property int xh: 4*Units.dp
    property real sc: 10*Units.dp
    property color boxBorderColor: "#808080" // works in both light & dark

    property real updownSize: 0.125 // fraction of box size

    width: qmapbox.width*sc + xw
    height: qmapbox.height*sc + xh

    property int gen: qmapbox.generation;

    onScChanged:
    {
        animx.stop();
        animy.stop();
        ensureVisible(qmapbox.currentBox)
    }

    NumberAnimation
    {
        id: animx
        target:mapflick
        property:"contentX"
        duration: 500
        easing.type: Easing.OutQuad
    }

    NumberAnimation
    {
        id: animy
        target:mapflick
        property:"contentY"
        duration: 500
        easing.type: Easing.OutQuad
    }

    function ensureVisible(boxid)
    {
        var i = qmapbox.getBoxIndexForID(boxid);
        if (i >= 0)
        {
            // centre of box
            var x = qmapbox.getBoxX(i)*sc + qmapbox.getBoxWidth(i)*sc/2 - mapflick.width/2
            var y = qmapbox.getBoxY(i)*sc + qmapbox.getBoxHeight(i)*sc/2 - mapflick.height/2

            if (x < 0) x = 0
            else if (x + mapflick.width > map.width) x = map.width - mapflick.width

            if (y < 0) y = 0
            else if (y + mapflick.height > map.height) y = map.height - mapflick.height
            animx.to = x
            animy.to = y
            animx.start();
            animy.start();
        }
    }

    function update()
    {
        // w, h, gap
        qmapbox.performLayout(10, 8, 3)
        ensureVisible(qmapbox.currentBox);
    }

    Component
    {
        id: alink
        Item
        {
            id: link
            property int aheight: map.sc*4/3 // thickness of arrow
            property int amargin: map.sc/2 

            property var from: gen, qmapbox.getLinkFrom(index)
            property var to: gen, qmapbox.getLinkTo(index)
            property bool bi: gen, qmapbox.getLinkBi(index)

            // triggered when map.sc changes
            onAheightChanged: relayout();

            // some link are not visible, but exist to establish connectivity
            visible: qmapbox.getLinkVisible(index)

            Arrow
            {
                id: arrow
                bi: link.bi
                strokeColor: boxBorderColor
            }

            function relayout()
            {
                var dy = (from.y - to.y)*map.sc
                var dx = (to.x - from.x)*map.sc

                var cx = (from.x + to.x)*map.sc/2
                var cy = (from.y + to.y)*map.sc/2

                var s = 0

                if (dy == 0)
                {
                    s = dx;
                    if (dx < 0)
                    {
                        s = -s;
                        arrow.rotate = Math.PI;
                    }
                    if (s > amargin) s-= amargin;
                }
                else if (dx == 0)
                {
                    s = dy;
                    arrow.rotate = Math.PI/2                    

                    if (dy < 0)
                    {
                        s = -s;
                        arrow.rotate = -arrow.rotate;
                    }
                    if (s > amargin) s-= amargin;
                }
                else if (dx > 0)
                {
                    // dy > 0 => top-right
                    // dy < 0 => bottom-right
                    s = Math.sqrt(dx*dx + dy*dy)
                    arrow.rotate = Math.atan(dy/dx)
                }
                else if (dx < 0)
                {
                    // dy > 0 => top-left
                    // dy < 0 => bottom-left
                    s = Math.sqrt(dx*dx + dy*dy)
                    arrow.rotate = Math.PI + Math.atan(dy/dx)
                }

                x = cx - s/2
                y = cy - s/2
                arrow.normw = s
                arrow.normh = aheight                    
            }
        }
    }

    Component
    {
        id: abox
        Item
        {
            x: qmapbox.getBoxX(index)*map.sc
            y: qmapbox.getBoxY(index)*map.sc
            width: qmapbox.getBoxWidth(index)*map.sc
            height: qmapbox.getBoxHeight(index)*map.sc
            
            property int boxItemCount: gen, qmapbox.getBoxItemCount(index)
            property bool boxDark: gen, qmapbox.getBoxDark(index)
        
            property string boxid: qmapbox.getBoxID(index)
            property bool boxActive: qmapbox.currentBox == boxid 
            property color borderColor: boxActive ? Theme.accentColor : map.boxBorderColor
            Rectangle 
            {
                id: box
                anchors.fill: parent
                border.width: Math.max((boxActive ? 0.5 : 0.3)*map.sc, Units.dp)
                border.color: borderColor
                radius: width/10
                color: boxm.pressed ? Theme.accentColor : (boxDark ? QControl.colorContrast(theme.backgroundColor, 0.1) : theme.backgroundColor)
                MouseArea
                {
                    id: boxm
                    anchors.fill: parent
                    onClicked: QControl.evalCommand("_goto " + boxid)
                }
            }

            DropShadow
            {
                anchors.fill: box
                verticalOffset: xh
                horizontalOffset: xw
                radius: 6*Units.dp
                samples: 1+radius*2
                color: "#60000000"
                source: box
                cached: true
                visible: Theme.isDark == false
            }

            Rectangle
            {
                // dot to indicate something of interest
                color: Theme.accentColor
                width: map.sc
                height: map.sc
                radius: width/2
                anchors
                {
                    left: box.left
                    bottom: box.bottom
                    margins: map.sc
                }
                visible: boxItemCount > 0
            }

            onBorderColorChanged:
            {
                up.requestPaint()
                down.requestPaint()
            }

            Canvas
            {
                // up arrow indicator
                id: up

                visible: gen, qmapbox.getBoxIndUp(index)

                property int size: box.height*updownSize
                width: size
                height: size

                anchors
                {
                    right: box.right
                    top: box.top
                    margins: map.sc
                }

                QShape
                {
                    id: triu
                    name: "triup"
                }
                
                onPaint:
                {
                    var ctx = up.getContext('2d');
                    ctx.save()
                    ctx.clearRect(0, 0, up.width, up.height)
                    ctx.globalCompositeOperation = "source-over";
                    ctx.fillStyle = borderColor
                    
                    triu.reset()
                    var tscale = up.size/triu.widthNow()            
                    triu.transform(tscale,0,0,tscale,0,tscale*triu.heightNow())

                    ctx.beginPath()
                    ctx.moveTo(triu.atx(0), triu.aty(0))
                    for (var i = 1; i < triu.size; ++i) ctx.lineTo(triu.atx(i), triu.aty(i));
                    ctx.closePath()
                    ctx.fill()
                    ctx.restore()
                }
            }

            Canvas
            {
                // down arrow indicator
                id: down

                visible: gen, qmapbox.getBoxIndDown(index)

                property int size: box.height*updownSize
                width: size
                height: size

                anchors
                {
                    right: box.right
                    bottom: box.bottom
                    margins: map.sc
                }

                QShape
                {
                    id: trid
                    name: "tridown"
                }
                
                onPaint:
                {
                    var ctx = down.getContext('2d');
                    ctx.save();
                    ctx.clearRect(0, 0, down.width, down.height)
                    ctx.globalCompositeOperation = "source-over";
                    ctx.fillStyle = borderColor

                    trid.reset()
                    var tscale = down.size/trid.widthNow()            
                    trid.transform(tscale,0,0,tscale,0,0)

                    ctx.beginPath()
                    ctx.moveTo(trid.atx(0), trid.aty(0))
                    for (var i = 1; i < trid.size; ++i) ctx.lineTo(trid.atx(i), trid.aty(i));
                    ctx.closePath()
                    ctx.fill()
                    ctx.restore();
                }
            }

                
            Text
            {
                    anchors.fill: parent
                    anchors.margins: Units.dp*8
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    
                    // this ratio controls the text size in relation to the
                    // boxes. Whenever we have a word that's too long to fit
                    // (eg "claustrophobic") we have to make it slightly smaller
                    font.pixelSize: Math.floor(map.sc*13/10)
                    font.family: Theme.fontFamily
                    text: gen, qmapbox.boxName(index)
                    wrapMode: Text.WordWrap
                    color: Theme.textColor
            }
        }
    }

    Repeater
    {
        model: qmapbox.boxCount
        delegate: abox
    }

    Repeater
    {
        model: qmapbox.linkCount
        delegate: alink
    }

}
