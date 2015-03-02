#include "StdAfx.h"
#include "FFMpegParseImage.h"


FFMpegParseImage::FFMpegParseImage(void)
{
    iFormatContext=NULL;
    iCodecContext=NULL;
    oFormatContext=NULL;
    oVideoCodecCtx=NULL;
    frame=NULL;
    dstFrame=NULL;
    convertContext=NULL;
    dstData=NULL;
}


FFMpegParseImage::~FFMpegParseImage(void)
{
    close();
}


bool FFMpegParseImage::openImageFile(const char *filename)
{
        /* register all formats and codecs */
    av_register_all();

    /* open input file, and allocate format context */
    if (avformat_open_input(&iFormatContext, filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", filename);
        return false;
    }

    /* retrieve stream information */
    if (avformat_find_stream_info(iFormatContext, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        return false;
    }
    //dump
    av_dump_format(iFormatContext, 0, filename, 0);

    videoIndex = av_find_best_stream(iFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    if (videoIndex < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(AVMEDIA_TYPE_VIDEO), filename);
        return false;
    }

    iStream = iFormatContext->streams[videoIndex];

    width = iStream->codec->width;
    height = iStream->codec->height;
    imagePixelFormat = iStream->codec->pix_fmt;

    return true;
}

bool FFMpegParseImage::initialOutputVideoFile(const char *filename)
{
    int ret;
    /* allocate the output media context */
    avformat_alloc_output_context2(&oFormatContext, NULL, NULL, filename);
    if (!oFormatContext) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        return false;
    }

    oFormat = oFormatContext->oformat;

    //dump
    av_dump_format(oFormatContext, 0, filename, 1);

    //if ((videoCodec = avcodec_find_encoder(oFormat->video_codec)) == NULL) {
    //    fprintf(stderr, "avcodec_find_encoder() error: Video codec not found.");
    //    return false;
    //}

        //查找mp4编码器
    oVideoCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(!oVideoCodec) {
        fprintf(stderr, "mp4 codec not found\n");
        return false;
    }

    /* add a video output stream */
    if ((oStream = avformat_new_stream(oFormatContext, oVideoCodec)) == NULL) {
        fprintf(stderr, "avformat_new_stream() error: Could not allocate video stream.\n");
        return false;
    }

    //if((ret =avcodec_open2(oStream->codec,videoCodec,NULL))<0) {
    //    printf("不能打开编码库");
    //    return false;
    //}

    //ret = avcodec_copy_context(oStream->codec, oVideoCodec);
    initialVideoCodec();
    
    /* open the output file, if needed */
    if (!(oFormat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&oFormatContext->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open '%s'\n", filename);
            return false;
        }
    }

    /* Write the stream header, if any. */
    ret = avformat_write_header(oFormatContext, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when write the head of output file\n");
        return false;
    }
}

bool FFMpegParseImage::decodeImageData()
{
    if(open_codec_context(&videoIndex, iFormatContext, &iCodecContext, AVMEDIA_TYPE_VIDEO)<0){
        fprintf(stderr, "Could not find codec of the input file\n");
        return false;
    }

    /* alloate the encoded raw picture */
    if ((frame = av_frame_alloc()) == NULL) {
        fprintf(stderr, "avcodec_alloc_frame() error: Could not allocate picture.\n");
        return false;
    }
        /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;
    int got_frame;
    int ret;

    /* read frames from the file */
    while (av_read_frame(iFormatContext, &packet) >= 0) {
        if (packet.stream_index == videoIndex) {
            ret = decode_packet(&got_frame,0);
            /* decode video frame */
            //packet.pts = av_rescale_q_rnd(packet.pts, iStream->time_base, oStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            //packet.dts = av_rescale_q_rnd(packet.dts, iStream->time_base, oStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            //packet.duration = av_rescale_q(packet.duration, iStream->time_base, oStream->time_base);  
            //packet.pos = -1;  
            //packet.stream_index=0;  

            //ret = av_interleaved_write_frame(oFormatContext,&packet);
            if (ret<0) {
                return false;
            }
        }
        av_free_packet(&packet);
    }
    
     /* alloate the encoded raw picture */
    if ((dstFrame = av_frame_alloc()) == NULL) {
        fprintf(stderr, "avcodec_alloc_frame() error: Could not allocate picture.");
        return false;
    }
    dstFrame->pts=0; // magic required by libx264


    int size = avpicture_get_size(AV_PIX_FMT_BGRA, width, height);
    if ((dstData = (uint8_t*)av_malloc(size)) == NULL) {
        fprintf(stderr, "av_malloc() error: Could not allocate picture buffer.");
        return false;
    }

    avpicture_fill((AVPicture*)dstFrame, dstData, AV_PIX_FMT_BGRA, width, height);

    convertContext = sws_getContext(width, height, imagePixelFormat,
    width, height, AV_PIX_FMT_BGRA, SWS_BILINEAR,
    NULL, NULL, NULL);
    if (convertContext == NULL) {
        fprintf(stderr, "sws_getCachedContext() error: Cannot initialize the conversion context.");
        return false;
    }

    ret = sws_scale(convertContext, frame->data, frame->linesize,
        0, height, dstFrame->data, dstFrame->linesize);

    
    int numBytes=avpicture_get_size(AV_PIX_FMT_BGRA, width, height);
    uint8_t *buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    bool bret = WriteJPEG(NULL, dstFrame, "F:\\testfiles\\000.bmp", AV_PIX_FMT_BGRA, buffer, numBytes);

    //        /* encode the image */
    //av_init_packet(&oPacket);
    //oPacket.data=NULL;
    //oPacket.size=0;
    //int gotPocket;
    ////picture->quality=video_c->global_quality;
    //if ((ret = avcodec_encode_video2(oVideoCodecCtx, &oPacket, dstFrame, &gotPocket)) < 0) {
    //    fprintf(stderr, "avcodec_encode_video2() error %d: Could not encode video packet.", ret);
    //    return false;
    //}
    //dstFrame->pts = dstFrame->pts + 1; // magic required by libx264

    ///* if zero size, it means the image was buffered */
    ////if (gotPocket != 0) {
    //    if (oPacket.pts != AV_NOPTS_VALUE) {
    //        oPacket.pts=av_rescale_q(oPacket.pts, oVideoCodecCtx->time_base, oStream->time_base);
    //    }
    //    if (oPacket.dts != AV_NOPTS_VALUE) {
    //        oPacket.dts=av_rescale_q(oPacket.dts, oVideoCodecCtx->time_base, oStream->time_base);
    //    }            
    //    oPacket.duration = av_rescale_q(oPacket.duration, oVideoCodecCtx->time_base, oStream->time_base);  
    //    oPacket.pos = -1;  
    //    oPacket.stream_index=oStream->index;
    ////} else {
    //    //fprintf(stderr, "avcodec_encode_video2() got_video_packet: %d.", gotPocket);
    //    //return false;
    ////}

    //if (oFormatContext!=NULL) {
    //    /* write the compressed frame in the media file */
    //    if ((ret = av_interleaved_write_frame(oFormatContext, &oPacket)) < 0) {
    //        fprintf(stderr, "av_write_frame() error %d while writing video frame.", ret);
    //        return false;
    //    }
    //}

    //ret = av_write_trailer(oFormatContext);
    //if (ret<0) {
    //    return false;
    //}

}

void FFMpegParseImage::close()
{
    /* close each codec */
    if (oVideoCodecCtx != NULL) {
        avcodec_close(oVideoCodecCtx);
        oVideoCodecCtx = NULL;
    }
    if (iCodecContext != NULL) {
        avcodec_close(iCodecContext);
        iCodecContext = NULL;
    }
    if (dstData != NULL) {
        av_free(dstData);
        dstData = NULL;
    }
    if (frame != NULL) {
        avcodec_free_frame(&frame);
        frame = NULL;
    }
    if (dstFrame != NULL) {
        avcodec_free_frame(&dstFrame);
        dstFrame = NULL;
    }
    //if (frame != null) {
    //    avcodec_free_frame(frame);
    //    frame = null;
    //}
    oStream = NULL;

    if (oFormatContext != NULL) {
        if ((oFormat->flags & AVFMT_NOFILE) == 0) {
            /* close the output file */
            avio_close(oFormatContext->pb);
        }

        /* free the streams */
        int nb_streams = oFormatContext->nb_streams;
        for(int i = 0; i < nb_streams; i++) {
            av_free(oFormatContext->streams[i]->codec);
            av_free(oFormatContext->streams[i]);
        }

        /* free the stream */
        av_free(oFormatContext);
        oFormatContext = NULL;
    }

    if (convertContext != NULL) {
        sws_freeContext(convertContext);
        convertContext = NULL;
    }
}

int FFMpegParseImage::open_codec_context(int *stream_idx,
                              AVFormatContext *fmt_ctx, AVCodecContext **codec_ctx, enum AVMediaType type)
{
    int ret;
    AVStream *st;
    AVCodec *dec = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file\n",
                av_get_media_type_string(type));
        return ret;
    } else {
        *stream_idx = ret;
        st = fmt_ctx->streams[*stream_idx];

        /* find decoder for the stream */
        *codec_ctx = st->codec;
        dec = avcodec_find_decoder((*codec_ctx)->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Init the decoders, with or without reference counting */
        if ((ret = avcodec_open2((*codec_ctx), dec, NULL)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
    }

    return 0;
}

int FFMpegParseImage::decode_packet(int *got_frame, int cached)
{
    int ret = 0;
    int decoded = packet.size;

    *got_frame = 0;

    if (packet.stream_index == videoIndex) {
        /* decode video frame */
        ret = avcodec_decode_video2(iCodecContext, frame, got_frame, &packet);
        if (ret < 0) {
            fprintf(stderr, "Error decoding video frame\n");
            return ret;
        }

        if (*got_frame) {
            //printf("video_frame%s n:%d coded_n:%d pts:%s\n",
            //       cached ? "(cached)" : "",
            //       video_frame_count++, frame->coded_picture_number,
            //       av_ts2timestr(frame->pts, &iCodecContext->time_base));

            ///* copy decoded frame to destination buffer:
            // * this is required since rawvideo expects non aligned data */
            //av_image_copy(video_dst_data, video_dst_linesize,
            //              (const uint8_t **)(frame->data), frame->linesize,
            //              iCodecContext->pix_fmt, iCodecContext->width, iCodecContext->height);

            ///* write to rawvideo file */
            //fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file);
        }
    } 
    //else if (pkt.stream_index == audio_stream_idx) {
    //    /* decode audio frame */
    //    ret = avcodec_decode_audio4(audio_dec_ctx, frame, got_frame, &pkt);
    //    if (ret < 0) {
    //        fprintf(stderr, "Error decoding audio frame (%s)\n", av_err2str(ret));
    //        return ret;
    //    }
    //    /* Some audio decoders decode only part of the packet, and have to be
    //     * called again with the remainder of the packet data.
    //     * Sample: fate-suite/lossless-audio/luckynight-partial.shn
    //     * Also, some decoders might over-read the packet. */
    //    decoded = FFMIN(ret, pkt.size);

    //    if (*got_frame) {
    //        size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample(frame->format);
    //        printf("audio_frame%s n:%d nb_samples:%d pts:%s\n",
    //               cached ? "(cached)" : "",
    //               audio_frame_count++, frame->nb_samples,
    //               av_ts2timestr(frame->pts, &audio_dec_ctx->time_base));

    //        /* Write the raw audio data samples of the first plane. This works
    //         * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
    //         * most audio decoders output planar audio, which uses a separate
    //         * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
    //         * In other words, this code will write only the first audio channel
    //         * in these cases.
    //         * You should use libswresample or libavfilter to convert the frame
    //         * to packed data. */
    //        fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);
    //    }
    //}

    /* If we use the new API with reference counting, we own the data and need
     * to de-reference it when we don't use it anymore */
    //if (*got_frame && api_mode == API_MODE_NEW_API_REF_COUNT)
    //    av_frame_unref(frame);

    return decoded;
}

bool FFMpegParseImage::initialVideoCodec()
{
    oVideoCodecCtx= oStream->codec;
    oVideoCodecCtx->codec_type=AVMEDIA_TYPE_VIDEO;
    oVideoCodecCtx->bit_rate = 3000000;// put sample parameters   

    /* resolution must be a multiple of two, but round up to 16 as often required */
    oVideoCodecCtx->width =width;//   
    oVideoCodecCtx->height = height;//   
  
    // frames per second   
    AVRational rate = av_d2q(25, 1001000);
    const AVRational* supported_framerates = oVideoCodec->supported_framerates;
    if (supported_framerates != NULL) {
        int idx = av_find_nearest_q_idx(rate, supported_framerates);
        rate = supported_framerates[idx];
    }
    oVideoCodecCtx->time_base= av_inv_q(rate);//(AVRational){1,25};

    oVideoCodecCtx->gop_size = 10; // emit one intra frame every ten frames
    //video_c->thread_count = 1;
    oVideoCodecCtx->max_b_frames=1;

    //if (videoQuality >= 0) {
    //    oVideoCodecCtx->flags=oVideoCodecCtx->flags|CODEC_FLAG_QSCALE;
    //    oVideoCodecCtx->global_quality=(int)round(FF_QP2LAMBDA * videoQuality);
    //}

    oVideoCodecCtx->pix_fmt = (AVPixelFormat)AV_PIX_FMT_YUV420P;

    // some formats want stream headers to be separate
    if ((oFormat->flags & AVFMT_GLOBALHEADER) != 0) {
        oVideoCodecCtx->flags = oVideoCodecCtx->flags | CODEC_FLAG_GLOBAL_HEADER;
    }

    if ((oVideoCodec->capabilities & CODEC_CAP_EXPERIMENTAL) != 0) {
        oVideoCodecCtx->strict_std_compliance=FF_COMPLIANCE_EXPERIMENTAL;
    }

    int ret = 0;
    if((ret =avcodec_open2(oVideoCodecCtx,oVideoCodec,NULL))<0) {
        printf("不能打开编码库");
        return false;
    }

    return true;
}


//以下内容是以前google出来的，不过这几天google被墙，等以后找到后再补上具体出处。

bool FFMpegParseImage::WriteJPEG (AVCodecContext *pCodecCtx, AVFrame *pFrame, char cFileName[], PixelFormat pix, uint8_t *buffer, int numBytes)
{
   bool bRet = false;
   AVCodec *pMJPEGCodec=NULL;
    pMJPEGCodec = avcodec_find_encoder(CODEC_ID_BMP);
   AVCodecContext *pMJPEGCtx = avcodec_alloc_context3(pMJPEGCodec);
   if( pMJPEGCtx )
   {
      pMJPEGCtx->bit_rate =0;
      pMJPEGCtx->width = width;
      pMJPEGCtx->height = height;
      pMJPEGCtx->pix_fmt = *pMJPEGCodec->pix_fmts;
      //pMJPEGCtx->codec_id = CODEC_ID_MJPEG;
      pMJPEGCtx->codec_type = AVMEDIA_TYPE_VIDEO;
      pMJPEGCtx->time_base.num =1;
      pMJPEGCtx->time_base.den =25;


      if( pMJPEGCodec && (avcodec_open2( pMJPEGCtx, pMJPEGCodec, NULL) >= 0) )
      {
         //pMJPEGCtx->qmin = pMJPEGCtx->qmax = 3;
         //pMJPEGCtx->mb_lmin = pMJPEGCtx->lmin = pMJPEGCtx->qmin * FF_QP2LAMBDA;
         //pMJPEGCtx->mb_lmax = pMJPEGCtx->lmax = pMJPEGCtx->qmax * FF_QP2LAMBDA;
         //pMJPEGCtx->flags |= CODEC_FLAG_QSCALE;
         //pFrame->quality = 10;
         //pFrame->pts = 0;

        av_init_packet(&oPacket);
        oPacket.data=NULL;
        oPacket.size=0;
        int gotPocket;
        int szBufferActual;
        //picture->quality=video_c->global_quality;
        if ((szBufferActual = avcodec_encode_video2(pMJPEGCtx, &oPacket, pFrame, &gotPocket)) < 0) {
            fprintf(stderr, "avcodec_encode_video2() error %d: Could not encode video packet.");
            return false;
        }

        if( SaveFrame(oPacket.size, oPacket.data, cFileName ) )
            bRet = true;

         avcodec_close(pMJPEGCtx);
      }
   }
   return bRet;
} 

bool FFMpegParseImage::SaveFrame(int nszBuffer, uint8_t *buffer, char cOutFileName[])
{
//printf("SaveFrame nszBuffer = %d, cOutFileName = %s\n", nszBuffer, cOutFileName);
bool bRet = false;

if( nszBuffer > 0 )
{
FILE *pFile = fopen(cOutFileName, "wb");
if(pFile)
{
fwrite(buffer, sizeof(uint8_t), nszBuffer, pFile);
bRet = true;
fclose(pFile);
}
}
   return bRet;
}
