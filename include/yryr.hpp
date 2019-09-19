/**
 * @brief ゆるくやる
 */

#ifndef YRYR_HPP
#define YRYR_HPP

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/noncopyable.hpp>

#include "glInclude.hpp"
#include "yrCommon.hpp"

#include "fwWindow.hpp"
#include "glDebug.hpp"

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace yr {

// ********************************************************************************
// Class
// ********************************************************************************

class YrGL : private boost::noncopyable {
public:
  YrGL(const char *appName) {
    glfwInit();

    window_ = fw::Window::create(1280, 720, appName);

    initGLEW();
    gl::Debug::setupInfo();
  }

  ~YrGL() {

    fw::Window::destroy(window_);

    glfwTerminate();
  }

  template <typename F> void run(F callback) {
    fw::Window::loop(window_, callback);
  }

private:
  GLFWwindow *window_ = nullptr;

  static void initGLEW() {

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
