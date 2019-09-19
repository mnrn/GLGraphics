/**
 * @brief  Library Test
 * @date   2016/03/19
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/assert.hpp>
#include <memory>

#include "scene_hello_triangle.hpp"
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
  yr::yrgl yryr("Hello Triangle!!!");

  // Create scene
  std::unique_ptr<scene::base> scene =
      std::make_unique<scene::hello_triangle>();

  // Enter the main loop
  yryr.run([&scene]() {
    scene->update(0.0f);
    scene->render();
  });

  return 0;
}
