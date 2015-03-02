#ifndef _FFMPEGUTIL_H_
#define _FFMPEGUTIL_H_

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavutil/avutil.h"
    #include "libavutil/file.h"
    #include "libavutil/avstring.h"
    #include "libavfilter/avfilter.h"
    #include "libavfilter/buffersink.h"
    #include "libavfilter/buffersrc.h"
}

#pragma once

typedef struct {
    enum AVCodecID id;
    const char *str;
} IdStrMap;

static const IdStrMap img_tags[] = {
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

class FFMpegUtil
{
public:
    FFMpegUtil(void);
    ~FFMpegUtil(void);

    static void RegisterForAll();
    static bool OpenInputFile(AVFormatContext* &input_fmt_ctx, const char* filename);
    static bool InitialOutputFmtCtx(AVFormatContext* &output_fmt_ctx, const char* filename);
    static bool InitialNewStream(AVFormatContext* &fmt_ctx, AVCodecID codec_id ,AVStream* &output_stream);
    static bool OpenCodecContext(AVCodecContext *codec_ctx);
    static int FindStreamIndex(AVFormatContext *fmt_ctx, enum AVMediaType type);
    static void CloseFmtCtx(AVFormatContext* &fmt_ctx);
    static void CloseCodecCtx(AVCodecContext* &codec_ctx);
    static void CloseFrame(AVFrame* &frame);
    static void CloseSwsContext(SwsContext* &context);
    static enum AVCodecID Str2ID(const IdStrMap *tags, const char *str);
    static enum AVCodecID GuessImageCodec(const char *filename);
    static int FFMpegUtil::CheckSampleFmt(AVCodec *codec, enum AVSampleFormat sample_fmt);
    static int FFMpegUtil::SelectSampleRate(AVCodec *codec);
    static int FFMpegUtil::SelectChannelLayout(AVCodec *codec);

};

#endif
