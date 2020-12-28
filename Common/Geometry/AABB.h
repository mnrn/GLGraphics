#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <limits>
#include <utility>

struct AABB {
  AABB() { Reset(); }
  void Reset() {
    min_ = glm::vec3(std::numeric_limits<float>::max());
    max_ = glm::vec3(std::numeric_limits<float>::lowest());
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

  glm::vec3 min_;
  glm::vec3 max_;
};
