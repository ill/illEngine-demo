#include "Graph.h"
#include "illEngine/Graphics/serial/Camera/Camera.h"

//TODO: move OpenGL code out of here.  That belongs in the renderer backend.
#include "illEngine/GlCommon/glLogging.h"
#include "illEngine/GlCommon/serial/glUtil.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CrappyBmFontRenderer.h"

void Graph::render(const glm::mat4& transform, illGraphics::Camera& camera) {
    glUseProgram(0);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(camera.getProjection()));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(camera.getModelView() * transform));
    
    //render background, TODO: immediate mode, problem?  Trollol.
    glBegin(GL_QUADS);

        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);

        glVertex2f(0.0f, 0.0f);
        glVertex2f(GRAPH_WIDTH, 0.0f);

        glColor4f(0.5f, 0.5f, 0.5f, 0.1f);

        glVertex2f(GRAPH_WIDTH, GRAPH_HEIGHT);
        glVertex2f(0.0f, GRAPH_HEIGHT);

    glEnd();

    //render line graph
    glBegin(GL_LINE_STRIP);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    //find max, a bit inefficient...
    float average = m_total / (float) m_numPoints;
    float max = average;

    {
        for(auto pointsIter = m_dataPoints.begin(); pointsIter != m_dataPoints.end(); pointsIter++) {
            if(*pointsIter > max) {
                max = *pointsIter;
            }
        }

        float xIncr = GRAPH_WIDTH / (float) m_maxData * 0.5f;
        float currX = 0.0f;

        if(max != 0.0f) {
            for(auto pointsIter = m_dataPoints.begin(); pointsIter != m_dataPoints.end(); pointsIter++, currX += xIncr) {
                glVertex2f(currX, ((*pointsIter) / max) * GRAPH_HEIGHT);
                currX += xIncr;
            }
        }
        else {
            glVertex2f(0.0f, 0.0f);
            glVertex2f(GRAPH_WIDTH, 0.0f);
        }
    }

    glEnd();
        
    //render average
    glBegin(GL_QUADS);

        float avgPt = max == 0.0f
            ? 0.0f
            : (average / max) * GRAPH_HEIGHT;

        glColor4f(1.0f, 1.0f, 0.0f, 0.1f);

        glVertex2f(0.0f, 0.0f);
        glVertex2f(GRAPH_WIDTH, 0.0f);
        glVertex2f(GRAPH_WIDTH, avgPt);
        glVertex2f(0.0f, avgPt);

    glEnd();

    m_fontRenderer->setupRender();
    m_fontRenderer->render(formatString("Max: %f", max).c_str(), transform * glm::translate(glm::vec3(0.0f, GRAPH_HEIGHT * 0.5f + m_fontRenderer->m_font.getLineHeight(), 0.0f)), camera);
    m_fontRenderer->render(formatString("%s: %f", m_name.c_str(), m_dataPoints.back()).c_str(), transform * glm::translate(glm::vec3(GRAPH_WIDTH, GRAPH_HEIGHT * 0.5f + m_fontRenderer->m_font.getLineHeight(), 0.0f)), camera);
    m_fontRenderer->render(formatString("^3Average: %f", average).c_str(), transform * glm::translate(glm::vec3(GRAPH_WIDTH, m_fontRenderer->m_font.getLineHeight(), 0.0f)), camera);
}