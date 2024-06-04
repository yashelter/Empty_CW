cmake_minimum_required(VERSION 3.21)
project(mp_os_lggr_srvr_lggr_tests)

#include(FetchContent)
#FetchContent_Declare(
#        googletest
#        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)

# For Windows users: prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)


find_package(Crow CONFIG REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

#FetchContent_MakeAvailable(
#        googletest)

add_executable(
        mp_os_lggr_srvr_lggr_tests
        server_logger_tests.cpp)
target_link_libraries(
        mp_os_lggr_srvr_lggr_tests
        PRIVATE
#        gtest_main
        Crow::Crow
)
target_link_libraries(
        mp_os_lggr_srvr_lggr_tests
        PUBLIC
        mp_os_cmmn)
target_link_libraries(
        mp_os_lggr_srvr_lggr_tests
        PUBLIC
        mp_os_lggr_lggr)
target_link_libraries(
        mp_os_lggr_srvr_lggr_tests
        PUBLIC
        mp_os_lggr_srvr_lggr)
set_target_properties(
        mp_os_lggr_srvr_lggr_tests PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server logger implementation library tests")

add_executable(
        serv_test
        server.cpp
        server.h
        server_test.cpp)
target_link_libraries(
        serv_test
        PUBLIC
#        gtest_main
        Crow::Crow
)
target_link_libraries(
        serv_test
        PUBLIC
        mp_os_cmmn)
target_link_libraries(
        serv_test
        PUBLIC
        mp_os_lggr_lggr)
target_link_libraries(
        serv_test
        PUBLIC
        mp_os_lggr_srvr_lggr)
target_link_libraries(
        serv_test
        PUBLIC
        ${BOOST_LIBRARIES})
target_link_libraries(
        serv_test
        PUBLIC
        ws2_32)
target_link_libraries(
        serv_test
        PUBLIC
        wsock32)
set_target_properties(
        serv_test PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server logger implementation library tests")