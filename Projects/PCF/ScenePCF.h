/**
 * @brief シャドウマッピングのテストシーン
 */

#ifndef SCENE_SHADOW_MAP_H
#define SCENE_SHADOW_MAP_H

#include "Scene/Scene.h"

#include <array>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <optional>
#include <string>

#include "Graphics/Shader.h"
#include "Mesh/ObjMesh.h"
#include "Primitive/Plane.h"
#include "Primitive/Teapot.h"
#include "Primitive/Torus.h"
#include "UI/Font.h"
#include "View/Camera.h"
#include "View/Frustum.h"

class ScenePCF : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void SetupFBO();
  void SetMatrices();
  void SetMaterialUniforms(const glm::vec3 &diff, const glm::vec3 &amb,
                          const glm::vec3 &spec, float shininess);
  void SetupCamera();
  void SetupLight();

  void Pass1();
  void Pass2();

  void DrawScene();
  void DrawStatus();

  Camera camera_;
  Camera lightView_;

  Plane plane_{20.0f, 20.0f, 1, 1};
  std::unique_ptr<ObjMesh> building_ =
      std::make_unique<ObjMesh>("./Assets/Models/SDCC/building.obj");

  float tPrev_ = 0.0f;
  float angle_ = glm::two_pi<float>() * 0.85f;

  enum RenderPass : std::int32_t {
    kRecordDepth,
    kShadeWithShadow,
    kPassNum,
  };
  std::array<ShaderProgram, kPassNum> progs_{};
  RenderPass pass_ = kRecordDepth;

  GLuint depthTex_ = 0;
  GLuint shadowFBO_ = 0;

  bool isPCF_ = true;
  bool isShadowOnly_ = false;

  std::unique_ptr<FontObj> fontObj_;
};

#endif
