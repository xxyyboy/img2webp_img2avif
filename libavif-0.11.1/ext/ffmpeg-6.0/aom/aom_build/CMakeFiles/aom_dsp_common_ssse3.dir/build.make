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
include CMakeFiles/aom_dsp_common_ssse3.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/aom_dsp_common_ssse3.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/aom_dsp_common_ssse3.dir/flags.make

CMakeFiles/aom_dsp_common_ssse3.dir/aom_dsp/x86/aom_subpixel_8t_ssse3.asm.o: CMakeFiles/aom_dsp_common_ssse3.dir/flags.make
CMakeFiles/aom_dsp_common_ssse3.dir/aom_dsp/x86/aom_subpixel_8t_ssse3.asm.o: ../aom_dsp/x86/aom_subpixel_8t_ssse3.asm
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building ASM_NASM object CMakeFiles/aom_dsp_common_ssse3.dir/aom_dsp/x86/aom_subpixel_8t_ssse3.asm.o"
	/usr/bin/nasm $(ASM_NASM_INCLUDES) $(ASM_NASM_FLAGS) -f elf64 -o CMakeFiles/aom_dsp_common_ssse3.dir/aom_dsp/x86/aom_subpixel_8t_ssse3.asm.o /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_dsp/x86/aom_subpixel_8t_ssse3.asm

CMakeFiles/aom_dsp_common_ssse3.dir/aom_dsp/x86/aom_subpixel_bilinear_ssse3.asm.o: CMakeFiles/aom_dsp_common_ssse3.dir/flags.make
CMakeFiles/aom_dsp_common_ssse3.dir/aom_dsp/x86/aom_subpixel_bilinear_ssse3.asm.o: ../aom_dsp/x86/aom_subpixel_bilinear_ssse3.asm
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building ASM_NASM object CMakeFiles/aom_dsp_common_ssse3.dir/aom_dsp/x86/aom_subpixel_bilinear_ssse3.asm.o"
	/usr/bin/nasm $(ASM_NASM_INCLUDES) $(ASM_NASM_FLAGS) -f elf64 -o CMakeFiles/aom_dsp_common_ssse3.dir/aom_dsp/x86/aom_subpixel_bilinear_ssse3.asm.o /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_dsp/x86/aom_subpixel_bilinear_ssse3.asm

aom_dsp_common_ssse3: CMakeFiles/aom_dsp_common_ssse3.dir/aom_dsp/x86/aom_subpixel_8t_ssse3.asm.o
aom_dsp_common_ssse3: CMakeFiles/aom_dsp_common_ssse3.dir/aom_dsp/x86/aom_subpixel_bilinear_ssse3.asm.o
aom_dsp_common_ssse3: CMakeFiles/aom_dsp_common_ssse3.dir/build.make

.PHONY : aom_dsp_common_ssse3

# Rule to build all files generated by this target.
CMakeFiles/aom_dsp_common_ssse3.dir/build: aom_dsp_common_ssse3

.PHONY : CMakeFiles/aom_dsp_common_ssse3.dir/build

CMakeFiles/aom_dsp_common_ssse3.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/aom_dsp_common_ssse3.dir/cmake_clean.cmake
.PHONY : CMakeFiles/aom_dsp_common_ssse3.dir/clean

CMakeFiles/aom_dsp_common_ssse3.dir/depend:
	cd /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build /data/nginx/SecNginx1202/3partylib/libavif-0.11.1/ext/ffmpeg-6.0/aom/aom_build/CMakeFiles/aom_dsp_common_ssse3.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/aom_dsp_common_ssse3.dir/depend
