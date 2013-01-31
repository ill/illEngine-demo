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

void renderSceneDebug(const GridVolume3D<>& gridVolume) {
    //render cells, including some outside of the scene
    glColor4f(1.0f, 1.0f, 1.0f, 0.05f);
    glBegin(GL_LINES);
    for (int z = -3; z <= (int)gridVolume.getCellNumber().z + 3; z++) {
        for (int x = -3; x <= (int)gridVolume.getCellNumber().x + 3; x ++) {
            glVertex3f(gridVolume.getCellDimensions().x * x, -gridVolume.getCellDimensions().y * 3, gridVolume.getCellDimensions().z * z);
            glVertex3f(gridVolume.getCellDimensions().x * x, gridVolume.getVolumeBounds().m_max.y + gridVolume.getCellDimensions().y * 3, gridVolume.getCellDimensions().z * z);
        }
    }

    for (int z = -3; z <= (int)gridVolume.getCellNumber().z + 3; z++) {
        for (int y = -3; y <= (int)gridVolume.getCellNumber().y + 3; y++) {
            glVertex3f(-gridVolume.getCellDimensions().x * 3, gridVolume.getCellDimensions().y * y, gridVolume.getCellDimensions().z * z);
            glVertex3f(gridVolume.getVolumeBounds().m_max.x + gridVolume.getCellDimensions().x * 3, gridVolume.getCellDimensions().y * y, gridVolume.getCellDimensions().z * z);
        }
    }

    for (int x = -3; x <= (int)gridVolume.getCellNumber().x + 3; x++) {
        for (int y = -3; y <= (int)gridVolume.getCellNumber().y + 3; y++) {
            glVertex3f(gridVolume.getCellDimensions().x * x, gridVolume.getCellDimensions().y * y, -gridVolume.getCellDimensions().z * 3);
            glVertex3f(gridVolume.getCellDimensions().x * x, gridVolume.getCellDimensions().y * y, gridVolume.getVolumeBounds().m_max.z + gridVolume.getCellDimensions().z * 3);
        }
    }
    glEnd();

    //render scene bounds
    glColor4f(1.0f, 1.0f, 0.0f, .25);

    //near
    glBegin(GL_LINE_LOOP);      
    glVertex3f(gridVolume.getVolumeBounds().m_min.x, gridVolume.getVolumeBounds().m_min.y, gridVolume.getVolumeBounds().m_min.z);
    glVertex3f(gridVolume.getVolumeBounds().m_max.x, gridVolume.getVolumeBounds().m_min.y, gridVolume.getVolumeBounds().m_min.z);
    glVertex3f(gridVolume.getVolumeBounds().m_max.x, gridVolume.getVolumeBounds().m_max.y, gridVolume.getVolumeBounds().m_min.z);
    glVertex3f(gridVolume.getVolumeBounds().m_min.x, gridVolume.getVolumeBounds().m_max.y, gridVolume.getVolumeBounds().m_min.z);
    glEnd();

    //far
    glBegin(GL_LINE_LOOP);      
    glVertex3f(gridVolume.getVolumeBounds().m_min.x, gridVolume.getVolumeBounds().m_min.y, gridVolume.getVolumeBounds().m_max.z);
    glVertex3f(gridVolume.getVolumeBounds().m_max.x, gridVolume.getVolumeBounds().m_min.y, gridVolume.getVolumeBounds().m_max.z);
    glVertex3f(gridVolume.getVolumeBounds().m_max.x, gridVolume.getVolumeBounds().m_max.y, gridVolume.getVolumeBounds().m_max.z);
    glVertex3f(gridVolume.getVolumeBounds().m_min.x, gridVolume.getVolumeBounds().m_max.y, gridVolume.getVolumeBounds().m_max.z);
    glEnd();

    //connect
    glBegin(GL_LINES);      
    glVertex3f(gridVolume.getVolumeBounds().m_min.x, gridVolume.getVolumeBounds().m_min.y, gridVolume.getVolumeBounds().m_min.z);
    glVertex3f(gridVolume.getVolumeBounds().m_min.x, gridVolume.getVolumeBounds().m_min.y, gridVolume.getVolumeBounds().m_max.z);

    glVertex3f(gridVolume.getVolumeBounds().m_max.x, gridVolume.getVolumeBounds().m_min.y, gridVolume.getVolumeBounds().m_min.z);
    glVertex3f(gridVolume.getVolumeBounds().m_max.x, gridVolume.getVolumeBounds().m_min.y, gridVolume.getVolumeBounds().m_max.z);

    glVertex3f(gridVolume.getVolumeBounds().m_max.x, gridVolume.getVolumeBounds().m_max.y, gridVolume.getVolumeBounds().m_min.z);
    glVertex3f(gridVolume.getVolumeBounds().m_max.x, gridVolume.getVolumeBounds().m_max.y, gridVolume.getVolumeBounds().m_max.z);

    glVertex3f(gridVolume.getVolumeBounds().m_min.x, gridVolume.getVolumeBounds().m_max.y, gridVolume.getVolumeBounds().m_min.z);
    glVertex3f(gridVolume.getVolumeBounds().m_min.x, gridVolume.getVolumeBounds().m_max.y, gridVolume.getVolumeBounds().m_max.z);
    glEnd();
}

void renderMeshEdgeListDebug(const MeshEdgeList<>& edgeList) {
    glPointSize(5.0f);
    
    //unclipped

    //all the points
    glPointSize(5.0f);

    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    glBegin(GL_POINTS);

    for(size_t point = 0; point < edgeList.m_points.size(); point++) {
        glVertex3fv(glm::value_ptr(edgeList.m_points[point]));
    }

    glEnd();

    //all the lines
    glLineWidth(1.0f);

    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

    glBegin(GL_LINES);

    for(size_t edge = 0; edge < edgeList.m_edges.size(); edge++) {
        glVertex3fv(glm::value_ptr(edgeList.m_points[edgeList.m_edges[edge].m_point[0]]));
        glVertex3fv(glm::value_ptr(edgeList.m_points[edgeList.m_edges[edge].m_point[1]]));
    }

    glEnd();
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

    m_cameraController.m_transform = createTransform(glm::vec3(5000.0f));

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
        m_hold ? 5000 : 1500);
    
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
        
    {
        GridVolume3D<> tempGrid(glm::vec3(50.0f), glm::uvec3(10000));

        //debug draw the frustum iterators
        //renderSceneDebug(tempGrid);

        MeshEdgeList<> meshEdgeList = m_hold 
            ? m_testFrustumCamera.getViewFrustum().getMeshEdgeList() 
            : m_camera.getViewFrustum().getMeshEdgeList();
    
        MeshEdgeList<> meshEdgeListRenderCopy = meshEdgeList;

        //clip the mesh against the bounds
        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(1.0f, 0.0f, 0.0f), -tempGrid.getVolumeBounds().m_min.x));
        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(0.0f, 1.0f, 0.0f), -tempGrid.getVolumeBounds().m_min.y));
        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, 1.0f), -tempGrid.getVolumeBounds().m_min.z));

        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(-1.0f, 0.0f, 0.0f), tempGrid.getVolumeBounds().m_max.x));
        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(0.0f, -1.0f, 0.0f), tempGrid.getVolumeBounds().m_max.y));
        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, -1.0f), tempGrid.getVolumeBounds().m_max.z));

        renderMeshEdgeListDebug(meshEdgeListRenderCopy);

        ConvexMeshIterator<> meshIterator = tempGrid.meshIteratorForMesh(&meshEdgeList, m_hold ? m_testFrustumCamera.getViewFrustum().m_direction : m_camera.getViewFrustum().m_direction);
        
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
        glPointSize(1.0f);

        glBegin(GL_POINTS);

        while(!meshIterator.atEnd()) {
            glVertex3fv(glm::value_ptr(vec3cast<unsigned int, glm::mediump_float>(meshIterator.getCurrentPosition()) * tempGrid.getCellDimensions()));

            meshIterator.forward();
        }

        glEnd();
    }
        
    //////////////
    //top down view
    glViewport(0, 0, m_engine->m_window->getResolution().x, m_engine->m_window->getResolution().y / 2);

    illGraphics::Camera topCamera;
    topCamera.setOrthoTransform(createTransform(glm::vec3(5000.0f, 10000.0f, 5000.0f), directionToMat3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
        -5000.0f * m_engine->m_window->getAspectRatio() * 2.0f, 5000.0f * m_engine->m_window->getAspectRatio() * 2.0f, -5000.0f, 5000.0f, -20000.0f, 20000.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(topCamera.getProjection()));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(topCamera.getModelView()));

    {
        GridVolume3D<> tempGrid(glm::vec3(50.0f), glm::uvec3(10000));

        //renderSceneDebug(tempGrid);

        MeshEdgeList<> meshEdgeList = m_hold 
            ? m_testFrustumCamera.getViewFrustum().getMeshEdgeList() 
            : m_camera.getViewFrustum().getMeshEdgeList();
    
        MeshEdgeList<> meshEdgeListRenderCopy = meshEdgeList;

        //clip the mesh against the bounds
        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(1.0f, 0.0f, 0.0f), -tempGrid.getVolumeBounds().m_min.x));
        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(0.0f, 1.0f, 0.0f), -tempGrid.getVolumeBounds().m_min.y));
        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, 1.0f), -tempGrid.getVolumeBounds().m_min.z));

        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(-1.0f, 0.0f, 0.0f), tempGrid.getVolumeBounds().m_max.x));
        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(0.0f, -1.0f, 0.0f), tempGrid.getVolumeBounds().m_max.y));
        meshEdgeListRenderCopy.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, -1.0f), tempGrid.getVolumeBounds().m_max.z));

        renderMeshEdgeListDebug(meshEdgeListRenderCopy);

        ConvexMeshIterator<> meshIterator = tempGrid.meshIteratorForMesh(&meshEdgeList, m_hold ? m_testFrustumCamera.getViewFrustum().m_direction : m_camera.getViewFrustum().m_direction);
                
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
        glPointSize(1.0f);

        glBegin(GL_POINTS);

        while(!meshIterator.atEnd()) {
            glVertex3fv(glm::value_ptr(vec3cast<unsigned int, glm::mediump_float>(meshIterator.getCurrentPosition()) * tempGrid.getCellDimensions()));

            meshIterator.forward();
        }

        glEnd();
    }

    glDepthMask(GL_TRUE);

    //put viewport back
    glViewport(0, 0, m_engine->m_window->getResolution().x, m_engine->m_window->getResolution().y);

    ERROR_CHECK_OPENGL;
}

}
