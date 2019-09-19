/**
 * @brief  Scene Base Class
 * @date   2017/03/19
 */

#ifndef SCENE_BASE_HPP
#define SCENE_BASE_HPP

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace scene {

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Base Class
 */
class base {
public:
  virtual ~base() = default;
  virtual void update(float d) = 0;
  virtual void render() const = 0;

private:
};

} // namespace Scene

#endif // end of SCENE_BASE_HPP
