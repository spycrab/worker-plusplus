// Copyright 2019 spycrab0
// Licensed under MIT License
// Refer to the LICENSE file included

// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

#include <worker++/team.hpp>

// Construct team
TEST(team, construct) {
  // ...without a member count
  workerpp::team<int, int>({[](const int &i) { return i * i; }});
  // ...with a member count
  workerpp::team<int, int>({[](const int &i) { return i * i; }}, 4);
}

// Run team
TEST(team, basic) {
  workerpp::team<int, int> team({[](const int &i) { return i * i; }});

  // Start a team
  team.start();
  // Stop it synchronously
  team.stop(false);
}

// Run team, this time with actual work
TEST(team, basic2) {
  workerpp::team<int, int> team({[](const int &i) { return i * i; }});

  for (int i = 0; i < 100; i++)
    team.add(i);
  // Start a team
  team.start();

  team.yield_any();
  team.yield_all();
}
