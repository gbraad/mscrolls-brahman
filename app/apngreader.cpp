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


#include "apngreader.h"
#include <QtEndian>

#include "pngpriv.h"

#include "logged.h"
#include "fd.h"


ApngReader* ApngReader::_theReader;


static void user_error_fn(png_structp png_ptr,
                          png_const_charp error_msg)
{
    LOG1("PNG Error: ", error_msg);
}

static void user_warning_fn(png_structp png_ptr,
                            png_const_charp warning_msg)
{
    DPF1("PNG warning: %s", warning_msg);
}

bool ApngReader::init(const string& filename)
{
    DPF("init");

    _filename = filename;

    FD file;
        
    if (!file.open(_filename.c_str()))
    {
        LOG2("ApngReader, cant open file ", _filename);
        return false;
    }

    FD::Pos sz;
    _data = file.readAll(&sz);
    _dataSize = sz;
    _dpos = _data;

    if (png_sig_cmp((png_const_bytep)_data, 0, 8) != 0)
    {
        LOG1("apng, wrong signature in ", _filename);
        return false;
    }

    //init png structs
    _png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                  NULL,
                                  user_error_fn,
                                  user_warning_fn);
    if(!_png)
    {
        LOG3("apng, init failed ", _filename);
        return false;
    }

    _info = png_create_info_struct(_png);
    if(!_info)
    {
        LOG3("apng, init failed ", _filename);
        return false;
    }

    png_set_progressive_read_fn(_png, NULL, &ApngReader::info_fn, &ApngReader::row_fn, &ApngReader::end_fn);

    //set png jump position
    if (setjmp(png_jmpbuf(_png)))
    {
        _infoRead = false;
        return false;
    }

    //read image sig + header
    bool valid = readChunk(8);
    while(valid && !_infoRead) valid = readChunk();
    
    return _infoRead;
}

bool ApngReader::readFrame(uint& index)
{
    DPF2("APNG reader, read frame %d from %d", index, _frameCount);
    
    if (setjmp(png_jmpbuf(_png)))
        return false;

    bool valid = true;
    while(valid && _framesRead <= index)
    {
        valid = readChunk();
    }

    if (valid)
    {
        ++index;
        return true;
    }
    
    DPF("APNG reader, read frame invalid");

    return false;
}

void ApngReader::info_fn(png_structp png_ptr, png_infop info_ptr)
{
    DPF("info_fn");
    
    ApngReader* reader = _theReader;
    Frame &frame = reader->_frame;

    //init png reading
    png_set_expand(png_ptr);
    png_set_strip_16(png_ptr);
    png_set_gray_to_rgb(png_ptr);
    png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    png_set_bgr(png_ptr);
    (void)png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    //init read frame struct
    frame.x = 0;
    frame.y = 0;
    frame.width = png_get_image_width(png_ptr, info_ptr);
    frame.height = png_get_image_height(png_ptr, info_ptr);
    frame.channels = png_get_channels(png_ptr, info_ptr);
    frame.delay_num = 0;
    frame.delay_den = 10;
    frame.dop = PNG_DISPOSE_OP_NONE;
    frame.bop = PNG_BLEND_OP_SOURCE;
        
    if (reader->_lastImg.isNull())
    {
        DPF("allocating image");
        reader->_lastImg = QImage(frame.width, frame.height, QImage::Format_ARGB32);
        // init image
        reader->_lastImg.fill(Qt::black);
    }
    else
    {
        // if image already present, ensure correct size
        assert(reader->_lastImg.width() == frame.width);
        assert(reader->_lastImg.height() == frame.height);
    }

    frame.p = (unsigned char*)reader->_lastImg.bits();
    frame.rowbytes = reader->_lastImg.bytesPerLine();
        
    frame.rows = new png_bytep[frame.height * sizeof(png_bytep)];
    for (quint32 j = 0; j < frame.height; j++)
        frame.rows[j] = frame.p + j * frame.rowbytes;

    //read apng information
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_acTL)) {
        reader->_animated = true;
        png_get_acTL(png_ptr, info_ptr, &reader->_frameCount, &reader->_plays);
        reader->_skipFirst = png_get_first_frame_is_hidden(png_ptr, info_ptr);

        //add extended APNG read functions
        png_set_progressive_frame_fn(png_ptr, &ApngReader::frame_info_fn, &ApngReader::frame_end_fn);
        //read info for first frame (skipped otherwise)
        if(!reader->_skipFirst)
            frame_info_fn(png_ptr, 0);
    } else
        reader->_animated = false;

    reader->_infoRead = true;
}

void ApngReader::row_fn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass)
{
    //if (row_num == 10) DPF1("row_fn %d", row_num);
    Q_UNUSED(pass)
        
	ApngReader* reader = _theReader;
    png_progressive_combine_row(png_ptr, reader->_frame.rows[row_num], new_row);
}

void ApngReader::end_fn(png_structp png_ptr, png_infop info_ptr)
{
    DPF("APNG reader, end_fn");
    ApngReader* reader = _theReader;
    ++reader->_framesRead;

#if 0    
    Q_UNUSED(info_ptr);
    auto reader = _readers[png_ptr];
    Frame &frame = reader->_frame;

    if(!reader->_animated) {
        //reader->copyOver();
        //reader->_allFrames.append(reader->_lastImg);
    }

    //delete[] frame.rows;
    //frame.rows = nullptr;

    //delete[] frame.p;
    //frame.p = nullptr;
#endif    
}

void ApngReader::frame_info_fn(png_structp png_ptr, png_uint_32 frame_num)
{
    //DPF1("frame_info_fn %d", frame_num);
    
    Q_UNUSED(frame_num);
    ApngReader* reader = _theReader;
    auto info_ptr = reader->_info;
    Frame &frame = reader->_frame;

    frame.x = png_get_next_frame_x_offset(png_ptr, info_ptr);
    frame.y = png_get_next_frame_y_offset(png_ptr, info_ptr);
    frame.width = png_get_next_frame_width(png_ptr, info_ptr);
    frame.height = png_get_next_frame_height(png_ptr, info_ptr);
    frame.delay_num = png_get_next_frame_delay_num(png_ptr, info_ptr);
    frame.delay_den = png_get_next_frame_delay_den(png_ptr, info_ptr);
    frame.dop = png_get_next_frame_dispose_op(png_ptr, info_ptr);
    frame.bop = png_get_next_frame_blend_op(png_ptr, info_ptr);

    // update rows to point into part of the background image
    for (quint32 j = 0; j < frame.height; j++)
        frame.rows[j] = frame.p + (j + frame.y)*frame.rowbytes + frame.x*4;

}

void ApngReader::frame_end_fn(png_structp png_ptr, png_uint_32 frame_num)
{
    ApngReader* reader = _theReader;
    ++reader->_framesRead;

#if 0

    Frame &frame = reader->_frame;
    auto &image = reader->_lastImg;

    if(frame_num == 0 && reader->_skipFirst)//TODO not always, only when reading animated
        return;
    if((frame_num == 0 && !reader->_skipFirst) ||
       (frame_num == 1 && reader->_skipFirst)) {
        frame.bop = PNG_BLEND_OP_SOURCE;
        if (frame.dop == PNG_DISPOSE_OP_PREVIOUS)
            frame.dop = PNG_DISPOSE_OP_BACKGROUND;
    }

    QImage temp;

    if(frame.dop == PNG_DISPOSE_OP_PREVIOUS)
        temp = image;

    if (frame.bop == PNG_BLEND_OP_OVER)
        reader->blendOver();
    else
        reader->copyOver();

    reader->_allFrames.append({image, frame.delay_num, frame.delay_den});

    if (frame.dop == PNG_DISPOSE_OP_PREVIOUS)
        image = temp;
    else if (frame.dop == PNG_DISPOSE_OP_BACKGROUND) {
        for(quint32 y = 0; y < frame.height; y++) {
            for(quint32 x = 0; x < frame.width; x++)
                image.setPixelColor(x + frame.x, y + frame.y, Qt::black);
        }
    }
#endif    
}

bool ApngReader::readChunk(quint32 len)
{
    //DPF1("read chunk %d", len);

    uchar* data;
    if(len == 0)
    {
        // read exactly 1 chunk
        //read 4 bytes -> size

        data = _read(4);

        if (data)
        {
            len = *(quint32*)data;
            len = qFromBigEndian(len) + 8;//type (4b) + crc (4b)
            _read(len);
            len += 4; 
        }
    }
    else
    {
        data = _read(len);
    }

    if (!data)
    {
        DPF("read error");
        return false;
    }

    png_process_data(_png, _info, (png_bytep)data, len);

    return true;
}

#if 0
void ApngReader::copyOver()
{
    for(quint32 y = 0; y < _frame.height; y++) {
        for(quint32 x = 0; x < _frame.width; x++) {
            auto px = x*4;

            QColor c;
            c.setBlue(_frame.rows[y][px]);
            c.setGreen(_frame.rows[y][px+1]);
            c.setRed(_frame.rows[y][px+2]);
            c.setAlpha(_frame.rows[y][px+3]);

            _lastImg.setPixelColor(x + _frame.x, y + _frame.y, c);
        }
    }
}

void ApngReader::blendOver()
{
    for(quint32 y = 0; y < _frame.height; y++) {
        for(quint32 x = 0; x < _frame.width; x++) {
            auto px = x*4;

            QColor src;
            src.setBlue(_frame.rows[y][px]);
            src.setGreen(_frame.rows[y][px+1]);
            src.setRed(_frame.rows[y][px+2]);
            src.setAlpha(_frame.rows[y][px+3]);

            if(src.alpha() == 0xFF)
                _lastImg.setPixelColor(x + _frame.x, y + _frame.y, src);
            else if(src.alpha() != 0x00) {
                auto dst = _lastImg.pixelColor(x + _frame.x, y + _frame.y);

                //do porter-duff blending
                if(dst.alpha() != 0x00) {
                    auto u = src.alpha() * 0xFF;
                    auto v = (0xFF - src.alpha()) * dst.alpha();
                    auto al = u + v;
                    src.setBlue((src.blue() * u + dst.blue() * v) / al);
                    src.setGreen((src.green() * u + dst.green() * v) / al);
                    src.setRed((src.red() * u + dst.red() * v) / al);
                    src.setAlpha(al/255);
                }

                _lastImg.setPixelColor(x + _frame.x, y + _frame.y, src);
            }
        }
    }
}
#endif


ApngReader::Frame::Frame() :
    x(0),
    y(0),
    width(0),
    height(0),
    channels(0),
    delay_num(0),
    delay_den(1),
    dop(PNG_DISPOSE_OP_NONE),
    bop(PNG_BLEND_OP_SOURCE),
    rowbytes(0),
    p(nullptr),
    rows(nullptr)
{}
