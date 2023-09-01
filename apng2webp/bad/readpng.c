#include <stdio.h>
#include <stdlib.h>
#include <png.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input.apng>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    FILE *in = fopen(input_file, "rb");
    if (!in) {
        fprintf(stderr, "Error opening input file: %s\n", input_file);
        return 1;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "Error creating PNG read struct\n");
        fclose(in);
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fprintf(stderr, "Error creating PNG info struct\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(in);
        return 1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error reading PNG\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(in);
        return 1;
    }

    png_init_io(png_ptr, in);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 num_frames, num_plays;
    if (png_get_acTL(png_ptr, info_ptr, &num_frames, &num_plays) == 0) {
        fprintf(stderr, "Not an animated PNG\n");
    } else {
        printf("Number of frames: %u\n", num_frames);
        printf("Number of plays: %u\n", num_plays);
    }

    fclose(in);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    return 0;
}

