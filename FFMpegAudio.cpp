#include "StdAfx.h"
#include "FFMpegAudio.h"


FFMpegAudio::FFMpegAudio(void)
{
}


FFMpegAudio::~FFMpegAudio(void)
{
}


bool FFMpegAudio::InitialOutputFmtCtx(const char* filename)
{
    if(filename == NULL){
        fprintf(stderr, "File name is null\n");
        return false;
    }
    outputFilename.clear();
    outputFilename.append(filename);

    if(!FFMpegUtil::InitialOutputFmtCtx(outputFmtCtx, filename)) exit(1);
    if(!FFMpegUtil::InitialNewStream(outputFmtCtx, 
        outputFmtCtx->oformat->audio_codec, outputAudioStream)) exit(1);
    return true;
}

bool FFMpegAudio::InitialAndOpenOutputCodecCtx(const char* filename)
{
    /* find the MP2 encoder */
    AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MP2);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    outputAudioCodecCtx = avcodec_alloc_context3(codec);
    if (!outputAudioCodecCtx) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    /* put sample parameters */
    outputAudioCodecCtx->bit_rate = 64000;

    /* check that the encoder supports s16 pcm input */
    outputAudioCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    if (!FFMpegUtil::CheckSampleFmt(codec, outputAudioCodecCtx->sample_fmt)) {
        fprintf(stderr, "Encoder does not support sample format %s",
                av_get_sample_fmt_name(outputAudioCodecCtx->sample_fmt));
        exit(1);
    }

    /* select other audio parameters supported by the encoder */
    outputAudioCodecCtx->sample_rate    = FFMpegUtil::SelectSampleRate(codec);
    outputAudioCodecCtx->channel_layout = FFMpegUtil::SelectChannelLayout(codec);
    outputAudioCodecCtx->channels       = av_get_channel_layout_nb_channels(outputAudioCodecCtx->channel_layout);

    /* open it */
    if (avcodec_open2(outputAudioCodecCtx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
    return true;
}


void FFMpegAudio::SetAudioStreamIndex()
{
    outputPacket.stream_index=0;
}
