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

const size_t NUM_TEXTURES = 7;
const size_t NUM_MATERIALS = 4;
const size_t NUM_MESHES = 3;
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

        engine.m_materialManager->initialize(loadArgs, nameMap);
    }

    //configure mesh manager
    {
        std::map<std::string, illGraphics::MeshId> * nameMap = new std::map<std::string, illGraphics::MeshId>();
        illGraphics::MeshLoadArgs * loadArgs = new illGraphics::MeshLoadArgs[NUM_MESHES];
        
        illGraphics::TextureId currRes = 0;

        (*nameMap)["Marine"] = currRes;
        loadArgs[currRes].m_path = "meshes/Marine/marine8.illmesh";
        ++currRes;

        (*nameMap)["MarineHelmet"] = currRes;
        loadArgs[currRes].m_path = "meshes/Marine/marine.illmesh";
        ++currRes;

        (*nameMap)["Wall"] = currRes;
        loadArgs[currRes].m_path = "meshes/Wall/wall.illmesh";
        ++currRes;

        engine.m_meshManager->initialize(loadArgs, nameMap);
    }

    //TODO: the rest
}