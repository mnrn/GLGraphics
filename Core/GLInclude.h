/**
 * @brief OpenGL include
 */

// ********************************************************************************
// Include guard
// ********************************************************************************

#ifndef GLINCLUDE_H
#define GLINCLUDE_H

// ********************************************************************************
// Including files
// ********************************************************************************

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <glm/glm.hpp>

#endif // GLINCLUDE_H
