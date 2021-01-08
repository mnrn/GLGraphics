/**
 * @brief 物理ベースレンダリングのテストシーン
 */

#ifndef SCENE_PBR_H
#define SCENE_PBR_H

#include "Scene/Scene.h"

#include <glm/gtc/constants.hpp>
#include <optional>
#include <string>

#include "Graphics/Shader.h"
#include "Mesh/ObjMesh.h"
#include "Primitive/Plane.h"
#include "Primitive/Teapot.h"

class ScenePBR : public Scene {
public:
  ScenePBR();
  void OnInit() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void SetMatrices();
  void UpdateGUI();
  void DrawScene();
  void DrawFloor();
  void DrawMesh(const glm::vec3 &pos, float rough, int metal,
                const glm::vec3 &color);

  ShaderProgram prog_;

  std::unique_ptr<ObjMesh> mesh_ =
      std::make_unique<ObjMesh>("./Assets/Models/Spot/spot_triangulated.obj");

  Teapot teapot_{50, glm::mat4(1.0f)};
  Plane plane_{20, 20, 1, 1};

  struct Param {
    glm::vec3 metalSpecular{1.0f, 0.71f, 0.29f};
    float metalRough = 0.43f;
    glm::vec3 dielectricBaseColor{0.2f, 0.33f, 0.17f};
    float dielectricRough = 0.43f;
  } param_;

  float tPrev_ = 0.0f;
  float lightAngle_ = 0.0f;
  float lightRotationSpeed_ = 1.5f;
  std::vector<glm::vec4> lightPositions_;
};

#endif
