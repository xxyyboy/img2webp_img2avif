# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake3

# The command to remove a file.
RM = /usr/bin/cmake3 -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build

# Include any dependencies generated for this target.
include CMakeFiles/lossless_encoder.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/lossless_encoder.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lossless_encoder.dir/flags.make

CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.o: CMakeFiles/lossless_encoder.dir/flags.make
CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.o: ../examples/lossless_encoder.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.o"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.o   -c /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/examples/lossless_encoder.c

CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.i"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/examples/lossless_encoder.c > CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.i

CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.s"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/examples/lossless_encoder.c -o CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.s

# Object files for target lossless_encoder
lossless_encoder_OBJECTS = \
"CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.o"

# External object files for target lossless_encoder
lossless_encoder_EXTERNAL_OBJECTS = \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/av1/arg_defs.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/args_helper.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/args.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/av1_config.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/md5_utils.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/tools_common.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/rawenc.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/y4menc.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/ivfdec.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_encoder_app_util.dir/common/ivfenc.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_encoder_app_util.dir/common/video_writer.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_encoder_app_util.dir/common/warnings.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_encoder_app_util.dir/common/y4minput.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_encoder_app_util.dir/examples/encoder_util.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_encoder_app_util.dir/common/webmenc.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/convert_argb.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/cpu_id.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/planar_functions.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/row_any.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/row_common.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/row_gcc.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/row_mips.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/row_neon.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/row_neon64.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/row_win.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/scale.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/scale_any.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/scale_common.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/scale_gcc.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/scale_mips.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/scale_neon.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/scale_neon64.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/scale_win.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/yuv.dir/third_party/libyuv/source/scale_uv.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/webm.dir/third_party/libwebm/common/hdr_util.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvmuxer.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvmuxerutil.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvwriter.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/webm.dir/third_party/libwebm/mkvparser/mkvparser.cc.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/webm.dir/third_party/libwebm/mkvparser/mkvreader.cc.o"

examples/lossless_encoder: CMakeFiles/lossless_encoder.dir/examples/lossless_encoder.c.o
examples/lossless_encoder: CMakeFiles/aom_common_app_util.dir/av1/arg_defs.c.o
examples/lossless_encoder: CMakeFiles/aom_common_app_util.dir/common/args_helper.c.o
examples/lossless_encoder: CMakeFiles/aom_common_app_util.dir/common/args.c.o
examples/lossless_encoder: CMakeFiles/aom_common_app_util.dir/common/av1_config.c.o
examples/lossless_encoder: CMakeFiles/aom_common_app_util.dir/common/md5_utils.c.o
examples/lossless_encoder: CMakeFiles/aom_common_app_util.dir/common/tools_common.c.o
examples/lossless_encoder: CMakeFiles/aom_common_app_util.dir/common/rawenc.c.o
examples/lossless_encoder: CMakeFiles/aom_common_app_util.dir/common/y4menc.c.o
examples/lossless_encoder: CMakeFiles/aom_common_app_util.dir/common/ivfdec.c.o
examples/lossless_encoder: CMakeFiles/aom_encoder_app_util.dir/common/ivfenc.c.o
examples/lossless_encoder: CMakeFiles/aom_encoder_app_util.dir/common/video_writer.c.o
examples/lossless_encoder: CMakeFiles/aom_encoder_app_util.dir/common/warnings.c.o
examples/lossless_encoder: CMakeFiles/aom_encoder_app_util.dir/common/y4minput.c.o
examples/lossless_encoder: CMakeFiles/aom_encoder_app_util.dir/examples/encoder_util.c.o
examples/lossless_encoder: CMakeFiles/aom_encoder_app_util.dir/common/webmenc.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/convert_argb.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/cpu_id.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/planar_functions.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/row_any.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/row_common.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/row_gcc.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/row_mips.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/row_neon.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/row_neon64.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/row_win.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/scale.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_any.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_common.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_gcc.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_mips.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_neon.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_neon64.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_win.cc.o
examples/lossless_encoder: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_uv.cc.o
examples/lossless_encoder: CMakeFiles/webm.dir/third_party/libwebm/common/hdr_util.cc.o
examples/lossless_encoder: CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvmuxer.cc.o
examples/lossless_encoder: CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvmuxerutil.cc.o
examples/lossless_encoder: CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvwriter.cc.o
examples/lossless_encoder: CMakeFiles/webm.dir/third_party/libwebm/mkvparser/mkvparser.cc.o
examples/lossless_encoder: CMakeFiles/webm.dir/third_party/libwebm/mkvparser/mkvreader.cc.o
examples/lossless_encoder: CMakeFiles/lossless_encoder.dir/build.make
examples/lossless_encoder: libaom.a
examples/lossless_encoder: CMakeFiles/lossless_encoder.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable examples/lossless_encoder"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lossless_encoder.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lossless_encoder.dir/build: examples/lossless_encoder

.PHONY : CMakeFiles/lossless_encoder.dir/build

CMakeFiles/lossless_encoder.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lossless_encoder.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lossless_encoder.dir/clean

CMakeFiles/lossless_encoder.dir/depend:
	cd /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/lossless_encoder.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lossless_encoder.dir/depend
