/**
 * @brief ゆるくやる
 */

#ifndef YRYR_HPP
#define YRYR_HPP

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/noncopyable.hpp>

#include "common.hpp"
#include "glinclude.hpp"

#include "debug.hpp"
#include "window.hpp"

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace yr {

// ********************************************************************************
// Class
// ********************************************************************************

class yrgl : private boost::noncopyable {
public:
  yrgl(const char *app_name) {
    glfwInit();

    window_ = window::create(1280, 720, app_name);

    initGlew();
    debug::setupInfo();
  }

  ~yrgl() {

    window::destroy(window_);

    glfwTerminate();
  }

  template <typename F> void run(F callback) {
    window::loop(window_, callback);
  }

private:
  GLFWwindow *window_ = nullptr;

  static void initGlew() {

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    const GLenum err = glewInit();
    if (err == GLEW_OK) {
      std::cout << "GLEW_OK: Glew Ver." << glewGetString(GLEW_VERSION)
                << std::endl;
    } else {
      std::cerr << "GLEW Error:" << glewGetErrorString(err) << std::endl;
    }
    if (!GLEW_ARB_vertex_array_object) {
      std::cerr << "ARB_vertex_array_object not avaliable" << std::endl;
    }
  }
};

} // namespace yr

#endif // end ifndef YRYR_HPP
