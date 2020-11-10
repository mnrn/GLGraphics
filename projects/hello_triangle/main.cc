/**
 * @brief  Library Test
 * @date   2016/03/19
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "app.h"
#include "scene_hello_triangle.h"

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
  App app("Hello Triangle!!!");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneHelloTriangle>();

  // Enter the main loop
  app.run([&scene](float dt) {
    scene->update(dt);
    scene->render();
  });

  return 0;
}
