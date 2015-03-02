#ifndef _FFMPEGIMAGE_H_
#define _FFMPEGIMAGE_H_

#include <string>
#include "FFMpegMedia.h"

using namespace std;

#pragma once
class FFMpegImage: public FFMpegMedia
{
public:
    FFMpegImage(void);
    ~FFMpegImage(void);

    bool InitialAndOpenOutputCodecCtx(const char* filename=NULL);

    bool ReadFrame(); // Decoded Frame
    bool SaveFrame();

private:
    bool OpenOutputImageCodecContext(const char* filename=NULL);
    
    
};

#endif
