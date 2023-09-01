#include <iostream>
#include <vector>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <webp/encode.h>
#include <webp/mux.h>

using namespace std;

struct Frame {
    std::vector<uint8_t> rgba;
    int width;
    int height;
};

bool load_apng(const char* filename, std::vector<Frame>& frames) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        std::cerr << "Error creating read struct" << std::endl;
        fclose(file);
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        std::cerr << "Error creating info struct" << std::endl;
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(file);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        std::cerr << "Error during libpng init_io" << std::endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(file);
        return false;
    }

    png_init_io(png_ptr, file);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    }

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
    }

    if (bit_depth == 16) {
        png_set_strip_16(png_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_ptr);
    }

    png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    png_read_update_info(png_ptr, info_ptr);

    std::vector<png_bytep> row_pointers(height);
    for (png_uint_32 y = 0; y < height; ++y) {
        row_pointers[y] = (png_byte*)png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));
    }

    png_read_image(png_ptr, row_pointers.data());
    fclose(file);

    Frame frame;
    frame.width = width;
    frame.height = height;
    frame.rgba.resize(width * height * 4);

    for (png_uint_32 y = 0; y < height; ++y) {
        memcpy(frame.rgba.data() + y * width * 4, row_pointers[y], width * 4);
        png_free(png_ptr, row_pointers[y]);
    }

    frames.push_back(frame);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return true;
}

bool save_webp(const char* filename, const std::vector<Frame>& frames) {
    if (frames.empty()) {
        std::cerr << "No frames to save" << std::endl;
        return false;
    }

    const Frame& frame = frames.front();
    uint8_t* output_data;
    size_t output_size = WebPEncodeRGBA(frame.rgba.data(), frame.width, frame.height, frame.width * 4, 75, &output_data);
    if (output_size == 0) {
        std::cerr << "Error encoding WebP image" << std::endl;
        return false;
    }

    FILE* file = fopen(filename, "wb");
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        WebPFree(output_data);
        return false;
    }

    fwrite(output_data, output_size, 1, file); 
    fclose(file); 
    WebPFree(output_data); return true; }

int main(int argc, char* argv[]) { 
    if (argc != 3) { 
        std::cerr << "Usage: " << argv[0] << " input.apng output.webp" << std::endl; 
        return 1; 
    }

    const char* input_filename = argv[1];
    const char* output_filename = argv[2];

    std::vector<Frame> frames;
    if (!load_apng(input_filename, frames)) {
        std::cerr << "Error loading APNG file: " << input_filename << std::endl;
        return 1;
    }

    if (!save_webp(output_filename, frames)) {
        std::cerr << "Error saving WebP file: " << output_filename << std::endl;
        return 1;
    }

    std::cout << "Converted " << input_filename << " to " << output_filename << std::endl;
    return 0;
}