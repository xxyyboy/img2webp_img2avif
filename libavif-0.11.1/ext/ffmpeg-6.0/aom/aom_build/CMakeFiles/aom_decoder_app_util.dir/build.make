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
include CMakeFiles/aom_decoder_app_util.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/aom_decoder_app_util.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/aom_decoder_app_util.dir/flags.make

CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.o: CMakeFiles/aom_decoder_app_util.dir/flags.make
CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.o: ../common/obudec.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.o"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.o   -c /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/common/obudec.c

CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.i"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/common/obudec.c > CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.i

CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.s"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/common/obudec.c -o CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.s

CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.o: CMakeFiles/aom_decoder_app_util.dir/flags.make
CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.o: ../common/video_reader.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.o"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.o   -c /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/common/video_reader.c

CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.i"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/common/video_reader.c > CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.i

CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.s"
	/root/data/iauto/tools/gcc550/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/common/video_reader.c -o CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.s

CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.o: CMakeFiles/aom_decoder_app_util.dir/flags.make
CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.o: ../common/webmdec.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.o"
	/root/data/iauto/tools/gcc550/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.o -c /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/common/webmdec.cc

CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.i"
	/root/data/iauto/tools/gcc550/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/common/webmdec.cc > CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.i

CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.s"
	/root/data/iauto/tools/gcc550/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/common/webmdec.cc -o CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.s

aom_decoder_app_util: CMakeFiles/aom_decoder_app_util.dir/common/obudec.c.o
aom_decoder_app_util: CMakeFiles/aom_decoder_app_util.dir/common/video_reader.c.o
aom_decoder_app_util: CMakeFiles/aom_decoder_app_util.dir/common/webmdec.cc.o
aom_decoder_app_util: CMakeFiles/aom_decoder_app_util.dir/build.make

.PHONY : aom_decoder_app_util

# Rule to build all files generated by this target.
CMakeFiles/aom_decoder_app_util.dir/build: aom_decoder_app_util

.PHONY : CMakeFiles/aom_decoder_app_util.dir/build

CMakeFiles/aom_decoder_app_util.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/aom_decoder_app_util.dir/cmake_clean.cmake
.PHONY : CMakeFiles/aom_decoder_app_util.dir/clean

CMakeFiles/aom_decoder_app_util.dir/depend:
	cd /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_decoder_app_util.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/aom_decoder_app_util.dir/depend
