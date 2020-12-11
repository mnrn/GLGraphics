#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "GLInclude.h"
#include "Primitive/Drawable.h"
#include <array>

class Frustum : public Drawable {
public:
  ~Frustum();

  void OnInit(const glm::vec3 &eyePt, const glm::vec3 &lookatPt,
              const glm::vec3 &upVec, float fovy, float aspectRatio, float near,
              float far);
  void Orient(const glm::vec3 &eyePt, const glm::vec3 &lookatPt,
              const glm::vec3 &upVec);
  void SetPerspective(float fovy, float aspectRatio, float near, float far);
  void Render() const;

  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetInvViewMatrix() const;
  glm::mat4 GetProjectionMatrix() const;
  glm::vec3 GetOrigin() const;

private:
  void DeleteBuffers();

  float fovy_;
  float ar_;
  float near_;
  float far_;

  glm::vec3 center_, u_, v_, n_;

  enum VertexBuffer {
    VertexPosition,
    IndexBuffer,
    VertexBufferSize,
  };
  std::array<GLuint, VertexBufferSize> vbo_{};
  GLuint vao_ = 0;
};

#endif
