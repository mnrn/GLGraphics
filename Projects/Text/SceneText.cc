/**
 * @brief Diffuse Reflection Model Test
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include "SceneText.h"

#include <boost/assert.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "UI/Text.h"

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneText::OnInit() {
  Text::Create();
  Font::Create();

  fontObj_ = Font::Get().Entry("./Assets/Fonts/Cica/Cica-Regular.ttf");
  fontObj_->Setup(48);
}

void SceneText::OnDestroy() {}

void SceneText::OnUpdate(float) {}

void SceneText::OnRender() {
  glClear(GL_COLOR_BUFFER_BIT);
  Text::Get().Render("Hello!", 100.0f, 500.0f, 1.0f, static_cast<float>(width_),
                     static_cast<float>(height_),
                     glm::vec4(0.8f, 0.3f, 0.1f, 1.0f), fontObj_);
  Text::Get().Render("Goodbye!", 500.0f, 100.0f, 1.0f, static_cast<float>(width_),
                     static_cast<float>(height_),
                     glm::vec4(0.3f, 0.5f, 0.8f, 1.0f), fontObj_);
}

void SceneText::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}
