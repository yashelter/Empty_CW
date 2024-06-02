cmake_minimum_required(VERSION 3.21)
project(cw_server)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(Crow CONFIG REQUIRED)
find_package(Boost COMPONENTS system thread REQUIRED)

include_directories (${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

add_executable(
        cw_server
        in_disk/controller/disk_controller.cpp
        in_disk/controller/disk_controller.h
        in_memory/controller/memory_controller.cpp
        in_memory/controller/memory_controller.h
        in_memory/tree/b_tree.h
        interfaces/controller_int.h
        server/server.cpp
        server/server.h
        utility_cw/converters.cpp
        utility_cw/converters.h
        interfaces/operation.h
        in_disk/tree/b_tree_disk.h
        main.cpp)
target_link_libraries(
        cw_server
        PUBLIC
        nlohmann_json::nlohmann_json)
target_link_libraries(
        cw_server
        PUBLIC
        Crow::Crow)
target_link_libraries(
        cw_server
        PUBLIC
        mp_os_lggr_srvr_lggr)
target_link_libraries(
        cw_server
        PUBLIC
        mp_os_allctr_allctr_srtd_lst)
target_link_libraries(
        cw_server
        PUBLIC
        mp_os_allctr_allctr_bndr_tgs)
target_link_libraries(
        cw_server
        PUBLIC
        mp_os_allctr_allctr_bdds_sstm)
target_link_libraries(
        cw_server
        PUBLIC
        mp_os_allctr_allctr_glbl_hp)
target_link_libraries(
        cw_server
        PUBLIC
        mp_os_allctr_allctr_rb_tr)
target_link_libraries(
        cw_server
        PUBLIC
        Boost::thread)
target_link_libraries(
        cw_server
        PUBLIC
        ${BOOST_LIBRARIES})
set_target_properties(
        cw_server PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server implementation")