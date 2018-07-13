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


#pragma once

#include "ifinfo.h"

struct PictureInfo: public InfoBase {};

#define _PS(_name, _tag) \
void pic ## _name(const char* name) { tagString(_tag, name); } \
void pic ## _name(const string& name) { tagString(_tag, name); }

struct PictureInfoBuilder: public InfoBuilder
{
    PictureInfoBuilder(emitFn* e, void* ctx) : InfoBuilder(e, ctx) {}

    void beginPic() { begin(); }
    void endPic() { end(); }

    _PS(Name, BRA_PIC_NAME)

    void picBrightness(double v) { tagDouble(BRA_PIC_BRIGHTNESS, v); }
    void picContast(double v) { tagDouble(BRA_PIC_CONTRAST, v); }
    void picSaturation(double v) { tagDouble(BRA_PIC_SATURATION, v); }
    void picLightness(double v) { tagDouble(BRA_PIC_LIGHTNESS, v); }
    void picGamma(double v) { tagDouble(BRA_PIC_GAMMA, v); }
    void picInline(int v) { tagBool(BRA_PIC_INLINE, v); }
};

#undef _PS

