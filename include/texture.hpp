/**
 * @brief
 */

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

// ********************************************************************************
// Include files
// ********************************************************************************

#include "gl_include.hpp"

#include <type_traits>

// ********************************************************************************
// Traits
// ********************************************************************************

template <typename T> struct texture_traits {
  using typename T::path_type path_type;
  static_assert(std::is_const<path_type>::value,
                "path_type needs const-qualified type!");

  static T construct(path_type texturePath) { return T(texturePath); }

  using typename T::handle_type handle_type;
  static handle_type handle(const T &t) { return t.handle_; }

  using typename T::target_type target_type;
  static target_type target(const T &t) { return t.target_; }
};

template <typename T> struct is_texture_type : std::false_type {};

struct texture_1d_tag {};
struct texture_2d_tag {};
struct texture_3d_tag {};
struct texture_cube_map_tag {};

struct texture_1d_array_tag {};
struct texture_2d_array_tag {};
struct texture_3d_array_tag {};
struct texture_cube_map_array_tag {};

// ********************************************************************************
// Details
// ********************************************************************************

namespace texture {

template <typename Texture> void bind(const Texture &t, GLenum unit) {
  static_assert(is_texture_type<Texture>::value, "Texture needs texture-type!");

  glActiveTexture(unit);
  glBindTexture(texture_traits<Texture>::target(t),
                texture_traits<Texture>::handle(t));
}

template <typename Texture> void unbind(const Texture &t) {
  static_assert(is_texture_type<Texture>::value, "Texture needs texture-type!");

  glBindTexture(texture_traits<Texture>::target(t), 0);
}

#include "texture.ipp"

} // namespace texture

#endif // ifndef TEXTURE_HPP
