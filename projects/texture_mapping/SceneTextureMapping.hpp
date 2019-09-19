/**
 * @brief Scene Texture Mapping
 * @date 2017/12/30
 */


#ifndef SCENE_TEXTURE_MAPPING_HPP
#define SCENE_TEXTURE_MAPPING_HPP


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

/**
 * @brief Scene Texture Mapping Class
 */
class TextureMapping : public Base {
public:
    TextureMapping();
    ~TextureMapping() override = default;

    void Update(float d) override;
    void Render() const override;
private:
    bool CompileAndLinkShader();

    void CreateTextures();

private:
    GLuint m_hTexture = 0;
    shader::Program m_Program;

}; // end of class TextureMapping


}  // end of namespace Scene


#endif  // end of ifndef SCENE_TEXTURE_MAPPING_HPP
