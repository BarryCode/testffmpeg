#ifndef _FFMPEGOPERATION_H_
#define _FFMPEGOPERATION_H_
#include "stdint.h"

#pragma once
class FFMpegOperation
{
public:
    FFMpegOperation(void);
    ~FFMpegOperation(void);

    static void ConvertImages(const char* src_file, const char* dst_file);
    static void SplitVideoIntoImages(const char* src_file, int number);
    static void MergeImagesIntoVideo(const char* image_file[], int count, const char* dst_file);
    static void DemuxVideoWithoutCodec(const char* src_file, 
        const char* dst_video_file, const char* dst_audio_file);
    static void SplitVideo(const char* src_file, int64_t start_ts, int64_t end_ts);
    static void MixAudios(const char* src_file1, const char* src_file2, const char* dst_file);
    static void MuxVideoAndAudio();

    static void MixVideoWithNewAudio(const char* src_video_file, const char* src_audio_file, const char* dst_file);
    //static void MuxVideoWithoutCodec(const char* src_file_v,const char* src_file_a);
    //
    //static void ConvertAudio(const char* src_file,const char* dst_file);

};

#endif
