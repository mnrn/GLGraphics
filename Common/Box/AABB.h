#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <cmath>
#include <utility>

class AABB {
public:
  AABB() { Reset(); }
  void Reset() {
    min_ = glm::vec3(std::numeric_limits<float>::max());
    max_ = glm::vec3(std::numeric_limits<float>::min());
  }

  void Merge(const glm::vec3& pt) {
    min_.x = std::fmin(min_.x, pt.x);
    max_.x = std::fmax(max_.x, pt.x);

    min_.y = std::fmin(min_.y, pt.y);
    max_.y = std::fmax(max_.y, pt.y);

    min_.z = std::fmin(min_.z, pt.z);
    max_.z = std::fmax(max_.z, pt.z);
  }

  std::pair<glm::vec3, glm::vec3> GetMinMax() const {
    return std::make_pair(min_, max_);
  }

private:
  glm::vec3 min_;
  glm::vec3 max_;
};
