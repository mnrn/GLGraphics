/**
 * @brief SSAOのレンダリングテストシーン
 */

#ifndef SCENE_SSAO_H
#define SCENE_SSAO_H

#include "Scene/Scene.h"

#include "SSAOCommon.h"

#include <array>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include "GBuffer.h"
#include "Graphics/Shader.h"
#include "Mesh/ObjMesh.h"
#include "Primitive/Plane.h"
#include "Primitive/Teapot.h"
#include "Primitive/Torus.h"
#include "UI/Font.h"
#include "View/Camera.h"

// ********************************************************************************
// Classes
// ********************************************************************************

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

  enum struct RenderPass {
    RecordGBuffer,
    SSAO,
    Blur,
    Lighting,
    Num,
  };
  std::array<ShaderProgram, to_i(RenderPass::Num)> progs_;
  GBuffer gbuffer_{};

  enum struct VertexBuffer {
    VertexPosition,
    TextureCoordinates,
    Size,
  };
  enum struct Textures {
    WoodTex,
    BrickTex,
    RandRotTex,
    Num,
  };
  std::array<GLuint, to_i(VertexBuffer::Size)> vbo_{};
  std::array<GLuint, to_i(Textures::Num)> textures_{};
  GLuint quad_ = 0;

  enum struct RenderType : int {
    SSAO,
    SSAOOnly,
    NoSSAO,
    Num,
  };
};

#endif
