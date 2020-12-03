/**
 * @brief  Wrapper for Scene and App class
 */

#ifndef SCENE_LOOP_H
#define SCENE_LOOP_H

// ********************************************************************************
// Includes
// ********************************************************************************

#include "GLInclude.h"

#include "App.h"
#include "Scene.h"

// ********************************************************************************
// Functions
// ********************************************************************************

namespace SceneLoop {
static int Run(App &app, std::unique_ptr<Scene> &&scene) {
  return app.Run(
      [&scene](int w, int h) {
        scene->SetDimensions(w, h);
        scene->OnInit();
        scene->OnResize(w, h);
      },
      [&scene](float t) { scene->OnUpdate(t); },
      [&scene]() { scene->OnRender(); });
}
} // namespace SceneLoop

#endif