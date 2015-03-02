// testffmpeg.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
/*
 * Copyright (c) 2012 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * libswscale API use example.
 * @example scaling_video.c
 */

/*
 * Copyright (c) 2014 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * libavformat AVIOContext API example.
 *
 * Make libavformat demuxer access media content through a custom
 * AVIOContext read callback.
 * @example avio_reading.c
 */

#include "FFMpegFrameRecorder.h"
#include "BitMapParse.h"
#include "FFMpegDemux.h"
#include "FFMpegParseImage.h"
#include "FFMpegImage.h"
#include "FFMpegVideo.h"
#include "FFMpegOperation.h"

int main(int argc, char **argv)
{
    //char * filename = new char(100);
    //Image image;
    //FFMpegFrameRecorder* recorder = new FFMpegFrameRecorder("myTest.H264");

    //recorder->init();
    //recorder->start();
    ////BitMapParse* bmp = new BitMapParse();
    ////for(int i=0; i<5; i++){
    //    //sprintf(filename,"F:\\testfiles\\test4.bmp");
    //    //bmp->Parse(filename, &image);

    //    recorder->readImage("F:\\testfiles\\test4.bmp");
    ////}
    //recorder->stop();
    //
    //////delete bmp;
    //delete recorder;
    ////delete filename;
    //video_to_pictures("F:\\测试视频\\追踪对象_脚.mp4", 10);
    //pictures_to_video("myTest.h264", AV_CODEC_ID_H264);

    //FFMpegDemux* demux = new FFMpegDemux("F:\\testfiles\\test4.bmp","F:\\testfiles\\result.MOV");
    //demux->init();
    //demux->openInputFile();
    //demux->openOutputFile();
    //demux->demux();
    //demux->close();

    //FFMpegParseImage* parse = new FFMpegParseImage();
    //parse->openImageFile("F:\\testfiles\\picture\\pic_png\\2_0.png");
    ////parse->initialOutputVideoFile("F:\\testfiles\\mytest.h264");
    //parse->decodeImageData();
    //parse->close();
    //delete parse;

    //FFMpegImage* image = new FFMpegImage();
    //image->LoadInputFile("F:\\testfiles\\picture\\2_0.png");
    //image->ReadFrame();
    //image->LoadOutputFile("F:\\testfiles\\picture\\0temp.png");
    //image->EncodeFrame();
    //image->WriteFrame(true);
    //delete image;
        
    
    //FFMpegVideo* video = new FFMpegVideo();
    //video->LoadInputFile("F:\\测试视频\\追踪对象_表演者脑袋.mp4");
    //video->InitialOutputContext(".bmp");
    //video->SplitInputVideoIntoImages(10);
    //delete video;

    //FFMpegOperation::ConvertImages("F:\\testfiles\\picture\\che0.jpg","F:\\testfiles\\picture\\0temp.bmp");
    //FFMpegOperation::ConvertImages("F:\\testfiles\\picture\\0temp.bmp","F:\\testfiles\\picture\\0temp.png");
    //FFMpegOperation::ConvertImages("F:\\testfiles\\picture\\0temp.bmp","F:\\testfiles\\picture\\0temp.jpg");
    //FFMpegOperation::ConvertImages("F:\\testfiles\\picture\\0temp.png","F:\\testfiles\\picture\\0temp.jpg");//fail
    //FFMpegOperation::SplitVideoIntoImages("F:\\testfiles\\video\\hand.mp4", 15);
    //FFMpegOperation::SplitVideoIntoImages("F:\\testfiles\\video\\hand.mp4", 15);
    
    //const char* imagefiles[15] = {
    //    "F:\\testfiles\\picture\\che0.jpg",
    //    "F:\\testfiles\\picture\\che1.jpg",
    //    "F:\\testfiles\\picture\\che2.jpg",
    //    "F:\\testfiles\\picture\\che3.jpg",
    //    "F:\\testfiles\\picture\\che4.jpg",
    //    "F:\\testfiles\\picture\\che5.jpg",
    //    "F:\\testfiles\\picture\\che6.jpg",
    //    "F:\\testfiles\\picture\\che7.jpg",
    //    "F:\\testfiles\\picture\\che8.jpg",
    //    "F:\\testfiles\\picture\\che9.jpg",
    //    "F:\\testfiles\\picture\\che10.jpg",
    //    "F:\\testfiles\\picture\\che11.jpg",
    //    "F:\\testfiles\\picture\\che12.jpg",
    //    "F:\\testfiles\\picture\\che13.jpg",
    //    "F:\\testfiles\\picture\\che14.jpg"};
    //FFMpegOperation::MergeImagesIntoVideo(imagefiles,15,"F:\\testfiles\\video\\mytestche.h264");
    //const char* imagefiles[15] = {
    //    "F:\\testfiles\\picture\\image0.bmp",
    //    "F:\\testfiles\\picture\\image1.bmp",
    //    "F:\\testfiles\\picture\\image2.bmp",
    //    "F:\\testfiles\\picture\\image3.bmp",
    //    "F:\\testfiles\\picture\\image4.bmp",
    //    "F:\\testfiles\\picture\\image5.bmp",
    //    "F:\\testfiles\\picture\\image6.bmp",
    //    "F:\\testfiles\\picture\\image7.bmp",
    //    "F:\\testfiles\\picture\\image8.bmp",
    //    "F:\\testfiles\\picture\\image9.bmp",
    //    "F:\\testfiles\\picture\\image10.bmp",
    //    "F:\\testfiles\\picture\\image11.bmp",
    //    "F:\\testfiles\\picture\\image12.bmp",
    //    "F:\\testfiles\\picture\\image13.bmp",
    //    "F:\\testfiles\\picture\\image14.bmp"};
    //FFMpegOperation::MergeImagesIntoVideo(imagefiles,15,"F:\\testfiles\\video\\mytestimage.h264");

    //FFMpegOperation::DemuxVideoWithoutCodec("F:\\testfiles\\video\\hand.mp4");
    //FFMpegOperation::DemuxVideoWithoutCodec("F:\\testfiles\\video\\face.f4v");
    //FFMpegOperation::DemuxVideoWithoutCodec("F:\\testfiles\\video\\IMG_1248.MOV");

    //FFMpegOperation::SplitVideo("F:\\testfiles\\video\\face.f4v", 30000, 80000);

    FFMpegOperation::MixAudios("F:\\testfiles\\music\\12.mp3", "F:\\testfiles\\music\\13.mp3", "F:\\testfiles\\music\\a.aac");




    //FFMpegOperation::ConvertAudio("F:\\testfiles\\video\\1.mp3", "F:\\testfiles\\video\\1.aac");
    return 0;
}
