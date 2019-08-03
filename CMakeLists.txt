# Copyright 2019 spycrab0
# Licensed under MIT License
# Refer to the LICENSE file included

# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.1)

project(worker++ CXX)

option(ENABLE_TESTS "Enable tests" OFF)

find_package(Threads)

add_library(worker++ INTERFACE)

target_include_directories(worker++ INTERFACE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(worker++ INTERFACE Threads::Threads)

if (ENABLE_TESTS)
find_package(GTest REQUIRED)
enable_testing()
add_subdirectory(test)
endif()
