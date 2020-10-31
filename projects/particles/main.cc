/**
 * @brief  Render Particles
 * @date   2017/08/27
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "app.h"
#include "scene_particles.h"

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
  std::unique_ptr<Scene::Base> pScene = std::make_unique<Scene::Particles>();

  // Enter the main loop
  app.run([&pScene](float dt) {
    pScene->Update(dt);
    pScene->Render();
  });

  return 0;
}
