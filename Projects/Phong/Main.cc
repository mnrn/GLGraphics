/**
 * @brief  Diffuse Model Test
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "ScenePhong.h"

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
  App app("Phong Shading Model");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<ScenePhong>();

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
