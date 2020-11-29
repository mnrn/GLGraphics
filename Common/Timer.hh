/**
 * @brief Timer
 */

#ifndef TIMER_HH
#define TIMER_HH

// ********************************************************************************
// Include files
// ********************************************************************************

#include <chrono>

// ********************************************************************************
// Namespace
// ********************************************************************************

// ********************************************************************************
// Class
// ********************************************************************************

template <typename ClockType = std::chrono::high_resolution_clock> class Timer {
public:
  // start timer
  void start() { start_ = ClockType::now(); }
  // end tismer
  void end() { end_ = ClockType::now(); }
  // get timer duration
  std::chrono::duration<double, std::milli> duration() const {
    return end_ - start_;
  }
  // get elapsed time
  double elapsed() const { return duration().count(); }

private:
  std::chrono::time_point<ClockType> start_;
  std::chrono::time_point<ClockType> end_;
};

#endif
