
#pragma once

extern "C"
{
	#include <libavutil/opt.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/channel_layout.h>
	#include <libavutil/common.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/samplefmt.h>
	#include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavutil/file.h"
    #include "libavutil/error.h"
}

void pictures_to_video(const char *filename, enum AVCodecID codec_id);

int video_to_pictures(const char *filename, int pic_count);

class FFMpegAdapter{};
