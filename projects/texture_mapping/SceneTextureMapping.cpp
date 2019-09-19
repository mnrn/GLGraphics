/**
 * @brief Scene Texture Mapping Class
 */



// ********************************************************************************
// Include files
// ********************************************************************************

#include <iostream>
#include <boost/assert.hpp>

#include "SceneTextureMapping.hpp"



// ********************************************************************************
// Namespace
// ********************************************************************************

namespace Scene {


// ********************************************************************************
// Special member functions
// ********************************************************************************

TextureMapping::TextureMapping()
{
    if (CompileAndLinkShader() == false) {
        BOOST_ASSERT_MSG(false, "failed to compile or link!");
        std::exit(EXIT_FAILURE);
    }

    CreateTextures();
}


// ********************************************************************************
// Override functions
// ********************************************************************************

void TextureMapping::Update(float d)
{
    static_cast<void>(d);
}


void TextureMapping::Render() const
{

}


// ********************************************************************************
// Shader
// ********************************************************************************

bool TextureMapping::CompileAndLinkShader()
{
    if (m_Program.Compile("./data/shaders/texture_mapping/texture_mapping.vert", shader::Type::Vertex)  == false) {
        std::cerr << "vertex shader failed to compile." << std::endl;
        return false;
    }

    if (m_Program.Compile("./data/shaders/texture_mapping/texture_mapping.frag", shader::Type::Fragment) == false) {
        std::cerr << "fragment shader failed to compile." << std::endl;
        return false;
    }

    if (m_Program.Link() == false) {
        std::cerr << "shader program failed to link." << std::endl;
        std::cerr << m_Program.GetLog() << std::endl;
        return false;
    }

    m_Program.Use();
    return true;
}



}  // end of namespace scene
