# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.8

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2017.2.3\bin\cmake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2017.2.3\bin\cmake\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/tcp.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tcp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tcp.dir/flags.make

CMakeFiles/tcp.dir/server.cpp.obj: CMakeFiles/tcp.dir/flags.make
CMakeFiles/tcp.dir/server.cpp.obj: ../server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tcp.dir/server.cpp.obj"
	C:\MinGW\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\tcp.dir\server.cpp.obj -c C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp\server.cpp

CMakeFiles/tcp.dir/server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcp.dir/server.cpp.i"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp\server.cpp > CMakeFiles\tcp.dir\server.cpp.i

CMakeFiles/tcp.dir/server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcp.dir/server.cpp.s"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp\server.cpp -o CMakeFiles\tcp.dir\server.cpp.s

CMakeFiles/tcp.dir/server.cpp.obj.requires:

.PHONY : CMakeFiles/tcp.dir/server.cpp.obj.requires

CMakeFiles/tcp.dir/server.cpp.obj.provides: CMakeFiles/tcp.dir/server.cpp.obj.requires
	$(MAKE) -f CMakeFiles\tcp.dir\build.make CMakeFiles/tcp.dir/server.cpp.obj.provides.build
.PHONY : CMakeFiles/tcp.dir/server.cpp.obj.provides

CMakeFiles/tcp.dir/server.cpp.obj.provides.build: CMakeFiles/tcp.dir/server.cpp.obj


# Object files for target tcp
tcp_OBJECTS = \
"CMakeFiles/tcp.dir/server.cpp.obj"

# External object files for target tcp
tcp_EXTERNAL_OBJECTS =

tcp.exe: CMakeFiles/tcp.dir/server.cpp.obj
tcp.exe: CMakeFiles/tcp.dir/build.make
tcp.exe: CMakeFiles/tcp.dir/linklibs.rsp
tcp.exe: CMakeFiles/tcp.dir/objects1.rsp
tcp.exe: CMakeFiles/tcp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable tcp.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\tcp.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tcp.dir/build: tcp.exe

.PHONY : CMakeFiles/tcp.dir/build

CMakeFiles/tcp.dir/requires: CMakeFiles/tcp.dir/server.cpp.obj.requires

.PHONY : CMakeFiles/tcp.dir/requires

CMakeFiles/tcp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\tcp.dir\cmake_clean.cmake
.PHONY : CMakeFiles/tcp.dir/clean

CMakeFiles/tcp.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp\cmake-build-debug C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp\cmake-build-debug C:\Users\Vovas\YandexDisk\4_course\nets\repository\tcp\cmake-build-debug\CMakeFiles\tcp.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tcp.dir/depend

