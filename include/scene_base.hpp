/**
 * @brief  Scene Base Class
 * @date   2017/03/19
 */

#ifndef SCENE_BASE_HPP
#define SCENE_BASE_HPP

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Base Class
 */
class SceneBase {
public:
  virtual ~SceneBase() = default;
  virtual void update(float d) = 0;
  virtual void render() const = 0;

private:
};

#endif // end of SCENE_BASE_HPP
