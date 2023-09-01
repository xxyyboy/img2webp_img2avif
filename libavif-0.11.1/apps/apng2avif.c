#include <avif/avif.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
    if (argc < 3) {
        printf("Usage: %s input.png output.avif\n", argv[0]);
        return 1;
    }

    const char * input_file = argv[1];
    const char * output_file = argv[2];

    // Open input file
    FILE * fp = fopen(input_file, "rb");
    if (!fp) {
        printf("Error: Cannot open input file %s\n", input_file);
        return 1;
    }

    // Read PNG header
    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        printf("Error: %s is not a valid PNG file\n", input_file);
        fclose(fp);
        return 1;
    }

    // Initialize PNG structures
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        printf("Error: Cannot initialize PNG read structure\n");
        fclose(fp);
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        printf("Error: Cannot initialize PNG info structure\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return 1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("Error: Cannot read PNG file\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return 1;
    }

    // Set up PNG input
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    // Read PNG frames
    int num_frames = png_set_num_frames(png_ptr, info_ptr);
    png_bytep * frames = (png_bytep *)malloc(num_frames * sizeof(png_bytep));
    for (int i = 0; i < num_frames; i++) {
        png_bytep frame = NULL;
        png_uint_32 frame_width, frame_height;
        int frame_bit_depth, frame_color_type, frame_interlace_type;

        png_read_frame_head(png_ptr, info_ptr);
        png_get_IHDR(png_ptr, info_ptr, &frame_width, &frame_height, &frame_bit_depth, &frame_color_type, &frame_interlace_type, NULL, NULL);
        frame = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr) * frame_height);
        png_read_image(png_ptr, &frame);
        frames[i] = frame;
    }

    // Close input file
    fclose(fp);

    // Initialize AVIF structures
    avifRWData raw = AVIF_DATA_EMPTY;

    avifImage * avif = avifImageCreateEmpty();
    avif->width = png_get_image_width(png_ptr, info_ptr);
    avif->height = png_get_image_height(png_ptr, info_ptr);
    avif->depth = 8;
    avif->yuvFormat = AVIF_PIXEL_FORMAT_YUV420;
    avifImageAllocatePlanes(avif, AVIF_PLANES_YUV);

    // Create AVIF encoder
    avifEncoder * encoder = avifEncoderCreate();
    //avifEncoderSetCodec(encoder, AVIF_CODEC_AV1);
    avifEncoderSetYUVFormat(encoder, avif->yuvFormat);
    avifEncoderSetQuality(encoder, 50.0f);

//    avifAddImageFlag
    avifEncoderAddImageFlags(encoder, AVIF_ADD_IMAGE_FLAG_FORCE_KEYFRAME);

    // Encode each frame
    for (int i = 0; i < num_frames; i++) {
        AVIF_API avifImage * frame = avifImageCreateEmpty();
        frame->width = avif->width;
        frame->height = avif->height;
        frame->depth = avif->depth;
        frame->yuvFormat = avif->yuvFormat;
        avifImageAllocatePlanes(frame, AVIF_PLANES_YUV);

        // Set frame data
        avifImageSetProfileICC(frame, NULL, 0);
        avifImageSetMetadataExif(frame, NULL, 0);
        avifImageSetMetadataXMP(frame, NULL, 0);
        avifImageSetMetadataICCP(frame, NULL, 0);
        avifImageSetMetadataIPCT(frame, NULL, 0);
        avifImageSetMetadataID32(frame, NULL, 0);
        avifImageSetMetadataCustom(frame, NULL, 0);

        avifRGBImage rgb;
        avifRGBImageSetDefaults(&rgb, frame);
        rgb.format = AVIF_RGB_FORMAT_RGBA;
        rgb.depth = 8;
        rgb.width = frame->width;
        rgb.height = frame->height;
        rgb.rowBytes = rgb.width * 4;
        rgb.pixels = frames[i];

        avifImageRGBToYUV(frame, &rgb);

        // Encode frame
        avifResult result = avifEncoderWriteFrame(encoder, avif, frame);
        if (result != AVIF_RESULT_OK) {
            printf("Error: Cannot encode frame %d\n", i);
            avifImageDestroy(frame);
            avifEncoderDestroy(encoder);
            avifImageDestroy(avif);
            free(frames);
            return 1;
        }

        avifImageDestroy(frame);
    }

    // Finalize encoding
    avifResult result = avifEncoderFinish(encoder, &raw);
    if (result != AVIF_RESULT_OK) {
        printf("Error: Cannot finalize encoding\n");
        avifEncoderDestroy(encoder);
        avifImageDestroy(avif);
        free(frames);
        return 1;
    }

    // Write output file
    FILE * out = fopen(output_file, "wb");
    if (!out) {
        printf("Error: Cannot open output file %s\n", output_file);
        avifEncoderDestroy(encoder);
        avifImageDestroy(avif);
        free(frames);
        return 1;
    }

    fwrite(raw.data, 1, raw.size, out);
    fclose(out);

    // Clean up
    avifEncoderDestroy(encoder);
    avifImageDestroy(avif);
    for (int i = 0; i < num_frames; i++) {
        free(frames[i]);
    }
    free(frames);

    return 0;
}