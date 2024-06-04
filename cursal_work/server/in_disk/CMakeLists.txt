cmake_minimum_required(VERSION 3.21)
project(cw_disk)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(Boost COMPONENTS system thread REQUIRED)

include_directories (${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})
find_package(crossguid CONFIG REQUIRED)

add_library(
        cw_disk
        SHARED
        controller/disk_controller.h
        controller/disk_controller.cpp
        tree/b_tree_disk.h)
target_include_directories(
        cw_disk
        PUBLIC
        ./controller
        ./tree)
target_link_libraries(
        cw_disk
        PUBLIC
        nlohmann_json::nlohmann_json)
target_link_libraries(cw_disk
        PUBLIC
        crossguid)
target_link_libraries(cw_disk
        PUBLIC
        cw_memory)
target_link_libraries(
        cw_disk
        PUBLIC
        mp_os_lggr_srvr_lggr)
target_link_libraries(
        cw_disk
        PUBLIC
        cw_interface)
target_link_libraries(
        cw_disk
        PUBLIC
        cw_utility)
target_link_libraries(
        cw_disk
        PUBLIC
        mp_os_allctr_allctr)
target_link_libraries(
        cw_disk
        PUBLIC
        Boost::thread)
target_link_libraries(
        cw_disk
        PUBLIC
        ${BOOST_LIBRARIES})
set_target_properties(
        cw_disk PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server logger implementation library")