#include "StdAfx.h"
#include "FFMpegOperation.h"
#include "FFMpegImage.h"
#include "FFMpegVideo.h"
#include "FFMpegAudio.h"
#include "FFMpegMixAudio.h"
#include "MediaEditor.h"

static AVFilterGraph *filter_graph;

typedef unsigned char  BYTE;

typedef unsigned short WORD;

typedef unsigned long  DWORD;

typedef struct tagBITMAPFILEHEADER{
     WORD bfType;                // the flag of bmp, value is "BM"
     DWORD    bfSize;                // size BMP file ,unit is bytes
     WORD    bfReserved1,bfReserved2;            // 0
     DWORD    bfOffBits;             // must be 54
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{

     DWORD    biSize;                // must be 0x28
     DWORD    biWidth;           //
     DWORD    biHeight;          //
     WORD biPlanes;          // must be 1
     WORD biBitCount;            //
     DWORD    biCompression;         //
     DWORD    biSizeImage;       //
     DWORD    biXPelsPerMeter;   //
     DWORD    biYPelsPerMeter;   //
     DWORD    biClrUsed;             //
     DWORD    biClrImportant;        //
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD{
     BYTE    rgbBlue;
     BYTE rgbGreen;
     BYTE rgbRed;
     BYTE rgbReserved;
}RGBQUAD;

typedef   struct   tagBITMAPINFO   {  
          BITMAPINFOHEADER         bmiHeader;  
          RGBQUAD                           bmiColors[1];  
  }   BITMAPINFO;  


static int av_create_bmp(char* filename,uint8_t *pRGBBuffer,int width,int height,int bpp)
{
    BITMAPFILEHEADER bmpheader;
    BITMAPINFO bmpinfo;
    FILE *fp;

    fp = fopen(filename,"wb");
    if(!fp)return -1;

    bmpheader.bfType = ('M'<<8)|'B';
    bmpheader.bfReserved1 = 0;
    bmpheader.bfReserved2 = 0;
    bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmpheader.bfSize = bmpheader.bfOffBits + width*height*bpp/8;
        
    bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpinfo.bmiHeader.biWidth = width;
    bmpinfo.bmiHeader.biHeight = height;
    bmpinfo.bmiHeader.biPlanes = 1;
    bmpinfo.bmiHeader.biBitCount = bpp;
    bmpinfo.bmiHeader.biCompression = 0;
    bmpinfo.bmiHeader.biSizeImage = 0;
    bmpinfo.bmiHeader.biXPelsPerMeter = 100;
    bmpinfo.bmiHeader.biYPelsPerMeter = 100;
    bmpinfo.bmiHeader.biClrUsed = 0;
    bmpinfo.bmiHeader.biClrImportant = 0;

    fwrite(&bmpheader,sizeof(BITMAPFILEHEADER),1,fp);
    fwrite(&bmpinfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
    fwrite(pRGBBuffer,width*height*bpp/8,1,fp);
    fclose(fp);

    return 0;
}


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
    //char imagename[200];
    //int i = 0;
    //FFMpegMedia* video = new FFMpegVideo();
    //video->LoadInputFileInfo(src_file);
    //video->OpenInputFileCodecCtx();

    //FFMpegMedia* image = new FFMpegImage();
    //image->SetInputWidth(video->GetInputWidth());
    //image->SetInputHeight(video->GetInputHeight());
    //image->SetInputPixFmt(video->GetInputPixFmt());
    //image->InitialAndOpenOutputCodecCtx(".bmp");
    //image->AllocOutputFrame();

    //while(video->ReadPacket()){
    //    if(video->GetCurFrameType() == FFMpegMedia::EVideo){
    //        if(video->DecodeCurPacket()){
    //            image->SetInputFrame(video->GetInputFrame());
    //            image->SetCurFrameType(FFMpegMedia::EVideo);
    //            if(i<number){
    //                sprintf(imagename,"F:\\tutu\\testfiles\\picture\\image%d.bmp",i);
    //                image->OpenOutputFile(imagename);
    //                image->EncodeFrame();
    //                image->SaveFrame();
    //                image->CloseOutputFile();
    //                i++;
    //            }
    //            
    //            image->SetInputFrame(NULL);
    //        }
    //        
    //    }
    //}
    //delete image;
    //delete video;
    uint8_t *buffer = NULL;
    char imagename[200];;
    MediaEditor::StartDecodeVideo(src_file, 360, 480, 0);
    for(int i=0;i<number;i++){
        sprintf(imagename,"F:\\tutu\\testfiles\\picture\\image%d.jpg",i);
        MediaEditor::GrabOneFrameToImage(imagename, 5000);
        //av_create_bmp(imagename,buffer,MediaEditor::GetDecWidth(),MediaEditor::GetDecHeight(),24);
    }
    MediaEditor::Destroy();

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

    video->InitialOutputFmtCtx("F:\\tutu\\testfiles\\video\\mytest.f4v");
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
    delete mix;
}

void FFMpegOperation::MuxVideoAndAudio(const char* src_video_file,
    const char* src_audio_file, const char* dst_file)
{
    FFMpegMixAudio* mux = new FFMpegMixAudio();
    mux->mux(src_video_file, src_audio_file, dst_file);
    delete mux;
}

void FFMpegOperation::MixVideoWithNewAudio(const char* src_video_file,
    const char* src_audio_file, const char* dst_file)
{
    const char* temp_v = "F:\\tutu\\testfiles\\video\\hand_without_audio.mp4";
    const char* temp_a = "F:\\tutu\\testfiles\\video\\hand_temp.aac";
    const char* temp_mix_a = "F:\\tutu\\testfiles\\video\\hand_temp_mix.aac";
    DemuxVideoWithoutCodec(src_video_file,temp_v,temp_a);
    MixAudios(src_audio_file,temp_a,temp_mix_a);
    MuxVideoAndAudio(temp_v,temp_mix_a,dst_file);

    //int cur_pts_v=0;
    //int cur_pts_a=0;
    //int frame_index=0;
    //AVPacket pkt;
    //AVStream* video_stream;
    //AVStream* audio_stream;

    //FFMpegMedia* input_video = new FFMpegVideo();
    //input_video->LoadInputFileInfo(src_video_file);
    //FFMpegMedia* audio = new FFMpegAudio();
    //audio->LoadInputFileInfo(src_audio_file);

    //input_video->InitialOutputFmtCtx(dst_file);
    //input_video->CopyAndOpenOutputVideoCodecCtx(input_video->GetInputVideoCodecCtx());
    //input_video->CopyAndOpenOutputAudioCodecCtx(audio->GetInputAudioCodecCtx());
    //input_video->OpenOutputFile();
    //video_stream = input_video->GetInputVideoStream();
    //audio_stream = audio->GetInputAudioStream();
    //input_video->SetInputAudioStream(audio_stream);
    //while (1) {
    //    AVFormatContext *ifmt_ctx;  
    //    int stream_index=0;  
    //    AVStream *in_stream, *out_stream;
  
    //    //Get an AVPacket  
    //    if(av_compare_ts(cur_pts_v,video_stream->time_base,cur_pts_a,audio_stream->time_base) <= 0){
  
    //        if(input_video->ReadPacket()){
    //            pkt = input_video->GetInputPacket();
    //            do{  
    //                in_stream  = video_stream;
    //                out_stream = video_stream;
  
    //                if(input_video->GetCurFrameType()==FFMpegMedia::EVideo){
    //                    //FIX£ºNo PTS (Example: Raw H.264)  
    //                    //Simple Write PTS  
    //                    if(pkt.pts==AV_NOPTS_VALUE){  
    //                        //Write PTS  
    //                        AVRational time_base1=in_stream->time_base;  
    //                        //Duration between 2 frames (us)  
    //                        int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(video_stream->r_frame_rate);  
    //                        //Parameters  
    //                        pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);  
    //                        pkt.dts=pkt.pts;  
    //                        pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);  
    //                        frame_index++;  
    //                    }  
  
    //                    cur_pts_v=pkt.pts;
    //                    break;  
    //                }  
    //            }while(audio->ReadPacket());
    //        }else{  
    //            break;  
    //        }  
    //    }else{  
    //        if(audio->ReadPacket()){  
    //            pkt = audio->GetInputPacket();
    //            do{  
    //                in_stream  = audio_stream;  
    //                out_stream = audio_stream;  
  
    //                if(input_video->GetCurFrameType()==FFMpegMedia::EAudio){  
  
    //                    //FIX£ºNo PTS  
    //                    //Simple Write PTS  
    //                    if(pkt.pts==AV_NOPTS_VALUE){  
    //                        //Write PTS  
    //                        AVRational time_base1=in_stream->time_base;  
    //                        //Duration between 2 frames (us)  
    //                        int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);  
    //                        //Parameters  
    //                        pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);  
    //                        pkt.dts=pkt.pts;  
    //                        pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);  
    //                        frame_index++;  
    //                    }  
    //                    cur_pts_a=pkt.pts;  
  
    //                    break;  
    //                }  
    //            }while(audio->ReadPacket());  
    //        }else{  
    //            break;  
    //        }  
  
    //    }
  
    //    input_video->SetOutputPacket(pkt);
    //    input_video->SaveFrame();
  
    //}

    //input_video->CloseOutputFile();

    //delete input_video;
    //delete audio;
}



//void FFMpegOperation::MuxVideoWithoutCodec(const char* src_file_v,const char* src_file_a)
//{
//    FFMpegMedia* video = new FFMpegVideo();
//    video->LoadInputFileInfo(src_file_v);
//
//    FFMpegMedia* input_audio = new FFMpegAudio();
//    input_audio->LoadInputFileInfo(src_file_a);
//
//    video->InitialOutputFmtCtx("F:\\tutu\\testfiles\\video\\mytest.f4v");
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
