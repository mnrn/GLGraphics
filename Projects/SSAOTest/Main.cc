/**
 * @brief  遅延レンダリングテスト
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "Scene/SceneLoop.h"
#include "SceneSSAOTest.h"

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
  App app("Screen Space Ambient Occlusion");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneSSAOTest>();

  // Enter the main loop
  return SceneLoop::Run(app, std::move(scene));
}
