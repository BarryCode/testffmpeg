#include "StdAfx.h"
//#include "FFMpegDemux.h"
//
//
//FFMpegDemux::FFMpegDemux(string& ifilename, string& ofilename):iFilename(ifilename),oFilename(ofilename)
//{
//    init();
//}
//
//FFMpegDemux::FFMpegDemux(const char* ifilename, const char* ofilename):iFilename(ifilename),oFilename(ofilename)
//{
//    init();
//}
//
//FFMpegDemux::~FFMpegDemux(void)
//{
//}
//
//void FFMpegDemux::init()
//{
//    iFormatContext=NULL;
//    oFormatContext=NULL;
//    oFormat=NULL;
//    frame=NULL;
//    videoIndex = -1;
//}
//
//bool FFMpegDemux::openInputFile()
//{
//    /* register all formats and codecs */
//    av_register_all();
//
//    /* open input file, and allocate format context */
//    if (avformat_open_input(&iFormatContext, iFilename.c_str(), NULL, NULL) < 0) {
//        fprintf(stderr, "Could not open source file %s\n", iFilename.c_str());
//        return false;
//    }
//
//    /* retrieve stream information */
//    if (avformat_find_stream_info(iFormatContext, NULL) < 0) {
//        fprintf(stderr, "Could not find stream information\n");
//        return false;
//    }
//    //dump
//    av_dump_format(iFormatContext, 0, iFilename.c_str(), 0);
//
//    videoIndex = av_find_best_stream(iFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
//
//    if (videoIndex < 0) {
//        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
//                av_get_media_type_string(AVMEDIA_TYPE_VIDEO), iFilename.c_str());
//        return false;
//    }
//
//    iStream = iFormatContext->streams[videoIndex];
//
//    return true;
//}
//
//bool FFMpegDemux::openOutputFile()
//{
//    int ret;
//    /* allocate the output media context */
//    avformat_alloc_output_context2(&oFormatContext, NULL, NULL, oFilename.c_str());
//    if (!oFormatContext) {
//        printf("Could not deduce output format from file extension: using MPEG.\n");
//        return false;
//    }
//
//    oFormat = oFormatContext->oformat;
//
//    //dump
//    av_dump_format(oFormatContext, 0, oFilename.c_str(), 1);
//
//    //if ((videoCodec = avcodec_find_encoder(oFormat->video_codec)) == NULL) {
//    //    fprintf(stderr, "avcodec_find_encoder() error: Video codec not found.");
//    //    return false;
//    //}
//
//    oStream = avformat_new_stream(oFormatContext, iStream->codec->codec);
//
//    if (!oStream) {
//        fprintf(stderr, "Failed allocating output stream\n");
//        return false;
//    }
//
//    //if((ret =avcodec_open2(oStream->codec,videoCodec,NULL))<0) {
//    //    printf("不能打开编码库");
//    //    return false;
//    //}
//
//    ret = avcodec_copy_context(oStream->codec, iStream->codec);
//    if (ret < 0) {
//        fprintf(stderr, "Failed to copy context from input to output stream codec context\n");
//        return false;
//    }
//    oStream->codec->codec_tag = 0;
//    if (oFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
//        oStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
//    
//    /* open the output file, if needed */
//    if (!(oFormat->flags & AVFMT_NOFILE)) {
//        ret = avio_open(&oFormatContext->pb, oFilename.c_str(), AVIO_FLAG_WRITE);
//        if (ret < 0) {
//            fprintf(stderr, "Could not open '%s'\n", oFilename.c_str());
//            return false;
//        }
//    }
//
//    /* Write the stream header, if any. */
//    ret = avformat_write_header(oFormatContext, NULL);
//    if (ret < 0) {
//        fprintf(stderr, "Error occurred when opening output file\n");
//        return false;
//    }
//}
//
//bool FFMpegDemux::demux()
//{
//    int ret;
//    frame = av_frame_alloc();
//    if (!frame) {
//        fprintf(stderr, "Could not allocate frame\n");
//        return false;
//    }
//
//    /* initialize packet, set data to NULL, let the demuxer fill it */
//    av_init_packet(&packet);
//    packet.data = NULL;
//    packet.size = 0;
//
//    /* read frames from the file */
//    while (av_read_frame(iFormatContext, &packet) >= 0) {
//        if (packet.stream_index == videoIndex) {
//
//            /* decode video frame */
//            packet.pts = av_rescale_q_rnd(packet.pts, iStream->time_base, oStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
//            packet.dts = av_rescale_q_rnd(packet.dts, iStream->time_base, oStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
//            packet.duration = av_rescale_q(packet.duration, iStream->time_base, oStream->time_base);  
//            packet.pos = -1;  
//            packet.stream_index=0;  
//
//            ret = av_interleaved_write_frame(oFormatContext,&packet);
//            if (ret<0) {
//                return false;
//            }
//        }
//        av_free_packet(&packet);
//    }
//
//    ret = av_write_trailer(oFormatContext);
//    if (ret<0) {
//        return false;
//    }
//    return true;
//}
//void FFMpegDemux::close()
//{
//    if (frame!=NULL) {
//        av_frame_free(&frame);
//    }
//
//    if (!(oFormat->flags & AVFMT_NOFILE)) {
//        /* Close the output file. */
//        avio_closep(&oFormatContext->pb);
//    }
//
//    if (oFormatContext!=NULL) {
//        avformat_free_context(oFormatContext);
//    }
//
//    if (iFormatContext!=NULL) {
//        avformat_close_input(&iFormatContext);
//    }
//}
//
////int FFMpegDemux::detachVideoFrame()
////{
////    if (packet.stream_index == videoIndex) {
////        /* decode video frame */
////        av_interleaved_write_frame(oFormatContext,&packet);
////    }
////}
