/**
 * @brief カスケードシャドウマッピング
 */

#ifndef CSM_H
#define CSM_H

#include <array>
#include <functional>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <optional>
#include <string>

#include "Geometry/AABB.h"
#include "Geometry/BSphere.h"
#include "Graphics/Shader.h"
#include "Mesh/ObjMesh.h"
#include "Primitive/Plane.h"
#include "Primitive/Teapot.h"
#include "Primitive/Torus.h"
#include "UI/Font.h"
#include "View/Camera.h"
#include "View/Frustum.h"

class CSM {
public:
  std::vector<float> ComputeSplitPlanes(int cascades, float near, float far,
                                        float lambda);
  void
  UpdateSplitPlanesUniform(int cascades, const std::vector<float> &splits,
                           const Camera &camera,
                           std::function<void(int, float)> loopEndCallback);
  void UpdateFrustums(int cascades, const std::vector<float> &splits,
                      const Camera &camera);
  std::vector<glm::mat4> ComputeCropMatrices(int cascades,
                                             const glm::vec3 &lightDir,
                                             float shadowMapSize);

private:
  glm::mat4 ComputeLightViewMatrix(const glm::vec3 &lightDir,
                                   const glm::vec3 &center,
                                   float offsetZ) const;
  glm::mat4 ComputeCropMatrix(const glm::mat4 &view, const glm::mat4 &proj,
                              float shadowMapSize) const;

  glm::mat4 ComputeCropMatrix(const glm::vec3 &min, const glm::vec3 &max) const;
  std::pair<float, float> FindZRange(const Frustum &frustum,
                                     const glm::mat4 &mv) const;
  std::pair<glm::vec2, glm::vec2> FindExtendsProj(const Frustum &frustum,
                                                  const glm::mat4 &mvp) const;

  std::vector<Frustum> frustums_{};
};

#endif
