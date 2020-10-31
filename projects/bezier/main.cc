/**
 * @brief  Library Test
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "app.h"
#include "scene_bezier.h"

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
  std::unique_ptr<SceneBase> scene = std::make_unique<SceneBezier>();

  // Enter the main loop
  app.run([&scene](float dt) {
    scene->update(dt);
    scene->render();
  });

  return 0;
}
