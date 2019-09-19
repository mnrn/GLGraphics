/**
 * @brief Timer
 */

#ifndef TIMER_HPP
#define TIMER_HPP

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

class timer {
public:
  // start timer
  void start() { start_ = std::chrono::high_resolution_clock::now(); }
  // end tismer
  void end() { end_ = std::chrono::high_resolution_clock::now(); }
  // get timer duration
  std::chrono::duration<double, std::milli> duration() const {
    return end_ - start_;
  }
  // get elapsed time
  double elapsed() const { return duration().count(); }

private:
  std::chrono::high_resolution_clock::time_point start_;
  std::chrono::high_resolution_clock::time_point end_;
};

#endif // end of TIMER_HPP
