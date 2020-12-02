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
  void Start() { start_ = ClockType::now(); }
  // end tismer
  void End() { end_ = ClockType::now(); }
  // get timer duration
  std::chrono::duration<double, std::milli> Duration() const {
    return end_ - start_;
  }
  // get elapsed time
  double Elapsed() const { return Duration().count(); }

private:
  std::chrono::time_point<ClockType> start_;
  std::chrono::time_point<ClockType> end_;
};

#endif
