/**
 * @brief カスケードシャドウマッピングのテストシーン
 */

#ifndef SCENE_CSM_H
#define SCENE_CSM_H

#include "Scene.h"

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
#include "Geometry/AABB.h"
#include "Geometry/BSphere.h"

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
  void SetMatrialUniforms(const glm::vec3 &diff, const glm::vec3 &amb,
                          const glm::vec3 &spec, float shininess);
  void SetupCamera();
  void SetupLight();

  void Pass1();
  void Pass2();

  void DrawScene();
  void DrawStatus();


  std::vector<float> ComputeSplitPlanes(int cascades, float near, float far);
  void UpdateSplitPlanesUniform(int cascades, const std::vector<float> splits);
  void UpdateFrustums(int cascades, const std::vector<float> splits);
  void UpdateCropMatrices(int cascades);

  glm::mat4 ComputeCropMatrix(const glm::vec3& min, const glm::vec3& max) const;
  std::pair<float, float> FindZRange(const Frustum &frustum,
                                     const glm::mat4 &mv) const;
  std::pair<glm::vec2, glm::vec2> FindExtendsProj(const Frustum &frustum,
                                                  const glm::mat4 &mvp) const;

  glm::mat4 ComputeLightViewMatrix(const glm::vec3 &lightDir, const glm::vec3& center,
                                   float z) const;
  glm::mat4 ComputeCropMatrix(const glm::mat4 &view,
                              const glm::mat4 &proj) const;

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
  int cascadeIdx_ = 0;

  CascadedShadowMapsFBO csmFBO_{};
  std::vector<BSphere> caster_{};
  std::vector<Frustum> cascadedFrustums_{};
  std::vector<glm::mat4> vpCrop_{};
};

#endif
