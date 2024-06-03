cmake_minimum_required(VERSION 3.21)
project(cw_interface)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(crossguid CONFIG REQUIRED)

add_library(
        cw_interface
        controller_int.h
        controller_int.cpp
        operation.h)
target_include_directories(
        cw_interface
        PUBLIC
        ./)
target_link_libraries(
        cw_interface
        PUBLIC
        nlohmann_json::nlohmann_json)
target_link_libraries(
        cw_interface
        PUBLIC
        crossguid)
target_link_libraries(
        cw_interface
        PUBLIC
        cw_utility)
set_target_properties(
        cw_interface PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server logger implementation library")