/**
 * @brief  Render Particles
 * @date   2017/08/27
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "SceneParticles.hpp"
#include "app.hpp"

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
  app.run([&pScene]() {
    pScene->Update(0.0f);
    pScene->Render();
  });

  return 0;
}
