# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ggame/OS/KP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ggame/OS/KP/build

# Include any dependencies generated for this target.
include CMakeFiles/kptools.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/kptools.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/kptools.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/kptools.dir/flags.make

CMakeFiles/kptools.dir/include/kptools.c.o: CMakeFiles/kptools.dir/flags.make
CMakeFiles/kptools.dir/include/kptools.c.o: ../include/kptools.c
CMakeFiles/kptools.dir/include/kptools.c.o: CMakeFiles/kptools.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ggame/OS/KP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/kptools.dir/include/kptools.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/kptools.dir/include/kptools.c.o -MF CMakeFiles/kptools.dir/include/kptools.c.o.d -o CMakeFiles/kptools.dir/include/kptools.c.o -c /home/ggame/OS/KP/include/kptools.c

CMakeFiles/kptools.dir/include/kptools.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kptools.dir/include/kptools.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ggame/OS/KP/include/kptools.c > CMakeFiles/kptools.dir/include/kptools.c.i

CMakeFiles/kptools.dir/include/kptools.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kptools.dir/include/kptools.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ggame/OS/KP/include/kptools.c -o CMakeFiles/kptools.dir/include/kptools.c.s

# Object files for target kptools
kptools_OBJECTS = \
"CMakeFiles/kptools.dir/include/kptools.c.o"

# External object files for target kptools
kptools_EXTERNAL_OBJECTS =

kptools: CMakeFiles/kptools.dir/include/kptools.c.o
kptools: CMakeFiles/kptools.dir/build.make
kptools: CMakeFiles/kptools.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ggame/OS/KP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable kptools"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kptools.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/kptools.dir/build: kptools
.PHONY : CMakeFiles/kptools.dir/build

CMakeFiles/kptools.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/kptools.dir/cmake_clean.cmake
.PHONY : CMakeFiles/kptools.dir/clean

CMakeFiles/kptools.dir/depend:
	cd /home/ggame/OS/KP/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ggame/OS/KP /home/ggame/OS/KP /home/ggame/OS/KP/build /home/ggame/OS/KP/build /home/ggame/OS/KP/build/CMakeFiles/kptools.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/kptools.dir/depend

