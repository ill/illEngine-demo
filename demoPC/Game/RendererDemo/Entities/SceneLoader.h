#ifndef ILL_SCENE_LOADER_H_
#define ILL_SCENE_LOADER_H_

namespace illRendererCommon {
class GraphicsScene;
class RendererBackend;
}

namespace Demo {
namespace Renderer {

//TODO: haven't quite figured out how to tie this in yet, just put it in the engine demo for now
class SceneLoader {
public:
    SceneLoader(const char * path, illRendererCommon::GraphicsScene * graphicsScene, illRendererCommon::RendererBackend * rendererBackend);
    ~SceneLoader();

protected:
    illRendererCommon::GraphicsScene * m_graphicsScene;
    illRendererCommon::RendererBackend * m_rendererBackend;
};

}
}

#endif