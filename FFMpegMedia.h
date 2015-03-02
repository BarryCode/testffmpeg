#ifndef _FFMPEGMEDIA_H_
#define _FFMPEGMEDIA_H_
#include <string>
#include "FFMpegUtil.h"

using namespace std;
#pragma once
class FFMpegMedia
{
public:
    enum EFrameType{
        EVideo = 0,
        EAudio,
        EOther
    };
    FFMpegMedia(void);
    virtual ~FFMpegMedia(void);

    virtual bool LoadInputFileInfo(const char* filename);
    virtual bool OpenInputFileCodecCtx();
    virtual bool InitialOutputFmtCtx(const char* filename);
    virtual bool InitialAndOpenOutputCodecCtx(const char* filename=NULL) = 0;
    virtual bool CopyAndOpenOutputVideoCodecCtx(const AVCodecContext *src);
    virtual bool CopyAndOpenOutputAudioCodecCtx(const AVCodecContext *src);
    virtual bool AllocOutputFrame();
    virtual bool OpenOutputFile(const char* filename = NULL);
    virtual bool CloseOutputFile();
    virtual bool ReadFrame();
    virtual bool SaveFrame();
    virtual bool EncodeFrame();
    virtual bool SeekFrame(int64_t time_stamp);
    virtual bool ReadPacket();
    virtual bool DecodeCurPacket();
    void SetInputFrame(AVFrame* frame);
    AVFrame* GetInputFrame();
    void SetOutputPacket(AVPacket packet);
    AVPacket GetInputPacket();
    void SetInputWidth(int width);
    int GetInputWidth();
    void SetInputHeight(int height);
    int GetInputHeight();
    void SetInputPixFmt(AVPixelFormat pixfmt);
    AVPixelFormat GetInputPixFmt();
    EFrameType GetCurFrameType();
    void SetCurFrameType(EFrameType type);
    AVCodecContext* GetInputAudioCodecCtx();
    AVCodecContext* GetInputVideoCodecCtx();
    AVStream* GetInputAudioStream();
    void SetInputAudioStream(AVStream* stream);
    bool InitialNewVideoStream();
    bool InitialNewAudioStream();

private:
    bool InitialInputFrame();
    bool InitialOutputVideoFrame();
    bool InitialOutputAudioFrame();
    void InitialPacket(AVPacket& packet);
    virtual void SetAudioStreamIndex();
    void Clear();

protected:
    AVFormatContext* inputFmtCtx;
    AVFormatContext* outputFmtCtx;
    AVOutputFormat* outputFmt;
    AVStream* inputVideoStream;
    AVStream* inputAudioStream;
    AVStream* outputVideoStream;
    AVStream* outputAudioStream;
    AVCodecContext* inputVideoCodecCtx;
    AVCodecContext* inputAudioCodecCtx;
    AVCodecContext* outputVideoCodecCtx;
    AVCodecContext* outputAudioCodecCtx;
    AVFrame* inputFrame;
    AVFrame* outputVideoFrame;
    AVFrame* outputAudioFrame;
    SwsContext* swsContext;
    FILE *outputFile;
    uint8_t* outputVideoData;
    int outputVideoDataSize;
    uint8_t* outputAudioData;
    int outputAudioDataSize;
    AVPacket inputPacket;
    AVPacket outputPacket;
    int videoIndex;
    int audioIndex;
    int gotDecodedFrame;
    int inputWidth;
    int inputHeight;
    AVPixelFormat inputPixelFormat;
    int outputWidth;
    int outputHeight;
    AVPixelFormat outputPixelFormat;
    string inputFilename;
    string outputFilename;
    string outputFileExt;
    EFrameType currentFrameType;
};

#endif
