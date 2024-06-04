cmake_minimum_required(VERSION 3.21)
project(cw_heart)

find_package(httplib CONFIG REQUIRED)

add_executable(
        cw_heart
        heart.cpp
        heart.cpp
        ../launcher/launcher.cpp
        ../launcher/launcher.h
        main.cpp)
target_link_libraries(
        cw_client
        PUBLIC
        httplib::httplib)
target_link_libraries(
        cw_heart
        PUBLIC
        mp_os_lggr_srvr_lggr)
set_target_properties(
        cw_heart PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server implementation")