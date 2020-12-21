/**
 * @brief 視錐台クラス
 */

#include "View/Frustum.h"

#include <boost/assert.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

void Frustum::SetupPerspective(float fovy, float aspectRatio, float near,
                               float far) {
  fovy_ = fovy;
  ar_ = aspectRatio;
  near_ = near;
  far_ = far;
}

void Frustum::SetupCorners(const glm::vec3 &eyePt, const glm::vec3 &lookatPt,
                           const glm::vec3 &upVec) {
  const glm::vec3 n = glm::normalize(eyePt - lookatPt);
  const glm::vec3 u = glm::normalize(glm::cross(upVec, n));
  const glm::vec3 v = glm::normalize(glm::cross(n, u));

  const float ndy = near_ * tanf(glm::radians(fovy_) / 2.0f);
  const float ndx = ar_ * ndy;
  const glm::vec3 nc = eyePt + n * near_;

  const float fdy = far_ * tanf(glm::radians(fovy_) / 2.0f);
  const float fdx = ar_ * fdy;
  const glm::vec3 fc = eyePt + n * far_;

  // near plane
  corners_[0] = nc - u * ndx - v * ndy;
  corners_[1] = nc - u * ndx + v * ndy;
  corners_[2] = nc + u * ndx + v * ndy;
  corners_[3] = nc + u * ndx - v * ndy;

  // far plane
  corners_[4] = fc - u * fdx - v * fdy;
  corners_[5] = fc - u * fdx + v * fdy;
  corners_[6] = fc + u * fdx + v * fdy;
  corners_[7] = fc + u * fdx - v * fdy;
}

glm::mat4 Frustum::GetProjectionMatrix() const {
  return glm::perspective(glm::radians(fovy_), ar_, near_, far_);
}

AABB Frustum::ComputeAABB(const glm::mat4 &m) const {
  AABB box;
  for (const auto corner : corners_) {
    box.Merge(glm::vec3(m * glm::vec4(corner, 1.0f)));
  }
  return box;
}
