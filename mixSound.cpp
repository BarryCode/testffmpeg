//
#include "StdAfx.h"
//#include <windows.h>
//#include "FFMpegUtil.h"
////#include <afx.h>
//
//const char *filter_descr = "amix=inputs=2,asetnsamples=n=1024";
//void getMixAudio();
//FILE * f;
////FILE* f1;
//
////ffmpeg -i 1.mp3 -i 2.mp3 -filter_complex amix=inputs=2:duration=longest:dropout_transition=2 a.mp3
//
//static AVFormatContext *fmt_ctx1;
//static AVFormatContext *fmt_ctx2;
//static AVFormatContext *fmt_ctx3;
//static AVCodecContext *dec_ctx1;
//static AVCodecContext *dec_ctx2;
//static AVCodecContext *dec_ctx3;
//
//AVFilterContext *buffersink_ctx;
//AVFilterContext *buffersrc_ctx1;
//AVFilterContext *buffersrc_ctx2;
//AVFilterContext *buffersrc_ctx3;
//AVFilterGraph *filter_graph;
//static int audio_stream_index = -1;
//
//
//AVFrame* frame1 = avcodec_alloc_frame();
//AVFrame* frame2 = avcodec_alloc_frame();
//AVFrame* frame3 = avcodec_alloc_frame();
//
//CRITICAL_SECTION crt;
//CRITICAL_SECTION crt_mix;
//
//static int open_input_file(const char *filename, AVFormatContext*& fmt_ctx, AVCodecContext*& dec_ctx)
//{
//    int ret;
//    AVCodec *dec;
//
//    if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
//        return ret;
//    }
//
//    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
//        return ret;
//    }
//
//    /* select the audio stream */
//    ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
//    if (ret < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Cannot find a audio stream in the input file\n");
//        return ret;
//    }
//    audio_stream_index = ret;
//    dec_ctx = fmt_ctx->streams[audio_stream_index]->codec;
//	dec_ctx->frame_size = 1024;
//
//    /* init the audio decoder */
//    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Cannot open audio decoder\n");
//        return ret;
//    }
//
//    return 0;
//}
//
//static int init_filters(const char *filters_descr)
//{
//	char args[512] = {0};
//	char name[100] = {0};
//    int ret;
//
//	//创建graph
//	filter_graph = avfilter_graph_alloc();
//
//	//创建输入输出
//	AVFilterInOut *outputs;// = avfilter_inout_alloc();
//	AVFilterInOut *inputs;// = avfilter_inout_alloc();
//	if ((ret = avfilter_graph_parse2(filter_graph, filters_descr, &inputs, &outputs)) < 0)
//		return ret;
//
//	AVRational time_base = {1,44100};//fmt_ctx->streams[audio_stream_index]->time_base;
//
//	//加入两个流
//    AVFilter *abuffersrc  = avfilter_get_by_name("abuffer");
//	if (!dec_ctx1->channel_layout)
//		dec_ctx1->channel_layout = av_get_default_channel_layout(dec_ctx1->channels);
//
//	sprintf_s(args, sizeof(args),
//		"time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%I64d",
//		time_base.num, time_base.den, dec_ctx1->sample_rate,
//		av_get_sample_fmt_name(dec_ctx1->sample_fmt), dec_ctx1->channel_layout);
//	sprintf_s(name, sizeof(name), "graph input from audio stream %d", 0);
//
//	ret = avfilter_graph_create_filter(&buffersrc_ctx1, abuffersrc, name,args, 0, filter_graph);
//
//	if ((ret = avfilter_link(buffersrc_ctx1, 0, inputs->filter_ctx, inputs->pad_idx)) < 0)
//		return ret;
//
//	//第二个流
//	AVFilter *abuffersrc2  = avfilter_get_by_name("abuffer");
//	if (!dec_ctx2->channel_layout)
//		dec_ctx2->channel_layout = av_get_default_channel_layout(dec_ctx2->channels);
//
//	sprintf_s(args, sizeof(args),
//		"time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%I64d",
//		time_base.num, time_base.den, dec_ctx2->sample_rate,
//		av_get_sample_fmt_name(dec_ctx2->sample_fmt), dec_ctx2->channel_layout);
//	sprintf_s(name, sizeof(name), "graph input from audio stream %d", 1);
//
//	ret = avfilter_graph_create_filter(&buffersrc_ctx2, abuffersrc2, name, args, NULL, filter_graph);
//	if ((ret = avfilter_link(buffersrc_ctx2, 0, inputs->next->filter_ctx, inputs->next->pad_idx)) < 0)
//		return ret;
//
//	//第三个流
//	AVFilter *abuffersrc3  = avfilter_get_by_name("abuffer");
//	if (!dec_ctx3->channel_layout)
//		dec_ctx3->channel_layout = av_get_default_channel_layout(dec_ctx3->channels);
//	sprintf_s(args, sizeof(args),
//		"time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%I64d",
//		time_base.num, time_base.den, dec_ctx3->sample_rate,
//		av_get_sample_fmt_name(dec_ctx3->sample_fmt), dec_ctx3->channel_layout);
//	sprintf_s(name, sizeof(name), "graph input from audio stream %d", 1);
//	ret = avfilter_graph_create_filter(&buffersrc_ctx3, abuffersrc3, name, args, NULL, filter_graph);
//	if ((ret = avfilter_link(buffersrc_ctx3, 0, inputs->next->next->filter_ctx, inputs->next->next->pad_idx)) < 0)
//		return ret;
//
//	//输出流
//	const enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE };
//	int64_t ch_layout = AV_CH_LAYOUT_MONO;
//	int ch_count = 1;
//	int sample_rate = 44100;
//	AVABufferSinkParams *abuffersink_params;
//     AVFilter *abuffersink = avfilter_get_by_name("abuffersink");
//	abuffersink_params = av_abuffersink_params_alloc();
//	abuffersink_params->sample_fmts     = sample_fmts;
//	abuffersink_params->channel_counts = &ch_count;
//	abuffersink_params->channel_layouts = &ch_layout;
//	abuffersink_params->sample_rates = &sample_rate;
//	sprintf_s(name, sizeof(name), "output audio stream");
//	ret = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, name, 0, 0, filter_graph);
//
//	AVFilterContext *format;
//	sprintf_s(args, sizeof(args), "sample_fmts=s16:sample_rates=%d:channel_layouts=0x3", 44100);
//	sprintf_s(name, sizeof(name), "audio format for output audio stream %d", 1);
//	ret = avfilter_graph_create_filter(&format,	avfilter_get_by_name("aformat"),name, args, NULL, filter_graph);
//	if (ret < 0)
//		return ret;
//
//	ret = avfilter_link(outputs->filter_ctx, outputs->pad_idx, format, 0);
//	if (ret < 0)
//		return ret;
//
//	if ((ret = avfilter_link(format, outputs->pad_idx, buffersink_ctx, 0)) < 0)
//		return ret;
//	//av_free(abuffersink_params);
//	//avfilter_inout_free(&outputs);
//
//	char s[50];
//	if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
//		return ret;
//
//    return 0;
//}
//
//void getAudio(AVFormatContext* fmt_ctx, AVCodecContext* ctx, AVFrame* frame, AVFilterContext* dst_filter)
//{
//	//	AVFrame* frame = avcodec_alloc_frame();
//	int got_frame = 0;
//	uint8_t str[2304];
//	while (1) 
//	{
//		AVPacket packet;
//		av_init_packet(&packet);
//		int ret = 0;
//		if ((ret = av_read_frame(fmt_ctx, &packet)) < 0)
//			break;
//
//		if (packet.stream_index == audio_stream_index) 
//		{
//			av_frame_unref(frame);
//			avcodec_get_frame_defaults(frame);
//			got_frame = 0;
//			EnterCriticalSection(&crt);
//			ret = avcodec_decode_audio4(ctx, frame, &got_frame, &packet);
//			if (ret < 0) {
//				av_log(NULL, AV_LOG_ERROR, "Error decoding audio\n");
//				continue;
//			}
//			LeaveCriticalSection(&crt);
//			if (got_frame) 
//			{			
//				if (av_buffersrc_add_frame_flags(dst_filter, frame, AV_BUFFERSRC_FLAG_PUSH) < 0) {
//					av_log(NULL, AV_LOG_ERROR, "Error while feeding the audio filtergraph\n");
//					break;
//				}
//				printf("buffer index 0 \n");
//			}
//		}
//		av_free_packet(&packet);
//		break;
//	}
//}
//
//DWORD WINAPI audio_first(LPVOID lpContext)
//{
//	getAudio(fmt_ctx1, dec_ctx1, frame1, buffersrc_ctx1);
//	return 0;
//}
//
//DWORD WINAPI audio_second(LPVOID lpContext)
//{
//	getAudio(fmt_ctx2, dec_ctx2, frame2, buffersrc_ctx2);
//	return 0;
//}
//
//DWORD WINAPI audio_third(LPVOID lpContext)
//{
//	getAudio(fmt_ctx3, dec_ctx3, frame3, buffersrc_ctx3);
//	return 0;
//}
//
//void getMixAudio()
//{
//	AVFrame* out_frame_ = avcodec_alloc_frame();
//	int ret = 0;
//	while (1) 
//	{
//		ret = av_buffersink_get_frame(buffersink_ctx, out_frame_);//, AV_BUFFERSINK_FLAG_NO_REQUEST);
//		char s[50];
//		if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
//		break;
//		if(ret >= 0)
//		{
//			printf("get mix audio  \n");
//			fwrite(out_frame_->data[0], 4096, 1, f);
//			if (out_frame_->data[1])
//			{
//				fwrite(out_frame_->data[1], 4096, 1, f);
//			}
//			
//		}
//	}
//	avfilter_graph_request_oldest(filter_graph);
//	avcodec_free_frame(&out_frame_);
//}
//
//int main(int argc, char **argv)
//{
//    int ret;
//    AVPacket packet;
//    AVFrame frame;
//    int got_frame;
//
////     if (argc != 2) {
////         fprintf(stderr, "Usage: %s file | %s\n", argv[0], player);
////         exit(1);
////     }
//
//    avcodec_register_all();
//    av_register_all();
//    avfilter_register_all();
//
//    f = fopen("F:\\testfiles\\music\\a.mp3","wb");
//
//	InitializeCriticalSection(&crt);
//	InitializeCriticalSection(&crt_mix);
//
//    ret = open_input_file("F:\\testfiles\\music\\12.mp3", fmt_ctx1, dec_ctx1);
//    ret = open_input_file("F:\\testfiles\\music\\13.mp3", fmt_ctx2, dec_ctx2);
//	ret = open_input_file("F:\\testfiles\\music\\14.mp3", fmt_ctx3, dec_ctx3);
//    ret = init_filters(filter_descr);
//
//	//分别处理输入
//	CreateThread(NULL, 0, audio_first, NULL, 0, 0);
//	CreateThread(NULL, 0, audio_second, NULL, 0, 0);
//	CreateThread(NULL, 0, audio_third, NULL, 0, 0);
//
//
//	getMixAudio();
//	Sleep(100000);
//
//    fclose(f);
//    exit(0);
//}
