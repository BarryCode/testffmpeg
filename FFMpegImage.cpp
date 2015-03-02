#include "StdAfx.h"
#include "FFMpegImage.h"

FFMpegImage::FFMpegImage(void)
{
}

FFMpegImage::~FFMpegImage(void)
{
}

bool FFMpegImage::InitialAndOpenOutputCodecCtx(const char* filename)
{
    if (!OpenOutputImageCodecContext(filename)) {
        exit(1);
    }

    outputWidth = outputVideoCodecCtx->width;
    outputHeight = outputVideoCodecCtx->height;
    outputPixelFormat = outputVideoCodecCtx->pix_fmt;

    return true;
}

bool FFMpegImage::OpenOutputImageCodecContext(const char* filename)
{
    bool bRet = false;
    const char* file_ext;
    if(filename != NULL){ 
        file_ext= filename;
    } else {
        file_ext = outputFilename.c_str();
    }
    AVCodecID image_codec_id = FFMpegUtil::GuessImageCodec(file_ext);
    AVCodec* image_codec = avcodec_find_encoder(image_codec_id);
    outputVideoCodecCtx = avcodec_alloc_context3(image_codec);
    if (outputVideoCodecCtx) {
        outputVideoCodecCtx->bit_rate =0;
        outputVideoCodecCtx->width = (inputWidth + 15) / 16 * 16;
        outputVideoCodecCtx->height = inputHeight;
        outputVideoCodecCtx->pix_fmt = *image_codec->pix_fmts;
        //image_codec_context->codec_id = image_codec_id;
        outputVideoCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
        outputVideoCodecCtx->time_base.num =1;
        outputVideoCodecCtx->time_base.den =25;


        if((avcodec_open2( outputVideoCodecCtx, image_codec, NULL) < 0)) {
            fprintf(stderr, "Failed to open output image codec\n");
            return false;
        }
    }
    return true;
}

bool FFMpegImage::ReadFrame()
{
    /* read frames from the file */
    if (ReadPacket()) {
        if (currentFrameType == EFrameType::EVideo) {
        /* decode video frame */
            int ret;
            if ((ret = avcodec_decode_video2(inputVideoCodecCtx, inputFrame, &gotDecodedFrame, &inputPacket)) < 0) {
                fprintf(stderr, "Error decoding video frame\n");
            }
            if(gotDecodedFrame == 0){
                fprintf(stderr, "Error decoding video frame, no frame\n");
            }
        }
        av_free_packet(&inputPacket);
    } else {
        return false;
    }

    return true;
}

bool FFMpegImage::SaveFrame()
{
    if(outputFile){
        if(fwrite(outputPacket.data, 1, outputPacket.size, outputFile)<0){
            return false;
        }
    }
    return true;
}
