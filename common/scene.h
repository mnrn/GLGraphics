/**
 * @brief  Scene Base Class
 * @date   2017/03/19
 */

#ifndef SCENE_H
#define SCENE_H

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
  virtual void OnRender() const = 0;
};

#endif
