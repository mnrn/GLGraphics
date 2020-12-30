#pragma once

// ********************************************************************************
// functions
// ********************************************************************************

template<typename E> constexpr auto to_i(E e) {
  return static_cast <typename std::underlying_type_t<E>>(e);
}
