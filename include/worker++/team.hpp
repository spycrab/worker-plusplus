// Copyright 2019 spycrab0
// Licensed under MIT License
// Refer to the LICENSE file included

// SPDX-License-Identifier: MIT

#pragma once

#include <cmath>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <vector>

#include "job.hpp"
#include "worker.hpp"

namespace workerpp {
template <typename T_in, typename T_out> class team {
public:
  team(job<T_in, T_out> job, size_t members = 4) {
    if (members == 0)
      throw std::runtime_error("workerpp::team must have at least one member!");

    for (size_t i = 0; i < members; i++)
      m_workers.push_back(std::make_unique<worker<T_in, T_out>>(job));
  }

  void start() {
    if (!m_queue.empty())
      distribute();

    for (auto &worker : m_workers)
      worker->start();
  }

  void stop(bool async = true) {
    for (auto &worker : m_workers)
      worker->stop();

    while (std::find_if(m_workers.begin(), m_workers.end(),
                        [](const auto &worker) { return worker->running(); }) !=
           m_workers.end()) {
    }
  }

  void distribute() {
    std::lock_guard<std::mutex> lk(m_queue_mutex);

    if (m_queue.empty())
      return;

    // This calculate how many items each worker should have in his queue
    const size_t ideal_average = static_cast<size_t>(
        std::ceil((std::accumulate(m_workers.begin(), m_workers.end(), 0,
                                   [](const size_t sum, const auto &worker) {
                                     return sum + worker->queue_size();
                                   }) +
                   m_queue.size()) /
                  static_cast<double>(m_workers.size())));

    for (const auto &worker : m_workers) {
      if (worker->queue_size() >= ideal_average)
        continue;

      while (worker->queue_size() < ideal_average) {
        worker->add(m_queue.front());
        m_queue.pop();
      }
    }
  }

  void add(const std::initializer_list<T_in> &inputs) {
    std::lock_guard<std::mutex> lk(m_queue_mutex);
    for (const auto &input : inputs) {
      m_queue.push(input);
    }
  }

  void add(const T_in &input) {
    std::lock_guard<std::mutex> lk(m_queue_mutex);
    m_queue.push(input);
  }

  std::vector<T_out> yield_all() {
    std::vector<T_out> all_results;

    // Wait until there's nothing more to process

    while (std::accumulate(m_workers.begin(), m_workers.end(), 0,
                           [](const size_t sum, const auto &worker) {
                             return sum + worker->queue_size();
                           }) > 0) {
    }

    for (const auto &worker : m_workers) {
      for (const auto &result : worker->results())
        all_results.push_back(result);

      worker->clear_results();
    }

    return all_results;
  }

  T_out yield_any(bool async = false) {

    auto result = std::find_if(
        m_workers.begin(), m_workers.end(),
        [](const auto &worker) { return worker->queue_size() > 0; });

    if (async) {
      if (result == m_workers.end())
        throw std::runtime_error("No results available!");

      return (*result)->yield_one();
    }

    while (true) {
      auto result = std::find_if(
          m_workers.begin(), m_workers.end(),
          [](const auto &worker) { return worker->queue_size() > 0; });
      if (result != m_workers.end())
        return (*result)->yield_one();
    }
  }

  size_t workers() const { return m_workers.size(); }

private:
  std::queue<T_in> m_queue;
  std::vector<T_out> m_results;
  std::vector<std::unique_ptr<worker<T_in, T_out>>> m_workers;

  std::mutex m_queue_mutex;
};
}; // namespace workerpp
