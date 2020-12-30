/**
 * @brief  Shader Relations
 * @date   2017/03/18
 */

// ********************************************************************************
// Include guard
// ********************************************************************************

#ifndef SHADER_H
#define SHADER_H

// ********************************************************************************
// Including file
// ********************************************************************************

#include "GLInclude.h"

#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

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
 */
class ShaderProgram : private boost::noncopyable {
public:
  ShaderProgram() = default;
  ~ShaderProgram();

  //*--------------------------------------------------------------------------------
  // Compile & Link
  //*--------------------------------------------------------------------------------

  bool Compile(const std::string &filepath, ShaderType type);
  bool Link();

  //*--------------------------------------------------------------------------------
  // Use Shader Program
  //*--------------------------------------------------------------------------------

  void Use() const;

  //*--------------------------------------------------------------------------------
  // Utility
  //*--------------------------------------------------------------------------------

  std::optional<std::string> CompileAndLink(
      const std::vector<std::pair<std::string, ShaderType>> &shaders);

  //*--------------------------------------------------------------------------------
  // Bind Location
  //*--------------------------------------------------------------------------------

  void BindAttribLocation(GLuint location, const char *name) const {
    glBindAttribLocation(handle_, location, name);
  }

  void BindFragDataLocation(GLuint location, const char *name) const {
    glBindFragDataLocation(handle_, location, name);
  }

  //*--------------------------------------------------------------------------------
  // Accessor
  //*--------------------------------------------------------------------------------

  const std::string &GetLog() const { return log_; }
  GLuint GetHandle() const { return handle_; }
  bool IsLinked() const { return isLinked_; }

  //*--------------------------------------------------------------------------------
  // Setting Uniform Variable(s)
  //*--------------------------------------------------------------------------------

  void SetUniform(const char *name, float x, float y, float z) const {
    SetUniform(name, glUniform3f, x, y, z);
  }
  void SetUniform(const char *name, const glm::vec3 &v) const {
    SetUniform(name, v.x, v.y, v.z);
  }
  void SetUniform(const char *name, const glm::vec4 &v) const {
    SetUniform(name, glUniform4f, v.x, v.y, v.z, v.w);
  }
  void SetUniform(const char *name, const glm::mat3 &m) const {
    SetUniform(name, glUniformMatrix3fv, 1,
               static_cast<unsigned char>(GL_FALSE), std::addressof(m[0][0]));
  }
  void SetUniform(const char *name, const glm::mat4 &m) const {
    SetUniform(name, glUniformMatrix4fv, 1,
               static_cast<unsigned char>(GL_FALSE), std::addressof(m[0][0]));
  }
  void SetUniform(const char *name, float f) const {
    SetUniform(name, glUniform1f, f);
  }
  void SetUniform(const char *name, int i) const {
    SetUniform(name, glUniform1i, i);
  }
  void SetUniform(const char *name, bool b) const {
    SetUniform(name, static_cast<int>(b));
  }

private:
  //*--------------------------------------------------------------------------------
  // Private functions
  //*--------------------------------------------------------------------------------

  template <typename F, typename... Args>
  void SetUniform(const char *name, F f, Args &&...args) const {
    auto location = GetUniformLocation(name);
    if (location >= 0) {
      f(location, std::forward<Args>(args)...);
    }
#if (!NDEBUG)
    else {
      BOOST_ASSERT_MSG(false, "Failed to get uniform location.");
    }
#endif
  }

  int GetUniformLocation(const char *name) const {
    return glGetUniformLocation(handle_, name);
  }

  bool IsFileExists(const std::string &filepath) const {
    std::error_code ec;
    const bool result = std::filesystem::exists(filepath, ec);
    return !ec && result;
  }

  bool CompileShader(const std::string &src, ShaderType type);
  GLuint CreateShader(ShaderType type) const;
  void StoreLog(GLuint handle);

  //*--------------------------------------------------------------------------------
  // Member Variable(s)
  //*--------------------------------------------------------------------------------

  GLuint handle_ = 0;
  bool isLinked_ = false;
  std::string log_;
};

#endif
