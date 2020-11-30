#pragma once

#include "TriangleMesh.h"

class Plane : public TriangleMesh {
public:
  Plane(float xsize, float zsize, int xdiv, int zdiv, float smax = 1.0f,
        float tmax = 1.0f);
};
