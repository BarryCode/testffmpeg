#include "StdAfx.h"
#include "BitMapParse.h"
#include "stdlib.h"


BitMapParse::BitMapParse(void)
{
    buffer = NULL;
    buffer_size = 0;
}


BitMapParse::~BitMapParse(void)
{
    if(buffer != NULL){
        av_file_unmap(buffer,buffer_size);
        buffer = NULL;
        buffer_size = 0;
    }
}

bool BitMapParse::Parse(const char* filename, Image* image)
{
    if(buffer != NULL){
        av_file_unmap(buffer,buffer_size);
        buffer = NULL;
        buffer_size = 0;
    }

    int ret = av_file_map(filename, &buffer, &buffer_size, 0, NULL);
    if (ret < 0) {
        return false;
    }

    memcpy(&image->width,buffer+18,4);
    memcpy(&image->widthStep,buffer+18,4);
    memcpy(&image->height,buffer+22,4);
    
    image->imageData = buffer+54;

    return true;
}

//static int av_create_bmp(char* filename,uint8_t *pRGBBuffer,int width,int height,int bpp)
//{
//    BitmapFileHeader bmpheader;
//    BitmapInfo bmpinfo;
//    FILE *fp;
//
//    fp = fopen(filename,"wb");
//    if(!fp)return -1;
//
//    bmpheader.bfType = ('M'<<8)|'B';
//    bmpheader.bfReserved1 = 0;
//    bmpheader.bfReserved2 = 0;
//    bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
//    bmpheader.bfSize = bmpheader.bfOffBits + width*height*bpp/8;
//        
//    bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//    bmpinfo.bmiHeader.biWidth = width;
//    bmpinfo.bmiHeader.biHeight = height;
//    bmpinfo.bmiHeader.biPlanes = 1;
//    bmpinfo.bmiHeader.biBitCount = bpp;
//    bmpinfo.bmiHeader.biCompression = 0;
//    bmpinfo.bmiHeader.biSizeImage = 0;
//    bmpinfo.bmiHeader.biXPelsPerMeter = 100;
//    bmpinfo.bmiHeader.biYPelsPerMeter = 100;
//    bmpinfo.bmiHeader.biClrUsed = 0;
//    bmpinfo.bmiHeader.biClrImportant = 0;
//
//    fwrite(&bmpheader,sizeof(BITMAPFILEHEADER),1,fp);
//    fwrite(&bmpinfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
//    fwrite(pRGBBuffer,width*height*bpp/8,1,fp);
//    fclose(fp);
//
//    return 0;
//}
