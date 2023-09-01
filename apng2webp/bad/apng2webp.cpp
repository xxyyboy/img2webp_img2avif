#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <cstring>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <webp/encode.h>
#include <webp/mux.h>
}

using namespace std;

bool apng_to_webp(const char* apng_file, const char* webp_file) {
    AVFormatContext* format_ctx = nullptr;
    if (avformat_open_input(&format_ctx, apng_file, nullptr, nullptr) != 0) {
        return false;
    }
    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        avformat_close_input(&format_ctx);
        return false;
    }

    int video_stream_index = -1;
    for (unsigned int i = 0; i < format_ctx->nb_streams; ++i) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1) {
        avformat_close_input(&format_ctx);
        return false;
    }

    AVCodecParameters* codecpar = format_ctx->streams[video_stream_index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        avformat_close_input(&format_ctx);
        return false;
    }

    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        avformat_close_input(&format_ctx);
        return false;
    }

    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return false;
    }

    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return false;
    }

    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        avcodec_close(codec_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return false;
    }

    AVPacket packet;
    av_init_packet(&packet);
    packet.data = nullptr;
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
        return false;
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

                    uint8_t* src_data[4] = {frame->data[0], frame->data[1], frame->data[2], nullptr};
                    int src_linesize[4] = {frame->linesize[0], frame->linesize[1], frame->linesize[2], 0};
                    WebPPictureImportRGBA(&pic, src_data[0], src_linesize[0]);

                    int duration_ms = av_rescale_q(frame->pkt_duration, format_ctx->streams[video_stream_index]->time_base, {1, 1000});
                    WebPAnimEncoderAdd(anim_encoder, &pic, timestamp, nullptr);
                    timestamp += duration_ms;

                    WebPPictureFree(&pic);
                }
            }
        }
        av_packet_unref(&packet);
    }

    WebPAnimEncoderAdd(anim_encoder, nullptr, timestamp, nullptr);

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
        return false;
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

    return true;
}

int main(int argc, char* argv[]) {
     if (argc != 3) {
         cout << "Usage: " << argv[0] << " input.apng output.webp" << endl; return 1; 
         } 
         
    if (!apng_to_webp(argv[1], argv[2])) {
         cout << "Failed to convert APNG to WEBP." << endl; 
         return 1; 
    } 
    
    cout << "APNG to WEBP conversion succeeded." << endl; 
    return 0; 
}

    