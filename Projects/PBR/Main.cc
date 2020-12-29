/**
 * @brief 物理ベースレンダリングテスト
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "Scene/SceneLoop.h"
#include "ScenePBR.h"

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
  App app("Physically Based Rendering Scene");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<ScenePBR>();

  // Enter the main loop
  return SceneLoop::Run(app, std::move(scene));
}
