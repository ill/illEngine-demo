/**
This file needs to be included in the same file that implements the main method.
*/

#include <SDL.h>

#include "illEngine/Logging/serial/SerialLogger.h"
#include "illEngine/FileSystem-Physfs/PhysFsFileSystem.h"

#include "Engine.h"

#include "illEngine/Pc/serial/SdlWindow.h"
#include "illEngine/Input/serial/InputManager.h"

#include "illEngine/Console/serial/consoleConsole.h"
#include "illEngine/Console/serial/DeveloperConsole.h"
#include "illEngine/Console/serial/VariableManager.h"

#include "illEngine/Graphics/serial/Material/Shader.h"
#include "illEngine/Graphics/serial/Material/ShaderProgram.h"
#include "illEngine/Graphics/serial/Material/Texture.h"
#include "illEngine/Graphics/serial/Material/Material.h"

#include "illEngine/Graphics/serial/Model/Animset.h"
#include "illEngine/Graphics/serial/Model/Mesh.h"
#include "illEngine/Graphics/serial/Model/Skeleton.h"
#include "illEngine/Graphics/serial/Model/SkeletonAnimation.h"

#include "illEngine/GlCommon/serial/GlBackend.h"

#include "FixedStepController.h"
#include "Game/MainController.h"

//tests
#include "Tests/tests.h"

//statically create all the objects, this way there will be no virtual table overhead from the interfaces
illLogging::SerialLogger thisLogger;
illLogging::Logger * illLogging::logger = &thisLogger;

illPhysFs::PhysFsFileSystem thisFileSystem;
illFileSystem::FileSystem * illFileSystem::fileSystem = &thisFileSystem;

Console::VariableManager consoleVariableManager;
Console::DeveloperConsole developerConsole;

illInput::InputManager inputManager;

SdlPc::SdlWindow window;

GlCommon::GlBackend graphicsBackend;

Demo::Engine engine;

/**
I'm still up in the air if I want to do XML again for resource configuration.
Maybe I'll use some kind of engine tools instead and the file will be a binary blob.
Maybe I'll even use Lua.
For now this will be hardcoded, ugh...
*/
inline void configureResourceManagers();

int main(int argc, char * argv[]) {
    Console::initConsoleConsole(&consoleVariableManager);    
    illLogging::logger->addLogDestination(&developerConsole);

    LOGGER_BEGIN_CATCH

    //tests
	testGeomUtil();
    testPool();
    testEndian();
    testSortDimensions();

    glm::mat4 trollLOL = glm::rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f)); 

    thisFileSystem.init(argv[0]);

    //TODO: make the game moddable and able to load the archives from the mod folder
    //TODO: set up creating of the game archive as part of the build
    illFileSystem::fileSystem->addPath("..\\..\\..\\assets");

    engine.m_window = &window;
    engine.m_developerConsole = &developerConsole;
    engine.m_consoleVariableManager = &consoleVariableManager;
    engine.m_inputManager = &inputManager;
    engine.m_graphicsBackend = &graphicsBackend;

    //resource managers
    illGraphics::ShaderManager shaderManager(engine.m_graphicsBackend);

    illGraphics::ShaderProgramLoader shaderProgramLoader(engine.m_graphicsBackend, &shaderManager);
    illGraphics::ShaderProgramManager shaderProgramManager(&shaderProgramLoader);

    illGraphics::TextureManager textureManager(engine.m_graphicsBackend);

    illGraphics::MaterialLoader materialLoader(&shaderProgramManager, &textureManager);
    illGraphics::MaterialManager materialManager(&materialLoader);

    illGraphics::AnimSetManager animSetManager(engine.m_graphicsBackend);
    illGraphics::SkeletonManager skeletonManager(engine.m_graphicsBackend);
    illGraphics::SkeletonAnimationManager skeletonAnimationManager(engine.m_graphicsBackend);
    illGraphics::MeshManager meshManager(engine.m_graphicsBackend);

    engine.m_shaderManager = &shaderManager;
    engine.m_shaderProgramManager = &shaderProgramManager;
    engine.m_textureManager = &textureManager;
    engine.m_materialManager = &materialManager;

    engine.m_animSetManager = &animSetManager;
    engine.m_skeletonManager = &skeletonManager;
    engine.m_skeletonAnimationManager = &skeletonAnimationManager;
    engine.m_meshManager = &meshManager;

    configureResourceManagers();
    
    window.setBackend(engine.m_graphicsBackend);
    window.setInputManager(engine.m_inputManager);
    engine.m_window->initialize();

    //run game loop
    Demo::FixedStepController loopController(new Demo::MainController(&engine), &engine);
    loopController.appLoop();

    //uninitialize things
    engine.m_window->uninitialize();
    
    LOGGER_END_CATCH(illLogging::logger)
        
    return 0;
}

const size_t NUM_TEXTURES = 49;
const size_t NUM_MATERIALS = 23;
const size_t NUM_MESHES = 41;
const size_t NUM_ANIMSET = 1;
const size_t NUM_SKELETONS = 1;
const size_t NUM_ANIMATIONS = 4;

void configureResourceManagers() {
    //configure texture manager
    {   
        std::map<std::string, illGraphics::TextureId> * nameMap = new std::map<std::string, illGraphics::TextureId>();
        illGraphics::TextureLoadArgs * loadArgs = new illGraphics::TextureLoadArgs[NUM_TEXTURES];

        illGraphics::TextureId currRes = 0;

        (*nameMap)["MarineDiffuse"] = currRes;
        loadArgs[currRes].m_path = "meshes/Marine/marine.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        ++currRes;

        (*nameMap)["MarineNormal"] = currRes;
        loadArgs[currRes].m_path = "meshes/Marine/marine_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        ++currRes;

        (*nameMap)["MarineSpecular"] = currRes;
        loadArgs[currRes].m_path = "meshes/Marine/marine_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        ++currRes;

        (*nameMap)["MarineHelmetDiffuse"] = currRes;
        loadArgs[currRes].m_path = "meshes/Marine/helmet.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        ++currRes;

        (*nameMap)["MarineHelmetNormal"] = currRes;
        loadArgs[currRes].m_path = "meshes/Marine/helmet_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        ++currRes;

        (*nameMap)["MarineHelmetSpecular"] = currRes;
        loadArgs[currRes].m_path = "meshes/Marine/helmet_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        ++currRes;

        (*nameMap)["GoobyPls"] = currRes;
        loadArgs[currRes].m_path = "meshes/Wall/goobyPls.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        ++currRes;

        (*nameMap)["bluetex3k2_d"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/bluetex3k2_d.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["bluetex3k2_s"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/bluetex3k2_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["bluetex3k2_local"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/bluetex3k2_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["sflgrate2a_d"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/sflgrate2a_d.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["sflgrate2_s"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/sflgrate2_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["sflgrate2_local"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/sflgrate2_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["sflgrate2adiag_d"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/sflgrate2adiag_d.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["sflgrate2diag_s"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/sflgrate2diag_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["sflgrate2diag_local"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/sflgrate2diag_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["conc_panel02_d"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/conc_panel02_d.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["mcityc7"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mcityc7.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["mcityc7kc_s"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mcityc7kc_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;        

        (*nameMap)["mcityc7_local"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mcityc7_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["mcityc7a"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mcityc7a.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["mcityc7akc_s"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mcityc7akc_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;        

        (*nameMap)["mcityc7a_local"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mcityc7a_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["bluetex3kdif"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/bluetex3kdif.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["bluetex3kspec"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/bluetex3kspec.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["bluetex3k_local"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/bluetex3k_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["mcityc27b"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mcityc27b.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["mcityc27b_s"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mcityc27b_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["mcityc27b_local"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mcityc27b_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["enwall5"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/enwall5.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["enwall5_s"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/enwall5_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["enwall5_local"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/enwall5_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["FighterBayBorderDiffuse"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/FighterBayBorderDiffuse.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["FighterBayBorderNormal"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/FighterBayBorderNormal.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["yelhaz2dif"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/yelhaz2dif.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["mchangar10"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mchangar10.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["mchangar10_s"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mchangar10_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["mchangar10_local"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/mchangar10_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["sofloor3_d"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/sofloor3_d.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["sofloor3_s"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/sofloor3_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["sofloor3_local"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/sofloor3_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["Yellow"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/Yellow.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["Gray"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/Gray.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["lanrock1_d"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/lanrock1_d.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["lanrock1_s"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/lanrock1_s.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["lanrock1_local"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/lanrock1_local.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["FighterTexture"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/FighterTexture.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["CeilingLightDiffuse"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/CeilingLightDiffuse.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        (*nameMap)["SpotLightDiffuse"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/SpotLightDiffuse.tga";
        loadArgs[currRes].m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        loadArgs[currRes].m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_REPEAT;
        ++currRes;

        engine.m_textureManager->initialize(loadArgs, nameMap);
    }

    //configure material manager
    {
        std::map<std::string, illGraphics::MaterialId> * nameMap = new std::map<std::string, illGraphics::MaterialId>();
        illGraphics::MaterialLoadArgs * loadArgs = new illGraphics::MaterialLoadArgs[NUM_MATERIALS];

        illGraphics::MaterialId currRes = 0;

        (*nameMap)["MarineSkin"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("MarineDiffuse");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("MarineSpecular");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("MarineNormal");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = true;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["MarineHelmetSkin"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("MarineHelmetDiffuse");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("MarineHelmetSpecular");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("MarineHelmetNormal");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = true;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["WallMaterial"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = -1;
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = -1;
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = -1;
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["GoobyPlsMaterial"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("GoobyPls");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = 6;
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = -1;
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["bluetex3k2"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("bluetex3k2_d");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("bluetex3k2_s");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f, 1.0f, 1.0f, 0.1f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("bluetex3k2_local");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["sflgrate2a"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("sflgrate2a_d");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("sflgrate2_s");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("sflgrate2_local");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["sflgrate2adiag"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("sflgrate2adiag_d");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("sflgrate2diag_s");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("sflgrate2diag_local");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["conc_panel02"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("conc_panel02_d");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = -1;
        loadArgs[currRes].m_specularBlend = glm::vec4(0.2f, 0.2f, 0.2f, 0.01f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = -1;
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["mcityc7"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("mcityc7");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("mcityc7kc_s");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("mcityc7_local");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["mcityc7a"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("mcityc7a");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("mcityc7akc_s");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("mcityc7a_local");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["bluetex3k"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("bluetex3kdif");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("bluetex3kspec");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("bluetex3k_local");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["mcityc27b"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("mcityc27b");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("mcityc27b_s");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("mcityc27b_local");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["enwall5"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("enwall5");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("enwall5_s");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("enwall5_local");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["FighterBayBorder"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("FighterBayBorderDiffuse");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = -1;
        loadArgs[currRes].m_specularBlend = glm::vec4(0.1f, 0.1f, 0.1f, 0.01f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("FighterBayBorderNormal");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["yelhaz2"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("yelhaz2dif");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = -1;
        loadArgs[currRes].m_specularBlend = glm::vec4(0.1f, 0.1f, 0.1f, 0.01f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = -1;
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["mchangar10"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("mchangar10");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("mchangar10_s");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("mchangar10_local");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["sofloor3"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("sofloor3_d");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("sofloor3_s");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("sofloor3_local");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["Yellow"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("Yellow");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = -1;
        loadArgs[currRes].m_specularBlend = glm::vec4(0.1f, 0.1f, 0.1f, 0.01f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = -1;
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["Gray"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("Gray");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = -1;
        loadArgs[currRes].m_specularBlend = glm::vec4(0.1f, 0.1f, 0.1f, 0.01f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = -1;
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["lanrock1"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("lanrock1_d");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = engine.m_textureManager->getIdForName("lanrock1_s");
        loadArgs[currRes].m_specularBlend = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = engine.m_textureManager->getIdForName("lanrock1_local");
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["FighterSkin"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("FighterTexture");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = -1;
        loadArgs[currRes].m_specularBlend = glm::vec4(0.1f, 0.1f, 0.1f, 0.01f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = -1;
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["CeilingLight"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("CeilingLightDiffuse");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = -1;
        loadArgs[currRes].m_specularBlend = glm::vec4(0.1f, 0.1f, 0.1f, 0.01f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = -1;
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        (*nameMap)["SpotLight"] = currRes;
        loadArgs[currRes].m_diffuseTextureIndex = engine.m_textureManager->getIdForName("SpotLightDiffuse");
        loadArgs[currRes].m_diffuseBlend = glm::vec4(1.0f);
        loadArgs[currRes].m_specularTextureIndex = -1;
        loadArgs[currRes].m_specularBlend = glm::vec4(0.1f, 0.1f, 0.1f, 0.01f);
        loadArgs[currRes].m_emissiveTextureIndex = -1;
        loadArgs[currRes].m_emissiveBlend = glm::vec4(0.0f);
        loadArgs[currRes].m_normalTextureIndex = -1;
        loadArgs[currRes].m_normalMultiplier = 1.0f;
        loadArgs[currRes].m_blendMode = illGraphics::MaterialLoadArgs::BlendMode::NONE;
        loadArgs[currRes].m_billboardMode = illGraphics::MaterialLoadArgs::BillboardMode::NONE;
        loadArgs[currRes].m_noLighting = false;
        loadArgs[currRes].m_skinning = false;
        loadArgs[currRes].m_forceForwardRendering = false;
        ++currRes;

        engine.m_materialManager->initialize(loadArgs, nameMap);
    }

    //configure mesh manager
    {
        std::map<std::string, illGraphics::MeshId> * nameMap = new std::map<std::string, illGraphics::MeshId>();
        illGraphics::MeshLoadArgs * loadArgs = new illGraphics::MeshLoadArgs[NUM_MESHES];
        
        illGraphics::MeshId currRes = 0;

        (*nameMap)["Marine"] = currRes;
        loadArgs[currRes].m_path = "meshes/Marine/marine8.illmesh";
        ++currRes;

        (*nameMap)["MarineHelmet"] = currRes;
        loadArgs[currRes].m_path = "meshes/Marine/marine.illmesh";
        ++currRes;

        (*nameMap)["Wall"] = currRes;
        loadArgs[currRes].m_path = "meshes/Wall/wall.illmesh";
        ++currRes;

        (*nameMap)["Beam"] = currRes;
        loadArgs[currRes].m_path = "meshes/TheGridMap/Beam.illmesh";
        ++currRes;

        (*nameMap)["DoorBorderArch"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/DoorBorderArch.illmesh";
        ++currRes;

        (*nameMap)["DoorBorderInner"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/DoorBorderInner.illmesh";
        ++currRes;

        (*nameMap)["DoorBorderOuter"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/DoorBorderOuter.illmesh";
        ++currRes;

        (*nameMap)["DoorA"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/DoorA.illmesh";
        ++currRes;

        (*nameMap)["DoorB"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/DoorB.illmesh";
        ++currRes;

        (*nameMap)["Hall"] = currRes;
        loadArgs[currRes].m_path = "meshes/TheGridMap/Hall.illmesh";
        ++currRes;

        (*nameMap)["InnerSanctum"] = currRes;
        loadArgs[currRes].m_path = "meshes/TheGridMap/InnerSanctum.illmesh";
        ++currRes;

        (*nameMap)["LargeSupport"] = currRes;
        loadArgs[currRes].m_path = "meshes/TheGridMap/LargeSupport.illmesh";
        ++currRes;

        (*nameMap)["OuterHall"] = currRes;
        loadArgs[currRes].m_path = "meshes/TheGridMap/OuterHall.illmesh";
        ++currRes;

        (*nameMap)["OuterHallCorner"] = currRes;
        loadArgs[currRes].m_path = "meshes/TheGridMap/OuterHallCorner.illmesh";
        ++currRes;

        (*nameMap)["Shaft"] = currRes;
        loadArgs[currRes].m_path = "meshes/TheGridMap/Shaft.illmesh";
        ++currRes;

        (*nameMap)["ShaftCover"] = currRes;
        loadArgs[currRes].m_path = "meshes/TheGridMap/ShaftCover.illmesh";
        ++currRes;

        (*nameMap)["SideRoom"] = currRes;
        loadArgs[currRes].m_path = "meshes/TheGridMap/SideRoom.illmesh";
        ++currRes;

        (*nameMap)["SmallSupport"] = currRes;
        loadArgs[currRes].m_path = "meshes/TheGridMap/SmallSupport.illmesh";
        ++currRes;

        (*nameMap)["HangarHallA"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarHallA.illmesh";
        ++currRes;

        (*nameMap)["HangarHallB"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarHallB.illmesh";
        ++currRes;

        (*nameMap)["HangarHallEnd"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarHallEnd.illmesh";
        ++currRes;

        (*nameMap)["HangarHallCorner"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarHallCorner.illmesh";
        ++currRes;

        (*nameMap)["FighterBay"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/FighterBay.illmesh";
        ++currRes;

        (*nameMap)["FighterBayBorder"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/FighterBayBorder.illmesh";
        ++currRes;

        (*nameMap)["HangarCommandFloor"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarCommandFloor.illmesh";
        ++currRes;

        (*nameMap)["HangarCommandFloor2"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarCommandFloor2.illmesh";
        ++currRes;

        (*nameMap)["HangarFloor"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarFloor.illmesh";
        ++currRes;

        (*nameMap)["HangarOpeningBorder"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarOpeningBorder.illmesh";
        ++currRes;

        (*nameMap)["HangarRoofTrim"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarRoofTrim.illmesh";
        ++currRes;

        (*nameMap)["HangarOpeningWall"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarOpeningWall.illmesh";
        ++currRes;

        (*nameMap)["HangarTunnel"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarTunnel.illmesh";
        ++currRes;

        (*nameMap)["HangarCommandWindowCorner"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarCommandWindowCorner.illmesh";
        ++currRes;

        (*nameMap)["HangarCommandWindow"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarCommandWindow.illmesh";
        ++currRes;

        (*nameMap)["HangarTunnelElbow"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarTunnelElbow.illmesh";
        ++currRes;

        (*nameMap)["HangarCeiling"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarCeiling.illmesh";
        ++currRes;

        (*nameMap)["HangarCeilingCorner"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/HangarCeilingCorner.illmesh";
        ++currRes;

        (*nameMap)["FighterClamp"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/FighterClamp.illmesh";
        ++currRes;

        (*nameMap)["Fighter"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/Fighter.illmesh";
        ++currRes;

        (*nameMap)["CeilingLight"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/CeilingLight.illmesh";
        ++currRes;

        (*nameMap)["SpotLight"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/SpotLight.illmesh";
        ++currRes;

        (*nameMap)["SpotlightMount"] = currRes;
        loadArgs[currRes].m_path = "meshes/Hangar/SpotlightMount.illmesh";
        ++currRes;

        engine.m_meshManager->initialize(loadArgs, nameMap);
    }

    //TODO: the rest
}