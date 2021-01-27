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
#include "View/Camera.h"
#include "View/Frustum.h"

class CSM {
public:
  std::vector<float> ComputeSplitPlanes(int cascades, float near, float far,
                                        float lambda);
  void UpdateSplitPlanesUniform(
      int cascades, const std::vector<float> &splits, const Camera &camera,
      const std::function<void(int, float)> &loopEndCallback);
  void UpdateFrustums(int cascades, const std::vector<float> &splits,
                      const Camera &camera);
  std::vector<glm::mat4> ComputeCropMatrices(int cascades,
                                             const glm::vec3 &lightDir,
                                             float shadowMapSize);

private:
  [[nodiscard]] glm::mat4 ComputeLightViewMatrix(const glm::vec3 &lightDir,
                                                 const glm::vec3 &center,
                                                 float offsetZ) const;
  [[nodiscard]] glm::mat4 ComputeCropMatrix(const glm::mat4 &view,
                                            const glm::mat4 &proj,
                                            float shadowMapSize) const;

  std::vector<Frustum> frustums_{};
};

#endif
