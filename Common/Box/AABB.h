#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <cmath>

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

  glm::mat4 ComputeCropMatrix() const {
    const float sx = 2.0f / (max_.x - min_.x);
    const float sy = 2.0f / (max_.y - min_.y);
    const float ox = -0.5f * (max_.x + min_.x) * sx;
    const float oy = -0.5f * (max_.y + min_.y) * sy;
#if true
    return glm::mat4(glm::vec4(sx, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, sy, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(ox, oy, 0.0f, 1.0f));
#else
    const float sz = 1.0f / (max_.z - min_.z);
    const float oz = -min_.z * sz;
    return glm::mat4(
        glm::vec4(sx, 0.0f, 0.0f, ox), glm::vec4(0.0f, sy, 0.0f, oy),
        glm::vec4(0.0f, 0.0f, sz, oz), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
#endif
  }

private:
  glm::vec3 min_;
  glm::vec3 max_;
};
