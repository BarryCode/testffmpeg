#ifndef FRAMERECORDER
#define FRAMERECORDER

#pragma once
//
//class Frame {
//public:
//    bool keyFrame;
//    //IplImage image;   // for video frame
//};


class FrameRecorder{
public:
    FrameRecorder(void);
    virtual ~FrameRecorder(void);

    int getImageWidth();
    void setImageWidth(int width);

    int getImageHeight();
    void setImageHeight(int height);

    int getPixelFormat();
    void setPixelFormat(int pixFmt);

    virtual void init() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    //virtual bool record(IplImage image) = 0;
    //virtual bool record(Buffer ... samples);
    //virtual bool record(int sampleRate, int audioChannels, Buffer ... samples);
    //virtual bool record(Image* image, int pixelFormat) = 0;

protected:
    int imageWidth, imageHeight;
    int pixelFormat, videoCodec, videoBitrate, gopSize;
    double frameRate, videoQuality;
    bool interleaved;
    //HashMap<String, String> videoOptions = new HashMap<String, String>();
    int frameNumber;
    long timestamp;
};

#endif
