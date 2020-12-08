#pragma once

#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class UniformDistribution {
public:
  /** 円周上の一様乱数 */
  template<typename RNG> glm::vec2 OnCircle(RNG& gen) {
    const float u = dist01_(gen);
    const float theta = glm::two_pi<float>() * u;
    const float x = glm::cos(theta);
    const float y = glm::sin(theta);
    return glm::vec2(x, y);
  }

  /** 円周内の一様乱数 */
  template <typename RNG> glm::vec2 InCircle(RNG& gen) {
    const float v = dist01_(gen);
    const float r = glm::sqrt(v);
    return r * OnCircle(gen);
  }

  /** 半球面上の一様乱数 */
  template <typename RNG> glm::vec3 OnHemisphere(RNG& gen) {
    const float v = dist01_(gen);
    const float r = glm::sqrt(1.0f - v * v);

    const float u = dist01_(gen);
    const float theta = glm::two_pi<float>() * u;
    const float x = glm::cos(theta);
    const float y = glm::sin(theta);
    return glm::vec3(r * x, r * y, u);
  }

  /**
   * @brief SSAO用の球体内乱数を生成します。
   * @note 点群の密度を均一にせず、密度が距離の二乗に反比例するように近似して分布させます。
   */
  // template <typename RNG> glm::vec3 InSphereForSSAO(RNG& gen) {
  //   const float v = dist_11_(gen);
  //   const float r = glm::sqrt(1.0f - v * v);
  //   const float w = dist01_(gen);
  //   return glm::vec3(w * r * OnCircle(gen), w);
  // }

private:
  std::uniform_real_distribution<float> dist01_{0.0f, 1.0f};
  // std::uniform_real_distribution<float> dist_11_{-1.0f, 1.0f};
};