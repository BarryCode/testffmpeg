#ifndef BITMAPPARSE
#define BITMAPPARSE

#pragma once
#include <stdint.h>
#include "StdAfx.h"
#include "FFMpegFrameRecorder.h"


class BitMapParse
{
public:
    class BitmapFileHeader {
    public:
         uint16_t    bfType;          // the flag of bmp, value is "BM"
         uint32_t    bfSize;                // size BMP file ,unit is uint8_ts
         uint16_t    bfReserved1,bfReserved2;            // 0
         uint32_t    bfOffBits;             // must be 54
    };

    struct BitmapInfoHeader{
         uint32_t    biSize;                // must be 0x28
         uint32_t    biWidth;           //
         uint32_t    biHeight;          //
         uint16_t    biPlanes;          // must be 1
         uint16_t    biBitCount;            //
         uint32_t    biCompression;         //
         uint32_t    biSizeImage;       //
         uint32_t    biXPelsPerMeter;   //
         uint32_t    biYPelsPerMeter;   //
         uint32_t    biClrUsed;             //
         uint32_t    biClrImportant;        //
    };

    struct RGBQuad{
         uint8_t rgbBlue;
         uint8_t rgbGreen;
         uint8_t rgbRed;
         uint8_t rgbReserved;
    };

    struct BitmapInfo{
              BitmapFileHeader  bmiHeader;
              RGBQuad           bmiColors[1];
    };

    BitMapParse(void);
    ~BitMapParse(void);

    bool Parse(const char* filename, Image* image);

public:
    BitmapFileHeader bmpFileHeader;
    BitmapInfoHeader bmpInfoHeader;
    size_t buffer_size;
    uint8_t *buffer;
};

#endif
