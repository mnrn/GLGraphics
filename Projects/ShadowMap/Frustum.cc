/**
 * @brief 視錐台クラス
 */

#include "Frustum.h"

#include <boost/assert.hpp>
#include <iostream>
#include <vector>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

Frustum::~Frustum() { DeleteBuffers(); }

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

void Frustum::SetPerspective(float fovy, float aspectRatio, float near,
                             float far) {
  DeleteBuffers();
  fovy_ = fovy;
  ar_ = aspectRatio;
  near_ = near;
  far_ = far;

  const float dy = near_ * tanf(glm::radians(fovy_) / 2.0f);
  const float dx = ar_ * dy;
  const float fdy = far_ * tanf(glm::radians(fovy_) / 2.0f);
  const float fdx = ar_ * fdy;

  const std::vector<GLfloat> vert{
      0,    0,   0,      dx,   dy,   -near_, -dx, dy,   -near_,
      -dx,  -dy, -near_, dx,   -dy,  -near_, fdx, fdy,  -far_,
      -fdx, fdy, -far_,  -fdx, -fdy, -far_,  fdx, -fdy, -far_};

  const std::vector<GLuint> el{0, 5, 0, 6, 0, 7, 0, 8,
                               // The near plane
                               1, 2, 2, 3, 3, 4, 4, 1,
                               // The far plane
                               5, 6, 6, 7, 7, 8, 8, 5};

  glGenBuffers(1, &vbo_[VertexPosition]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[VertexPosition]);
  glBufferData(GL_ARRAY_BUFFER, 9 * 3 * sizeof(GLfloat), vert.data(),
               GL_STATIC_DRAW);

  glGenBuffers(1, &vbo_[IndexBuffer]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[IndexBuffer]);
  glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLuint), el.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[VertexPosition]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_[IndexBuffer]);
  glBindVertexArray(0);
}

void Frustum::Render() const {
  if (vao_ != 0) {
    glBindVertexArray(vao_);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
}

void Frustum::DeleteBuffers() {
  if (!vbo_.empty()) {
    glDeleteBuffers(static_cast<GLsizei>(vbo_.size()), vbo_.data());
  }
  if (vao_ != 0) {
    glDeleteVertexArrays(1, &vao_);
    vao_ = 0;
  }
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
