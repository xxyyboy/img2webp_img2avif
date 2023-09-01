#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avassert.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
    AVFormatContext * input_ctx = NULL;
    AVStream * video_stream = NULL;
    AVCodecContext * decoder_ctx = NULL;
    AVCodec * decoder = NULL;
    AVPacket packet;
    int video_stream_index = -1;
    int ret = 0, i = 0;

    // Check input arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(1);
    }

    // Open input file
    if ((ret = avformat_open_input(&input_ctx, argv[1], NULL, NULL)) < 0) {
        fprintf(stderr, "Could not open input file '%s'", argv[1]);
        exit(1);
    }

    // Find video stream
    if ((ret = avformat_find_stream_info(input_ctx, NULL)) < 0) {
        fprintf(stderr, "Could not find stream information");
        exit(1);
    }

    for (i = 0; i < input_ctx->nb_streams; i++) {
        if (input_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        fprintf(stderr, "Could not find video stream");
        exit(1);
    }

    video_stream = input_ctx->streams[video_stream_index];

    // Find decoder for the video stream
    decoder = avcodec_find_decoder(video_stream->codecpar->codec_id);
    if (!decoder) {
        fprintf(stderr, "Failed to find decoder for stream #%u", video_stream_index);
        exit(1);
    }

    // Allocate a codec context for the decoder
    decoder_ctx = avcodec_alloc_context3(decoder);
    if (!decoder_ctx) {
        fprintf(stderr, "Failed to allocate codec context for decoder");
        exit(1);
    }

    // Copy codec parameters from input stream to decoder context
    if ((ret = avcodec_parameters_to_context(decoder_ctx, video_stream->codecpar)) < 0) {
        fprintf(stderr, "Failed to copy codec parameters to decoder context");
        exit(1);
    }

    // Initialize the decoder
    if ((ret = avcodec_open2(decoder_ctx, decoder, NULL)) < 0) {
        fprintf(stderr, "Failed to open codec for decoder");
        exit(1);
    }

    // Open output file
    AVFormatContext * output_ctx = NULL;
    if ((ret = avformat_alloc_output_context2(&output_ctx, NULL, NULL, argv[2])) < 0) {
        fprintf(stderr, "Could not create output context");
        exit(1);
    }

    // Find encoder for the AV1 format
    AVCodec * encoder = avcodec_find_encoder_by_name("libaom-av1");
    if (!encoder) {
        fprintf(stderr, "Failed to find encoder for AV1 format");
        exit(1);
    }

    // Allocate a codec context for the encoder
    AVCodecContext * encoder_ctx = avcodec_alloc_context3(encoder);
    if (!encoder_ctx) {
        fprintf(stderr, "Failed to allocate codec context for encoder");
        exit(1);
    }

    // Set encoder options
    encoder_ctx->width = decoder_ctx->width;
    encoder_ctx->height = decoder_ctx->height;
    encoder_ctx->time_base = video_stream->time_base;
    encoder_ctx->framerate = video_stream->avg_frame_rate;
    encoder_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    encoder_ctx->global_quality = 30;
    encoder_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // Open the encoder
    if ((ret = avcodec_open2(encoder_ctx, encoder, NULL)) < 0) {
        fprintf(stderr, "Failed to open codec for encoder");
        exit(1);
    }

    // Add output stream to the output context
    AVStream * out_stream = avformat_new_stream(output_ctx, encoder);
    if (!out_stream) {
        fprintf(stderr, "Failed to create output stream");
        exit(1);
    }

    // Copy codec parameters from encoder context to output stream
    if ((ret = avcodec_parameters_from_context(out_stream->codecpar, encoder_ctx)) < 0) {
        fprintf(stderr, "Failed to copy codec parameters to output stream");
        exit(1);
    }

    // Write the output header
    if ((ret = avformat_write_header(output_ctx, NULL)) < 0) {
        fprintf(stderr, "Failed to write output header");
        exit(1);
    }

    // Decode frames from input file and encode to output file
    while (1) {
        // Read packet from input file
        if ((ret = av_read_frame(input_ctx, &packet)) < 0) {
            break;
        }

        // Check if packet belongs to video stream
        if (packet.stream_index == video_stream_index) {
            // Send packet to decoder
            if ((ret = avcodec_send_packet(decoder_ctx, &packet)) < 0) {
                fprintf(stderr, "Failed to send packet to decoder");
                exit(1);
            }

            // Receive frames from decoder and encode to output file
            while (1) {
                AVFrame * frame = av_frame_alloc();
                if (!frame) {
                    fprintf(stderr, "Failed to allocate frame");
                    exit(1);
                }

                // Receive frame from decoder
                ret = avcodec_receive_frame(decoder_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    av_frame_free(&frame);
                    break;
                } else if (ret < 0) {
                    fprintf(stderr, "Failed to receive frame from decoder");
                    exit(1);
                }

                // Convert frame format to YUV420P
                AVFrame * yuv_frame = av_frame_alloc();
                yuv_frame->format = AV_PIX_FMT_YUV420P;
                yuv_frame->width = decoder_ctx->width;
                yuv_frame->height = decoder_ctx->height;
                if ((ret = av_frame_get_buffer(yuv_frame, 32)) < 0) {
                    fprintf(stderr, "Failed to allocate buffer for YUV420P frame");
                    exit(1);
                }

                /*
                if ((ret = av_image_copy_plane(yuv_frame->data[0],
                                               yuv_frame->linesize[0],
                                               frame->data[0],
                                               frame->linesize[0],
                                               decoder_ctx->width,
                                               decoder_ctx->height)) < 0 ||
                    (ret = av_image_copy_plane(yuv_frame->data[1],
                                               yuv_frame->linesize[1],
                                               frame->data[1],
                                               frame->linesize[1],
                                               decoder_ctx->width / 2,
                                               decoder_ctx->height / 2)) < 0 ||
                    (ret = av_image_copy_plane(yuv_frame->data[2],
                                               yuv_frame->linesize[2],
                                               frame->data[2],
                                               frame->linesize[2],
                                               decoder_ctx->width / 2,
                                               decoder_ctx->height / 2)) < 0) {
                    fprintf(stderr, "Failed to convert frame to YUV420P format");
                    exit(1);
                }
                */
                av_image_copy_plane(yuv_frame->data[0],yuv_frame->linesize[0],frame->data[0],frame->linesize[0],decoder_ctx->width,decoder_ctx->height);
                av_image_copy_plane(yuv_frame->data[1],yuv_frame->linesize[1],frame->data[1],frame->linesize[1],decoder_ctx->width / 2,decoder_ctx->height / 2);
                av_image_copy_plane(yuv_frame->data[2],yuv_frame->linesize[2],frame->data[2],frame->linesize[2],decoder_ctx->width / 2,decoder_ctx->height / 2);
                
                // Send YUV420P frame to encoder
                if ((ret = avcodec_send_frame(encoder_ctx, yuv_frame)) < 0) {
                    fprintf(stderr, "Failed to send frame to encoder");
                    exit(1);
                }

                // Receive packets from encoder and write to output file
                while (1) {
                    AVPacket enc_pkt;
                    av_init_packet(&enc_pkt);
                    enc_pkt.data = NULL;
                    enc_pkt.size = 0;

                    // Receive packet from encoder
                    ret = avcodec_receive_packet(encoder_ctx, &enc_pkt);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        av_packet_unref(&enc_pkt);
                        break;
                    } else if (ret < 0) {
                        fprintf(stderr, "Failed to receive packet from encoder");
                        exit(1);
                    }

                    // Write packet to output file
                    if ((ret = av_write_frame(output_ctx, &enc_pkt)) < 0) {
                        fprintf(stderr, "Failed to write packet to output file");
                        exit(1);
                    }

                    av_packet_unref(&enc_pkt);
                }

                av_frame_free(&yuv_frame);
                av_frame_free(&frame);
            }
        }

        av_packet_unref(&packet);
    }

    // Write the output trailer
    if ((ret = av_write_trailer(output_ctx)) < 0) {
        fprintf(stderr, "Failed to write output trailer");
        exit(1);
    }

    // Free resources
    avcodec_free_context(&decoder_ctx);
    avcodec_free_context(&encoder_ctx);
    avformat_close_input(&input_ctx);
    avformat_free_context(input_ctx);
    avformat_free_context(output_ctx);

    return 0;
}