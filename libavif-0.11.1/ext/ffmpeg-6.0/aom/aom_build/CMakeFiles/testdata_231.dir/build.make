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

# Utility rule file for testdata_231.

# Include the progress variables for this target.
include CMakeFiles/testdata_231.dir/progress.make

CMakeFiles/testdata_231:
	/usr/bin/cmake3 -DAOM_CONFIG_DIR="/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build" -DAOM_ROOT="/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom" -DAOM_TEST_FILE="av1-1-b8-01-size-66x32.ivf.md5" -DAOM_TEST_CHECKSUM=3ad5a58a0ee5086af370b22ab2b5b7592a4f33e7 -P /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/test/test_data_download_worker.cmake

testdata_231: CMakeFiles/testdata_231
testdata_231: CMakeFiles/testdata_231.dir/build.make

.PHONY : testdata_231

# Rule to build all files generated by this target.
CMakeFiles/testdata_231.dir/build: testdata_231

.PHONY : CMakeFiles/testdata_231.dir/build

CMakeFiles/testdata_231.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testdata_231.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testdata_231.dir/clean

CMakeFiles/testdata_231.dir/depend:
	cd /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/testdata_231.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/testdata_231.dir/depend

