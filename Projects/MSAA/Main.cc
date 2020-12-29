/**
 * @brief  Library Test
 * @date   2016/03/19
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "Scene/SceneLoop.h"
#include "SceneMSAA.h"

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
  App app("Multi Sampling Anti Aliasing Scene", Default::Window::Width,
          Default::Window::Height, 8);

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneMSAA>();

  // Enter the main loop
  return SceneLoop::Run(app, std::move(scene));
}
