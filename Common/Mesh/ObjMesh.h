#pragma once

#include "GLInclude.h"

#include "Primitive/TriangleMesh.h"
#include <vector>

class ObjMesh : public TriangleMesh {
public:
  explicit ObjMesh(const std::string &path);
};
