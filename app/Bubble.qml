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
import QtGraphicalEffects 1.0
import Material 0.3

PopupBase
{
    id: bubble
    anchors.fill: parent

    property alias text: tiplabel.text
    property color fgColor: theme.primaryColor

    property Item hostArea

    overlayLayer: "tooltipOverlayLayer"
    globalMouseAreaEnabled: false
    visible: false

    property int boxpad: 16*Units.dp
    property int lw: 4*Units.dp // line width
    property int padding: boxpad + lw

    // shadow
    property int xw: 4*Units.dp
    property int xh: 4*Units.dp

    // to dim
    //backgroundColor: Qt.rgba(0.2, 0.2, 0.2, 0.9)
    //backgroundColor: "transparent"

    property var hotspot: Qt.point(0,0)

    property int x1
    property int x2
    property int y1
    property int y2
    property bool isBelow
        
    function layout()
    {
        x1 = Math.max(content.x - boxpad, 0)
        x2 = x1 + content.width + 2*boxpad

        var x = Math.min(hotspot.x, x1)
        blob.width = Math.max(hotspot.x, x2) - x + lw*2

        y1 = Math.max(content.y - boxpad, 0)
        y2 = y1 + content.height + 2*boxpad

        var y = Math.min(hotspot.y, y1)
        blob.height = Math.max(hotspot.y, y2) - y + lw*2

        //console.log(x1, y1, x2, y2)
        //console.log(x, y, blob.width, blob.height)
        
        x1 = x1 + lw/2 - x
        y1 = y1 + lw/2 - y

        x2 = x2 - lw/2 - x
        y2 = y2 - lw/2 - y

        blob.x = x
        blob.y = y
    }

    onOpened:
    {
        var hpos = hostArea.mapToItem(bubble, 0, 0)

        var hx2 = hpos.x + hostArea.width
        var rx = width - hx2
        
        var cx
        var cy
        if (rx >= hpos.x)
        {
            // put box on right of host
            cx = hx2 + (rx - content.width)/2
            if (cx + content.width + padding > width)
                cx = width - content.width - padding
        }
        else
        {
            // put box left of host
            cx = Math.max((hpos.x - content.width)/2, padding)
        }

        var hy2 = hpos.y + hostArea.height
        var by = height - hy2

        if (by >= hpos.y)
        {
            // put box below host
            cy = hy2 + (by - content.height)/2;
            if (cy + content.height + padding > height)
                cy = height - content.height - padding

            isBelow = true
        }
        else
        {
            // put box above host
            cy = Math.max((hpos.y - content.height)/2, padding)

            isBelow = false
        }
        
        content.x = cx
        content.y = cy

        hotspot = hostArea.mapToItem(bubble, hostArea.width/2, hostArea.height/2)
        
        //console.log("hotspot ", hotspot, hostArea.width, hostArea.height);
        layout()
        
        blob.requestPaint();

        visible = true
    }

    onClosed: visible = false;

    function show()
    {
        timer.stop()
        if(text !== "" && !showing) open()
    }

    Canvas
    {
        id: blob
        z: 1
        antialiasing: true
        
        property real alpha: 1.0
        property string fillColor: "whitesmoke"
        property int radius: Math.min(width, height)/20

        onPaint:
        {
            layout()
            
            var ctx = blob.getContext('2d')

            ctx.save()
            ctx.clearRect(0, 0, blob.width, blob.height)
            
            ctx.globalAlpha = blob.alpha;
            ctx.globalCompositeOperation = "source-over"

            var bw = x2 - x1
            var gap = Math.min(bw/8, 30*Units.dp)

            ctx.beginPath()
            ctx.moveTo(hotspot.x - x, hotspot.y - y)

            if (isBelow)
            {
                ctx.lineTo(x1 + bw/2 + gap, y1)
                ctx.lineTo(x2 - radius, y1)
                ctx.quadraticCurveTo(x2, y1, x2, y1 + radius)
                ctx.lineTo(x2, y2 - radius)
                ctx.quadraticCurveTo(x2, y2, x2 - radius, y2)
                ctx.lineTo(x1 + radius, y2)
                ctx.quadraticCurveTo(x1, y2, x1, y2 - radius)
                ctx.lineTo(x1, y1 + radius)
                ctx.quadraticCurveTo(x1, y1, x1 + radius, y1)
                ctx.lineTo(x1 + bw/2 - gap, y1)
            }
            else
            {
                ctx.lineTo(x1 + bw/2 - gap, y2)                
                ctx.lineTo(x1 + radius, y2)
                ctx.quadraticCurveTo(x1, y2, x1, y2 - radius)
                ctx.lineTo(x1, y1 + radius)
                ctx.quadraticCurveTo(x1, y1, x1 + radius, y1)
                ctx.lineTo(x2 - radius, y1)
                ctx.quadraticCurveTo(x2, y1, x2, y1 + radius)
                ctx.lineTo(x2, y2 - radius)
                ctx.quadraticCurveTo(x2, y2, x2 - radius, y2)
                ctx.lineTo(x1 + bw/2 + gap, y2)
            }
            ctx.closePath()
            
            ctx.lineWidth = lw
            ctx.lineJoin = "round"
            ctx.fillStyle = fillColor
            ctx.strokeStyle = bubble.fgColor
            
            ctx.fill()
            ctx.stroke()
            ctx.restore()
        }

        TapHandler
        {
            onTapped: if (showing) close();
        }
    }

    Item
    {
        id: content
        z: 2
        
        width: tiplabel.paintedWidth + 16 * Units.dp
        height: Math.max(40*Units.dp, tiplabel.paintedHeight + 16*Units.dp)
        
        Text
        {
            id: tiplabel
            anchors.centerIn: parent        
            z: 1
            
            font.pixelSize: 20*Units.dp
            font.family: Theme.fontFamily
            color: theme.accentColor
        }
    }

    DropShadow
    {
        anchors.fill: blob
        verticalOffset: xh
        horizontalOffset: xw
        radius: 6*Units.dp
        samples: 1+radius*2
        color: "#60000000"
        source: blob
        cached: true
    }

    MouseArea
    {
        // absorb all mouse events making the layer modal
        z: 0
        anchors.fill: parent
    }
}
