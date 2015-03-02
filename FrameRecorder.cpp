#include "StdAfx.h"
#include "FrameRecorder.h"


FrameRecorder::FrameRecorder(void)
{
}


FrameRecorder::~FrameRecorder(void)
{
}

int FrameRecorder::getImageWidth()
{
    return imageWidth;
}
void FrameRecorder::setImageWidth(int width) {
    imageWidth = width;
}

int FrameRecorder::getImageHeight() {
    return imageHeight;
}
void FrameRecorder::setImageHeight(int height) {
    imageHeight = height;
}

int FrameRecorder::getPixelFormat(){
    return pixelFormat;
}

void FrameRecorder::setPixelFormat(int pixFmt){
    pixelFormat = pixFmt;
}

//bool FrameRecorder::record(Buffer ... samples);{
//    return record(0, 0, samples);
//}
//boolean FrameRecorder::record(int sampleRate, int audioChannels, Buffer ... samples){
//    throw new UnsupportedOperationException("This FrameRecorder does not support audio.");
//}
//void FrameRecorder::record(Frame frame) throws Exception {
//    if (frame == null || (frame.image == null && frame.samples == null)) {
//        record((IplImage)null);
//    } else {
//        if (frame.image != null) {
//            frame.keyFrame = record(frame.image);
//        }
//        if (frame.samples != null) {
//            frame.keyFrame = record(frame.sampleRate, frame.audioChannels, frame.samples);
//        }
//    }
//}