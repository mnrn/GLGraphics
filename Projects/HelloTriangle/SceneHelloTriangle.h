/**
 * @brief Scene Triangle
 * @date 2017/
 */

#ifndef SCENE_HELLO_TRIANGLE_H
#define SCENE_HELLO_TRIANGLE_H

// ********************************************************************************
// Including files
// ********************************************************************************

#include <array>
#include <optional>
#include <string>

#include "Graphics/Shader.h"
#include "Scene/Scene.h"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Hello Triangle Class
 */
class SceneHelloTriangle : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void CreateVAO();

  enum VertexBuffer {
    Position,
    Color,
    BufferNum,
  };
  std::array<GLuint, BufferNum> vbo_;
  GLuint vao_ = 0;
  ShaderProgram prog_;
};

#endif // SCENE_HELLO_TRIANGLE_H
