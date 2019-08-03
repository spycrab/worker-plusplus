// Copyright 2019 spycrab0
// Licensed under MIT License
// Refer to the LICENSE file included

// SPDX-License-Identifier: MIT

#pragma once

#include <functional>

namespace workerpp {

template <typename T_in, typename T_out = void> class job {
public:
  job(std::function<T_out(const T_in &&)> function) : m_function(function) {}

  T_out run(const T_in &&input) const { return m_function(std::move(input)); }
  T_out run(const T_in &input) const { return m_function(std::move(input)); }
private:
  std::function<T_out(const T_in &&)> m_function;
};

} // namespace workerpp
