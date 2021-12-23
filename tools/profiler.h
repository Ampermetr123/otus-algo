#pragma once

#include <chrono>
#include <iostream>
#include <functional>

using namespace std::chrono;

class LogDuration {
 static inline const auto report_lambda = []([[maybe_unused]] unsigned long long dur) {
     std::cerr << "Duration: "<< dur << " ms" << std::endl;
  };

  using endfunc_t = std::function<void(long long dur)>;

public:
  explicit LogDuration(endfunc_t endfunc = report_lambda): start(steady_clock::now()), endfunc(endfunc)  {
  }

  ~LogDuration() {
    auto finish = steady_clock::now();
    auto dur = finish - start;
    auto ms = duration_cast<milliseconds>(dur).count();
    endfunc(ms);
  }

private:
  endfunc_t endfunc;
  steady_clock::time_point start;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(...) \
  LogDuration UNIQ_ID(__LINE__)(__VA_ARGS__);
