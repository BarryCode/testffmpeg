#include "StdAfx.h"
#include "FFMpegOperation.h"
#include "FFMpegImage.h"
#include "FFMpegVideo.h"
#include "FFMpegAudio.h"
#include "FFMpegMixAudio.h"

static AVFilterGraph *filter_graph;

FFMpegOperation::FFMpegOperation(void)
{
}


FFMpegOperation::~FFMpegOperation(void)
{
}

void FFMpegOperation::ConvertImages(const char* src_file, const char* dst_file)
{
    FFMpegMedia* image = new FFMpegImage();
    image->LoadInputFileInfo(src_file);
    image->OpenInputFileCodecCtx();

    image->OpenOutputFile(dst_file);
    image->InitialAndOpenOutputCodecCtx();
    image->AllocOutputFrame();

    image->ReadFrame();
    image->EncodeFrame();
    image->SaveFrame();
    delete image;
}

void FFMpegOperation::SplitVideoIntoImages(const char* src_file, int number)
{
    char imagename[200];
    int i = 0;
    FFMpegMedia* video = new FFMpegVideo();
    video->LoadInputFileInfo(src_file);
    video->OpenInputFileCodecCtx();

    FFMpegMedia* image = new FFMpegImage();
    image->SetInputWidth(video->GetInputWidth());
    image->SetInputHeight(video->GetInputHeight());
    image->SetInputPixFmt(video->GetInputPixFmt());
    image->InitialAndOpenOutputCodecCtx(".bmp");
    image->AllocOutputFrame();

    while(video->ReadPacket()){
        if(video->GetCurFrameType() == FFMpegMedia::EVideo){
            if(video->DecodeCurPacket()){
                image->SetInputFrame(video->GetInputFrame());
                image->SetCurFrameType(FFMpegMedia::EVideo);
                if(i<number){
                    sprintf(imagename,"F:\\testfiles\\picture\\image%d.bmp",i);
                    image->OpenOutputFile(imagename);
                    image->EncodeFrame();
                    image->SaveFrame();
                    image->CloseOutputFile();
                    i++;
                }
                
                image->SetInputFrame(NULL);
            }
            
        }
    }
    delete image;
    delete video;

}

void FFMpegOperation::MergeImagesIntoVideo(const char* image_file[], int count, const char* dst_file)
{
    FFMpegMedia* video = new FFMpegVideo();
    //video->InitialOutputFmtCtx(dst_file);
    FFMpegImage* image;
    image = new FFMpegImage();
    image->LoadInputFileInfo(image_file[0]);
    video->SetInputWidth(image->GetInputWidth());
    video->SetInputHeight(image->GetInputHeight());
    video->SetInputPixFmt(image->GetInputPixFmt());
    delete image;
    video->InitialAndOpenOutputCodecCtx();
    video->AllocOutputFrame();
    video->OpenOutputFile(dst_file);
    int i = 0;
    int index = 0;
    for(i = 0; i< count*25; i++){
        index = (i/25)%count;
        image = new FFMpegImage();
        image->LoadInputFileInfo(image_file[index]);
        image->OpenInputFileCodecCtx();
        image->ReadFrame();

        video->SetInputFrame(image->GetInputFrame());
        video->SetCurFrameType(image->GetCurFrameType());
        video->EncodeFrame();
        video->SaveFrame();
        video->SetInputFrame(NULL);
        delete image;
    }

    video->CloseOutputFile();
    delete video;
}

void FFMpegOperation::DemuxVideoWithoutCodec(const char* src_file,
        const char* dst_video_file, const char* dst_audio_file)
{
    FFMpegMedia* input_video = new FFMpegVideo();
    input_video->LoadInputFileInfo(src_file);
    //input_video->OpenInputFileCodecCtx();
    input_video->InitialOutputFmtCtx(dst_video_file);
    input_video->CopyAndOpenOutputVideoCodecCtx(input_video->GetInputVideoCodecCtx());
    input_video->OpenOutputFile();

    FFMpegMedia* audio = new FFMpegAudio();
    audio->InitialOutputFmtCtx(dst_audio_file);
    audio->CopyAndOpenOutputAudioCodecCtx(input_video->GetInputAudioCodecCtx());
    audio->OpenOutputFile();
    audio->SetInputAudioStream(input_video->GetInputAudioStream());
    while(input_video->ReadPacket()){
        if(input_video->GetCurFrameType() == FFMpegMedia::EVideo){
            input_video->SetOutputPacket(input_video->GetInputPacket());
            input_video->SaveFrame();
        } else if(input_video->GetCurFrameType() == FFMpegMedia::EAudio){
            audio->SetOutputPacket(input_video->GetInputPacket());
            audio->SaveFrame();
        }
    }
    delete audio;
    delete input_video;
}

void FFMpegOperation::SplitVideo(const char* src_file, int64_t start_ts, int64_t end_ts)
{
    FFMpegMedia* video = new FFMpegVideo();
    video->LoadInputFileInfo(src_file);

    video->InitialOutputFmtCtx("F:\\testfiles\\video\\mytest.f4v");
    video->InitialNewAudioStream();
    
    video->CopyAndOpenOutputVideoCodecCtx(video->GetInputVideoCodecCtx());
    video->CopyAndOpenOutputAudioCodecCtx(video->GetInputAudioCodecCtx());
    video->OpenOutputFile();

    video->SeekFrame(start_ts);
    AVPacket packet;

    while(video->ReadPacket()){
        packet = video->GetInputPacket();
        if(packet.pts > end_ts) 
            break;
        video->SetOutputPacket(packet);
        video->SaveFrame();
    }

    delete video;

}

void FFMpegOperation::MixAudios(const char* src_file1, const char* src_file2, const char* dst_file)
{
    FFMpegMixAudio* mix = new FFMpegMixAudio();
    mix->mix_audios(src_file1, src_file2, dst_file);
}

void FFMpegOperation::MixVideoWithNewAudio(const char* src_video_file,
    const char* src_audio_file, const char* dst_file)
{

}

//void FFMpegOperation::MuxVideoWithoutCodec(const char* src_file_v,const char* src_file_a)
//{
//    FFMpegMedia* video = new FFMpegVideo();
//    video->LoadInputFileInfo(src_file_v);
//
//    FFMpegMedia* input_audio = new FFMpegAudio();
//    input_audio->LoadInputFileInfo(src_file_a);
//
//    video->InitialOutputFmtCtx("F:\\testfiles\\video\\mytest.f4v");
//    video->InitialNewAudioStream();
//    
//    video->CopyAndOpenOutputVideoCodecCtx(video->GetInputVideoCodecCtx());
//    video->CopyAndOpenOutputAudioCodecCtx(input_audio->GetInputAudioCodecCtx());
//    video->OpenOutputFile();
//    video->SetInputAudioStream(input_audio->GetInputAudioStream());
//    //av_compare_ts
//}
//
//void FFMpegOperation::ConvertAudio(const char* src_file,const char* dst_file)
//{
//    FFMpegMedia* audio = new FFMpegAudio();
//    audio->LoadInputFileInfo(src_file);
//    audio->OpenInputFileCodecCtx();
//    
//    audio->OpenOutputFile(dst_file);
//    audio->InitialAndOpenOutputCodecCtx();
//    audio->AllocOutputFrame();
//
//    audio->ReadFrame();
//    audio->EncodeFrame();
//    audio->SaveFrame();
//    delete audio;
//    
//}
