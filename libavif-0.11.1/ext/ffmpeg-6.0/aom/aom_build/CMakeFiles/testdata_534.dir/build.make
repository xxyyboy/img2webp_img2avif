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

# Utility rule file for testdata_534.

# Include the progress variables for this target.
include CMakeFiles/testdata_534.dir/progress.make

CMakeFiles/testdata_534:
	/usr/bin/cmake3 -DAOM_CONFIG_DIR="/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build" -DAOM_ROOT="/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom" -DAOM_TEST_FILE="av1-1-b8-22-svc-L2T1.ivf.md5" -DAOM_TEST_CHECKSUM=0f75f2ac44e61fc83be70c955410fa378e433237 -P /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/test/test_data_download_worker.cmake

testdata_534: CMakeFiles/testdata_534
testdata_534: CMakeFiles/testdata_534.dir/build.make

.PHONY : testdata_534

# Rule to build all files generated by this target.
CMakeFiles/testdata_534.dir/build: testdata_534

.PHONY : CMakeFiles/testdata_534.dir/build

CMakeFiles/testdata_534.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testdata_534.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testdata_534.dir/clean

CMakeFiles/testdata_534.dir/depend:
	cd /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/testdata_534.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/testdata_534.dir/depend

