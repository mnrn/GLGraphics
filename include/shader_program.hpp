/**
 * @brief  Shader Relations
 * @date   2017/03/18
 */

// ********************************************************************************
// Include guard
// ********************************************************************************

#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

// ********************************************************************************
// Including file
// ********************************************************************************

#include "gl_include.h"

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>

// ********************************************************************************
// Class(es)
// ********************************************************************************

/**
 * @brief Shader type
 */
enum class ShaderType {
  Vertex,
  Fragment,
  Geometry,
  TessControl,
  TessEvaluation,
  Compute,
};

/**
 * @brief GLSL Shader Program Class
 * @ref
 * https://github.com/daw42/glslcookbook/blob/master/ingredients/glslprogram.cpp
 */
class ShaderProgram : private boost::noncopyable {
public:
  ~ShaderProgram() {
    if (handle_ == 0) {
      return;
    }

    // Query the number of attached shaders
    GLint num = 0;
    glGetProgramiv(handle_, GL_ATTACHED_SHADERS, &num);

    // Get the shader names
    GLuint *names = new GLuint[static_cast<std::size_t>(num)];
    glGetAttachedShaders(handle_, num, nullptr, names);

    // Delete
    for (GLint i = 0; i < num; i++) {
      glDeleteShader(names[i]);
    }
    delete[] names;
    glDeleteProgram(handle_);
  }

  //*--------------------------------------------------------------------------------
  // Compile & Link
  //*--------------------------------------------------------------------------------

  bool compile(const char *filepath, ShaderType type) {

    if (!isFileExists(filepath)) {
      log_ = std::string("File Not Found : ") + filepath;
      return false;
    }

    if (handle_ <= 0) {

      handle_ = glCreateProgram();
      if (handle_ == 0) {
        log_ = "Unable to create shader program.";
        return false;
      }
    }

    std::ifstream infile(filepath, std::ios::in);
    if (!infile) {
      log_ = std::string("Can't open file : ") + filepath;
      return false;
    }

    std::stringstream code;
    code << infile.rdbuf();
    infile.close();

    return compile(code.str(), type);
  }

  bool link() {

    if (isLinked_) {
      return true;
    }
    if (handle_ == 0) {
      return false;
    }

    glLinkProgram(handle_);

    // Check for errors
    int status = GL_FALSE;
    glGetProgramiv(handle_, GL_LINK_STATUS, &status);
    if (GL_FALSE == status) {
      storeLog(handle_);
      return false;
    } else {
      isLinked_ = true;
      return isLinked_;
    }
  }

  //*--------------------------------------------------------------------------------
  // Use Shader Program
  //*--------------------------------------------------------------------------------

  void use() const {
    if (handle_ > 0 && isLinked_) {
      glUseProgram(handle_);
    }
  }

  //*--------------------------------------------------------------------------------
  // Bind Location
  //*--------------------------------------------------------------------------------

  void bindAttribLocation(GLuint location, const char *name) const {
    glBindAttribLocation(handle_, location, name);
  }

  void bindFragDataLocation(GLuint location, const char *name) const {
    glBindFragDataLocation(handle_, location, name);
  }

  //*--------------------------------------------------------------------------------
  // Accessor
  //*--------------------------------------------------------------------------------

  const std::string &log() const { return log_; }
  GLuint handle() const { return handle_; }
  bool isLinked() const { return isLinked_; }

  //*--------------------------------------------------------------------------------
  // Setting Uniform Variable(s)
  //*--------------------------------------------------------------------------------

  void setUniform(const char *name, float x, float y, float z) const {
    setUniform(name, glUniform3f, x, y, z);
  }
  void setUniform(const char *name, const glm::vec3 &v) const {
    setUniform(name, v.x, v.y, v.z);
  }
  void setUniform(const char *name, const glm::vec4 &v) const {
    setUniform(name, glUniform4f, v.x, v.y, v.z, v.w);
  }
  void setUniform(const char *name, const glm::mat3 &m) const {
    setUniform(name, glUniformMatrix3fv, 1,
               static_cast<unsigned char>(GL_FALSE), std::addressof(m[0][0]));
  }
  void setUniform(const char *name, const glm::mat4 &m) const {
    setUniform(name, glUniformMatrix4fv, 1,
               static_cast<unsigned char>(GL_FALSE), std::addressof(m[0][0]));
  }
  void setUniform(const char *name, float f) const {
    setUniform(name, glUniform1f, f);
  }
  void setUniform(const char *name, int i) const {
    setUniform(name, glUniform1i, i);
  }
  void setUniform(const char *name, bool b) const {
    setUniform(name, static_cast<int>(b));
  }

private:
  //*--------------------------------------------------------------------------------
  // Private functions
  //*--------------------------------------------------------------------------------

  template <typename F, typename... Args>
  void setUniform(const char *name, F f, Args &&... args) const {
    auto location = getUniformLocation(name);
    if (location >= 0) {
      f(location, std::forward<Args>(args)...);
    }
  }

  int getUniformLocation(const char *name) const {
    return glGetUniformLocation(handle_, name);
  }

  bool isFileExists(const char *filepath) const {
    boost::system::error_code error;
    const bool result = boost::filesystem::exists(filepath, error);
    return !error && result;
  }

  bool compile(const std::string &src, ShaderType type) {
    GLuint handle = 0;
    switch (type) {
    case ShaderType::Vertex:
      handle = glCreateShader(GL_VERTEX_SHADER);
      break;
    case ShaderType::Fragment:
      handle = glCreateShader(GL_FRAGMENT_SHADER);
      break;
    case ShaderType::Geometry:
      handle = glCreateShader(GL_GEOMETRY_SHADER);
      break;
    case ShaderType::TessControl:
      handle = glCreateShader(GL_TESS_CONTROL_SHADER);
      break;
    case ShaderType::TessEvaluation:
      handle = glCreateShader(GL_TESS_EVALUATION_SHADER);
      break;
    case ShaderType::Compute:
      handle = glCreateShader(GL_COMPUTE_SHADER);
      break;
    default:
      return false;
    }

    const char *code = src.c_str();
    glShaderSource(handle, 1, std::addressof(code), nullptr);
    glCompileShader(handle);

    // Check for errors
    int res = GL_FALSE;
    glGetShaderiv(handle, GL_COMPILE_STATUS, std::addressof(res));
    if (GL_FALSE == res) {
      storeLog(handle);
      return false;
    } else {
      glAttachShader(handle_, handle);
      return true;
    }
  }

  void storeLog(GLuint handle) {
    int length = 0;
    log_ = "";

    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
    if (length <= 0) {
      return;
    }

    char *log = new char[static_cast<std::size_t>(length)];
    int written = 0;
    glGetProgramInfoLog(handle, length, &written, log);
    log_ = log;
    delete[] log;
  }

  //*--------------------------------------------------------------------------------
  // Member Variable(s)
  //*--------------------------------------------------------------------------------

  GLuint handle_ = 0;
  bool isLinked_ = false;

  std::string log_;
};

#endif // end ifndef SHADER_PROGRAM_HPP
