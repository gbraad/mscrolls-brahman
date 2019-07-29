/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2017.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 * 
 *  contact@voidware.com
 */

#pragma once

#include <iostream>
#include <string>
#include <math.h>
#include "types.h"
#include "cutils.h"

class Colour
{
public:

    static uint _toARGB(uchar a, uchar r, uchar g, uchar b)
    {
        return ((uint)a << 24) | ((uint)r << 16) | ((uint)g << 8) | b;
    }

    static float clamp(float vmin, float vmax, float v)
    {
        if (v < vmin) v = vmin;
        else if (v > vmax) v = vmax;
        return v;
    }

    static uchar floatToUChar(float n)
    {
        // rounds
        return (uchar)clamp(0.0f, 255.0f, n*255.0f + 0.5f);
    }

    uchar alpha() const { return _argb >> 24; }
    uchar red() const { return (_argb >> 16) & 0xff; }
    uchar green() const { return (_argb >> 8) & 0xff; }
    uchar blue() const { return _argb & 0xff; }

    struct HSB
    {
        float hue, saturation, brightness;

        HSB(Colour col) 
        {
            int r = col.red();
            int g = col.green();
            int b = col.blue();

            int hi = u_max (r, g, b);
            int lo = u_min (r, g, b);

            if (hi != 0)
            {
                saturation = (hi - lo) / (float) hi;

                if (saturation > 0)
                {
                    float invDiff = 1.0f / (hi - lo);
                    float red   = (hi - r) * invDiff;
                    float green = (hi - g) * invDiff;
                    float blue  = (hi - b) * invDiff;

                    if (r == hi)
                        hue = blue - green;
                    else if (g == hi)
                        hue = 2.0f + red - blue;
                    else
                        hue = 4.0f + green - red;

                    hue *= 1.0f / 6.0f;

                    if (hue < 0)
                        ++hue;
                }
                else hue = 0;
            }
            else saturation = hue = 0;

            brightness = hi / 255.0f;
        }

        Colour toColour() const 
        {
            return Colour(toRGB(hue, saturation, brightness, 0xff));
        }

        static uint toRGB (float h, float s, float v, uchar alpha)
        {
            uchar intV = floatToUChar(v);
            
            if (s <= 0)
                return _toARGB(alpha, intV, intV, intV);
            
            s = u_min (1.0f, s);
            h = (h - floor (h)) * 6.0f + 0.00001f;
            float f = h - floor (h);
            uchar x = floatToUChar(v * (1.0f - s));

            if (h < 1.0f)   return _toARGB(alpha, intV, floatToUChar (v * (1.0f - (s * (1.0f - f)))), x);
            if (h < 2.0f)   return _toARGB (alpha, floatToUChar (v * (1.0f - s * f)), intV, x);
            if (h < 3.0f)   return _toARGB (alpha, x, intV, floatToUChar (v * (1.0f - (s * (1.0f - f)))));
            if (h < 4.0f)   return _toARGB (alpha, x, floatToUChar (v * (1.0f - s * f)), intV);
            if (h < 5.0f)   return _toARGB (alpha, floatToUChar (v * (1.0f - (s * (1.0f - f)))), x, intV);
            return _toARGB (alpha, intV, x, floatToUChar (v * (1.0f - s * f)));
        }
    };
    
    Colour() { _argb = 0; }
    Colour (uint argb) : _argb(argb) {}
    Colour (uint r, uint g, uint b) { _argb = _toARGB(0xff, r, g, b); }
    Colour (uint r, uint g, uint b, uint a) { _argb = _toARGB(a, r, g, b); }
    Colour (uint r, uint g, uint b, float a)
    { _argb = _toARGB(floatToUChar(a), r, g, b); }

    Colour withAlpha (uint newAlpha) const 
    {
        return Colour((newAlpha << 24) | (_argb & 0xffffff));
    }

    Colour withAlpha (float newAlpha) const
    {
        return withAlpha((uint)floatToUChar(newAlpha));
    }

    Colour overlaidWith (Colour src) const
    {
        uint destAlpha = alpha();

        if (destAlpha == 0) return src;

        int invA = 0xff - src.alpha();
        int resA = 0xff - (((0xff - destAlpha) * invA) >> 8);

        if (resA <= 0) return *this;

        int da = (invA * destAlpha) / resA;

        return Colour ((src.red() + ((((int) red() - src.red()) * da) >> 8)),
                       (src.green() + ((((int) green() - src.green()) * da) >> 8)),
                       (src.blue()  + ((((int) blue()  - src.blue())  * da) >> 8)), (uint)resA);
    }

    bool operator==(const Colour& c) const { return _argb == c._argb; }
    bool operator!=(const Colour& c) const { return _argb != c._argb; }

    explicit operator bool() const { return _argb != 0; }

    enum Colours
    {
        black = 0xff000000,
        white = 0xffffffff,
    };

    static float getPerceivedBrightness(Colour c)
    {
        float r = c.red()/255.0f;
        float g = c.green()/255.0f;
        float b = c.blue()/255.0f;
        return sqrt (r * r * 0.241f + g * g * 0.691f + b * b * 0.068f);
    }

    bool isGrey() const
    {
        return red() == green() && green() == blue();
    }
    
    Colour contrasting(float amount) const
    {
        return overlaidWith ((getPerceivedBrightness(*this) >= 0.5f
                              ? Colour(black)
                              : Colour(white)).withAlpha(amount));
    }

    std::string toString() const
    {
        char buf[16];
        sprintf(buf, "#%02X%02X%02X%02X", alpha(), red(), green(), blue());
        return buf;
    }

    friend std::ostream& operator<<(std::ostream& os, const Colour& c)
    {
        os << c.toString();
        return os;
    }

    uint _argb;
};
