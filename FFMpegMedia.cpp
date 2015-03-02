#include "StdAfx.h"
#include "FFMpegMedia.h"


FFMpegMedia::FFMpegMedia(void):
    inputFmtCtx(NULL),
    outputFmtCtx(NULL),
    inputVideoStream(NULL),
    inputAudioStream(NULL),
    outputVideoStream(NULL),
    outputAudioStream(NULL),
    inputVideoCodecCtx(NULL),
    inputAudioCodecCtx(NULL),
    outputVideoCodecCtx(NULL),
    outputAudioCodecCtx(NULL),
    inputFrame(NULL),
    outputVideoFrame(NULL),
    outputAudioFrame(NULL),
    outputFile(NULL),
    swsContext(NULL),
    outputVideoData(NULL),
    videoIndex(0),
    audioIndex(0),
    gotDecodedFrame(0),
    inputWidth(0),
    inputHeight(0),
    inputPixelFormat(AV_PIX_FMT_NONE)
{
    FFMpegUtil::RegisterForAll();
}


FFMpegMedia::~FFMpegMedia(void)
{
    Clear();
}

bool FFMpegMedia::LoadInputFileInfo(const char* filename)
{
    if(filename == NULL){
        fprintf(stderr, "File name is null\n");
        return false;
    }
    inputFilename.clear();
    inputFilename.append(filename);

    if (!FFMpegUtil::OpenInputFile(inputFmtCtx, filename)){
        exit(1);
    }
    if((videoIndex = FFMpegUtil::FindStreamIndex(inputFmtCtx, AVMEDIA_TYPE_VIDEO))>=0){
        inputVideoStream = inputFmtCtx->streams[videoIndex];
        inputVideoCodecCtx = inputVideoStream->codec;

        inputWidth = inputVideoCodecCtx->width;
        inputHeight = inputVideoCodecCtx->height;
        inputPixelFormat = inputVideoCodecCtx->pix_fmt;
    }

    if((audioIndex = FFMpegUtil::FindStreamIndex(inputFmtCtx, AVMEDIA_TYPE_AUDIO))>=0){
        inputAudioStream = inputFmtCtx->streams[audioIndex];
        inputAudioCodecCtx = inputAudioStream->codec;
    }

    InitialPacket(inputPacket);

    return true;
}

bool FFMpegMedia::OpenInputFileCodecCtx()
{
    if(inputVideoCodecCtx!=NULL){
        if (!FFMpegUtil::OpenCodecContext(inputVideoCodecCtx)){
            exit(1);
        }

    }

    if(inputAudioCodecCtx!=NULL){
        if (!FFMpegUtil::OpenCodecContext(inputAudioCodecCtx)){
            exit(1);
        }
    }
    
    //If open the video codec context, it means also needs a 
    //frame to store the decoded frame.
    if(inputFrame == NULL){
        if(!InitialInputFrame()) exit(1);
    }
    return true;
}

bool FFMpegMedia::InitialOutputFmtCtx(const char* filename)
{
    if(filename == NULL){
        fprintf(stderr, "File name is null\n");
        return false;
    }
    outputFilename.clear();
    outputFilename.append(filename);

    if(!FFMpegUtil::InitialOutputFmtCtx(outputFmtCtx, filename)) exit(1);
    if(!InitialNewVideoStream()) exit(1);
    return true;
}

bool FFMpegMedia::CopyAndOpenOutputVideoCodecCtx(const AVCodecContext *src)
{
    AVCodec* codec;
    if(outputFmtCtx != NULL){
        codec = avcodec_find_encoder(outputFmtCtx->oformat->video_codec);
        if (avcodec_copy_context(outputVideoStream->codec,src) < 0) {
            fprintf(stderr, "Failed to copy context from input to output stream codec context\n");
            return false;
        }
        outputVideoCodecCtx= outputVideoStream->codec;
        outputVideoStream->codec->codec_tag = 0;  
  
        if (outputFmtCtx->oformat->flags & AVFMT_GLOBALHEADER)  
            outputVideoStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

    } else {
        codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        if(!codec) {
            fprintf(stderr, "codec not found\n");
            return false;
        }
        outputVideoCodecCtx = avcodec_alloc_context3(codec);
        if (avcodec_copy_context(outputVideoCodecCtx,src) < 0) {
            fprintf(stderr, "Failed to copy context from input to output stream codec context\n");
            return false;
        }
    }

    /* Init the video decoder */
    if (avcodec_open2(outputVideoCodecCtx, codec, NULL) < 0) {
        fprintf(stderr, "Failed to open %d codec\n", outputVideoCodecCtx->codec_id);
        return false;
    }

    outputWidth = outputVideoCodecCtx->width;
    outputHeight = outputVideoCodecCtx->height;
    outputPixelFormat = outputVideoCodecCtx->pix_fmt;
    return true;
}


bool FFMpegMedia::CopyAndOpenOutputAudioCodecCtx(const AVCodecContext *src)
{
    AVCodec* codec;
    if(outputFmtCtx != NULL){
        codec = avcodec_find_encoder(outputFmtCtx->oformat->audio_codec);
        if (avcodec_copy_context(outputAudioStream->codec,src) < 0) {
            fprintf(stderr, "Failed to copy context from input to output stream codec context\n");
            return false;
        }
        outputAudioCodecCtx= outputAudioStream->codec;
        outputAudioStream->codec->codec_tag = 0;  
  
        if (outputFmtCtx->oformat->flags & AVFMT_GLOBALHEADER)  
            outputAudioStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;  

    } else {
        codec = avcodec_find_encoder(src->codec_id);
        if(!codec) {
            fprintf(stderr, "codec not found\n");
            return false;
        }
        outputAudioCodecCtx = avcodec_alloc_context3(codec);
        if (avcodec_copy_context(outputAudioCodecCtx,src) < 0) {
            fprintf(stderr, "Failed to copy context from input to output stream codec context\n");
            return false;
        }
    }
    
    ///* Init the audio decoder */
    //if (avcodec_open2(outputAudioCodecCtx, codec, NULL) < 0) {
    //    fprintf(stderr, "Failed to open %d codec\n", outputVideoCodecCtx->codec_id);
    //    return false;
    //}

    return true;
}

bool FFMpegMedia::OpenOutputFile(const char* filename)
{
    if(outputFmtCtx != NULL){
        /* open the output file, if needed */
        if (!(outputFmtCtx->oformat->flags & AVFMT_NOFILE)) {
            if (avio_open(&outputFmtCtx->pb, outputFilename.c_str(), AVIO_FLAG_WRITE) < 0) {
                fprintf(stderr, "Could not open '%s'\n", filename);
                return false;
            }
        }
        /* Write the stream header, if any. */
        if (avformat_write_header(outputFmtCtx, NULL) < 0) {
            fprintf(stderr, "Error occurred when opening output file\n");
            return false;
        }

    } else {
        if(filename == NULL){
            fprintf(stderr, "File name is null\n");
            return false;
        }
        outputFilename.clear();
        outputFilename.append(filename);

        outputFile = fopen(filename, "wb");
        if (!outputFile) {
            return false;
        }
    }

    InitialPacket(outputPacket);
    return true;
}

bool FFMpegMedia::AllocOutputFrame()
{
    if(outputVideoCodecCtx!=NULL){
        //if (!FFMpegUtil::OpenCodecContext(outputVideoCodecCtx)){
        //    exit(1);
        //}

        //If open the video codec context, it means also needs a 
        //frame to store the decoded frame.
        if(outputVideoFrame == NULL){
            if(!InitialOutputVideoFrame()) exit(1);
        }
    }

    if(outputAudioCodecCtx!=NULL){
        if(outputAudioFrame == NULL){
            if(!InitialOutputAudioFrame()) exit(1);
        }
    }

    return true;
}

bool FFMpegMedia::CloseOutputFile()
{
    if(outputFmtCtx != NULL){
        if (av_write_trailer(outputFmtCtx)<0) {
            return false;
        }
        if (!(outputFmtCtx->oformat->flags & AVFMT_NOFILE)) {
            /* Close the output file. */
            avio_closep(&outputFmtCtx->pb);
        }
    } else {
        if(outputFile != NULL){
            fclose(outputFile);
            outputFile=NULL;
        }
    }
    return true;
}

bool FFMpegMedia::ReadFrame()
{    
    /* read frames from the file */
    while (ReadPacket()) {
        if (currentFrameType == EFrameType::EVideo) {
        /* decode video frame */
            if (avcodec_decode_video2(inputVideoCodecCtx, inputFrame, &gotDecodedFrame, &inputPacket) < 0) {
                fprintf(stderr, "Error decoding video frame\n");
            }
            if(gotDecodedFrame == 0){
                fprintf(stderr, "Error decoding video frame, no frame\n");
            }
        } else if(currentFrameType == EFrameType::EAudio){
            if (avcodec_decode_audio4(inputVideoCodecCtx, inputFrame, &gotDecodedFrame, &inputPacket) < 0) {
                fprintf(stderr, "Error decoding video frame\n");
            }
        }

        av_free_packet(&inputPacket);
    }

    return true;
}

bool FFMpegMedia::SaveFrame()
{
    if(outputFmtCtx != NULL){
        AVStream* inputstream, *outputstream;
        if(currentFrameType == FFMpegMedia::EVideo){
            inputstream = inputVideoStream;
            outputstream = outputVideoStream;
            outputPacket.stream_index=0;
        } else {
            inputstream = inputAudioStream;
            outputstream = outputAudioStream;
        }
        outputPacket.pts = av_rescale_q_rnd(outputPacket.pts, inputstream->time_base, 
            outputstream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        outputPacket.dts = av_rescale_q_rnd(outputPacket.dts, inputstream->time_base, 
            outputstream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        outputPacket.duration = av_rescale_q(outputPacket.duration, inputstream->time_base,
            outputstream->time_base);
        outputPacket.pos = -1;

        SetAudioStreamIndex();

        if (av_interleaved_write_frame(outputFmtCtx,&outputPacket)<0) {
            return false;
        }
    } else {
        if(outputFile){
            if(fwrite(outputPacket.data, 1, outputPacket.size, outputFile)<0){
                return false;
            }
        }
    }

    av_free_packet(&outputPacket);
    return true;
}

bool FFMpegMedia::EncodeFrame()
{
    int size;
    if(currentFrameType == FFMpegMedia::EVideo){
        int ret;
        ret = sws_scale(swsContext, inputFrame->data, inputFrame->linesize,
            0, inputHeight, outputVideoFrame->data, outputVideoFrame->linesize);
        if(ret<0){
            fprintf(stderr, "sws_scale() error: Cannot sws_scale the conversion context.\n");
            return false;
        }

        int gotPocket;
        //picture->quality=video_c->global_quality;
        if ((size = avcodec_encode_video2(outputVideoCodecCtx, &outputPacket, outputVideoFrame, &gotPocket)) < 0) {
            fprintf(stderr, "avcodec_encode_video2() error %d: Could not encode video packet.\n");
            return false;
        }

        if(gotPocket == 0){
            fprintf(stderr, "avcodec_encode_video2() error %d: no frame.\n");
            return false;
        }
    } else if(currentFrameType == FFMpegMedia::EAudio){
    }

    return true; 
}

bool FFMpegMedia::SeekFrame(int64_t time_stamp)
{
    if( avformat_seek_file(inputFmtCtx, videoIndex, time_stamp, time_stamp, time_stamp, AVSEEK_FLAG_FRAME)<0){
        return false;
    }
    return true;
}

bool FFMpegMedia::ReadPacket()
{
    while (av_read_frame(inputFmtCtx, &inputPacket) >= 0) {
        if (inputPacket.stream_index == videoIndex) {
            currentFrameType = EVideo;
            return true;
        } else if (inputPacket.stream_index == audioIndex){
            currentFrameType = EAudio;
            return true;
        }
    }
    return false;
}

bool FFMpegMedia::DecodeCurPacket(){

    /* decode video frame */
    if (avcodec_decode_video2(inputVideoCodecCtx, inputFrame, &gotDecodedFrame, &inputPacket) < 0) {
        return false;
    }
    if(gotDecodedFrame == 0){
        return false;
    }
    av_free_packet(&inputPacket);
    return true;
}

void FFMpegMedia::SetInputFrame(AVFrame* frame)
{
    inputFrame = frame;
}

AVFrame* FFMpegMedia::GetInputFrame()
{
    return inputFrame;
}

void FFMpegMedia::SetOutputPacket(AVPacket packet)
{
    outputPacket = packet;
}

AVPacket FFMpegMedia::GetInputPacket()
{
    return inputPacket;
}

void FFMpegMedia::SetInputWidth(int width)
{
    inputWidth = width;
}

int FFMpegMedia::GetInputWidth()
{
    return inputWidth;
}

void FFMpegMedia::SetInputHeight(int height)
{
    inputHeight = height;
}

int FFMpegMedia::GetInputHeight()
{
    return inputHeight;
}

void FFMpegMedia::SetInputPixFmt(AVPixelFormat pixfmt)
{
    inputPixelFormat = pixfmt;
}

AVPixelFormat FFMpegMedia::GetInputPixFmt()
{
    return inputPixelFormat;
}

FFMpegMedia::EFrameType FFMpegMedia::GetCurFrameType()
{
    return currentFrameType;
}

void FFMpegMedia::SetCurFrameType(EFrameType type)
{
    currentFrameType = type;
}

AVCodecContext* FFMpegMedia::GetInputAudioCodecCtx()
{
    return inputAudioCodecCtx;
}

AVCodecContext* FFMpegMedia::GetInputVideoCodecCtx()
{
    return inputVideoCodecCtx;
}

AVStream* FFMpegMedia::GetInputAudioStream()
{
    return inputAudioStream;
}
void FFMpegMedia::SetInputAudioStream(AVStream* stream)
{
    inputAudioStream = stream;
}

bool FFMpegMedia::InitialNewVideoStream()
{
   return FFMpegUtil::InitialNewStream(outputFmtCtx, 
        outputFmtCtx->oformat->video_codec, outputVideoStream);
}
bool FFMpegMedia::InitialNewAudioStream()
{
    return FFMpegUtil::InitialNewStream(outputFmtCtx, 
        outputFmtCtx->oformat->audio_codec, outputAudioStream);
}

//PRIVATE FUNCTIONS
bool FFMpegMedia::InitialInputFrame()
{
    inputFrame = av_frame_alloc();
    if (!inputFrame) {
        fprintf(stderr, "Could not allocate frame\n");
        return false;
    }
    return true;
}

bool FFMpegMedia::InitialOutputVideoFrame()
{
    /* alloate the encoded raw picture */
    if ((outputVideoFrame = av_frame_alloc()) == NULL) {
        fprintf(stderr, "avcodec_alloc_frame() error: Could not allocate picture.\n");
        return false;
    }
    outputVideoFrame->pts=0; // magic required by libx264


    outputVideoDataSize = avpicture_get_size(outputPixelFormat, outputWidth, outputHeight);
    if ((outputVideoData = (uint8_t*)av_malloc(outputVideoDataSize)) == NULL) {
        fprintf(stderr, "av_malloc() error: Could not allocate picture buffer.\n");
        return false;
    }

    avpicture_fill((AVPicture*)outputVideoFrame, outputVideoData, outputPixelFormat, outputWidth, outputHeight);

    swsContext = sws_getContext(inputWidth, inputHeight, inputPixelFormat,
    outputWidth, outputHeight, outputPixelFormat, SWS_FAST_BILINEAR,
    NULL, NULL, NULL);
    if (swsContext == NULL) {
        fprintf(stderr, "sws_getCachedContext() error: Cannot initialize the conversion context.\n");
        return false;
    }

    return true;
}

bool FFMpegMedia::InitialOutputAudioFrame()
{
    /* alloate the encoded raw picture */
    if ((outputAudioFrame = av_frame_alloc()) == NULL) {
        fprintf(stderr, "avcodec_alloc_frame() error: Could not allocate audio frame.\n");
        return false;
    }
}

void FFMpegMedia::InitialPacket(AVPacket& packet)
{
    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;
}

void FFMpegMedia::SetAudioStreamIndex()
{
}

void FFMpegMedia::Clear()
{
    FFMpegUtil::CloseCodecCtx(inputVideoCodecCtx);
    FFMpegUtil::CloseCodecCtx(inputAudioCodecCtx);
    FFMpegUtil::CloseFmtCtx(inputFmtCtx);
    CloseOutputFile();
    FFMpegUtil::CloseCodecCtx(outputVideoCodecCtx);
    FFMpegUtil::CloseCodecCtx(outputAudioCodecCtx);
    FFMpegUtil::CloseFmtCtx(outputFmtCtx);
    FFMpegUtil::CloseSwsContext(swsContext);

    FFMpegUtil::CloseFrame(inputFrame);
    FFMpegUtil::CloseFrame(outputVideoFrame);
    FFMpegUtil::CloseFrame(outputAudioFrame);
    if(outputVideoData!=NULL){
        av_free(outputVideoData);
        outputVideoData = NULL;
    }
}
