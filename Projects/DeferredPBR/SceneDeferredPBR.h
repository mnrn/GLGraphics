/**
 * @brief PBRの遅延レンダリングのテストシーン
 */

#ifndef SCENE_DEFFERED_PBR_H
#define SCENE_DEFFERED_PBR_H

#include "Scene.h"

#include <glm/gtc/constants.hpp>
#include <optional>
#include <string>

#include "GBuffer.h"
#include "Mesh/ObjMesh.h"
#include "Primitive/Plane.h"
#include "Primitive/Teapot.h"
#include "Primitive/Torus.h"
#include "Shader.hh"

class SceneDeferredPBR : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void CreateVAO();
  void SetMatrices();
  bool IsAnimate() const;
  void Pass1();
  void Pass2();
  void DrawScene();
  void DrawFloor();
  void DrawMesh(const glm::vec3 &pos, float rough, int metal,
                const glm::vec3 &color);

  static constexpr inline float kFOVY = 60.0f;
  static constexpr inline float kMetalRough = 0.43f;
  static constexpr inline int kNumCows = 9;
  static constexpr glm::vec3 kBaseCowColor{0.2f, 0.33f, 0.17f};

  std::unique_ptr<ObjMesh> mesh_ =
      std::make_unique<ObjMesh>("./Assets/Models/Spot/spot_triangulated.obj");
  Plane plane_{20, 20, 1, 1};

  float tPrev_ = 0.0f;
  float lightAngle_ = 0.0f;
  float lightRotationSpeed_ = 1.5f;
  std::vector<glm::vec4> lightPositions_{glm::vec4(7.0f, 3.0f, 0.0f, 1.0f),
                                         glm::vec4(0.0f, 0.15f, 0.0f, 0.0f),
                                         glm::vec4(-7.0f, 3.0f, 7.0f, 1.0f)};
  ShaderProgram prog_;
  GBuffer gbuffer_;
  GLuint quad_ = 0;
  std::array<GLuint, 2> vbo_;
};

#endif
