cmake_minimum_required(VERSION 3.21)
project(mp_os_arthmtc_frctn)

add_subdirectory(tests)

add_library(
        mp_os_arthmtc_frctn
        src/fraction.cpp
        ../big_integer/include/big_int.h
        tests/tests_fractions.cpp)
target_include_directories(
        mp_os_arthmtc_frctn
        PUBLIC
        ./include)
target_link_libraries(
        mp_os_arthmtc_frctn
        PUBLIC
        mp_os_cmmn)
target_link_libraries(
        mp_os_arthmtc_frctn
        PUBLIC
        mp_os_allctr_allctr)
target_link_libraries(
        mp_os_arthmtc_frctn
        PUBLIC
        mp_os_arthmtc_bg_intgr)
target_link_libraries(
        mp_os_arthmtc_frctn
        PUBLIC
        mp_os_cmmn)
set_target_properties(
        mp_os_arthmtc_frctn PROPERTIES
        LANGUAGES CXX
        LINKER_LANGUAGE CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "fraction implementation library")