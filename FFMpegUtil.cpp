#include "StdAfx.h"
#include "FFMpegUtil.h"

FFMpegUtil::FFMpegUtil(void)
{
}

FFMpegUtil::~FFMpegUtil(void)
{
}

void FFMpegUtil::RegisterForAll()
{
    av_register_all();
    avcodec_register_all();
    //avfilter_register_all();
}

bool FFMpegUtil::OpenInputFile(AVFormatContext* &input_fmt_ctx, const char* filename)
{
    /* open input file, and allocate format context */
    if (avformat_open_input(&input_fmt_ctx, filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", filename);
        return false;
    }

    /* retrieve stream information */
    if (avformat_find_stream_info(input_fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        return false;
    }

    av_dump_format(input_fmt_ctx, 0, filename, 0);

    return true;
}

bool FFMpegUtil::InitialOutputFmtCtx(AVFormatContext* &output_fmt_ctx, const char* filename)
{
    /* allocate the output media context */
    avformat_alloc_output_context2(&output_fmt_ctx, NULL, NULL, filename);
    if (!output_fmt_ctx) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        return false;
    }
    
    av_dump_format(output_fmt_ctx, 0, filename, 1);

    return true;
}

bool FFMpegUtil::InitialNewStream(AVFormatContext* &fmt_ctx, AVCodecID codec_id, AVStream* &output_stream)
{
    AVCodec* codec = avcodec_find_encoder(codec_id);
    if(!codec) {
        fprintf(stderr, "codec not found\n");
        return false;
    }

    /* add a video output stream */
    if ((output_stream = avformat_new_stream(fmt_ctx, codec)) == NULL) {
        fprintf(stderr, "avformat_new_stream() error: Could not allocate video stream.");
        return false;
    }

    return true;
    //if(output_fmt_ctx->oformat->audio_codec != AV_CODEC_ID_NONE){
    //    codec = avcodec_find_encoder(output_fmt_ctx->oformat->audio_codec);
    //    if(!codec) {
    //        fprintf(stderr, "codec not found\n");
    //        return false;
    //    }

    //    /* add a video output stream */
    //    if ((output_audio_stream = avformat_new_stream(output_fmt_ctx, codec)) == NULL) {
    //        fprintf(stderr, "avformat_new_stream() error: Could not allocate video stream.");
    //        return false;
    //    }
    //}

}

bool FFMpegUtil::OpenCodecContext(AVCodecContext *codec_ctx)
{
    int ret;
    AVCodec *dec = NULL;

    if(codec_ctx == NULL) return false;

    /* find decoder for the stream */
    dec = avcodec_find_decoder(codec_ctx->codec_id);
    if (!dec) {
        fprintf(stderr, "Failed to find %d codec\n", codec_ctx->codec_id);
        return false;
    }

    /* Init the video decoder */
    if (avcodec_open2(codec_ctx, dec, NULL) < 0) {
        fprintf(stderr, "Failed to open %d codec\n", codec_ctx->codec_id);
        return false;
    }

    return true;
}

int FFMpegUtil::FindStreamIndex(AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    return av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
}

void FFMpegUtil::CloseFmtCtx(AVFormatContext* &fmt_ctx)
{
    if (fmt_ctx!=NULL) {
        avformat_close_input(&fmt_ctx);
        fmt_ctx = NULL;
    }
}

void FFMpegUtil::CloseCodecCtx(AVCodecContext* &codec_ctx)
{
    if (codec_ctx!=NULL) {
        avcodec_close(codec_ctx);
        codec_ctx = NULL;
    }
}

void FFMpegUtil::CloseFrame(AVFrame* &frame)
{
    if (frame != NULL) {
        avcodec_free_frame(&frame);
        frame = NULL;
    }
}

void FFMpegUtil::CloseSwsContext(SwsContext* &context)
{
    if (context != NULL) {
        sws_freeContext(context);
        context = NULL;
    }
}

enum AVCodecID FFMpegUtil::Str2ID(const IdStrMap *tags, const char *str)
{
    str = strrchr(str, '.');
    if (!str)
        return AV_CODEC_ID_NONE;
    str++;

    while (tags->id) {
        if (!av_strcasecmp(str, tags->str))
            return tags->id;

        tags++;
    }
    return AV_CODEC_ID_NONE;
}

enum AVCodecID FFMpegUtil::GuessImageCodec(const char *filename)
{
    return Str2ID(img_tags, filename);
}


/* check that a given sample format is supported by the encoder */
int FFMpegUtil::CheckSampleFmt(AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

/* just pick the highest supported samplerate */
int FFMpegUtil::SelectSampleRate(AVCodec *codec)
{
    const int *p;
    int best_samplerate = 0;

    if (!codec->supported_samplerates)
        return 44100;

    p = codec->supported_samplerates;
    while (*p) {
        best_samplerate = FFMAX(*p, best_samplerate);
        p++;
    }
    return best_samplerate;
}

/* select layout with the highest channel count */
int FFMpegUtil::SelectChannelLayout(AVCodec *codec)
{
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels   = 0;

    if (!codec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;

    p = codec->channel_layouts;
    while (*p) {
        int nb_channels = av_get_channel_layout_nb_channels(*p);

        if (nb_channels > best_nb_channels) {
            best_ch_layout    = *p;
            best_nb_channels = nb_channels;
        }
        p++;
    }
    return best_ch_layout;
}
