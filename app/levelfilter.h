#pragma once

#if !defined(__APPLE__)
#include "filter.h"
#endif

#include "logged.h"
#include "types.h"

#define GETR(_p) (_p)[0]
#define GETG(_p) (_p)[1]
#define GETB(_p) (_p)[2]

#define GETI(_p) (GETR(_p) + GETG(_p) + GETB(_p))

struct RawPixels
{
    uchar*  _data;
    size_t  _size;
    int     _w;
    int     _h;
    int     _pixelSize;
    int     _stride;

    void init()
    {
        _stride = _w*_pixelSize;
        _size = _h*_stride;
    }
};

struct PixBox
{
    RawPixels*  _pix;
    int         _x;
    int         _y;
    
    int         _bw;
    int         _bh;
    
    int         _xi;
    int         _yi;
    uchar*      _dp = 0;

    PixBox(RawPixels* pix, int x, int y, int w, int h)
        : _pix(pix), _x(x), _y(y), _bw(w), _bh(h) {}

    void _update()
    {
        _dp = _pix->_data + _yi*_pix->_stride + _xi*_pix->_pixelSize;
    }

    operator bool() const { return _dp != 0; }

    void start()
    {
        _xi = _x;
        _yi = _y;
        _update();
    }

    void next()
    {
        _dp += _pix->_pixelSize;
        if (++_xi - _x >= _bw)
        {
            _xi = _x;
            ++_yi;

            _update();
            
            if (_yi - _y >= _bh)
            {
                _dp = 0;  // end
            }
        }
    }
};

struct LevelFilter
{
    RawPixels*  pix = 0;
    int         _min;
    int         _max;
    int         _dm = 0;

    void        reset()
    {
        pix = 0;
    }

    bool        valid() const { return pix != 0; }

    void quantiles_u8i(size_t nb_min, size_t nb_max)
    {
        size_t h_size = 256;
        size_t histo[256];
        size_t i;

        size_t size = pix->_w * pix->_h;
        uchar* dp = pix->_data;
    
        /* make a cumulative histogram */
        memset(histo, 0, h_size * sizeof(size_t));
        for (i = 0; i < size; i++)
        {
            int iv = GETI(dp)/3;
            histo[iv] += 1;
            dp += pix->_pixelSize;
        }
    
        for (i = 1; i < h_size; i++) histo[i] += histo[i - 1];

        /* get the new min/max */

        /* simple forward traversal of the cumulative histogram */
        /* search the first value > nb_min */
        i = 0;
        while (i < h_size && histo[i] <= nb_min) i++;
    
        /* the corresponding histogram value is the current cell position */
        _min = i;

        /* simple backward traversal of the cumulative histogram */
        /* search the first value <= size - nb_max */
        i = h_size - 1;
        /* i is unsigned, we check i<h_size instead of i>=0 */
        while (i < h_size && histo[i] > (size - nb_max)) i--;
    
        /*
         * if we are not at the end of the histogram,
         * get to the next cell,
         * the last (backward) value > size - nb_max
         */
        if (i < h_size - 1) i++;
        _max = i;
    }

    void findbalance(size_t nb_min = 0, size_t nb_max = 0)
    {
        int i;

        int size = pix->_w * pix->_h;
        uchar* dp;
    
        /* compute min and max */
        if (nb_min || nb_max)
        {
            quantiles_u8i(nb_min, nb_max);

            // consistent with overall minmax
            _min *= 3;
            _max *= 3;
        }
        else
        {
            // find overall min and max
            _min = 255*3;
            _max = 0;
            dp = pix->_data;
            for (i = 0; i < size; i++)
            {
                int iv = GETI(dp);
                if (iv < _min) _min = iv;
                if (iv > _max) _max = iv;
                dp += pix->_pixelSize;
            }
        }

        _dm = _max - _min;
    }

    void colorBalance(PixBox& pb)
    {
        if (_dm > 0)
        {
            //LOG3("Scaling levels by ", 3*255.0/_dm);

            for (pb.start(); pb; pb.next())
            {
                uchar* dp = pb._dp;
                int s = GETI(dp);
                if (!s) continue;
        
                double ss = 3*255.0*(1.0 - ((double)_min)/s)/_dm;

                /* if m * s > 1, a projection is needed by adjusting s */
                //if (m*ss > 1) ss = 1.0/m;

                int r = GETR(dp);
                int g = GETG(dp);
                int b = GETB(dp);

                // find max component
                int m = r;
                if (g > m) m = g;
                if (b > m) m = b;

                double mf = 255.0/m;
                if (ss > mf) ss = mf; // also prevents 255 overflow

                // only needed when using non-zero nb_min and max
                if (ss > 0)
                {
                    GETR(dp) *= ss;
                    GETG(dp) *= ss;
                    GETB(dp) *= ss;
                }
            }
        }
    }
};

#if 0
static inline bool levelFilter(RawPixels* rp)
{
    LOG1("Performing Level filter size ", rp->_w << "x" << rp->_h);

    LevelFilter lf;
    lf.pix = rp->_pix;
    lf.findbalance();

    PixBox pb(rp, 0, 0, rp->_w, rp->_h);
    lf.colorBalance(pb);

#if 0
    // this version takes 1% of pixels to min/max
    int size = rp->_w * rp->_h;
    int n = size/100; // 1%
    colorbalance(rp, n, n);
#endif
    
    return true;
}
#endif
