#include <png.h>
#include <webp/encode.h>
#include <webp/mux.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Function to convert a single PNG image to WebP format
bool convert_png_to_webp(const uint8_t *rgba, int width, int height, int quality, WebPPicture *webp_pic)
{
  if (!WebPPictureImportRGBA(webp_pic, rgba, width * 4))
  {
    printf("Error: WebPPictureImportRGBA failed.\n");
    return false;
  }

  WebPConfig config;
  if (!WebPConfigInit(&config) || !WebPConfigLosslessPreset(&config, quality))
  {
    printf("Error: WebPConfig initialization failed.\n");
    return false;
  }

  if (!WebPValidateConfig(&config))
  {
    printf("Error: Invalid WebPConfig.\n");
    return false;
  }

  WebPMemoryWriter writer;
  WebPMemoryWriterInit(&writer);
  webp_pic->custom_ptr = &writer;
  webp_pic->writer = WebPMemoryWrite;

  bool success = WebPEncode(&config, webp_pic);
  if (!success)
  {
    printf("Error: WebPEncode failed.\n");
  }

  webp_pic->custom_ptr = NULL;
  WebPMemoryWriterClear(&writer);
  return success;
}

// Function to process APNG frames
void process_apng_frames(const char *input, const char *output)
{
  // Read the APNG file
  FILE *fp = fopen(input, "rb");
  if (!fp)
  {
    printf("Error: Can't open input file.\n");
    return;
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info = png_create_info_struct(png_ptr);

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);

  // Check if the file is an APNG
  if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_acTL))
  {
    printf("Error: The input file is not an APNG.\n");
    fclose(fp);
    return;
  }

  // Get the number of frames and the loop count
  png_uint_32 num_frames = png_get_num_frames(png_ptr, info_ptr);
  png_uint_32 loop_count = png_get_num_plays(png_ptr, info_ptr);

  // Initialize the WebP mux object
  WebPMux *mux = WebPMuxNew();
  if (mux == NULL)
  {
    printf("Error: WebPMuxNew failed.\n");
    fclose(fp);
    return;
  }

  // Process each frame
  for (png_uint_32 i = 0; i < num_frames; i++)
  {
    // Read the frame
    png_read_frame_head(png_ptr, info_ptr);

    // Get the frame delay
    png_uint_16 delay_num = png_get_next_frame_delay_num(png_ptr, info_ptr);
    png_uint_16 delay_den = png_get_next_frame_delay_den(png_ptr, info_ptr);

    if (delay_den == 0)
    {
      delay_den = 100; // Use the default denominator value
    }
    
    int duration = (delay_num * 1000) / delay_den;

    // Decode the frame
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);

    // Convert the frame to WebP format
    WebPPicture webp_pic;
    WebPPictureInit(&webp_pic);
    webp_pic.width = width;
    webp_pic.height = height;

    if (!convert_png_to_webp((uint8_t *)row_pointers[0], width, height, 75, &webp_pic))
    {
      printf("Error: Failed to convert frame %u to WebP format.\n", (unsigned int)i);
      WebPPictureFree(&webp_pic);
      continue;
    }

    // Add the WebP frame to the mux object
    WebPData webp_data;
    WebPDataInit(&webp_data);
    webp_data.bytes =(uint8_t*)webp_pic.custom_ptr;
    webp_data.size = webp_pic.size;

    WebPMuxFrameInfo frame_info;
    memset(&frame_info, 0, sizeof(frame_info));

    frame_info.bitstream = webp_data;
    frame_info.id = WEBP_CHUNK_ANMF;
    frame_info.dispose_method = WEBP_MUX_DISPOSE_BACKGROUND;
    frame_info.blend_method = WEBP_MUX_NO_BLEND;
    frame_info.duration = duration;

    WebPMuxError err = WebPMuxPushFrame(mux, &frame_info, 1);
    if (err != WEBP_MUX_OK)
    {
      printf("Error: WebPMuxPushFrame failed (error code: %d).\n", err);
    }

    // Free the WebP picture
    WebPPictureFree(&webp_pic);
  }

  // Set the loop count
  WebPMuxAnimParams anim_params;
  anim_params.bgcolor = 0xFFFFFFFF;
  anim_params.loop_count = loop_count;
  WebPMuxSetAnimationParams(mux, &anim_params);

  // Assemble the final WebP data
  WebPData output_data;
  WebPDataInit(&output_data);
  WebPMuxAssemble(mux, &output_data);

  // Save the WebP animation
  fp = fopen(output, "wb");

  if (!fp)
  {
    printf("Error: Can't open output file.\n");
    WebPDataClear(&output_data);
    WebPMuxDelete(mux);
    return;
  }
  fwrite(output_data.bytes, output_data.size, 1, fp);
  fclose(fp);

  // Cleanup
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  WebPDataClear(&output_data);
  WebPMuxDelete(mux);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Usage: %s input.apng output.webp\n", argv[0]);
    return 1;
  }

  process_apng_frames(argv[1], argv[2]);
  return 0;
}