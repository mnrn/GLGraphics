/**
 * @brief  Library Test
 */

// ********************************************************************************
// Including files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "Scene/SceneLoop.h"
#include "SceneBezier.h"

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
  App app("Bezier Curve Tesselation");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneBezier>();

  // Enter the main loop
  return SceneLoop::Run(app, std::move(scene));
}
