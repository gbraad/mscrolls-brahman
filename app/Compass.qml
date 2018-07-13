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
import QtQml 2.2

import Material 0.3
import com.voidware.brahman 1.0

Item 
{
    id: compass

    property string fgColor: "black"
    property string bgColor: "white"
    property string ringColor: Qt.tint(fgColor, "#A0FFFFFF")
    MouseArea 
    {
        anchors.fill: parent
        onClicked:
        {
            var q
            if (canvas.up && triu.isInside(mouse.x, mouse.y)) q = 8
            else if (canvas.down && trid.isInside(mouse.x, mouse.y)) q = 9
            else
            {
                var s = Math.min(width, height)
                var a = Math.atan2(s/2 - mouse.y, mouse.x - s/2)*200/Math.PI + 200
                q = 7 - ((Math.floor(a/50 + 0.5) + 1) & 7)
            }
            if (qmapbox.cango(q)) QControl.compassDirection(q);
        }
    }

    onFgColorChanged: canvas.requestPaint()

    QShape
    {
        id: triu
        name: "triup"
    }

    QShape
    {
        id: trid
        name: "tridown"
    }
    
    Canvas
    {
        id: canvas
        anchors.fill: parent
        anchors.margins: 8*Units.dp

        antialiasing: true
        property real alpha: 1.0

        property bool ne: qmapbox.gone
        property bool se: qmapbox.gose
        property bool sw: qmapbox.gosw
        property bool nw: qmapbox.gonw
        property bool n:  qmapbox.gon
        property bool e:  qmapbox.goe
        property bool s:  qmapbox.gos
        property bool w:  qmapbox.gow
        property bool up: qmapbox.goup
        property bool down: qmapbox.godown

        onAlphaChanged: requestPaint();
        onNwChanged: requestPaint();
        onNeChanged: requestPaint();
        onSeChanged: requestPaint();
        onSwChanged: requestPaint();
        onNChanged: requestPaint();
        onEChanged: requestPaint();
        onSChanged: requestPaint();
        onWChanged: requestPaint();
        onUpChanged: requestPaint();
        onDownChanged: requestPaint();

        onPaint:
        {
            var ctx = canvas.getContext('2d');

            ctx.save();
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            ctx.globalAlpha = canvas.alpha;
            ctx.globalCompositeOperation = "source-over";

            var scale = Math.min(canvas.width/312, canvas.height/312)

            ctx.strokeStyle = fgColor
            ctx.fillStyle = ringColor
            ctx.lineWidth = Units.dp 
            ctx.lineJoin = "bevel"

            var ts = canvas.width/6
            
            if (up)
            {
                triu.reset()
                var tscale = ts/triu.widthNow()            
                triu.transform(tscale,0,0,tscale,canvas.width - ts, tscale*triu.heightNow())

                ctx.beginPath()
                ctx.moveTo(triu.atx(0), triu.aty(0))
                for (var i = 1; i < triu.size; ++i) ctx.lineTo(triu.atx(i), triu.aty(i));
                ctx.closePath()
                ctx.fill()
                ctx.stroke()
            }

            if (down)
            {
                trid.reset()
                var tscale = ts/trid.widthNow()            
                trid.transform(tscale,0,0,tscale,canvas.width - ts, canvas.height - tscale*trid.heightNow())

                ctx.beginPath()
                ctx.moveTo(trid.atx(0), trid.aty(0))
                for (var i = 1; i < trid.size; ++i) ctx.lineTo(trid.atx(i), trid.aty(i));
                ctx.closePath()
                ctx.fill()
                ctx.stroke()
            }

            ctx.scale(scale, scale)
            ctx.lineWidth = 2*Units.dp // for rings

            // draw ring edges
            ctx.path = "m228 156a72 72 0 1 1 -144 0 72 72 0 1 1 144 0z"
            ctx.stroke()
            ctx.path = "m252 156a96 96 0 1 1 -192 0 96 96 0 1 1 192 0z"
            ctx.stroke()

            // draw ring fill
            ctx.fillStyle = ringColor
            ctx.path = "m156 60c-53.019 0-96 42.981-96 96s42.981 96 96 96 96-42.981 96-96-42.981-96-96-96zm0 24c39.764 0 72 32.236 72 72s-32.236 72-72 72-72-32.236-72-72 32.236-72 72-72z"
            ctx.fill()

            ctx.lineWidth = Units.dp // for the pointers

            if (ne)
            {
                ctx.fillStyle = bgColor
                ctx.path ="m156 156 68-68-68 32z"
                ctx.stroke()
                ctx.fill()
                ctx.path = "m156 156 68-68-32 68z"
                ctx.fillStyle = fgColor
                ctx.fill()
            }

            if (se)
            {
                ctx.fillStyle = bgColor
                ctx.path = "m156 156 68 68-32-68z"
                ctx.stroke()
                ctx.fill()
                ctx.path = "m156 156 68 68-68-32z"
                ctx.fillStyle = fgColor
                ctx.fill()
            }

            if (sw)
            {
                ctx.fillStyle = bgColor
                ctx.path = "m156 156-68 68 68-32z"
                ctx.stroke()
                ctx.fill()
                ctx.path = "m156 156-68 68 32-68z"
                ctx.fillStyle = fgColor
                ctx.fill()
            }

            if (nw)
            {
                ctx.fillStyle = bgColor
                ctx.path = "m156 156-68-68 32 68z"
                ctx.stroke()
                ctx.fill()
                ctx.path = "m156 156-68-68 68 32z"
                ctx.fillStyle = fgColor
                ctx.fill()
            }

            if (n)
            {
                ctx.fillStyle = bgColor
                ctx.path = "m156 156-24-24 24-132z"
                ctx.stroke()
                ctx.fill()
                ctx.path = "m156 156 24-24-24-132z"
                ctx.fillStyle = fgColor
                ctx.fill()
            }

            if (e)
            {
                ctx.fillStyle = bgColor
                ctx.path = "m156 156 24-24 132 24z"
                ctx.stroke()
                ctx.fill()
                ctx.path = "m156 156 24 24 132-24z"
                ctx.fillStyle = fgColor
                ctx.fill()
            }

            if (s)
            {
                ctx.fillStyle = bgColor
                ctx.path = "m156 156 24 24-24 132z"
                ctx.stroke()
                ctx.fill()
                ctx.path = "m156 156-24 24 24 132z"
                ctx.fillStyle = fgColor
                ctx.fill()
            }

            if (w)
            {
                ctx.fillStyle = bgColor
                ctx.path = "m156 156-24 24-132-24z"
                ctx.stroke()
                ctx.fill()
                ctx.path = "m156 156-24-24-132 24z"
                ctx.fillStyle = fgColor
                ctx.fill()
            }

            ctx.restore();
        }
    }
}
