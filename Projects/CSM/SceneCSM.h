/**
 * @brief カスケードシャドウマッピングのテストシーン
 */

#ifndef SCENE_CSM_H
#define SCENE_CSM_H

#include "Scene/Scene.h"

#include <array>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <optional>
#include <string>

#include "Geometry/AABB.h"
#include "Geometry/BSphere.h"
#include "Graphics/Shader.h"
#include "Mesh/ObjMesh.h"
#include "Primitive/Plane.h"
#include "Primitive/Teapot.h"
#include "Primitive/Torus.h"
#include "View/Camera.h"
#include "View/Frustum.h"

#include "CascadedShadowMapsFBO.h"

class SceneCSM : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  void OnPreRender();

  std::optional<std::string> CompileAndLinkShader();
  void SetMatrices();
  void SetMaterialUniforms(const glm::vec3 &diff, const glm::vec3 &amb,
                           const glm::vec3 &spec, float shininess);
  void SetupCamera();

  void Pass1();
  void Pass2();

  void UpdateGUI();
  void DrawScene();

  Camera camera_;

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
  int cascadeIdx_ = 0;

  CascadedShadowMapsFBO csmFBO_{};
  std::vector<glm::mat4> vpCrops_{};

  struct Param {
    int cascades = 3;
    float schemeLambda = 0.5f;
    bool isPCF = true;
    bool isShadowOnly = false;
    bool isVisibleIndicator = false;
    float rotSpeed = 0.0f;
  } param_{};
};

#endif
