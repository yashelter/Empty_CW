cmake_minimum_required(VERSION 3.21)
project(cw_memory)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(Boost COMPONENTS system thread REQUIRED)

include_directories (${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})
find_package(crossguid CONFIG REQUIRED)

add_library(
        cw_memory
        controller/memory_controller.h
        controller/memory_controller.cpp
        tree/b_tree.h)
target_include_directories(
        cw_memory
        PUBLIC
        ./controller
        ./tree)
target_link_libraries(
        cw_memory
        PUBLIC
        nlohmann_json::nlohmann_json)
target_link_libraries(cw_memory
        PUBLIC
        crossguid)
target_link_libraries(
        cw_memory
        PUBLIC
        mp_os_allctr_allctr_srtd_lst)
target_link_libraries(
        cw_memory
        PUBLIC
        mp_os_allctr_allctr_bndr_tgs)
target_link_libraries(
        cw_memory
        PUBLIC
        mp_os_allctr_allctr_bdds_sstm)
target_link_libraries(
        cw_memory
        PUBLIC
        mp_os_allctr_allctr_glbl_hp)
target_link_libraries(
        cw_memory
        PUBLIC
        mp_os_allctr_allctr_rb_tr)
target_link_libraries(
        cw_memory
        PUBLIC
        mp_os_lggr_srvr_lggr)
target_link_libraries(
        cw_memory
        PUBLIC
        cw_interface)
target_link_libraries(
        cw_memory
        PUBLIC
        Boost::thread)
target_link_libraries(
        cw_memory
        PUBLIC
        ${BOOST_LIBRARIES})
set_target_properties(
        cw_memory PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server logger implementation library")