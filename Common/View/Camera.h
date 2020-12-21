#pragma once

#include "GLInclude.h"

#include <optional>

#include "View/Frustum.h"

enum struct ProjectionType {
  Perspective,
  Ortho,
  Num,
};

class Camera {
public:
  void SetupOrient(const glm::vec3 &eyePt, const glm::vec3 &lookatPt,
                   const glm::vec3 &upVec);
  void SetupPerspective(float fovy, float aspectRatio, float near, float far);

  void SetPosition(const glm::vec3 &eyePt) { eyePt_ = eyePt; }

  glm::vec3 GetPosition() const { return eyePt_; }
  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetInverseViewMatrix() const;
  glm::mat4 GetProjectionMatrix() const;

  // void OnPostUpdate() { frustum_.SetupCorners(eyePt_, lookatPt_, upVec_); }

private:
  // ProjectionType type_ = ProjectionType::Perspective;

  glm::vec3 eyePt_;
  glm::vec3 lookatPt_;
  glm::vec3 upVec_;

  Frustum frustum_;
};
