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
class GraphicsBackend;

class Material;
struct MaterialLoadArgs;
typedef uint32_t MaterialId;
typedef ConfigurableResourceManager<MaterialId, Material, MaterialLoadArgs, GraphicsBackend> MaterialManager;

class Shader;
typedef uint64_t ShaderId;
typedef ResourceManager<ShaderId, Shader, GraphicsBackend> ShaderManager;

class ShaderProgram;
struct ShaderProgramLoader;
typedef uint64_t ShaderProgramId;
typedef ResourceManager<ShaderProgramId, ShaderProgram, ShaderProgramLoader> ShaderProgramManager;

class Texture;
struct TextureLoadArgs;
typedef uint32_t TextureId;
typedef ConfigurableResourceManager<TextureId, Texture, TextureLoadArgs, GraphicsBackend> TextureManager;

class AnimSet;
struct AnimSetLoadArgs;
typedef uint32_t AnimSetId;
typedef ConfigurableResourceManager<AnimSetId, AnimSet, AnimSetLoadArgs, GraphicsBackend> AnimSetManager;

class Mesh;
struct MeshLoadArgs;
typedef uint32_t MeshId;
typedef ConfigurableResourceManager<MeshId, Mesh, MeshLoadArgs, GraphicsBackend> MeshManager;

class Skeleton;
struct SkeletonLoadArgs;
typedef uint32_t SkeletonId;
typedef ConfigurableResourceManager<SkeletonId, Skeleton, SkeletonLoadArgs, GraphicsBackend> SkeletonManager;

class SkeletonAnimation;
struct SkeletonAnimationLoadArgs;
typedef uint32_t SkeletonAnimationId;
typedef ConfigurableResourceManager<SkeletonAnimationId, SkeletonAnimation, SkeletonAnimationLoadArgs, GraphicsBackend> SkeletonAnimationManager;
}

namespace illInput {
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
    illGraphics::GraphicsBackend * m_graphicsBackend;

    illGraphics::MaterialManager * m_materialManager;
    illGraphics::ShaderManager * m_shaderManager;
    illGraphics::ShaderProgramManager * m_shaderProgramManager;
    illGraphics::TextureManager * m_textureManager;
    
    illGraphics::AnimSetManager * m_animSetManager;
    illGraphics::MeshManager * m_meshManager;
    illGraphics::SkeletonManager * m_skeletonManager;
    illGraphics::SkeletonAnimationManager * m_skeletonAnimationManager;

    illInput::InputManager * m_inputManager;
};

}

#endif