#include <set>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Engine.h"
#include "illEngine/Graphics/Window.h"

#include "FrustumCullVisualizerController.h"
#include "illEngine/Util/Illmesh/IllmeshLoader.h"
#include "illEngine/Graphics/serial/Material/Shader.h"
#include "illEngine/Graphics/serial/BitmapFont.h"
#include "illEngine/Input/serial/InputManager.h"

#include "illEngine/GlCommon/glLogging.h"
#include "illEngine/GlCommon/serial/glUtil.h"

#include "illEngine/Util/Geometry/GridVolume3D.h"

//TODO: for now I'm testing a bunch of stuff, normally all rendering is done through the renderer
#include <GL/glew.h>

void renderFrustumSceneDebug(const Box<>&sceneBounds, const glm::vec3& chunkDimensions, const glm::uvec3& chunkNumber) {
    //render cells, including some outside of the scene
    glColor4f(1.0f, 1.0f, 1.0f, 0.15f);
    glBegin(GL_LINES);
    for (int z = -3; z <= (int)chunkNumber.z + 3; z++) {
        for (int x = -3; x <= (int)chunkNumber.x + 3; x ++) {
            glVertex3f(chunkDimensions.x * x, -chunkDimensions.y * 3, chunkDimensions.z * z);
            glVertex3f(chunkDimensions.x * x, sceneBounds.m_max.y + chunkDimensions.y * 3, chunkDimensions.z * z);
        }
    }

    for (int z = -3; z <= (int)chunkNumber.z + 3; z++) {
        for (int y = -3; y <= (int)chunkNumber.y + 3; y++) {
            glVertex3f(-chunkDimensions.x * 3, chunkDimensions.y * y, chunkDimensions.z * z);
            glVertex3f(sceneBounds.m_max.x + chunkDimensions.x * 3, chunkDimensions.y * y, chunkDimensions.z * z);
        }
    }

    for (int x = -3; x <= (int)chunkNumber.x + 3; x++) {
        for (int y = -3; y <= (int)chunkNumber.y + 3; y++) {
            glVertex3f(chunkDimensions.x * x, chunkDimensions.y * y, -chunkDimensions.z * 3);
            glVertex3f(chunkDimensions.x * x, chunkDimensions.y * y, sceneBounds.m_max.z + chunkDimensions.z * 3);
        }
    }
    glEnd();

    //render scene bounds
    glColor4f(1.0f, 1.0f, 0.0f, .25);

    //near
    glBegin(GL_LINE_LOOP);      
    glVertex3f(sceneBounds.m_min.x, sceneBounds.m_min.y, sceneBounds.m_min.z);
    glVertex3f(sceneBounds.m_max.x, sceneBounds.m_min.y, sceneBounds.m_min.z);
    glVertex3f(sceneBounds.m_max.x, sceneBounds.m_max.y, sceneBounds.m_min.z);
    glVertex3f(sceneBounds.m_min.x, sceneBounds.m_max.y, sceneBounds.m_min.z);
    glEnd();

    //far
    glBegin(GL_LINE_LOOP);      
    glVertex3f(sceneBounds.m_min.x, sceneBounds.m_min.y, sceneBounds.m_max.z);
    glVertex3f(sceneBounds.m_max.x, sceneBounds.m_min.y, sceneBounds.m_max.z);
    glVertex3f(sceneBounds.m_max.x, sceneBounds.m_max.y, sceneBounds.m_max.z);
    glVertex3f(sceneBounds.m_min.x, sceneBounds.m_max.y, sceneBounds.m_max.z);
    glEnd();

    //connect
    glBegin(GL_LINES);      
    glVertex3f(sceneBounds.m_min.x, sceneBounds.m_min.y, sceneBounds.m_min.z);
    glVertex3f(sceneBounds.m_min.x, sceneBounds.m_min.y, sceneBounds.m_max.z);

    glVertex3f(sceneBounds.m_max.x, sceneBounds.m_min.y, sceneBounds.m_min.z);
    glVertex3f(sceneBounds.m_max.x, sceneBounds.m_min.y, sceneBounds.m_max.z);

    glVertex3f(sceneBounds.m_max.x, sceneBounds.m_max.y, sceneBounds.m_min.z);
    glVertex3f(sceneBounds.m_max.x, sceneBounds.m_max.y, sceneBounds.m_max.z);

    glVertex3f(sceneBounds.m_min.x, sceneBounds.m_max.y, sceneBounds.m_min.z);
    glVertex3f(sceneBounds.m_min.x, sceneBounds.m_max.y, sceneBounds.m_max.z);
    glEnd();

    glDisable(GL_BLEND);
}

namespace Demo {
    
FrustumCullVisualizerController::FrustumCullVisualizerController(Engine * engine)
    : GameControllerBase(),
    m_engine(engine),
    m_hold(false)
{
    //This is all put together to test some stuff, this is in no way how to normally do these things.  Everything should normally be done through the renderer front end when that's done.
    
    //initialize the input (this would normally initialize using console variables)    
    m_holdFrustumIteratorCallback.m_controller = this;

    m_holdFrustumIterator.m_inputCallback = &m_holdFrustumIteratorCallback;
    
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_END), &m_holdFrustumIterator);
    
    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_cameraController.m_inputContext);
    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_frustumInputContext);

    m_cameraController.m_speed = 50.0f;
    m_cameraController.m_rollSpeed = 50.0f;
}

FrustumCullVisualizerController::~FrustumCullVisualizerController() {
    m_engine->m_inputManager->getInputContextStack(0)->popInputContext();
    m_engine->m_inputManager->getInputContextStack(0)->popInputContext();
}

void FrustumCullVisualizerController::update(float seconds) {
    m_cameraController.update(seconds);        
}

void FrustumCullVisualizerController::updateSound(float seconds) {

}
 
void FrustumCullVisualizerController::render() {
    //render top portion
    m_camera.setPerspectiveTransform(m_cameraController.m_transform, m_engine->m_window->getAspectRatio() * 2.0f, illGraphics::DEFAULT_FOV * m_cameraController.m_zoom, illGraphics::DEFAULT_NEAR,
        m_hold ? 5000 : illGraphics::DEFAULT_FAR);
    
    glViewport(0, m_engine->m_window->getResolution().y / 2, m_engine->m_window->getResolution().x, m_engine->m_window->getResolution().y / 2);

    //TODO: for now I'm testing a bunch of stuff, normally all rendering is done through the renderer   

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);    
    glDisable(GL_BLEND);
        
    //draw a font
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //debug drawing
    glUseProgram(0);
        
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
    
    //debug draw the frustum iterators
    //renderSceneDebug(Box<>(glm::vec3(3.0f * -20.0f), glm::vec3(3.0f * 20.0f - 0.1f)), glm::vec3(20.0f), glm::uvec3(6));

    /*if(m_testFrustumIter) {
        renderFrustumIterDebug(m_testFrustumIter->m_debugger, m_mapToWorld, m_camera, m_fontShader, m_debugFont);
    }*/
    
    {
        GridVolume3D<> tempGrid(glm::vec3(50.0f), glm::uvec3(80));

        MeshEdgeList<> meshEdgeList = m_hold 
            ? m_testFrustumCamera.getViewFrustum().getMeshEdgeList() 
            : m_camera.getViewFrustum().getMeshEdgeList();
    
        ConvexMeshIterator<> meshIterator = tempGrid.meshIteratorForMesh(&meshEdgeList, m_hold ? m_testFrustumCamera.getViewFrustum().m_direction : m_camera.getViewFrustum().m_direction);
        
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
        glPointSize(5.0f);

        glBegin(GL_POINTS);

        while(!meshIterator.atEnd()) {
            glVertex3fv(glm::value_ptr(vec3cast<unsigned int, glm::mediump_float>(meshIterator.getCurrentPosition()) * glm::vec3(50.0f)));

            meshIterator.forward();
        }
    }

    glEnd();
    
    //////////////
    //top down view
    glViewport(0, 0, m_engine->m_window->getResolution().x, m_engine->m_window->getResolution().y / 2);

    illGraphics::Camera topCamera;
    topCamera.setOrthoTransform(createTransform(glm::vec3(0.0f, 6000.0f, 0.0f), directionToMat3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
        -2000.0f, 2000.0f, -2000.0f * m_engine->m_window->getAspectRatio() * 0.5f, 2000.0f * m_engine->m_window->getAspectRatio() * 0.5f, 0.0f, 12000.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(topCamera.getProjection()));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(topCamera.getModelView()));

    {
        GridVolume3D<> tempGrid(glm::vec3(50.0f), glm::uvec3(80));

        MeshEdgeList<> meshEdgeList = m_hold 
            ? m_testFrustumCamera.getViewFrustum().getMeshEdgeList() 
            : m_camera.getViewFrustum().getMeshEdgeList();
    
        ConvexMeshIterator<> meshIterator = tempGrid.meshIteratorForMesh(&meshEdgeList, m_hold ? m_testFrustumCamera.getViewFrustum().m_direction : m_camera.getViewFrustum().m_direction);
        
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
        glPointSize(5.0f);

        glBegin(GL_POINTS);

        while(!meshIterator.atEnd()) {
            glVertex3fv(glm::value_ptr(vec3cast<unsigned int, glm::mediump_float>(meshIterator.getCurrentPosition()) * glm::vec3(50.0f)));

            meshIterator.forward();
        }
    }

    glDepthMask(GL_TRUE);

    ERROR_CHECK_OPENGL;
}

}
