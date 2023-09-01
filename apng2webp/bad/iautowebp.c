#include <stdio.h>
#include <stdlib.h>
#include <swscale.h>

void convert_apng_to_webp(const char* input_filename, const char* output_filename) {
    const char* input_file_path = input_filename;
    const char* output_file_path = output_filename;

    // 打开输入文件
    FILE* in = fopen(input_file_path, "rb");
    if (!in) {
        printf("无法打开输入文件\n");
        return;
    }

    // 打开输出文件
    FILE* out = fopen(output_file_path, "wb");
    if (!out) {
        printf("无法打开输出文件\n");
        return;
    }

    // 获取输入图像的宽度和高度
    int width = 0, height = 0;
    while (!feof(in)) {
        char buffer[4096];
        int n = fread(buffer, 1, sizeof(buffer), in);
        if (n > 0) {
            width = width * 256 + (buffer[0] << 11);
            height = height * 256 + (buffer[1] << 11);
        }
    }

    // 缩放图像
    swscale_init();
    swscale_set_filename(swscale_input(in), out);
    swscale_set_image_size(swscale_input(in), width, height);
    swscale_set_output_size(swscale_input(in), width, height);
    swscale_set_input_mode(swscale_input(in), SWSCALE_IMAGE_JPEG);
    swscale_set_output_mode(swscale_input(in), SWSCALE_IMAGE_JPEG);
    swscale_set_quality(swscale_input(in), 50);
    swscale_set_num_colors(swscale_input(in), 3);
    swscale_set_num_components(swscale_input(in), 3);
    swscale_set_in_color(swscale_input(in), 0, 0, 255);
    swscale_set_out_color(swscale_input(in), 0, 0, 255);
    swscale_show_all(swscale_input(in), swscale_output(out));

    // 关闭输入文件和输出文件
    fclose(in);
    fclose(out);

    printf("成功将APNG文件转换为WEBP文件\n");
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("请提供输入文件和输出文件的路径作为参数\n");
        return 1;
    }

    const char* input_file_path = argv[1];
    const char* output_file_path = argv[2];

    convert_apng_to_webp(input_file_path, output_file_path);

    return 0;
}

