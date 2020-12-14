/**
 * @brief 視錐台クラス
 */

#include "Frustum.h"

#include <boost/assert.hpp>
#include <iostream>
#include <vector>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Frustum::Orient(const glm::vec3 &eyePt, const glm::vec3 &lookatPt,
                     const glm::vec3 &upVec) {
  // NOTE: 右手系は視線がz軸の負の方向を向いていることになります。
  // そのため、視線のベクトルを反転させてz軸と一致させています。
  n_ = glm::normalize(eyePt - lookatPt);
  u_ = glm::normalize(glm::cross(upVec, n_));
  v_ = glm::normalize(glm::cross(n_, u_));
  center_ = eyePt;
}

void Frustum::SetPerspective(float fovy, float aspectRatio, float near,
                             float far) {
  fovy_ = fovy;
  ar_ = aspectRatio;
  near_ = near;
  far_ = far;
}

std::array<glm::vec3, 8> Frustum::ComputeCorners() const {
  const float ndy = near_ * tanf(glm::radians(fovy_) / 2.0f);
  const float ndx = ar_ * ndy;
  const glm::vec3 nc = center_ + n_ * near_;

  const float fdy = far_ * tanf(glm::radians(fovy_) / 2.0f);
  const float fdx = ar_ * fdy;
  const glm::vec3 fc = center_ + n_ * far_;

  return {
      glm::vec3(nc - u_ * ndx - v_ * ndy), glm::vec3(nc - u_ * ndx + v_ * ndy),
      glm::vec3(nc + u_ * ndx + v_ * ndy), glm::vec3(nc + u_ * ndx - v_ * ndy),

      glm::vec3(fc - u_ * fdx - v_ * fdy), glm::vec3(fc - u_ * fdx + v_ * fdy),
      glm::vec3(fc + u_ * fdx + v_ * fdy), glm::vec3(fc + u_ * fdx - v_ * fdy)};
}

AABB Frustum::ComputeAABB(const glm::mat4& m) const {
  AABB box;
  
  const std::array<glm::vec3, 8> corners = ComputeCorners();
  for (int i = 0; i < 8; i++) {
    box.Merge(glm::vec3(m * glm::vec4(corners[i], 1.0f)));
  }

  return box;
}

glm::mat4 Frustum::GetViewMatrix() const {
  const glm::mat4 rot(u_.x, v_.x, n_.x, 0.0f, u_.y, v_.y, n_.y, 0.0f, u_.z,
                      v_.z, n_.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  const glm::mat4 trans = glm::translate(
      glm::mat4(1.0f), glm::vec3(-center_.x, -center_.y, -center_.z));
  return rot * trans;
}

glm::mat4 Frustum::GetInvViewMatrix() const {
  const glm::mat4 rot(u_.x, u_.y, u_.z, 0.0f, v_.x, v_.y, v_.z, 0.0f, n_.x,
                      n_.y, n_.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  const glm::mat4 trans = glm::translate(
      glm::mat4(1.0f), glm::vec3(center_.x, center_.y, center_.z));
  return trans * rot;
}
glm::mat4 Frustum::GetProjectionMatrix() const {
  return glm::perspective(glm::radians(fovy_), ar_, near_, far_);
}
glm::vec3 Frustum::GetOrigin() const { return center_; }
