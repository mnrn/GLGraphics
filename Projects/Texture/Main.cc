/**
 * @brief  Texture Test
 */

// ********************************************************************************
// Including files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "Scene/SceneLoop.h"
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
