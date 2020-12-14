/**
 * @brief 視錐台クラス
 */

#include "View/Frustum.h"

#include <boost/assert.hpp>
#include <iostream>
#include <vector>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>


void Frustum::OnInit(const glm::vec3 &eyePt, const glm::vec3 &lookatPt,
                     const glm::vec3 &upVec, float near, float far, float fovy,
                     float aspectRatio) {
  Orient(eyePt, lookatPt, upVec);
  SetPerspective(fovy, aspectRatio, near, far);
}

void Frustum::Orient(const glm::vec3 &eyePt, const glm::vec3 &lookatPt,
                     const glm::vec3 &upVec) {
  n_ = glm::normalize(eyePt - lookatPt);
  u_ = glm::normalize(glm::cross(upVec, n_));
  v_ = glm::normalize(glm::cross(n_, u_));
  center_ = eyePt;
}

void Frustum::SetupCorners() {
  const float ndy = near_ * tanf(glm::radians(fovy_) / 2.0f);
  const float ndx = ar_ * ndy;
  const glm::vec3 nc = center_ + n_ * near_;

  const float fdy = far_ * tanf(glm::radians(fovy_) / 2.0f);
  const float fdx = ar_ * fdy;
  const glm::vec3 fc = center_ + n_ * far_;

  corners_.clear();
  corners_.resize(8);
  // near plane
  corners_[0] = glm::vec4(nc - u_ * ndx - v_ * ndy, 1.0f);
  corners_[1] = glm::vec4(nc - u_ * ndx + v_ * ndy, 1.0f);
  corners_[2] = glm::vec4(nc + u_ * ndx + v_ * ndy, 1.0f);
  corners_[3] = glm::vec4(nc + u_ * ndx - v_ * ndy, 1.0f);
  // far plane
  corners_[4] = glm::vec4(fc - u_ * fdx - v_ * fdy, 1.0f);
  corners_[5] = glm::vec4(fc - u_ * fdx + v_ * fdy, 1.0f);
  corners_[6] = glm::vec4(fc + u_ * fdx + v_ * fdy, 1.0f);
  corners_[7] = glm::vec4(fc + u_ * fdx - v_ * fdy, 1.0f);
}

AABB Frustum::ComputeAABB(const glm::mat4 &m) const {
  AABB box;

  for (int i = 0; i < 8; i++) {
    box.Merge(glm::vec3(m * corners_[i]));
  }

  return box;
}

void Frustum::SetPerspective(float fovy, float aspectRatio, float near,
                             float far) {
  fovy_ = fovy;
  ar_ = aspectRatio;
  near_ = near;
  far_ = far;

  SetupCorners();
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
