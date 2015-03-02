#include <math.h>
#include "StdAfx.h"
#include "FFMpegFrameRecorder.h"

using namespace std;

const char *get_error_text(const int error)
{
    static char error_buffer[255];
    av_strerror(error, error_buffer, sizeof(error_buffer));
    return error_buffer;
}

double round(double r)
{
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

double max(double r, double s)
{
    return (r > s) ? r : s;
}

FFMpegFrameRecorder::FFMpegFrameRecorder(const char* filename)
{
    imageWidth=1280;
    imageHeight=720;
    pixelFormat=AV_PIX_FMT_YUV420P;
    videoCodec=AV_CODEC_ID_NONE;
    videoBitrate=3000000;
    frameRate=25;
    gopSize = 10;

    oFilename.clear();
    oFilename.append(filename);
}


FFMpegFrameRecorder::~FFMpegFrameRecorder(void)
{
    release();
}

void FFMpegFrameRecorder::init()
{
    av_register_all();
    avcodec_register_all();
}

void FFMpegFrameRecorder::start()
{
    initial();
    if (!openOutputFile()) {
        return;
    }

    if (!allocMemory()) {
        return;
    }

}

void FFMpegFrameRecorder::stop()
{
    if (oFormatContext != NULL) {
        /* write the trailer, if any */
        av_write_trailer(oFormatContext);

        if ((oFormat->flags & AVFMT_NOFILE) == 0) {
            /* close the output file */
            avio_close(oFormatContext->pb);
        }
    }
}

bool FFMpegFrameRecorder::record(Image* image, int pixelFormat)
{
    //if (image == NULL) {
    //    /* no more frame to compress. The codec has a latency of a few
    //        frames if using B frames, so we get the last frames by
    //        passing the same picture again */
    //    return false;
    //}

    //int ret;
    //int width = image->width;
    //int height = image->height;
    //int step = image->width;
    //uint8_t* data = image->imageData;

    //if (video_c->pix_fmt != pixelFormat || video_c->width != width || video_c->height != height) {
    //    /* convert to the codec pixel format if needed */
    //    img_convert_ctx = sws_getCachedContext(img_convert_ctx, width, height, (AVPixelFormat)AV_PIX_FMT_BGR24,
    //            video_c->width, video_c->height, video_c->pix_fmt, SWS_BILINEAR,
    //            NULL, NULL, NULL);
    //    if (img_convert_ctx == NULL) {
    //        fprintf(stderr, "sws_getCachedContext() error: Cannot initialize the conversion context.");
    //        return false;
    //    }
    //    avpicture_fill((AVPicture*)tmp_picture, data, AV_PIX_FMT_BGR24, width, height);

    //            // 翻转RGB图像  
    //tmp_picture->data[0]  += tmp_picture->linesize[0] * (height - 1);  
    //tmp_picture->linesize[0] *= -1;                     
    //tmp_picture->data[1]  += tmp_picture->linesize[1] * (height / 2 - 1);  
    //tmp_picture->linesize[1] *= -1;  
    //tmp_picture->data[2]  += tmp_picture->linesize[2] * (height / 2 - 1);  
    //tmp_picture->linesize[2] *= -1;  

    //    //tmp_picture->linesize[0]=step;
    //    ret = sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
    //                0, height, picture->data, picture->linesize);
    //}

    ///* encode the image */
    //av_init_packet(video_pkt);
    //video_pkt->data=video_outbuf;
    //video_pkt->size=video_outbuf_size;
    ////picture->quality=video_c->global_quality;
    //if ((ret = avcodec_encode_video2(video_c, video_pkt, picture, &got_video_packet)) < 0) {
    //    fprintf(stderr, "avcodec_encode_video2() error %d: Could not encode video packet.", ret);
    //    return false;
    //}
    //picture->pts = picture->pts + 1; // magic required by libx264

    ///* if zero size, it means the image was buffered */
    //if (got_video_packet != 0) {
    //    if (video_pkt->pts != AV_NOPTS_VALUE) {
    //        video_pkt->pts=av_rescale_q(video_pkt->pts, video_c->time_base, video_st->time_base);
    //    }
    //    if (video_pkt->dts != AV_NOPTS_VALUE) {
    //        video_pkt->dts=av_rescale_q(video_pkt->dts, video_c->time_base, video_st->time_base);
    //    }
    //    video_pkt->stream_index=video_st->index;
    //} else {
    //    fprintf(stderr, "avcodec_encode_video2() got_video_packet: %d.", got_video_packet);
    //    //return false;
    //}

    //if (oFormatContext!=NULL) {
    //    /* write the compressed frame in the media file */
    //    if ((ret = av_interleaved_write_frame(oFormatContext, video_pkt)) < 0) {
    //        fprintf(stderr, "av_write_frame() error %d while writing video frame.", ret);
    //        return false;
    //    }
    //}
    return true;
}

bool FFMpegFrameRecorder::record(AVFrame* frame)
{
    return true;
}

bool FFMpegFrameRecorder::readImage(const char* filename)
{
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



        /* read frames from the file */
    if (av_read_frame(iFormatContext, &packet) >= 0) {
        if (packet.stream_index == videoIndex) {
            int ret = 0;
            ret = av_interleaved_write_frame(oFormatContext,&packet);
            if (ret<0) {
                return false;
            }
        }
        av_free_packet(&packet);
    }

    return true;
}

void FFMpegFrameRecorder::initial()
{        
    picture = NULL;
    tmp_picture = NULL;
    picture_buf = NULL;
    frame = NULL;
    video_outbuf = NULL;
    oFormatContext = NULL;
    video_c = NULL;
    video_st = NULL;
    got_video_packet = 0;
    img_convert_ctx = NULL;
    iFormatContext = NULL;
}

bool FFMpegFrameRecorder::initialVideoCodec()
{
    video_c= video_st->codec;
    video_c->codec_type=AVMEDIA_TYPE_VIDEO;
    video_c->bit_rate = videoBitrate;// put sample parameters   

    /* resolution must be a multiple of two, but round up to 16 as often required */
    video_c->width =(imageWidth + 15) / 16 * 16;//   
    video_c->height = imageHeight;//   
  
    // frames per second   
    AVRational rate = av_d2q(frameRate, 1001000);
    const AVRational* supported_framerates = video_codec->supported_framerates;
    if (supported_framerates != NULL) {
        int idx = av_find_nearest_q_idx(rate, supported_framerates);
        rate = supported_framerates[idx];
    }
    video_c->time_base= av_inv_q(rate);//(AVRational){1,25};

    video_c->gop_size = gopSize; // emit one intra frame every ten frames
    //video_c->thread_count = 1;
    video_c->max_b_frames=1;

    if (videoQuality >= 0) {
        video_c->flags=video_c->flags|CODEC_FLAG_QSCALE;
        video_c->global_quality=(int)round(FF_QP2LAMBDA * videoQuality);
    }

    video_c->pix_fmt = (AVPixelFormat)pixelFormat;

    // some formats want stream headers to be separate
    if ((oFormat->flags & AVFMT_GLOBALHEADER) != 0) {
        video_c->flags = video_c->flags | CODEC_FLAG_GLOBAL_HEADER;
    }

    if ((video_codec->capabilities & CODEC_CAP_EXPERIMENTAL) != 0) {
        video_c->strict_std_compliance=FF_COMPLIANCE_EXPERIMENTAL;
    }

    int ret = 0;
    if((ret =avcodec_open2(video_c,video_codec,NULL))<0) {
        printf("不能打开编码库");
        release();
        return false;
    }

    return true;
}

bool FFMpegFrameRecorder::openOutputFile()
{
    int ret;
    /* allocate the output media context */
    avformat_alloc_output_context2(&oFormatContext, NULL, NULL, oFilename.c_str());
    if (!oFormatContext) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        return false;
    }

    oFormat = oFormatContext->oformat;

    //dump
    av_dump_format(oFormatContext, 0, oFilename.c_str(), 1);

    //if ((videoFormatContextodec = avcodec_find_encoder(oFormat->video_codec)) == NULL) {
    //    fprintf(stderr, "avcodec_find_encoder() error: Video codec not found.");
    //    return false;
    //}
    
    
    //查找mp4编码器
    video_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(!video_codec) {
        fprintf(stderr, "mp4 codec not found\n");
        release();
        return false;
    }

    /* add a video output stream */
    if ((video_st = avformat_new_stream(oFormatContext, video_codec)) == NULL) {
        fprintf(stderr, "avformat_new_stream() error: Could not allocate video stream.");
        release();
        return false;
    }

    //if((ret =avcodec_open2(oStream->codec,videoFormatContextodec,NULL))<0) {
    //    printf("不能打开编码库");
    //    return false;
    //}

    initialVideoCodec();

    /* open the output file, if needed */
    if (!(oFormat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&oFormatContext->pb, oFilename.c_str(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open '%s'\n", oFilename.c_str());
            return false;
        }
    }

    /* Write the stream header, if any. */
    ret = avformat_write_header(oFormatContext, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error oFormatContextcurred when opening output file\n");
        return false;
    }
}

bool FFMpegFrameRecorder::allocMemory()
{
    /* now that all the parameters are set, we can open the audio and
    video codecs and alloate the necessary encode buffers */
    if (video_st != NULL) {

        video_outbuf = NULL;

        /* alloate the encoded raw picture */
        if ((picture = av_frame_alloc()) == NULL) {
            fprintf(stderr, "avcodec_alloc_frame() error: Could not allocate picture.");
            release();
            return false;
        }
        picture->pts=0; // magic required by libx264

        int size = avpicture_get_size(video_c->pix_fmt, video_c->width, video_c->height);
        if ((picture_buf = (uint8_t*)av_malloc(size)) == NULL) {
            fprintf(stderr, "av_malloc() error: Could not allocate picture buffer.");
            release();
            return false;
        }
        
        avpicture_fill((AVPicture*)picture, picture_buf, video_c->pix_fmt, video_c->width, video_c->height);

        /* if the output format is not equal to the image format, then a temporary
            picture is needed too. It is then converted to the required output format */
        if ((tmp_picture = av_frame_alloc()) == NULL) {
            fprintf(stderr, "avcodec_alloc_frame() error: Could not allocate temporary picture.");
            release();
            return false;
        }

        /* initialize packet, set data to NULL, let the demuxer fill it */
        av_init_packet(&packet);
        packet.data = NULL;
        packet.size = 0;
    }
    return true;
}

void FFMpegFrameRecorder::release(){
    /* close each codec */
    if (video_c != NULL) {
        avcodec_close(video_c);
        video_c = NULL;
    }
    if (picture_buf != NULL) {
        av_free(picture_buf);
        picture_buf = NULL;
    }
    if (picture != NULL) {
        avcodec_free_frame(&picture);
        picture = NULL;
    }
    if (tmp_picture != NULL) {
        avcodec_free_frame(&tmp_picture);
        tmp_picture = NULL;
    }
    if (video_outbuf != NULL) {
        av_free(video_outbuf);
        video_outbuf = NULL;
    }
    //if (frame != null) {
    //    avcodec_free_frame(frame);
    //    frame = null;
    //}
    video_st = NULL;

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

    if (img_convert_ctx != NULL) {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = NULL;
    }
}
