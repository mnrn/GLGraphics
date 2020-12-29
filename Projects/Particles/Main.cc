/**
 * @brief  Render Particles
 * @date   2017/08/27
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "App.h"
#include "Scene/SceneLoop.h"
#include "SceneParticles.h"

#ifdef __APPLE__
#include <boost/assert.hpp>
#endif

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

#ifdef __APPLE__
  BOOST_ASSERT_MSG(false, "Can't execute in MacOS");
#endif

  App app("Compute Particles");

  // Create scene
  std::unique_ptr<Scene> scene = std::make_unique<SceneParticles>();

  // Enter the main loop
  return SceneLoop::Run(app, std::move(scene));
}
