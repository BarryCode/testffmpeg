#include "StdAfx.h"
//#include "FFMpegDemux.h"
//
//
//FFMpegDemux::FFMpegDemux(void):
//    inputVideo(NULL)
//{
//}
//
//FFMpegDemux::~FFMpegDemux(void)
//{
//    delete inputVideo;
//}
//
//bool FFMpegDemux::OpenInputVideo(const char* filename)
//{
//    inputVideo = new FFMpegVideo();
//    inputVideo->OpeninputFile(filename);
//}
//
//bool FFMpegDemux::OpenOutputVideo(const char* filename)
//{
//    outputVideo = new FFMpegVideo();
//    outputVideo->InitialOutputFmtCtx(filename);
//    outputVideo->CopyOutputCodecCtx(inputVideo);
//    outputVideo->OpenOutputFile(filename);
//}
//
//bool FFMpegDemux::OpenOutputAudio(const char* filename)
//{
//
//}
//
//bool FFMpegDemux::Demux()
//{
//
//}
//
//void FFMpegDemux::CloseInputVideo()
//{
//    inputVideo->CloseInputFile();
//}
//
//void FFMpegDemux::CloseoutputVideo()
//{
//    outputVideo->CloseOutputFmtCtx();
//}
//
//void FFMpegDemux::CloseOutputAudio()
//{
//}
