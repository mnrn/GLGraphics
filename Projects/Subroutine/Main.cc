/**
 * @brief  Subroutine Test
 */

// ********************************************************************************
// Including files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "Scene/SceneLoop.h"
#include "SceneSubroutine.h"

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
  App app("Subroutine");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneSubroutine>();

  // Enter the main loop
  return SceneLoop::Run(app, std::move(scene));
}
