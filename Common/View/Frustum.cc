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

  type_ = ProjectionType::Perspective;
}

void Frustum::SetupOrtho(float left, float right, float bottom, float top,
  float near, float far) {
  left_ = left;
  right_ = right;
  bottom_ = bottom;
  top_ = top;
  near_ = near;
  far_ = far;

  type_ = ProjectionType::Ortho;
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

  corners_[0] = glm::vec3(-1.0, 1.0, 1.0);
  corners_[1] = glm::vec3(1.0, 1.0, 1.0);
  corners_[2] = glm::vec3(1.0, -1.0, 1.0);
  corners_[3] = glm::vec3(-1.0, -1.0, 1.0);
       
  corners_[4] = glm::vec3(-1.0, 1.0, -1.0);
  corners_[5] = glm::vec3(1.0, 1.0, -1.0);
  corners_[6] = glm::vec3(1.0, -1.0, -1.0);
  corners_[7] = glm::vec3(-1.0, -1.0, -1.0);

  const auto kView = glm::lookAt(eyePt, lookatPt, upVec);
  const auto kProj = GetProjectionMatrix(); 
  const auto kInvVP = glm::inverse(kProj * kView);
  for (int i = 0; i < 8; i++) {
    const auto corner = kInvVP * glm::vec4(corners_[i], 1.0f);
    corners_[i] = glm::vec3(corner) / corner.w;
  }
}

glm::mat4 Frustum::GetProjectionMatrix() const {
  if (type_ == ProjectionType::Perspective) {
    return glm::perspective(glm::radians(fovy_), ar_, near_, far_);
  } else {
    return glm::ortho(left_, right_, bottom_, top_, near_, far_);
  }
}
