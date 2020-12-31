/**
 * @brief 遅延レンダリングのテストシーン
 */

#ifndef SCENE_SSAO_PROTO_H
#define SCENE_SSAO_PROTO_H

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

class SceneSSAOProto : public Scene {
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

  void BuildKernel(std::uint32_t seed);
  void BuildRandRotTex(std::uint32_t seed);

  void Pass1();
  void Pass2();
  void Pass3();
  void Pass4();

  void DrawScene();
  void DrawQuad();

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
};

#endif