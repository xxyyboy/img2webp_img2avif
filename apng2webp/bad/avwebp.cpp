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
#include <libavformat/avformat.h>
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

    ofstream webp_output(webp_file, ofstream::binary);
    if (!webp_output) {
        av_frame_free(&frame);
        avcodec_close(codec_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return false;
    }

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

                    WebPConfig config;
                    WebPConfigInit(&config);
                    config.lossless = 1;

                    WebPMemoryWriter writer;
                    WebPMemoryWriterInit(&writer);
                    pic.writer = WebPMemoryWrite;
                    pic.custom_ptr = &writer;

                    WebPEncode(&config, &pic);

                    webp_output.write(reinterpret_cast<const char*>(writer.mem), writer.size);

                    WebPPictureFree(&pic);
                    WebPMemoryWriterClear(&writer);
                }
            }
        }
        av_packet_unref(&packet);
    }

    webp_output.close();

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
