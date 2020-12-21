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

  corners_.clear();
  corners_.resize(8);

  // near plane
  corners_[0] = glm::vec4(nc - u * ndx - v * ndy, 1.0f);
  corners_[1] = glm::vec4(nc - u * ndx + v * ndy, 1.0f);
  corners_[2] = glm::vec4(nc + u * ndx + v * ndy, 1.0f);
  corners_[3] = glm::vec4(nc + u * ndx - v * ndy, 1.0f);

  // far plane
  corners_[4] = glm::vec4(fc - u * fdx - v * fdy, 1.0f);
  corners_[5] = glm::vec4(fc - u * fdx + v * fdy, 1.0f);
  corners_[6] = glm::vec4(fc + u * fdx + v * fdy, 1.0f);
  corners_[7] = glm::vec4(fc + u * fdx - v * fdy, 1.0f);
}

AABB Frustum::ComputeAABB(const glm::mat4 &m) const {
  AABB box;

  for (int i = 0; i < 8; i++) {
    box.Merge(glm::vec3(m * corners_[i]));
  }

  return box;
}

glm::mat4 Frustum::GetProjectionMatrix() const {
  return glm::perspective(glm::radians(fovy_), ar_, near_, far_);
}
