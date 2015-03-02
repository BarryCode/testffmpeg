#ifndef _FFMPEGAUDIO_H_
#define _FFMPEGAUDIO_H_
#include "FFMpegMedia.h"


#pragma once
class FFMpegAudio: public FFMpegMedia
{
public:
    FFMpegAudio(void);
    ~FFMpegAudio(void);

    bool InitialOutputFmtCtx(const char* filename);
    bool InitialAndOpenOutputCodecCtx(const char* filename=NULL);
    void SetAudioStreamIndex();
};

#endif
