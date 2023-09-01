#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#include <libavutil/mastering_display_metadata.h>

int main(int argc, char * argv[])
{
    if (argc != 3) {
        printf("Usage: %s input.apng output.avif\n", argv[0]);
        return 1;
    }

    const char * input_filename = argv[1];
    const char * output_filename = argv[2];

    AVFormatContext * input_format_ctx = NULL;
    if (avformat_open_input(&input_format_ctx, input_filename, NULL, NULL) != 0) {
        printf("Error: Couldn't open input file\n");
        return 1;
    }

    if (avformat_find_stream_info(input_format_ctx, NULL) < 0) {
        printf("Error: Couldn't find stream info\n");
        return 1;
    }

    int video_stream_index = -1;
    for (int i = 0; i < input_format_ctx->nb_streams; i++) {
        if (input_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        printf("Error: Couldn't find a video stream\n");
        return 1;
    }

    AVCodec * input_codec = avcodec_find_decoder(input_format_ctx->streams[video_stream_index]->codecpar->codec_id);
    if (input_codec == NULL) {
        printf("Error: Unsupported codec\n");
        return 1;
    }

    AVCodecContext * input_codec_ctx = avcodec_alloc_context3(input_codec);
    if (avcodec_parameters_to_context(input_codec_ctx, input_format_ctx->streams[video_stream_index]->codecpar) < 0) {
        printf("Error: Couldn't initialize codec context\n");
        return 1;
    }

    if (avcodec_open2(input_codec_ctx, input_codec, NULL) < 0) {
        printf("Error: Couldn't open codec\n");
        return 1;
    }

    AVFrame * frame = av_frame_alloc();
    AVPacket packet;
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;

    // Set up output context and encoder
    AVFormatContext * output_format_ctx = NULL;
    avformat_alloc_output_context2(&output_format_ctx, NULL, "avif", output_filename);
    if (!output_format_ctx) {
        printf("Error: Couldn't create output context\n");
        return 1;
    }

    AVStream * output_stream = avformat_new_stream(output_format_ctx, NULL);
    if (!output_stream) {
        printf("Error: Couldn't create output stream\n");
        return 1;
    }

    AVCodec * output_codec = avcodec_find_encoder(AV_CODEC_ID_AV1); //AV_CODEC_ID_AV1_IMAGE AV_CODEC_ID_AV1
    if (!output_codec) {
        printf("Error: Couldn't find AV1 encoder\n");
        return 1;
    }

    AVCodecContext * output_codec_ctx = avcodec_alloc_context3(output_codec);
    output_codec_ctx->width = input_codec_ctx->width;
    output_codec_ctx->height = input_codec_ctx->height;
    output_codec_ctx->time_base = (AVRational) { 1, 1 };
    output_codec_ctx->framerate = input_codec_ctx->framerate;
    output_codec_ctx->bit_rate = 1000000;
    output_codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    output_codec_ctx->color_range = AVCOL_RANGE_MPEG;

    // Set color properties for the output codec context
    output_codec_ctx->color_primaries = AVCOL_PRI_BT709;
    output_codec_ctx->color_trc = AVCOL_TRC_IEC61966_2_1;
    output_codec_ctx->colorspace = AVCOL_SPC_BT709;

    av_opt_set(output_codec_ctx->priv_data, "crf", "10", 0);
    av_opt_set(output_codec_ctx->priv_data, "preset", "slow", 0);
    av_opt_set(output_codec_ctx->priv_data, "forced-idr", "1", 0);
    av_opt_set(output_codec_ctx->priv_data, "still_picture", "1", 0);

    AVDictionary * codec_options = NULL;
    av_dict_set(&codec_options, "strict", "experimental", 0);
    av_dict_set(&codec_options, "major_brand", "avif", 0);
    av_dict_set(&codec_options, "minor_version", "0", 0);
    av_dict_set(&codec_options, "compatible_brands", "avif", 0);

    if (avcodec_open2(output_codec_ctx, output_codec, &codec_options) < 0) {
        printf("Error: Couldn't open output codec\n");
        return 1;
    }

    if (avcodec_parameters_from_context(output_stream->codecpar, output_codec_ctx) < 0) {
        printf("Error: Couldn't initialize output stream codec parameters\n");
        return 1;
    }

    output_stream->time_base = output_codec_ctx->time_base;
    // Set color properties for the output stream
    output_stream->codecpar->color_primaries = output_codec_ctx->color_primaries;
    output_stream->codecpar->color_trc = output_codec_ctx->color_trc;
    output_stream->codecpar->color_space = output_codec_ctx->colorspace;
    output_stream->codecpar->color_range = output_codec_ctx->color_range;


    // Open output file
    if (!(output_format_ctx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&output_format_ctx->pb, output_filename, AVIO_FLAG_WRITE) < 0) {
            printf("Error: Couldn't open output file\n");
            return 1;
        }
    }

    // Write file header
    if (avformat_write_header(output_format_ctx, &codec_options) < 0) {
        printf("Error: Couldn't write output file header\n");
        return 1;
    }

    struct SwsContext * sws_ctx = sws_getContext(input_codec_ctx->width,
                                                 input_codec_ctx->height,
                                                 input_codec_ctx->pix_fmt,
                                                 output_codec_ctx->width,
                                                 output_codec_ctx->height,
                                                 output_codec_ctx->pix_fmt,
                                                 SWS_BILINEAR,
                                                 NULL,
                                                 NULL,
                                                 NULL);

    AVFrame * output_frame = av_frame_alloc();
    output_frame->format = output_codec_ctx->pix_fmt;
    output_frame->width = output_codec_ctx->width;
    output_frame->height = output_codec_ctx->height;
    av_frame_get_buffer(output_frame, 0);

    // Read and process frames from input file
    // Read and process frames from input file
    int64_t current_pts = 0;
    int64_t output_pts = 0;
    int end_of_stream = 0;
    while (!end_of_stream) {
        if (!avcodec_is_open(input_codec_ctx)) {
            break;
        }

        int ret = av_read_frame(input_format_ctx, &packet);
        if (ret < 0) {
            packet.data = NULL;
            packet.size = 0;
            end_of_stream = 1;
        }

        if (end_of_stream || packet.stream_index == video_stream_index) {
            int response = avcodec_send_packet(input_codec_ctx, &packet);
            if (response < 0 && response != AVERROR_EOF) {
                printf("Error: Couldn't send packet\n");
                return 1;
            }

            while (response >= 0) {
                response = avcodec_receive_frame(input_codec_ctx, frame);
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                    break;
                } else if (response < 0) {
                    printf("Error: Couldn't receive frame\n");
                    return 1;
                }

                // Convert frame to YUV420P
                sws_scale(sws_ctx,
                          (const uint8_t * const *)frame->data,
                          frame->linesize,
                          0,
                          input_codec_ctx->height,
                          output_frame->data,
                          output_frame->linesize);

                // Set proper pts for the output frame
                output_frame->pts = av_rescale_q(frame->pts, input_codec_ctx->time_base, output_codec_ctx->time_base);

                // Encode frame
                AVPacket output_packet;
                av_init_packet(&output_packet);
                output_packet.data = NULL;
                output_packet.size = 0;

                if (avcodec_send_frame(output_codec_ctx, output_frame) < 0) {
                    printf("Error: Couldn't send output frame\n");
                    return 1;
                }

                while (1) {
                    ret = avcodec_receive_packet(output_codec_ctx, &output_packet);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        break;
                    } else if (ret < 0) {
                        printf("Error: Couldn't receive output packet (error code: %d)\n", ret);
                        return 1;
                    }

                    // Set proper timestamps for the output packet
                    output_packet.pts = output_pts;
                    output_packet.dts = output_pts;
                    output_pts++;

                    output_packet.stream_index = output_stream->index;
                    av_interleaved_write_frame(output_format_ctx, &output_packet);
                }
            }
        }
    }

    // Flush the encoder
    avcodec_send_frame(output_codec_ctx, NULL);
    while (1) {
        AVPacket output_packet;
        av_init_packet(&output_packet);
        output_packet.data = NULL;
        output_packet.size = 0;

        int ret = avcodec_receive_packet(output_codec_ctx, &output_packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            printf("Error: Couldn't receive output packet (error code: %d)\n", ret);
            return 1;
        }

        // Set proper timestamps for the output packet
        output_packet.pts = output_pts;
        output_packet.dts = output_pts;
        output_pts++;

        output_packet.stream_index = output_stream->index;
        av_interleaved_write_frame(output_format_ctx, &output_packet);
    }

    av_write_trailer(output_format_ctx);

    if (!(output_format_ctx->oformat->flags & AVFMT_NOFILE)) {
        avio_closep(&output_format_ctx->pb);
    }

    avformat_free_context(output_format_ctx);
    av_frame_free(&output_frame);
    av_frame_free(&frame);
    avcodec_close(input_codec_ctx);
    avcodec_free_context(&input_codec_ctx);
    avcodec_close(output_codec_ctx);
    avcodec_free_context(&output_codec_ctx);
    avformat_close_input(&input_format_ctx);

    return 0;
}