/**
 * @brief SSAOのレンダリングテストシーン
 */

#ifndef SCENE_SSAO_H
#define SCENE_SSAO_H

#include "Scene/Scene.h"

#include <glm/gtc/constants.hpp>
#include <memory>
#include <optional>
#include <string>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include "GBuffer.h"
#include "Graphics/Shader.h"
#include "Mesh/ObjMesh.h"
#include "Primitive/Plane.h"
#include "Primitive/Teapot.h"
#include "Primitive/Torus.h"
#include "UI/Font.h"

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
  void CreateVAO();
  void SetMatrices();

  void BuildKernel(std::uint32_t seed);
  void BuildRandRotTex(std::uint32_t seed);

  void Pass1();
  void Pass2();
  void Pass3();
  void Pass4();

  void DrawText();
  void DrawScene();
  void DrawQuad();

  static constexpr inline size_t kKernelSize =
      64; // NOTE: シェーダーのカーネルサイズと一致させる必要があります。
  static constexpr inline float kRotSpeed = 1.0f;
  static constexpr inline float kFOVY = 50.0f;

  Plane plane_{10.0f, 10.0f, 1, 1, 7, 10};
  std::unique_ptr<ObjMesh> mesh_ =
      std::make_unique<ObjMesh>("./Assets/Models/Tests/Teapot/teapot.obj");

  float angle_ = glm::pi<float>() / 2.0f;
  float tPrev_ = 0.0f;

  glm::vec4 lightPos_{3.0f, 3.0f, 1.5f, 1.0f};
  glm::mat4 sceneProj_{1.0f};

  ShaderProgram prog_{};
  GBuffer gbuffer_{};

  enum VertexBuffer {
    VertexPosition,
    TextureCoordinates,
    VertexBufferSize,
  };
  enum Textures {
    WoodTex,
    BrickTex,
    RandRotTex,
    TexturesNum,
  };
  std::array<GLuint, VertexBufferSize> vbo_{};
  std::array<GLuint, TexturesNum> textures_{};
  GLuint quad_ = 0;

  enum RenderType {
    SSAO,
    SSAOOnly,
    NoSSAO,
    RenderTypeNum,
  };
  int type_ = RenderType::SSAO;

  std::unique_ptr<FontObj> fontObj_;
};

#endif
