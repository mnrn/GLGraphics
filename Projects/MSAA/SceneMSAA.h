/**
 * @brief MSAAのテストシーン
 * @date 2020
 */

#ifndef SCENE_MSAA_H
#define SCENE_MSAA_H

// ********************************************************************************
// Including files
// ********************************************************************************

#include <array>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <optional>
#include <string>

#include "Scene/Scene.h"

#include "Graphics/Shader.h"
#include "UI/Font.h"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Hello Triangle Class
 */
class SceneMSAA : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void CreateVAO();

  void DrawQuad();
  void DrawText();

  static constexpr inline float kRotSpeed = glm::pi<float>() / 8.0f;

  float angle_ = glm::pi<float>() / 2.0f;
  float tPrev_ = 0.0f;

  bool isEnabledMSAA_ = true;
  bool isEnabledCentroid_ = true;

  enum VertexBuffer {
    Position,
    Normal,
    TexCoord,
    BufferNum,
  };
  std::array<GLuint, BufferNum> vbo_;
  GLuint quad_ = 0;
  ShaderProgram centroid_;
  ShaderProgram noCentroid_;

  std::unique_ptr<FontObj> fontObj_;
};

#endif // SCENE_MSAA_H
