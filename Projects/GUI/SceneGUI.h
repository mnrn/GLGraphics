/**
 * @brief ImGui Demo
 */

#ifndef SCENE_GUI_H
#define SCENE_GUI_H

// ********************************************************************************
// Including files
// ********************************************************************************

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>
#include <optional>
#include <string>

#include "Graphics/Shader.h"
#include "Scene/Scene.h"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Diffuse Class
 */
class SceneGUI : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  struct Param {
    bool showDemoWindow = true;
    bool showAnotherWindow = false;
    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.0f);
    float f = 0.0f;
    int counter = 0;
  } param_;
};

#endif // SCENE_HELLO_TRIANGLE_H
