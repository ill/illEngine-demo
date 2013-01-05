#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <stdint.h>

namespace Console {
class DeveloperConsole;
class VariableManager;
}

template <typename Key, typename T, typename Loader> class ResourceManager;
template<typename Id, typename T, typename LoadArgs, typename Loader> class ConfigurableResourceManager;

namespace illGraphics {
class Window;
class RendererBackend;

class Shader;
typedef ResourceManager<uint64_t, Shader, RendererBackend> ShaderManager;

class ShaderProgram;
struct ShaderProgramLoader;
typedef ResourceManager<uint64_t, ShaderProgram, ShaderProgramLoader> ShaderProgramManager;

class Texture;
struct TextureLoadArgs;
typedef ConfigurableResourceManager<uint32_t, Texture, TextureLoadArgs, RendererBackend> TextureManager;

class Material;
class MaterialManager;

//class Mesh;
//class MeshManager;
}

namespace Input {
class InputManager;
}

namespace Demo {

/**
TODO: this is generally a good struct for any simple game with a single window so put this in util sometime or something
*/
struct Engine {
public:
    Console::DeveloperConsole * m_developerConsole;
    Console::VariableManager * m_consoleVariableManager;

    illGraphics::Window * m_window;
    illGraphics::RendererBackend * m_rendererBackend;

    illGraphics::ShaderManager * m_shaderManager;
    illGraphics::ShaderProgramManager * m_shaderProgramManager;
    illGraphics::TextureManager * m_textureManager;
    //illGraphics::MaterialManager * m_materialProgramManager;
    //illGraphics::MeshManager * m_meshManager;

    Input::InputManager * m_inputManager;
};

}

#endif