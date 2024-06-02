cmake_minimum_required(VERSION 3.21)
project(mp_os)

set(CMAKE_CXX_STANDARD 23)

#set(VCPKG_TARGET_TRIPLET x64-windows)
#set(CMAKE_TOOLCHAIN_FILE C:/Programming/vcpkg/scripts/buildsystems/vcpkg.cmake)
#include(C:/Programming/vcpkg/scripts/buildsystems/vcpkg.cmake)

add_subdirectory(allocator)
add_subdirectory(common)
add_subdirectory(logger)
add_subdirectory(cursal_work)