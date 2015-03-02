#ifndef FFMPEGFRAMERECORDER
#define FFMPEGFRAMERECORDER

#pragma once
#include <string>
#include "FrameRecorder.h"

using namespace std;

//extern "C"
//{
//    #include "libavcodec/avcodec.h"
//    #include "libavformat/avformat.h"
//    #include "libswscale/swscale.h"
//    #include "libavutil/avutil.h"
//    #include "libavutil/file.h"
//}


extern "C"
{
	#include <libavutil/opt.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/channel_layout.h>
	#include <libavutil/common.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/samplefmt.h>
	#include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavutil/file.h"
    #include "libavutil/error.h"
}

struct Image{
    Image():width(0),height(0),widthStep(0),imageData(NULL){};
    int width;
    int height;
    int widthStep;
    uint8_t* imageData;
};

class FFMpegFrameRecorder :
    public FrameRecorder
{
public:
    FFMpegFrameRecorder(const char* filename);
    ~FFMpegFrameRecorder(void);

    virtual void init();
    virtual void start();
    virtual void stop();
    //virtual bool record(IplImage image) = 0;
    //virtual bool record(Buffer ... samples);
    //virtual bool record(int sampleRate, int audioChannels, Buffer ... samples);
    bool record(Image* image, int pixelFormat);
    bool record(AVFrame* frame);

    bool readImage(const char* filename);

private:
    void initial();
    bool initialVideoCodec();
    bool openOutputFile();
    bool allocMemory();
    void release();

private:
    string oFilename;
    AVFrame* picture;
    AVFrame* tmp_picture;
    uint8_t* picture_buf;
    uint8_t* video_outbuf;
    int video_outbuf_size;
    AVFrame* frame;
    AVOutputFormat* oFormat;
    AVFormatContext* oFormatContext;
    AVFormatContext* iFormatContext;
    AVCodec* video_codec;
    AVCodecContext* video_c;
    AVStream* video_st;
    SwsContext* img_convert_ctx;
    AVPacket packet;
    int got_video_packet;
    int videoIndex;
};

#endif
