#ifndef _FFMPEGVIDEO_H_
#define _FFMPEGVIDEO_H_
#include "FFMpegMedia.h"

#pragma once
class FFMpegVideo: public FFMpegMedia
{
public:
    FFMpegVideo(void);
    ~FFMpegVideo(void);
    
    bool InitialAndOpenOutputCodecCtx(const char* filename=NULL);

    bool ReadFrame(); // Decoded Frame

private:
};

#endif
