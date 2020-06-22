#pragma once
#include <chrono>
#include <deque>
#include <optional>
#include <utility>

namespace PhotoGraph::time {
using clock_t = std::chrono::steady_clock;
using time_point_t = clock_t::time_point;
using duration_t = clock_t::duration;

struct TimeInterval {
  time_point_t start, end;
  TimeInterval(time_point_t start, time_point_t end) : start{std::move(start)}, end{std::move(end)} {}
};

class TimeIntervals {
  std::deque<TimeInterval> intervals_;
  std::optional<time_point_t> current_start_{};
  duration_t max_age_{std::chrono::minutes(5)};

public:
  void cleanup(time_point_t current) {
    for (auto it{intervals_.begin()}; it != intervals_.end() and current - it->end > max_age_;
         it = intervals_.erase(it)) {}
  }
  void insert(time_point_t start, time_point_t end) { intervals_.emplace_back(std::move(start), std::move(end)); }
  void setCurrentStart(time_point_t current);
  void endCurrent(time_point_t current);
  double computeRelevance(time_point_t current, time_point_t creation) const;
};

class TimePoints {
  std::deque<time_point_t> time_points_;
  duration_t max_age_{std::chrono::minutes(5)};

public:
  void cleanup(time_point_t current) {
    for (auto it{time_points_.begin()}; it != time_points_.end() and current - *it > max_age_;
         it = time_points_.erase(it)) {}
  }
  void insert(time_point_t current) { time_points_.emplace_back(std::move(current)); }
  double computeRelevance(time_point_t current) const;
};
} // namespace PhotoGraph::time