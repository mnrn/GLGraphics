/**
 * @brief  Subroutine Test
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include "SceneSubroutine.h"

#include <memory>

#include "App.h"

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
  App app("Subroutine");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneSubroutine>();

  // Enter the main loop
  return app.Run(
      [&scene](int w, int h) {
        scene->SetDimensions(w, h);
        scene->OnInit();
        scene->OnResize(w, h);
      },
      [&scene](float dt) { scene->OnUpdate(dt); },
      [&scene]() { scene->OnRender(); });
}
