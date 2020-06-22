#include "TimeIntervals.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace PhotoGraph::time;

using double_dur_t = std::chrono::duration<double>;

void TimeIntervals::setCurrentStart(time_point_t current) {
  assert(not current_start_.has_value());
  current_start_.emplace(current);
}
void TimeIntervals::endCurrent(time_point_t current) {
  insert(current_start_.value(), current);
  current_start_.reset();
}
inline double computeTerm(const double_dur_t& start, const double_dur_t& end, const double t_max) {
  const double a1{std::min(start.count(), t_max)}, a2{end.count()};
  return (a1 - a2) * ((t_max - a1) + (t_max - a2));
}
double TimeIntervals::computeRelevance(time_point_t current, time_point_t creation) const {
  double value{0};
  const double t_max{std::chrono::duration_cast<double_dur_t>(max_age_).count()};
  for (const TimeInterval& interval : intervals_) {
    const auto start{std::chrono::duration_cast<double_dur_t>(current - interval.start)},
        end{std::chrono::duration_cast<double_dur_t>(current - interval.end)};
    value += computeTerm(start, end, t_max);
    // std::cout << start.count() << " -> " << end.count() << " (" << term << ") ";
  }
  if (current_start_.has_value())
    value += computeTerm(std::chrono::duration_cast<double_dur_t>(current - *current_start_), double_dur_t{}, t_max);
  const double effective_age{
      std::clamp<double>(std::chrono::duration_cast<double_dur_t>(current - creation).count(), 1, t_max)};
  return value / (effective_age * (2. * t_max - effective_age));
}

double TimePoints::computeRelevance(time_point_t current) const {
  double value{0};
  for (const time_point_t& point : time_points_) {
    const auto duration{std::chrono::duration_cast<double_dur_t>(current - point)},
        max_age{std::chrono::duration_cast<double_dur_t>(max_age_)};
    const double past{duration.count()}, t_max{max_age.count()};
    assert(past <= t_max);
    const double term{1.f - past / t_max};
    value += term;
  }
  return std::tanh(value);
}
