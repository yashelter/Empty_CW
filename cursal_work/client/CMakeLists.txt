cmake_minimum_required(VERSION 3.21)
project(cw_client)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(httplib CONFIG REQUIRED)

add_executable(
        cw_client
        main.cpp
        client.h
        client.cpp)
target_link_libraries(
        cw_client
        PUBLIC
        nlohmann_json::nlohmann_json)
target_link_libraries(
        cw_client
        PUBLIC
        httplib::httplib)
set_target_properties(
        cw_client PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "client  implementation")