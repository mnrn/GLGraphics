/**
 * @brief  カスケードシャドウマッピングのテスト
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "SceneLoop.h"
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
  App app("Cascaded shadow mapping");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneCSM>();

  // Enter the main loop
  return SceneLoop::Run(app, std::move(scene));
}