/**
 * @brief 物理ベースレンダリングのテストシーン
 */

#ifndef SCENE_PBR_H
#define SCENE_PBR_H

#include "Scene.h"

#include <glm/gtc/constants.hpp>
#include <optional>
#include <string>

#include "Mesh/ObjMesh.h"
#include "Primitive/Plane.h"
#include "Primitive/Teapot.h"
#include "Shader.hh"

class ScenePBR : public Scene {
public:
  void OnInit() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void SetMatrices();
  void DrawScene();
  void DrawFloor();
  void DrawSpot(const glm::vec3 &pos, float rough, int metal,
                const glm::vec3 &color);

  static constexpr inline float kFOVY = 60.0f;
  static constexpr inline float kMetalRough = 0.43f;
  static constexpr inline int kNumCows = 9;
  static constexpr glm::vec3 kBaseCowColor{0.2f, 0.33f, 0.17f};

  ShaderProgram prog_;

  std::unique_ptr<ObjMesh> spotCowMesh_ =
      std::make_unique<ObjMesh>("./Assets/Models/Tests/Cube/cube.obj");

  Teapot teapot_{50, glm::mat4(1.0f)};
  Plane plane_{20, 20, 1, 1};

  float tPrev_ = 0.0f;
  float lightAngle_ = 0.0f;
  float lightRotationSpeed_ = 1.5f;
  std::vector<glm::vec4> lightPositions_ = {glm::vec4(5.0f, 5.0f, 5.0f, 1.0f),
                                            glm::vec4(0.0f, 0.15f, -1.0f, 0.0f),
                                            glm::vec4(-7.0f, 3.0f, 7.0f, 1.0f)};
};

#endif
