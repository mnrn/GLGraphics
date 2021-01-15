/**
 * @brief 遅延レンダリングのテストシーン
 */

#ifndef SCENE_DEFERRED_H
#define SCENE_DEFERRED_H

#include "Scene/Scene.h"

#include <glm/gtc/constants.hpp>
#include <optional>
#include <string>

#include "GBuffer.h"
#include "Graphics/Shader.h"
#include "Primitive/Plane.h"
#include "Primitive/Teapot.h"
#include "Primitive/Torus.h"

class SceneDeferred : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  void Pass1();
  void Pass2();
  void SetMatrices();
  std::optional<std::string> CompileAndLinkShader();

  static inline constexpr float rotSpeed_ = glm::pi<float>() / 8.0f;

  Plane plane_{50.0f, 50.0f, 1, 1};
  Torus torus_{0.7f * 1.5f, 0.3f * 1.5f, 50, 50};
  Teapot teapot_{14, glm::mat4(1.0f)};

  float angle_ = glm::pi<float>() / 2.0f;
  float tPrev_ = 0.0f;

  ShaderProgram prog_;
  GBuffer gbuffer_;

  GLuint quad_ = 0;
  std::array<GLuint, 2> vbo_;
};

#endif
