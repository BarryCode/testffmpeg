#include "StdAfx.h"
#include "FFMpegAdapter.h"
#include <string>
using namespace std;

typedef unsigned char  BYTE;

typedef unsigned short WORD;

typedef unsigned long  DWORD;


void YUVRotate90(BYTE *des,BYTE *src,int width,int height) 
{ 
    int i=0,j=0,n=0; 
    int hw=width/2,hh=height/2; 
    for(j=width;j>0;j--) {
        for(i=0;i<height;i++) {
            des[n++] = src[width*i+j]; 
        }
    }

    unsigned char *ptmp = src+width*height;
    for (j=hw;j>0;j--) {
        for(i=0;i<hh;i++) {
            des[n++] = ptmp[hw*i+j];
        }      
    }

    ptmp = src+width*height*5/4;
    for(j=hw;j>0;j--) {
        for(i=0;i<hh;i++) {
            des[n++] = ptmp[hw*i+j];
        }
    }
}

void RGBRotate90(BYTE *des,BYTE *src,int width,int height) 
{
    if ((!des)||(!src)) 
    { 
        return; 
    } 
         
    int n = 0; 
    int linesize = width*3; 
    int i,j; 
    for (j=width;j>0;j--) 
        for (i=0;i<height;i++) 
        { 
        memcpy(&des[n],&src[linesize*i+j*3-3],3); 
        n+=3; 
        } 
     
    /* 
     
    if((!des)||(!src)) 
    { 
       return; 
    } 
    int n = 0; 
    int linesize = width*3; 
    int i; 
    int j; 
    // 顺时针的旋转的算法 
    for(j = 0;j < width ;j++) 
       for(i= height;i>0;i--) 
       { 
        memcpy(&des[n],&src[linesize*(i-1)+j*3-3],3); 
        n+=3; 
       } 
    */ 
}
#pragma pack(1)

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

/*
 * Video encoding example
 */
void pictures_to_video(const char *pfilename, enum AVCodecID codec_id)
{
    FILE *file;
    uint8_t *szTxt[5];
    char * file_name[5];
    size_t buffer_size[5];
    uint8_t *buffer[5];
  
    int nWidth = 0;
    int nHeight= 0;
  
    int nDataLen[5];
    int maxLen = 0;
  
    int nLen;  
  
    string sfilename;
    char * filestring = (char*)malloc(100*sizeof(char));
    for (int fileI = 0; fileI < 5; fileI ++) {
        sprintf(filestring,"f:\\testfiles\\pic%d.bmp",fileI);
        int ret = av_file_map(filestring, &buffer[fileI], &buffer_size[fileI], 0, NULL);
        if (ret < 0) {
            return;
        }
  
        //BMP bmi;//BITMAPINFO bmi;  
        //int nHeadLen = sizeof(BMP);  
        BITMAPFILEHEADER bmpFHeader;
        BITMAPINFOHEADER bmpIHeader;
        memcpy(&bmpFHeader,buffer[fileI],sizeof(BITMAPFILEHEADER));  
        sizeof(DWORD);
        sizeof(WORD);
        DWORD nHeadLen = bmpFHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
        memcpy(&bmpIHeader,buffer[fileI]+sizeof(BITMAPFILEHEADER),nHeadLen);
  sizeof(BITMAPINFOHEADER);
        nWidth = bmpIHeader.biWidth;// 464;// bmi.bmpInfo.bmiHeader.biWidth;// ;  
        nHeight = bmpIHeader.biHeight;//362;// bmi.bmpInfo.bmiHeader.biHeight;// ;  
  
        szTxt[fileI] = buffer[fileI] + bmpFHeader.bfOffBits;
        nDataLen[fileI] = buffer_size[fileI]-bmpFHeader.bfOffBits;
        if(nDataLen[fileI]>maxLen) maxLen = nDataLen[fileI];
    }

    av_register_all();  
    avcodec_register_all();  
    AVFrame *m_pRGBFrame =  new AVFrame[1];  //RGB帧数据    
    AVFrame *m_pYUVFrame = new AVFrame[1];;  //YUV帧数据  
    AVCodecContext *c= NULL;  
    AVCodecContext *in_c= NULL;  
    AVCodec *pCodecH264; //编码器  
    uint8_t * yuv_buff;//  
  
    //查找h264编码器  
    pCodecH264 = avcodec_find_encoder(CODEC_ID_H264);
    if(!pCodecH264)  
    {  
      fprintf(stderr, "h264 codec not found\n");  
      exit(1);  
    }  
  
    c= avcodec_alloc_context3(pCodecH264);
    c->bit_rate = 3000000;// put sample parameters
    c->width =nWidth;//   
    c->height = nHeight;//   
  
    // frames per second   
    AVRational rate;  
    rate.num = 1;  
    rate.den = 25;  
    c->time_base= rate;//(AVRational){1,25};  
    c->gop_size = 10; // emit one intra frame every ten frames   
    c->max_b_frames=1;  
    c->thread_count = 1;  
    c->pix_fmt = AV_PIX_FMT_YUV420P;//PIX_FMT_RGB24;  
  
    //av_opt_set(c->priv_data, /*"preset"*/"libvpx-1080p.ffpreset", /*"slow"*/NULL, 0);  
    //打开编码器  
    if(avcodec_open2(c,pCodecH264,NULL)<0)  
        printf("不能打开编码库");  
  
    int size = c->width * c->height;  
  
    //将rgb图像数据填充rgb帧  
    uint8_t * rgb_buff = new uint8_t[maxLen];  
    yuv_buff = (uint8_t *) malloc((size * 3) / 2); // size for YUV 420   
  
    //图象编码  
    int outbuf_size=100000;  
    uint8_t * outbuf= (uint8_t*)malloc(outbuf_size);   
    int u_size = 0;  
    FILE *f=NULL;   
    char * filename = "myData.h264";
    f = fopen(filename, "wb");
    if (!f) {
        printf( "could not open %s\n", filename);  
        exit(1);  
    }  
  
    //初始化SwsContext
    SwsContext * scxt = sws_getContext(c->width,c->height,PIX_FMT_BGR24,c->width,c->height,PIX_FMT_YUV420P,SWS_POINT,NULL,NULL,NULL);  
  
    AVPacket avpkt;
  
    //AVFrame *pTFrame=new AVFrame  
    for (int i=0;i<250;++i)  
    {
        //AVFrame *m_pYUVFrame = new AVFrame[1];  
    
        int index = (i / 25) % 5;  
        memcpy(rgb_buff,szTxt[index],nDataLen[index]);  
  
        avpicture_fill((AVPicture*)m_pRGBFrame, (uint8_t*)rgb_buff, PIX_FMT_RGB24, nWidth, nHeight);  
    
        //将YUV buffer 填充YUV Frame  
        avpicture_fill((AVPicture*)m_pYUVFrame, (uint8_t*)yuv_buff, PIX_FMT_YUV420P, nWidth, nHeight);  
  
        // 翻转RGB图像  
        m_pRGBFrame->data[0]  += m_pRGBFrame->linesize[0] * (nHeight - 1);  
        m_pRGBFrame->linesize[0] *= -1;                     
        m_pRGBFrame->data[1]  += m_pRGBFrame->linesize[1] * (nHeight / 2 - 1);  
        m_pRGBFrame->linesize[1] *= -1;  
        m_pRGBFrame->data[2]  += m_pRGBFrame->linesize[2] * (nHeight / 2 - 1);  
        m_pRGBFrame->linesize[2] *= -1;  


        //将RGB转化为YUV  
        sws_scale(scxt,m_pRGBFrame->data,m_pRGBFrame->linesize,0,c->height,m_pYUVFrame->data,m_pYUVFrame->linesize);  
    
        int got_packet_ptr = 0;  
        av_init_packet(&avpkt);  
        avpkt.data = outbuf;  
        avpkt.size = outbuf_size;  
        u_size = avcodec_encode_video2(c, &avpkt, m_pYUVFrame, &got_packet_ptr);  
        if (u_size == 0)  
        {
        fwrite(avpkt.data, 1, avpkt.size, f);  
        }
    }

    fclose(f);   
    delete []m_pRGBFrame;  
    delete []m_pYUVFrame;  
    delete []rgb_buff;  
    free(outbuf);  
    avcodec_close(c);  
    av_free(c);
    for( int i = 0; i<5; i++){
    av_file_unmap(buffer[i], buffer_size[i]);
    }
}

int video_to_pictures(const char *in_filename, int pic_count)
{
    AVFormatContext *ifmt_ctx = NULL;
    AVStream *stream_v = NULL;
    AVFrame *frame = NULL;
    AVFrame *dst_frame = NULL;
    AVFrame *tmp_frame = NULL;
    AVPacket pkt;
    AVCodecContext *icode_ctx = NULL;
    AVCodec *codec = NULL;
    AVPicture *dst_picture = NULL;
    struct SwsContext *img_convert_ctx;sws_scale;
    int ret, i;
    int videoindex=-1;
    int64_t stream_pos = 0;
    int64_t interval = 0;
    int64_t frame_number = 0;
    int numBytes;
    uint8_t *buffer;
    int tmp_numBytes;
    uint8_t *tmp_buffer;
    int frame_finished;
    char *out_picture_name = "picture000";//输出文件名（Output file URL

    av_register_all();
    //输入（Input）
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
        printf( "Could not open input file.");
        return -1;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        printf( "Failed to retrieve input stream information");
        return -1;
    }

    /* dump input information to stderr */
    av_dump_format(ifmt_ctx, 0, in_filename, 0);

    videoindex = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    if (videoindex < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(AVMEDIA_TYPE_VIDEO), in_filename);
        ret = -1;
        goto end;
    } else {
        stream_v = ifmt_ctx->streams[videoindex];
        if (!stream_v) {
        fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
        ret = -1;
        goto end;
        }
    }

    // Get a pointer to the codec context for the video stream
    icode_ctx = stream_v->codec;
    // Find the decoder for the video stream
    codec = avcodec_find_decoder(icode_ctx->codec_id);
    if (codec == NULL) {
        fprintf(stderr, "Codec not found/n");
        ret = -1;
        goto end; // Codec not found
    }
    // Inform the codec that we can handle truncated bitstreams -- i.e.,
    // bitstreams where frame boundaries can fall in the middle of packets
    if(codec->capabilities & CODEC_CAP_TRUNCATED)
        icode_ctx->flags|=CODEC_FLAG_TRUNCATED;
    // Open codec
    if (avcodec_open2(icode_ctx, codec, NULL)<0) {
        fprintf(stderr, "pFrameRGB==NULL/n");
        ret = -1;
        goto end;// Could not open codec
    }
    
    int rotate = 0;
    //get the rotate information
    AVDictionaryEntry *rotate_tag = av_dict_get(stream_v->metadata, "rotate", NULL, 0);
    if (rotate_tag && *rotate_tag->value && strcmp(rotate_tag->value, "0")) {
        if (!strcmp(rotate_tag->value, "90")) {
            rotate = 90;
        } else if (!strcmp(rotate_tag->value, "180")) {
            rotate = 180;
        } else if (!strcmp(rotate_tag->value, "270")) {
            rotate = 270;
        } else {
            rotate = 0;
        }

        tmp_frame = av_frame_alloc();
        if (!tmp_frame) {
            fprintf(stderr, "Could not allocate frame\n");
            ret = AVERROR(ENOMEM);
            goto end;
        }
            // Determine required buffer size and allocate buffer
        tmp_numBytes = avpicture_get_size(PIX_FMT_BGR24/*PIX_FMT_YUV420P*/, icode_ctx->height,
            icode_ctx->width);
        tmp_buffer = (uint8_t*)av_malloc(tmp_numBytes*sizeof(uint8_t));
        // Assign appropriate parts of buffer to image planes in pFrameRGB
        avpicture_fill((AVPicture *)tmp_frame, tmp_buffer, PIX_FMT_BGR24,
            icode_ctx->height, icode_ctx->width);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }
    dst_frame = av_frame_alloc();
    if (!dst_frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }
        // Determine required buffer size and allocate buffer
    numBytes = avpicture_get_size(PIX_FMT_BGR24/*PIX_FMT_YUV420P*/, icode_ctx->width,
        icode_ctx->height);
    buffer = (uint8_t*)av_malloc(numBytes*sizeof(uint8_t));
    // Assign appropriate parts of buffer to image planes in pFrameRGB
    avpicture_fill((AVPicture *)dst_frame, buffer, PIX_FMT_BGR24,
        icode_ctx->width, icode_ctx->height);

    
    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    interval = stream_v->duration/pic_count;
    //if (avpicture_alloc(dst_picture,PIX_FMT_BGR24, icode_ctx->width, icode_ctx->height) < 0) {
    //    goto end;
    //}
    /* seek the frame, then read frames from the file */
    //ret = av_seek_frame(ifmt_ctx, videoindex, stream_pos, 0);
    i=0;
    stream_pos = i*interval;
    ret = avformat_seek_file(ifmt_ctx, videoindex, stream_pos,stream_pos,stream_pos, AVSEEK_FLAG_FRAME);
    while ( /*i<pic_count &&*/ av_read_frame(ifmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        if (pkt.stream_index == videoindex) {
            if (avcodec_decode_video2(icode_ctx, frame, &frame_finished,
                                      &pkt) < 0) {
                fprintf(stderr, "Error decoding video frame\n");
                ret = -1;
                goto end;
            }

            if (frame_finished) {
                // other codes

                img_convert_ctx = sws_getContext(icode_ctx->width,
                    icode_ctx->height,
                    icode_ctx->pix_fmt,
                    icode_ctx->width,
                    icode_ctx->height,
                    PIX_FMT_BGR24,
                    SWS_BICUBIC, NULL,
                    NULL,
                    NULL);


                sws_scale(img_convert_ctx,
                    (const uint8_t*  const*)frame->data,
                    frame->linesize,
                    0,
                    icode_ctx->height,
                    dst_frame->data,
                    dst_frame->linesize);

                if(rotate != 0){
                    if(rotate == 90){
                        RGBRotate90(tmp_frame->data[0],dst_frame->data[0],icode_ctx->width,icode_ctx->height);
                    } else if(rotate == 180) {
                    } else if(rotate == 270) {
                    }
                }
                /*av_picture_copy((AVPicture *)dst_frame, (AVPicture *)frame, icode_ctx->pix_fmt, 
                icode_ctx->width, icode_ctx->height);*/
                // Save the frame to disk
                //if (i<pic_count) {
                    char pic[200];
                    sprintf(pic,"F:\\tutu\\testfiles\\picture\\image%d.bmp",i);
                    i++;
                    //printf("%d/n",*pFrameRGB);
                    if(rotate == 0){
                        av_create_bmp(pic,dst_frame->data[0],icode_ctx->width,icode_ctx->height,24);
                    } else {
                        av_create_bmp(pic,tmp_frame->data[0],icode_ctx->height,icode_ctx->width,24);
                    }
                    //stream_pos = i*interval;
                    //avformat_seek_file(ifmt_ctx, videoindex, stream_pos,stream_pos,stream_pos, AVSEEK_FLAG_FRAME);
                    //printf("/n/n%",sizeof(pFrameRGB));
               // }
            }
        }
        av_free_packet(&orig_pkt);
    }

end:
    av_frame_free(&frame);
    av_frame_free(&dst_frame);
    avcodec_close(icode_ctx);
    avformat_close_input(&ifmt_ctx);
    //avpicture_free(dst_picture);

    return ret;
}

//int pifu::ks_output_image(const char * e_filename, ks_image_t *src, int bitrate)
//{
//    char suffix[128];
//    ks_get_suffix(e_filename, suffix);
// 
//    if (strcmp(suffix, ".jpg") != 0 &&
//        strcmp(suffix, ".jpeg") != 0 &&
//        strcmp(suffix, ".png") != 0 &&
//        strcmp(suffix, ".bmp") != 0 )
//        return -1;
//    ks_av_register_all();   
//    AVFormatContext * oc = NULL;
// 
//    /* allocate the output media context */
//    avformat_alloc_output_context2(&oc, NULL, NULL, e_filename);
//    if (!oc)        
//        return -1;
// 
//    if (strcmp(suffix, ".jpg") == 0 || strcmp(suffix, ".jpeg") == 0 ){
//        oc->oformat->video_codec = CODEC_ID_MJPEG;
//    }
//    else if (strcmp(suffix, ".png") == 0){
//        oc->oformat->video_codec = CODEC_ID_PNG;
//    }
//    else if (strcmp(suffix, ".bmp") == 0){
//        oc->oformat->video_codec = CODEC_ID_BMP;
//    }
//    else{
//        av_free(oc);
//        return -1;
//    }
// 
//    AVCodecContext *c;
//    AVStream *st;
// 
//    st = av_new_stream(oc, 0);
//    if (!st) {
//        av_free(oc);
//        return -1;
//    }
// 
//    av_dump_format(oc, 0, e_filename, 1);
// 
//    c = st->codec;
//    c->codec_id = oc->oformat->video_codec;
//    c->codec_type = AVMEDIA_TYPE_VIDEO;
// 
//    /* put sample parameters */
//    if (bitrate == 0) 
//        bitrate = src->i_width*src->i_height/5;
//    c->bit_rate = bitrate; // 无用??
//    /* resolution must be a multiple of two */
//    c->width = src->i_width;
//    c->height = src->i_height;
// 
//    c->time_base.den = 25;
//    c->time_base.num = 1;
//    c->gop_size = 12; /* emit one intra frame every twelve frames at most */
// 
//    if (c->codec_id == CODEC_ID_MJPEG)
//        c->pix_fmt = PIX_FMT_YUVJ422P; 
//    else if (c->codec_id == CODEC_ID_PNG)
//        c->pix_fmt = PIX_FMT_RGB24;
//    else if (c->codec_id == CODEC_ID_BMP)
//        c->pix_fmt = PIX_FMT_BGR24;
// 
//    // some formats want stream headers to be separate
//    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
//        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
// 
//    AVCodec * codec = avcodec_find_encoder(c->codec_id);
//    if (!codec) {
//        av_free(oc);
//        return -1;
//    }
// 
//    /* open the codec */
//    if (avcodec_open(c, codec) < 0) {
//        av_free(oc);
//        return -1;
//    }
//    //avcodec_encode_video    
//     
//    AVFrame * picture = ks_alloc_picture(c->pix_fmt, c->width, c->height);
//    if (!picture) {
//        avcodec_close(c);
//        av_free(oc);
//        return -1;
//    }
// 
// 
//    if (avio_open(&oc->pb, e_filename, AVIO_FLAG_WRITE) < 0) {
//        avcodec_close(c);
//        av_free(oc);
//        return -1;           
//    }
// 
//    av_write_header(oc);
// 
//    int video_outbuf_size = c->width*c->height*3 + 1000000;
//    uint8_t *video_outbuf = (uint8_t *)av_malloc(video_outbuf_size);
// 
//    int csp_type;
//    if (c->codec_id == CODEC_ID_MJPEG)
//        csp_type = PIFU_CSP_YUVJ422P; 
//    else if (c->codec_id == CODEC_ID_PNG)
//        csp_type = PIFU_CSP_RGB;
//    else if (c->codec_id == CODEC_ID_BMP)
//        csp_type = PIFU_CSP_BGR;
// 
//    pifu::ks_image_t pic(csp_type, c->width, c->height);
//    pifu::KsSwscale sws;
//    sws.scale(src, &pic);
// 
//    if (csp_type == PIFU_CSP_YUVJ422P)
//    {
//        for (int i = 0; i < c->height; i++)
//            memcpy(picture->data[0] + i*picture->linesize[0], pic.plane[0] + i*pic.i_stride[0], c->width);
//        for (int i = 0; i < c->height; i++)
//        {
//            memcpy(picture->data[1] + i*picture->linesize[1], pic.plane[1] + i*pic.i_stride[1], c->width/2);
//            memcpy(picture->data[2] + i*picture->linesize[2], pic.plane[2] + i*pic.i_stride[2], c->width/2);
//        }
//    }
//    else
//    {
//        for (int i = 0; i < c->height; i++)
//            memcpy(picture->data[0] + i*picture->linesize[0], pic.plane[0] + i*pic.i_stride[0], c->width*3);
//    }
//
//    int out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
// 
//    /* if zero size, it means the image was buffered */
//    if (out_size > 0) {
//        AVPacket pkt;
//        av_init_packet(&pkt); 
//        pkt.pts = 0;
//        pkt.flags |= AV_PKT_FLAG_KEY;
//        pkt.stream_index= 0;
//        pkt.data= video_outbuf;
//        pkt.size= out_size;        
//        av_interleaved_write_frame(oc, &pkt);
//    } 
// 
//    av_write_trailer(oc);
//    avcodec_close(c);
//    av_free(picture->data[0]);
//    av_free(picture);
// 
//    //avformat_free_context(oc);
//    av_freep(&oc->streams[0]); 
//    avio_close(oc->pb);    
//    av_free(oc);
//    av_free(video_outbuf);
// 
//    return 0;
//     
//     
//}