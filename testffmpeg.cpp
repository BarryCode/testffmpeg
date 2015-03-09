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
#include "FFMpegAdapter.h"


int main(int argc, char **argv)
{

    //FFMpegOperation::ConvertImages("F:\\tutu\\testfiles\\picture\\che0.jpg","F:\\tutu\\testfiles\\picture\\0temp.bmp");
    //FFMpegOperation::ConvertImages("F:\\tutu\\testfiles\\picture\\0temp.bmp","F:\\tutu\\testfiles\\picture\\0temp.png");
    //FFMpegOperation::ConvertImages("F:\\tutu\\testfiles\\picture\\0temp.bmp","F:\\tutu\\testfiles\\picture\\0temp.jpg");
    //FFMpegOperation::ConvertImages("F:\\tutu\\testfiles\\picture\\0temp.png","F:\\tutu\\testfiles\\picture\\0temp.jpg");//fail

    //video_to_pictures("F:\\tutu\\testfiles\\video\\hand.mp4", 15);

    FFMpegOperation::SplitVideoIntoImages("F:\\tutu\\testfiles\\video\\hand.mp4", 15);
    //FFMpegOperation::SplitVideoIntoImages("F:\\tutu\\testfiles\\video\\IMG_1248.MOV", 15);
    
    //const char* imagefiles[15] = {
    //    "F:\\tutu\\testfiles\\picture\\che0.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che1.jpg",
    //    "F:\\tutu\\testfiles\\picture\\image2.bmp",
    //    "F:\\tutu\\testfiles\\picture\\che3.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che4.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che5.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che6.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che7.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che8.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che9.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che10.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che11.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che12.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che13.jpg",
    //    "F:\\tutu\\testfiles\\picture\\che14.jpg"};
    //FFMpegOperation::MergeImagesIntoVideo(imagefiles,15,"F:\\tutu\\testfiles\\video\\mytestche.h264");
    //const char* imagefiles[15] = {
    //    "F:\\tutu\\testfiles\\picture\\image0.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image1.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image2.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image3.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image4.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image5.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image6.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image7.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image8.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image9.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image10.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image11.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image12.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image13.bmp",
    //    "F:\\tutu\\testfiles\\picture\\image14.bmp"};
    //FFMpegOperation::MergeImagesIntoVideo(imagefiles,15,"F:\\tutu\\testfiles\\video\\mytestimage.h264");

    //FFMpegOperation::DemuxVideoWithoutCodec("F:\\tutu\\testfiles\\video\\hand.mp4",
    //    "F:\\tutu\\testfiles\\video\\hand_without_audio.mp4",
    //    "F:\\tutu\\testfiles\\video\\hand_audio.m4a");
    //FFMpegOperation::DemuxVideoWithoutCodec("F:\\tutu\\testfiles\\video\\face.f4v",
    //    "F:\\tutu\\testfiles\\video\\face_without_audio.f4v",
    //    "F:\\tutu\\testfiles\\video\\face_audio.m4a");
    //FFMpegOperation::DemuxVideoWithoutCodec("F:\\tutu\\testfiles\\video\\IMG_1248.MOV",
    //    "F:\\tutu\\testfiles\\video\\IMG_1248_without_audio.f4v",
    //    "F:\\tutu\\testfiles\\video\\IMG_1248_audio.m4a");

    //FFMpegOperation::SplitVideo("F:\\tutu\\testfiles\\video\\face.f4v", 30000, 80000);

    //FFMpegOperation::MixAudios("F:\\tutu\\testfiles\\music\\15.m4a", 
    //    "F:\\tutu\\testfiles\\music\\16.m4a", 
    //    "F:\\tutu\\testfiles\\music\\a.aac");
    //FFMpegOperation::MixAudios("F:\\tutu\\testfiles\\music\\15.m4a", 
    //    "F:\\tutu\\testfiles\\music\\16.m4a", 
    //    "F:\\tutu\\testfiles\\music\\a.mp3");
    //FFMpegOperation::MixAudios("F:\\tutu\\testfiles\\music\\13.mp3", 
    //    "F:\\tutu\\testfiles\\music\\14.mp3", 
    //    "F:\\tutu\\testfiles\\music\\b.mp3");
    //FFMpegOperation::MixAudios("F:\\tutu\\testfiles\\music\\13.mp3", 
    //    "F:\\tutu\\testfiles\\music\\14.mp3", 
    //    "F:\\tutu\\testfiles\\music\\b.aac");

    //FFMpegOperation::MuxVideoAndAudio("F:\\tutu\\testfiles\\video\\face.f4v",
    //    "F:\\tutu\\testfiles\\music\\14.mp3",
    //    "F:\\tutu\\testfiles\\video\\face_new_audio.f4v");
    //FFMpegOperation::MuxVideoAndAudio("F:\\tutu\\testfiles\\video\\VID20150202001.mp4",
    //    "F:\\tutu\\testfiles\\music\\13.mp3",
    //    "F:\\tutu\\testfiles\\video\\VID20150202001_new_audio.mp4");
    //FFMpegOperation::MuxVideoAndAudio("F:\\tutu\\testfiles\\video\\VID20150202001.mp4",
    //    "F:\\tutu\\testfiles\\music\\15.m4a",
    //    "F:\\tutu\\testfiles\\video\\VID20150202001_new_audio.mp4");
    //FFMpegOperation::MuxVideoAndAudio("F:\\tutu\\testfiles\\video\\IMG_1248.MOV",
    //    "F:\\tutu\\testfiles\\music\\13.mp3",
    //    "F:\\tutu\\testfiles\\video\\IMG_1248_new_audio.MOV");//fail


    //FFMpegOperation::MixVideoWithNewAudio("F:\\tutu\\testfiles\\video\\hand.mp4", "F:\\tutu\\testfiles\\music\\13.mp3", "F:\\tutu\\testfiles\\video\\a.mp4");//fail

    return 0;
}
