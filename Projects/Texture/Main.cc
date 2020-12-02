/**
 * @brief  Texture Test
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "SceneLoop.h"
#include "SceneTexture.h"

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
  App app("Texture Mapping");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneTexture>();

  // Enter the main loop
  return SceneLoop::Run(app, std::move(scene));
}
