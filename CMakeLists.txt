# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

set(CMP0079 NEW)

project ("nairaland")

add_subdirectory("thirdparty/certify")

include_directories("http" "nairaland" "C:/Users/pius.ogunleye/vcpkg/installed/x86-windows/include")

add_subdirectory("http")
# Include sub-projects.
add_subdirectory ("nairaland")

target_link_libraries(nairaland httpclient certify::core)
