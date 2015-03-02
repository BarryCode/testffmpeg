#ifndef FFMPEGDEMUX
#define FFMPEGDEMUX
#pragma once
#include <string>
using namespace std;
extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavutil/avutil.h"
    #include "libavutil/file.h"
}

class FFMpegDemux
{
public:
    FFMpegDemux(string& ifilename, string& ofilename);
    FFMpegDemux(const char* ifilename, const char* ofilename);
    ~FFMpegDemux(void);

    void init();
    bool openInputFile();
    bool openOutputFile();
    bool demux();
    void close();

private:
    //int detachVideoFrame();

private:
    AVFormatContext* iFormatContext;
    AVFormatContext* oFormatContext;
    AVOutputFormat *oFormat;
    AVStream *iStream;
    AVStream *oStream;
    AVCodec* videoCodec;
    AVFrame* frame;
    AVPacket packet;
    string iFilename;
    string oFilename;
    int videoIndex;
    //int audioIndex;
    //int gotFrame;
};

#endif
