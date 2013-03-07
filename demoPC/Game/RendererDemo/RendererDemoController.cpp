#include <set>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Engine.h"
#include "illEngine/Graphics/Window.h"

#include "RendererDemoController.h"
#include "illEngine/Graphics/serial/Model/Mesh.h"
#include "illEngine/Graphics/serial/Material/Material.h"
#include "illEngine/Input/serial/InputManager.h"

#include "illEngine/DeferredShadingRenderer/serial/DeferredShadingScene.h"
#include "illEngine/DeferredShadingRenderer/serial/Gl3_3/DeferredShadingBackendGl3_3.h"

#include "illEngine/RendererCommon/serial/StaticMeshNode.h"

//TODO: for now I'm testing a bunch of stuff, normally all rendering is done through the renderer
#include <GL/glew.h>

namespace Demo {
    
RendererDemoController::RendererDemoController(Engine * engine)
    : GameControllerBase(),
    m_engine(engine)
{
    //This is all put together to test some stuff, this is in no way how to normally do these things.  Everything should normally be done through the renderer front end when that's done.
        
    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_cameraController.m_inputContext);

    m_cameraController.m_speed = 50.0f;
    m_cameraController.m_rollSpeed = 50.0f;

    m_rendererBackend = new illDeferredShadingRenderer::DeferredShadingBackendGl3_3((GlCommon::GlBackend *)m_engine->m_graphicsBackend);

	m_graphicsScene = new illDeferredShadingRenderer::DeferredShadingScene(static_cast<illDeferredShadingRenderer::DeferredShadingBackend *> (m_rendererBackend),
        m_engine->m_meshManager, m_engine->m_materialManager,
        glm::vec3(200.0f), glm::uvec3(20), 
        glm::vec3(50.0f), glm::uvec3(80));

	//for now place a bunch of random lights and meshes
    /*new illRendererCommon::StaticMeshNode(m_graphicsScene, 
        m_engine->m_meshManager->getIdForName("Marine"), m_engine->m_materialManager->getIdForName("MarineSkin"),
        glm::mat4(), Box<>(glm::vec3(-100.0f), glm::vec3(100.0f)));*/
}

RendererDemoController::~RendererDemoController() {
    m_engine->m_inputManager->getInputContextStack(0)->popInputContext();

	delete m_graphicsScene;
    delete m_rendererBackend;
}

void RendererDemoController::update(float seconds) {
    m_cameraController.update(seconds);        
}

void RendererDemoController::updateSound(float seconds) {

}
 
void RendererDemoController::render() {
    m_camera.setPerspectiveTransform(m_cameraController.m_transform, 
        m_engine->m_window->getAspectRatio(), 
        illGraphics::DEFAULT_FOV * m_cameraController.m_zoom);

    m_graphicsScene->render(m_camera);
}

}
