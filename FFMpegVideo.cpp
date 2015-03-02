#include "StdAfx.h"
#include "FFMpegVideo.h"


FFMpegVideo::FFMpegVideo(void)
{
}


FFMpegVideo::~FFMpegVideo(void)
{
}

//bool FFMpegVideo::SplitInputVideoIntoImages(int number)
//{
//    int64_t interval = 0;
//    int64_t stream_pos = 0;
//    int ret = 0;
//    if(number>0){
//        interval = inputVideoStream->duration/number;
//        stream_pos = 0;
//        if(avformat_seek_file(inputFmtCtx, videoIndex, stream_pos,
//            stream_pos,stream_pos, AVSEEK_FLAG_FRAME)<0){
//            exit(1);
//        }
//    }
//
//    int i = 0;
//    while(ReadFrame()){
//        if (inputPacket.stream_index == videoIndex && gotDecodedFrame){
//
//            char pic[200];
//            sprintf(pic,"F:\\testfiles\\picture\\image%d.rgb",i);
//
//            OpenOutputFile(pic);
//            SaveFrame();
//            CloseOutputFile();
//            i++;
//
//            if(number>0){
//                stream_pos = i*interval;
//                if(avformat_seek_file(inputFmtCtx, videoIndex, stream_pos,
//                    stream_pos,stream_pos, AVSEEK_FLAG_FRAME)<0){
//                    exit(1);
//                }
//            }
//        }
//    }
//
//    return true;
//}

bool FFMpegVideo::InitialAndOpenOutputCodecCtx(const char* filename)
{
    AVCodec* codec;
    if(outputFmtCtx!=NULL){
        codec = avcodec_find_encoder(outputFmtCtx->oformat->video_codec);
        if(!codec) {
            fprintf(stderr, "codec not found\n");
            return false;
        }
        outputVideoCodecCtx= outputVideoStream->codec;
    } else {
        
        codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        if(!codec) {
            fprintf(stderr, "codec not found\n");
            return false;
        }
        outputVideoCodecCtx = avcodec_alloc_context3(codec);
    }

    outputVideoCodecCtx->bit_rate = 90000000;// put sample parameters
    outputVideoCodecCtx->width =inputWidth;//
    outputVideoCodecCtx->height = inputHeight;//
    outputVideoCodecCtx->codec_type=AVMEDIA_TYPE_VIDEO;
  
    // frames per second
    AVRational rate;
    rate.num = 1;
    rate.den = 25;
    outputVideoCodecCtx->time_base= rate;//(AVRational){1,25};  
    outputVideoCodecCtx->gop_size = 10; // emit one intra frame every ten frames   
    outputVideoCodecCtx->max_b_frames=1;
    outputVideoCodecCtx->thread_count = 1;
    outputVideoCodecCtx->pix_fmt = *codec->pix_fmts;//

    
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

bool FFMpegVideo::ReadFrame()
{
    /* read frames from the file */
    while (av_read_frame(inputFmtCtx, &inputPacket) >= 0) {
        if (inputPacket.stream_index == videoIndex) {
        /* decode video frame */
            if (avcodec_decode_video2(inputVideoCodecCtx, inputFrame, &gotDecodedFrame, &inputPacket) < 0) {
                fprintf(stderr, "Error decoding video frame\n");
            }
            if(gotDecodedFrame == 0){
                fprintf(stderr, "Error decoding video frame, no frame\n");
            }
        }
        

        av_free_packet(&inputPacket);
    }

    return true;
}
