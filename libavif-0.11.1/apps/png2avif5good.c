/* kenyuan 20230803 */

#include <avif/avif.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <stdio.h>

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

AVPacket * encode_avif_frame(AVCodecContext * codec_ctx, AVFrame * frame)
{
    AVPacket * pkt = av_packet_alloc();
    int ret = avcodec_send_frame(codec_ctx, frame);
    if (ret < 0) {
        av_packet_free(&pkt);
        return NULL;
    }
    ret = avcodec_receive_packet(codec_ctx, pkt);
    if (ret < 0) {
        av_packet_free(&pkt);
        return NULL;
    }
    return pkt;
}

int main(int argc, char * argv[])
{
    if (argc < 3) {
        printf("Usage: %s <input.apng> <output.avif>\n", argv[0]);
        return 1;
    }

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

    // Get APNG timescale
    int64_t timescale = format_ctx->streams[video_stream_index]->time_base.den;

    // Prepare APNG decoder
    AVCodec * apng_decoder = avcodec_find_decoder(format_ctx->streams[video_stream_index]->codecpar->codec_id);
    AVCodecContext * apng_decoder_ctx = avcodec_alloc_context3(apng_decoder);
    avcodec_parameters_to_context(apng_decoder_ctx, format_ctx->streams[video_stream_index]->codecpar);
    avcodec_open2(apng_decoder_ctx, apng_decoder, NULL);

    // Prepare AVIF encoder
    avifEncoder * avif_encoder = avifEncoderCreate();
    avif_encoder->maxThreads = 1;
    avif_encoder->minQuantizer = 0;
    avif_encoder->maxQuantizer = 0;
    avif_encoder->minQuantizerAlpha = 0;
    avif_encoder->maxQuantizerAlpha = 0;
    avif_encoder->tileRowsLog2 = 0;
    avif_encoder->tileColsLog2 = 0;
    avif_encoder->codecChoice = AVIF_CODEC_CHOICE_AUTO;
    avif_encoder->timescale = timescale;
    avif_encoder->keyframeInterval = 0;

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
                                                             AV_PIX_FMT_YUVA420P,
                                                             SWS_BILINEAR,
                                                             NULL,
                                                             NULL,
                                                             NULL);

                AVFrame * yuv_frame = av_frame_alloc();
                yuv_frame->format = AV_PIX_FMT_YUVA420P;
                yuv_frame->width = frame->width;
                yuv_frame->height = frame->height;
                av_frame_get_buffer(yuv_frame, 32);

                sws_scale(sws_ctx,
                          (const uint8_t * const *)frame->data,
                          frame->linesize,
                          0,
                          frame->height,
                          yuv_frame->data,
                          yuv_frame->linesize);

                sws_freeContext(sws_ctx);

                // Create an AVIF image from the YUV frame
                avifImage * avif_image = avifImageCreate(yuv_frame->width, yuv_frame->height, 8, AVIF_PIXEL_FORMAT_YUV420);
                avifImageAllocatePlanes(avif_image, AVIF_PLANES_YUV | AVIF_PLANES_A);

                // Copy the YUV frame data into the AVIF image
                for (int plane = 0; plane < 3; ++plane) {
                    uint8_t * src = yuv_frame->data[plane];
                    uint8_t * dst = avif_image->yuvPlanes[plane];
                    int src_stride = yuv_frame->linesize[plane];
                    int dst_stride = avif_image->yuvRowBytes[plane];
                    int plane_height = (plane == 0) ? yuv_frame->height : (yuv_frame->height + 1) / 2;

                    for (int y = 0; y < plane_height; ++y) {
                        memcpy(dst, src, dst_stride);
                        src += src_stride;
                        dst += dst_stride;
                    }
                }

                // Check if the original APNG frame has an alpha channel
                if (apng_decoder_ctx->pix_fmt == AV_PIX_FMT_RGBA) {
                    // Copy the alpha data from the APNG frame to the AVIF image
                    uint8_t *src_alpha = yuv_frame->data[3];
                    uint8_t *dst_alpha = avif_image->alphaPlane;
                    int src_alpha_stride = yuv_frame->linesize[3];
                    int dst_alpha_stride = avif_image->alphaRowBytes;

                    for (int y = 0; y < yuv_frame->height; ++y) {
                        memcpy(dst_alpha, src_alpha, dst_alpha_stride);
                        src_alpha += src_alpha_stride;
                        dst_alpha += dst_alpha_stride; 
                    } 
                } 
                else { // Fill the alpha channel with the original APNG frame's background color 
                    uint8_t *dst_alpha   = avif_image->alphaPlane; 
                    int dst_alpha_stride = avif_image->alphaRowBytes;
                    for (int y = 0; y < yuv_frame->height; ++y) {
                        memset(dst_alpha, 255, dst_alpha_stride);
                        dst_alpha += dst_alpha_stride;
                    }
                }

                // Add the AVIF image to the encoder
                uint64_t duration = frame->duration * timescale / format_ctx->streams[video_stream_index]->time_base.den;
                avifResult result = avifEncoderAddImage(avif_encoder, avif_image, duration, AVIF_ADD_IMAGE_FLAG_NONE);

                if (result != AVIF_RESULT_OK) {
                    printf("Error: Could not add AVIF image: %s\n", avifResultToString(result));
                    return 1;
                }

                avifImageDestroy(avif_image);
                av_frame_free(&yuv_frame);
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
        printf("Error: Could not finish encoding AVIF image sequence: %s\n", avifResultToString(result));
        return 1;
    }

    FILE * output_file = fopen(argv[2], "wb");
    fwrite(avif_data.data, 1, avif_data.size, output_file);
    fclose(output_file);

    // Clean upS
    avifRWDataFree(&avif_data);
    avifEncoderDestroy(avif_encoder);
    avcodec_free_context(&apng_decoder_ctx);
    avformat_close_input(&format_ctx);

    return 0;
}
