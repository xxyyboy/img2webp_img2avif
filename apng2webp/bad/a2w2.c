#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <png.h>
#include <webp/encode.h>

using namespace std;

// APNG帧数据结构
struct APNGFrame
{
    png_bytep buffer;          // 帧数据
    png_uint_32 width;         // 帧宽度
    png_uint_32 height;        // 帧高度
    png_uint_16 delay_num;     // 帧延迟数值分子
    png_uint_16 delay_den;     // 帧延迟数值分母
    png_byte bit_depth;        // 帧位深度
    png_byte color_type;       // 帧颜色类型
    png_colorp palette;         // 调色板
    int palette_size;  // 调色板大小
    png_bytep trans_alpha;     // 透明度通道
    png_bytep row_pointers[4]; // 行指针
};

// APNG帧控制信息
struct APNGFrameControl {
    png_uint_32 width;          // 帧宽度
    png_uint_32 height;         // 帧高度
    png_uint_32 x_offset;       // 帧X偏移量
    png_uint_32 y_offset;       // 帧Y偏移量
    png_uint_16 delay_num;      // 帧延迟数值分子
    png_uint_16 delay_den;      // 帧延迟数值分母
    int dispose_op;             // 帧处理方式
    int blend_op;               // 帧混合方式
    bool has_fcTL;              // 是否包含fcTL块
    bool has_fdAT;              // 是否包含fdAT块
    png_uint_32 num_frames;     // 帧序列号
    png_uint_32 num_plays;      // 播放次数
    vector<uint8_t> frame_data; // 帧数据
};

// 读取APNG帧数据
bool read_apng_frame(APNGFrame &frame, png_structp png_ptr, png_infop info_ptr)
{
    // 读取IHDR块
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type, compression_type, filter_method;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_method);
    frame.width = width;
    frame.height = height;
    frame.bit_depth = bit_depth;
    frame.color_type = color_type;
    // 读取PLTE块和tRNS块
    if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
        png_get_PLTE(png_ptr, info_ptr, &frame.palette, &frame.palette_size);
        png_get_tRNS(png_ptr, info_ptr, &frame.trans_alpha, NULL, NULL);
    }
    // 读取IDAT块
    png_size_t row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    frame.buffer = new png_byte[row_bytes * height];
    frame.row_pointers[0] = frame.buffer;
    for (png_uint_32 i = 1; i < height; ++i)
    {
        frame.row_pointers[i] = frame.row_pointers[i - 1] + row_bytes;
    }
    png_read_image(png_ptr, frame.row_pointers);
    // 读取fcTL块
    png_uint_16 delay_num, delay_den;
    png_byte dispose_op, blend_op;
    png_uint_32 x_offset, y_offset;

    if (png_get_next_frame_fcTL(png_ptr, info_ptr,  &frame.width, &frame.height, &x_offset, &y_offset, &frame.delay_num, &frame.delay_den, &dispose_op, &blend_op))
    {
        return true;
    }
    return false;
}

// 释放APNG帧数据
void free_apng_frame(APNGFrame &frame)
{
    delete[] frame.buffer;
    delete[] frame.palette;
    delete[] frame.trans_alpha;
}

// 设置WebP图片像素数据
static void WebPPictureSetARGB(WebPPicture* pic, uint32_t argb, int x, int y) {
    uint8_t a = (argb >> 24) & 0xff;
    uint8_t r = (argb >> 16) & 0xff;
    uint8_t g = (argb >> 8) & 0xff;
    uint8_t b = argb & 0xff;
    uint32_t* dst = pic->argb + 4 * (y * pic->argb_stride + x);
    dst[0] = b;
    dst[1] = g;
    dst[2] = r;
    dst[3] = a;
}

// 将APNG帧数据转换为WEBP格式
bool apng_frame_to_webp(const APNGFrame &frame, vector<uint8_t> &webp_data)
{
    // 初始化WebP编码器
    WebPConfig config;
    WebPConfigInit(&config);
    config.lossless = false;
    config.quality = 75;
    config.method = 4;
    config.target_size = 0;
    config.target_PSNR = 0;
    config.segments = 4;
    config.sns_strength = 50;
    config.filter_strength = 60;
    config.filter_sharpness = 0;
    config.filter_type = 1;
    config.autofilter = 1;
    config.alpha_compression = 1;
    config.alpha_filtering = 1;
    config.alpha_quality = 75;
    config.pass = 6;
    config.show_compressed = 0;
    config.preprocessing = 0;
    config.partitions = 0;
    config.partition_limit = 0;
    config.emulate_jpeg_size = 0;
    config.thread_level = 0;
    config.low_memory = 0;
    config.near_lossless = 100;
    config.exact = 0;
    config.use_delta_palette = 0;
    config.use_sharp_yuv = 0;
    config.qmin = 0;
    config.qmax = 100;

    // 设置WebP编码器参数
    WebPPicture pic;
    WebPPictureInit(&pic);
    pic.width = frame.width;
    pic.height = frame.height;
    pic.use_argb = true;
    WebPPictureAlloc(&pic);
    if (frame.color_type == PNG_COLOR_TYPE_PALETTE)
    {
        // 调色板模式
        for (png_uint_32 y = 0; y < frame.height; ++y)
        {
            png_bytep row = frame.row_pointers[y];
            for (png_uint_32 x = 0; x < frame.width; ++x)
            {
                png_bytep px = &(row[x]);
                uint8_t alpha = 255;
                if (frame.trans_alpha)
                {
                    alpha = frame.trans_alpha[*px];
                }
                //uint8_t r = frame.palette[3 * (*px)];
                //uint8_t g = frame.palette[3 * (*px) + 1];
                //uint8_t b = frame.palette[3 * (*px) + 2];

                uint8_t r = frame.palette->red;
                uint8_t g = frame.palette->green;
                uint8_t b = frame.palette->blue;


                uint32_t argb = (alpha << 24) | (r << 16) | (g << 8) | b;
                WebPPictureSetARGB(&pic, argb, x, y);
            }
        }
    }
    else if (frame.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        // 灰度+透明度模式
        for (png_uint_32 y = 0; y < frame.height; ++y)
        {
            png_bytep row = frame.row_pointers[y];
            for (png_uint_32 x = 0; x < frame.width; ++x)
            {
                png_bytep px = &(row[2 * x]);
                uint8_t alpha = px[1];
                uint8_t gray = px[0];
                uint32_t argb = (alpha << 24) | (gray << 16) | (gray << 8) | gray;
                WebPPictureSetARGB(&pic, argb, x, y);
            }
        }
    }
    else if (frame.color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    {
        // RGB+透明度模式
        for (png_uint_32 y = 0; y < frame.height; ++y)
        {
            png_bytep row = frame.row_pointers[y];
            for (png_uint_32 x = 0; x < frame.width; ++x)
            {
                png_bytep px = &(row[4 * x]);
                uint8_t alpha = px[3];
                uint8_t r = px[0];
                uint8_t g = px[1];
                uint8_t b = px[2];
                uint32_t argb = (alpha << 24) | (r << 16) | (g << 8) | b;
                WebPPictureSetARGB(&pic, argb, x, y);
            }
        }
    }
    else
    {
        // 其他模式不支持
        return false;
    }
    // 编码WebP图片
    uint8_t *output_data = NULL;
    size_t output_size = 0;
    if (!WebPEncode(&config, &pic, &output_data, &output_size))
    {
        return false;
    }
    // 将编码后的数据添加到输出数据中
    webp_data.insert(webp_data.end(), output_data, output_data + output_size);
    // 释放资源
    WebPPictureFree(&pic);
    free(output_data);
    return true;
}

// 读取APNG数据块
void PNGAPI apng_read_chunk(png_structp png_ptr, png_unknown_chunkp chunk) {
    if (!chunk->data || chunk->size < 4) {
        return;
    }
    if (memcmp(chunk->data, "acTL", 4) == 0) {
        // 读取acTL块
        png_uint_32 num_frames = png_get_uint_32(chunk->data + 8);
        png_uint_32 num_plays = png_get_uint_32(chunk->data + 12);
        APNGFrameControl fc = {0};
        fc.num_frames = num_frames;
        fc.num_plays = num_plays;
        apng_frames.push_back(fc);
    } else if (memcmp(chunk->data, "fcTL", 4) == 0) {
        // 读取fcTL块
        APNGFrameControl& fc = apng_frames.back();
        fc.width = png_get_uint_32(chunk->data + 12);
        fc.height = png_get_uint_32(chunk->data + 16);
        fc.x_offset = png_get_uint_32(chunk->data + 20);
        fc.y_offset = png_get_uint_32(chunk->data + 24);
        fc.delay_num = png_get_uint_16(chunk->data + 28);
        fc.delay_den = png_get_uint_16(chunk->data + 30);
        fc.dispose_op = chunk->data[32];
        fc.blend_op = chunk->data[33];
        fc.has_fcTL = true;
    } else if (memcmp(chunk->data, "fdAT", 4) == 0) {
        // 读取fdAT块
        APNGFrameControl& fc = apng_frames.back();
        fc.frame_data.push_back(chunk->data[4]);
        fc.frame_data.push_back(chunk->data[5]);
        fc.frame_data.insert(fc.frame_data.end(), chunk->data + 8, chunk->data + chunk->size);
        fc.has_fdAT = true;
    }
}

// 将APNG文件转换为WEBP文件
bool apng_to_webp(const char *apng_file, const char *webp_file)
{
    // 打开APNG文件
    FILE *fp = fopen(apng_file, "rb");
    if (!fp)
    {
        return false;
    }
    // 读取APNG文件头
    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
    {
        fclose(fp);
        return false;
    }
    // 初始化PNG解码器和信息对象
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fclose(fp);
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL);
        fclose(fp);
        return false;
    }
    // 设置错误处理器
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return false;
    }
    // 设置文件指针
    png_init_io(png_ptr, fp);
    // 设置读取APNG文件
    png_set_sig_bytes(png_ptr, 8);
    png_set_chunk_cache_max(png_ptr, 1024 * 1024, 0);
    png_set_user_limits(png_ptr, PNG_USER_WIDTH_MAX, PNG_USER_HEIGHT_MAX);
    png_uint_32 apng_flags = PNG_DISPOSE_OP_NONE | PNG_BLEND_OP_SOURCE;
    png_set_keep_unknown_chunks(png_ptr, PNG_HANDLE_CHUNK_ALWAYS, (png_const_bytep) "acTL", 1);
    png_set_keep_unknown_chunks(png_ptr, PNG_HANDLE_CHUNK_ALWAYS, (png_const_bytep) "fcTL", 1);
    png_set_keep_unknown_chunks(png_ptr, PNG_HANDLE_CHUNK_ALWAYS, (png_const_bytep) "fdAT", 1);
    png_set_read_user_chunk_fn(png_ptr, &apng_read_chunk, &apng_flags);
    // 读取APNG帧数据
    vector<APNGFrame> frames;
    APNGFrame frame;
    while (read_apng_frame(frame, png_ptr, info_ptr))
    {
        frames.push_back(frame);
    }
    // 关闭PNG解码器和文件指针
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    // 将APNG帧数据转换为WEBP格式
    vector<uint8_t> webp_data;
    for (size_t i = 0; i < frames.size(); ++i)
    {
        if (!apng_frame_to_webp(frames[i], webp_data))
        {
            for (size_t j = i; j < frames.size(); ++j)
            {
                free_apng_frame(frames[j]);
            }
            return false;
        }
        free_apng_frame(frames[i]);
    }
    // 写入WEBP文件
    FILE *webp_fp = fopen(webp_file, "wb");
    if (!webp_fp)
    {
        return false;
    }
    fwrite(webp_data.data(), 1, webp_data.size(), webp_fp);
    fclose(webp_fp);
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " input.apng output.webp" << endl;
        return 1;
    }
    if (!apng_to_webp(argv[1], argv[2]))
    {
        cout << "Failed to convert APNG to WEBP." << endl;
        return 1;
    }
    cout << "APNG to WEBP conversion succeeded." << endl;
    return 0;
}