cmake_minimum_required(VERSION 3.21)
project(cw_server_lib)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(crossguid CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)

add_library(
        cw_server_lib
        SHARED
        server.h
        server.cpp
)

target_include_directories(
        cw_server_lib
        PUBLIC
        ./
)

target_link_libraries(
        cw_server_lib
        PUBLIC
        cw_memory
        # cw_disk  # Uncomment if needed
        cw_interface
        nlohmann_json::nlohmann_json
        crossguid
        mp_os_lggr_srvr_lggr
        Crow::Crow
)


set_target_properties(
        cw_server_lib PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server logger implementation library"
)
