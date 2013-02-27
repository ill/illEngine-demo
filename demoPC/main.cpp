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

#include "illEngine/GlCommon/serial/GlRenderer.h"

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

GlCommon::GlRenderer rendererBackend;

Demo::Engine engine;

int main(int argc, char * argv[]) {
    Console::initConsoleConsole(&consoleVariableManager);    
    illLogging::logger->addLogDestination(&developerConsole);

    LOGGER_BEGIN_CATCH

    //tests
	testGeomUtil();
    testVectorManager();
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
    engine.m_rendererBackend = &rendererBackend;

    illGraphics::ShaderManager shaderManager(engine.m_rendererBackend);

    illGraphics::ShaderProgramLoader shaderProgramLoader(engine.m_rendererBackend, &shaderManager);
    illGraphics::ShaderProgramManager shaderProgramManager(&shaderProgramLoader);
    illGraphics::TextureManager textureManager(engine.m_rendererBackend);
    //Graphics::MaterialManager materialProgramManager;

    engine.m_shaderManager = &shaderManager;
    engine.m_shaderProgramManager = &shaderProgramManager;
    engine.m_textureManager = &textureManager;
    
    //run game loop
    window.setRenderer(engine.m_rendererBackend);
    window.setInputManager(engine.m_inputManager);
    engine.m_window->initialize();

    Demo::FixedStepController loopController(new Demo::MainController(&engine), &engine);
    loopController.appLoop();

    //uninitialize things
    engine.m_window->uninitialize();
    
    LOGGER_END_CATCH(illLogging::logger)
        
    return 0;
}
