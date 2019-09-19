/**
 * @brief  Library Test
 * @date   2016/03/19
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/assert.hpp>
#include <memory>

#include "SceneHelloTriangle.hpp"
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
  yr::YrGL yryr("Hello Triangle!!!");

  // Create scene
  std::unique_ptr<Scene::Base> pScene =
      std::make_unique<Scene::HelloTriangle>();

  // Enter the main loop
  yryr.run([&pScene]() {
    pScene->update(0.0f);
    pScene->render();
  });

  return 0;
}
