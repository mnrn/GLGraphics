/**
 * @brief  Diffuse Model Test
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "Scene/SceneLoop.h"
#include "SceneText.h"

// ********************************************************************************
// Entry point
// ********************************************************************************

int main(
#if true
    void
#else
    int argc, char **argv
#endif
) {
  App app("Text Rendering.");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneText>();

  // Enter the main loop
  return SceneLoop::Run(app, std::move(scene));
}
