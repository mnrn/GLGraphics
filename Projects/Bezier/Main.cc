/**
 * @brief  Library Test
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "SceneBezier.h"

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
  App app("Hello Bezier!!!");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneBezier>();

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
