/**
 * @brief  遅延レンダリングテスト
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "SceneDeferred.h"
#include "SceneLoop.h"

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
  App app("Deferred Rendering");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneDeferred>();

  // Enter the main loop
  return SceneLoop::Run(app, std::move(scene));
}