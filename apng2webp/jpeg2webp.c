#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <webp/encode.h>
#include "jpeglib.h"

static int ReadJPEG(const char *filename, uint8_t **data, int *width, int *height)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *infile;
  JSAMPARRAY buffer;
  int row_stride;

  if ((infile = fopen(filename, "rb")) == NULL)
  {
    fprintf(stderr, "Error opening JPEG file %s\n", filename);
    return 0;
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);
  *width = cinfo.output_width;
  *height = cinfo.output_height;

  *data = (uint8_t *)malloc(cinfo.output_width * cinfo.output_height * cinfo.output_components);
  buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, cinfo.output_width * cinfo.output_components, 1);
  row_stride = cinfo.output_width * cinfo.output_components;

  while (cinfo.output_scanline < cinfo.output_height)
  {
    jpeg_read_scanlines(&cinfo, buffer, 1);
    memcpy(*data + (cinfo.output_scanline - 1) * row_stride, buffer[0], row_stride);
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);

  return 1;
}

static int WriteWebP(const char *filename, const uint8_t *data, int width, int height)
{
  int success;
  uint8_t *output;
  size_t output_size;

  WebPConfig config;
  WebPPicture picture;
  WebPMemoryWriter memory_writer;

  if (!WebPConfigPreset(&config, WEBP_PRESET_DEFAULT, 75))
  {
    fprintf(stderr, "Error with WebP config preset\n");
    return 0;
  }

  if (!WebPValidateConfig(&config))
  {
    fprintf(stderr, "Invalid WebP config\n");
    return 0;
  }

  if (!WebPPictureInit(&picture))
  {
    fprintf(stderr, "Error with WebP picture init\n");
    return 0;
  }
  picture.width = width;
  picture.height = height;
  picture.use_argb = 1;

  if (!WebPPictureImportRGB(&picture, data, width * 3))
  {
    fprintf(stderr, "Error importing RGB data to WebP picture\n");
    WebPPictureFree(&picture);
    return 0;
  }

  WebPMemoryWriterInit(&memory_writer);
  picture.writer = WebPMemoryWrite;
  picture.custom_ptr = &memory_writer;

  success = WebPEncode(&config, &picture);
  if (!success)
  {
    fprintf(stderr, "Error encoding WebP picture\n");
    WebPPictureFree(&picture);
    return 0;
  }

  FILE *outfile = fopen(filename, "wb");
  if (outfile == NULL)
  {
    fprintf(stderr, "Error opening WebP file %s\n", filename);
    WebPPictureFree(&picture);
    return 0;
  }

  fwrite(memory_writer.mem, memory_writer.size, 1, outfile);
  fclose(outfile);

  WebPPictureFree(&picture);
  WebPMemoryWriterClear(&memory_writer);
  return 1;
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s input.jpg output.webp\n", argv[0]);
    return 1;
  }

  const char *input_file = argv[1];
  const char *output_file = argv[2];

  uint8_t *jpeg_data;
  int width, height;

  if (!ReadJPEG(input_file, &jpeg_data, &width, &height))
  {
    fprintf(stderr, "Failed to read JPEG file\n");
    return 1;
  }

  if (!WriteWebP(output_file, jpeg_data, width, height))
  {
    fprintf(stderr, "Failed to write WebP file\n");
    free(jpeg_data);
    return 1;
  }

  free(jpeg_data);
  return 0;
}
