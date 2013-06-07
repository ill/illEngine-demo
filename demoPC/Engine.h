#ifndef ILL_ENGINE_H_
#define ILL_ENGINE_H_

#include <stdint.h>

namespace illConsole {
class DeveloperConsole;
}

template <typename Key, typename T, typename Loader> class ResourceManager;
template<typename Id, typename T, typename LoadArgs, typename Loader> class ConfigurableResourceManager;

namespace illGraphics {
class Window;
class GraphicsBackend;

class Material;
struct MaterialLoader;
struct MaterialLoadArgs;
typedef uint32_t MaterialId;
typedef ConfigurableResourceManager<MaterialId, Material, MaterialLoadArgs, MaterialLoader> MaterialManager;

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

struct CrappyBmFontRenderer;

namespace Demo {

class FixedStepController;

/**
TODO: this is generally a good struct for any simple game with a single window so put this in util sometime or something
*/
struct Engine {
public:
    Engine()
        : m_showingFps(false),
        m_showingRendererPerf(false)
    {}

    //TODO: think of a cleverer way to protect these with getters, or not...  What idiot programmer would ever use these unsafely anyway?  (hint, hint... don't try to change these, just use them)
    //just don't mess with these, do you really want me to go in and write getters when I can just leave them public?
    //I have to set them from main() somehow anyway and I'd have to do PIMPL or make the constructor take them or something.  meh...
    illConsole::DeveloperConsole * m_developerConsole;

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

    CrappyBmFontRenderer * m_crappyFontRenderer;
    FixedStepController * m_gameController;
    
    //some debugging vars, TODO: think of a cleverer way to do this later
    bool m_showingFps;
    bool m_showingRendererPerf;
};

}

#endif