cmake_minimum_required(VERSION 3.21)
project(mp_os_lggr
        VERSION 1.0
        DESCRIPTION "cursal work libraries"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(heart)