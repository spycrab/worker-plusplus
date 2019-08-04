# Copyright 2019 spycrab0
# Licensed under MIT License
# Refer to the LICENSE file included

# SPDX-License-Identifier: MIT

include(GoogleTest)

function(add_unittest test_name)
  add_executable(${test_name} ${test_name}.cpp)
  target_link_libraries(${test_name} PRIVATE gtest gtest_main worker++)
  gtest_add_tests(TARGET ${test_name})
endfunction(add_unittest)

add_unittest(job)
add_unittest(worker)


