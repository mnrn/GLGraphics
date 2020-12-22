#pragma once

#include "GLInclude.h"

#include <string>
#include <vector>

#include "Primitive/TriangleMesh.h"

class ObjMesh : public TriangleMesh {
public:
  explicit ObjMesh(const std::string &path);
};
