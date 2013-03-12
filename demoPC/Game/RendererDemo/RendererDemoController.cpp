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

void renderSceneDebug(const GridVolume3D<>& gridVolume);
void renderMeshEdgeListDebug(const MeshEdgeList<>& edgeList);

namespace Demo {

void RendererDemoController::ChangeDebugMode::onRelease() {
    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_controller->m_rendererBackend)->m_debugMode = 
        static_cast<illDeferredShadingRenderer::DeferredShadingBackend::DebugMode>(m_mode);
}

void RendererDemoController::ToggleCamera::onRelease() {
    m_controller->m_engine->m_inputManager->getInputContextStack(0)->popInputContext();

    if(m_controller->m_whichCamera) {
        m_controller->m_whichCamera = false;
        m_controller->m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_controller->m_cameraController.m_inputContext);
    }
    else {
        m_controller->m_whichCamera = true;
        m_controller->m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_controller->m_occlusionCameraController.m_inputContext);
    }
}

RendererDemoController::RendererDemoController(Engine * engine)
    : GameControllerBase(),
    m_engine(engine),
    m_whichCamera(false),
    m_occlusionDebug(false),

    m_topDown(false),
    m_drawFrustum(false),
    m_drawGrid(false)
{ 
    //set up inputs
    m_noneDebugMode.m_controller = this;
    m_noneDebugMode.m_mode = static_cast<int>(illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::NONE);
    
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

    m_occlusionDebugToggle.m_value = &m_occlusionDebug;

    m_topDownToggle.m_value = &m_topDown;
    m_drawFrustumToggle.m_value = &m_drawFrustum;
    m_drawGridToggle.m_value = &m_drawGrid;

    m_toggleCamera.m_controller = this;

    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_1), &m_noneDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_2), &m_depthDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_3), &m_normalDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_4), &m_diffuseDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_5), &m_specularDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_6), &m_diffuseAccumulationDebugMode);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_7), &m_specularAccumulationDebugMode);

    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_l), &m_drawLightsToggle);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_b), &m_drawBoundsToggle);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_c), &m_performOcclusionToggle);

    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_o), &m_occlusionDebugToggle);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_p), &m_toggleCamera);

    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_t), &m_topDownToggle);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_f), &m_drawFrustumToggle);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_g), &m_drawGridToggle);

    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_inputContext);

    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_cameraController.m_inputContext);

    m_cameraController.m_speed = 50.0f;
    m_cameraController.m_rollSpeed = 50.0f;

    m_occlusionCameraController.m_speed = 50.0f;
    m_occlusionCameraController.m_rollSpeed = 50.0f;

    //setup renderer
    m_rendererBackend = new illDeferredShadingRenderer::DeferredShadingBackendGl3_3((GlCommon::GlBackend *)m_engine->m_graphicsBackend);

    m_drawLightsToggle.m_value = &static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugLights;
    m_drawBoundsToggle.m_value = &static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugBounds;
    m_performOcclusionToggle.m_value = &static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_performCull;

	m_graphicsScene = new illDeferredShadingRenderer::DeferredShadingScene(static_cast<illDeferredShadingRenderer::DeferredShadingBackend *> (m_rendererBackend),
        m_engine->m_meshManager, m_engine->m_materialManager,
        glm::vec3(200.0f), glm::uvec3(5), 
        glm::vec3(200.0f), glm::uvec3(5));
        
        /*glm::vec3(200.0f), glm::uvec3(10), 
        glm::vec3(50.0f), glm::uvec3(40));*/

    m_viewport = static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->registerViewport();

    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->initialize(m_engine->m_window->getResolution(), 
        engine->m_shaderProgramManager);
    
    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_occlusionCamera = &m_occlusionCamera;

	//for now place a bunch of random lights and meshes
    for(unsigned int mesh = 0; mesh < 1000; mesh++) {
        glm::vec3 pos = glm::linearRand(glm::vec3(0.0f), glm::vec3(1000.0f));

        {
            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
                m_engine->m_meshManager->getIdForName("Marine"), m_engine->m_materialManager->getIdForName("MarineSkin"),
                glm::translate(pos), Box<>(glm::vec3(-33.0f, -12.0f, -2.0f), glm::vec3(33.0f, 12.0f, 73.0f)));

            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
        }

        {
            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
                m_engine->m_meshManager->getIdForName("MarineHelmet"), m_engine->m_materialManager->getIdForName("MarineHelmetSkin"),
                glm::translate(pos), Box<>(glm::vec3(-8.0f, -8.0f, 65.0f), glm::vec3(8.0f, 8.0f, 80.0f)));

            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
        }
    }
    
    for(unsigned int lightInstance = 0; lightInstance < 1000; lightInstance++) {
        illGraphics::PointLight * lightObj = new illGraphics::PointLight(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)), 1.0f, glm::linearRand(1.0f, 50.0f), glm::linearRand(60.0f, 100.0f));

        for(unsigned int light = 0; light < 1; light++) {
            new illRendererCommon::LightNode(m_graphicsScene,
                lightObj,
                glm::translate(glm::linearRand(glm::vec3(0.0f), glm::vec3(1000.0f))), 
                Box<>(glm::vec3(-lightObj->m_attenuationEnd), glm::vec3(lightObj->m_attenuationEnd)));
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
    m_occlusionCameraController.update(seconds);
}

void RendererDemoController::updateSound(float seconds) {

}
 
void RendererDemoController::render() {
    m_camera.setPerspectiveTransform(m_cameraController.m_transform, 
        m_occlusionDebug ? m_engine->m_window->getAspectRatio() * 2.0f : m_engine->m_window->getAspectRatio(), 
        illGraphics::DEFAULT_FOV * m_cameraController.m_zoom, illGraphics::DEFAULT_NEAR, 2000.0f);

    if(m_topDown) {
        m_occlusionCamera.setOrthoTransform(createTransform(glm::vec3(
                m_graphicsScene->getGridVolume().getVolumeBounds().m_max.x * 0.5f, 
                m_graphicsScene->getGridVolume().getVolumeBounds().m_max.y + 50.0f, 
                m_graphicsScene->getGridVolume().getVolumeBounds().m_max.z * 0.5f),
                directionToMat3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))),

            -(m_graphicsScene->getGridVolume().getVolumeBounds().m_max.x * 0.5f + 50.0f) * m_engine->m_window->getAspectRatio() * 2.0f, 
            (m_graphicsScene->getGridVolume().getVolumeBounds().m_max.x * 0.5f + 50.0f) * m_engine->m_window->getAspectRatio() * 2.0f,

            -(m_graphicsScene->getGridVolume().getVolumeBounds().m_max.z * 0.5f + 50.0f), 
            m_graphicsScene->getGridVolume().getVolumeBounds().m_max.z * 0.5f + 50.0f,

            0.0f, 
            m_graphicsScene->getGridVolume().getVolumeBounds().m_max.y + 100.0f);
    }
    else {
        m_occlusionCamera.setPerspectiveTransform(m_occlusionCameraController.m_transform,
            m_engine->m_window->getAspectRatio() * 2.0f,
            illGraphics::DEFAULT_FOV * m_cameraController.m_zoom, illGraphics::DEFAULT_NEAR, 5000.0f);
    }

    m_camera.setViewport(glm::ivec2(0, 0), glm::ivec2(m_engine->m_window->getResolution().x, m_engine->m_window->getResolution().y));

    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugOcclusion = m_occlusionDebug;
    m_graphicsScene->render(m_camera, m_viewport);

    if(m_occlusionDebug) {
        glUseProgram(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glShadeModel(GL_SMOOTH);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(m_occlusionCamera.getProjection()));

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(m_occlusionCamera.getModelView()));

        glViewport(m_camera.getViewportCorner().x, m_camera.getViewportCorner().y,
            m_camera.getViewportDimensions().x, m_camera.getViewportDimensions().y / 2);

        if(m_drawGrid) {
            renderSceneDebug(m_graphicsScene->getGridVolume());
        }

        //clip the mesh against the bounds
        if(m_drawFrustum) {
            MeshEdgeList<> meshEdgeList = m_camera.getViewFrustum().getMeshEdgeList();

            meshEdgeList.convexClip(Plane<>(glm::vec3(1.0f, 0.0f, 0.0f), -m_graphicsScene->getGridVolume().getVolumeBounds().m_min.x));
            meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 1.0f, 0.0f), -m_graphicsScene->getGridVolume().getVolumeBounds().m_min.y));
            meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, 1.0f), -m_graphicsScene->getGridVolume().getVolumeBounds().m_min.z));
            meshEdgeList.convexClip(Plane<>(glm::vec3(-1.0f, 0.0f, 0.0f), m_graphicsScene->getGridVolume().getVolumeBounds().m_max.x));
            meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, -1.0f, 0.0f), m_graphicsScene->getGridVolume().getVolumeBounds().m_max.y));
            meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, -1.0f), m_graphicsScene->getGridVolume().getVolumeBounds().m_max.z));

            renderMeshEdgeListDebug(meshEdgeList);
        }
    }
}

}
