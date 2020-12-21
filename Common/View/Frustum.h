#pragma once

#include "GLInclude.h"

#include <vector>

#include "Box/AABB.h"
#include "Primitive/Drawable.h"

class Frustum {
public:
  void SetupPerspective(float fovy, float aspectRatio, float near, float far);
  void SetupCorners(const glm::vec3 &eyePt, const glm::vec3 &lookatPt,
                    const glm::vec3 &upVec);

  glm::mat4 GetProjectionMatrix() const;

  AABB ComputeAABB(const glm::mat4 &m) const;

private:
  float fovy_;
  float ar_;
  float near_;
  float far_;

  std::vector<glm::vec4> corners_;
};
