/**
 * @brief 遅延レンダリングのテストシーン
 */

#ifndef SCENE_SSAO_TEST_H
#define SCENE_SSAO_TEST_H

#include "Scene/Scene.h"

#include <glm/gtc/constants.hpp>
#include <memory>
#include <optional>
#include <string>

#include "GBuffer.h"
#include "Graphics/Shader.h"
#include "Mesh/ObjMesh.h"
#include "Primitive/Plane.h"
#include "Primitive/Teapot.h"
#include "Primitive/Torus.h"
#include "View/Camera.h"

class SceneSSAOTest : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  void SetMatrices();
  void CreateVAO();
  std::optional<std::string> CompileAndLinkShader();
  void SetupShaderConfig();

  void Pass1();
  void Pass2();

  void DrawScene();
  void DrawQuad();

  static inline constexpr float rotSpeed_ = 0.2f;

  Plane plane_{10.0f, 10.0f, 1, 1, 10, 7};
  std::unique_ptr<ObjMesh> mesh_ =
      std::make_unique<ObjMesh>("./Assets/Models/Tests/Teapot/teapot.obj");

  float angle_ = glm::pi<float>() / 2.0f;
  float tPrev_ = 0.0f;

  Camera camera_{};

  enum RenderPass {
    RecordGBuffer,
    Lighting,
  };
  std::array<ShaderProgram, 2> progs_{};
  GBuffer gbuffer_{};

  enum Textures {
    WoodTex,
    BrickTex,
    RandRotTex,
  };
  std::array<GLuint, 3> textures_{};

  GLuint quadVAO_ = 0;
  GLuint quadVBO_ = 0;
};

#endif
