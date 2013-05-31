#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Engine.h"
#include "illEngine/Graphics/Window.h"
#include "illEngine/Console/serial/DeveloperConsole.h"

#include "RendererDemoController.h"
#include "illEngine/Graphics/serial/Model/Mesh.h"
#include "illEngine/Graphics/serial/Material/Material.h"
#include "illEngine/Input/serial/InputManager.h"

#include "illEngine/DeferredShadingRenderer/serial/DeferredShadingScene.h"
#include "illEngine/DeferredShadingRenderer/serial/Gl3_3/DeferredShadingBackendGl3_3.h"

#include "illEngine/RendererCommon/serial/StaticMeshNode.h"
#include "illEngine/RendererCommon/serial/LightNode.h"

#include "../../Util/CrappyBmFontRenderer.h"

#include <fstream>

//TODO: for now I'm testing a bunch of stuff, normally all rendering is done through the renderer
#include <GL/glew.h>

void renderSceneDebug(const GridVolume3D<>& gridVolume);
void renderMeshEdgeListDebug(const MeshEdgeList<>& edgeList);

namespace Demo {

/*void RendererDemoController::ChangeDebugMode::onRelease() {
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
}*/

RendererDemoController::RendererDemoController(Engine * engine, Scene scene)
    : GameControllerBase(),
    m_engine(engine),
    m_whichCamera(false),
    m_occlusionDebug(false),
    m_performCull(true),

    m_showPerformance(true),
    m_perObjectOcclusion(false),
    m_topDown(false),
    m_drawFrustum(false),
    m_drawGrid(false)
{ 
    //set up graphs
    m_numTraversedCellsGraph.m_name.assign("Traversed Cells");
    m_numEmptyCellsGraph.m_name.assign("Empty Cells");
    m_numNonEmptyCellsGraph.m_name.assign("Non-Empty Cells");
    m_numCellQueriesGraph.m_name.assign("Cell Queries");
    m_numCellsUnqueriedGraph.m_name.assign("Cells Unqueried");
    m_numRenderedCellsGraph.m_name.assign("Rendered Cells");
    m_numCulledCellsGraph.m_name.assign("Culled Cells");
    m_cellRequeryDurationGraph.m_name.assign("Cell Requry Frames");
    m_numProcessedNodesGraph.m_name.assign("Processed Nodes");
    m_numOverflowedQueriesGraph.m_name.assign("Overflowed Queries");

    m_numTraversedCellsGraph.m_fontRenderer = m_engine->m_crappyFontRenderer;
    m_numEmptyCellsGraph.m_fontRenderer = m_engine->m_crappyFontRenderer;
    m_numNonEmptyCellsGraph.m_fontRenderer = m_engine->m_crappyFontRenderer;
    m_numCellQueriesGraph.m_fontRenderer = m_engine->m_crappyFontRenderer;
    m_numCellsUnqueriedGraph.m_fontRenderer = m_engine->m_crappyFontRenderer;
    m_numRenderedCellsGraph.m_fontRenderer = m_engine->m_crappyFontRenderer;    
    m_numCulledCellsGraph.m_fontRenderer = m_engine->m_crappyFontRenderer;
    m_cellRequeryDurationGraph.m_fontRenderer = m_engine->m_crappyFontRenderer;
    m_numProcessedNodesGraph.m_fontRenderer = m_engine->m_crappyFontRenderer;
    m_numOverflowedQueriesGraph.m_fontRenderer = m_engine->m_crappyFontRenderer;

    //set up inputs        
    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_inputContext);

    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_cameraController.m_inputContext);

    m_cameraController.m_speed = 50.0f;
    m_cameraController.m_rollSpeed = 50.0f;

    m_occlusionCameraController.m_speed = 50.0f;
    m_occlusionCameraController.m_rollSpeed = 50.0f;

    //setup renderer
    m_rendererBackend = new illDeferredShadingRenderer::DeferredShadingBackendGl3_3((GlCommon::GlBackend *)m_engine->m_graphicsBackend);
    
    m_cv_ren_deferredDebugMode = new illConsole::ConsoleVariable("NONE", "Which aspect of the deferred shading engine is being visualized.",
        [&] (illConsole::ConsoleVariable * var, const char * value) {
            std::istringstream stream(value);

            std::string dest;
            if(m_engine->m_developerConsole->getParamString(stream, dest) 
                    && m_engine->m_developerConsole->checkParamEnd(stream)) {
                if(dest.compare("NONE") == 0) {
                    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugMode = 
                        illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::NONE;
                    return true;
                }
                else if(dest.compare("DEPTH") == 0) {
                    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugMode = 
                        illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::DEPTH;
                    return true;
                }
                else if(dest.compare("NORMAL") == 0) {
                    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugMode = 
                        illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::NORMAL;
                    return true;
                }
                else if(dest.compare("DIFFUSE") == 0) {
                    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugMode = 
                        illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::DIFFUSE;
                    return true;
                }
                else if(dest.compare("SPECULAR") == 0) {
                    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugMode = 
                        illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::SPECULAR;
                    return true;
                }
                else if(dest.compare("DIFFUSE_ACCUMULATION") == 0) {
                    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugMode = 
                        illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::DIFFUSE_ACCUMULATION;
                    return true;
                }
                else if(dest.compare("SPECULAR_ACCUMULATION") == 0) {
                    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugMode = 
                        illDeferredShadingRenderer::DeferredShadingBackend::DebugMode::SPECULAR_ACCUMULATION;
                    return true;
                }
                
                return false;
            }

            return false;
        });

    m_engine->m_developerConsole->m_variableManager->addVariable("ren_deferredDebugMode", m_cv_ren_deferredDebugMode);

    m_cv_ren_showPerf = new illConsole::ConsoleVariable("0", "TODO: description",
        [&] (illConsole::ConsoleVariable * var, const char * value) {
            std::istringstream stream(value);

            bool dest;
            if(m_engine->m_developerConsole->getParamBool(stream, dest) 
                    && m_engine->m_developerConsole->checkParamEnd(stream)) {
                m_showPerformance = dest;                
                return true;
            }

            return false;
        });

    m_engine->m_developerConsole->m_variableManager->addVariable("ren_showPerf", m_cv_ren_showPerf);

    m_cv_ren_stencilLighting = new illConsole::ConsoleVariable("1", "TODO: description",
        [&] (illConsole::ConsoleVariable * var, const char * value) {
            std::istringstream stream(value);

            bool dest;
            if(m_engine->m_developerConsole->getParamBool(stream, dest) 
                    && m_engine->m_developerConsole->checkParamEnd(stream)) {
                static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_stencilLightingPass = dest;                
                return true;
            }

            return false;
        });

    m_engine->m_developerConsole->m_variableManager->addVariable("ren_stencilLighting", m_cv_ren_stencilLighting);

    m_cv_ren_occlusionCull = new illConsole::ConsoleVariable("1", "TODO: description",
        [&] (illConsole::ConsoleVariable * var, const char * value) {
            std::istringstream stream(value);

            bool dest;
            if(m_engine->m_developerConsole->getParamBool(stream, dest) 
                    && m_engine->m_developerConsole->checkParamEnd(stream)) {
                m_performCull = dest;                
                return true;
            }

            return false;
        });

    m_engine->m_developerConsole->m_variableManager->addVariable("ren_occlusionCull", m_cv_ren_occlusionCull);

    m_cv_ren_showLights = new illConsole::ConsoleVariable("0", "TODO: description",
        [&] (illConsole::ConsoleVariable * var, const char * value) {
            std::istringstream stream(value);

            bool dest;
            if(m_engine->m_developerConsole->getParamBool(stream, dest) 
                    && m_engine->m_developerConsole->checkParamEnd(stream)) {
                static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugLights = dest;                
                return true;
            }

            return false;
        });

    m_engine->m_developerConsole->m_variableManager->addVariable("ren_showLights", m_cv_ren_showLights);

    m_cv_ren_showBounds = new illConsole::ConsoleVariable("0", "TODO: description",
        [&] (illConsole::ConsoleVariable * var, const char * value) {
            std::istringstream stream(value);

            bool dest;
            if(m_engine->m_developerConsole->getParamBool(stream, dest) 
                    && m_engine->m_developerConsole->checkParamEnd(stream)) {
                static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugBounds = dest;                
                return true;
            }

            return false;
        });

    m_engine->m_developerConsole->m_variableManager->addVariable("ren_showBounds", m_cv_ren_showBounds);

    m_cv_ren_showGrid = new illConsole::ConsoleVariable("0", "TODO: description",
        [&] (illConsole::ConsoleVariable * var, const char * value) {
            std::istringstream stream(value);

            bool dest;
            if(m_engine->m_developerConsole->getParamBool(stream, dest) 
                    && m_engine->m_developerConsole->checkParamEnd(stream)) {
                m_drawGrid = dest;                
                return true;
            }

            return false;
        });

    m_engine->m_developerConsole->m_variableManager->addVariable("ren_showGrid", m_cv_ren_showGrid);

    m_cv_ren_showCullDebug = new illConsole::ConsoleVariable("0", "TODO: description",
        [&] (illConsole::ConsoleVariable * var, const char * value) {
            std::istringstream stream(value);

            bool dest;
            if(m_engine->m_developerConsole->getParamBool(stream, dest) 
                    && m_engine->m_developerConsole->checkParamEnd(stream)) {
                m_occlusionDebug = dest;                
                return true;
            }

            return false;
        });

    m_engine->m_developerConsole->m_variableManager->addVariable("ren_showCullDebug", m_cv_ren_showCullDebug);

    m_cv_ren_controlCullCamera = new illConsole::ConsoleVariable("0", "TODO: description",
        [&] (illConsole::ConsoleVariable * var, const char * value) {
            std::istringstream stream(value);

            bool dest;
            if(m_engine->m_developerConsole->getParamBool(stream, dest) 
                    && m_engine->m_developerConsole->checkParamEnd(stream)) {
                m_whichCamera = dest;
                size_t cameraStackPos;

                if(m_whichCamera) {
                    m_engine->m_inputManager->getInputContextStack(0)->findInputContextStackPos(&m_cameraController.m_inputContext, cameraStackPos);
                    m_engine->m_inputManager->getInputContextStack(0)->replaceInputContext(&m_occlusionCameraController.m_inputContext, cameraStackPos);
                }
                else {
                    m_engine->m_inputManager->getInputContextStack(0)->findInputContextStackPos(&m_occlusionCameraController.m_inputContext, cameraStackPos);
                    m_engine->m_inputManager->getInputContextStack(0)->replaceInputContext(&m_cameraController.m_inputContext, cameraStackPos);
                }
                return true;
            }

            return false;
        });

    m_engine->m_developerConsole->m_variableManager->addVariable("ren_controlCullCamera", m_cv_ren_controlCullCamera);

    m_cv_ren_showFrustum = new illConsole::ConsoleVariable("0", "TODO: description",
        [&] (illConsole::ConsoleVariable * var, const char * value) {
            std::istringstream stream(value);

            bool dest;
            if(m_engine->m_developerConsole->getParamBool(stream, dest) 
                    && m_engine->m_developerConsole->checkParamEnd(stream)) {
                m_drawFrustum = dest;
                return true;
            }

            return false;
        });

    m_engine->m_developerConsole->m_variableManager->addVariable("ren_showFrustum", m_cv_ren_showFrustum);
    
    m_cm_ren_freezeFrustum = new illConsole::ConsoleCommand("TODO: description",
        [&] (const illConsole::ConsoleCommand *, const char * params) {
            std::istringstream stream(params ? params : "");

            if(m_engine->m_developerConsole->checkParamEnd(stream)) {
                static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugCapturingFrustumIter = true;
            }
        });

    m_engine->m_developerConsole->m_commandManager->addCommand("ren_freezeFrustum", m_cm_ren_freezeFrustum);

    m_cm_ren_unfreezeFrustum = new illConsole::ConsoleCommand("TODO: description",
        [&] (const illConsole::ConsoleCommand *, const char * params) {
            std::istringstream stream(params ? params : "");

            if(m_engine->m_developerConsole->checkParamEnd(stream)) {
                //delete static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumIterator;
                static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumIterator = NULL;
            }
        });

    m_engine->m_developerConsole->m_commandManager->addCommand("ren_unfreezeFrustum", m_cm_ren_unfreezeFrustum);

    m_engine->m_developerConsole->consoleInput("..\\..\\..\\bindDebugStuff.cfg");

    m_cm_ren_advanceFrustum = new illConsole::ConsoleCommand("TODO: description",
        [&] (const illConsole::ConsoleCommand *, const char * params) {
            std::istringstream stream(params ? params : "");

            if(m_engine->m_developerConsole->checkParamEnd(stream)) {
                if(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumIterator
                    && !static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumIterator->atEnd()) {
                    static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumIterator->forward();
                    static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumTraversals.push_back(
                        static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumIterator->getCurrentPosition());
                }
            }
        });

    m_engine->m_developerConsole->m_commandManager->addCommand("ren_advanceFrustum", m_cm_ren_advanceFrustum);

    m_cm_ren_restartFrustum = new illConsole::ConsoleCommand("TODO: description",
        [&] (const illConsole::ConsoleCommand *, const char * params) {
            std::istringstream stream(params ? params : "");

            if(m_engine->m_developerConsole->checkParamEnd(stream)) {
                if(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugBackupFrustumIterator) {
                    //delete static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumIterator;
                    static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumIterator = 
                        new MultiConvexMeshIterator<>(*static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugBackupFrustumIterator);

                    static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumTraversals.clear();

                    if(!static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumIterator->atEnd()) {
                        static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumTraversals.push_back(
                            static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugFrustumIterator->getCurrentPosition());
                    }
                }
            }
        });

    m_engine->m_developerConsole->m_commandManager->addCommand("ren_restartFrustum", m_cm_ren_restartFrustum);

    switch(scene) {
    case Scene::THE_GRID:

        //at the moment, just parse the hack file exported from 3DS max, illscene parsing coming soon
        {
            m_cameraController.m_speed = 10.0f;
            m_cameraController.m_rollSpeed = 50.0f;

            std::ifstream openFile("..\\..\\..\\assets\\maps\\HangarTest.txt");

            //read number of static meshes
            //int numMeshes;
            //openFile >> numMeshes;
            
            m_graphicsScene = new illDeferredShadingRenderer::DeferredShadingScene(static_cast<illDeferredShadingRenderer::DeferredShadingBackend *> (m_rendererBackend),
                m_engine->m_meshManager, m_engine->m_materialManager,
                
                glm::vec3(38.0f), glm::uvec3(33, 20, 33), 
                glm::vec3(38.0f), glm::uvec3(33, 20, 33));

                //glm::vec3(25.0f), glm::uvec3(50, 30, 50), 
                //glm::vec3(25.0f), glm::uvec3(50, 30, 50));
                            
                //glm::vec3(5.0f, 12.0f, 5.0f), glm::uvec3(26, 2, 42), 
                //glm::vec3(5.0f, 12.0f, 5.0f), glm::uvec3(26, 2, 42));

            //for now just add these as dynamic meshes, later they will be static meshes
            //for(int mesh = 0; mesh < numMeshes; mesh++) {
            while(!openFile.eof()) {
                //read mesh name
                std::string meshName;
                openFile >> meshName;

                if(openFile.eof()) {
                    break;
                }

                enum class ObjectType {
                    MESH,
                    POINT_LIGHT,
                    SPOT_LIGHT,
                    VOLUME_LIGHT,
                    START
                } objectType = ObjectType::MESH;
                
                if(meshName.compare("__PointLight__") == 0) {
                    objectType = ObjectType::POINT_LIGHT;
                }
                else if(meshName.compare("__SpotLight__") == 0) {
                    objectType = ObjectType::SPOT_LIGHT;
                }
                else if(meshName.compare("__VolumeLight__") == 0) {
                    objectType = ObjectType::VOLUME_LIGHT;
                }
                else if(meshName.compare("__Start__") == 0) {
                    objectType = ObjectType::START;
                }

                glm::mat4 transform;

                //read the 3x4 transform                
                for(unsigned int row = 0; row < 3; row++) {
                    for(unsigned int column = 0; column < 4; column++) {
                        openFile >> transform[column][row];
                    }
                }

                if(objectType == ObjectType::START) {
                    m_cameraController.m_transform = transform;
                    continue;
                }
        
                if(objectType == ObjectType::POINT_LIGHT || objectType == ObjectType::SPOT_LIGHT || objectType == ObjectType::VOLUME_LIGHT) {
                    Box<> bounds;

                    if(objectType == ObjectType::VOLUME_LIGHT) {
                        //read the bounding box
                        for(unsigned int vec = 0; vec < 3; vec++) {
                            openFile >> bounds.m_min[vec];
                        }

                        for(unsigned int vec = 0; vec < 3; vec++) {
                            openFile >> bounds.m_max[vec];
                        }
                    }
                    
                    //light color
                    glm::vec3 color;

                    for(unsigned int vec = 0; vec < 3; vec++) {
                        openFile >> color[vec];
                    }

                    //light intensity
                    float intensity;
                    openFile >> intensity;

                    //whether or not using specular
                    bool specular;
                    openFile >> specular;

                    if(objectType == ObjectType::POINT_LIGHT || objectType == ObjectType::SPOT_LIGHT) {
                        //near attenuation
                        float nearAtten;
                        openFile >> nearAtten;

                        //far attenuation
                        float farAtten;
                        openFile >> farAtten;

                        if(objectType == ObjectType::POINT_LIGHT) {
                            illGraphics::PointLight * lightObj = new illGraphics::PointLight(color, intensity, specular, nearAtten, farAtten);

                            auto newLight = new illRendererCommon::LightNode(m_graphicsScene, transform, Box<>(glm::vec3(-farAtten), glm::vec3(farAtten)));
                            newLight->m_light = RefCountPtr<illGraphics::LightBase>(lightObj);
                        }
                        else {
                            float coneStart;
                            openFile >> coneStart;

                            float coneEnd;
                            openFile >> coneEnd;

                            illGraphics::SpotLight * lightObj = new illGraphics::SpotLight(color, intensity, specular, nearAtten, farAtten, coneStart, coneEnd);

                            Box<> bounds(glm::vec3(0.0f));

                            //Computing bounding box of light cone
                            //TODO: move this to the util geometry code somewhere
                            {
                                glm::vec3 direction = glm::mat3(transform) * glm::vec3(0.0f, 0.0f, -1.0f);
                                glm::vec3 endPos = direction * farAtten;
                                glm::mediump_float coneDir = glm::degrees(glm::acos(coneEnd));

                                bounds.addPoint(endPos);

                                bounds.addPoint(glm::mat3(glm::rotate(coneDir, glm::vec3(1.0f, 0.0f, 0.0f))) * endPos);
                                bounds.addPoint(glm::mat3(glm::rotate(-coneDir, glm::vec3(1.0f, 0.0f, 0.0f))) * endPos);

                                bounds.addPoint(glm::mat3(glm::rotate(coneDir, glm::vec3(0.0f, 1.0f, 0.0f))) * endPos);
                                bounds.addPoint(glm::mat3(glm::rotate(-coneDir, glm::vec3(0.0f, 1.0f, 0.0f))) * endPos);

                                bounds.addPoint(glm::mat3(glm::rotate(coneDir, glm::vec3(0.0f, 0.0f, 1.0f))) * endPos);
                                bounds.addPoint(glm::mat3(glm::rotate(-coneDir, glm::vec3(0.0f, 0.0f, 1.0f))) * endPos);
                            }

                            auto newLight = new illRendererCommon::LightNode(m_graphicsScene, transform, bounds);
                            newLight->m_light = RefCountPtr<illGraphics::LightBase>(lightObj);
                        }
                    }
                    else {
                        bool directional;
                        openFile >> directional;

                        glm::vec3 vector;

                        for(unsigned int vec = 0; vec < 3; vec++) {
                            openFile >> vector[vec];
                        }

                        illGraphics::VolumeLight * lightObj = new illGraphics::VolumeLight(color, intensity, specular, directional, vector);

                        int numPlanes;
                        openFile >> numPlanes;

                        Plane<> lastPlane;
                        glm::mediump_float lastAttenuation;

                        for(int plane = 0; plane < numPlanes; plane++) {
                            openFile >> lastPlane.m_normal.x;
                            openFile >> lastPlane.m_normal.y;
                            openFile >> lastPlane.m_normal.z;
                            openFile >> lastPlane.m_distance;

                            openFile >> lastAttenuation;

                            lightObj->m_planes[plane] = lastPlane;
                            lightObj->m_planeFalloff[plane] = lastAttenuation;
                        }

                        //pad the remaining planes with the previously read plane
                        for(unsigned int plane = numPlanes; plane < illGraphics::MAX_LIGHT_VOLUME_PLANES; plane++) {
                            lightObj->m_planes[plane] = lastPlane;
                            lightObj->m_planeFalloff[plane] = lastAttenuation;
                        }

                        auto newLight = new illRendererCommon::LightNode(m_graphicsScene, transform, bounds);
                        newLight->m_light = RefCountPtr<illGraphics::LightBase>(lightObj);
                    }
                }
                else {
                    Box<> bounds;
                    
                    //read the bounding box
                    for(unsigned int vec = 0; vec < 3; vec++) {
                        openFile >> bounds.m_min[vec];
                    }

                    for(unsigned int vec = 0; vec < 3; vec++) {
                        openFile >> bounds.m_max[vec];
                    }

                    //read occluder type
                    int occluderType;
                    openFile >> occluderType;

                    if(occluderType > 2) {
                        LOG_FATAL_ERROR("Invalid occluder type %d for mesh %s", occluderType, meshName.c_str());
                    }

                    illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene,
                        transform, bounds, (illRendererCommon::StaticMeshNode::OccluderType) occluderType);
                    
                    node->m_meshId = m_engine->m_meshManager->getIdForName(meshName.c_str());

                    //read number of primitive groups
                    int numGroups;
                    openFile >> numGroups;

                    node->m_primitiveGroups.resize(numGroups);

                    //read material names
                    for(int group = 0; group < numGroups; group++) {
                        std::string matName;
                        openFile >> matName;

                        node->m_primitiveGroups[group].m_materialId = m_engine->m_materialManager->getIdForName(matName.c_str());
                        node->m_primitiveGroups[group].m_visible = true;
                    }

                    node->load(m_engine->m_meshManager, m_engine->m_materialManager);
                }
            }
        }

        LOG_INFO("Scene done loading");

        break;

    case Scene::ORGANIZED:

        m_graphicsScene = new illDeferredShadingRenderer::DeferredShadingScene(static_cast<illDeferredShadingRenderer::DeferredShadingBackend *> (m_rendererBackend),
            m_engine->m_meshManager, m_engine->m_materialManager,        
            glm::vec3(100.0f), glm::uvec3(20, 2, 20), 
            glm::vec3(25.0f), glm::uvec3(80, 8, 80));

        //the test multimesh
        {
            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene,
                glm::mat4(), Box<>(glm::vec3(-100.0f), glm::vec3(100.0f)));

            node->m_meshId = m_engine->m_meshManager->getIdForName("HangarHallA");
            node->m_primitiveGroups.resize(4);

            node->m_primitiveGroups[0].m_materialId = m_engine->m_materialManager->getIdForName("mcityc7");
            node->m_primitiveGroups[0].m_visible = true;

            node->m_primitiveGroups[1].m_materialId = m_engine->m_materialManager->getIdForName("GoobyPlsMaterial");
            node->m_primitiveGroups[1].m_visible = true;

            node->m_primitiveGroups[2].m_materialId = m_engine->m_materialManager->getIdForName("MarineHelmetSkin");
            node->m_primitiveGroups[2].m_visible = true;

            node->m_primitiveGroups[3].m_materialId = m_engine->m_materialManager->getIdForName("MarineSkin");
            node->m_primitiveGroups[3].m_visible = true;

            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
        }

        break;

    //case Scene::ORGANIZED:

    //    m_graphicsScene = new illDeferredShadingRenderer::DeferredShadingScene(static_cast<illDeferredShadingRenderer::DeferredShadingBackend *> (m_rendererBackend),
    //        m_engine->m_meshManager, m_engine->m_materialManager,        
    //        glm::vec3(100.0f), glm::uvec3(20, 2, 20), 
    //        glm::vec3(25.0f), glm::uvec3(80, 8, 80));
    //    
    //    //marines
    //    for(unsigned int mesh = 0; mesh < 200; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(0.0f), glm::vec3(500.0f, 200.0f, 500.0f));

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("Marine"), m_engine->m_materialManager->getIdForName("MarineSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-33.0f, -12.0f, -2.0f), glm::vec3(33.0f, 12.0f, 73.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("MarineHelmet"), m_engine->m_materialManager->getIdForName("MarineHelmetSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-8.0f, -8.0f, 65.0f), glm::vec3(8.0f, 8.0f, 80.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }
    //    }

    //    //small walls
    //    for(unsigned int mesh = 0; mesh < 500; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(0.0f), glm::vec3(500.0f, 200.0f, 500.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(0.7f, 5.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }

    //    //huge walls
    //    for(unsigned int mesh = 0; mesh < 3; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(0.0f), glm::vec3(500.0f, 200.0f, 500.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(30.0f, 100.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }

    //    //gooby pls
    //    {
    //        glm::vec3 pos(1000.0f, 400.0f, 1000.0f);
    //        glm::mediump_float scale(200.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("GoobyPlsMaterial"),
    //            glm::scale(glm::translate(pos), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);

    //        new illRendererCommon::LightNode(m_graphicsScene,
    //            new illGraphics::PointLight(glm::vec3(1.0f),
    //                1.0f, 100.0f, 400.0f),
    //            glm::translate(glm::vec3(1000.0f, 500.0f, 900.0f)), 
    //            Box<>(glm::vec3(-400.0f), glm::vec3(400.0f)));
    //    }

    //    //lights
    //    for(unsigned int lightInstance = 0; lightInstance < 200; lightInstance++) {
    //        illGraphics::PointLight * lightObj = new illGraphics::PointLight(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)),
    //            1.0f, glm::linearRand(1.0f, 50.0f), glm::linearRand(60.0f, 100.0f));

    //        for(unsigned int light = 0; light < 1; light++) {
    //            new illRendererCommon::LightNode(m_graphicsScene,
    //                lightObj,
    //                glm::translate(glm::linearRand(glm::vec3(0.0f), glm::vec3(500.0f, 200.0f, 500.0f))), 
    //                Box<>(glm::vec3(-lightObj->m_attenuationEnd), glm::vec3(lightObj->m_attenuationEnd)));
    //        }
    //    }

    //    //marines
    //    for(unsigned int mesh = 0; mesh < 100; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(700.0f, 0.0f, 700.0f), glm::vec3(900.0f, 200.0f, 900.0f));

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("Marine"), m_engine->m_materialManager->getIdForName("MarineSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-33.0f, -12.0f, -2.0f), glm::vec3(33.0f, 12.0f, 73.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("MarineHelmet"), m_engine->m_materialManager->getIdForName("MarineHelmetSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-8.0f, -8.0f, 65.0f), glm::vec3(8.0f, 8.0f, 80.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }
    //    }

    //    //small walls
    //    for(unsigned int mesh = 0; mesh < 500; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(700.0f, 0.0f, 700.0f), glm::vec3(900.0f, 200.0f, 900.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(0.7f, 5.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }

    //    //huge walls
    //    for(unsigned int mesh = 0; mesh < 3; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(700.0f, 0.0f, 700.0f), glm::vec3(900.0f, 200.0f, 900.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(30.0f, 100.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }

    //    //lights
    //    for(unsigned int lightInstance = 0; lightInstance < 200; lightInstance++) {
    //        illGraphics::PointLight * lightObj = new illGraphics::PointLight(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)),
    //            1.0f, glm::linearRand(1.0f, 50.0f), glm::linearRand(60.0f, 100.0f));

    //        for(unsigned int light = 0; light < 1; light++) {
    //            new illRendererCommon::LightNode(m_graphicsScene,
    //                lightObj,
    //                glm::translate(glm::linearRand(glm::vec3(700.0f, 0.0f, 700.0f), glm::vec3(900.0f, 200.0f, 900.0f))), 
    //                Box<>(glm::vec3(-lightObj->m_attenuationEnd), glm::vec3(lightObj->m_attenuationEnd)));
    //        }
    //    }

    //    //marines
    //    for(unsigned int mesh = 0; mesh < 100; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(300.0f, 0.0f, 700.0f), glm::vec3(500.0f, 200.0f, 900.0f));

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("Marine"), m_engine->m_materialManager->getIdForName("MarineSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-33.0f, -12.0f, -2.0f), glm::vec3(33.0f, 12.0f, 73.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("MarineHelmet"), m_engine->m_materialManager->getIdForName("MarineHelmetSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-8.0f, -8.0f, 65.0f), glm::vec3(8.0f, 8.0f, 80.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }
    //    }

    //    //small walls
    //    for(unsigned int mesh = 0; mesh < 500; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(300.0f, 0.0f, 700.0f), glm::vec3(500.0f, 200.0f, 900.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(0.7f, 5.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }

    //    //huge walls
    //    for(unsigned int mesh = 0; mesh < 3; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(300.0f, 0.0f, 700.0f), glm::vec3(500.0f, 200.0f, 900.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(30.0f, 100.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }

    //    //lights
    //    for(unsigned int lightInstance = 0; lightInstance < 200; lightInstance++) {
    //        illGraphics::PointLight * lightObj = new illGraphics::PointLight(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)),
    //            1.0f, glm::linearRand(1.0f, 50.0f), glm::linearRand(60.0f, 100.0f));

    //        for(unsigned int light = 0; light < 1; light++) {
    //            new illRendererCommon::LightNode(m_graphicsScene,
    //                lightObj,
    //                glm::translate(glm::linearRand(glm::vec3(300.0f, 0.0f, 700.0f), glm::vec3(500.0f, 200.0f, 900.0f))), 
    //                Box<>(glm::vec3(-lightObj->m_attenuationEnd), glm::vec3(lightObj->m_attenuationEnd)));
    //        }
    //    }

    //    //marines
    //    for(unsigned int mesh = 0; mesh < 300; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(1500.0f, 0.0f, 1000.0f), glm::vec3(1600.0f, 200.0f, 1500.0f));

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("Marine"), m_engine->m_materialManager->getIdForName("MarineSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-33.0f, -12.0f, -2.0f), glm::vec3(33.0f, 12.0f, 73.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("MarineHelmet"), m_engine->m_materialManager->getIdForName("MarineHelmetSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-8.0f, -8.0f, 65.0f), glm::vec3(8.0f, 8.0f, 80.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }
    //    }

    //    //small walls
    //    for(unsigned int mesh = 0; mesh < 500; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(1500.0f, 0.0f, 1000.0f), glm::vec3(1600.0f, 200.0f, 1500.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(0.7f, 5.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }

    //    //huge walls
    //    for(unsigned int mesh = 0; mesh < 3; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(1500.0f, 0.0f, 1000.0f), glm::vec3(1600.0f, 200.0f, 1500.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(30.0f, 100.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }

    //    //lights
    //    for(unsigned int lightInstance = 0; lightInstance < 200; lightInstance++) {
    //        illGraphics::PointLight * lightObj = new illGraphics::PointLight(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)),
    //            1.0f, glm::linearRand(1.0f, 50.0f), glm::linearRand(60.0f, 100.0f));

    //        for(unsigned int light = 0; light < 1; light++) {
    //            new illRendererCommon::LightNode(m_graphicsScene,
    //                lightObj,
    //                glm::translate(glm::linearRand(glm::vec3(1500.0f, 0.0f, 1000.0f), glm::vec3(1600.0f, 200.0f, 1500.0f))), 
    //                Box<>(glm::vec3(-lightObj->m_attenuationEnd), glm::vec3(lightObj->m_attenuationEnd)));
    //        }
    //    }

    //    break;

    //case Scene::SHORT_CHAOS:

    //    m_graphicsScene = new illDeferredShadingRenderer::DeferredShadingScene(static_cast<illDeferredShadingRenderer::DeferredShadingBackend *> (m_rendererBackend),
    //        m_engine->m_meshManager, m_engine->m_materialManager,        
    //        glm::vec3(100.0f), glm::uvec3(20, 2, 20), 
    //        glm::vec3(25.0f), glm::uvec3(80, 8, 80));
    //    
    //    //marines
    //    for(unsigned int mesh = 0; mesh < 500; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(0.0f), glm::vec3(2000.0f, 200.0f, 2000.0f));

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("Marine"), m_engine->m_materialManager->getIdForName("MarineSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-33.0f, -12.0f, -2.0f), glm::vec3(33.0f, 12.0f, 73.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("MarineHelmet"), m_engine->m_materialManager->getIdForName("MarineHelmetSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-8.0f, -8.0f, 65.0f), glm::vec3(8.0f, 8.0f, 80.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }
    //    }

    //    //small walls
    //    for(unsigned int mesh = 0; mesh < 500; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(0.0f), glm::vec3(2000.0f, 200.0f, 2000.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(0.7f, 5.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }

    //    //huge walls
    //    for(unsigned int mesh = 0; mesh < 3; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(0.0f), glm::vec3(2000.0f, 200.0f, 2000.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(30.0f, 100.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }
    //
    //    //lights
    //    for(unsigned int lightInstance = 0; lightInstance < 500; lightInstance++) {
    //        illGraphics::PointLight * lightObj = new illGraphics::PointLight(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)),
    //            1.0f, glm::linearRand(1.0f, 50.0f), glm::linearRand(60.0f, 100.0f));

    //        for(unsigned int light = 0; light < 1; light++) {
    //            new illRendererCommon::LightNode(m_graphicsScene,
    //                lightObj,
    //                glm::translate(glm::linearRand(glm::vec3(0.0f), glm::vec3(2000.0f, 200.0f, 2000.0f))), 
    //                Box<>(glm::vec3(-lightObj->m_attenuationEnd), glm::vec3(lightObj->m_attenuationEnd)));
    //        }
    //    }

    //    break;

    //case Scene::CHAOS:

    //    m_graphicsScene = new illDeferredShadingRenderer::DeferredShadingScene(static_cast<illDeferredShadingRenderer::DeferredShadingBackend *> (m_rendererBackend),
    //        m_engine->m_meshManager, m_engine->m_materialManager,        
    //        glm::vec3(100.0f), glm::uvec3(10), 
    //        glm::vec3(25.0f), glm::uvec3(40));
    //            
    //    //marines
    //    for(unsigned int mesh = 0; mesh < 1000; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(0.0f), glm::vec3(1000.0f));

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("Marine"), m_engine->m_materialManager->getIdForName("MarineSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-33.0f, -12.0f, -2.0f), glm::vec3(33.0f, 12.0f, 73.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }

    //        {
    //            illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //                m_engine->m_meshManager->getIdForName("MarineHelmet"), m_engine->m_materialManager->getIdForName("MarineHelmetSkin"),
    //                glm::translate(pos), Box<>(glm::vec3(-8.0f, -8.0f, 65.0f), glm::vec3(8.0f, 8.0f, 80.0f)));

    //            node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //        }
    //    }

    //    //small walls
    //    for(unsigned int mesh = 0; mesh < 995; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(0.0f), glm::vec3(1000.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(0.7f, 5.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }

    //    //huge walls
    //    for(unsigned int mesh = 0; mesh < 5; mesh++) {
    //        glm::vec3 pos = glm::linearRand(glm::vec3(0.0f), glm::vec3(1000.0f));
    //        glm::vec3 rotAxis = glm::sphericalRand(1.0f);
    //        glm::mediump_float rotAngle = glm::linearRand(0.0f, 360.0f);
    //        glm::mediump_float scale = glm::linearRand(30.0f, 100.0f);

    //        illRendererCommon::StaticMeshNode * node = new illRendererCommon::StaticMeshNode(m_graphicsScene, 
    //            m_engine->m_meshManager->getIdForName("Wall"), m_engine->m_materialManager->getIdForName("WallMaterial"),
    //            glm::scale(glm::rotate(glm::translate(pos), rotAngle, rotAxis), glm::vec3(scale)),

    //            Box<>(glm::vec3(-4.0f * scale), glm::vec3(4.0f * scale)));

    //        node->load(m_engine->m_meshManager, m_engine->m_materialManager);
    //    }
    //
    //    //lights
    //    for(unsigned int lightInstance = 0; lightInstance < 1000; lightInstance++) {
    //        illGraphics::PointLight * lightObj = new illGraphics::PointLight(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)),
    //            1.0f, glm::linearRand(1.0f, 50.0f), glm::linearRand(60.0f, 100.0f));

    //        for(unsigned int light = 0; light < 1; light++) {
    //            new illRendererCommon::LightNode(m_graphicsScene,
    //                lightObj,
    //                glm::translate(glm::linearRand(glm::vec3(0.0f), glm::vec3(1000.0f))), 
    //                Box<>(glm::vec3(-lightObj->m_attenuationEnd), glm::vec3(lightObj->m_attenuationEnd)));
    //        }
    //    }

    //    break;
    }

    m_viewport = static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->registerViewport();

    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->initialize(glm::uvec2(m_engine->m_window->m_screenWidth, m_engine->m_window->m_screenHeight), 
        engine->m_shaderProgramManager);
    
    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_occlusionCamera = &m_occlusionCamera;
}

RendererDemoController::~RendererDemoController() {
    m_engine->m_inputManager->getInputContextStack(0)->popInputContext();
    m_engine->m_inputManager->getInputContextStack(0)->popInputContext();

    //TODO: clean the cvars

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

    m_camera.setViewport(glm::ivec2(0, 0), glm::ivec2(m_engine->m_window->m_screenWidth, m_engine->m_window->m_screenHeight));

    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_debugOcclusion = m_occlusionDebug;
    
    static_cast<illDeferredShadingRenderer::DeferredShadingBackend *>(m_rendererBackend)->m_performCull = m_performCull;
    static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_performCull = m_performCull;
    
    static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugPerObjectCull = m_perObjectOcclusion;

    m_graphicsScene->setupFrame();

    if(m_occlusionDebug) {
        MeshEdgeList<> meshEdgeList = m_camera.getViewFrustum().getMeshEdgeList();

        meshEdgeList.convexClip(Plane<>(glm::vec3(1.0f, 0.0f, 0.0f), -m_graphicsScene->getGridVolume().getVolumeBounds().m_min.x));
        meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 1.0f, 0.0f), -m_graphicsScene->getGridVolume().getVolumeBounds().m_min.y));
        meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, 1.0f), -m_graphicsScene->getGridVolume().getVolumeBounds().m_min.z));
        meshEdgeList.convexClip(Plane<>(glm::vec3(-1.0f, 0.0f, 0.0f), m_graphicsScene->getGridVolume().getVolumeBounds().m_max.x));
        meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, -1.0f, 0.0f), m_graphicsScene->getGridVolume().getVolumeBounds().m_max.y));
        meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, -1.0f), m_graphicsScene->getGridVolume().getVolumeBounds().m_max.z));

        m_graphicsScene->render(m_camera, m_viewport, &meshEdgeList);
    }
    else {
        m_graphicsScene->render(m_camera, m_viewport, NULL);
    }
    
    if(m_showPerformance) {
        m_numTraversedCellsGraph.addDataPoint(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumTraversedCells);

        m_numEmptyCellsGraph.addDataPoint(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumEmptyCells);

        m_numNonEmptyCellsGraph.addDataPoint(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumTraversedCells
            - static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumEmptyCells);

        m_numCellQueriesGraph.addDataPoint(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumQueries);

        m_numCellsUnqueriedGraph.addDataPoint(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumUnqueried);

        m_numRenderedCellsGraph.addDataPoint(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumTraversedCells
            - static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumEmptyCells
            - static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumCulledCells);

        m_numCulledCellsGraph.addDataPoint(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumCulledCells);
        
        m_cellRequeryDurationGraph.addDataPoint(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugRequeryDuration);

        m_numProcessedNodesGraph.addDataPoint(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumRenderedNodes);

        m_numOverflowedQueriesGraph.addDataPoint(static_cast<illDeferredShadingRenderer::DeferredShadingScene *>(m_graphicsScene)->m_debugNumOverflowedQueries);
    }

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
    glLoadMatrixf(glm::value_ptr(m_camera.getProjection()));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(m_camera.getModelView()));

    //debug draw the axes
    glBegin(GL_LINES);
    //x Red
        glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(5.0f, 0.0f, 0.0f);

    //y Green
        glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 5.0f, 0.0f);

    //z Blue
        glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 5.0f);
    glEnd();
    
    if(m_showPerformance) {
        illGraphics::Camera graphCam;
        graphCam.setOrthoTransform(glm::mat4(), 0.0f, m_engine->m_window->m_screenWidth, 0.0f, m_engine->m_window->m_screenHeight);

        float currY = m_engine->m_window->m_screenHeight - GRAPH_HEIGHT;
        m_numTraversedCellsGraph.render(glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

        currY -= GRAPH_HEIGHT + 5.0f;
        m_numEmptyCellsGraph.render(glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

        currY -= GRAPH_HEIGHT + 5.0f;
        m_numNonEmptyCellsGraph.render(glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

        currY -= GRAPH_HEIGHT + 5.0f;
        m_numCellQueriesGraph.render(glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

        currY -= GRAPH_HEIGHT + 5.0f;
        m_numCellsUnqueriedGraph.render(glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

        currY -= GRAPH_HEIGHT + 5.0f;
        m_numRenderedCellsGraph.render(glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

        currY -= GRAPH_HEIGHT + 5.0f;
        m_numCulledCellsGraph.render(glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

        /*currY -= GRAPH_HEIGHT + 5.0f;
        m_cellRequeryDurationGraph.render(glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);*/

        currY -= GRAPH_HEIGHT + 5.0f;
        m_numProcessedNodesGraph.render(glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

        currY -= GRAPH_HEIGHT + 5.0f;
        m_numOverflowedQueriesGraph.render(glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);
        
        m_numOverflowedQueriesGraph.m_fontRenderer->setupRender();

        int numTotalBytes = 0;

        {
            auto vol = m_graphicsScene->getGridVolume();
            int numCells = vol.getCellNumber().x * vol.getCellNumber().y * vol.getCellNumber().z;
            int numBytes = numCells * sizeof(illRendererCommon::GraphicsScene::NodeContainer);
            numTotalBytes += numBytes;

            currY -= m_numOverflowedQueriesGraph.m_fontRenderer->m_font.getLineHeight();

            m_numOverflowedQueriesGraph.m_fontRenderer->render(formatString("Scene Grid: (%u x %u x %u) (%u cells total).",
                    vol.getCellNumber().x, 
                    vol.getCellNumber().y, 
                    vol.getCellNumber().z, 
                    numCells
                ).c_str(), 
                glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

            currY -= m_numOverflowedQueriesGraph.m_fontRenderer->m_font.getLineHeight();

            m_numOverflowedQueriesGraph.m_fontRenderer->render(formatString("Dynamic Scene Cells: %u bytes per cell. %u bytes / %f megabytes total.",                    
                    sizeof(illRendererCommon::GraphicsScene::NodeContainer),
                    numBytes,
                    (float) (numBytes) / 1024.0f / 1024.0f
                ).c_str(), 
                glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

            currY -= m_numOverflowedQueriesGraph.m_fontRenderer->m_font.getLineHeight();

            numBytes = numCells * sizeof(illRendererCommon::GraphicsScene::StaticNodeContainer);
            numTotalBytes += numBytes;

            m_numOverflowedQueriesGraph.m_fontRenderer->render(formatString("Static Scene Cells: %u bytes per cell. %u bytes / %f megabytes total.",
                    sizeof(illRendererCommon::GraphicsScene::StaticNodeContainer),
                    numBytes,
                    (float) (numBytes) / 1024.0f / 1024.0f
                ).c_str(), 
                glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);
        }

        {
            auto vol = m_graphicsScene->getInteractionGridVolume();
            int numCells = vol.getCellNumber().x * vol.getCellNumber().y * vol.getCellNumber().z;
            int numBytes = numCells * sizeof(illRendererCommon::GraphicsScene::LightNodeContainer);
            numTotalBytes += numBytes;

            currY -= m_numOverflowedQueriesGraph.m_fontRenderer->m_font.getLineHeight();

            m_numOverflowedQueriesGraph.m_fontRenderer->render(formatString("Interaction Grid: (%u x %u x %u) (%u total).",
                    vol.getCellNumber().x, 
                    vol.getCellNumber().y, 
                    vol.getCellNumber().z, 
                    numCells                    
                ).c_str(), 
                glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

            currY -= m_numOverflowedQueriesGraph.m_fontRenderer->m_font.getLineHeight();

            m_numOverflowedQueriesGraph.m_fontRenderer->render(formatString("Dynamic Light Cells: %u bytes per cell. %u bytes / %f megabytes total.",                    
                    sizeof(illRendererCommon::GraphicsScene::NodeContainer),
                    numBytes,
                    (float) (numBytes) / 1024.0f / 1024.0f
                ).c_str(), 
                glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);

            currY -= m_numOverflowedQueriesGraph.m_fontRenderer->m_font.getLineHeight();

            numBytes = numCells * sizeof(illRendererCommon::GraphicsScene::StaticLightNodeContainer);
            numTotalBytes += numBytes;

            m_numOverflowedQueriesGraph.m_fontRenderer->render(formatString("Static Light Cells: %u bytes per cell. %u bytes / %f megabytes total.",
                    sizeof(illRendererCommon::GraphicsScene::StaticNodeContainer),
                    numBytes,
                    (float) (numBytes) / 1024.0f / 1024.0f
                ).c_str(), 
                glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);
        }

        currY -= m_numOverflowedQueriesGraph.m_fontRenderer->m_font.getLineHeight();

        m_numOverflowedQueriesGraph.m_fontRenderer->render(formatString("Total Cell Memory: %u bytes / %f megabytes.",
                    numTotalBytes,
                    (float) (numTotalBytes) / 1024.0f / 1024.0f
                ).c_str(), 
                glm::translate(glm::vec3(0.0f, currY, 0.0f)), graphCam);
    }

    if(m_occlusionDebug) {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(m_occlusionCamera.getProjection()));

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(m_occlusionCamera.getModelView()));

        glViewport(m_camera.getViewportCorner().x, m_camera.getViewportCorner().y,
            m_camera.getViewportDimensions().x, m_camera.getViewportDimensions().y / 2);

        //debug draw the axes
        glBegin(GL_LINES);
        //x Red
            glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(5.0f, 0.0f, 0.0f);

        //y Green
            glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 5.0f, 0.0f);

        //z Blue
            glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 5.0f);
        glEnd();

        glEnable(GL_DEPTH_TEST);

        /*if(m_drawGrid) {
            renderSceneDebug(m_graphicsScene->getGridVolume());
        }*/

        //clip the mesh against the bounds
        /*if(m_drawFrustum) {
            MeshEdgeList<> meshEdgeList = m_camera.getViewFrustum().getMeshEdgeList();

            meshEdgeList.convexClip(Plane<>(glm::vec3(1.0f, 0.0f, 0.0f), -m_graphicsScene->getGridVolume().getVolumeBounds().m_min.x));
            meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 1.0f, 0.0f), -m_graphicsScene->getGridVolume().getVolumeBounds().m_min.y));
            meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, 1.0f), -m_graphicsScene->getGridVolume().getVolumeBounds().m_min.z));
            meshEdgeList.convexClip(Plane<>(glm::vec3(-1.0f, 0.0f, 0.0f), m_graphicsScene->getGridVolume().getVolumeBounds().m_max.x));
            meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, -1.0f, 0.0f), m_graphicsScene->getGridVolume().getVolumeBounds().m_max.y));
            meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, -1.0f), m_graphicsScene->getGridVolume().getVolumeBounds().m_max.z));

            renderMeshEdgeListDebug(meshEdgeList);
        }*/
    }
}

}
