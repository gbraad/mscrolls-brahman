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

Image
{
    property string imageName
    property int count: 0
    property bool paused: false

    // this is meant to cache from image provider, but it doesnt?
    cache: true

    property bool animationEnabled: true

    onWidthChanged: updateImage()
    onHeightChanged: updateImage()

    function updateImage()
    {
        if (suffix(imageName) == ".svg")
        {
            // re-render when app size changes
            sourceSize = QControl.svgRenderSize(width, height)
        }
    }
    
    function suffix(name)
    {
        return name.substring(name.lastIndexOf('.')).toLowerCase();
    }

    function play(name)
    {
        if (imageName != name) count = 0
        imageName = name;
        animt.stop()
        if (imageName.length > 0)
        {
            var s = suffix(name)
            if (s == ".png" || s == ".apng")
            {
                // only PNG animated
                step()
                if (animationEnabled) animt.start()
            }
            else if (s == ".svg")
            {
                sourceSize = QControl.svgRenderSize(width, height)
                source = imageName                
            }
            else
            {
                source = imageName
            }
        }
        else
        {
            source = ""
        }
    }

    function step()
    {
        source = "image://anim/" + count + "/" + imageName;
        ++count;
    }

    function update()
    {
        if (QControl.providerAnimated())
        {
            if (!paused) step()
            animt.start()
        }
    }

    Timer
    {
        id: animt
        interval: 100
        onTriggered: update()
    }
    
}

