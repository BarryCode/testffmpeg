#include "StdAfx.h"
#include "FFMpegMixAudio.h"


FFMpegMixAudio::FFMpegMixAudio(void):audio_stream_index(0),
    filter_graph(NULL),
    fmt_ctx1(NULL),
    fmt_ctx2(NULL),
    dec_ctx1(NULL),
    dec_ctx2(NULL),
    dec_ctx(NULL),
    buffersink_ctx(NULL),
    buffersrc_ctx1(NULL),
    buffersrc_ctx2(NULL)
{
}


FFMpegMixAudio::~FFMpegMixAudio(void)
{
	avcodec_free_frame(&pFrame);
}

void FFMpegMixAudio::mix_audios(const char* src_file1, const char* src_file2, const char* dst_file)
{
    int ret;
    AVPacket packet;
    AVFrame frame;
    int got_frame;
    FILE* f;
    AVFrame* frame1 = avcodec_alloc_frame();
    AVFrame* frame2 = avcodec_alloc_frame();
    const char *filter_descr = "amix=inputs=2:duration=first:dropout_transition=3,asetnsamples=n=1024";
    AVOutputFormat* fmt;
    AVStream* audio_st;
    AVCodec* pCodec;
    int size;
    uint8_t *frame_buf;

    avcodec_register_all();
    av_register_all();
    avfilter_register_all();

    pFormatCtx = avformat_alloc_context();  
    fmt = av_guess_format(NULL, dst_file, NULL);  
    pFormatCtx->oformat = fmt;

    if (avio_open(&pFormatCtx->pb,dst_file, AVIO_FLAG_READ_WRITE) < 0){  
    printf("Failed to open output file!\n");  
    exit(1);  
    }  
    
    pCodec = avcodec_find_encoder(fmt->audio_codec);  
    if (!pCodec){  
        printf("Can not find encoder!\n");  
        exit(1); 
    }  

    audio_st = avformat_new_stream(pFormatCtx, pCodec);
    if (audio_st==NULL){  
        exit(1); 
    }  
    pCodecCtx = audio_st->codec;  
    pCodecCtx->codec_id = fmt->audio_codec;  
    pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;  
    pCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16P;  
    pCodecCtx->sample_rate= 44100;  
    pCodecCtx->channel_layout=AV_CH_LAYOUT_MONO;  
    pCodecCtx->channels = av_get_channel_layout_nb_channels(pCodecCtx->channel_layout);  
    pCodecCtx->bit_rate = 64000; 
  
    //Show some information  
    av_dump_format(pFormatCtx, 0, dst_file, 1);
  
    if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0){  
        printf("Failed to open encoder!\n");  
        exit(1); 
    }  
    pFrame = av_frame_alloc();  
    pFrame->nb_samples= pCodecCtx->frame_size;  
    pFrame->format= pCodecCtx->sample_fmt; 
    size = av_samples_get_buffer_size(NULL, pCodecCtx->channels,pCodecCtx->frame_size,pCodecCtx->sample_fmt, 1);  
    frame_buf = (uint8_t *)av_malloc(size);  
    avcodec_fill_audio_frame(pFrame, pCodecCtx->channels, pCodecCtx->sample_fmt,(const uint8_t*)frame_buf, size, 1);  
      
    //Write Header  
    avformat_write_header(pFormatCtx,NULL); 
    //f = fopen(dst_file,"wb");

    ret = open_input_file(src_file1, fmt_ctx1, dec_ctx1);
    ret = open_input_file(src_file2, fmt_ctx2, dec_ctx2);
    ret = init_filters(filter_descr);

	//分别处理输入
    getAudio(fmt_ctx1, dec_ctx1, frame1, buffersrc_ctx1);
    getAudio(fmt_ctx2, dec_ctx2, frame2, buffersrc_ctx2);

	getMixAudio();
        //Flush Encoder  
    //ret = flush_encoder(pFormatCtx,0);
    //if (ret < 0) {  
    //    printf("Flushing encoder failed\n");  
    //    exit(1);  
    //}  
  
    //Write Trailer  
    av_write_trailer(pFormatCtx); 
        if (audio_st){  
        avcodec_close(audio_st->codec);  
        //av_free(pFrame);
        av_free(frame_buf);  
    }  
    avio_close(pFormatCtx->pb);  
    avformat_free_context(pFormatCtx);  
  
    //fclose(f);
}

int FFMpegMixAudio::open_input_file(const char *filename, AVFormatContext*& fmt_ctx, AVCodecContext*& dec_ctx)
{
    int ret;
    AVCodec *dec;

    if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    /* select the audio stream */
    ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find a audio stream in the input file\n");
        return ret;
    }
    audio_stream_index = ret;
    dec_ctx = fmt_ctx->streams[audio_stream_index]->codec;
	dec_ctx->frame_size = 1024;

    /* init the audio decoder */
    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open audio decoder\n");
        return ret;
    }

    return 0;
}

int FFMpegMixAudio::init_filters(const char *filters_descr)
{
	char args[512] = {0};
	char name[100] = {0};
    int ret;

	//创建graph
	filter_graph = avfilter_graph_alloc();

	//创建输入输出
	AVFilterInOut *outputs;// = avfilter_inout_alloc();
	AVFilterInOut *inputs;// = avfilter_inout_alloc();
	if ((ret = avfilter_graph_parse2(filter_graph, filters_descr, &inputs, &outputs)) < 0)
		return ret;

	AVRational time_base = {1,44100};//fmt_ctx->streams[audio_stream_index]->time_base;

	//加入两个流
    AVFilter *abuffersrc  = avfilter_get_by_name("abuffer");
	if (!dec_ctx1->channel_layout)
		dec_ctx1->channel_layout = av_get_default_channel_layout(dec_ctx1->channels);

	sprintf_s(args, sizeof(args),
		"time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%I64d",
		time_base.num, time_base.den, dec_ctx1->sample_rate,
		av_get_sample_fmt_name(dec_ctx1->sample_fmt), dec_ctx1->channel_layout);
	sprintf_s(name, sizeof(name), "graph input from audio stream %d", 0);

	ret = avfilter_graph_create_filter(&buffersrc_ctx1, abuffersrc, name,args, 0, filter_graph);

	if ((ret = avfilter_link(buffersrc_ctx1, 0, inputs->filter_ctx, inputs->pad_idx)) < 0)
		return ret;

	//第二个流
	AVFilter *abuffersrc2  = avfilter_get_by_name("abuffer");
	if (!dec_ctx2->channel_layout)
		dec_ctx2->channel_layout = av_get_default_channel_layout(dec_ctx2->channels);

	sprintf_s(args, sizeof(args),
		"time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%I64d",
		time_base.num, time_base.den, dec_ctx2->sample_rate,
		av_get_sample_fmt_name(dec_ctx2->sample_fmt), dec_ctx2->channel_layout);
	sprintf_s(name, sizeof(name), "graph input from audio stream %d", 1);

	ret = avfilter_graph_create_filter(&buffersrc_ctx2, abuffersrc2, name, args, NULL, filter_graph);
	if ((ret = avfilter_link(buffersrc_ctx2, 0, inputs->next->filter_ctx, inputs->next->pad_idx)) < 0)
		return ret;

	////第三个流
	//AVFilter *abuffersrc3  = avfilter_get_by_name("abuffer");
	//if (!dec_ctx3->channel_layout)
	//	dec_ctx3->channel_layout = av_get_default_channel_layout(dec_ctx3->channels);
	//sprintf_s(args, sizeof(args),
	//	"time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%I64d",
	//	time_base.num, time_base.den, dec_ctx3->sample_rate,
	//	av_get_sample_fmt_name(dec_ctx3->sample_fmt), dec_ctx3->channel_layout);
	//sprintf_s(name, sizeof(name), "graph input from audio stream %d", 1);
	//ret = avfilter_graph_create_filter(&buffersrc_ctx3, abuffersrc3, name, args, NULL, filter_graph);
	//if ((ret = avfilter_link(buffersrc_ctx3, 0, inputs->next->next->filter_ctx, inputs->next->next->pad_idx)) < 0)
	//	return ret;

	//输出流
	const enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE };
	int64_t ch_layout = AV_CH_LAYOUT_MONO;
	int ch_count = 1;
	int sample_rate = 44100;
	AVABufferSinkParams *abuffersink_params;
     AVFilter *abuffersink = avfilter_get_by_name("abuffersink");
	abuffersink_params = av_abuffersink_params_alloc();
	abuffersink_params->sample_fmts     = sample_fmts;
	abuffersink_params->channel_counts = &ch_count;
	abuffersink_params->channel_layouts = &ch_layout;
	abuffersink_params->sample_rates = &sample_rate;
	sprintf_s(name, sizeof(name), "output audio stream");
	ret = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, name, 0, 0, filter_graph);

	AVFilterContext *format;
	sprintf_s(args, sizeof(args), "sample_fmts=s16p:sample_rates=%d:channel_layouts=mono",44100);
	sprintf_s(name, sizeof(name), "audio format for output audio stream %d", 1);
	ret = avfilter_graph_create_filter(&format,	avfilter_get_by_name("aformat"),name, args, NULL, filter_graph);
	if (ret < 0)
		return ret;

	ret = avfilter_link(outputs->filter_ctx, outputs->pad_idx, format, 0);
	if (ret < 0)
		return ret;

	if ((ret = avfilter_link(format, outputs->pad_idx, buffersink_ctx, 0)) < 0)
		return ret;
	//av_free(abuffersink_params);
	//avfilter_inout_free(&outputs);

	char s[50];
	if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
		return ret;

    return 0;
}

void FFMpegMixAudio::getAudio(AVFormatContext* fmt_ctx, AVCodecContext* ctx, AVFrame* frame, AVFilterContext* dst_filter)
{
	//	AVFrame* frame = avcodec_alloc_frame();
	int got_frame = 0;
	uint8_t str[2304];
	while (1) 
	{
		AVPacket packet;
		av_init_packet(&packet);
		int ret = 0;
		if ((ret = av_read_frame(fmt_ctx, &packet)) < 0)
			break;

		if (packet.stream_index == audio_stream_index) 
		{
			av_frame_unref(frame);
			avcodec_get_frame_defaults(frame);
			got_frame = 0;
			//EnterCriticalSection(&crt);
			ret = avcodec_decode_audio4(ctx, frame, &got_frame, &packet);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error decoding audio\n");
				continue;
			}
			//LeaveCriticalSection(&crt);
			if (got_frame) 
			{			
				if (av_buffersrc_add_frame_flags(dst_filter, frame, AV_BUFFERSRC_FLAG_PUSH) < 0) {
					av_log(NULL, AV_LOG_ERROR, "Error while feeding the audio filtergraph\n");
					break;
				}
				printf("buffer index 0 \n");
			}
		}
		av_free_packet(&packet);
		//break;
	}
}

void FFMpegMixAudio::getMixAudio()
{
	int ret = 0;
    int got_output;
    //open_mp3_encodec();
    AVPacket pkt;
	while (1) 
	{
		ret = av_buffersink_get_frame(buffersink_ctx, pFrame);//, AV_BUFFERSINK_FLAG_NO_REQUEST);
		char s[50];
		if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		break;
        
		if(ret >= 0)
		{
            av_init_packet(&pkt);
            pkt.data = NULL; // packet data will be allocated by the encoder
            pkt.size = 0;

            ret = avcodec_encode_audio2(pCodecCtx, &pkt, pFrame, &got_output);
            if (ret < 0) {
                fprintf(stderr, "Error encoding audio frame\n");
                exit(1);
            }
			printf("get mix audio  \n");
            if (got_output) {
                ret = av_write_frame(pFormatCtx, &pkt); 
                //fwrite(pkt.data, 1, pkt.size, f);
                av_free_packet(&pkt);
            }
			//fwrite(out_frame_->data[0], 4096, 1, f);
			//if (out_frame_->data[1])
			//{
			//	fwrite(out_frame_->data[1], 4096, 1, f);
			//}
			
		}
	}
	avfilter_graph_request_oldest(filter_graph);
}

void FFMpegMixAudio::open_mp3_encodec()
{
    /* find the MP2 encoder */
    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_MP3);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    dec_ctx = avcodec_alloc_context3(codec);
    if (!dec_ctx) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    /* put sample parameters */
    dec_ctx->bit_rate = 128000;
    dec_ctx->channels = 1;
    /* check that the encoder supports s16 pcm input */
    dec_ctx->sample_fmt = AV_SAMPLE_FMT_S16;
    /* select other audio parameters supported by the encoder */
    dec_ctx->sample_rate    = 44100;
    dec_ctx->channel_layout = AV_CH_LAYOUT_MONO;
    dec_ctx->channels       = av_get_channel_layout_nb_channels(dec_ctx->channel_layout);

    /* open it */
    if (avcodec_open2(dec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
}