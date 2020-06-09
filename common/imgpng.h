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

#include <stdio.h>
#include <assert.h>
#include <vector>
#include "types.h"
#include "png.h"
#include "pngpriv.h"
#include "strutils.h"
#include "utils.h"
#include "logged.h"

struct ImgData
{
    int                 _width;
    int                 _height;
    uint                _bpp;
    uint                _stride;

    uchar*              _data;
    bool                _ownsData;

    ImgData() { _init(); }

    explicit ImgData(const ImgData& s)
    {
        // copy images uses DONATE semantics
        _init();
        *this = s;
    }

    ImgData& operator=(const ImgData& s)
    {
        // copy images uses DONATE semantics
        _consume(s);
        return *this;
    }
    
    ~ImgData() { purge(); }

    size_t              size() const { return _height*_stride; }
    bool                valid() const { return _data != 0; }

    void format(int w, int h, int bpp)
    {
        purge();
        
        _width = w;
        _height = h;
        _bpp = bpp;
        _calcStride();
    }

    void existingData(const uchar* raw)
    {
        // reference existing pixel data without copying
        assert(raw);
        assert(!_data);

        _ownsData = true;
        _data = (uchar*)raw; 
    }

    void create()
    {
        // will create data for given format
        assert(!_data);

        size_t sz = size();
        if (sz)
        {
            _data = new uchar[sz];
            _ownsData = true;
        }
    }

    void purge()
    {
        if (_ownsData) delete [] _data;
        _init();
    }

    void copy(ImgData& src)
    {
        // make a deep copy
        purge();

        // copy over internals
        _width = src._width;
        _height = src._height;
        _bpp = src._bpp;
        _stride = src._stride;

        size_t sz = _stride * _height;
        _data = new uchar[sz];
        memcpy(_data, src._data, sz);
    
        _ownsData = true;

    }

#if 0
    // currently not used
    void copyScaled(const ImgData& src, int sx, int sy)
    {
        assert(sx >= 1);
        assert(sy >= 1);
        assert(src.valid());

        purge();

        _width = src._width*sx;
        _height = src._height*sy;
        _bpp = src._bpp;
        _calcStride();

        size_t sz = _stride * _height;
        _data = new uchar[sz];
        _ownsData = true;

        int bs = _bpp/8;

        for (int i = 0; i < src._height; ++i)
        {
            uchar* dp0 = _data + sy*i*_stride;
            for (int iy = 0; iy < sy; ++iy)
            {
                const uchar* sp = src._data + i*src._stride;
                uchar* dp = dp0 + iy*_stride;
                for (int j = 0; j < src._width; ++j)
                {
                    for (int ix = 0; ix < sx; ++ix)
                        for (int k = 0; k < bs; ++k) *dp++ = sp[k];
                    
                    sp += bs;
                }
            }
        }
    }
#endif    

    void clip(int x, int y, int bw, int bh)
    {
        // we are to be positioned on a background size (bw, bh) 
        // at background PPS (x,y).

        int w = _width;
        int h = _height;

        // reduce our size to not overlap base
        if (x + w > bw) w = bw - x;
        if (y + h > bh) h = bh - y;

        if (x < 0)
        {
            x = -x;
            w -= x;
        }
        else x = 0;

        if (y < 0)
        {
            y = -y;
            h -= y;
        }
        else y = 0;

        if (w <= 0 || h <= 0)
        {
            purge();
        }
        else /*if (w != _width || h != _height)*/
        {
            //LOG3("image clipped x:", x << " y:" << y << " w:" << w << " h:" << h << " against bw:" << bw << " bh:" << bh);
            ImgData dst;
            dst.format(w, h, _bpp);
            dst.create();
            dst.blit(*this, x, y, 0, 0, w, h);
            _consume(dst);
        }
    }

    void crop(int top, int right, int bottom, int left)
    {
         LOG3("image cropped top:", top << " right:" << right << " bottom:" << bottom << " left:" << left);
         ImgData dst;
         dst.format(_width-left-right, _height-top-bottom, _bpp);
         dst.create();
         dst.blit(*this, left, top, 0, 0, _width-right, _height-bottom);
         _consume(dst);
    }


    void blit(const ImgData& src, int sx, int sy, int dx, int dy, int w, int h)
    {
        assert(src._bpp == _bpp); // only same depth for now...

        // clip extents
        if (sx + w > src._width) w = src._width - sx;
        if (sy + h > src._height) h = src._height - sy;

        if (dx + w > _width) w = _width - dx;
        if (dy + h > _height) h = _height - dy;
        
        const uchar* sp = src._data + sy*src._stride + sx*(_bpp/8);
        uchar* dp = _data + dy*_stride + dx*(_bpp/8);
        int spanx = w*(_bpp/8);
        
        while (h--)
        {
            memcpy(dp, sp, spanx);
            dp += _stride;
            sp += src._stride;
        }
    }


    void blitAlpha(const ImgData& src,
                   int sx, int sy, int dx, int dy, int w, int h)
    {
        assert(src._bpp == _bpp); // only same depth for now...

        // clip extents
        if (sx + w > src._width) w = src._width - sx;
        if (sy + h > src._height) h = src._height - sy;

        if (dx + w > _width) w = _width - dx;
        if (dy + h > _height) h = _height - dy;

        int bs = _bpp/8;

        // only work for RGBA
        assert(bs == 4);
        
        const uchar* sp0 = src._data + sy*src._stride + sx*bs;
        uchar* dp0 = _data + dy*_stride + dx*bs;

        while (h--)
        {
            uchar* dp = dp0;
            const uchar* sp = sp0;

            for (int i = 0; i < w; ++i)
            {
                int u = sp[3] * 0xff;
                int v = (0xff - sp[3]) * (int)dp[3];
                int al = u + v;

                *dp = ((*sp++)*u + (*dp)*v)/al; ++dp;
                *dp = ((*sp++)*u + (*dp)*v)/al; ++dp;
                *dp = ((*sp++)*u + (*dp)*v)/al; ++dp;

                ++sp;
                ++dp; // leave original alpha

                /*
                  // hack to indicate transparency
                if (u != 65025)
                {
                    dp[-4] = 0xff;
                    dp[-3] = 0;
                    dp[-2] = 0;
                }
                */

            }

            dp0 += _stride;
            sp0 += src._stride;
        }
    }


#if 0
    // currently not used
    void blitMasked(const ImgData& src, const ImgData& mask, int dx, int dy)
    {
        // copy all the src img with the given mask to this at (dx, dy)
        
        assert(src._bpp == _bpp); // only same depth for now...

        assert(mask.valid());
        assert(src._width == mask._width);
        assert(src._height == mask._height);

        int w = src._width;
        if (dx + w > _width) w = _width - dx;

        int h = src._height;
        if (dy + h > _height) h = _height - dy;

        int bs = _bpp/8;
        uchar* dp0 = _data + dy*_stride + dx*bs;
        
        for (int i = 0; i < h; ++i)
        {
            const uchar* sp = src._data + i*src._stride;
            const uchar* mp = mask._data + i*mask._stride;
            uchar* dp = dp0 + i*_stride;

            for (int j = 0; j < w; ++j)
            {
                if (*mp++)
                {
                    for (int k = 0; k < bs; ++k) *dp++ = *sp++;
                }
                else
                {
                    sp += bs;
                    dp += bs;
                }
            }
        }
    }
#endif    


private:

    void _consume(const ImgData& src)
    {
        // consume the internal data of src leaving src empty
        // do not copy the image data

        purge();

        // copy over internals
        _width = src._width;
        _height = src._height;
        _bpp = src._bpp;
        _stride = src._stride;

        _data = src._data;
        _ownsData = src._ownsData;
        
        const_cast<ImgData&>(src)._init();
    }


    void _init()
    {
        _width = 0;
        _height = 0;
        _bpp = 0;
        _stride = 0;
        _data = 0;
        _ownsData = false;
    }
    
    void _calcStride()
    {
        // pixels are 1, 2 or 4 bytes
        int a = _bpp/8;
        _stride = a*_width;
    }
};

struct ImgPng
{
    // libpng helper

    typedef std::string string;

    enum CompressLevel
    {
        level_default,
        level_best,
        level_fast,
        level_none
    };

    ImgData*      _img;
    CompressLevel _compressLevel = level_default;
    string        _filename;    // full path

    ImgPng(ImgData* img) : _img(img) {}

    static void user_error_fn(png_structp png_ptr,
                              png_const_charp error_msg)
    {
        LOG1("PNG error, ", error_msg);
    }

    static void user_warning_fn(png_structp png_ptr,
                                png_const_charp warning_msg)
    {
        LOG2("PNG warning, ", warning_msg);
    }


    FILE *fp;
    png_struct* png_ptr;
    png_info* info_ptr;
    //png_color* palette;

    bool _writeBegin(const char* comment = 0)
    {
        LOG3("PNG, writing image, ", _filename);

        /* Open the file */
        fp = fopen(_filename.c_str(), "wb");
        if (fp == NULL)
        {
            LOG1("PNG, can't open output file '", _filename << "'\n");
            return false;
        }

        /* Create and initialize the png_struct with the desired error handler
         * functions.  If you want to use the default stderr and longjump method,
         * you can supply NULL for the last three parameters.  We also check that
         * the library version is compatible with the one used at compile time,
         * in case we are using dynamically linked libraries.  REQUIRED.
         */
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                          (void*)this,
                                          user_error_fn,
                                          user_warning_fn);

        if (png_ptr == NULL)
        {
            fclose(fp);
            return false;
        }

         /* Allocate/initialize the image information data.  REQUIRED */
        info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == NULL)
        {
            fclose(fp);
            png_destroy_write_struct(&png_ptr,  NULL);
            return false;
        }

        /* Set error handling.  REQUIRED if you aren't supplying your own
         * error handling functions in the png_create_write_struct() call.
         */
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            /* If we get here, we had a problem writing the file */
            fclose(fp);
            png_destroy_write_struct(&png_ptr, &info_ptr);
            return false;
        }

        png_init_io(png_ptr, fp);

        info_ptr->width = _img->_width;
        info_ptr->height = _img->_height;
        info_ptr->pixel_depth = _img->_bpp;
        info_ptr->channels = _img->_bpp/8;
        
        info_ptr->bit_depth = _img->_bpp/info_ptr->channels;
	info_ptr->compression_type = info_ptr->filter_type = 0;
	info_ptr->valid = 0;
        info_ptr->interlace_type = PNG_INTERLACE_NONE;
        info_ptr->color_type = PNG_COLOR_TYPE_RGB;

        if (info_ptr->channels == 4)
            info_ptr->color_type |= PNG_COLOR_MASK_ALPHA;

        int compress_level;

        switch (_compressLevel)
        {
        case level_none:
            compress_level = Z_NO_COMPRESSION;
            break;
	case level_fast:
            compress_level = Z_BEST_SPEED;
            break;
	case level_best:
            compress_level = Z_BEST_COMPRESSION;
            break;
	default:
            compress_level = Z_DEFAULT_COMPRESSION;
            break;
	}

        png_set_compression_level(png_ptr, compress_level);

        if (comment && *comment)
        {
             // Text
            png_text text;
            memset(&text, 0, sizeof(text));
            //text.compression = PNG_TEXT_COMPRESSION_NONE;

            const char* key = "www.strandgames.com";

            // compress the comment too!
            text.compression = PNG_TEXT_COMPRESSION_zTXt;
            text.key = (char*)key;
            text.text = (char*)comment;
            text.text_length = strlen(comment);
        
            int num_text = 1;
            png_set_text(png_ptr, info_ptr, &text, num_text);
            
        }

        info_ptr->rowbytes = info_ptr->width*info_ptr->channels*(info_ptr->bit_depth/8);
        return true;
    }

    bool _writeInfo()
    {
        png_write_info(png_ptr, info_ptr);
        return true;
    }

    uchar** _makeRowPointers(ImgData& img)
    {
        // caller must delete
        uchar** rp = new uchar*[img._height];
        for (int i = 0; i < img._height; ++i)
            rp[i] = img._data + i*img._stride;
        return rp;
    }

    bool _writeData()
    {
        if (info_ptr->rowbytes == _img->_stride)
        {
            const uchar* p = _img->_data;
            for (int i = 0; i < _img->_height; ++i)
            {
                png_write_row(png_ptr, p);
                p += _img->_stride;
            }
        }
        else
        {
            LOG3("PNG, width:", _img->_width);
            LOG3("PNG,  stride:", _img->_stride << " rowsize:" << info_ptr->rowbytes);
            
            uint8_t *row_pointers = new uint8_t[info_ptr->rowbytes];
            for (int i = 0; i < _img->_height; ++i)
            {
                assert(0);
                png_write_row(png_ptr, row_pointers);
            }

            delete [] row_pointers;
        }

        return true;
    }

    bool _writeEnd()
    {
        // write IEND marker
        png_write_end(png_ptr, info_ptr);
        
        png_destroy_write_struct(&png_ptr, (png_infopp)&info_ptr);

        fclose(fp);

        return true;
    }

    bool write(const char* file_name, const char* comment = 0)
    {
        // always save PNG
        _filename = changeSuffix(file_name, ".png");
        return _writeBegin(comment) &&
            _writeInfo() && _writeData() && _writeEnd();
    }

    bool write(const string& name, const char* comment = 0)
    {
        return write(name.c_str(), comment);
    }

};

struct ImgAPng: public ImgPng
{
    struct Frame
    {
        ImgData         _img;
        int             _x;
        int             _y;

        Frame(ImgData& f, int x, int y) : _img(f), _x(x), _y(y) {}
    };

    typedef std::vector<Frame>       Frames;

    // start with a base picture
    ImgAPng(ImgData* img) : ImgPng(img) {}

    Frames      _frames;
    int         _playbackDelay = 10; // 0-100%

    size_t      size() const { return _frames.size(); }

    void addFrame(ImgData& f, int x, int y)
    {
        // NB: we take ownership of f

        if (f.valid())
        {
            assert(_img && _img->valid());

            // ensure frame does not go out of bounds
            f.clip(x, y, _img->_width, _img->_height);

            if (f.valid())
                _frames.emplace_back(Frame(f, x, y));
        }
    }

    bool write(const string& name, const char* comment = 0)
    {
        return write(name.c_str(), comment);
    }

    bool write(const char* file_name, const char* comment = 0)
    {
        int nf = _frames.size();
        
        _filename = changeSuffix(file_name, (nf > 0 ? ".apng" : ".png"));
        
        bool r = _writeBegin(comment);
        
        if (r)
        {
            if (nf)
            {
                // loop
                LOG3("APNG, setting ", nf << " frames");
                png_set_acTL(png_ptr, info_ptr, nf + 1, 0);

                //png_set_first_frame_is_hidden(png_ptr, info_ptr, 1);
            }
            
            r = _writeInfo();

            if (r && nf)
            {

                //LOG3("APNG, writing base frame, w: ", _img->_width << " h:" << _img->_height);
                png_write_frame_head(png_ptr, info_ptr,
                                     NULL,
                                     _img->_width, _img->_height,
                                     0,0,
                                     _playbackDelay, 100,
                                     PNG_DISPOSE_OP_NONE, PNG_BLEND_OP_SOURCE);
            }
            
            r = r && _writeData();
            
            if (r && nf)
                png_write_frame_tail(png_ptr, info_ptr);
        }

        if (r)
        {
            // write the frames
            for (int i = 0; i < nf; ++i)
            {
                Frame& fi = _frames[i];

                //LOG3("APNG, writing frame ", i << " w: " << fi._img._width << " h:" << fi._img._height << " at x:" << fi._x << " y:" << fi._y);


                png_write_frame_head(png_ptr, info_ptr,
                                     NULL,
                                     fi._img._width, fi._img._height,
                                     fi._x, fi._y,
                                     12, 100,
                                     PNG_DISPOSE_OP_NONE, PNG_BLEND_OP_SOURCE);

                                

                uchar** rows = _makeRowPointers(fi._img);

                png_write_image(png_ptr, rows);
                delete [] rows;
                
                png_write_frame_tail(png_ptr, info_ptr);
            }

            r = _writeEnd();
        }
        
        return r;
    }

};
