#pragma once

#include "TriangleMesh.h"

class Torus : public TriangleMesh {
public:
  Torus(GLfloat outerRadius, GLfloat innerRadius, GLuint nsides, GLuint nrings);
};
