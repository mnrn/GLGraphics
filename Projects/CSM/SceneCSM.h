/**
 * @brief カスケードシャドウマッピングのテストシーン
 */

#ifndef SCENE_CSM_H
#define SCENE_CSM_H

#include "Scene.h"

#include <array>
#include <glm/gtc/constants.hpp>
#include <optional>
#include <string>

#include "Frustum.h"
#include "Primitive/Plane.h"
#include "Primitive/Teapot.h"
#include "Primitive/Torus.h"
#include "Shader.h"
#include "CascadedShadowMapFBO.h"

class SceneCSM : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void SetMatrices();
  void SetMatrialUniforms(const glm::vec3 &diff, const glm::vec3 &amb,
                          const glm::vec3& spec, float shininess);
  void ComputeSplitPlanes(int split, float lambda, float near, float far);
  void ComputeShadowMatrices();

  void Pass1();
  void Pass2();
  void Pass3();
  void DrawScene();

  std::vector<glm::vec4> GetWorldSpaceFrustum() const;
  glm::mat4 GetLightPVMatrix(const std::vector<glm::vec4>&) const;

  static constexpr inline float kFOVY = 50.0f;
  static constexpr inline float kRotSpeed = 0.2f;
  static constexpr inline float kLambda = 0.5f;
  static constexpr inline float kEyeNear = 0.1f;
  static constexpr inline float kEyeFar = 100.0f;
  static constexpr inline float kCameraCenter = 2.0f;

  static constexpr inline int kCascadesNum = 3;
  static constexpr inline int kShadowMapWidth = 512;
  static constexpr inline int kShadowMapHeight = 512;

  Teapot teapot_{14, glm::mat4(1.0f)};
  Plane plane_{40.0f, 40.0f, 2, 2};
  Torus torus_{0.7f * 2.0f, 0.3f * 2.0f, 50, 50};

  glm::vec3 camPt_;
  Frustum lightFrustum0_{};
  std::array<glm::mat4, kCascadesNum> shadowMatrices_{};

  float tPrev_ = 0.0f;
  float angle_ = glm::quarter_pi<float>();

  enum RenderPass : std::int32_t {
    kRecordDepth,
    kShadeWithCascadedShadow,
    kDebugFrustum,
    kPassNum,
  };
  std::array<ShaderProgram, kPassNum> progs_{};
  RenderPass pass_ = kRecordDepth;
  int curShadowId_ = 0;

  CascadedMapFBO csmFBO_{};
  std::vector<float> splitPlanes_{};
};

#endif
