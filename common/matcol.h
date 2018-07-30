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

#include <string>
#include <math.h>
#include "colour.h"
#include "cutils.h"

struct MatCol
{
    typedef long iterator;
    typedef std::string string;

    static Colour get(const char* name, int shade = 0)
    {
        // zero not found
        unsigned int mc = 0;
        _get(&mc, name, shade);
        return mc;
    }

    static Colour get(const string& name, int shade = 0)
    {
        return get(name.c_str(), shade);
    }

    static Colour get(const Colour& c, int shade)
    {
        string name = _identify(c);
        //printf("identify %s as %s\n", c.toString().c_str(), name.c_str());
        if (!name.empty()) return get(name, shade);
        return _shade(c._argb, shade);
    }

    static string getContrastName(const char* name);

    static string getContrastName(const string& name)
    {
        return getContrastName(name.c_str());
    }

    static iterator begin()
    {
        return (iterator)bump(_getTable(0));
    }

    static iterator next(iterator it)
    {
        if (it)
        {
            MatColRec* mci = (MatColRec*)it;
            it = (iterator)bump(++mci);
        }
        return it;
    }

    static Colour atColour(iterator it, int shade = 0)
    {
        unsigned int mc = 0; // black default

        if (!shade) shade = 500; // default shade

        MatColRec* mci = (MatColRec*)it;
        if (mci)
        {
            bool ok = false;

            if (shade != 500)
            {
                // locate other shade
                ok = _get(&mc, atName(it).c_str(), shade);
            }

            if (!ok) mc = mci->_val | 0xff000000;
        }
        return mc;
    }

    static string atName(iterator it)
    {
        MatColRec* mci = (MatColRec*)it;
        if (mci) return mci->_name;
        return string();
    }

    static int baseColourCount()
    {
        int cc = 0;
        for (iterator it = begin(); it; it = next(it)) ++cc;
        return cc;
    }

private:

    struct MatColRec
    {
        const char*         _name;
        int                 _shade;
        unsigned int        _val;
    };


    static void _shade(int& r, int& g, int& b, double pc)
    { 
        double t = pc < 0 ? 0 : 255;
        double p = pc < 0 ? -pc : pc;
        
        r = floor(((t - r)*p) + r + 0.5);
        g = floor(((t - g)*p) + g + 0.5);
        b = floor(((t - b)*p) + b + 0.5);
    }

    static void _shade(int& r, int& g, int& b, int shade)
    {
        static const float sf[] = 
        {
            0.9f, 0.7f, 0.5f, 1.0f/3, 1.0f/6, 0.0f, -1.0f/8, -1.0f/4, -3.0f/8, -0.5f 
        };

        if (shade > 900) shade = 900;
        if (shade < 0) shade = 0;

        double f = sf[shade/100];
        _shade(r, g, b, f);
    }

    static unsigned int _shade(unsigned int c, int shade)
    {
        int r = (c >> 16) & 0xff;
        int g = (c >> 8) & 0xff;
        int b = c & 0xff;
        _shade(r, g, b, shade);
        
        unsigned int sc = (c & 0xff000000) | (r << 16) | (g << 8) | b;
        //printf("shaded %08X (%d) to %08X\n", c, shade, sc);
        return sc;
    }

    static const MatColRec* _getTable(size_t* size);

    static string _identify(const Colour& c, int* shade = 0)
    {
        // reverse lookup of material colour
        size_t sz;
        const MatColRec* mci = _getTable(&sz);
        while (sz--)
        {
            if (Colour(mci->_val | 0xff000000) == c)
            {
                if (shade) *shade = mci->_shade;
                return mci->_name;
            }
            ++mci;
        }

        // map greys onto black or white
        if (c.isGrey())
        {
            unsigned int v = c.blue();
            return v >= 0x80 ? "White" : "Black";
        }
        
        return string();
    }

    static bool _get(unsigned int* mcol, const char* name, int shade)
    {
        if (!shade) shade = 500; // default shade

        size_t sz;
        const MatColRec* mci = _getTable(&sz);

        unsigned int fallback1 = 0;
        unsigned int fallback2 = 0;

        while (sz--)
        {
            if (!u_stricmp(mci->_name, name))
            {
                // last resort if shade not found
                fallback2 = mci->_val | 0xff000000;
                
                if (shade < 0)
                {
                    int v = -shade;

                    // when matching dark alt shades, they only go to -700
                    // but +ve ones go to 900, we can take a darker version
                    if (v >= 700) v = 900;
                    
                    if (v == mci->_shade) fallback1 = fallback2;
                } 

                if (shade == mci->_shade)
                {
                    // found it!
                    *mcol = fallback2;
                    return true;
                }
            }
            ++mci;
        }

        if (fallback1) fallback2 = fallback1;
        else if (fallback2)
        {
            // only have one shade. 
            if (shade < 0)
            {
                shade = -shade;
                if (shade >= 700) shade = 900;
            }

            // synthetic
            fallback2 = _shade(fallback2, shade);
        }
        
        if (fallback2)
        {
            *mcol = fallback2;
            return true;
        }

        return false;
    }

    static const MatColRec* bump(const MatColRec* mc)
    {
        // move to shade 500
        size_t sz;
        const MatColRec* mci = _getTable(&sz);
        const MatColRec* mce = mci + sz;

        while (mc < mce)
        {
            if (mc->_shade == 500) return mc;
            ++mc;
        }
        return 0; // over
    }

};

inline std::string MatCol::getContrastName(const char* name)
{
    struct Contrast
    {
        const char* _name;
        const char* _cname;
    };

    // color wheel order clockwise
    // red, pink, purple, deep purple, indigo, blue, light blue, cyan, teal, green, light green, lime, yellow, amber, orange, deep orange, brown, blue grey, grey, black, white

    static const Contrast ctab[] =
        {
            { "red",                  "blue" },
            { "pink",                 "light blue" },
            { "purple",               "teal" }, // cyan
            { "deep purple",          "pink" }, // teal
            { "indigo",               "red" }, // "green",
            { "blue",                 "deep orange" }, //"light green",
            { "light blue",           "indigo" }, // "lime",
            { "cyan",                 "pink" }, // "yellow",
            { "teal",                 "orange" }, // "amber",
            { "green",                "deep orange" }, // orange
            { "light green",          "deep orange" },
            { "lime",                 "blue grey" }, // "red",
            { "yellow",               "brown" }, // "pink",
            { "amber",                "purple" },
            { "orange",               "deep purple" },
            { "deep orange",          "indigo" },

            { "brown",                "blue grey" },
            { "blue grey",            "dark blue" }, // brown
            { "dark blue",            "red" },
            { "grey",                 "black" },
            { "black",                "white" },
            { "white",                "black" },

        };

    for (int i = 0; i < ASIZE(ctab); ++i)
    {
        const Contrast* ci = ctab + i;
        if (!u_stricmp(name, ci->_name)) return ci->_cname;
    }

    return string();
}

inline const MatCol::MatColRec* MatCol::_getTable(size_t* size)
{
    static const MatColRec matColTable[] = {
        //<!--reds-->
        { "Red", 50, 0xFFEBEE },
        { "Red", 100, 0xFFCDD2 },
        { "Red", 200, 0xEF9A9A },
        { "Red", 300, 0xE57373 },
        { "Red", 400, 0xEF5350 },
        { "Red", 500, 0xF44336 },
        { "Red", 600, 0xE53935 },
        { "Red", 700, 0xD32F2F },
        { "Red", 800, 0xC62828 },
        { "Red", 900, 0xB71C1C },
        { "Red", -100, 0xFF8A80 },
        { "Red", -200, 0xFF5252 },
        { "Red", -400, 0xFF1744 },
        { "Red", -700, 0xD50000 },

        //<!-- pinks -->
        { "Pink", 50, 0xFCE4EC },
        { "Pink", 100, 0xF8BBD0 },
        { "Pink", 200, 0xF48FB1 },
        { "Pink", 300, 0xF06292 },
        { "Pink", 400, 0xEC407A },
        { "Pink", 500, 0xE91E63 },
        { "Pink", 600, 0xD81B60 },
        { "Pink", 700, 0xC2185B },
        { "Pink", 800, 0xAD1457 },
        { "Pink", 900, 0x880E4F },
        { "Pink", -100, 0xFF80AB },
        { "Pink", -200, 0xFF4081 },
        { "Pink", -400, 0xF50057 },
        { "Pink", -700, 0xC51162 },

        //<!-- purples -->
        { "Purple", 50, 0xF3E5F5 },
        { "Purple", 100, 0xE1BEE7 },
        { "Purple", 200, 0xCE93D8 },
        { "Purple", 300, 0xBA68C8 },
        { "Purple", 400, 0xAB47BC },
        { "Purple", 500, 0x9C27B0 },
        { "Purple", 600, 0x8E24AA },
        { "Purple", 700, 0x7B1FA2 },
        { "Purple", 800, 0x6A1B9A },
        { "Purple", 900, 0x4A148C },
        { "Purple", -100, 0xEA80FC },
        { "Purple", -200, 0xE040FB },
        { "Purple", -400, 0xD500F9 },
        { "Purple", -700, 0xAA00FF },

        //<!-- Deep Purples -->
        { "Deep Purple", 50, 0xEDE7F6 },
        { "Deep Purple", 100, 0xD1C4E9 },
        { "Deep Purple", 200, 0xB39DDB },
        { "Deep Purple", 300, 0x9575CD },
        { "Deep Purple", 400, 0x7E57C2 },
        { "Deep Purple", 500, 0x673AB7 },
        { "Deep Purple", 600, 0x5E35B1 },
        { "Deep Purple", 700, 0x512DA8 },
        { "Deep Purple", 800, 0x4527A0 },
        { "Deep Purple", 900, 0x311B92 },
        { "Deep Purple", -100, 0xB388FF },
        { "Deep Purple", -200, 0x7C4DFF },
        { "Deep Purple", -400, 0x651FFF },
        { "Deep Purple", -700, 0x6200EA },

        //<!-- Indigo", -->
        { "Indigo", 50, 0xE8EAF6 },
        { "Indigo", 100, 0xC5CAE9 },
        { "Indigo", 200, 0x9FA8DA },
        { "Indigo", 300, 0x7986CB },
        { "Indigo", 400, 0x5C6BC0 },
        { "Indigo", 500, 0x3F51B5 },
        { "Indigo", 600, 0x3949AB },
        { "Indigo", 700, 0x303F9F },
        { "Indigo", 800, 0x283593 },
        { "Indigo", 900, 0x1A237E },
        { "Indigo", -100, 0x8C9EFF },
        { "Indigo", -200, 0x536DFE },
        { "Indigo", -400, 0x3D5AFE },
        { "Indigo", -700, 0x304FFE },

        // <!--Blue-->
        { "Blue", 50, 0xE3F2FD },
        { "Blue", 100, 0xBBDEFB },
        { "Blue", 200, 0x90CAF9 },
        { "Blue", 300, 0x64B5F6 },
        { "Blue", 400, 0x42A5F5 },
        { "Blue", 500, 0x2196F3 },
        { "Blue", 600, 0x1E88E5 },
        { "Blue", 700, 0x1976D2 },
        { "Blue", 800, 0x1565C0 },
        { "Blue", 900, 0x0D47A1 },
        { "Blue", -100, 0x82B1FF },
        { "Blue", -200, 0x448AFF },
        { "Blue", -400, 0x2979FF },
        { "Blue", -700, 0x2962FF },

        // invent a new color
        {"Dark Blue", 500, 0x008eea },


        // <!-- light Blue-->
        { "light Blue", 50, 0xE1F5FE },
        { "light Blue", 100, 0xB3E5FC },
        { "Light Blue", 200, 0x81D4fA },
        { "Light Blue", 300, 0x4fC3F7 },
        { "Light Blue", 400, 0x29B6FC },
        { "Light Blue", 500, 0x03A9F4 },
        { "Light Blue", 600, 0x039BE5 },
        { "Light Blue", 700, 0x0288D1 },
        { "Light Blue", 800, 0x0277BD },
        { "Light Blue", 900, 0x01579B },
        { "Light Blue", -100, 0x80D8FF },
        { "Light Blue", -200, 0x40C4FF },
        { "Light Blue", -400, 0x00B0FF },
        { "Light Blue", -700, 0x0091EA },

        // <!-- Cyan", -->
        { "Cyan", 50, 0xE0F7FA },
        { "Cyan", 100, 0xB2EBF2 },
        { "Cyan", 200, 0x80DEEA },
        { "Cyan", 300, 0x4DD0E1 },
        { "Cyan", 400, 0x26C6DA },
        { "Cyan", 500, 0x00BCD4 },
        { "Cyan", 600, 0x00ACC1 },
        { "Cyan", 700, 0x0097A7 },
        { "Cyan", 800, 0x00838F },
        { "Cyan", 900, 0x006064 },
        { "Cyan", -100, 0x84FFFF },
        { "Cyan", -200, 0x18FFFF },
        { "Cyan", -400, 0x00E5FF },
        { "Cyan", -700, 0x00B8D4 },

        // <!-- Teal -->
        { "Teal", 50, 0xE0F2F1 },
        { "Teal", 100, 0xB2DFDB },
        { "Teal", 200, 0x80CBC4 },
        { "Teal", 300, 0x4DB6AC },
        { "Teal", 400, 0x26A69A },
        { "Teal", 500, 0x009688 },
        { "Teal", 600, 0x00897B },
        { "Teal", 700, 0x00796B },
        { "Teal", 800, 0x00695C },
        { "Teal", 900, 0x004D40 },
        { "Teal", -100, 0xA7FFEB },
        { "Teal", -200, 0x64FFDA },
        { "Teal", -400, 0x1DE9B6 },
        { "Teal", -700, 0x00BFA5 },

        // <!-- Green -->
        { "Green", 50, 0xE8F5E9 },
        { "Green", 100, 0xC8E6C9 },
        { "Green", 200, 0xA5D6A7 },
        { "Green", 300, 0x81C784 },
        { "Green", 400, 0x66BB6A },
        { "Green", 500, 0x4CAF50 },
        { "Green", 600, 0x43A047 },
        { "Green", 700, 0x388E3C },
        { "Green", 800, 0x2E7D32 },
        { "Green", 900, 0x1B5E20 },
        { "Green", -100, 0xB9F6CA },
        { "Green", -200, 0x69F0AE },
        { "Green", -400, 0x00E676 },
        { "Green", -700, 0x00C853 },

        // <!--Light Green-->
        { "Light Green", 50, 0xF1F8E9 },
        { "Light Green", 100, 0xDCEDC8 },
        { "Light Green", 200, 0xC5E1A5 },
        { "Light Green", 300, 0xAED581 },
        { "Light Green", 400, 0x9CCC65 },
        { "Light Green", 500, 0x8BC34A },
        { "Light Green", 600, 0x7CB342 },
        { "Light Green", 700, 0x689F38 },
        { "Light Green", 800, 0x558B2F },
        { "Light Green", 900, 0x33691E },
        { "Light Green", -100, 0xCCFF90 },
        { "Light Green", -200, 0xB2FF59 },
        { "Light Green", -400, 0x76FF03 },
        { "Light Green", -700, 0x64DD17 },

        // <!-- Lime-->
        { "Lime", 50, 0xF9FBE7 },
        { "Lime", 100, 0xF0F4C3 },
        { "Lime", 200, 0xE6EE9C },
        { "Lime", 300, 0xDCE775 },
        { "Lime", 400, 0xD4E157 },
        { "Lime", 500, 0xCDDC39 },
        { "Lime", 600, 0xC0CA33 },
        { "Lime", 700, 0xA4B42B },
        { "Lime", 800, 0x9E9D24 },
        { "Lime", 900, 0x827717 },
        { "Lime", -100, 0xF4FF81 },
        { "Lime", -200, 0xEEFF41 },
        { "Lime", -400, 0xC6FF00 },
        { "Lime", -700, 0xAEEA00 },

        // <!--Yellow -->
        { "Yellow",  50, 0xFFFDE7 },
        { "Yellow",  100, 0xFFF9C4 },
        { "Yellow",  200, 0xFFF590 },
        { "Yellow",  300, 0xFFF176 },
        { "Yellow",  400, 0xFFEE58 },
        { "Yellow",  500, 0xFFEB3B },
        { "Yellow",  600, 0xFDD835 },
        { "Yellow",  700, 0xFBC02D },
        { "Yellow",  800, 0xF9A825 },
        { "Yellow",  900, 0xF57F17 },
        { "Yellow",  -100, 0xFFFF82 },
        { "Yellow",  -200, 0xFFFF00 },
        { "Yellow",  -400, 0xFFEA00 },
        { "Yellow",  -700, 0xFFD600 },

        //<!--amber-->
        { "Amber", 50, 0xFFF8E1 },
        { "Amber", 100, 0xFFECB3 },
        { "Amber", 200, 0xFFE082 },
        { "Amber", 300, 0xFFD54F },
        { "Amber", 400, 0xFFCA28 },
        { "Amber", 500, 0xFFC107 },
        { "Amber", 600, 0xFFB300 },
        { "Amber", 700, 0xFFA000 },
        { "Amber", 800, 0xFF8F00 },
        { "Amber", 900, 0xFF6F00 },
        { "Amber", -100, 0xFFE57F },
        { "Amber", -200, 0xFFD740 },
        { "Amber", -400, 0xFFC400 },
        { "Amber", -700, 0xFFAB00 },

        //<!--orange-->
        { "Orange",  50, 0xFFF3E0 },
        { "Orange",  100, 0xFFE0B2 },
        { "Orange",  200, 0xFFCC80 },
        { "Orange",  300, 0xFFB74D },
        { "Orange",  400, 0xFFA726 },
        { "Orange",  500, 0xFF9800 },
        { "Orange",  600, 0xFB8C00 },
        { "Orange",  700, 0xF57C00 },
        { "Orange",  800, 0xEF6C00 },
        { "Orange",  900, 0xE65100 },
        { "Orange",  -100, 0xFFD180 },
        { "Orange",  -200, 0xFFAB40 },
        { "Orange",  -400, 0xFF9100 },
        { "Orange",  -700, 0xFF6D00 },

        // <!--Deep Orange", -->
        { "Deep Orange",  50, 0xFBE9A7 },
        { "Deep Orange",  100, 0xFFCCBC },
        { "Deep Orange",  200, 0xFFAB91 },
        { "Deep Orange",  300, 0xFF8A65 },
        { "Deep Orange",  400, 0xFF7043 },
        { "Deep Orange",  500, 0xFF5722 },
        { "Deep Orange",  600, 0xF4511E },
        { "Deep Orange",  700, 0xE64A19 },
        { "Deep Orange",  800, 0xD84315 },
        { "Deep Orange",  900, 0xBF360C },
        { "Deep Orange",  -100, 0xFF9E80 },
        { "Deep Orange",  -200, 0xFF6E40 },
        { "Deep Orange",  -400, 0xFF3D00 },
        { "Deep Orange",  -700, 0xDD2600 },

        //<!--brown -->
        { "Brown", 50, 0xEFEBE9 },
        { "Brown", 100, 0xD7CCC8 },
        { "Brown", 200, 0xBCAAA4 },
        { "Brown", 300, 0xA1887F },
        { "Brown", 400, 0x8D6E63 },
        { "Brown", 500, 0x795548 },
        { "Brown", 600, 0x6D4C41 },
        { "Brown", 700, 0x5D4037 },
        { "Brown", 800, 0x4E342E },
        { "Brown", 900, 0x3E2723 },

        // lies!
        { "Brown", -100, 0x795548 },
        { "Brown", -200, 0x795548 },
        { "Brown", -400, 0x795548 },
        { "Brown", -700, 0x795548 },

        // <!--Grey-->
        { "Grey", 50, 0xFAFAFA },
        { "Grey", 100, 0xF5F5F5 },
        { "Grey", 200, 0xEEEEEE },
        { "Grey", 300, 0xE0E0E0 },
        { "Grey", 400, 0xBDBDBD },
        { "Grey", 500, 0x9E9E9E },
        { "Grey", 600, 0x757575 },
        { "Grey", 700, 0x616161 },
        { "Grey", 800, 0x424242 },
        { "Grey", 900, 0x212121 },

        //<!--Blue", Grey-->
        { "Blue Grey", 50, 0xECEFF1 },
        { "Blue Grey", 100, 0xCFD8DC },
        { "Blue Grey", 200, 0xB0BBC5 },
        { "Blue Grey", 300, 0x90A4AE },
        { "Blue Grey", 400, 0x78909C },
        { "Blue Grey", 500, 0x607D8B },
        { "Blue Grey", 600, 0x546E7A },
        { "Blue Grey", 700, 0x455A64 },
        { "Blue Grey", 800, 0x37474F },
        { "Blue Grey", 900,  0x263238 },

        { "Black", 500, 0x000000 },
        { "White", 500, 0xffffff },
    };

    if (size) *size = sizeof(matColTable)/sizeof(matColTable[0]);
    return matColTable;
}
