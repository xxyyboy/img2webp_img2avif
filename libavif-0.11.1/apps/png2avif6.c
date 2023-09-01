#include <avif/avif.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <stdio.h>

void init_ffmpeg()
{
    av_register_all();
    avcodec_register_all();
}

AVFrame * decode_apng_frame(AVCodecContext * codec_ctx, AVPacket * pkt)
{
    AVFrame * frame = av_frame_alloc();
    int ret = avcodec_send_packet(codec_ctx, pkt);
    if (ret < 0) {
        av_frame_free(&frame);
        return NULL;
    }
    ret = avcodec_receive_frame(codec_ctx, frame);
    if (ret < 0) {
        av_frame_free(&frame);
        return NULL;
    }
    return frame;
}

int main(int argc, char * argv[])
{
    if (argc < 3) {
        printf("Usage: %s <input.apng> <output.avif>\n", argv[0]);
        return 1;
    }

    init_ffmpeg();

    // Read input APNG file
    AVFormatContext * format_ctx = NULL;
    if (avformat_open_input(&format_ctx, argv[1], NULL, NULL) < 0) {
        printf("Error: Could not open input file.\n");
        return 1;
    }
    avformat_find_stream_info(format_ctx, NULL);
    int video_stream_index = -1;
    for (int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1) {
        printf("Error: Could not find video stream in input file.\n");
        return 1;
    }

    // Prepare APNG decoder
    AVCodec * apng_decoder = avcodec_find_decoder(format_ctx->streams[video_stream_index]->codecpar->codec_id);
    AVCodecContext * apng_decoder_ctx = avcodec_alloc_context3(apng_decoder);
    avcodec_parameters_to_context(apng_decoder_ctx, format_ctx->streams[video_stream_index]->codecpar);
    avcodec_open2(apng_decoder_ctx, apng_decoder, NULL);

    // Prepare AVIF encoder
    avifEncoder * avif_encoder = avifEncoderCreate();
    avif_encoder->codecChoice = AVIF_CODEC_CHOICE_AOM;
    avif_encoder->minQuantizer = 10;
    avif_encoder->maxQuantizer = 10;
    avif_encoder->tileRowsLog2 = 0;
    avif_encoder->tileColsLog2 = 0;
    avif_encoder->speed = 6;

    avifImage * avif_image = avifImageCreate(apng_decoder_ctx->width, apng_decoder_ctx->height, 8, AVIF_PIXEL_FORMAT_YUV420);

    AVPacket pkt;
    int frame_index = 0;
    while (av_read_frame(format_ctx, &pkt) >= 0) {
        if (pkt.stream_index == video_stream_index) {
            AVFrame * frame = decode_apng_frame(apng_decoder_ctx, &pkt);
            if (frame) {
                struct SwsContext * sws_ctx = sws_getContext(frame->width,
                                                             frame->height,
                                                             apng_decoder_ctx->pix_fmt,
                                                             frame->width,
                                                             frame->height,
                                                             AV_PIX_FMT_YUV420P,
                                                             SWS_BILINEAR,
                                                             NULL,
                                                             NULL,
                                                             NULL);

                avifImageAllocatePlanes(avif_image, AVIF_PLANES_YUV);

                sws_scale(sws_ctx,
                          (const uint8_t * const *)frame->data,
                          frame->linesize,
                          0,
                          frame->height,
                          avif_image->yuvPlanes,
                          avif_image->yuvRowBytes);

                sws_freeContext(sws_ctx);

                // Encode the AVIF image
                avifEncoderAddImage(avif_encoder, avif_image, 1, AVIF_ADD_IMAGE_FLAG_SINGLE);

                av_frame_free(&frame);
                frame_index++;
            }
        }
        av_packet_unref(&pkt);
    }

    // Finish encoding and write the AVIF data to the output file
    avifRWData avif_data = AVIF_DATA_EMPTY;
    avifResult result = avifEncoderFinish(avif_encoder, &avif_data);
    if (result != AVIF_RESULT_OK) {
        printf("Error: Could not finish encoding AVIF image: %s\n", avifResultToString(result));
        return 1;
    }
    FILE * output_file = fopen(argv[2], "wb");
    if (!output_file) {
        printf("Error: Could not open output file for writing.\n");
        return 1;
    }
    fwrite(avif_data.data, 1, avif_data.size, output_file);
    fclose(output_file);
    // Clean up
    avifRWDataFree(&avif_data);
    avifEncoderDestroy(avif_encoder);
    avifImageDestroy(avif_image);
    avcodec_free_context(&apng_decoder_ctx);
    avformat_close_input(&format_ctx);

    return 0;
}