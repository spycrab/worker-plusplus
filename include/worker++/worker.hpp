// Copyright 2019 spycrab0
// Licensed under MIT License
// Refer to the LICENSE file included

// SPDX-License-Identifier: MIT

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include "job.hpp"

namespace workerpp {

template <typename T_in, typename T_out> class worker {
public:
  worker(const job<T_in, T_out> &job, bool stop_if_queue_empty = false)
      : m_job(job), m_running(false),
        m_stop_if_queue_empty(stop_if_queue_empty) {}

  ~worker() {
    if (m_running)
      stop(false);
  }

  void loop() {
    std::lock_guard<std::mutex> thread_guard(m_thread_mutex);

    m_running = true;

    while (true) {
      while (!m_queue.empty()) {
        // TODO: Is this okay?
        auto front = m_queue.front();

        {
          std::lock_guard<std::mutex> queue_guard(m_queue_mutex);
          m_queue.pop();
        }

        auto result = m_job.run(front);

        {
          std::lock_guard<std::mutex> result_guard(m_result_mutex);
          m_results.push_back(std::move(result));
        }
      }

      if (m_stop_if_queue_empty)
        break;

      if (m_stop_requested)
        break;

      // We've got nothing to do at the moment, wait for something to happen
      {
        std::unique_lock<std::mutex> lk(m_queue_mutex);
        m_wake.wait(lk,
                    [this] { return m_stop_requested || !m_queue.empty(); });
      };
    }

    m_running = false;
  }

  bool running() const { return m_running; }

  size_t queue_size() const { return m_queue.size(); }

  void add(T_in item) {
    {
      std::lock_guard<std::mutex> queue_guard(m_queue_mutex);
      m_queue.push(item);
    }
    m_wake.notify_one();
  }

  void start() {
    if (m_running)
      return;

    m_thread = std::thread([this] { loop(); });
    m_thread.detach();

    while (!m_running) {
    }
  }

  const std::vector<T_out> &results() const { return m_results; }

  void clear_results() {
    std::lock_guard<std::mutex> result_guard(m_result_mutex);
    m_results.clear();
  }

  T_out yield_one() {
    std::lock_guard<std::mutex> result_guard(m_result_mutex);
    if (m_results.size() == 0)
      throw std::runtime_error("No results available!");

    T_out one = m_results.back();
    m_results.pop_back();

    return one;
  }

  void stop(bool async = true) {
    if (!m_running)
      return;

    {
      std::lock_guard<std::mutex> wait_guard(m_queue_mutex);
      m_stop_requested = true;
    }
    m_wake.notify_one();

    if (async)
      return;

    {
      // Wait for the thread to finish
      std::lock_guard<std::mutex> lk(m_thread_mutex);
    }
  }

private:
  const job<T_in, T_out> &m_job;
  std::queue<T_in> m_queue;
  std::vector<T_out> m_results;

  std::mutex m_thread_mutex;
  std::mutex m_queue_mutex;
  std::mutex m_result_mutex;

  std::atomic<bool> m_running;
  bool m_stop_requested = false;
  const bool m_stop_if_queue_empty = false;

  std::condition_variable m_wake;

  std::thread m_thread;
};
} // namespace workerpp
