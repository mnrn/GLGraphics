/**
 * @brief  Library Test
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "scene_bezier.hpp"
#include "yryr.hpp"

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
  yr::yrgl yryr("Hello Bezier!!!");

  // Create scene
  std::unique_ptr<SceneBase> scene = std::make_unique<SceneBezier>();

  // Enter the main loop
  yryr.run([&scene]() {
    scene->update(0.0f);
    scene->render();
  });

  return 0;
}
