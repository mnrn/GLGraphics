/**
 * @brief  カスケードシャドウマッピングのテスト
 */

// ********************************************************************************
// Includeing files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "Scene/SceneLoop.h"
#include "SceneCSM.h"

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
  App app("Cascaded Shadow Mapping");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneCSM>();

  // Enter the main loop
  return SceneLoop::Run(app, std::move(scene));
}
