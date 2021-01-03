/**
 * @brief SSAOのテストシーン
 */

#ifndef SCENE_SSAO_H
#define SCENE_SSAO_H

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

class SceneSSAO : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void SetupShaderConfig();
  void CreateVAO();
  void SetMatrices();
  void SetupSSAO();

  void Pass1();
  void Pass2();
  void Pass3();
  void Pass4();

  void DrawScene();
  void DrawQuad();
  void DrawGUI();

  Plane plane_{10.0f, 10.0f, 1, 1, 10, 7};
  std::unique_ptr<ObjMesh> mesh_ =
      std::make_unique<ObjMesh>("./Assets/Models/Tests/Teapot/teapot.obj");

  Camera camera_{};

  enum RenderPass {
    RecordGBufferPass,
    SSAOPass,
    BlurPass,
    LightingPass,
    PassMax,
  };
  std::array<ShaderProgram, PassMax> progs_{};
  GBuffer gbuffer_{};

  enum Textures { WoodTex, BrickTex, RandRotTex, TexturesMax };
  std::array<GLuint, TexturesMax> textures_{};

  GLuint quadVAO_ = 0;
  GLuint quadVBO_ = 0;

  enum RenderType {
    RenderSSAO,
    RenderSSAOOnly,
    RenderNoSSAO,
    RenderTypeNum,
  };

  struct Param {
    int type = RenderSSAO;
  } param_{};
};

#endif
