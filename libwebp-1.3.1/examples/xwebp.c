// Copyright 2011 Google Inc. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the COPYING file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS. All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
// -----------------------------------------------------------------------------
//
//    simple command line calling the WebPEncode function.
//    Encodes a raw .YUV into WebP bitstream
//
// Author: Skal (pascal.massimino@gmail.com)
// kenyuan 20230701  support APNG,GIF
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "webp/config.h"
#endif

//apng
#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <webp/encode.h>
#include <webp/mux.h>
#include <jpeglib.h>

#include "../examples/example_util.h"
#include "../imageio/image_dec.h"
#include "../imageio/imageio_util.h"
#include "../imageio/webpdec.h"
#include "./stopwatch.h"
#include "./unicode.h"
#include "sharpyuv/sharpyuv.h"
#include "webp/encode.h"

//gif
#include <gif_lib.h>
#include "./gifdec.h"
#include "./unicode_gif.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <utime.h>

#ifndef WEBP_DLL
#ifdef __cplusplus
extern "C"
{
#endif

  extern void *VP8GetCPUInfo; // opaque forward declaration.

#ifdef __cplusplus
} // extern "C"
#endif
#endif // WEBP_DLL

//------------------------------------------------------------------------------

static int retry = -1;
static int forcesmall = 0;

/*apng begin------------------------------------*/
int apng_to_webp(const char *apng_file, const char *webp_file, WebPConfig inconfig)
{
  AVFormatContext *format_ctx = NULL;
  if (avformat_open_input(&format_ctx, apng_file, NULL, NULL) != 0)
  {
    return -1;
  }

  if (avformat_find_stream_info(format_ctx, NULL) < 0)
  {
    avformat_close_input(&format_ctx);
    return -1;
  }

  int video_stream_index = -1;
  unsigned int i = 0;
  for (i = 0; i < format_ctx->nb_streams; ++i)
  {
    if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
    {
      video_stream_index = i;
      break;
    }
  }

  if (video_stream_index == -1)
  {
    avformat_close_input(&format_ctx);
    return -1;
  }

  AVCodecParameters *codecpar = format_ctx->streams[video_stream_index]->codecpar;
  const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
  if (!codec)
  {
    avformat_close_input(&format_ctx);
    return -1;
  }

  AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
  if (!codec_ctx)
  {
    avformat_close_input(&format_ctx);
    return -1;
  }

  if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0)
  {
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
    return -1;
  }

  if (avcodec_open2(codec_ctx, codec, NULL) < 0)
  {
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
    return -1;
  }

  AVFrame *frame = av_frame_alloc();
  if (!frame)
  {
    avcodec_close(codec_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
    return -1;
  }

  AVPacket packet;
  av_init_packet(&packet);
  packet.data = NULL;
  packet.size = 0;

  if (!WebPValidateConfig(&inconfig)) {
    printf("Error! Invalid configuration.\n");
    return -1;
  }

  WebPAnimEncoderOptions anim_encoder_options;
  WebPAnimEncoderOptionsInit(&anim_encoder_options);
  anim_encoder_options.anim_params.loop_count = 0;

  WebPAnimEncoder *anim_encoder = WebPAnimEncoderNew(codec_ctx->width, codec_ctx->height, &anim_encoder_options);
  if (!anim_encoder)
  {
    av_frame_free(&frame);
    avcodec_close(codec_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
    return -1;
  }

  int timestamp = 0;
  int frame_size = 0;

  while (av_read_frame(format_ctx, &packet) >= 0)
  {
    if (packet.stream_index == video_stream_index)
    {
      if (avcodec_send_packet(codec_ctx, &packet) >= 0)
      {
        while (avcodec_receive_frame(codec_ctx, frame) >= 0)
        {
          frame_size++;
          WebPPicture pic;
          WebPPictureInit(&pic);
          pic.width = codec_ctx->width;
          pic.height = codec_ctx->height;
          pic.use_argb = 1;
          WebPPictureAlloc(&pic);

          uint8_t *src_data[4] = {frame->data[0], frame->data[1], frame->data[2], NULL};
          int src_linesize[4] = {frame->linesize[0], frame->linesize[1], frame->linesize[2], 0};
          WebPPictureImportRGBA(&pic, src_data[0], src_linesize[0]);

          AVRational fps = {1, 1000};

          int duration_ms = av_rescale_q(frame->pkt_duration, format_ctx->streams[video_stream_index]->time_base, fps);
          WebPAnimEncoderAdd(anim_encoder, &pic, timestamp, &inconfig);
          timestamp += duration_ms;

          WebPPictureFree(&pic);
        }
      }
    }
    av_packet_unref(&packet);
  }

  if(0 >= frame_size){
    printf("Error! No APNG frame be handled.\n");
    return -1;
  }

  WebPAnimEncoderAdd(anim_encoder, NULL, timestamp, NULL);

  WebPData webp_data;
  WebPDataInit(&webp_data);
  WebPAnimEncoderAssemble(anim_encoder, &webp_data);

  // 写入WebP文件
  FILE *webp_fp = fopen(webp_file, "wb");
  if (!webp_fp)
  {
    WebPDataClear(&webp_data);
    WebPAnimEncoderDelete(anim_encoder);
    av_frame_free(&frame);
    avcodec_close(codec_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
    return -1;
  }

  fwrite(webp_data.bytes, 1, webp_data.size, webp_fp);
  fclose(webp_fp);

  // 释放资源
  WebPDataClear(&webp_data);
  WebPAnimEncoderDelete(anim_encoder);
  av_frame_free(&frame);
  avcodec_close(codec_ctx);
  avcodec_free_context(&codec_ctx);
  avformat_close_input(&format_ctx);

  return 0;
}
/*apng end------------------------------------*/

/*gif begin------------------------------------*/
static int transparent_index = GIF_INDEX_INVALID;  // Opaque by default.

static const char* const kErrorMessagesgif[-WEBP_MUX_NOT_ENOUGH_DATA + 1] = {
  "WEBP_MUX_NOT_FOUND", "WEBP_MUX_INVALID_ARGUMENT", "WEBP_MUX_BAD_DATA",
  "WEBP_MUX_MEMORY_ERROR", "WEBP_MUX_NOT_ENOUGH_DATA"
};

static const char* ErrorStringgif(WebPMuxError err) {
  assert(err <= WEBP_MUX_NOT_FOUND && err >= WEBP_MUX_NOT_ENOUGH_DATA);
  return kErrorMessagesgif[-err];
}

enum {
  METADATA_ICC_GIF  = (1 << 0),
  METADATA_XMP_GIF  = (1 << 1),
  METADATA_ALL_GIF  = METADATA_ICC_GIF | METADATA_XMP_GIF
};

int gif_to_webp(const char *in_file, const char *out_file, WebPConfig inconfig){
  int verbose = 0;
  int gif_error = GIF_ERROR;
  WebPMuxError err = WEBP_MUX_OK;
  int ok = -1;
  GifFileType* gif = NULL;
  int frame_duration = 0;
  int frame_timestamp = 0;
  GIFDisposeMethod orig_dispose = GIF_DISPOSE_NONE;

  WebPPicture frame;                // Frame rectangle only (not disposed).
  WebPPicture curr_canvas;          // Not disposed.
  WebPPicture prev_canvas;          // Disposed.

  WebPAnimEncoder* enc = NULL;
  WebPAnimEncoderOptions enc_options;
  WebPConfig config = inconfig;

  int frame_number = 0;     // Whether we are processing the first frame.
  int done;
  int c;
  int quiet = 0;
  WebPData webp_data;

  int keep_metadata = METADATA_XMP_GIF;  // ICC not output by default.
  WebPData icc_data;
  int stored_icc = 0;         // Whether we have already stored an ICC profile.
  WebPData xmp_data;
  int stored_xmp = 0;         // Whether we have already stored an XMP profile.
  int loop_count = 0;         // default: infinite
  int stored_loop_count = 0;  // Whether we have found an explicit loop count.
  int loop_compatibility = 0;
  WebPMux* mux = NULL;

  int default_kmin = 1;  // Whether to use default kmin value.
  int default_kmax = 1;

  if (!WebPConfigInit(&config) || !WebPAnimEncoderOptionsInit(&enc_options) ||
      !WebPPictureInit(&frame) || !WebPPictureInit(&curr_canvas) ||
      !WebPPictureInit(&prev_canvas)) {
    fprintf(stderr, "Error! Version mismatch!\n");
    FREE_WARGV_AND_RETURN(-1);
  }
  config.lossless = 1;  // Use lossless compression by default.

  WebPDataInit(&webp_data);
  WebPDataInit(&icc_data);
  WebPDataInit(&xmp_data);

  enc_options.allow_mixed = 1;
  loop_compatibility = 1;

  if (!WebPValidateConfig(&config)) {
    fprintf(stderr, "Error! Invalid configuration.\n");
    goto End;
  }

  if (in_file == NULL) {
    fprintf(stderr, "No input file specified!\n");
    goto End;
  }

  // Start the decoder object
  gif = DGifOpenFileUnicode(in_file, &gif_error);
  if (gif == NULL) goto End;

  // Loop over GIF images
  done = 0;
  do {
    GifRecordType type;
    if (DGifGetRecordType(gif, &type) == GIF_ERROR) goto End;

    switch (type) {
      case IMAGE_DESC_RECORD_TYPE: {
        GIFFrameRect gif_rect;
        GifImageDesc* const image_desc = &gif->Image;

        if (!DGifGetImageDesc(gif)) goto End;

        if (frame_number == 0) {
          if (verbose) {
            printf("Canvas screen: %d x %d\n", gif->SWidth, gif->SHeight);
          }
          // Fix some broken GIF global headers that report
          // 0 x 0 screen dimension.
          if (gif->SWidth == 0 || gif->SHeight == 0) {
            image_desc->Left = 0;
            image_desc->Top = 0;
            gif->SWidth = image_desc->Width;
            gif->SHeight = image_desc->Height;
            if (gif->SWidth <= 0 || gif->SHeight <= 0) {
              goto End;
            }
            if (verbose) {
              printf("Fixed canvas screen dimension to: %d x %d\n",
                     gif->SWidth, gif->SHeight);
            }
          }
          // Allocate current buffer.
          frame.width = gif->SWidth;
          frame.height = gif->SHeight;
          frame.use_argb = 1;
          if (!WebPPictureAlloc(&frame)) goto End;
          GIFClearPic(&frame, NULL);
          if (!(WebPPictureCopy(&frame, &curr_canvas) &&
                WebPPictureCopy(&frame, &prev_canvas))) {
            fprintf(stderr, "Error allocating canvas.\n");
            goto End;
          }

          // Background color.
          GIFGetBackgroundColor(gif->SColorMap, gif->SBackGroundColor,
                                transparent_index,
                                &enc_options.anim_params.bgcolor);

          // Initialize encoder.
          enc = WebPAnimEncoderNew(curr_canvas.width, curr_canvas.height,
                                   &enc_options);
          if (enc == NULL) {
            fprintf(stderr,
                    "Error! Could not create encoder object. Possibly due to "
                    "a memory error.\n");
            goto End;
          }
        }

        // Some even more broken GIF can have sub-rect with zero width/height.
        if (image_desc->Width == 0 || image_desc->Height == 0) {
          image_desc->Width = gif->SWidth;
          image_desc->Height = gif->SHeight;
        }

        if (!GIFReadFrame(gif, transparent_index, &gif_rect, &frame)) {
          goto End;
        }
        // Blend frame rectangle with previous canvas to compose full canvas.
        // Note that 'curr_canvas' is same as 'prev_canvas' at this point.
        GIFBlendFrames(&frame, &gif_rect, &curr_canvas);

        if (!WebPAnimEncoderAdd(enc, &curr_canvas, frame_timestamp, &config)) {
          fprintf(stderr, "Error while adding frame #%d: %s\n", frame_number,
                  WebPAnimEncoderGetError(enc));
          goto End;
        } else {
          ++frame_number;
        }

        // Update canvases.
        GIFDisposeFrame(orig_dispose, &gif_rect, &prev_canvas, &curr_canvas);
        GIFCopyPixels(&curr_canvas, &prev_canvas);

        // Force frames with a small or no duration to 100ms to be consistent
        // with web browsers and other transcoding tools. This also avoids
        // incorrect durations between frames when padding frames are
        // discarded.
        if (frame_duration <= 10) {
          frame_duration = 100;
        }

        // Update timestamp (for next frame).
        frame_timestamp += frame_duration;

        // In GIF, graphic control extensions are optional for a frame, so we
        // may not get one before reading the next frame. To handle this case,
        // we reset frame properties to reasonable defaults for the next frame.
        orig_dispose = GIF_DISPOSE_NONE;
        frame_duration = 0;
        transparent_index = GIF_INDEX_INVALID;
        break;
      }
      case EXTENSION_RECORD_TYPE: {
        int extension;
        GifByteType* data = NULL;
        if (DGifGetExtension(gif, &extension, &data) == GIF_ERROR) {
          goto End;
        }
        if (data == NULL) continue;

        switch (extension) {
          case COMMENT_EXT_FUNC_CODE: {
            break;  // Do nothing for now.
          }
          case GRAPHICS_EXT_FUNC_CODE: {
            if (!GIFReadGraphicsExtension(data, &frame_duration, &orig_dispose,
                                          &transparent_index)) {
              goto End;
            }
            break;
          }
          case PLAINTEXT_EXT_FUNC_CODE: {
            break;
          }
          case APPLICATION_EXT_FUNC_CODE: {
            if (data[0] != 11) break;    // Chunk is too short
            if (!memcmp(data + 1, "NETSCAPE2.0", 11) ||
                !memcmp(data + 1, "ANIMEXTS1.0", 11)) {
              if (!GIFReadLoopCount(gif, &data, &loop_count)) {
                goto End;
              }
              if (verbose) {
                fprintf(stderr, "Loop count: %d\n", loop_count);
              }
              stored_loop_count = loop_compatibility ? (loop_count != 0) : 1;
            } else { 
              // An extension containing metadata.
              // We only store the first encountered chunk of each type, and
              // only if requested by the user.
              const int is_xmp = (keep_metadata & METADATA_XMP_GIF) &&
                                 !stored_xmp &&
                                 !memcmp(data + 1, "XMP DataXMP", 11);
              const int is_icc = (keep_metadata & METADATA_ICC_GIF) &&
                                 !stored_icc &&
                                 !memcmp(data + 1, "ICCRGBG1012", 11);
              if (is_xmp || is_icc) {
                if (!GIFReadMetadata(gif, &data,
                                     is_xmp ? &xmp_data : &icc_data)) {
                  goto End;
                }
                if (is_icc) {
                  stored_icc = 1;
                } else if (is_xmp) {
                  stored_xmp = 1;
                }
              }
            }
            break;
          }
          default: {
            break;  // skip
          }
        }
        while (data != NULL) {
          if (DGifGetExtensionNext(gif, &data) == GIF_ERROR) goto End;
        }
        break;
      }
      case TERMINATE_RECORD_TYPE: {
        done = 1;
        break;
      }
      default: {
        if (verbose) {
          fprintf(stderr, "Skipping over unknown record type %d\n", type);
        }
        break;
      }
    }
  } while (!done);

  // Last NULL frame.
  if (!WebPAnimEncoderAdd(enc, NULL, frame_timestamp, NULL)) {
    fprintf(stderr, "Error flushing WebP muxer.\n");
    fprintf(stderr, "%s\n", WebPAnimEncoderGetError(enc));
  }

  if (!WebPAnimEncoderAssemble(enc, &webp_data)) {
    fprintf(stderr, "%s\n", WebPAnimEncoderGetError(enc));
    goto End;
  }
  // If there's only one frame, we don't need to handle loop count.
  if (frame_number == 1) {
    loop_count = 0;
  } else if (!loop_compatibility) {
    if (!stored_loop_count) {
      // if no loop-count element is seen, the default is '1' (loop-once)
      // and we need to signal it explicitly in WebP. Note however that
      // in case there's a single frame, we still don't need to store it.
      if (frame_number > 1) {
        stored_loop_count = 1;
        loop_count = 1;
      }
    } else if (loop_count > 0 && loop_count < 65535) {
      // adapt GIF's semantic to WebP's (except in the infinite-loop case)
      loop_count += 1;
    }
  }
  // loop_count of 0 is the default (infinite), so no need to signal it
  if (loop_count == 0) stored_loop_count = 0;

  if (stored_loop_count || stored_icc || stored_xmp) {
    // Re-mux to add loop count and/or metadata as needed.
    mux = WebPMuxCreate(&webp_data, 1);
    if (mux == NULL) {
      fprintf(stderr, "ERROR: Could not re-mux to add loop count/metadata.\n");
      goto End;
    }
    WebPDataClear(&webp_data);

    if (stored_loop_count) {  // Update loop count.
      WebPMuxAnimParams new_params;
      err = WebPMuxGetAnimationParams(mux, &new_params);
      if (err != WEBP_MUX_OK) {
        fprintf(stderr, "ERROR (%s): Could not fetch loop count.\n",
                ErrorStringgif(err));
        goto End;
      }
      new_params.loop_count = loop_count;
      err = WebPMuxSetAnimationParams(mux, &new_params);
      if (err != WEBP_MUX_OK) {
        fprintf(stderr, "ERROR (%s): Could not update loop count.\n",
                ErrorStringgif(err));
        goto End;
      }
    }

    if (stored_icc) {   // Add ICCP chunk.
      err = WebPMuxSetChunk(mux, "ICCP", &icc_data, 1);
      if (verbose) {
        fprintf(stderr, "ICC size: %d\n", (int)icc_data.size);
      }
      if (err != WEBP_MUX_OK) {
        fprintf(stderr, "ERROR (%s): Could not set ICC chunk.\n",
                ErrorStringgif(err));
        goto End;
      }
    }

    if (stored_xmp) {   // Add XMP chunk.
      err = WebPMuxSetChunk(mux, "XMP ", &xmp_data, 1);
      if (verbose) {
        fprintf(stderr, "XMP size: %d\n", (int)xmp_data.size);
      }
      if (err != WEBP_MUX_OK) {
        fprintf(stderr, "ERROR (%s): Could not set XMP chunk.\n",
                ErrorStringgif(err));
        goto End;
      }
    }

    err = WebPMuxAssemble(mux, &webp_data);
    if (err != WEBP_MUX_OK) {
      fprintf(stderr, "ERROR (%s): Could not assemble when re-muxing to add "
              "loop count/metadata.\n", ErrorStringgif(err));
      goto End;
    }
  }

  if (out_file != NULL) {
    if (!ImgIoUtilWriteFile((const char*)out_file, webp_data.bytes,
                            webp_data.size)) {
      WFPRINTF(stderr, "Error writing output file: %s\n", out_file);
      goto End;
    }
    if (!quiet) {
      if (!WSTRCMP(out_file, "-")) {
        fprintf(stderr, "Saved %d bytes to STDIO\n",
                (int)webp_data.size);
      } else {
        WFPRINTF(stderr, "Saved output file (%d bytes): %s\n",
                 (int)webp_data.size, out_file);
      }
    }
  } else {
    if (!quiet) {
      fprintf(stderr, "Nothing written; use -o flag to save the result "
                      "(%d bytes).\n", (int)webp_data.size);
    }
  }

  // All OK.
  ok = 0;
  gif_error = GIF_OK;

 End:
  WebPDataClear(&icc_data);
  WebPDataClear(&xmp_data);
  WebPMuxDelete(mux);
  WebPDataClear(&webp_data);
  WebPPictureFree(&frame);
  WebPPictureFree(&curr_canvas);
  WebPPictureFree(&prev_canvas);
  WebPAnimEncoderDelete(enc);

  if (gif_error != GIF_OK) {
    GIFDisplayError(gif, gif_error);
  }
  if (gif != NULL) {
#if LOCAL_GIF_PREREQ(5,1)
    DGifCloseFile(gif, &gif_error);
#else
    DGifCloseFile(gif);
#endif
  }

  return ok;
}

/*gif   end------------------------------------*/

#define MAX_PATH 2048
struct stat in_file_stat, temp_file_stat, out_file_stat;
int in_exists = 0, temp_exists = 0, out_exists = 0;

char in_file_path[MAX_PATH] = {0};
char temp_file_path[MAX_PATH] = {0};
char out_file_path[MAX_PATH] = {0};

char *in_file_p = &in_file_path[0];
char *temp_file_p = &temp_file_path[0];
char *out_file_p = &out_file_path[0];

/*stat file*/
int statfile(char *in_file, char *out_file)
{
  /*  判断in_file out_file:
      1:新in_file 无out_file 或 异常out_file: 转换 或 不转换
      2:老的in_file out_file :不转换，直接返回 退出
      3:新的in_file 老的out_file : 转换
  */

  // infile
  if (0 == access(in_file, F_OK))
  {
    in_exists = 1;
    if (stat(in_file, &in_file_stat) != 0)
    {
      printf("Error getting stat of in_file");
      return -1;
    }

    if (1024 > in_file_stat.st_size)
    {
      printf("input file size is too small. < 1k \n");
      return -1;
    }
    memcpy(&in_file_path, in_file, strlen(in_file));
  }
  else
  {
    printf("input file not found.\n");
    return -1;
  }

  // tempfile
  sprintf(temp_file_path, "%s.temp", out_file);
  if (0 == access(temp_file_p, F_OK))
  {
    temp_exists = 1;
    if (stat(temp_file_p, &temp_file_stat) != 0)
    {
      printf("Error getting stat of temp_file.\n");
    }
  }

  // outfile
  memcpy(&out_file_path, out_file, strlen(out_file));
  if (0 == access(out_file, F_OK))
  {
    out_exists = 1;
    if (stat(out_file, &out_file_stat) != 0)
    {
      printf("Error getting stat of out_file.\n");
    }
  }

  // time_t         st_atime;            //文件最后被访问的时间    Access Time
  // time_t         st_mtime;            //文件内容最后被修改的时间 Modify Time
  // time_t         st_ctime;            //文件状态改变时间    Change Time
  if (1 == in_exists && 1 == out_exists)
  {
    if ( 2 >= (out_file_stat.st_mtime-in_file_stat.st_mtime) && (out_file_stat.st_mtime > in_file_stat.st_mtime) )
    { // same Modify Time
      printf("Input file and output file is match, cancel convert, exit.\n");
      if (1 == temp_exists)
      {
        if (60 * 60 > abs(temp_file_stat.st_ctime - time(NULL)))
        { // rubbish temp file, delete it
          remove(temp_file_p);
        }
      }
      return -1;
    }
  }
  else if (1 == in_exists && 1 == temp_exists)
  {
    if(-1 == retry) 
    {
      retry = 2147483647;
    }

    if (in_file_stat.st_mtime == temp_file_stat.st_mtime && retry > abs(temp_file_stat.st_ctime - time(NULL)))
    { // maybe bad convert history, keep temp file in retry seconds interval.
      printf("Input file is bad or we set the flag forcesmall and output file is bigger than input file, cancel convert. \nMaybe you can remove temp file, then retry. tempfile:%s \n",temp_file_p);
      return -1;
    }

    if (60 * 5 > abs(temp_file_stat.st_ctime - time(NULL))) //another thread might be converting
    {
      printf("Another pro/thread might be converting now.\n");
      return -1;
    }
    remove(temp_file_p);  // rubbish temp file, delete it
  }

  return 0;
}

  // time_t         st_atime;            //文件最后被访问的时间    Access Time
  // time_t         st_mtime;            //文件内容最后被修改的时间 Modify Time
  // time_t         st_ctime;            //文件状态改变时间    Change Time
int setSucFileStat(){
  //in_file_stat -> temp_file_stat
  //move tempfile to outfile

  // tempfile
  if (0 == access(temp_file_p, F_OK))
  {
    temp_exists = 1;
    if (stat(temp_file_p, &temp_file_stat) == 0)
    {
      struct utimbuf timebuf;
      timebuf.modtime = in_file_stat.st_mtime;
      timebuf.actime = temp_file_stat.st_atime;
      utime(temp_file_p, &timebuf);

      if(temp_file_stat.st_size > in_file_stat.st_size && 1 == forcesmall)
      {
        printf("output file is bigger than input file, cancel convertion. keep tempfile:%s\n", temp_file_p);
        return -1;  
      }
      else if(1000 < temp_file_stat.st_size)
      {
        AVFormatContext *format_ctx = NULL;
        if (avformat_open_input(&format_ctx, temp_file_p, NULL, NULL) != 0)
        {
          return -1;
        }
        avformat_close_input(&format_ctx);
        rename(temp_file_p, out_file_p);
        return 0;
      }
    }
  }
  printf("Error getting stat of temp_file.\n");
  return -1;
}

int setFailFileStat(){
  // tempfile
  if (0 != access(temp_file_p,F_OK))
  {
    int fd=creat(temp_file_p,O_CREAT);
    if(-1 == fd){
      printf("Error convert, touch tempfile error. tempfile:%s\n",temp_file_p);
      return -1;
    }
  }

  struct utimbuf timebuf;
  timebuf.modtime = in_file_stat.st_mtime;
  timebuf.actime = time(NULL);
  utime(temp_file_p, &timebuf);
  return 0;
}

static int verbose = 0;

static int ReadYUV(const uint8_t *const data, size_t data_size,
                   WebPPicture *const pic)
{
  const int use_argb = pic->use_argb;
  const int uv_width = (pic->width + 1) / 2;
  const int uv_height = (pic->height + 1) / 2;
  const int y_plane_size = pic->width * pic->height;
  const int uv_plane_size = uv_width * uv_height;
  const size_t expected_data_size = y_plane_size + 2 * uv_plane_size;

  if (data_size != expected_data_size)
  {
    fprintf(stderr,
            "input data doesn't have the expected size (%d instead of %d)\n",
            (int)data_size, (int)expected_data_size);
    return 0;
  }

  pic->use_argb = 0;
  if (!WebPPictureAlloc(pic))
    return 0;
  ImgIoUtilCopyPlane(data, pic->width, pic->y, pic->y_stride,
                     pic->width, pic->height);
  ImgIoUtilCopyPlane(data + y_plane_size, uv_width,
                     pic->u, pic->uv_stride, uv_width, uv_height);
  ImgIoUtilCopyPlane(data + y_plane_size + uv_plane_size, uv_width,
                     pic->v, pic->uv_stride, uv_width, uv_height);
  return use_argb ? WebPPictureYUVAToARGB(pic) : 1;
}

#ifdef HAVE_WINCODEC_H

static int ReadPicture(const char *const filename, WebPPicture *const pic,
                       int keep_alpha, Metadata *const metadata)
{
  int ok = 0;
  const uint8_t *data = NULL;
  size_t data_size = 0;
  if (pic->width != 0 && pic->height != 0)
  {
    ok = ImgIoUtilReadFile(filename, &data, &data_size);
    ok = ok && ReadYUV(data, data_size, pic);
  }
  else
  {
    // If no size specified, try to decode it using WIC.
    ok = ReadPictureWithWIC(filename, pic, keep_alpha, metadata);
    if (!ok)
    {
      ok = ImgIoUtilReadFile(filename, &data, &data_size);
      ok = ok && ReadWebP(data, data_size, pic, keep_alpha, metadata);
    }
  }
  if (!ok)
  {
    WFPRINTF(stderr, "Error! Could not process file %s\n",
             (const W_CHAR *)filename);
  }
  WebPFree((void *)data);
  return ok;
}

#else // !HAVE_WINCODEC_H

static int ReadPicture(const char *const filename, WebPPicture *const pic,
                       int keep_alpha, Metadata *const metadata)
{
  const uint8_t *data = NULL;
  size_t data_size = 0;
  int ok = 0;

  ok = ImgIoUtilReadFile(filename, &data, &data_size);
  if (!ok)
    goto End;

  if (pic->width == 0 || pic->height == 0)
  {
    WebPImageReader reader = WebPGuessImageReader(data, data_size);
    ok = reader(data, data_size, pic, keep_alpha, metadata);
  }
  else
  {
    // If image size is specified, infer it as YUV format.
    ok = ReadYUV(data, data_size, pic);
  }
End:
  if (!ok)
  {
    WFPRINTF(stderr, "Error! Could not process file %s\n",
             (const W_CHAR *)filename);
  }
  WebPFree((void *)data);
  return ok;
}

#endif // !HAVE_WINCODEC_H

static void AllocExtraInfo(WebPPicture *const pic)
{
  const int mb_w = (pic->width + 15) / 16;
  const int mb_h = (pic->height + 15) / 16;
  pic->extra_info =
      (uint8_t *)WebPMalloc(mb_w * mb_h * sizeof(*pic->extra_info));
}

static void PrintByteCount(const int bytes[4], int total_size,
                           int *const totals)
{
  int s;
  int total = 0;
  for (s = 0; s < 4; ++s)
  {
    fprintf(stderr, "| %7d ", bytes[s]);
    total += bytes[s];
    if (totals)
      totals[s] += bytes[s];
  }
  fprintf(stderr, "| %7d    (%.1f%%)\n", total, 100.f * total / total_size);
}

static void PrintPercents(const int counts[4])
{
  int s;
  const int total = counts[0] + counts[1] + counts[2] + counts[3];
  for (s = 0; s < 4; ++s)
  {
    fprintf(stderr, "|         %3d%%", (int)(100. * counts[s] / total + .5));
  }
  fprintf(stderr, "| %7d\n", total);
}

static void PrintValues(const int values[4])
{
  int s;
  for (s = 0; s < 4; ++s)
  {
    fprintf(stderr, "| %7d ", values[s]);
  }
  fprintf(stderr, "|\n");
}

static void PrintFullLosslessInfo(const WebPAuxStats *const stats,
                                  const char *const description)
{
  fprintf(stderr, "Lossless-%s compressed size: %d bytes\n",
          description, stats->lossless_size);
  fprintf(stderr, "    * Header size: %d bytes, image data size: %d\n",
          stats->lossless_hdr_size, stats->lossless_data_size);
  if (stats->lossless_features)
  {
    fprintf(stderr, "    * Lossless features used:");
    if (stats->lossless_features & 1)
      fprintf(stderr, " PREDICTION");
    if (stats->lossless_features & 2)
      fprintf(stderr, " CROSS-COLOR-TRANSFORM");
    if (stats->lossless_features & 4)
      fprintf(stderr, " SUBTRACT-GREEN");
    if (stats->lossless_features & 8)
      fprintf(stderr, " PALETTE");
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "    * Precision Bits: histogram=%d transform=%d cache=%d\n",
          stats->histogram_bits, stats->transform_bits, stats->cache_bits);
  if (stats->palette_size > 0)
  {
    fprintf(stderr, "    * Palette size:     %d\n", stats->palette_size);
  }
}

static void PrintExtraInfoLossless(const WebPPicture *const pic,
                                   int short_output,
                                   const char *const file_name)
{
  const WebPAuxStats *const stats = pic->stats;
  if (short_output)
  {
    fprintf(stderr, "%7d %2.2f\n", stats->coded_size, stats->PSNR[3]);
  }
  else
  {
    WFPRINTF(stderr, "File:            %s\n", (const W_CHAR *)file_name);
    fprintf(stderr, "Dimension: %d x %d\n", pic->width, pic->height);
    fprintf(stderr, "Output:        %d bytes (%.2f bpp)\n", stats->coded_size,
            8.f * stats->coded_size / pic->width / pic->height);
    PrintFullLosslessInfo(stats, "ARGB");
  }
}

static void PrintExtraInfoLossy(const WebPPicture *const pic, int short_output,
                                int full_details,
                                const char *const file_name)
{
  const WebPAuxStats *const stats = pic->stats;
  if (short_output)
  {
    fprintf(stderr, "%7d %2.2f\n", stats->coded_size, stats->PSNR[3]);
  }
  else
  {
    const int num_i4 = stats->block_count[0];
    const int num_i16 = stats->block_count[1];
    const int num_skip = stats->block_count[2];
    const int total = num_i4 + num_i16;
    WFPRINTF(stderr, "File:            %s\n", (const W_CHAR *)file_name);
    fprintf(stderr, "Dimension: %d x %d%s\n",
            pic->width, pic->height,
            stats->alpha_data_size ? " (with alpha)" : "");
    fprintf(stderr, "Output:        "
                    "%d bytes Y-U-V-All-PSNR %2.2f %2.2f %2.2f     %2.2f dB\n"
                    "                     (%.2f bpp)\n",
            stats->coded_size,
            stats->PSNR[0], stats->PSNR[1], stats->PSNR[2], stats->PSNR[3],
            8.f * stats->coded_size / pic->width / pic->height);
    if (total > 0)
    {
      int totals[4] = {0, 0, 0, 0};
      fprintf(stderr, "block count:    intra4:         %6d    (%.2f%%)\n"
                      "                            intra16:        %6d    (%.2f%%)\n"
                      "                            skipped:        %6d    (%.2f%%)\n",
              num_i4, 100.f * num_i4 / total,
              num_i16, 100.f * num_i16 / total,
              num_skip, 100.f * num_skip / total);
      fprintf(stderr, "bytes used:    header:                 %6d    (%.1f%%)\n"
                      "                         mode-partition: %6d    (%.1f%%)\n",
              stats->header_bytes[0],
              100.f * stats->header_bytes[0] / stats->coded_size,
              stats->header_bytes[1],
              100.f * stats->header_bytes[1] / stats->coded_size);
      if (stats->alpha_data_size > 0)
      {
        fprintf(stderr, "                         transparency:     %6d (%.1f dB)\n",
                stats->alpha_data_size, stats->PSNR[4]);
      }
      fprintf(stderr, " Residuals bytes    "
                      "|segment 1|segment 2|segment 3"
                      "|segment 4|    total\n");
      if (full_details)
      {
        fprintf(stderr, "    intra4-coeffs:    ");
        PrintByteCount(stats->residual_bytes[0], stats->coded_size, totals);
        fprintf(stderr, " intra16-coeffs:    ");
        PrintByteCount(stats->residual_bytes[1], stats->coded_size, totals);
        fprintf(stderr, "    chroma coeffs:    ");
        PrintByteCount(stats->residual_bytes[2], stats->coded_size, totals);
      }
      fprintf(stderr, "        macroblocks:    ");
      PrintPercents(stats->segment_size);
      fprintf(stderr, "            quantizer:    ");
      PrintValues(stats->segment_quant);
      fprintf(stderr, "     filter level:    ");
      PrintValues(stats->segment_level);
      if (full_details)
      {
        fprintf(stderr, "------------------+---------");
        fprintf(stderr, "+---------+---------+---------+-----------------\n");
        fprintf(stderr, " segments total:    ");
        PrintByteCount(totals, stats->coded_size, NULL);
      }
    }
    if (stats->lossless_size > 0)
    {
      PrintFullLosslessInfo(stats, "alpha");
    }
  }
}

static void PrintMapInfo(const WebPPicture *const pic)
{
  if (pic->extra_info != NULL)
  {
    const int mb_w = (pic->width + 15) / 16;
    const int mb_h = (pic->height + 15) / 16;
    const int type = pic->extra_info_type;
    int x, y;
    for (y = 0; y < mb_h; ++y)
    {
      for (x = 0; x < mb_w; ++x)
      {
        const int c = pic->extra_info[x + y * mb_w];
        if (type == 1)
        { // intra4/intra16
          fprintf(stderr, "%c", "+."[c]);
        }
        else if (type == 2)
        { // segments
          fprintf(stderr, "%c", ".-*X"[c]);
        }
        else if (type == 3)
        { // quantizers
          fprintf(stderr, "%.2d ", c);
        }
        else if (type == 6 || type == 7)
        {
          fprintf(stderr, "%3d ", c);
        }
        else
        {
          fprintf(stderr, "0x%.2x ", c);
        }
      }
      fprintf(stderr, "\n");
    }
  }
}

//------------------------------------------------------------------------------

static int MyWriter(const uint8_t *data, size_t data_size,
                    const WebPPicture *const pic)
{
  FILE *const out = (FILE *)pic->custom_ptr;
  return data_size ? (fwrite(data, data_size, 1, out) == 1) : 1;
}

// Dumps a picture as a PGM file using the IMC4 layout.
static int DumpPicture(const WebPPicture *const picture, const char *PGM_name)
{
  int y;
  const int uv_width = (picture->width + 1) / 2;
  const int uv_height = (picture->height + 1) / 2;
  const int stride = (picture->width + 1) & ~1;
  const uint8_t *src_y = picture->y;
  const uint8_t *src_u = picture->u;
  const uint8_t *src_v = picture->v;
  const uint8_t *src_a = picture->a;
  const int alpha_height =
      WebPPictureHasTransparency(picture) ? picture->height : 0;
  const int height = picture->height + uv_height + alpha_height;
  FILE *const f = WFOPEN(PGM_name, "wb");
  if (f == NULL)
    return 0;
  fprintf(f, "P5\n%d %d\n255\n", stride, height);
  for (y = 0; y < picture->height; ++y)
  {
    if (fwrite(src_y, picture->width, 1, f) != 1)
      return 0;
    if (picture->width & 1)
      fputc(0, f); // pad
    src_y += picture->y_stride;
  }
  for (y = 0; y < uv_height; ++y)
  {
    if (fwrite(src_u, uv_width, 1, f) != 1)
      return 0;
    if (fwrite(src_v, uv_width, 1, f) != 1)
      return 0;
    src_u += picture->uv_stride;
    src_v += picture->uv_stride;
  }
  for (y = 0; y < alpha_height; ++y)
  {
    if (fwrite(src_a, picture->width, 1, f) != 1)
      return 0;
    if (picture->width & 1)
      fputc(0, f); // pad
    src_a += picture->a_stride;
  }
  fclose(f);
  return 1;
}

// -----------------------------------------------------------------------------
// Metadata writing.

enum
{
  METADATA_EXIF = (1 << 0),
  METADATA_ICC = (1 << 1),
  METADATA_XMP = (1 << 2),
  METADATA_ALL = METADATA_EXIF | METADATA_ICC | METADATA_XMP
};

static const int kChunkHeaderSize = 8;
static const int kTagSize = 4;

static void PrintMetadataInfo(const Metadata *const metadata,
                              int metadata_written)
{
  if (metadata == NULL || metadata_written == 0)
    return;

  fprintf(stderr, "Metadata:\n");
  if (metadata_written & METADATA_ICC)
  {
    fprintf(stderr, "    * ICC profile:    %6d bytes\n", (int)metadata->iccp.size);
  }
  if (metadata_written & METADATA_EXIF)
  {
    fprintf(stderr, "    * EXIF data:        %6d bytes\n", (int)metadata->exif.size);
  }
  if (metadata_written & METADATA_XMP)
  {
    fprintf(stderr, "    * XMP data:         %6d bytes\n", (int)metadata->xmp.size);
  }
}

// Outputs, in little endian, 'num' bytes from 'val' to 'out'.
static int WriteLE(FILE *const out, uint32_t val, int num)
{
  uint8_t buf[4];
  int i;
  for (i = 0; i < num; ++i)
  {
    buf[i] = (uint8_t)(val & 0xff);
    val >>= 8;
  }
  return (fwrite(buf, num, 1, out) == 1);
}

static int WriteLE24(FILE *const out, uint32_t val)
{
  return WriteLE(out, val, 3);
}

static int WriteLE32(FILE *const out, uint32_t val)
{
  return WriteLE(out, val, 4);
}

static int WriteMetadataChunk(FILE *const out, const char fourcc[4],
                              const MetadataPayload *const payload)
{
  const uint8_t zero = 0;
  const size_t need_padding = payload->size & 1;
  int ok = (fwrite(fourcc, kTagSize, 1, out) == 1);
  ok = ok && WriteLE32(out, (uint32_t)payload->size);
  ok = ok && (fwrite(payload->bytes, payload->size, 1, out) == 1);
  return ok && (fwrite(&zero, need_padding, need_padding, out) == need_padding);
}

// Sets 'flag' in 'vp8x_flags' and updates 'metadata_size' with the size of the
// chunk if there is metadata and 'keep' is true.
static int UpdateFlagsAndSize(const MetadataPayload *const payload,
                              int keep, int flag,
                              uint32_t *vp8x_flags, uint64_t *metadata_size)
{
  if (keep && payload->bytes != NULL && payload->size > 0)
  {
    *vp8x_flags |= flag;
    *metadata_size += kChunkHeaderSize + payload->size + (payload->size & 1);
    return 1;
  }
  return 0;
}

// Writes a WebP file using the image contained in 'memory_writer' and the
// metadata from 'metadata'. Metadata is controlled by 'keep_metadata' and the
// availability in 'metadata'. Returns true on success.
// For details see doc/webp-container-spec.txt#extended-file-format.
static int WriteWebPWithMetadata(FILE *const out,
                                 const WebPPicture *const picture,
                                 const WebPMemoryWriter *const memory_writer,
                                 const Metadata *const metadata,
                                 int keep_metadata,
                                 int *const metadata_written)
{
  const char kVP8XHeader[] = "VP8X\x0a\x00\x00\x00";
  const int kAlphaFlag = 0x10;
  const int kEXIFFlag = 0x08;
  const int kICCPFlag = 0x20;
  const int kXMPFlag = 0x04;
  const size_t kRiffHeaderSize = 12;
  const size_t kMaxChunkPayload = ~0 - kChunkHeaderSize - 1;
  const size_t kMinSize = kRiffHeaderSize + kChunkHeaderSize;
  uint32_t flags = 0;
  uint64_t metadata_size = 0;
  const int write_exif = UpdateFlagsAndSize(&metadata->exif,
                                            !!(keep_metadata & METADATA_EXIF),
                                            kEXIFFlag, &flags, &metadata_size);
  const int write_iccp = UpdateFlagsAndSize(&metadata->iccp,
                                            !!(keep_metadata & METADATA_ICC),
                                            kICCPFlag, &flags, &metadata_size);
  const int write_xmp = UpdateFlagsAndSize(&metadata->xmp,
                                           !!(keep_metadata & METADATA_XMP),
                                           kXMPFlag, &flags, &metadata_size);
  uint8_t *webp = memory_writer->mem;
  size_t webp_size = memory_writer->size;

  *metadata_written = 0;

  if (webp_size < kMinSize)
    return 0;
  if (webp_size - kChunkHeaderSize + metadata_size > kMaxChunkPayload)
  {
    fprintf(stderr, "Error! Addition of metadata would exceed "
                    "container size limit.\n");
    return 0;
  }

  if (metadata_size > 0)
  {
    const int kVP8XChunkSize = 18;
    const int has_vp8x = !memcmp(webp + kRiffHeaderSize, "VP8X", kTagSize);
    const uint32_t riff_size = (uint32_t)(webp_size - kChunkHeaderSize +
                                          (has_vp8x ? 0 : kVP8XChunkSize) +
                                          metadata_size);
    // RIFF
    int ok = (fwrite(webp, kTagSize, 1, out) == 1);
    // RIFF size (file header size is not recorded)
    ok = ok && WriteLE32(out, riff_size);
    webp += kChunkHeaderSize;
    webp_size -= kChunkHeaderSize;
    // WEBP
    ok = ok && (fwrite(webp, kTagSize, 1, out) == 1);
    webp += kTagSize;
    webp_size -= kTagSize;
    if (has_vp8x)
    { // update the existing VP8X flags
      webp[kChunkHeaderSize] |= (uint8_t)(flags & 0xff);
      ok = ok && (fwrite(webp, kVP8XChunkSize, 1, out) == 1);
      webp += kVP8XChunkSize;
      webp_size -= kVP8XChunkSize;
    }
    else
    {
      const int is_lossless = !memcmp(webp, "VP8L", kTagSize);
      if (is_lossless)
      {
        // Presence of alpha is stored in the 37th bit (29th after the
        // signature) of VP8L data.
        if (webp[kChunkHeaderSize + 4] & (1 << 4))
          flags |= kAlphaFlag;
      }
      ok = ok && (fwrite(kVP8XHeader, kChunkHeaderSize, 1, out) == 1);
      ok = ok && WriteLE32(out, flags);
      ok = ok && WriteLE24(out, picture->width - 1);
      ok = ok && WriteLE24(out, picture->height - 1);
    }
    if (write_iccp)
    {
      ok = ok && WriteMetadataChunk(out, "ICCP", &metadata->iccp);
      *metadata_written |= METADATA_ICC;
    }
    // Image
    ok = ok && (fwrite(webp, webp_size, 1, out) == 1);
    if (write_exif)
    {
      ok = ok && WriteMetadataChunk(out, "EXIF", &metadata->exif);
      *metadata_written |= METADATA_EXIF;
    }
    if (write_xmp)
    {
      ok = ok && WriteMetadataChunk(out, "XMP ", &metadata->xmp);
      *metadata_written |= METADATA_XMP;
    }
    return ok;
  }

  // No metadata, just write the original image file.
  return (fwrite(webp, webp_size, 1, out) == 1);
}

//------------------------------------------------------------------------------

static int ProgressReport(int percent, const WebPPicture *const picture)
{
  fprintf(stderr, "[%s]: %3d %%            \r",
          (char *)picture->user_data, percent);
  return 1; // all ok
}

//------------------------------------------------------------------------------

static void HelpShort(void)
{
  printf("Usage:\n\n");
  printf("     cwebp [options] -q quality input.png -o output.webp\n\n");
  printf("where quality is between 0 (poor) to 100 (very good).\n");
  printf("Typical value is around 80.\n\n");
  printf("Try -longhelp for an exhaustive list of advanced options.\n");
}

static void HelpLong(void)
{
  printf("Usage:\n");

  printf(" cwebp [-preset <...>] [options] in_file [-o out_file]\n\n");
  printf("If input size (-s) for an image is not specified, it is\n"
         "assumed to be a PNG, JPEG, TIFF or WebP file.\n");
  printf("Note: Animated PNG and WebP files are not supported.\n");
#ifdef HAVE_WINCODEC_H
  printf("Windows builds can take as input any of the files handled by WIC.\n");
#endif
  printf("\nOptions:\n");
  printf("    -h / -help ............. short help\n");
  printf("    -H / -longhelp ......... long help\n");
  printf("    -q <float> ............. quality factor (0:small..100:big), "
         "default=75\n");
  printf("    -alpha_q <int> ......... transparency-compression quality (0..100),"
         "\n                                                     default=100\n");
  printf("    -preset <string> ....... preset setting, one of:\n");
  printf("                                                        default, photo, picture,\n");
  printf("                                                        drawing, icon, text\n");
  printf("         -preset must come first, as it overwrites other parameters\n");
  printf("    -z <int> ............... activates lossless preset with given\n"
         "                                                     level in [0:fast, ..., 9:slowest]\n");
  printf("\n");
  printf("    -m <int> ............... compression method (0=fast, 6=slowest), "
         "default=4\n");
  printf("    -segments <int> ........ number of segments to use (1..4), "
         "default=4\n");
  printf("    -size <int> ............ target size (in bytes)\n");
  printf("    -psnr <float> .......... target PSNR (in dB. typically: 42)\n");
  printf("\n");
  printf("    -s <int> <int> ......... input size (width x height) for YUV\n");
  printf("    -sns <int> ............. spatial noise shaping (0:off, 100:max), "
         "default=50\n");
  printf("    -f <int> ............... filter strength (0=off..100), "
         "default=60\n");
  printf("    -sharpness <int> ....... "
         "filter sharpness (0:most .. 7:least sharp), default=0\n");
  printf("    -strong ................ use strong filter instead "
         "of simple (default)\n");
  printf("    -nostrong .............. use simple filter instead of strong\n");
  printf("    -sharp_yuv ............. use sharper (and slower) RGB->YUV "
         "conversion\n");
  printf("    -partition_limit <int> . limit quality to fit the 512k limit on\n");
  printf("                                                     "
         "the first partition (0=no degradation ... 100=full)\n");
  printf("    -pass <int> ............ analysis pass number (1..10)\n");
  printf("    -qrange <min> <max> .... specifies the permissible quality range\n"
         "                                                     (default: 0 100)\n");
  printf("    -crop <x> <y> <w> <h> .. crop picture with the given rectangle\n");
  printf("    -resize <w> <h> ........ resize picture (*after* any cropping)\n");
  printf("    -retry <int> ........... can retry conversion after (int) seconds. -1 means never retry.\n");
  printf("    -forcesmall ............ if output file is bigger than input file, then cancel conversion.\n");
  printf("    -mt .................... use multi-threading if available\n");
  printf("    -low_memory ............ reduce memory usage (slower encoding)\n");
  printf("    -map <int> ............. print map of extra info\n");
  printf("    -print_psnr ............ prints averaged PSNR distortion\n");
  printf("    -print_ssim ............ prints averaged SSIM distortion\n");
  printf("    -print_lsim ............ prints local-similarity distortion\n");
  printf("    -d <file.pgm> .......... dump the compressed output (PGM file)\n");
  printf("    -alpha_method <int> .... transparency-compression method (0..1), "
         "default=1\n");
  printf("    -alpha_filter <string> . predictive filtering for alpha plane,\n");
  printf("                                                     one of: none, fast (default) or best\n");
  printf("    -exact ................. preserve RGB values in transparent area, "
         "default=off\n");
  printf("    -blend_alpha <hex> ..... blend colors against background color\n"
         "                                                     expressed as RGB values written in\n"
         "                                                     hexadecimal, e.g. 0xc0e0d0 for red=0xc0\n"
         "                                                     green=0xe0 and blue=0xd0\n");
  printf("    -noalpha ............... discard any transparency information\n");
  printf("    -lossless .............. encode image losslessly, default=off\n");
  printf("    -near_lossless <int> ... use near-lossless image preprocessing\n"
         "                                                     (0..100=off), default=100\n");
  printf("    -hint <string> ......... specify image characteristics hint,\n");
  printf("                                                     one of: photo, picture or graph\n");

  printf("\n");
  printf("    -metadata <string> ..... comma separated list of metadata to\n");
  printf("                                                     ");
  printf("copy from the input to the output if present.\n");
  printf("                                                     "
         "Valid values: all, none (default), exif, icc, xmp\n");

  printf("\n");
  printf("    -short ................. condense printed message\n");
  printf("    -quiet ................. don't print anything\n");
  printf("    -version ............... print version number and exit\n");
#ifndef WEBP_DLL
  printf("    -noasm ................. disable all assembly optimizations\n");
#endif
  printf("    -v ..................... verbose, e.g. print encoding/decoding "
         "times\n");
  printf("    -progress .............. report encoding progress\n");
  printf("\n");
  printf("Experimental Options:\n");
  printf("    -jpeg_like ............. roughly match expected JPEG size\n");
  printf("    -af .................... auto-adjust filter strength\n");
  printf("    -pre <int> ............. pre-processing filter\n");
  printf("\n");
  printf("Supported input formats:\n    %s, APNG, GIF\n", WebPGetEnabledInputFileFormats());
}

//------------------------------------------------------------------------------
// Error messages

static const char *const kErrorMessages[VP8_ENC_ERROR_LAST] = {
    "OK",
    "OUT_OF_MEMORY: Out of memory allocating objects",
    "BITSTREAM_OUT_OF_MEMORY: Out of memory re-allocating byte buffer",
    "NULL_PARAMETER: NULL parameter passed to function",
    "INVALID_CONFIGURATION: configuration is invalid",
    "BAD_DIMENSION: Bad picture dimension. Maximum width and height "
    "allowed is 16383 pixels.",
    "PARTITION0_OVERFLOW: Partition #0 is too big to fit 512k.\n"
    "To reduce the size of this partition, try using less segments "
    "with the -segments option, and eventually reduce the number of "
    "header bits using -partition_limit. More details are available "
    "in the manual (`man cwebp`)",
    "PARTITION_OVERFLOW: Partition is too big to fit 16M",
    "BAD_WRITE: Picture writer returned an I/O error",
    "FILE_TOO_BIG: File would be too big to fit in 4G",
    "USER_ABORT: encoding abort requested by user"};

//------------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
  int return_value = -1;
  time_t start_time = time(NULL);
  const char *in_file = NULL, *out_file = NULL, *dump_file = NULL;
  FILE *out = NULL;
  int c;

  int short_output = 0;
  int quiet = 0;
  int keep_alpha = 1;
  int blend_alpha = 0;
  uint32_t background_color = 0xffffffu;
  int crop = 0, crop_x = 0, crop_y = 0, crop_w = 0, crop_h = 0;
  int resize_w = 0, resize_h = 0;
  int lossless_preset = 6;
  int use_lossless_preset = -1; // -1=unset, 0=don't use, 1=use it
  int show_progress = 0;
  int keep_metadata = 0;
  int metadata_written = 0;
  WebPPicture picture;
  int print_distortion = -1;    // -1=off, 0=PSNR, 1=SSIM, 2=LSIM
  WebPPicture original_picture; // when PSNR or SSIM is requested
  WebPConfig config;
  WebPAuxStats stats;
  WebPMemoryWriter memory_writer;
  int use_memory_writer;
  Metadata metadata;
  Stopwatch stop_watch;

  config.method = 3;
  config.quality = 80;

  INIT_WARGV(argc, argv);

  MetadataInit(&metadata);
  WebPMemoryWriterInit(&memory_writer);
  if (!WebPPictureInit(&picture) ||
      !WebPPictureInit(&original_picture) ||
      !WebPConfigInit(&config))
  {
    fprintf(stderr, "Error! Version mismatch!\n");
    FREE_WARGV_AND_RETURN(-1);
  }

  if (argc == 1)
  {
    HelpShort();
    FREE_WARGV_AND_RETURN(0);
  }

  for (c = 1; c < argc; ++c)
  {
    int parse_error = 0;
    if (!strcmp(argv[c], "-h") || !strcmp(argv[c], "-help"))
    {
      HelpShort();
      FREE_WARGV_AND_RETURN(0);
    }
    else if (!strcmp(argv[c], "-H") || !strcmp(argv[c], "-longhelp"))
    {
      HelpLong();
      FREE_WARGV_AND_RETURN(0);
    }
    else if (!strcmp(argv[c], "-o") && c + 1 < argc)
    {
      out_file = (const char *)GET_WARGV(argv, ++c);
    }
    else if (!strcmp(argv[c], "-d") && c + 1 < argc)
    {
      dump_file = (const char *)GET_WARGV(argv, ++c);
      config.show_compressed = 1;
    }
    else if (!strcmp(argv[c], "-print_psnr"))
    {
      config.show_compressed = 1;
      print_distortion = 0;
    }
    else if (!strcmp(argv[c], "-print_ssim"))
    {
      config.show_compressed = 1;
      print_distortion = 1;
    }
    else if (!strcmp(argv[c], "-print_lsim"))
    {
      config.show_compressed = 1;
      print_distortion = 2;
    }
    else if (!strcmp(argv[c], "-short"))
    {
      ++short_output;
    }
    else if (!strcmp(argv[c], "-s") && c + 2 < argc)
    {
      picture.width = ExUtilGetInt(argv[++c], 0, &parse_error);
      picture.height = ExUtilGetInt(argv[++c], 0, &parse_error);
      if (picture.width > WEBP_MAX_DIMENSION || picture.width < 0 ||
          picture.height > WEBP_MAX_DIMENSION || picture.height < 0)
      {
        fprintf(stderr,
                "Specified dimension (%d x %d) is out of range.\n",
                picture.width, picture.height);
        goto Error;
      }
    }
    else if (!strcmp(argv[c], "-m") && c + 1 < argc)
    {
      config.method = ExUtilGetInt(argv[++c], 0, &parse_error);
      use_lossless_preset = 0; // disable -z option
    }
    else if (!strcmp(argv[c], "-q") && c + 1 < argc)
    {
      config.quality = ExUtilGetFloat(argv[++c], &parse_error);
      use_lossless_preset = 0; // disable -z option
    }
    else if (!strcmp(argv[c], "-z") && c + 1 < argc)
    {
      lossless_preset = ExUtilGetInt(argv[++c], 0, &parse_error);
      if (use_lossless_preset != 0)
        use_lossless_preset = 1;
    }
    else if (!strcmp(argv[c], "-alpha_q") && c + 1 < argc)
    {
      config.alpha_quality = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-alpha_method") && c + 1 < argc)
    {
      config.alpha_compression = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-alpha_cleanup"))
    {
      // This flag is obsolete, does opposite of -exact.
      config.exact = 0;
    }
    else if (!strcmp(argv[c], "-exact"))
    {
      config.exact = 1;
    }
    else if (!strcmp(argv[c], "-blend_alpha") && c + 1 < argc)
    {
      blend_alpha = 1;
      // background color is given in hex with an optional '0x' prefix
      background_color = ExUtilGetInt(argv[++c], 16, &parse_error);
      background_color = background_color & 0x00ffffffu;
    }
    else if (!strcmp(argv[c], "-alpha_filter") && c + 1 < argc)
    {
      ++c;
      if (!strcmp(argv[c], "none"))
      {
        config.alpha_filtering = 0;
      }
      else if (!strcmp(argv[c], "fast"))
      {
        config.alpha_filtering = 1;
      }
      else if (!strcmp(argv[c], "best"))
      {
        config.alpha_filtering = 2;
      }
      else
      {
        fprintf(stderr, "Error! Unrecognized alpha filter: %s\n", argv[c]);
        goto Error;
      }
    }
    else if (!strcmp(argv[c], "-noalpha"))
    {
      keep_alpha = 0;
    }
    else if (!strcmp(argv[c], "-lossless"))
    {
      config.lossless = 1;
    }
    else if (!strcmp(argv[c], "-near_lossless") && c + 1 < argc)
    {
      config.near_lossless = ExUtilGetInt(argv[++c], 0, &parse_error);
      config.lossless = 1; // use near-lossless only with lossless
    }
    else if (!strcmp(argv[c], "-hint") && c + 1 < argc)
    {
      ++c;
      if (!strcmp(argv[c], "photo"))
      {
        config.image_hint = WEBP_HINT_PHOTO;
      }
      else if (!strcmp(argv[c], "picture"))
      {
        config.image_hint = WEBP_HINT_PICTURE;
      }
      else if (!strcmp(argv[c], "graph"))
      {
        config.image_hint = WEBP_HINT_GRAPH;
      }
      else
      {
        fprintf(stderr, "Error! Unrecognized image hint: %s\n", argv[c]);
        goto Error;
      }
    }
    else if (!strcmp(argv[c], "-size") && c + 1 < argc)
    {
      config.target_size = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-psnr") && c + 1 < argc)
    {
      config.target_PSNR = ExUtilGetFloat(argv[++c], &parse_error);
    }
    else if (!strcmp(argv[c], "-sns") && c + 1 < argc)
    {
      config.sns_strength = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-f") && c + 1 < argc)
    {
      config.filter_strength = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-retry") && c + 1 < argc)
    {
      retry = ExUtilGetInt(argv[++c], 0, &parse_error);
      if(1800 > retry) {
        retry = 1800;
        printf("retry must bigger than 1800, system set retry=1800 automatically.\n");
      }
    }
    else if (!strcmp(argv[c], "-af"))
    {
      config.autofilter = 1;
    }
    else if (!strcmp(argv[c], "-jpeg_like"))
    {
      config.emulate_jpeg_size = 1;
    }
    else if (!strcmp(argv[c], "-mt"))
    {
      ++config.thread_level; // increase thread level
    }
    else if (!strcmp(argv[c], "-forcesmall"))
    {
      forcesmall=1;
    }
    else if (!strcmp(argv[c], "-low_memory"))
    {
      config.low_memory = 1;
    }
    else if (!strcmp(argv[c], "-strong"))
    {
      config.filter_type = 1;
    }
    else if (!strcmp(argv[c], "-nostrong"))
    {
      config.filter_type = 0;
    }
    else if (!strcmp(argv[c], "-sharpness") && c + 1 < argc)
    {
      config.filter_sharpness = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-sharp_yuv"))
    {
      config.use_sharp_yuv = 1;
    }
    else if (!strcmp(argv[c], "-pass") && c + 1 < argc)
    {
      config.pass = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-qrange") && c + 2 < argc)
    {
      config.qmin = ExUtilGetInt(argv[++c], 0, &parse_error);
      config.qmax = ExUtilGetInt(argv[++c], 0, &parse_error);
      if (config.qmin < 0)
        config.qmin = 0;
      if (config.qmax > 100)
        config.qmax = 100;
    }
    else if (!strcmp(argv[c], "-pre") && c + 1 < argc)
    {
      config.preprocessing = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-segments") && c + 1 < argc)
    {
      config.segments = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-partition_limit") && c + 1 < argc)
    {
      config.partition_limit = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-map") && c + 1 < argc)
    {
      picture.extra_info_type = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-crop") && c + 4 < argc)
    {
      crop = 1;
      crop_x = ExUtilGetInt(argv[++c], 0, &parse_error);
      crop_y = ExUtilGetInt(argv[++c], 0, &parse_error);
      crop_w = ExUtilGetInt(argv[++c], 0, &parse_error);
      crop_h = ExUtilGetInt(argv[++c], 0, &parse_error);
    }
    else if (!strcmp(argv[c], "-resize") && c + 2 < argc)
    {
      resize_w = ExUtilGetInt(argv[++c], 0, &parse_error);
      resize_h = ExUtilGetInt(argv[++c], 0, &parse_error);
#ifndef WEBP_DLL
    }
    else if (!strcmp(argv[c], "-noasm"))
    {
      VP8GetCPUInfo = NULL;
#endif
    }
    else if (!strcmp(argv[c], "-version"))
    {
      const int version = WebPGetEncoderVersion();
      const int sharpyuv_version = SharpYuvGetVersion();
      printf("%d.%d.%d\n",
             (version >> 16) & 0xff, (version >> 8) & 0xff, version & 0xff);
      printf("libsharpyuv: %d.%d.%d\n",
             (sharpyuv_version >> 24) & 0xff, (sharpyuv_version >> 16) & 0xffff,
             sharpyuv_version & 0xff);
      printf("xwebversion:20230818\n");
      FREE_WARGV_AND_RETURN(0);
    }
    else if (!strcmp(argv[c], "-progress"))
    {
      show_progress = 1;
    }
    else if (!strcmp(argv[c], "-quiet"))
    {
      quiet = 1;
    }
    else if (!strcmp(argv[c], "-preset") && c + 1 < argc)
    {
      WebPPreset preset;
      ++c;
      if (!strcmp(argv[c], "default"))
      {
        preset = WEBP_PRESET_DEFAULT;
      }
      else if (!strcmp(argv[c], "photo"))
      {
        preset = WEBP_PRESET_PHOTO;
      }
      else if (!strcmp(argv[c], "picture"))
      {
        preset = WEBP_PRESET_PICTURE;
      }
      else if (!strcmp(argv[c], "drawing"))
      {
        preset = WEBP_PRESET_DRAWING;
      }
      else if (!strcmp(argv[c], "icon"))
      {
        preset = WEBP_PRESET_ICON;
      }
      else if (!strcmp(argv[c], "text"))
      {
        preset = WEBP_PRESET_TEXT;
      }
      else
      {
        fprintf(stderr, "Error! Unrecognized preset: %s\n", argv[c]);
        goto Error;
      }
      if (!WebPConfigPreset(&config, preset, config.quality))
      {
        fprintf(stderr, "Error! Could initialize configuration with preset.\n");
        goto Error;
      }
    }
    else if (!strcmp(argv[c], "-metadata") && c + 1 < argc)
    {
      static const struct
      {
        const char *option;
        int flag;
      } kTokens[] = {
          {"all", METADATA_ALL},
          {"none", 0},
          {"exif", METADATA_EXIF},
          {"icc", METADATA_ICC},
          {"xmp", METADATA_XMP},
      };
      const size_t kNumTokens = sizeof(kTokens) / sizeof(kTokens[0]);
      const char *start = argv[++c];
      const char *const end = start + strlen(start);

      while (start < end)
      {
        size_t i;
        const char *token = strchr(start, ',');
        if (token == NULL)
          token = end;

        for (i = 0; i < kNumTokens; ++i)
        {
          if ((size_t)(token - start) == strlen(kTokens[i].option) &&
              !strncmp(start, kTokens[i].option, strlen(kTokens[i].option)))
          {
            if (kTokens[i].flag != 0)
            {
              keep_metadata |= kTokens[i].flag;
            }
            else
            {
              keep_metadata = 0;
            }
            break;
          }
        }
        if (i == kNumTokens)
        {
          fprintf(stderr, "Error! Unknown metadata type '%.*s'\n",
                  (int)(token - start), start);
          FREE_WARGV_AND_RETURN(-1);
        }
        start = token + 1;
      }
#ifdef HAVE_WINCODEC_H
      if (keep_metadata != 0 && keep_metadata != METADATA_ICC)
      {
        // TODO(jzern): remove when -metadata is supported on all platforms.
        fprintf(stderr, "Warning: only ICC profile extraction is currently"
                        " supported on this platform!\n");
      }
#endif
    }
    else if (!strcmp(argv[c], "-v"))
    {
      verbose = 1;
    }
    else if (!strcmp(argv[c], "--"))
    {
      if (c + 1 < argc)
        in_file = (const char *)GET_WARGV(argv, ++c);
      break;
    }
    else if (argv[c][0] == '-')
    {
      fprintf(stderr, "Error! Unknown option '%s'\n", argv[c]);
      HelpLong();
      FREE_WARGV_AND_RETURN(-1);
    }
    else
    {
      in_file = (const char *)GET_WARGV(argv, c);
    }

    if (parse_error)
    {
      HelpLong();
      FREE_WARGV_AND_RETURN(-1);
    }
  }
  if (in_file == NULL)
  {
    fprintf(stderr, "No input file specified!\n");
    HelpShort();
    goto Error;
  }

  if (use_lossless_preset == 1)
  {
    if (!WebPConfigLosslessPreset(&config, lossless_preset))
    {
      fprintf(stderr, "Invalid lossless preset (-z %d)\n", lossless_preset);
      goto Error;
    }
  }

  // Check for unsupported command line options for lossless mode and log
  // warning for such options.
  if (!quiet && config.lossless == 1)
  {
    if (config.target_size > 0 || config.target_PSNR > 0)
    {
      fprintf(stderr, "Encoding for specified size or PSNR is not supported"
                      " for lossless encoding. Ignoring such option(s)!\n");
    }
    if (config.partition_limit > 0)
    {
      fprintf(stderr, "Partition limit option is not required for lossless"
                      " encoding. Ignoring this option!\n");
    }
  }
  // If a target size or PSNR was given, but somehow the -pass option was
  // omitted, force a reasonable value.
  if (config.target_size > 0 || config.target_PSNR > 0)
  {
    if (config.pass == 1)
      config.pass = 6;
  }

  if (!WebPValidateConfig(&config))
  {
    fprintf(stderr, "Error! Invalid configuration.\n");
    goto Error;
  }

  // Read the input. We need to decide if we prefer ARGB or YUVA
  // samples, depending on the expected compression mode (this saves
  // some conversion steps).
  picture.use_argb = (config.lossless || config.use_sharp_yuv ||
                      config.preprocessing > 0 ||
                      crop || (resize_w | resize_h) > 0);
  if (verbose)
  {
    StopwatchReset(&stop_watch);
  }

  /*判断in_file out_file:
      1:新in_file 无out_file 或 异常out_file： 转换 或 不转换
      2:老的in_file out_file         ：不转换，直接返回 退出
      3:新的in_file 老的out_file : 转换
  */
  if (0 != statfile(in_file, out_file))
  {
    return -1;
  }
  else{
    out_file = temp_file_p; //point to temp file
  }

  /* 判断apng */
  AVFormatContext *format_ctx = NULL;
  if (avformat_open_input(&format_ctx, in_file, NULL, NULL) != 0)
  {
    printf("Failed to open file:%s.\n", in_file);
    goto Error;
  }

  if (0 == strcmp("apng", format_ctx->iformat->name))
  { // apng
    if (0 != apng_to_webp(in_file, out_file, config))
    {
      printf("Failed to convert APNG to WEBP.\n");
      goto Error;
    }

    avformat_close_input(&format_ctx);
    printf("Succeeded to convert APNG to WEBP.\n");
    return_value = 0;
    goto Error;
  }
  else if (0 == strcmp("gif", format_ctx->iformat->name))
  { //gif
    if (0 != gif_to_webp(in_file, out_file, config))
    {
      printf("Failed to convert GIF to WEBP.\n");
      goto Error;
    }

    avformat_close_input(&format_ctx);
    printf("Succeeded to convert GIF to WEBP.\n");

    return_value = 0;
    goto Error;
  }

  if (!ReadPicture(in_file, &picture, keep_alpha,
                   (keep_metadata == 0) ? NULL : &metadata))
  {
    WFPRINTF(stderr, "Error! Cannot read input picture file '%s'\n",
             (const W_CHAR *)in_file);
    goto Error;
  }

  picture.progress_hook = (show_progress && !quiet) ? ProgressReport : NULL;

  if (blend_alpha)
  {
    WebPBlendAlpha(&picture, background_color);
  }

  if (verbose)
  {
    const double read_time = StopwatchReadAndReset(&stop_watch);
    fprintf(stderr, "Time to read input: %.3fs\n", read_time);
  }

  // The bitstream should be kept in memory when metadata must be appended
  // before writing it to a file/stream, and/or when the near-losslessly encoded
  // bitstream must be decoded for distortion computation (lossy will modify the
  // 'picture' but not the lossless pipeline).
  // Otherwise directly write the bitstream to a file.
  use_memory_writer = (out_file != NULL && keep_metadata) ||
                      (!quiet && print_distortion >= 0 && config.lossless &&
                       config.near_lossless < 100);

  // Open the output
  if (out_file != NULL)
  {
    const int use_stdout = !WSTRCMP(out_file, "-");
    out = use_stdout ? ImgIoUtilSetBinaryMode(stdout) : WFOPEN(out_file, "wb");
    if (out == NULL)
    {
      WFPRINTF(stderr, "Error! Cannot open output file '%s'\n",
               (const W_CHAR *)out_file);
      goto Error;
    }
    else
    {
      if (!short_output && !quiet)
      {
        WFPRINTF(stderr, "Saving file '%s'\n", (const W_CHAR *)out_file);
      }
    }
    if (use_memory_writer)
    {
      picture.writer = WebPMemoryWrite;
      picture.custom_ptr = (void *)&memory_writer;
    }
    else
    {
      picture.writer = MyWriter;
      picture.custom_ptr = (void *)out;
    }
  }
  else
  {
    out = NULL;
    if (use_memory_writer)
    {
      picture.writer = WebPMemoryWrite;
      picture.custom_ptr = (void *)&memory_writer;
    }
    if (!quiet && !short_output)
    {
      fprintf(stderr, "No output file specified (no -o flag). Encoding will\n");
      fprintf(stderr, "be performed, but its results discarded.\n\n");
    }
  }
  if (!quiet)
  {
    picture.stats = &stats;
    picture.user_data = (void *)in_file;
  }

  // Crop & resize.
  if (verbose)
  {
    StopwatchReset(&stop_watch);
  }
  if (crop != 0)
  {
    // We use self-cropping using a view.
    if (!WebPPictureView(&picture, crop_x, crop_y, crop_w, crop_h, &picture))
    {
      fprintf(stderr, "Error! Cannot crop picture\n");
      goto Error;
    }
  }
  if ((resize_w | resize_h) > 0)
  {
    WebPPicture picture_no_alpha;
    if (config.exact)
    {
      // If -exact, we can't premultiply RGB by A otherwise RGB is lost if A=0.
      // We rescale an opaque copy and assemble scaled A and non-premultiplied
      // RGB channels. This is slower but it's a very uncommon use case. Color
      // leak at sharp alpha edges is possible.
      if (!WebPPictureCopy(&picture, &picture_no_alpha))
      {
        fprintf(stderr, "Error! Cannot copy temporary picture\n");
        goto Error;
      }

      // We enforced picture.use_argb = 1 above. Now, remove the alpha values.
      {
        int x, y;
        uint32_t *argb_no_alpha = picture_no_alpha.argb;
        for (y = 0; y < picture_no_alpha.height; ++y)
        {
          for (x = 0; x < picture_no_alpha.width; ++x)
          {
            argb_no_alpha[x] |= 0xff000000; // Opaque copy.
          }
          argb_no_alpha += picture_no_alpha.argb_stride;
        }
      }

      if (!WebPPictureRescale(&picture_no_alpha, resize_w, resize_h))
      {
        fprintf(stderr, "Error! Cannot resize temporary picture\n");
        goto Error;
      }
    }

    if (!WebPPictureRescale(&picture, resize_w, resize_h))
    {
      fprintf(stderr, "Error! Cannot resize picture\n");
      goto Error;
    }

    if (config.exact)
    { // Put back the alpha information.
      int x, y;
      uint32_t *argb_no_alpha = picture_no_alpha.argb;
      uint32_t *argb = picture.argb;
      for (y = 0; y < picture_no_alpha.height; ++y)
      {
        for (x = 0; x < picture_no_alpha.width; ++x)
        {
          argb[x] = (argb[x] & 0xff000000) | (argb_no_alpha[x] & 0x00ffffff);
        }
        argb_no_alpha += picture_no_alpha.argb_stride;
        argb += picture.argb_stride;
      }
      WebPPictureFree(&picture_no_alpha);
    }
  }
  if (verbose && (crop != 0 || (resize_w | resize_h) > 0))
  {
    const double preproc_time = StopwatchReadAndReset(&stop_watch);
    fprintf(stderr, "Time to crop/resize picture: %.3fs\n", preproc_time);
  }

  if (picture.extra_info_type > 0)
  {
    AllocExtraInfo(&picture);
  }
  // Save original picture for later comparison. Only for lossy as lossless does
  // not modify 'picture' (even near-lossless).
  if (print_distortion >= 0 && !config.lossless &&
      !WebPPictureCopy(&picture, &original_picture))
  {
    fprintf(stderr, "Error! Cannot copy temporary picture\n");
    goto Error;
  }

  // Compress.
  if (verbose)
  {
    StopwatchReset(&stop_watch);
  }
  if (!WebPEncode(&config, &picture))
  {
    fprintf(stderr, "Error! Cannot encode picture as WebP\n");
    fprintf(stderr, "Error code: %d (%s)\n",
            picture.error_code, kErrorMessages[picture.error_code]);
    goto Error;
  }
  if (verbose)
  {
    const double encode_time = StopwatchReadAndReset(&stop_watch);
    fprintf(stderr, "Time to encode picture: %.3fs\n", encode_time);
  }

  // Get the decompressed image for the lossless pipeline.
  if (!quiet && print_distortion >= 0 && config.lossless)
  {
    if (config.near_lossless == 100)
    {
      // Pure lossless: image was not modified, make 'original_picture' a view
      // of 'picture' by copying all members except the freeable pointers.
      original_picture = picture;
      original_picture.memory_ = original_picture.memory_argb_ = NULL;
    }
    else
    {
      // Decode the bitstream stored in 'memory_writer' to get the altered image
      // to 'picture'; save the 'original_picture' beforehand.
      assert(use_memory_writer);
      original_picture = picture;
      if (!WebPPictureInit(&picture))
      { // Do not free 'picture'.
        fprintf(stderr, "Error! Version mismatch!\n");
        goto Error;
      }

      picture.use_argb = 1;
      if (!ReadWebP(
              memory_writer.mem, memory_writer.size, &picture,
              /*keep_alpha=*/WebPPictureHasTransparency(&original_picture),
              /*metadata=*/NULL))
      {
        fprintf(stderr, "Error! Cannot decode encoded WebP bitstream\n");
        fprintf(stderr, "Error code: %d (%s)\n", picture.error_code,
                kErrorMessages[picture.error_code]);
        goto Error;
      }
      picture.stats = original_picture.stats;
    }
    original_picture.stats = NULL;
  }

  // Write the YUV planes to a PGM file. Only available for lossy.
  if (dump_file)
  {
    if (picture.use_argb)
    {
      fprintf(stderr, "Warning: can't dump file (-d option) "
                      "in lossless mode.\n");
    }
    else if (!DumpPicture(&picture, dump_file))
    {
      WFPRINTF(stderr, "Warning, couldn't dump picture %s\n",
               (const W_CHAR *)dump_file);
    }
  }

  if (use_memory_writer && out != NULL &&
      !WriteWebPWithMetadata(out, &picture, &memory_writer, &metadata,
                             keep_metadata, &metadata_written))
  {
    fprintf(stderr, "Error writing WebP file!\n");
    goto Error;
  }

  if (out == NULL && keep_metadata)
  {
    // output is disabled, just display the metadata stats.
    const struct
    {
      const MetadataPayload *const payload;
      int flag;
    } *iter, info[] = {{&metadata.exif, METADATA_EXIF},
                       {&metadata.iccp, METADATA_ICC},
                       {&metadata.xmp, METADATA_XMP},
                       {NULL, 0}};
    uint32_t unused1 = 0;
    uint64_t unused2 = 0;

    for (iter = info; iter->payload != NULL; ++iter)
    {
      if (UpdateFlagsAndSize(iter->payload, !!(keep_metadata & iter->flag),
                             /*flag=*/0, &unused1, &unused2))
      {
        metadata_written |= iter->flag;
      }
    }
  }

  if (!quiet)
  {
    if (!short_output || print_distortion < 0)
    {
      if (config.lossless)
      {
        PrintExtraInfoLossless(&picture, short_output, in_file);
      }
      else
      {
        PrintExtraInfoLossy(&picture, short_output, config.low_memory, in_file);
      }
    }
    if (!short_output && picture.extra_info_type > 0)
    {
      PrintMapInfo(&picture);
    }
    if (print_distortion >= 0)
    { // print distortion
      static const char *distortion_names[] = {"PSNR", "SSIM", "LSIM"};
      float values[5];
      if (!WebPPictureDistortion(&picture, &original_picture,
                                 print_distortion, values))
      {
        fprintf(stderr, "Error while computing the distortion.\n");
        goto Error;
      }
      if (!short_output)
      {
        fprintf(stderr, "%s: ", distortion_names[print_distortion]);
        fprintf(stderr, "B:%.2f G:%.2f R:%.2f A:%.2f    Total:%.2f\n",
                values[0], values[1], values[2], values[3], values[4]);
      }
      else
      {
        fprintf(stderr, "%7d %.4f\n", picture.stats->coded_size, values[4]);
      }
    }
    if (!short_output)
    {
      PrintMetadataInfo(&metadata, metadata_written);
    }
  }
  return_value = 0;

Error:
  WebPMemoryWriterClear(&memory_writer);
  WebPFree(picture.extra_info);
  MetadataFree(&metadata);
  WebPPictureFree(&picture);
  WebPPictureFree(&original_picture);
  if (out != NULL && out != stdout)
  {
    fclose(out);
  }

  if(0 == return_value){
    setSucFileStat();
  }
  else{
    setFailFileStat();
  }
  
  printf("cost: %d sec\n", time(NULL)-start_time);
  FREE_WARGV_AND_RETURN(return_value);
}

//------------------------------------------------------------------------------
