cmake_minimum_required(VERSION 3.21)
project(tree_test)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

find_package(Boost COMPONENTS system thread REQUIRED)

FetchContent_MakeAvailable(
        googletest)

add_executable(
        tree_test
        b_tree.h
        ../controller/memory_controller.cpp
        ../controller/memory_controller.h
        ../../../structure/student.cpp
        ../../../structure/student.h
        test.cpp)
target_link_libraries(
        tree_test
        PRIVATE
        gtest_main)
target_link_libraries(
        tree_test
        PUBLIC
        mp_os_allctr_allctr_srtd_lst)
target_link_libraries(
        tree_test
        PUBLIC
        mp_os_allctr_allctr_bndr_tgs)
target_link_libraries(
        tree_test
        PUBLIC
        mp_os_allctr_allctr_bdds_sstm)
target_link_libraries(
        tree_test
        PUBLIC
        mp_os_allctr_allctr_glbl_hp)
target_link_libraries(
        tree_test
        PUBLIC
        mp_os_allctr_allctr_rb_tr)
target_link_libraries(
        tree_test
        PUBLIC
        mp_os_lggr_srvr_lggr)
target_link_libraries(
        tree_test
        PUBLIC
        cw_interface)
set_target_properties(
        tree_test PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server logger implementation library")

add_custom_command(TARGET tree_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:cw_server> $<TARGET_FILE_DIR:tree_test>
        COMMAND_EXPAND_LISTS
        )