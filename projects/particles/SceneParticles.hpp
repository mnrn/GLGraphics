/**
 * @brief 
 */


#ifndef SCENE_PARTICLES_HPP
#define SCENE_PARTICLES_HPP


// ********************************************************************************
// Include files
// ********************************************************************************

#include "SceneBase.hpp"
#include "ShaderProgram.hpp"


// ********************************************************************************
// Namespace
// ********************************************************************************

namespace Scene {


// ********************************************************************************
// Class
// ********************************************************************************

class Particles : public Base {
public:
    Particles();
    ~Particles() override = default;

    void Update(float t) override;
    void Render() const override;
private:
    void InitBuffer();
    bool CompileAndLinkShader();
    
    GLuint hParticlesVAO_ = 0;
    GLuint hBlackHoleVAO_ = 0;
    GLuint hBlackHoleBuffer_ = 0;

    Shader::Program renderProgram_;
    Shader::Program computeProgram_;

    glm::vec4 blackHole1Pos_{ 5.0f,  0.0f, 0.0f, 1.0f };
    glm::vec4 blackHole2Pos_{ -5.0f, 0.0f, 0.0f, 1.0f };

    float angle_ = 0.0f;

    static constexpr int32_t width_ = 1280;
    static constexpr int32_t height_ = 720;
    static constexpr int32_t particlesXNum_ = 100;
    static constexpr int32_t particlesYNum_ = 100;
    static constexpr int32_t particlesZNum_ = 100;
    static constexpr int32_t totalParticlesNum_ = particlesXNum_ * particlesYNum_ * particlesZNum_;
    static constexpr int32_t localSizeX_ = 1000;  //!< per work group in the x dimension.
};


}  // end of namespace scene


#endif  // end of ifndef SCENE_PARTICLES_HPP
