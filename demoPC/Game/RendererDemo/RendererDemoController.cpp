#include <set>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Engine.h"
#include "illEngine/Graphics/Window.h"

#include "RendererDemoController.h"
#include "illEngine/Util/Illmesh/IllmeshLoader.h"
#include "illEngine/Graphics/serial/Material/Shader.h"
#include "illEngine/Graphics/serial/BitmapFont.h"
#include "illEngine/Input/serial/InputManager.h"

#include "illEngine/GlCommon/glLogging.h"
#include "illEngine/GlCommon/serial/glUtil.h"

#include "illEngine/DeferredShadingRenderer/serial/DeferredShadingScene.h"
#include "illEngine/DeferredShadingRenderer/serial/Gl3_3/DeferredShadingBackendGl3_3.h"

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

	m_graphicsScene = new illDeferredShadingRenderer::DeferredShadingScene(m_rendererBackend, glm::vec3(50.0f), glm::uvec3(1000), 
        glm::vec3(10.0f), glm::uvec3(10000));

	//for now place a bunch of random lights and meshes
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
}

}
