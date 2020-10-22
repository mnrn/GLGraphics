/**
 * @brief  Library Test
 * @date   2016/03/19
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "app.hpp"
#include "scene_hello_triangle.hpp"

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
  std::unique_ptr<SceneBase> scene = std::make_unique<SceneHelloTriangle>();

  // Enter the main loop
  app.run([&scene]() {
    scene->update(0.0f);
    scene->render();
  });

  return 0;
}
