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

void setupTestFrustumIterator(ConvexMeshIterator<>& convexMeshIterator, const illGraphics::Camera& camera, MeshEdgeList<>& meshEdgeList) {
    //set up test mesh edge list
    meshEdgeList.clear();

    //the edges
    for(unsigned int edge = 0; edge < 12; edge++) {
        meshEdgeList.m_edges.push_back(MeshEdgeList<>::Edge(FRUSTUM_EDGE_LIST[edge][0], FRUSTUM_EDGE_LIST[edge][1]));
    }

    //the points
    for(unsigned int point = 0; point < 8; point++) {
        meshEdgeList.m_points.push_back(camera.getViewFrustum().m_points[point]);
    }

    meshEdgeList.computePointEdgeMap();
    
    //clip the mesh edge list against some planes
    meshEdgeList.convexClip(Plane<>(glm::vec3(1.0f, 0.0f, 0.0f), 2000.0f));
    meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 1.0f, 0.0f), 2000.0f));
    meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, 1.0f), 2000.0f));

    meshEdgeList.convexClip(Plane<>(glm::vec3(-1.0f, 0.0f, 0.0f), 1999.9999f));
    meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, -1.0f, 0.0f), 1999.9999f));
    meshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, -1.0f), 1999.9999f));
    
    if(!meshEdgeList.m_points.empty()) {
        meshEdgeList.computeBounds();

        //get intersection of frustum and bounds
        Box<int> iterBounds(glm::ivec3(-40), glm::ivec3(40));
        Box<int> frustumGrid(meshEdgeList.m_bounds.grid<int>(glm::vec3(50.0f)));

        if(iterBounds.intersects(frustumGrid)) {
            iterBounds.constrain(frustumGrid);

            //create a copy of the clipped mesh
            convexMeshIterator.initialize(&meshEdgeList, 
                camera.getViewFrustum().m_direction, 
                frustumGrid,
                glm::vec3(50.0f));
        }
    }
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
    m_cameraTransform.m_transform = m_cameraController.m_transform;
    m_camera.setTransform(m_cameraTransform, m_engine->m_window->getAspectRatio(), illGraphics::DEFAULT_FOV * m_cameraController.m_zoom, illGraphics::DEFAULT_NEAR, 1000.0f, m_cameraController.m_orthoMode);
    
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
    
    ConvexMeshIterator<> meshIterator;
    MeshEdgeList<> meshEdgeList;
        
    setupTestFrustumIterator(meshIterator, m_hold ? m_testFrustumCamera : m_camera, meshEdgeList);
    
    glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
    glPointSize(5.0f);

    glBegin(GL_POINTS);

    while(!meshIterator.atEnd()) {
        glVertex3fv(glm::value_ptr(vec3cast<int, glm::mediump_float>(meshIterator.getCurrentPosition()) * glm::vec3(50.0f)));

        meshIterator.forward();
    }

    glEnd();

    glDepthMask(GL_TRUE);

    ERROR_CHECK_OPENGL;
}

}
