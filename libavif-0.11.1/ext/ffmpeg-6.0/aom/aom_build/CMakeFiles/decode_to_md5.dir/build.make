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
include CMakeFiles/decode_to_md5.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/decode_to_md5.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/decode_to_md5.dir/flags.make

CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.o: CMakeFiles/decode_to_md5.dir/flags.make
CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.o: ../examples/decode_to_md5.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.o"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.o   -c /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/examples/decode_to_md5.c

CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.i"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/examples/decode_to_md5.c > CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.i

CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.s"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/examples/decode_to_md5.c -o CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.s

# Object files for target decode_to_md5
decode_to_md5_OBJECTS = \
"CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.o"

# External object files for target decode_to_md5
decode_to_md5_EXTERNAL_OBJECTS = \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/av1/arg_defs.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/args_helper.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/args.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/av1_config.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/md5_utils.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/tools_common.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/rawenc.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/y4menc.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/ivfdec.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.o" \
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

examples/decode_to_md5: CMakeFiles/decode_to_md5.dir/examples/decode_to_md5.c.o
examples/decode_to_md5: CMakeFiles/aom_common_app_util.dir/av1/arg_defs.c.o
examples/decode_to_md5: CMakeFiles/aom_common_app_util.dir/common/args_helper.c.o
examples/decode_to_md5: CMakeFiles/aom_common_app_util.dir/common/args.c.o
examples/decode_to_md5: CMakeFiles/aom_common_app_util.dir/common/av1_config.c.o
examples/decode_to_md5: CMakeFiles/aom_common_app_util.dir/common/md5_utils.c.o
examples/decode_to_md5: CMakeFiles/aom_common_app_util.dir/common/tools_common.c.o
examples/decode_to_md5: CMakeFiles/aom_common_app_util.dir/common/rawenc.c.o
examples/decode_to_md5: CMakeFiles/aom_common_app_util.dir/common/y4menc.c.o
examples/decode_to_md5: CMakeFiles/aom_common_app_util.dir/common/ivfdec.c.o
examples/decode_to_md5: CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.o
examples/decode_to_md5: CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.o
examples/decode_to_md5: CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/convert_argb.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/cpu_id.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/planar_functions.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/row_any.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/row_common.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/row_gcc.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/row_mips.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/row_neon.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/row_neon64.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/row_win.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/scale.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_any.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_common.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_gcc.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_mips.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_neon.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_neon64.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_win.cc.o
examples/decode_to_md5: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_uv.cc.o
examples/decode_to_md5: CMakeFiles/webm.dir/third_party/libwebm/common/hdr_util.cc.o
examples/decode_to_md5: CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvmuxer.cc.o
examples/decode_to_md5: CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvmuxerutil.cc.o
examples/decode_to_md5: CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvwriter.cc.o
examples/decode_to_md5: CMakeFiles/webm.dir/third_party/libwebm/mkvparser/mkvparser.cc.o
examples/decode_to_md5: CMakeFiles/webm.dir/third_party/libwebm/mkvparser/mkvreader.cc.o
examples/decode_to_md5: CMakeFiles/decode_to_md5.dir/build.make
examples/decode_to_md5: libaom.a
examples/decode_to_md5: CMakeFiles/decode_to_md5.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable examples/decode_to_md5"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/decode_to_md5.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/decode_to_md5.dir/build: examples/decode_to_md5

.PHONY : CMakeFiles/decode_to_md5.dir/build

CMakeFiles/decode_to_md5.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/decode_to_md5.dir/cmake_clean.cmake
.PHONY : CMakeFiles/decode_to_md5.dir/clean

CMakeFiles/decode_to_md5.dir/depend:
	cd /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/decode_to_md5.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/decode_to_md5.dir/depend

