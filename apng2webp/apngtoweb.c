//在下面代码中增加对PNG转webp的支持：

#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <webp/encode.h>
#include <webp/decode.h>
#include <webp/mux.h>
#include <jpeglib.h> 


int apng_to_webp(const char* apng_file, const char* webp_file) {
    AVFormatContext* format_ctx = NULL;
    if (avformat_open_input(&format_ctx, apng_file, NULL, NULL) != 0) {
        return -1;
    }
    
    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        avformat_close_input(&format_ctx);
        return -1;
    }

    int video_stream_index = -1;
    unsigned int i = 0;
    for (i = 0; i < format_ctx->nb_streams; ++i) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        avformat_close_input(&format_ctx);
        return -1;
    }

    AVCodecParameters* codecpar = format_ctx->streams[video_stream_index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        avformat_close_input(&format_ctx);
        return -1;
    }

    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        avformat_close_input(&format_ctx);
        return -1;
    }

    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        avcodec_close(codec_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    AVPacket packet;
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;

    WebPAnimEncoderOptions anim_encoder_options;
    WebPAnimEncoderOptionsInit(&anim_encoder_options);
    anim_encoder_options.anim_params.loop_count = 0;

    WebPAnimEncoder* anim_encoder = WebPAnimEncoderNew(codec_ctx->width, codec_ctx->height, &anim_encoder_options);
    if (!anim_encoder) {
        av_frame_free(&frame);
        avcodec_close(codec_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    int timestamp = 0;
    while (av_read_frame(format_ctx, &packet) >= 0) {
        if (packet.stream_index == video_stream_index) {
            if (avcodec_send_packet(codec_ctx, &packet) >= 0) {
                while (avcodec_receive_frame(codec_ctx, frame) >= 0) {
                    WebPPicture pic;
                    WebPPictureInit(&pic);
                    pic.width = codec_ctx->width;
                    pic.height = codec_ctx->height;
                    pic.use_argb = 1;
                    WebPPictureAlloc(&pic);

                    uint8_t* src_data[4] = {frame->data[0], frame->data[1], frame->data[2], NULL};
                    int src_linesize[4] = {frame->linesize[0], frame->linesize[1], frame->linesize[2], 0};
                    WebPPictureImportRGBA(&pic, src_data[0], src_linesize[0]);

                    AVRational fps = {1, 1000};

                    int duration_ms = av_rescale_q(frame->pkt_duration, format_ctx->streams[video_stream_index]->time_base, fps);
                    WebPAnimEncoderAdd(anim_encoder, &pic, timestamp, NULL);
                    timestamp += duration_ms;

                    WebPPictureFree(&pic);
                }
            }
        }
        av_packet_unref(&packet);
    }

    WebPAnimEncoderAdd(anim_encoder, NULL, timestamp, NULL);

    WebPData webp_data;
    WebPDataInit(&webp_data);
    WebPAnimEncoderAssemble(anim_encoder, &webp_data);
    
    // 写入WebP文件
    FILE* webp_fp = fopen(webp_file, "wb");
    if (!webp_fp) {
        WebPDataClear(&webp_data);
        WebPAnimEncoderDelete(anim_encoder);
        av_frame_free(&frame);
        avcodec_close(codec_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    fwrite(webp_data.bytes, 1, webp_data.size, webp_fp);
    fclose(webp_fp);

    // 释放资源
    WebPDataClear(&webp_data);
    WebPAnimEncoderDelete(anim_encoder);
    av_frame_free(&frame);
    avcodec_close(codec_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);

    return 0;
}

int file_convert(const char* in_file, const char* out_file){
    AVFormatContext* format_ctx = NULL;
    if (avformat_open_input(&format_ctx, in_file, NULL, NULL) != 0) {
        return -1;
    }

    int ret = 0;
    /* 判断图片的格式 */
    if (0 == strcmp("apng", format_ctx->iformat->name)) { //apng
        if (0 != apng_to_webp(in_file, out_file)) {
            printf("Failed to convert APNG to WEBP.\n");
            ret = -1; 
        }
        else ret = 0;
    }
    else if (0 == strcmp("gif", format_ctx->iformat->name)) { //gif
        ret = -1;
    }
    else if (0 == strcmp("image2", format_ctx->iformat->name)) { //image2
		printf("Failed, the file type is: %s .\n",format_ctx->iformat->name);
        ret = -1;
    }
    else {
		printf("Failed, the file type is: %s .\n",format_ctx->iformat->name);
        ret = -1;
    }
    avformat_close_input(&format_ctx);
    return ret;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
		printf("Usage: %s input.apng output.webp \n",argv[0]);
        return 1; 
    }
    
    if (0 != file_convert(argv[1], argv[2])) {
		printf("Failed, Only support APNG to WEBP.\n");
        return -1; 
    }

	printf("APNG to WEBP conversion succeeded.\n");
    return 0; 
}

