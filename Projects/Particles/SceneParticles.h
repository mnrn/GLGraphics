/**
 * @brief
 */

#ifndef SCENE_PARTICLES_H
#define SCENE_PARTICLES_H

// ********************************************************************************
// Including files
// ********************************************************************************

#include "Scene/Scene.h"

#include <array>

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
  bool CompileAndLinkShader();

  static constexpr inline int32_t particlesXNum_ = 100;
  static constexpr inline int32_t particlesYNum_ = 100;
  static constexpr inline int32_t particlesZNum_ = 100;
  static constexpr inline int32_t totalParticlesNum_ =
      particlesXNum_ * particlesYNum_ * particlesZNum_;
  static constexpr inline int32_t localSizeX_ =
      1000; //!< コンピュートシェーダーの値と同じにする必要があります。

  std::array<GLuint, 2> computeBuffer_;
  GLuint hParticlesVAO_ = 0;
  GLuint hBlackHoleBuffer_ = 0;
  GLuint hBlackHoleVAO_ = 0;

  ShaderProgram render_;
  ShaderProgram compute_;

  glm::vec4 blackHole1Pos_{5.0f, 0.0f, 0.0f, 1.0f};
  glm::vec4 blackHole2Pos_{-5.0f, 0.0f, 0.0f, 1.0f};

  float angle_ = 0.0f;
};

#endif // end of ifndef SCENE_PARTICLES_H
