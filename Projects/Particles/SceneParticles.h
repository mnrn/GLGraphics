/**
 * @brief パーティクル
 */

#ifndef SCENE_PARTICLES_H
#define SCENE_PARTICLES_H

// ********************************************************************************
// Including files
// ********************************************************************************

#include "Scene/Scene.h"

#include <array>
#include <string>
#include <optional>

#include "Graphics/Shader.h"

// ********************************************************************************
// Class
// ********************************************************************************

class SceneParticles : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  void InitBuffer();
  std::optional<std::string> CompileAndLinkShader();

  void UpdateGUI();
  void ComputeParticles();
  void DrawParticles();

  std::array<GLuint, 2> computeBuffer_{};
  GLuint hParticlesVAO_ = 0;
  GLuint hBlackHoleBuffer_ = 0;
  GLuint hBlackHoleVAO_ = 0;

  ShaderProgram render_{};
  ShaderProgram compute_{};

  float angle_ = 0.0f;
  float tPrev_ = 0.0f;

  struct Param {
    glm::vec4 particleColor{0.015f, 0.05f, 0.3f, 0.1f};
    float particleSize = 1.0f;
    float particleMass = 0.15f;
    float gravity = 1000.0f;
    float gravityAngle = 0.0f;
    float deltaTime = 0.5f;
    float limitRange = 45.0f;
    glm::vec3 clearColor{0.351f, 0.351f, 0.351f};
  } param_{};
};

#endif
