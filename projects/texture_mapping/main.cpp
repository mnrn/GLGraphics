/**
 * @brief  Texture Mapping
 * @date   2017/12/30
 */


// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "yryr.hpp"
#include "SceneTextureMapping.hpp"


// ********************************************************************************
// Entry point
// ********************************************************************************

int main(
#if true
void
#else
int argc, char** argv
#endif
) {

    yr::Yryr yryr("Texture Mapping");

    // Create scene
    std::unique_ptr<Scene::Base> pScene = std::make_unique<Scene::TextureMapping>();

    // Enter the main loop
    yryr.run([&pScene](){ 
        pScene->Update(0.0f);
        pScene->Render();
    });
    
    return 0;
}
