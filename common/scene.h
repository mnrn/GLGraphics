/**
 * @brief  Scene Base Class
 * @date   2017/03/19
 */

#ifndef SCENE_H
#define SCENE_H

// ********************************************************************************
// Includes
// ********************************************************************************

#include "GLInclude.h"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Base Class
 */
class Scene {
public:
  virtual ~Scene() = default;
  virtual void OnUpdate(float d) = 0;
  virtual void OnRender() = 0;
  virtual void OnResize(int w, int h) = 0;

  void SetDimensions(int w, int h) {
    width_ = w;
    height_ = h;
  }

protected:
  int width_;
  int height_;
  glm::mat4 model_;
  glm::mat4 view_;
  glm::mat4 proj_;
};

#endif
