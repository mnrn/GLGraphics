#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "GLInclude.h"

#include "Primitive/Drawable.h"
#include "Box/AABB.h"

#include <vector>

class Frustum {
public:
  void OnInit(const glm::vec3 &eyePt, const glm::vec3 &lookatPt,
              const glm::vec3 &upVec, float fovy, float aspectRatio, float near,
              float far);
  void Orient(const glm::vec3 &eyePt, const glm::vec3 &lookatPt,
              const glm::vec3 &upVec);
  void SetPerspective(float fovy, float aspectRatio, float near, float far);
  void SetupCorners();
  AABB ComputeAABB(const glm::mat4 &m) const;

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
  std::vector<glm::vec4> corners_;
};

#endif
