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
include CMakeFiles/resize_util.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/resize_util.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/resize_util.dir/flags.make

CMakeFiles/resize_util.dir/examples/resize_util.c.o: CMakeFiles/resize_util.dir/flags.make
CMakeFiles/resize_util.dir/examples/resize_util.c.o: ../examples/resize_util.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/resize_util.dir/examples/resize_util.c.o"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/resize_util.dir/examples/resize_util.c.o   -c /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/examples/resize_util.c

CMakeFiles/resize_util.dir/examples/resize_util.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/resize_util.dir/examples/resize_util.c.i"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/examples/resize_util.c > CMakeFiles/resize_util.dir/examples/resize_util.c.i

CMakeFiles/resize_util.dir/examples/resize_util.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/resize_util.dir/examples/resize_util.c.s"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/examples/resize_util.c -o CMakeFiles/resize_util.dir/examples/resize_util.c.s

# Object files for target resize_util
resize_util_OBJECTS = \
"CMakeFiles/resize_util.dir/examples/resize_util.c.o"

# External object files for target resize_util
resize_util_EXTERNAL_OBJECTS = \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/av1/arg_defs.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/args_helper.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/args.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/av1_config.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/md5_utils.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/tools_common.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/rawenc.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/y4menc.c.o" \
"/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_common_app_util.dir/common/ivfdec.c.o" \
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

resize_util: CMakeFiles/resize_util.dir/examples/resize_util.c.o
resize_util: CMakeFiles/aom_common_app_util.dir/av1/arg_defs.c.o
resize_util: CMakeFiles/aom_common_app_util.dir/common/args_helper.c.o
resize_util: CMakeFiles/aom_common_app_util.dir/common/args.c.o
resize_util: CMakeFiles/aom_common_app_util.dir/common/av1_config.c.o
resize_util: CMakeFiles/aom_common_app_util.dir/common/md5_utils.c.o
resize_util: CMakeFiles/aom_common_app_util.dir/common/tools_common.c.o
resize_util: CMakeFiles/aom_common_app_util.dir/common/rawenc.c.o
resize_util: CMakeFiles/aom_common_app_util.dir/common/y4menc.c.o
resize_util: CMakeFiles/aom_common_app_util.dir/common/ivfdec.c.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/convert_argb.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/cpu_id.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/planar_functions.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/row_any.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/row_common.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/row_gcc.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/row_mips.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/row_neon.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/row_neon64.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/row_win.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/scale.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_any.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_common.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_gcc.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_mips.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_neon.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_neon64.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_win.cc.o
resize_util: CMakeFiles/yuv.dir/third_party/libyuv/source/scale_uv.cc.o
resize_util: CMakeFiles/webm.dir/third_party/libwebm/common/hdr_util.cc.o
resize_util: CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvmuxer.cc.o
resize_util: CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvmuxerutil.cc.o
resize_util: CMakeFiles/webm.dir/third_party/libwebm/mkvmuxer/mkvwriter.cc.o
resize_util: CMakeFiles/webm.dir/third_party/libwebm/mkvparser/mkvparser.cc.o
resize_util: CMakeFiles/webm.dir/third_party/libwebm/mkvparser/mkvreader.cc.o
resize_util: CMakeFiles/resize_util.dir/build.make
resize_util: libaom.a
resize_util: CMakeFiles/resize_util.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable resize_util"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/resize_util.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/resize_util.dir/build: resize_util

.PHONY : CMakeFiles/resize_util.dir/build

CMakeFiles/resize_util.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/resize_util.dir/cmake_clean.cmake
.PHONY : CMakeFiles/resize_util.dir/clean

CMakeFiles/resize_util.dir/depend:
	cd /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/resize_util.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/resize_util.dir/depend
