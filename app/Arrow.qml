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
import Material 0.3

import com.voidware.brahman 1.0

Item 
{
    id: arrow

    property string strokeColor: "black"
    property string fillColor: Theme.backgroundColor
    property real rotate: 0.0

    property int normw: 0
    property int normh: 0
    property bool bi: false

    onRotateChanged: canvas.requestPaint()
    onNormwChanged: canvas.requestPaint()
    onNormhChanged: canvas.requestPaint()
    onBiChanged: canvas.requestPaint()
    onFillColorChanged: canvas.requestPaint()

    QShape
    {
        id: ashape
        name: bi ? "arrowbi" : "arrow"
    }
    
    Canvas
    {
        id: canvas

        width:  Math.max(normw, normh)
        height: Math.max(normw, normh)
        
        antialiasing: true
        property real alpha: 1.0

        onAlphaChanged: requestPaint();

        onPaint:
        {
            ashape.reset()
            var ctx = canvas.getContext('2d')

            ctx.save()
            ctx.clearRect(0, 0, canvas.width, canvas.height)

            ctx.globalAlpha = canvas.alpha;
            ctx.globalCompositeOperation = "source-over"

            // and scale
            var sx = normw/ashape.widthNow()
            var sy = normh/ashape.heightNow()

            var c = Math.cos(rotate)
            var s = -Math.sin(rotate)

            ashape.transform(c*sx,s*sx,-s*sy,c*sy,(s*normh-c*normw+canvas.width)/2, (-c*normh-s*normw+canvas.height)/2)
            
            ctx.beginPath()
            ctx.moveTo(ashape.atx(0), ashape.aty(0))
            for (var i = 1; i < ashape.size; ++i) ctx.lineTo(ashape.atx(i), ashape.aty(i));
            ctx.closePath()

            ctx.lineWidth = Units.dp*2
            ctx.lineJoin = "round"
            ctx.fillStyle = fillColor
            ctx.strokeStyle = strokeColor

            ctx.fill();
            ctx.stroke();
            
            ctx.restore();
        }
    }
}
