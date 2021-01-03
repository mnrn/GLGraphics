/**
 * @brief Diffuse Reflection Model Test
 */

// ********************************************************************************
// Including files
// ********************************************************************************

#include "SceneGUI.h"

#include <boost/assert.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneGUI::OnInit() {}

void SceneGUI::OnDestroy() {}

void SceneGUI::OnUpdate(float) {}

void SceneGUI::OnRender() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (param_.showDemoWindow) {
    ImGui::ShowDemoWindow(&param_.showDemoWindow);
  }

  ImGui::Begin("Hello, world!");

  ImGui::Text("This is some useful text.");
  ImGui::Checkbox("Demo Window", &param_.showDemoWindow);
  ImGui::Checkbox("Another Window", &param_.showAnotherWindow);

  ImGui::SliderFloat("float", &param_.f, 0.0f, 1.0f);
  ImGui::ColorEdit3("Clear Color",
                    reinterpret_cast<float *>(&param_.clearColor));

  if (ImGui::Button("Button")) {
    param_.counter++;
  }
  ImGui::SameLine();
  ImGui::Text("Counter = %d", param_.counter);

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  ImGui::End();

  if (param_.showAnotherWindow) {
    ImGui::Begin("Another Window", &param_.showAnotherWindow);
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me")) {
      param_.showAnotherWindow = false;
    }
    ImGui::End();
  }

  ImGui::Render();
  glClearColor(param_.clearColor.x, param_.clearColor.y, param_.clearColor.z,
               param_.clearColor.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SceneGUI::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}
