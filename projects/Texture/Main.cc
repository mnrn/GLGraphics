/**
 * @brief  Texture Test
 * @date   2016/03/19
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
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
  return app.Run([&scene](int w, int h) { scene->OnResize(w, h); },
                 [&scene](float dt) { scene->OnUpdate(dt); },
                 [&scene]() { scene->OnRender(); });
}
