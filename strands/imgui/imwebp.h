
#pragma once


unsigned char* decodeWebp(const unsigned char* data,
                          int dsize,
                          int* w,
                          int* h,
                          int* chans,
                          int desired_channels);

void decodeWebpFree(unsigned char*);
