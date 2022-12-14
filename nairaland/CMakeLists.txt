# CMakeList.txt : CMake project for nairaland, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)

#include(${PROJECT_SOURCE_DIR}/http/CMakeLists.txt)

# Add source to this project's executable.
add_executable (nairaland "nairaland.cpp" "nairaland.h")

# TODO: Add tests and install targets if needed.
