#include "StdAfx.h"
#include "MediaEditor.h"

AVFormatContext* MediaEditor::inputFmtCtx = NULL;
AVCodecContext* MediaEditor::iAudioDecCtx = NULL;
AVCodecContext* MediaEditor::iVideoDecCtx = NULL;
AVCodecContext* MediaEditor::oVideoDecCtx = NULL;
AVFilterGraph *MediaEditor::filterGraph = NULL;
AVFilterContext *MediaEditor::bufferSrcCtx = NULL;
AVFilterContext *MediaEditor::bufferSinkCtx = NULL;
int MediaEditor::audioStreamIndex = 0;
int MediaEditor::videoStreamIndex = 0;
int MediaEditor::iWidth = 0;
int MediaEditor::iHeight = 0;
int MediaEditor::iPixFormat = -1;
int MediaEditor::oWidth = 0;
int MediaEditor::oHeight = 0;
int MediaEditor::oPixFormat = -1;

static FILE* oFile = NULL;

int MediaEditor::StartDecodeVideo(const char *inFilePath, int outWidth, int outHeight, int outPix)
{
    int ret = 0;

    av_register_all();
    avcodec_register_all();
    avfilter_register_all();

    if ((ret = OpenInputFile(inFilePath))<0)
        return ret;

    if ((ret = OpenInputCodec(AVMediaType::AVMEDIA_TYPE_VIDEO)) < 0)
        return ret;

    if(outWidth != 0 && outHeight != 0){
        oWidth = outWidth;
        oHeight = outHeight;
    } else {
        oWidth = iWidth;
        oHeight = iHeight;
    }

    return ret;
}

int MediaEditor::GrabOneFrameToImage(const char *outFilePath, int64_t timestamp)
{
    char args[50];
    int ret = 0;
    int got_frame;
    AVPacket packet;
    AVFrame* frame= av_frame_alloc();
    AVFrame* out_frame = av_frame_alloc();

    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;

    ret = OpenOutputImageFileAndCodecContext(outFilePath);
    
    //sprintf(args, "format=pix_fmts=bgra");
    sprintf(args, "crop=%d:%d", oWidth, oHeight);
    if((ret = InitFilters(args)) < 0){
        av_log(NULL, AV_LOG_ERROR, "Initial filter failed\n");
        return ret;
    }

    ret = av_seek_frame(inputFmtCtx, videoStreamIndex, timestamp, AVSEEK_FLAG_FRAME);
    /* read one video packet */
    while (av_read_frame(inputFmtCtx, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            got_frame = 0;
            ret = avcodec_decode_video2(iVideoDecCtx, frame, &got_frame, &packet);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error decoding video\n");
                break;
            }

            if (got_frame) {
                frame->pts = av_frame_get_best_effort_timestamp(frame);

                /* push the decoded frame into the filtergraph */
                if (av_buffersrc_add_frame_flags(bufferSrcCtx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
                    break;
                }

                /* pull filtered frames from the filtergraph */
                while (1) {
                    ret = av_buffersink_get_frame(bufferSinkCtx, out_frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    if (ret < 0)
                        return ret;
                    ret = EncodeAndWriteFrame(out_frame);
                    av_frame_unref(out_frame);
                }
                av_frame_unref(frame);
                break;
            }
        }
        av_free_packet(&packet);
    }
    CloseOutputImageFile();
    av_frame_free(&frame);
    av_frame_free(&out_frame);
    avfilter_graph_free(&filterGraph);

    return ret;
}

int MediaEditor::GetDecWidth()
{
    return oWidth;
}

int MediaEditor::GetDecHeight()
{
    return oHeight;
}

void MediaEditor::Destroy()
{
    avcodec_close(iAudioDecCtx);
    avcodec_close(iVideoDecCtx);
    avcodec_close(oVideoDecCtx);
    avformat_close_input(&inputFmtCtx);
}

int MediaEditor::OpenInputFile(const char *inFilePath)
{
    int ret;

    if ((ret = avformat_open_input(&inputFmtCtx, inFilePath, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(inputFmtCtx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    av_dump_format(inputFmtCtx, 0, inFilePath, 0);

    return 0;
}

int MediaEditor::OpenInputCodec(const AVMediaType mediaType)
{
    int ret;
    int stream_index;
    AVCodec *dec;
    AVCodecContext *dec_ctx;
    /* select the audio stream */
    ret = av_find_best_stream(inputFmtCtx, mediaType, -1, -1, &dec, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find a audio stream in the input file\n");
        return ret;
    }

    stream_index = ret;
    dec_ctx = inputFmtCtx->streams[stream_index]->codec;
    av_opt_set_int(dec_ctx, "refcounted_frames", 1, 0);

    if (mediaType == AVMediaType::AVMEDIA_TYPE_AUDIO) {
        audioStreamIndex = stream_index;
        iAudioDecCtx = dec_ctx;
    } else if(mediaType == AVMediaType::AVMEDIA_TYPE_VIDEO){
        videoStreamIndex = stream_index;
        iVideoDecCtx = dec_ctx;

        iWidth = iVideoDecCtx->width;
        iHeight = iVideoDecCtx->height;
        iPixFormat = iVideoDecCtx->pix_fmt;
    } else {
        av_log(NULL, AV_LOG_ERROR, "donot support other decoder\n");
        return -1;
    }

    /* init the audio decoder */
    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open audio decoder\n");
        return ret;
    }

    return 0;
}

int MediaEditor::OpenOutputImageFileAndCodecContext(const char* filename)
{
    int bRet = -1;
    AVCodecID image_codec_id = GuessImageCodec(filename);
    AVCodec* image_codec = avcodec_find_encoder(image_codec_id);
    oVideoDecCtx = avcodec_alloc_context3(image_codec);
    if (oVideoDecCtx) {
        oVideoDecCtx->bit_rate =0;
        oVideoDecCtx->width = (oWidth + 15) / 16 * 16;
        oVideoDecCtx->height = oHeight;
        oVideoDecCtx->pix_fmt = *image_codec->pix_fmts;
        //image_codec_context->codec_id = image_codec_id;
        oVideoDecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
        oVideoDecCtx->time_base.num =1;
        oVideoDecCtx->time_base.den =25;


        if((bRet = avcodec_open2( oVideoDecCtx, image_codec, NULL)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to open output image codec\n");
            return bRet;
        }
    }

    oPixFormat = oVideoDecCtx->pix_fmt;

    oFile = fopen(filename, "wb");
    if (!oFile) {
        av_log(NULL, AV_LOG_ERROR, "Failed to open output file\n");
        return -1;
    }

    return bRet;
}

int MediaEditor::EncodeAndWriteFrame(AVFrame* frame)
{
    int ret;
    int got_packet;
    AVPacket packet;
    packet.data = NULL;
    packet.size = 0;
    //picture->quality=video_c->global_quality;
    if ((ret = avcodec_encode_video2(oVideoDecCtx, &packet, frame, &got_packet)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "avcodec_encode_video2() error, Could not encode video packet.\n");
        return ret;
    }

    if(got_packet){
        if(oFile){
            if((ret = fwrite(packet.data, 1, packet.size, oFile))<0){
                return ret;
            }
        }
    }

    return ret;
}

int MediaEditor::WriteFrame(AVFrame* frame, int size)
{
    int ret = 0;

    if(oFile){
        if((ret = fwrite(frame->data, 1, size, oFile))<0){
            return ret;
        }
    }

    return ret;
}

void MediaEditor::CloseOutputImageFile()
{
    if(oFile != NULL){
        fclose(oFile);
        oFile=NULL;
    }
}

int MediaEditor::InitFilters(const char *filters_descr)
{
    char args[512];
    int ret = 0;
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVRational time_base = inputFmtCtx->streams[videoStreamIndex]->time_base;
    enum AVPixelFormat pix_fmts[] = { (AVPixelFormat)oPixFormat, AV_PIX_FMT_NONE };

    filterGraph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filterGraph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    sprintf(args,
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            iVideoDecCtx->width, iVideoDecCtx->height, iVideoDecCtx->pix_fmt,
            time_base.num, time_base.den,
            iVideoDecCtx->sample_aspect_ratio.num, iVideoDecCtx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&bufferSrcCtx, buffersrc, "in",
                                       args, NULL, filterGraph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        goto end;
    }

    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&bufferSinkCtx, buffersink, "out",
                                       NULL, NULL, filterGraph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(bufferSinkCtx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
        goto end;
    }

    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = bufferSrcCtx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = bufferSinkCtx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(filterGraph, filters_descr,
                                    &inputs, &outputs, NULL)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(filterGraph, NULL)) < 0)
        goto end;

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

AVCodecID MediaEditor::Str2ID(const ImgIdStrMap *tags, const char *str)
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

AVCodecID MediaEditor::GuessImageCodec(const char *filename)
{
    return Str2ID(img_exts, filename);
}
