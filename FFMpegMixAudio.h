#pragma once
#include "FFMpegUtil.h"

class FFMpegMixAudio
{
public:
    FFMpegMixAudio(void);
    ~FFMpegMixAudio(void);

    void mix_audios(const char* src_file1, const char* src_file2, const char* dst_file);
    
private:
    
    int open_input_file(const char *filename, AVFormatContext*& fmt_ctx, AVCodecContext*& dec_ctx);
    int init_filters(const char *filters_descr);
    void getAudio(AVFormatContext* fmt_ctx, AVCodecContext* ctx, AVFrame* frame, AVFilterContext* dst_filter);
    void getMixAudio();

    void open_mp3_encodec();

private:

    int audio_stream_index;
    AVFilterGraph* filter_graph;
    AVFormatContext *fmt_ctx1;
    AVFormatContext *fmt_ctx2;
    AVCodecContext *dec_ctx1;
    AVCodecContext *dec_ctx2;
    AVCodecContext *dec_ctx;

    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx1;
    AVFilterContext *buffersrc_ctx2;
    
    AVFormatContext* pFormatCtx;
    AVFrame* pFrame;
    AVCodecContext* pCodecCtx;
};

