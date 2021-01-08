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

  void ComputeParticles();
  void DrawParticles();

  std::array<GLuint, 2> computeBuffer_{};
  GLuint hParticlesVAO_ = 0;
  GLuint hBlackHoleBuffer_ = 0;
  GLuint hBlackHoleVAO_ = 0;

  ShaderProgram render_{};
  ShaderProgram compute_{};

  float angle_ = 0.0f;
};

#endif // end of ifndef SCENE_PARTICLES_H
