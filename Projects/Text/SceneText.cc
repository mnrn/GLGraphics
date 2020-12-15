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

  Font::Get().Entry("./Assets/Fonts/Tests/Arial/arial.ttf");
}

void SceneText::OnDestroy() {}

void SceneText::OnUpdate(float) {}

void SceneText::OnRender() {
  glClear(GL_COLOR_BUFFER_BIT);
  /*
    Text::Get().Render("Hello!", 50.0f, 50.0f, 1.0f, static_cast<float>(width_),
                       static_cast<float>(height_),
                       glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), fontObj_);
                       */
}

void SceneText::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}
