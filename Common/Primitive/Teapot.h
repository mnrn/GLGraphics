#pragma once

#include "TriangleMesh.h"

class Teapot : public TriangleMesh {
public:
  Teapot(int grid, const glm::mat4 &lidTrasform);

private:
  void GeneratePatches(std::vector<GLfloat> &p, std::vector<GLfloat> &n,
                       std::vector<GLfloat> &tc, std::vector<GLuint> &el,
                       int grid);
  void BuildPatchReflect(int patchNum, std::vector<GLfloat> &B,
                         std::vector<GLfloat> &dB, std::vector<GLfloat> &v,
                         std::vector<GLfloat> &n, std::vector<GLfloat> &tc,
                         std::vector<GLuint> &el, int &index, int &elIndex,
                         int &tcIndex, int grid, bool reflectX, bool reflectY);
  void BuildPatch(glm::vec3 patch[][4], std::vector<GLfloat> &B,
                  std::vector<GLfloat> &dB, std::vector<GLfloat> &v,
                  std::vector<GLfloat> &n, std::vector<GLfloat> &tc,
                  std::vector<GLuint> &el, int &index, int &elIndex,
                  int &tcIndex, int grid, glm::mat3 reflect, bool invertNormal);
  void GetPatch(int patchNum, glm::vec3 patch[][4], bool reverseV);

  void ComputeBasisFunctions(std::vector<GLfloat> &B, std::vector<GLfloat> &dB,
                             int grid);
  glm::vec3 Evaluate(int gridU, int gridV, std::vector<GLfloat> &B,
                     glm::vec3 patch[][4]);
  glm::vec3 EvaluateNormal(int gridU, int gridV, std::vector<GLfloat> &B,
                           std::vector<GLfloat> &dB, glm::vec3 patch[][4]);
  void MoveLid(int grid, std::vector<GLfloat> &p,
               const glm::mat4 &lidTransform);
};
