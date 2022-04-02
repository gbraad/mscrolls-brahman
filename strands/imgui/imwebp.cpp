
// webp

#include "imwebp.h"
#include "src/webp/decode.h"
#include "logged.h"


unsigned char* decodeWebp(const unsigned char* data,
                          int dsize,
                          int* w,
                          int* h,
                          int* chans,
                          int desired_channels)
{
    uint8_t* pix;

    pix = WebPDecodeRGBA((const uint8_t*)data, (size_t)dsize, w, h);
    if (pix)
    {
        *chans = 4;
    }
    return pix;
}

void decodeWebpFree(unsigned char* data)
{
    WebPFree(data);
}
       
