/**
 * @brief  Render Particles
 * @date   2017/08/27
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "SceneParticles.h"

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

  App app("Compute Particles");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneParticles>();

  // Enter the main loop
  return app.Run([&scene](int w, int h) { scene->OnResize(w, h); },
                 [&scene](float dt) { scene->OnUpdate(dt); },
                 [&scene]() { scene->OnRender(); });
}
