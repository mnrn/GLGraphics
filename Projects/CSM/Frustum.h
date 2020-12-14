#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "GLInclude.h"

#include <array>

#include "Primitive/Drawable.h"
#include "Box/AABB.h"

class Frustum {
public:
  void Orient(const glm::vec3 &eyePt, const glm::vec3 &lookatPt,
              const glm::vec3 &upVec);
  void SetPerspective(float fovy, float aspectRatio, float near, float far);

  std::array<glm::vec3, 8> ComputeCorners() const;
  AABB ComputeAABB(const glm::mat4& m) const;

  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetInvViewMatrix() const;
  glm::mat4 GetProjectionMatrix() const;
  glm::vec3 GetOrigin() const;

private:
  float fovy_;
  float ar_;
  float near_;
  float far_;

  glm::vec3 center_, u_, v_, n_;
};

#endif
