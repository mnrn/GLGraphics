/**
 * @brief  Render Particles
 * @date   2017/08/27
 */


// ********************************************************************************
// Include files
// ********************************************************************************

#include <memory>

#include "yryr.hpp"
#include "SceneParticles.hpp"


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

    yr::Yryr yryr("Compute Particles");

    // Create scene
    std::unique_ptr<Scene::Base> pScene = std::make_unique<Scene::Particles>();

    // Enter the main loop
    yryr.run([&pScene](){ 
        pScene->Update(0.0f);
        pScene->Render();
    });
    
    return 0;
}
