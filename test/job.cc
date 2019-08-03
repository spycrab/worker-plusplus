#include <string>

#include <gtest/gtest.h>

#include <worker++/job.hpp>

TEST(job, construct) {
  // Construct with one parameter
  workerpp::job<int>([](const int &&) {});

  // Construct with two parameters
  workerpp::job<int, int>([](const int &&) { return 0; });

  // Construct with non-primitive parameter
  workerpp::job<int, std::string>([](const int &&) { return ""; });
}

TEST(job, run) {
  workerpp::job<int, int> negate([](const int &&i) { return -i; });

  // Run on rvalue
  ASSERT_EQ(negate.run(10), -10);

  int value = 20;

  // Run on a reference
  {
    int &ref = value;
    ASSERT_EQ(negate.run(ref), -ref);
  }

  // Run on a const reference
  {
    const int &ref = 20;
    ASSERT_EQ(negate.run(ref), -ref);
  }
}
