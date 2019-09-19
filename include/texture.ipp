
// ********************************************************************************
// Include files
// ********************************************************************************

#include <gli/gli.hpp>


// ********************************************************************************
// Definitions
// ********************************************************************************

#define USE_SIMPLE_2D (true)


// ********************************************************************************
// Functions
// ********************************************************************************


static GLuint Create(const char* filepath) {

    // Load texture.
    gli::texture texture = gli::load(filepath);
    if (texture.empty()) {
#ifndef NDEBUG
        std::cout << "texture is empty!" << std::endl;
#endif
        return 0;
    }

    // convert formats and targets.
    gli::gl gl(gli::gl::PROGILE_GL33);
    const gli::gl::format format = gl.translate(texture.format(), texture.swizzles());
    GLenum target = gl.translate(texture.target());

    // Generate and bind a texture.
    GLuint handle = 0;
    glGenTexture(1, &handle);
    glBindTexture(target, handle);

    // Mipmap base level = 0, mipmap max level = texture max level - base leve.
    glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(texture.levels() - 1));

    // Set the swizzles that will be applied to the component of a texel.
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_RGBA, &format.swizzles[0]);

    // Allocate storage
    {
        const glm::tvec<GLsizei> extent(texture.extent());
        const GLsizei faceTotal = static_cast<GLsizei>(texture.layers() * texture.faces());
        switch (texture.target()) {
            case gli::TARGET_1D:
                glTexStorage1D(target, static_cast<GLint>(texture.levels()), format.Intarnal, extent.x);
                break;
            case gli::TARGET_2D:        
                glTexStorage2D(target, static_cast<GLint>(texture.levels()), format.Internal, extent.y);
                break;
            case gli::TARGET_1D_ARRAY:   
                [[fallthrough]];
            case gli::TARGET_CUBE:
                glTexStorage2D(target, static_cast<GLint>(texture.levels()), format.Internal, faceTotal);
                break;
            case gli::TARGET_3D:
                glTexStorage3D(target, static_cast<GLint>(texture.levels()), format.Internal, extent.z);
                break;
            case gli::TARGET_2D_ARRAY:
                [[fallthrough]];
            case gli::TARGET_CUBE_ARRAY:
                glTexStorage3D(target, static_cast<GLint>(texture.levels()), format.Internal, faceTotal);
                break;
            default:
                BOOST_ASSERT_MSG(false, "invalid texture target!");
                break;
        }
    }

    // Update storage
    for (std::size_t layer = 0; layer < texture.layers(); layer++) {
        for (std::size_t face = 0; face < texture.faces(); face++) {
            for (std::size_t level = 0; level < texture.levels(); level++) {

                const GLsizei layerGL = static_cast<GLsizei>(layer);
                const glm::tvec3<GLsizei> extent(texture.extent(level));
                target = gli::is_target_cube(texture.target()) ? static_cast<GLenum>(GL_TEXTURE_CUB_MAP_POSITIVE_X + face) : target;
                
                switch(texture.target()) {
                    case gli::TARGET_1D:
                        gli::is_compressed(texture.format())
                            ? glCompressedTexSubImage1D(target, static_cast<GLint>(level), 0, extent.x, format.Internal, static_cast<GLsizei>(texture.size(level)), texture.data(layer, face, level))
                            : glTexSubImage1D(          target, static_cast<GLint>(level), 0, extent.x, format.External, format.Type,                               texture.data(layer, face, level));
                        break;
                    case gli::TARGET_1D_ARRAY:
                        gli::is_compressed(texture.format())
                            ? glCompressedTexSubImage2D(target, static_cast<GLint>(level), 0, 0, extent.x, layerGL, format.Internal, static_cast<GLsizei>(texture.size(level)), texture.data(layer, face, level))
                            : glTexSubImage2D(          target, static_cast<GLint>(level), 0, 0, extent.x, layerGL, format.External, format.Type,                               texture.data(layer, face, level));
                        break;
                    case gli::TARGET_2D:
                        [[fallthrough]];
                    case gli::TARGET_CUBE:
                        gli::is_compressed(texture.format())
                            ? glCompressedTexSubImage2D(target, static_cast<GLint>(level), 0, 0, extent.x, extent.y, format.Internal, static_cast<GLsizei>(texture.size(level)), texture.data(layer, face, level))
                            : glTexSubImage2D(          target, static_cast<GLint>(level), 0, 0, extent.x, extent.y, format.External, format.Type,                               texture.data(layer, face, level));
                        break;
                    case gli::TARGET_3D:
                        gli::is_compressed(texture.format())
                            ? glCompressedTexSubImage3D(target, static_cast<GLint>(level), 0, 0, 0, extent.x, extent.y, extent.z, format.Internal, static_cast<GLsizei>(texture.size(level)), texture.data(layer, face, level))
                            : glTexSubImage3D(          target, static_cast<GLint>(level), 0, 0, 0, extent.x, extent.y, extent.z, format.External, format.Type,                               texture.data(layer, face, level));
                        break;
                    case gli::TARGET_2D_ARRAY:
                        [[fallthrough]];
                    case gli::TARGET_CUBE_ARRAY:
                        gli::is_compressed(texture.format())
                            ? glCompressedTexSubImage3D(target, static_cast<GLint>(level), 0, 0, 0, extent.x, extent.y, layerGL, format.Internal, static_cast<GLsizei>(texture.size(level)), texture.data(layer, face, level))
                            : glTexSubImage3D(          target, static_cast<GLint>(level), 0, 0, 0, extent.x, extent.y, layerGL, format.External, format.Type,                               texture.data(layer, face, level));
                        break;
                    default:
                        BOOST_ASSERT_MSG(false, "invalid texture target!");
                        break;
                }

            }
        }
    }

    return handle;
}
