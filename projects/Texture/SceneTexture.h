#ifndef SCENE_TEXTURE_H
#define SCENE_TEXTURE_H

#include "Primitive/Cube.h"
#include "Scene.h"
#include "Shader.hh"

class SceneTexture : public Scene {
public:
  SceneTexture();
  ~SceneTexture() override = default;

  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();

  ShaderProgram prog_;
  float angle_;
  Cube cube_;
};

#endif
