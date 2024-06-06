cmake_minimum_required(VERSION 3.21)
project(cw_server_lib)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(crossguid CONFIG REQUIRED)

find_package(Crow CONFIG REQUIRED)

add_library(
        cw_server_lib
        SHARED
        server.h
        server.cpp)
target_include_directories(
        cw_server_lib
        PUBLIC
        ./)
target_link_libraries(
        cw_server_lib
        PUBLIC
        cw_memory)
#target_link_libraries(
  #      cw_server_lib
 #       PUBLIC
 #       cw_disk)
target_link_libraries(
        cw_server_lib
        PUBLIC
        cw_interface)
target_link_libraries(
        cw_server_lib
        PUBLIC
        nlohmann_json::nlohmann_json)
target_link_libraries(cw_server_lib
        PUBLIC
        crossguid)
target_link_libraries(
        cw_server_lib
        PUBLIC
        mp_os_lggr_srvr_lggr)
target_link_libraries(
        cw_server_lib
        PUBLIC
        cw_interface)
target_link_libraries(
        cw_server_lib
        PUBLIC
        Crow::Crow)
target_link_libraries(
        cw_server_lib
        PUBLIC
        ws2_32)
target_link_libraries(
        cw_server_lib
        PUBLIC
        wsock32)
set_target_properties(
        cw_server_lib PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server logger implementation library")