#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <limits>
#include <utility>

class AABB {
public:
  AABB() { Reset(); }
  void Reset() {
    min_ = glm::vec3(std::numeric_limits<float>::max());
    max_ = glm::vec3(std::numeric_limits<float>::min());
  }

  void Merge(const glm::vec3 &pt) { Merge(pt.x, pt.y, pt.z); }

  void Merge(float x, float y, float z) {
    min_.x = std::fmin(min_.x, x);
    max_.x = std::fmax(max_.x, x);

    min_.y = std::fmin(min_.y, y);
    max_.y = std::fmax(max_.y, y);

    min_.z = std::fmin(min_.z, z);
    max_.z = std::fmax(max_.z, z);
  }

  std::pair<glm::vec3, glm::vec3> GetMinMax() const {
    return std::make_pair(min_, max_);
  }

private:
  glm::vec3 min_;
  glm::vec3 max_;
};
