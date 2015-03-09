#pragma once
#include "stdint.h"

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavutil/avutil.h"
    #include "libavutil/file.h"
    #include "libavutil/avstring.h"
    #include "libavutil/opt.h"
    #include "libavfilter/avfilter.h"
    #include "libavfilter/buffersink.h"
    #include "libavfilter/buffersrc.h"
}

typedef struct {
    enum AVCodecID id;
    const char *str;
} ImgIdStrMap;

static const ImgIdStrMap img_exts[] = {
    { AV_CODEC_ID_MJPEG,      "jpeg"     },
    { AV_CODEC_ID_MJPEG,      "jpg"      },
    { AV_CODEC_ID_MJPEG,      "jps"      },
    { AV_CODEC_ID_MJPEG,      "mpo"      },
    { AV_CODEC_ID_LJPEG,      "ljpg"     },
    { AV_CODEC_ID_JPEGLS,     "jls"      },
    { AV_CODEC_ID_PNG,        "png"      },
    { AV_CODEC_ID_PNG,        "pns"      },
    { AV_CODEC_ID_PNG,        "mng"      },
    { AV_CODEC_ID_PPM,        "ppm"      },
    { AV_CODEC_ID_PPM,        "pnm"      },
    { AV_CODEC_ID_PGM,        "pgm"      },
    { AV_CODEC_ID_PGMYUV,     "pgmyuv"   },
    { AV_CODEC_ID_PBM,        "pbm"      },
    { AV_CODEC_ID_PAM,        "pam"      },
    { AV_CODEC_ID_ALIAS_PIX,  "pix"      },
    { AV_CODEC_ID_MPEG1VIDEO, "mpg1-img" },
    { AV_CODEC_ID_MPEG2VIDEO, "mpg2-img" },
    { AV_CODEC_ID_MPEG4,      "mpg4-img" },
    { AV_CODEC_ID_FFV1,       "ffv1-img" },
    { AV_CODEC_ID_RAWVIDEO,   "y"        },
    { AV_CODEC_ID_RAWVIDEO,   "raw"      },
    { AV_CODEC_ID_BMP,        "bmp"      },
    { AV_CODEC_ID_TARGA,      "tga"      },
    { AV_CODEC_ID_TIFF,       "tiff"     },
    { AV_CODEC_ID_TIFF,       "tif"      },
    { AV_CODEC_ID_SGI,        "sgi"      },
    { AV_CODEC_ID_PTX,        "ptx"      },
    { AV_CODEC_ID_PCX,        "pcx"      },
    { AV_CODEC_ID_SUNRAST,    "sun"      },
    { AV_CODEC_ID_SUNRAST,    "ras"      },
    { AV_CODEC_ID_SUNRAST,    "rs"       },
    { AV_CODEC_ID_SUNRAST,    "im1"      },
    { AV_CODEC_ID_SUNRAST,    "im8"      },
    { AV_CODEC_ID_SUNRAST,    "im24"     },
    { AV_CODEC_ID_SUNRAST,    "im32"     },
    { AV_CODEC_ID_SUNRAST,    "sunras"   },
    { AV_CODEC_ID_JPEG2000,   "j2c"      },
    { AV_CODEC_ID_JPEG2000,   "jp2"      },
    { AV_CODEC_ID_JPEG2000,   "jpc"      },
    { AV_CODEC_ID_JPEG2000,   "j2k"      },
    { AV_CODEC_ID_DPX,        "dpx"      },
    { AV_CODEC_ID_EXR,        "exr"      },
    { AV_CODEC_ID_PICTOR,     "pic"      },
    { AV_CODEC_ID_V210X,      "yuv10"    },
    { AV_CODEC_ID_WEBP,       "webp"     },
    { AV_CODEC_ID_XBM,        "xbm"      },
    { AV_CODEC_ID_XFACE,      "xface"    },
    { AV_CODEC_ID_XWD,        "xwd"      },
    { AV_CODEC_ID_NONE,       NULL       }
};

class MediaEditor
{
public:
    static int StartDecodeVideo(const char *inFilePath, int outWidth, int outHeight, int outPix);
    static int GrabOneFrameToImage(const char *outFilePath, int64_t timestamp);
    static int GetDecWidth();
    static int GetDecHeight();

    static void Destroy();

private:
    static int OpenInputFile(const char *inFilePath);
    static int OpenInputCodec(const AVMediaType mediaType);
    static int OpenOutputImageFileAndCodecContext(const char* filename);
    static int EncodeAndWriteFrame(AVFrame* frame);
    static void CloseOutputImageFile();
    static int InitFilters(const char *filters_descr);

    static AVCodecID Str2ID(const ImgIdStrMap *tags, const char *str);
    static AVCodecID GuessImageCodec(const char *filename);

private:
    static AVFormatContext *inputFmtCtx;
    static AVCodecContext *iAudioDecCtx;
    static AVCodecContext *iVideoDecCtx;
    static AVCodecContext *oVideoDecCtx;
    static AVFilterGraph *filterGraph;
    static AVFilterContext *bufferSrcCtx;
    static AVFilterContext *bufferSinkCtx;
    static int audioStreamIndex;
    static int videoStreamIndex;
    static int iWidth;
    static int iHeight;
    static int iPixFormat;
    static int oWidth;
    static int oHeight;
    static int oPixFormat;
    
};

