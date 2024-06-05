cmake_minimum_required(VERSION 3.21)
project(cw_server)

add_subdirectory(utility_cw)
add_subdirectory(interfaces)
add_subdirectory(in_memory)
# add_subdirectory(in_disk)
add_subdirectory(server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(Crow CONFIG REQUIRED)

add_executable(
        cw_server
        main.cpp
        ../structure/student.h
        ../structure/student.cpp
)

target_link_libraries(
        cw_server
        PUBLIC
        nlohmann_json::nlohmann_json
        mp_os_lggr_srvr_lggr
        cw_server_lib
)

set_target_properties(
        cw_server PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server implementation"
)

add_custom_command(TARGET cw_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:cw_server_lib>
        $<TARGET_FILE_DIR:cw_server>
)
