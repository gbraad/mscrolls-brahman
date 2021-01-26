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


#ifndef APNGREADER_H
#define APNGREADER_H

#include <stdio.h>
#include <assert.h>
#include <string>
#include <QImage>

#include "png.h"
#include "levelfilter.h"


#if 0
#define DPF(_x)  printf("#######" _x "\n")
#define DPF1(_x, _a)  printf("#######" _x "\n", _a)
#define DPF2(_x, _a, _b)  printf("#######" _x "\n", _a, _b)
#else
#define DPF(_x)
#define DPF1(_x, _a)
#define DPF2(_x, _a, _b)
#endif

#ifndef PNG_APNG_SUPPORTED
#error libpng with APNG patch is required
#endif
#ifndef PNG_READ_APNG_SUPPORTED
#error libpng with APNG patch and APNG read support is required
#endif

struct ApngReader
{
    typedef std::string string;
    
    ApngReader()
    {
        _png = 0;
        _info = 0;
        _infoRead = false;
        _animated = false;
        _skipFirst = false;
        _frameCount = 1;
        _plays = 0;
        _data = 0;
    }


    ~ApngReader()
    {
        DPF("~ApngReader");
    
        if(_png)
            png_destroy_read_struct(&_png, &_info, NULL);
        
        delete[] _frame.rows;
        delete [] _data;
        
        delete _frame.p2;
    }

    bool valid() const
    {
        return _infoRead;
    }

    bool init(const string& filename);
    
    bool readFrame(uint& index);

    bool isAnimated() const { return _animated; }
    QSize size() const { return _imageSize; }
    int frames() const { return _frameCount; }
    int plays() const { return _plays; }

    int currentDelay() const
    {
        double d = (double)_frame.delay_num/_frame.delay_den;
        return qRound(d * 1000);
    }

    bool lastFrame() const
    {
        return _animated && _framesRead >= _frameCount-2;
    }

    QImage      _lastImg;
    QImage      _prevImg;
    string      _filename;
    uchar*      _data;
    uchar*      _dpos;
    size_t      _dataSize = 0;

    uchar* _read(size_t n)
    {
        uchar* p = _dpos;
        _dpos += n;
        if ((uint)(_dpos - _data) > _dataSize) p = 0;  // fail
        return p;
    }

    bool        _autoLevel = false;
    RawPixels   _pix;
    LevelFilter _levels;
    

 private:
    

    png_structp _png;
    png_infop _info;

    //image info
    bool        _infoRead;
    bool        _animated;
    bool        _skipFirst;
    QSize       _imageSize;
    quint32     _frameCount;
    quint32     _plays;

    struct Frame
    {
        quint32 x = 0;
        quint32 y = 0;
        quint32 width = 0;
        quint32 height = 0;
        quint32 channels = 0;

        quint16 delay_num = 0;
        quint16 delay_den = 1;
        quint8 dop = PNG_DISPOSE_OP_NONE;
        quint8 bop = PNG_BLEND_OP_SOURCE;

        quint32 rowbytes = 0;
        unsigned char* p = 0;
        png_bytepp rows = 0;

        // for animation
        unsigned char* p2 = 0;

    } _frame;

    unsigned int _framesRead = 0;

    static void info_fn(png_structp png_ptr, png_infop info_ptr);
    static void row_fn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass);
    static void end_fn(png_structp png_ptr, png_infop info_ptr);

    static void frame_info_fn(png_structp png_ptr, png_uint_32 frame_num);
    static void frame_end_fn(png_structp png_ptr, png_uint_32 frame_num);

    bool readChunk(quint32 len = 0);
    void copyOver();
    void blendOver();

    void calcLevels();
    void applyLevels();
    
};

#endif // APNGREADER_H
