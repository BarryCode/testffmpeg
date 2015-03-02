#ifndef FFMPEGDEMUX
#define FFMPEGDEMUX
#pragma once
#include <string>
#include "FFMpegVideo.h"

using namespace std;

class FFMpegDemux
{
public:
    FFMpegDemux();
    ~FFMpegDemux(void);

    bool OpenInputVideo(const char* filename);
    bool OpenOutputVideo(const char* filename);
    bool OpenOutputAudio(const char* filename);
    bool Demux();
    void CloseInputVideo();
    void CloseoutputVideo();
    void CloseOutputAudio();

private:
    //int detachVideoFrame();

private:
    FFMpegVideo* inputVideo;
    FFMpegVideo* outputVideo;
};

#endif
