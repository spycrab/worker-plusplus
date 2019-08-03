#include <iostream>

#include <gtest/gtest.h>

#include <worker++/worker.hpp>

TEST(worker, construct) {
  workerpp::worker<int, int> w({[](const int &&i) { return 0; }});
}

TEST(worker, basic) {
  workerpp::worker<int, int> worker({[](const int &&i) { return i * i; }});

  for (int i = 1; i < 10; i++)
    worker.add(i);

  ASSERT_EQ(worker.running(), false);
  worker.start();
  ASSERT_EQ(worker.running(), true);
}

TEST(worker, insert_while_running) {
  workerpp::worker<int, int> worker({[](const int &&i) { return i * i; }});

  worker.start();

  for (int i = 1; i < 10; i++)
    worker.add(i);
}

TEST(worker, sync_stop) {
  workerpp::worker<int, int> worker({[](const int &&i) { return i * i; }});

  ASSERT_EQ(worker.running(), false);
  worker.start();
  ASSERT_EQ(worker.running(), true);
  worker.stop(false);
  ASSERT_EQ(worker.running(), false);
}
