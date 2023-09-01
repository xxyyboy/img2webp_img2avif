#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
    if (argc != 3) {
        printf("Usage: %s input.png output.avif\n", argv[0]);
        return 1;
    }

    const char * input_filename = argv[1];
    const char * output_filename = argv[2];

    // 初始化FFmpeg库
    //av_register_all();
    avformat_network_init();

    // 打开输入文件
    AVFormatContext * input_format_context = NULL;
    if (avformat_open_input(&input_format_context, input_filename, NULL, NULL) != 0) {
        fprintf(stderr, "Error: Could not open input file '%s'\n", input_filename);
        return 1;
    }

    // 获取输入文件信息
    if (avformat_find_stream_info(input_format_context, NULL) < 0) {
        fprintf(stderr, "Error: Could not find input stream information\n");
        return 1;
    }

    // 查找视频流
    int video_stream_index = -1;
    for (int i = 0; i < input_format_context->nb_streams; i++) {
        if (input_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        fprintf(stderr, "Error: Could not find video stream in the input file\n");
        return 1;
    }

    // 获取视频流编解码器参数
    AVCodecParameters * input_codec_parameters = input_format_context->streams[video_stream_index]->codecpar;

    // 查找视频流编解码器
    AVCodec * input_codec = avcodec_find_decoder(input_codec_parameters->codec_id);
    if (input_codec == NULL) {
        fprintf(stderr, "Error: Could not find input codec\n");
        return 1;
    }

    // 打开视频流编解码器
    AVCodecContext * input_codec_context = avcodec_alloc_context3(input_codec);
    if (avcodec_parameters_to_context(input_codec_context, input_codec_parameters) < 0) {
        fprintf(stderr, "Error: Could not copy input codec parameters to decoder context\n");
        return 1;
    }

    if (avcodec_open2(input_codec_context, input_codec, NULL) < 0) {
        fprintf(stderr, "Error: Could not open input codec\n");
        return 1;
    }

    // 创建输出文件
    AVFormatContext * output_format_context = NULL;
    if (avformat_alloc_output_context2(&output_format_context, NULL, "avif", output_filename) < 0) {
        fprintf(stderr, "Error: Could not create output context\n");
        return 1;
    }

    // 查找输出文件编码器
    AVCodec * output_codec = avcodec_find_encoder_by_name("libaom-av1");
    if (output_codec == NULL) {
        fprintf(stderr, "Error: Could not find output codec\n");
        return 1;
    }

    // 创建输出文件流
    AVStream * output_stream = avformat_new_stream(output_format_context, output_codec);
    if (output_stream == NULL) {
        fprintf(stderr, "Error: Could not create output stream\n");
        return 1;
    }

    // 获取输出文件流编解码器参数
    AVCodecParameters * output_codec_parameters = output_stream->codecpar;

    // 设置输出文件流编解码器参数
    output_codec_parameters->codec_type = AVMEDIA_TYPE_VIDEO;
    output_codec_parameters->codec_id = output_codec->id;
    output_codec_parameters->width = input_codec_parameters->width;
    output_codec_parameters->height = input_codec_parameters->height;
    output_codec_parameters->format = AV_PIX_FMT_YUV420P;

    // 打开输出文件编解码器
    AVCodecContext * output_codec_context = avcodec_alloc_context3(output_codec);
    if (avcodec_parameters_to_context(output_codec_context, output_codec_parameters) < 0) {
        fprintf(stderr, "Error: Could not copy output codec parameters to encoder context\n");
        return 1;
    }

    output_codec_context->time_base = input_format_context->streams[video_stream_index]->time_base;
    output_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
    output_codec_context->global_quality = 30;
    output_codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    output_codec_context->framerate.num = input_codec_context->framerate.num;
    output_codec_context->framerate.den = input_codec_context->framerate.den;

    if (avcodec_open2(output_codec_context, output_codec, NULL) < 0) {
        fprintf(stderr, "Error: Could not open output codec\n");
        return 1;
    }

    // 打开输出文件
    if (!(output_format_context->oformat->flags & AVFMT_NOFILE)) {
        int ret = avio_open(&output_format_context->pb, output_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Error: Could not open output file '%s'\n", output_filename);
            return 1;
        }
    }

    // 写入文件头
    if (avformat_write_header(output_format_context, NULL) < 0) {
        fprintf(stderr, "Error: Could not write output file header\n");
        return 1;
    }

    // 分配帧缓存
    AVFrame * frame = av_frame_alloc();
    if (frame == NULL) {
        fprintf(stderr, "Error: Could not allocate frame\n");
        return 1;
    }

    // 分配输出帧缓存
    AVFrame * output_frame = av_frame_alloc();
    if (output_frame == NULL) {
        fprintf(stderr, "Error: Could not allocate output frame\n");
        return 1;
    }

    output_frame->width = input_codec_parameters->width;
    output_frame->height = input_codec_parameters->height;
    output_frame->format = AV_PIX_FMT_YUV420P;

    if (av_frame_get_buffer(output_frame, 32) < 0) {
        fprintf(stderr, "Error: Could not allocate output frame data\n");
        return 1;
    }

    // 初始化SwsContext以进行图像缩放和格式转换
    struct SwsContext * sws_context = sws_getContext(input_codec_parameters->width,
                                                     input_codec_parameters->height,
                                                     input_codec_context->pix_fmt,
                                                     output_codec_parameters->width,
                                                     output_codec_parameters->height,
                                                     AV_PIX_FMT_YUV420P,
                                                     SWS_BICUBIC,
                                                     NULL,
                                                     NULL,
                                                     NULL);
    if (!sws_context) {
        fprintf(stderr, "Error: Could not initialize SwsContext\n");
        return 1;
    }

    // 初始化输出数据包
    AVPacket output_packet;
    av_init_packet(&output_packet);
    output_packet.data = NULL;
    output_packet.size = 0;

    // 读取输入文件帧并编码为AV1格式
    int frame_index = 0;
    while (1) {

        AVPacket input_packet;
        av_init_packet(&input_packet);
        input_packet.data = NULL;
        input_packet.size = 0;

        if (av_read_frame(input_format_context, &input_packet) < 0) {
            break;
        }

        if (input_packet.stream_index == video_stream_index) {
            int ret = avcodec_send_packet(input_codec_context, &input_packet);
            if (ret < 0) {
                fprintf(stderr, "Error: Could not send input packet to decoder\n");
                return 1;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(input_codec_context, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    fprintf(stderr, "Error: Could not receive input frame from decoder\n");
                    return 1;
                }

                // 将输入帧转换为YUV420P格式
                int ret = sws_scale(sws_context,
                                    (const uint8_t * const *)frame->data,
                                    frame->linesize,
                                    0,
                                    input_codec_parameters->height,
                                    output_frame->data,
                                    output_frame->linesize);

                if (ret < 0) {
                    fprintf(stderr, "Error: Could not convert input frame to YUV420P format\n");
                    return 1;
                }

                //output_frame->pts = frame_index++;
                output_frame->pts = frame->best_effort_timestamp;

                // 编码输出帧为AV1格式
                ret = avcodec_send_frame(output_codec_context, output_frame);
                if (ret < 0) {
                    fprintf(stderr, "Error: Could not send output frame to encoder\n");
                    return 1;
                }

                while (ret >= 0) {
                    ret = avcodec_receive_packet(output_codec_context, &output_packet);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        break;
                    } else if (ret < 0) {
                        fprintf(stderr, "Error: Could not receive output packet from encoder\n");
                        return 1;
                    }

                    // 写入输出数据包到文件
                    av_packet_rescale_ts(&output_packet, output_codec_context->time_base, output_stream->time_base);
                    output_packet.stream_index = output_stream->index;

                    ret = av_interleaved_write_frame(output_format_context, &output_packet);
                    if (ret < 0) {
                        fprintf(stderr, "Error: Could not write output packet to file\n");
                        return 1;
                    }
                    
                    av_packet_unref(&output_packet);
                }
            }
        }

        av_packet_unref(&input_packet);
    }

    // 写入文件尾
    if ((av_write_trailer(output_format_context)) < 0) {
        fprintf(stderr, "Failed to write output trailer");
        exit(1);
    }

    // 关闭输出文件
    if (!(output_format_context->oformat->flags & AVFMT_NOFILE))
        avio_closep(&output_format_context->pb);

    // 释放资源
    avcodec_free_context(&input_codec_context);
    avcodec_free_context(&output_codec_context);
    avformat_close_input(&input_format_context);
    avformat_free_context(output_format_context);
    av_frame_free(&frame);
    av_frame_free(&output_frame);

    return 0;
}