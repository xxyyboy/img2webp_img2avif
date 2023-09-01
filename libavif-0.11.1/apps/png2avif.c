#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
    AVFormatContext * formatContext = NULL;
    AVCodec * codec = NULL;
    AVStream * stream = NULL;
    AVOutputFormat * outputFormat = NULL;
    AVCodecContext * codecContext = NULL;
    AVFrame * frame = NULL;
    AVPacket packet = { 0 };
    int ret = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.png output.avif\n", argv[0]);
        return 1;
    }

    // 打开输入文件
    FILE * inputFile = fopen(argv[1], "rb");
    if (!inputFile) {
        fprintf(stderr, "Failed to open input file %s\n", argv[1]);
        return 1;
    }

    // 创建输出流
    formatContext = avformat_alloc_context();
    if (!formatContext) {
        fprintf(stderr, "Failed to allocate format context\n");
        return 1;
    }

    AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_AV1);  
    //codec = avcodec_find_encoder_by_name("libaom-av1");
    if (codec == NULL) {  
    // 找不到 AVIF 编译器，处理错误  
    }

    // 创建输出格式
    outputFormat = av_output_format_by_name("av1");
    if (!outputFormat) {
        fprintf(stderr, "Failed to find output format\n");
        return 1;
    }

    // 设置输出格式
    ret = avformat_set_output_format(formatContext, outputFormat);
    if (ret < 0) {
        fprintf(stderr, "Failed to set output format\n");
        return 1;
    }

    // 创建视频流
    stream = avformat_new_stream(formatContext, codec);
    if (!stream) {
        fprintf(stderr, "Failed to create video stream\n");
        return 1;
    }
    //   codec = avcodec_find_encoder_by_name("libaom-av1");
    // 设置视频编码器
    codec = avcodec_find_encoder(outputFormat->codec_id);
    if (!codec) {
        fprintf(stderr, "Failed to find video codec\n");
        return 1;
    }

    // 创建视频编码器上下文
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        fprintf(stderr, "Failed to allocate video codec context\n");
        return 1;
    }

    // 设置视频编码器参数
    avcodec_open2(codecContext, codec, NULL);
    if (avcodec_init(codecContext)) {
        fprintf(stderr, "Failed to initialize video codec\n");
        return 1;
    }

    // 设置视频流参数
    avstream_set_codec(stream, codec);
    avstream_set_codec_context(stream, codecContext);
    avstream_set_format(stream, outputFormat);

    // 写入视频数据
    while (1) {
        // 读取 PNG 帧
        fread(&frame, 1, sizeof(AVFrame), inputFile);
        if (feof(inputFile)) {
            break;
        }
        // 压缩视频帧
        ret = avcodec_send_frame(codecContext, &frame);
        if (ret < 0) {
            fprintf(stderr, "Failed to send video frame\n");
            return 1;
        }
        // 接收压缩后的视频帧
        ret = avcodec_receive_frame(codecContext, &packet);
        if (ret < 0) {
            fprintf(stderr, "Failed to receive video frame\n");
            return 1;
        }
        // 输出压缩后的视频帧
        ret = avformat_write_header(formatContext, NULL);
        if (ret < 0) {
            fprintf(stderr, "Failed to write output header\n");
            return 1;
        }
        ret = avformat_write_frame(formatContext, NULL);
        if (ret < 0) {
            fprintf(stderr, "Failed to write video frame\n");
            return 1;
        }
        ret = avformat_close_stream(formatContext, stream);
        if (ret < 0) {
            fprintf(stderr, "Failed to close video stream\n");
            return 1;
        }
    }

    // 关闭视频编码器
    avcodec_close(codecContext);
    avcodec_free_context(&codecContext);
    // 完成输出
    ret = avformat_write_header(formatContext, NULL);
    if (ret < 0) {
        fprintf(stderr, "Failed to write output header\n");
        return 1;
    }
    ret = avformat_write_frame(formatContext, NULL);
    if (ret < 0) {
        fprintf(stderr, "Failed to write video frame\n");
        return 1;
    }
    ret = avformat_close_stream(formatContext, stream);
    if (ret < 0) {
        fprintf(stderr, "Failed to close video stream\n");
        return 1;
    }

    // 关闭输出流
    ret = avformat_close_context(formatContext);
    if (ret < 0) {
        fprintf(stderr, "Failed to close output context\n");
        return 1;
    }

    // 释放内存
    fclose(inputFile);
    avformat_free_context(formatContext);
    avcodec_free_context(&codecContext);
    av_packet_unref(&packet);
    av_frame_free(&frame);
    return 0;
}