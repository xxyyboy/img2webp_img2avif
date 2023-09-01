#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <webp/encode.h>
#include <webp/mux.h>


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
        frame_picture.argb = (uint32_t *)row_pointers[0];
        frame_picture.argb_stride = width;

        if (!WebPAnimEncoderAdd(enc, &frame_picture, frame * delay_ms, NULL)) {
            fprintf(stderr, "Error adding frame %d to WebP animation\n", frame);
            return -1;
        }

        for (int y = 0; y < height; y++) {
            free(row_pointers[y]);
        }
        free(row_pointers);
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
    //png_set_apng_aware(png_ptr);
    png_read_info(png_ptr, info_ptr);

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
    fprintf(stderr, "Usage: %s input.apng output.webp\n", argv[0]);
    return 1;
  }

  if (apng2webp(argv[1], argv[2]) != 0) {
    fprintf(stderr, "Error! Failed to convert APNG to WebP.\n");
    return 1;
  }

  printf("Successfully converted %s to %s\n", argv[1], argv[2]);
  return 0;
}

