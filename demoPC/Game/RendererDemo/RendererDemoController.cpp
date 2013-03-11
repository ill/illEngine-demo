#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
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
#include "illEngine/RendererCommon/serial/LightNode.h"

//TODO: for now I'm testing a bunch of stuff, normally all rendering is done through the renderer
#include <GL/glew.h>

namespace Demo {

void RendererDemoController::ChangeDebugMode::onRelease() {
    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_controller->m_rendererBackend)->m_debugMode = 
        static_cast<illDeferredShadingRenderer::DeferredShadingBackend::DebugMode>(m_mode);
}

RendererDemoController::RendererDemoController(Engine * engine)
    : GameControllerBase(),
    m_engine(engine)
{        
    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_cameraController.m_inputContext);

    m_cameraController.m_speed = 50.0f;
    m_cameraController.m_rollSpeed = 50.0f;

    //set up inputs
    m_noneDebugMode.m_controller = this;
    m_noneDebugMode.m_mode = static_cast<int>(illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::NONE);

    m_lightPosDebugMode.m_controller = this;
    m_lightPosDebugMode.m_mode = static_cast<int>(illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::LIGHT_POS);

    m_wireDebugMode.m_controller = this;
    m_wireDebugMode.m_mode = static_cast<int>(illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::WIRE);

    m_solidDebugMode.m_controller = this;
    m_solidDebugMode.m_mode = static_cast<int>(illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::SOLID);

    m_depthDebugMode.m_controller = this;
    m_depthDebugMode.m_mode = static_cast<int>(illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::DEPTH);

    m_normalDebugMode.m_controller = this;
    m_normalDebugMode.m_mode = static_cast<int>(illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::NORMAL);

    m_diffuseDebugMode.m_controller = this;
    m_diffuseDebugMode.m_mode = static_cast<int>(illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::DIFFUSE);

    m_specularDebugMode.m_controller = this;
    m_specularDebugMode.m_mode = static_cast<int>(illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::SPECULAR);

    m_diffuseAccumulationDebugMode.m_controller = this;
    m_diffuseAccumulationDebugMode.m_mode = static_cast<int>(illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::DIFFUSE_ACCUMULATION);

    m_specularAccumulationDebugMode.m_controller = this;
    m_specularAccumulationDebugMode.m_mode = static_cast<int>(illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::SPECULAR_ACCUMULATION);

    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_1), &m_noneDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_2), &m_lightPosDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_3), &m_wireDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_4), &m_solidDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_5), &m_depthDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_6), &m_normalDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_7), &m_diffuseDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_8), &m_specularDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_9), &m_diffuseAccumulationDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_0), &m_specularAccumulationDebugMode);

    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_inputContext);

    //setup renderer
    m_rendererBackend = new illDeferredShadingRenderer::DeferredShadingBackendGl3_3((GlCommon::GlBackend *)m_engine->m_graphicsBackend);

	m_graphicsScene = new illDeferredShadingRenderer::DeferredShadingScene(static_cast<illDeferredShadingRenderer::DeferredShadingBackend *> (m_rendererBackend),
        m_engine->m_meshManager, m_engine->m_materialManager,
        glm::vec3(200.0f), glm::uvec3(5), 
        glm::vec3(200.0f), glm::uvec3(5));
        
        /*glm::vec3(200.0f), glm::uvec3(10), 
        glm::vec3(50.0f), glm::uvec3(40));*/

    m_rendererBackend->initialize(m_engine->m_window->getResolution());
    
	//for now place a bunch of random lights and meshes
    for(unsigned int mesh = 0; mesh < 1000; mesh++) {
        glm::vec3 pos = glm::linearRand(glm::vec3(0.0f), glm::vec3(1000.0f));

        {
            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
                m_engine->m_meshManager->getIdForName("Marine"), m_engine->m_materialManager->getIdForName("MarineSkin"),
                glm::translate(pos), Box<>(glm::vec3(-100.0f), glm::vec3(100.0f)));

            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
        }

        {
            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
                m_engine->m_meshManager->getIdForName("MarineHelmet"), m_engine->m_materialManager->getIdForName("MarineHelmetSkin"),
                glm::translate(pos), Box<>(glm::vec3(-100.0f), glm::vec3(100.0f)));

            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
        }
    }
    
    for(unsigned int lightInstance = 0; lightInstance < 1000; lightInstance++) {
        illGraphics::LightBase * lightObj = new illGraphics::PointLight(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)), 1.0f, glm::linearRand(1.0f, 50.0f), glm::linearRand(60.0f, 100.0f));

        for(unsigned int light = 0; light < 1; light++) {
            new illRendererCommon::LightNode(m_graphicsScene,
                lightObj,
                glm::translate(glm::linearRand(glm::vec3(0.0f), glm::vec3(1000.0f))), 
                Box<>(glm::vec3(-500.0f), glm::vec3(500.0f)));
        }
    }

    /*new illRendererCommon::LightNode(m_graphicsScene,
        new illGraphics::PointLight(glm::vec3(1.0f), 1.0f, 20.0f, 500.0f),
        glm::translate(glm::vec3(500.0f)), 
        Box<>(glm::vec3(-500.0f), glm::vec3(500.0f)));*/
}

RendererDemoController::~RendererDemoController() {
    m_engine->m_inputManager->getInputContextStack(0)->popInputContext();
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
        illGraphics::DEFAULT_FOV * m_cameraController.m_zoom, illGraphics::DEFAULT_NEAR, 5000.0f);

    m_camera.setViewport(glm::ivec2(0, 0), glm::ivec2(m_engine->m_window->getResolution().x, m_engine->m_window->getResolution().y));

    m_graphicsScene->render(m_camera);
}

}
