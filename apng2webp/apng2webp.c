#include <iostream>
#include <vector>
#include <png.h>
#include <webp/encode.h>
#include <webp/mux.h>
#include <string.h>
#include <math.h>

struct Frame {
    std::vector<uint8_t> rgba;
    int width;
    int height;
    png_uint_32 x_offset, y_offset, sub_width, sub_height;
    png_uint_16 delay_num, delay_den;
    png_byte dispose_op, blend_op;
    int duration;  // Frame duration in milliseconds
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
    png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);

    png_read_update_info(png_ptr, info_ptr);

    png_uint_32 num_frames = png_get_num_frames(png_ptr, info_ptr);
    frames.reserve(num_frames);

    uint32_t first = (png_get_first_frame_is_hidden(png_ptr, info_ptr) != 0) ? 1 : 0;

    std::vector<uint8_t> prev_frame_rgba(width * height * 4, 0);
    std::vector<uint8_t> background(width * height * 4, 0);

    png_byte dispose_op=0, blend_op=0;


 for (png_uint_32 frame_num = 0; frame_num < num_frames; ++frame_num) {
        png_read_frame_head(png_ptr, info_ptr);

        png_uint_32 x_offset, y_offset, sub_width, sub_height;
        png_uint_16 delay_num, delay_den;
        
        png_get_next_frame_fcTL(png_ptr, info_ptr, &sub_width, &sub_height, &x_offset, &y_offset,
                                &delay_num, &delay_den, &dispose_op, &blend_op);

        if (frame_num == first) {
            blend_op = PNG_BLEND_OP_SOURCE;
            if (dispose_op == PNG_DISPOSE_OP_PREVIOUS) {
                dispose_op = PNG_DISPOSE_OP_BACKGROUND;
            }
        }

        Frame frame;
        frame.width = width;
        frame.height = height;
        frame.x_offset = x_offset;
        frame.y_offset = y_offset;
        frame.dispose_op = dispose_op;
        frame.blend_op = blend_op;

        if (delay_den == 0) {
            delay_den = 100;
        }
        frame.duration = ceil(1000 * (double)delay_num / delay_den);
        //frame.duration = delay_num * 1000 / (delay_den>0 ? delay_den : 100);  // Convert to milliseconds
        frame.rgba.resize(width * height * 4, 0);

        std::cout << "Processing frame " << frame_num << std::endl;
        std::cout << "  x_offset: " << x_offset << std::endl;
        std::cout << "  y_offset: " << y_offset << std::endl;
        std::cout << "  sub_width: " << sub_width << std::endl;
        std::cout << "  sub_height: " << sub_height << std::endl;
        std::cout << "  delay_num: " << delay_num << std::endl;
        std::cout << "  delay_den: " << delay_den << std::endl;
        std::cout << "  dispose_op: " << static_cast<int>(dispose_op) << std::endl;
        std::cout << "  blend_op: " << static_cast<int>(blend_op) << std::endl;
        std::cout << "  duration: " << frame.duration << std::endl;
        
        std::vector<png_bytep> row_pointers(sub_height);

        for (png_uint_32 y = 0; y < sub_height; ++y) {
            row_pointers[y] = frame.rgba.data() + (y + y_offset) * width * 4 + x_offset * 4;
        }

        // Read fdAT or IDAT
        png_read_image(png_ptr, row_pointers.data());

        if (frame_num > 0) {
            if (frames[frame_num - 1].dispose_op == PNG_DISPOSE_OP_PREVIOUS) {
                memcpy(frame.rgba.data(), prev_frame_rgba.data(), width * height * 4);
            } else if (frames[frame_num - 1].dispose_op == PNG_DISPOSE_OP_BACKGROUND) {
                memcpy(frame.rgba.data(), background.data(), width * height * 4);
            }
        }

        // Blend and dispose operations
        if (frame.blend_op == PNG_BLEND_OP_SOURCE) {
            for (png_uint_32 y = 0; y < sub_height; ++y) {
                memcpy(&frame.rgba[(y + y_offset) * width * 4 + x_offset * 4],
                    row_pointers[y], sub_width * 4);
            }
        } else if (frame.blend_op == PNG_BLEND_OP_OVER) {
            for (png_uint_32 y = 0; y < sub_height; ++y) {
                for (png_uint_32 x = 0; x < sub_width; ++x) {
                    size_t src_offset = y * sub_width * 4 + x * 4;
                    size_t dst_offset = (y + y_offset) * width * 4 + (x + x_offset) * 4;

                    uint8_t src_alpha = row_pointers[y][src_offset + 3];
                    uint8_t dst_alpha = frame.rgba[dst_offset + 3];
                    uint8_t out_alpha = src_alpha + dst_alpha * (255 - src_alpha) / 255;

                    if (out_alpha != 0) {
                        for (int c = 0; c < 3; ++c) {
                            frame.rgba[dst_offset + c] =
                                (row_pointers[y][src_offset + c] * src_alpha +
                                frame.rgba[dst_offset + c] * dst_alpha * (255 - src_alpha) / 255) /
                                out_alpha;
                        }
                    }
                    frame.rgba[dst_offset + 3] = out_alpha;
                }
            }
        }

        if (frame.dispose_op == PNG_DISPOSE_OP_PREVIOUS) {
            prev_frame_rgba = std::vector<uint8_t>(frame.rgba);
        } else if (frame.dispose_op == PNG_DISPOSE_OP_BACKGROUND) {
            for (png_uint_32 y = 0; y < sub_height; ++y) {
                for (png_uint_32 x = 0; x < sub_width; ++x) {
                    size_t offset = (y + y_offset) * width * 4 + (x + x_offset) * 4;
                    if (frame.rgba[offset + 3] == 0) {
                        memset(&frame.rgba[offset], 0, 4);
                    }
                }
            }
        }


        frames.push_back(frame);
    }

    fclose(file);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return true;
}

bool save_webp(const char* filename, const std::vector<Frame>& frames) {
    if (frames.empty()) {
        std::cerr << "No frames to save" << std::endl;
        return false;
    }

    WebPAnimEncoderOptions enc_options;
    WebPAnimEncoderOptionsInit(&enc_options);
    enc_options.anim_params.loop_count = 0;  // Infinite loop

    WebPAnimEncoder* enc = WebPAnimEncoderNew(frames.front().width, frames.front().height, &enc_options);
    if (!enc) {
        std::cerr << "Error creating WebP anim encoder" << std::endl;
        return false;
    }

    int timestamp = 0;
    for (size_t i = 0; i < frames.size(); ++i) {
        const Frame& frame = frames[i];

        WebPPicture picture;
        if (!WebPPictureInit(&picture)) {
            std::cerr << "Error initializing WebPPicture" << std::endl;
            WebPAnimEncoderDelete(enc);
            return false;
        }

        picture.width = frame.width;
        picture.height = frame.height;
        picture.use_argb = 1;
        picture.argb_stride = frame.width;

        if (!WebPPictureImportRGBA(&picture, &frame.rgba[0], frame.width * 4)) {
            std::cerr << "Error importing RGBA data to WebPPicture" << std::endl;
            WebPPictureFree(&picture);
            WebPAnimEncoderDelete(enc);
            return false;
        }
        
        if (!WebPAnimEncoderAdd(enc, &picture, timestamp, NULL)) {
            std::cerr << "Error adding frame to WebP anim encoder"
                      << std::endl;
            WebPPictureFree(&picture);
            WebPAnimEncoderDelete(enc);
            return false;
        }
        else{
            timestamp += frame.duration;
        }
        WebPPictureFree(&picture);
    }

    WebPData webp_data;
    WebPDataInit(&webp_data);
    if (!WebPAnimEncoderAssemble(enc, &webp_data)) {
        std::cerr << "Error assembling WebP animation" << std::endl;
        WebPDataClear(&webp_data);
        WebPAnimEncoderDelete(enc);
        return false;
    }

    FILE* file = fopen(filename, "wb");
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        WebPDataClear(&webp_data);
        WebPAnimEncoderDelete(enc);
        return false;
    }

    fwrite(webp_data.bytes, webp_data.size, 1, file);
    fclose(file);

    WebPDataClear(&webp_data);
    WebPAnimEncoderDelete(enc);
    return true;
}

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

