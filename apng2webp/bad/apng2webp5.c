#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <webp/encode.h>
#include <webp/mux.h>


void rgba_to_argb(png_structp png_ptr, png_row_infop row_info, png_bytep data) {
    for (int x = 0; x < row_info->rowbytes; x += 4) {
        png_byte r = data[x];
        png_byte g = data[x + 1];
        png_byte b = data[x + 2];
        png_byte a = data[x + 3];
        data[x] = a;
        data[x + 1] = r;
        data[x + 2] = g;
        data[x + 3] = b;
    }
}

int process_frames(png_structp png_ptr, png_infop info_ptr, WebPAnimEncoder *enc) {
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    int num_frames = png_get_num_frames(png_ptr, info_ptr);
    png_uint_16 delay_num, delay_den;
    uint32_t *prev_frame_data = NULL;
    int timestamp_ms = 0;

    for (int frame = 0; frame < num_frames; frame++) {
        png_byte dispose_op = png_get_next_frame_dispose_op(png_ptr, info_ptr);
        png_byte blend_op = png_get_next_frame_blend_op(png_ptr, info_ptr);

        png_read_frame_head(png_ptr, info_ptr);
        delay_num = png_get_next_frame_delay_num(png_ptr, info_ptr);
        delay_den = png_get_next_frame_delay_den(png_ptr, info_ptr);
        int delay_ms = delay_num * 1000 / (delay_den ? delay_den : 100);

        png_bytepp row_pointers = (png_bytepp)malloc(sizeof(png_bytep) * height);
        for (int y = 0; y < height; y++) {
            row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
        }

        // Add this line to handle interlaced PNG images
        
        //png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);
        png_read_image(png_ptr, row_pointers);

        // If the blend operation is PNG_BLEND_OP_SOURCE and we have a previous frame,
        // we need to copy the previous frame data before updating with the current frame.
        if (blend_op == PNG_BLEND_OP_SOURCE && prev_frame_data != NULL) {
            for (int y = 0; y < height; y++) {
                memcpy(row_pointers[y], prev_frame_data + y * width * 4, width * 4);
            }
        }

        WebPPicture frame_picture;
        WebPPictureInit(&frame_picture);
        frame_picture.width = width;
        frame_picture.height = height;
        frame_picture.argb = (uint32_t *)malloc(width * height * 4);
        frame_picture.argb_stride = width;

        // Copy row data from row_pointers to frame_picture.argb with RGBA to ARGB conversion
        for (int y = 0; y < height; y++) {
            uint32_t *dst = frame_picture.argb + y * width;
            png_bytep src = row_pointers[y];
            for (int x = 0; x < width; x++) {
                uint32_t r = *src++;
                uint32_t g = *src++;
                uint32_t b = *src++;
                uint32_t a = *src++;
                dst[x] = (a << 24) | (r << 16) | (g << 8) | b;
            }
            free(row_pointers[y]);
        }
        free(row_pointers);

        if (!WebPAnimEncoderAdd(enc, &frame_picture, timestamp_ms, NULL)) {
            fprintf(stderr, "Error adding frame %d to WebP animation\n", frame);
            WebPPictureFree(&frame_picture);
            if (prev_frame_data != NULL) {
                free(prev_frame_data);
            }
            return -1;
        }

        // Update the timestamp
        timestamp_ms += delay_ms;

        // If the dispose operation is PNG_DISPOSE_OP_NONE, we need to store the current
        // frame data for blending with the next frame.
        if (dispose_op == PNG_DISPOSE_OP_NONE) {
            if (prev_frame_data == NULL) {
                prev_frame_data = (uint32_t *)malloc(width * height * 4);
            }
            memcpy(prev_frame_data, frame_picture.argb, width * height * 4);
        } else if (prev_frame_data != NULL) {
            free(prev_frame_data);
            prev_frame_data = NULL;
        }

        WebPPictureFree(&frame_picture);
    }

    if (prev_frame_data != NULL) {
        free(prev_frame_data);
    }

    return 0;
}

int apng2webp(const char *input, const char *output) {
    FILE *fp = fopen(input, "rb");
    if (!fp) {
        fprintf(stderr, "Error opening input file: %s\n", input);
        return -1;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "Error creating png read struct\n");
        fclose(fp);
        return -1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fprintf(stderr, "Error creating png info struct\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error reading png file\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return -1;
    }

    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);

    png_set_expand(png_ptr);
    png_set_gray_to_rgb(png_ptr);
    png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    png_set_scale_16(png_ptr);
    png_set_read_user_transform_fn(png_ptr, rgba_to_argb);

    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);

    WebPAnimEncoderOptions enc_options;
    WebPAnimEncoderOptionsInit(&enc_options);
    enc_options.anim_params.loop_count = 0;
    WebPAnimEncoder *enc = WebPAnimEncoderNew(width, height, &enc_options);
    if (!enc) {
        fprintf(stderr, "Error creating WebP animation encoder\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return -1;
    }

    if (process_frames(png_ptr, info_ptr, enc) != 0) {
        WebPAnimEncoderDelete(enc);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return -1;
    }

    fclose(fp);

    WebPData webp_data;
    WebPDataInit(&webp_data);
    if (!WebPAnimEncoderAssemble(enc, &webp_data)) {
        fprintf(stderr, "Error assembling WebP animation\n");
        WebPAnimEncoderDelete(enc);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return -1;
    }

    fp = fopen(output, "wb");
    if (!fp) {
        fprintf(stderr, "Error opening output file: %s\n", output);
        WebPDataClear(&webp_data);
        WebPAnimEncoderDelete(enc);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return -1;
    }

    fwrite(webp_data.bytes, 1, webp_data.size, fp);
    fclose(fp);

    WebPDataClear(&webp_data);
    WebPAnimEncoderDelete(enc);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.apng> <output.webp>\n", argv[0]);
        return 1;
    }

    return apng2webp(argv[1], argv[2]);
}

