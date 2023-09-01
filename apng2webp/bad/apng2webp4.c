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

    for (int frame = 0; frame < num_frames; frame++) {
        png_read_frame_head(png_ptr, info_ptr);
        delay_num = png_get_next_frame_delay_num(png_ptr, info_ptr);
        delay_den = png_get_next_frame_delay_den(png_ptr, info_ptr);
        int delay_ms = delay_num * 1000 / (delay_den ? delay_den : 100);

        png_bytepp row_pointers = (png_bytepp)malloc(sizeof(png_bytep) * height);
        for (int y = 0; y < height; y++) {
            row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
        }

        png_read_image(png_ptr, row_pointers);

        WebPPicture frame_picture;
        WebPPictureInit(&frame_picture);
        frame_picture.width = width;
        frame_picture.height = height;
        frame_picture.argb = (uint32_t *)malloc(width * height * 4);
        frame_picture.argb_stride = width;

        for (int y = 0; y < height; y++) {
            memcpy(frame_picture.argb + y * width, row_pointers[y], width * 4);
            free(row_pointers[y]);
        }
        free(row_pointers);

        if (!WebPAnimEncoderAdd(enc, &frame_picture, frame * delay_ms, NULL)) {
            fprintf(stderr, "Error adding frame %d to WebP animation\n", frame);
            WebPPictureFree(&frame_picture);
            return -1;
        }

        WebPPictureFree(&frame_picture);
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

