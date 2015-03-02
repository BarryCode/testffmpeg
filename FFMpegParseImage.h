#ifndef FFMPEGPARSEIMAGE
#define FFMPEGPARSEIMAGE

#pragma once

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavutil/avutil.h"
    #include "libavutil/file.h"
}
class FFMpegParseImage
{
public:
    FFMpegParseImage(void);
    ~FFMpegParseImage(void);

    bool openImageFile(const char *filename);
    bool initialOutputVideoFile(const char *filename);
    bool decodeImageData();
    void close();

    static int open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx, AVCodecContext **codec_ctx, enum AVMediaType type);
    int decode_packet(int *got_frame, int cached);
    bool initialVideoCodec();
    bool WriteJPEG (AVCodecContext *pCodecCtx, AVFrame *pFrame, char cFileName[], PixelFormat pix, uint8_t *buffer, int numBytes);
    bool SaveFrame(int nszBuffer, uint8_t *buffer, char cOutFileName[]);

private:
    AVFormatContext* iFormatContext;
    AVFormatContext* oFormatContext;
    AVOutputFormat* oFormat;
    AVCodecContext *iCodecContext;
    AVCodec* oVideoCodec;
    AVCodecContext* oVideoCodecCtx;
    AVStream* oStream;
    AVStream* iStream;
    AVFrame* frame;
    AVFrame* dstFrame;
    uint8_t* dstData;
    SwsContext* convertContext;
    int width;
    int height;
    AVPixelFormat imagePixelFormat;
    int videoIndex;
    AVPacket packet;
    AVPacket oPacket;
    uint8_t* packetBuffer;
    int packetBufferSize;
};

#endif
