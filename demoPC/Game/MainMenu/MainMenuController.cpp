#include <set>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Engine.h"
#include "illEngine/Graphics/Window.h"

#include "MainMenuController.h"
#include "illEngine/Util/Illmesh/IllmeshLoader.h"
#include "illEngine/Graphics/serial/Material/Shader.h"
#include "illEngine/Graphics/serial/BitmapFont.h"
#include "illEngine/Input/serial/InputManager.h"

#include "illEngine/GlCommon/glLogging.h"
#include "illEngine/GlCommon/serial/glUtil.h"

//TODO: for now I'm testing a bunch of stuff, normally all rendering is done through the renderer
#include <GL/glew.h>

enum FontHorzAlign {
    FN_H_LEFT, FN_H_CENTER, FN_H_RIGHT
};

enum FontVertAlign {
    FN_V_TOP, FN_V_CENTER, FN_V_BOTTOM
};

void renderTextDebug(const char * text, const glm::mat4& transform, const illGraphics::BitmapFont& font, 
        const illGraphics::Camera& camera, GLuint prog, 
        FontHorzAlign horzAlign = FN_H_LEFT, FontVertAlign vertAlign = FN_V_TOP) {
    glm::vec4 currColor = glm::vec4(1.0f);  //set the color to white initially

    glm::mat4 currentTransform = transform;

    GLuint buffer = *((GLuint *) font.getMesh().getMeshBackendData() + 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    GLint pos = getProgramAttribLocation(prog, "position");
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, (GLsizei) font.getMesh().m_meshFrontendData->getVertexSize(), (char *)NULL + font.getMesh().m_meshFrontendData->getPositionOffset());
    glEnableVertexAttribArray(pos);

    GLint tex = getProgramAttribLocation(prog, "texCoords");
    glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, (GLsizei) font.getMesh().m_meshFrontendData->getVertexSize(), (char *)NULL + font.getMesh().m_meshFrontendData->getTexCoordOffset());
    glEnableVertexAttribArray(tex);

    buffer = *((GLuint *) font.getMesh().getMeshBackendData() + 1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    
    if(horzAlign != FN_H_LEFT || vertAlign != FN_V_TOP)  {
        glm::vec2 printDimensions = font.getPrintDimensions(text);
        glm::vec2 translate;

        switch (vertAlign) {
        case FN_V_CENTER:
            translate.y = printDimensions.y * 0.5f;            
            break;

        case FN_V_BOTTOM:
            translate.y = printDimensions.y;
            break;

        default:
            translate.y = 0;
            break;
        }

        switch (horzAlign) {
        case FN_H_CENTER: {
                const char ** textPtr = &text;
                translate.x = font.getLineWidth(textPtr) * 0.5f;
            }
            break;

        case FN_H_RIGHT: {
                const char ** textPtr = &text;
                translate.x = font.getLineWidth(textPtr);
            }
            break;

        default:
            translate.x = 0;
        }

        currentTransform = glm::translate(transform, glm::vec3(translate, 0.0f));
    }

    while(*text) {
        //check if color code
        if(font.getColorCode(&text, currColor)) {
            continue;
        }

        //parse special characters
        switch (*text) {
        case '\n': {    //newline
            glm::vec2 translate;
            
            text++;

            glm::vec3 currentPosition = getTransformPosition(currentTransform);

            switch (horzAlign) {
            case FN_H_CENTER: {
                    const char ** textPtr = &text;
                    translate.x = font.getLineWidth(textPtr) * 0.5f;
                }
                break;

            case FN_H_RIGHT: {
                    const char ** textPtr = &text;
                    translate.x = font.getLineWidth(textPtr);
                }
                break;

            default:
                translate.x = 0;
            }

            translate.y = currentPosition.y - font.getLineHeight();

            currentTransform = glm::translate(transform, glm::vec3(translate, 0.0f));

            continue;
        }

        case ' ': //space
            currentTransform = glm::translate(currentTransform, glm::vec3(/*font.getSpacingHorz()*/5.0f, 0.0f, 0.0f));
            text++;
            continue;

        case '\t': //tab
            currentTransform = glm::translate(currentTransform, glm::vec3(font.getSpacingHorz() * 4.0f, 0.0f, 0.0f));
            text++;
            continue;
        }

        //m_backend->renderCharacter(camera, transform, font, currColor, *text);
        {
            GLint mvp = getProgramUniformLocation(prog, "modelViewProjectionMatrix");
            glUniformMatrix4fv(mvp, 1, false, glm::value_ptr(camera.getCanonical() * currentTransform));
        }

        {
            GLint color = getProgramUniformLocation(prog, "color");
            glUniform4fv(color, 1, glm::value_ptr(currColor));
        }

        {
            GLuint texture = *((GLuint *) font.getPageTexture(font.getCharData(*text).m_texturePage).getTextureData());
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        
        if(font.getCharData(*text).m_advance != 0.0f) {
            glDrawRangeElements(GL_TRIANGLES, 
                font.getCharData(*text).m_meshIndex, font.getCharData(*text).m_meshIndex + 6, 6, 
                GL_UNSIGNED_SHORT, (char *)NULL + font.getCharData(*text).m_meshIndex * sizeof(uint16_t));
            
            currentTransform = glm::translate(currentTransform, glm::vec3(font.getCharData(*text).m_advance, 0.0f, 0.0f));
        }

        text++;
    }
    
    glDisableVertexAttribArray(pos);
    glDisableVertexAttribArray(tex);
}

void renderMeshEdgeListDebug(const MeshEdgeList<>& edgeList) {
    glPointSize(5.0f);
    
    //unclipped

    //all the points
    glPointSize(5.0f);

    glColor4f(1.0f, 0.0f, 0.0f, 0.1f);

    glBegin(GL_POINTS);

    for(size_t point = 0; point < edgeList.m_points.size(); point++) {
        glVertex3fv(glm::value_ptr(edgeList.m_points[point]));
    }

    glEnd();

    //all the lines
    glLineWidth(1.0f);

    glColor4f(0.0f, 1.0f, 0.0f, 0.1f);

    glBegin(GL_LINES);

    for(size_t edge = 0; edge < edgeList.m_edges.size(); edge++) {
        glVertex3fv(glm::value_ptr(edgeList.m_points[edgeList.m_edges[edge].m_point[0]]));
        glVertex3fv(glm::value_ptr(edgeList.m_points[edgeList.m_edges[edge].m_point[1]]));
    }

    glEnd();
}

void renderSceneDebug(const Box<>&sceneBounds, const glm::vec3& chunkDimensions, const glm::uvec3& chunkNumber) {
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

void renderFrustumIterDebug(const ConvexMeshIterator<>::Debugger& iterator, const illGraphics::Camera& camera, const illGraphics::ShaderProgram& fontShader, const illGraphics::BitmapFont& font) {
    glUseProgram(0);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);   
    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);   
    glLoadMatrixf(glm::value_ptr(camera.getProjection()));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(camera.getModelView()));

    glm::vec3 drawVec;

    //render mesh
    glLineWidth(5.0f);

    glBegin(GL_LINES);

    //inactive edges
    glColor4f(1.0f, 1.0f, 0.0f, 0.25f);

    for(size_t edge = 0; edge < iterator.m_iterator->m_meshEdgeList->m_edges.size(); edge++) {
        if(!iterator.m_iterator->m_isEdgeChecked[edge]) {
            glVertex3fv(glm::value_ptr(iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[edge].m_point[0]]));
            glVertex3fv(glm::value_ptr(iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[edge].m_point[1]]));
        }
    }
    
    //active edges
    glColor4f(0.0f, 1.0f, 0.0f, 0.25f);

    for(std::unordered_map<size_t, unsigned int>::const_iterator iter = iterator.m_iterator->m_activeEdges.begin(); iter != iterator.m_iterator->m_activeEdges.end(); iter++) {
        glVertex3fv(glm::value_ptr(iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[iter->first].m_point[0]]));
        glVertex3fv(glm::value_ptr(iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[iter->first].m_point[1]]));
    }

    //discarded edges
    glColor4f(1.0f, 0.0f, 0.0f, 0.25f);

    for(std::unordered_set<size_t>::const_iterator iter = iterator.m_discarededEdges.begin(); iter != iterator.m_discarededEdges.end(); iter++) {
        glVertex3fv(glm::value_ptr(iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[*iter].m_point[0]]));
        glVertex3fv(glm::value_ptr(iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[*iter].m_point[1]]));
    }

    glEnd();

    glLineWidth(1.0f);

    //the frustum bounds
    glColor4f(1.0f, 1.0f, 1.0f, 0.25f);

    glBegin(GL_LINE_LOOP);

    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_min.x, iterator.m_meshEdgeList.m_bounds.m_min.y, iterator.m_meshEdgeList.m_bounds.m_min.z);
    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_max.x, iterator.m_meshEdgeList.m_bounds.m_min.y, iterator.m_meshEdgeList.m_bounds.m_min.z);
    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_max.x, iterator.m_meshEdgeList.m_bounds.m_max.y, iterator.m_meshEdgeList.m_bounds.m_min.z);
    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_min.x, iterator.m_meshEdgeList.m_bounds.m_max.y, iterator.m_meshEdgeList.m_bounds.m_min.z);

    glEnd();

    glBegin(GL_LINE_LOOP);

    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_min.x, iterator.m_meshEdgeList.m_bounds.m_min.y, iterator.m_meshEdgeList.m_bounds.m_max.z);
    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_max.x, iterator.m_meshEdgeList.m_bounds.m_min.y, iterator.m_meshEdgeList.m_bounds.m_max.z);    
    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_max.x, iterator.m_meshEdgeList.m_bounds.m_max.y, iterator.m_meshEdgeList.m_bounds.m_max.z);
    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_min.x, iterator.m_meshEdgeList.m_bounds.m_max.y, iterator.m_meshEdgeList.m_bounds.m_max.z);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_min.x, iterator.m_meshEdgeList.m_bounds.m_min.y, iterator.m_meshEdgeList.m_bounds.m_min.z);
    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_min.x, iterator.m_meshEdgeList.m_bounds.m_min.y, iterator.m_meshEdgeList.m_bounds.m_max.z);

    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_max.x, iterator.m_meshEdgeList.m_bounds.m_min.y, iterator.m_meshEdgeList.m_bounds.m_min.z);
    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_max.x, iterator.m_meshEdgeList.m_bounds.m_min.y, iterator.m_meshEdgeList.m_bounds.m_max.z);

    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_max.x, iterator.m_meshEdgeList.m_bounds.m_max.y, iterator.m_meshEdgeList.m_bounds.m_min.z);
    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_max.x, iterator.m_meshEdgeList.m_bounds.m_max.y, iterator.m_meshEdgeList.m_bounds.m_max.z);

    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_min.x, iterator.m_meshEdgeList.m_bounds.m_max.y, iterator.m_meshEdgeList.m_bounds.m_min.z);
    glVertex3f(iterator.m_meshEdgeList.m_bounds.m_min.x, iterator.m_meshEdgeList.m_bounds.m_max.y, iterator.m_meshEdgeList.m_bounds.m_max.z);

    glEnd();
        
    //the direction
    /*drawVec = iterator.m_frustum.m_nearTipPoint + iterator.m_frustum.m_direction * 10.0f;

    glBegin(GL_LINES);
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3f(iterator.m_frustum.m_nearTipPoint.x, iterator.m_frustum.m_nearTipPoint.y, iterator.m_frustum.m_nearTipPoint.z);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3f(iterator.m_frustum.m_farTipPoint.x, iterator.m_frustum.m_farTipPoint.y, iterator.m_frustum.m_farTipPoint.z);

    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3f(iterator.m_frustum.m_nearTipPoint.x, iterator.m_frustum.m_nearTipPoint.y, iterator.m_frustum.m_nearTipPoint.z);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3f(iterator.m_frustum.m_nearTipPoint.x + iterator.m_iterator->m_directionSign.x * 20.0f, iterator.m_frustum.m_nearTipPoint.y, iterator.m_frustum.m_nearTipPoint.z);

    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3f(iterator.m_frustum.m_nearTipPoint.x, iterator.m_frustum.m_nearTipPoint.y, iterator.m_frustum.m_nearTipPoint.z);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3f(iterator.m_frustum.m_nearTipPoint.x, iterator.m_frustum.m_nearTipPoint.y + iterator.m_iterator->m_directionSign.y * 20.0f, iterator.m_frustum.m_nearTipPoint.z);

    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glVertex3f(iterator.m_frustum.m_nearTipPoint.x, iterator.m_frustum.m_nearTipPoint.y, iterator.m_frustum.m_nearTipPoint.z);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3f(iterator.m_frustum.m_nearTipPoint.x, iterator.m_frustum.m_nearTipPoint.y, iterator.m_frustum.m_nearTipPoint.z + iterator.m_iterator->m_directionSign.z * 20.0f);
    glEnd();*/

    //the world bounds
    glColor4f(1.0f, 1.0f, 1.0f, 0.1f);

    glBegin(GL_LINE_LOOP);

    glVertex3f(iterator.m_iterator->m_worldRange.m_min.x, iterator.m_iterator->m_worldRange.m_min.y, iterator.m_iterator->m_worldRange.m_min.z);
    glVertex3f(iterator.m_iterator->m_worldRange.m_max.x, iterator.m_iterator->m_worldRange.m_min.y, iterator.m_iterator->m_worldRange.m_min.z);
    glVertex3f(iterator.m_iterator->m_worldRange.m_max.x, iterator.m_iterator->m_worldRange.m_max.y, iterator.m_iterator->m_worldRange.m_min.z);
    glVertex3f(iterator.m_iterator->m_worldRange.m_min.x, iterator.m_iterator->m_worldRange.m_max.y, iterator.m_iterator->m_worldRange.m_min.z);

    glEnd();

    glBegin(GL_LINE_LOOP);

    glVertex3f(iterator.m_iterator->m_worldRange.m_min.x, iterator.m_iterator->m_worldRange.m_min.y, iterator.m_iterator->m_worldRange.m_max.z);
    glVertex3f(iterator.m_iterator->m_worldRange.m_max.x, iterator.m_iterator->m_worldRange.m_min.y, iterator.m_iterator->m_worldRange.m_max.z);    
    glVertex3f(iterator.m_iterator->m_worldRange.m_max.x, iterator.m_iterator->m_worldRange.m_max.y, iterator.m_iterator->m_worldRange.m_max.z);
    glVertex3f(iterator.m_iterator->m_worldRange.m_min.x, iterator.m_iterator->m_worldRange.m_max.y, iterator.m_iterator->m_worldRange.m_max.z);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(iterator.m_iterator->m_worldRange.m_min.x, iterator.m_iterator->m_worldRange.m_min.y, iterator.m_iterator->m_worldRange.m_min.z);
    glVertex3f(iterator.m_iterator->m_worldRange.m_min.x, iterator.m_iterator->m_worldRange.m_min.y, iterator.m_iterator->m_worldRange.m_max.z);

    glVertex3f(iterator.m_iterator->m_worldRange.m_max.x, iterator.m_iterator->m_worldRange.m_min.y, iterator.m_iterator->m_worldRange.m_min.z);
    glVertex3f(iterator.m_iterator->m_worldRange.m_max.x, iterator.m_iterator->m_worldRange.m_min.y, iterator.m_iterator->m_worldRange.m_max.z);

    glVertex3f(iterator.m_iterator->m_worldRange.m_max.x, iterator.m_iterator->m_worldRange.m_max.y, iterator.m_iterator->m_worldRange.m_min.z);
    glVertex3f(iterator.m_iterator->m_worldRange.m_max.x, iterator.m_iterator->m_worldRange.m_max.y, iterator.m_iterator->m_worldRange.m_max.z);

    glVertex3f(iterator.m_iterator->m_worldRange.m_min.x, iterator.m_iterator->m_worldRange.m_max.y, iterator.m_iterator->m_worldRange.m_min.z);
    glVertex3f(iterator.m_iterator->m_worldRange.m_min.x, iterator.m_iterator->m_worldRange.m_max.y, iterator.m_iterator->m_worldRange.m_max.z);

    glEnd();

    //the bounds
    glBegin(GL_LINES);
    glColor4f(0.0f, 0.5f, 0.0f, 0.5f);

    if(iterator.m_iterator->m_dimensionOrder[SLICE_DIM] == 0) {
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else if(iterator.m_iterator->m_dimensionOrder[Y_DIM] == 0) {
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    }
    else if(iterator.m_iterator->m_dimensionOrder[X_DIM] == 0) {
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    }
    
    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f * iterator.m_iterator->m_directionSign.x, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f * iterator.m_iterator->m_directionSign.x, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    if(iterator.m_iterator->m_dimensionOrder[SLICE_DIM] == 1) {
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else if(iterator.m_iterator->m_dimensionOrder[Y_DIM] == 1) {
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    }
    else if(iterator.m_iterator->m_dimensionOrder[X_DIM] == 1) {
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    }

    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f * iterator.m_iterator->m_directionSign.x, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f * iterator.m_iterator->m_directionSign.x, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    if(iterator.m_iterator->m_dimensionOrder[SLICE_DIM] == 2) {
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else if(iterator.m_iterator->m_dimensionOrder[Y_DIM] == 2) {
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    }
    else if(iterator.m_iterator->m_dimensionOrder[X_DIM] == 2) {
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    }

    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);


    glColor4f(0.0f, 1.0f, 1.0f, 0.2f);
    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);


    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);


    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);


    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);


    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);


    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);


    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);


    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);


    glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);

    glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
        iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
        iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z + iterator.m_iterator->m_cellDimensions.z * 0.5f);
    glEnd();

    //the slice plane normal and distance
    glBegin(GL_LINES);
    glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
    drawVec = iterator.m_iterator->m_slicePlane.m_normal * -iterator.m_iterator->m_slicePlane.m_distance;
    glVertex3fv(glm::value_ptr(drawVec));

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    drawVec += iterator.m_iterator->m_slicePlane.m_normal * 100.0f;
    glVertex3fv(glm::value_ptr(drawVec));

    glEnd();

    //slice planes   
    glBegin(GL_QUADS); 
    {
        glm::vec3 drawPoint;

        //front plane
        glColor4f(1.0f, 1.0f, 1.0f, 0.05f);

        drawPoint[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] = iterator.m_iterator->m_sliceStart;
        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]];
        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        //back plane
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);

        drawPoint[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] = iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] * iterator.m_iterator->m_directionSign[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]];
        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]];
        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[1]];

        glVertex3fv(glm::value_ptr(drawPoint));
    }
    glEnd();

    //grid
    glBegin(GL_LINES);
    {
        glm::mediump_float start;
        glm::mediump_float sign;
        glm::mediump_float dimensions;
        int numLines;

        glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
        sign = iterator.m_iterator->m_directionSign[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

        start = sign >= 0
            ? iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]]
            : iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

        dimensions = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

        numLines = glm::abs((int) iterator.m_iterator->m_range.m_max[iterator.m_iterator->m_dimensionOrder[Y_DIM]] - (int) iterator.m_iterator->m_range.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]]);

        for(int line = 1; line <= numLines; line ++) {
            glm::vec3 drawPoint;

            drawPoint[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] = iterator.m_iterator->m_sliceStart;
            drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = start + line * dimensions;
            drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];

            glVertex3fv(glm::value_ptr(drawPoint));

            drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

            glVertex3fv(glm::value_ptr(drawPoint));
        }

        sign = iterator.m_iterator->m_directionSign[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        start = sign >= 0
            ? iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]]
            : iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        dimensions = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        numLines = glm::abs((int) iterator.m_iterator->m_range.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]] - (int) iterator.m_iterator->m_range.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]]);

        for(int line = 1; line <= numLines; line ++) {
            glm::vec3 drawPoint;

            drawPoint[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] = iterator.m_iterator->m_sliceStart;
            drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]];
            drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = start + line * dimensions;

            glVertex3fv(glm::value_ptr(drawPoint));

            drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

            glVertex3fv(glm::value_ptr(drawPoint));
        }
    }
    glEnd();

    //current row rasterizing debug
    glLineWidth(3.0f);

    /*glBegin(GL_LINES);
    {
        glm::vec3 drawPoint;

        //row bottom
        glColor4f(1.0f, 1.0f, 0.0f, 0.2f);

        drawPoint[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] = iterator.m_iterator->m_sliceStart;
        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_lineBottom;
        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        //row top
        glColor4f(1.0f, 1.0f, 0.0f, 0.6f);

        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_lineTop;
        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        //side bounds
        glColor4f(0.0f, 1.0f, 1.0f, 0.2f);

        drawPoint[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] = iterator.m_iterator->m_sliceStart + 2.0f;

        //min
        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];
        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[Y_DIM]] * iterator.m_sliceMin.x;         

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));
                
        //min world
        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];
        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_leftSlicePoint;         

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        glColor4f(0.0f, 1.0f, 1.0f, 0.6f);

        //max
        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];
        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[Y_DIM]] * iterator.m_iterator->m_sliceMax.x;         

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));
        
        //max world
        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];
        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_rightSlicePoint;        

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        //top and bottom bounds
        glColor4f(0.0f, 1.0f, 1.0f, 0.2f);

        //min
        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[X_DIM]] * iterator.m_sliceMin.y;
        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]];         

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));

        glColor4f(0.0f, 1.0f, 1.0f, 0.6f);

        //max
        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[X_DIM]] * iterator.m_iterator->m_sliceMax.y;
        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]];         

        glVertex3fv(glm::value_ptr(drawPoint));

        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

        glVertex3fv(glm::value_ptr(drawPoint));
    }
    glEnd();*/

    //the 3D line/slice intersection points
    glPointSize(10.0f);

    glBegin(GL_POINTS);

    //current plane (white)
    glColor4f(1.0f, 1.0f, 1.0f, .5f);

    for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_pointList[!iterator.m_iterator->m_currentPointList].size(); elementIndex++) {
        glVertex3fv(glm::value_ptr(iterator.sliceTo3D(iterator.m_iterator->m_pointList[!iterator.m_iterator->m_currentPointList][elementIndex], iterator.m_pointListMissingDim[!iterator.m_iterator->m_currentPointList][elementIndex])));
    }

    //last plane (yellow)
    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

    for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_pointList[iterator.m_iterator->m_currentPointList].size(); elementIndex++) {
        glVertex3fv(glm::value_ptr(iterator.sliceTo3D(iterator.m_iterator->m_pointList[iterator.m_iterator->m_currentPointList][elementIndex], iterator.m_pointListMissingDim[iterator.m_iterator->m_currentPointList][elementIndex])));
    }

    glEnd();

    //line connecting polygon points in sorted order

    glBegin(GL_LINES);

    for(std::vector<glm::vec2*>::const_iterator iter = iterator.m_sortedSlicePoints.begin(); iter != iterator.m_sortedSlicePoints.end(); ) {
        glColor4f(1.0f, 0.0f, 1.0f, 0.2f);
        glVertex3fv(glm::value_ptr(iterator.sliceTo3D(**iter, iterator.m_iterator->m_sliceStart) + glm::vec3(5.0f)));

        iter++;

        if(iter == iterator.m_sortedSlicePoints.end()) {
            break;
        }

        glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(iterator.sliceTo3D(**iter, iterator.m_iterator->m_sliceStart) + glm::vec3(5.0f)));
    }

    glEnd();

    glLineWidth(6.0f);
        
    //the clipped convex hull polygon slice
    glLineWidth(20.0f);

    glBegin(GL_LINES);

    //"right"
    for(size_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE].size();) {
        glColor4f(0.7f, 0.7f, 1.0f, 0.3f);
        glVertex3fv(glm::value_ptr(iterator.sliceTo3D(*iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE][elementIndex], iterator.m_iterator->m_sliceStart)));

        elementIndex++;

        if(elementIndex == iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE].size()) {
            break;
        }

        glColor4f(0.7f, 0.7f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(iterator.sliceTo3D(*iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE][elementIndex], iterator.m_iterator->m_sliceStart)));
    }

    glEnd();


    glBegin(GL_LINES);

    //"left"
    for(size_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE].size();) {
        glColor4f(0.3f, 0.3f, 1.0f, 0.3f);
        glVertex3fv(glm::value_ptr(iterator.sliceTo3D(*iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE][elementIndex], iterator.m_iterator->m_sliceStart)));

        elementIndex++;

        if(elementIndex == iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE].size()) {
            break;
        }

        glColor4f(0.3f, 0.3f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(iterator.sliceTo3D(*iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE][elementIndex], iterator.m_iterator->m_sliceStart)));
    }

    glEnd();
    
    //the points
    glPointSize(20.0f);

    glBegin(GL_POINTS);

    //right
    glColor4f(0.7f, 0.7f, 1.0f, 0.3f);
    for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE].size(); elementIndex++) {      
        glVertex3fv(glm::value_ptr(iterator.sliceTo3D(*iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE][elementIndex], iterator.m_iterator->m_sliceStart)));
    }

    //left
    glColor4f(0.3f, 0.3f, 1.0f, 0.3f);
    for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE].size(); elementIndex++) {      
        glVertex3fv(glm::value_ptr(iterator.sliceTo3D(*iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE][elementIndex], iterator.m_iterator->m_sliceStart)));
    }

    glEnd();

    //draw the so far rasterized points
    glPointSize(3.0f);

    glBegin(GL_POINTS);

    glColor4f(1.0f, 1.0f, 1.0f, 0.2f);

    for(std::vector<glm::vec3>::const_iterator iter = iterator.m_rasterizedCells.begin(); iter != iterator.m_rasterizedCells.end(); iter++) {
        glVertex3fv(glm::value_ptr(*iter));
    }

    glEnd();

    //draw the current point

    glBegin(GL_POINTS);

    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);

    glVertex3fv(glm::value_ptr(iterator.m_iterator->m_cellDimensions * vec3cast<int, glm::mediump_float>(iterator.m_iterator->m_currentPosition) + (iterator.m_iterator->m_cellDimensions * 0.5f/* * vec3cast<int8_t, glm::mediump_float>(iterator.m_iterator->m_directionSign)*/)));

    glEnd();

    glLineWidth(1.0f);

    glShadeModel(GL_FLAT);

    glPointSize(1.0f);

    //draw the debug text for various things
    glUseProgram(getProgram(fontShader));

    {
        GLint diff = getProgramUniformLocation(getProgram(fontShader), "diffuseMap");
        glUniform1i(diff, 0);
    }

    //debug text
    /*renderTextDebug("abcdefghijklmnopqrstuvwxyz\nABCDEFGHIJKLMNOPQRSTUVWXYZ",
        glm::mat4(),
        font, camera, getProgram(fontShader));*/

    //messages
    /*{
        int message = 0;

        for(std::list<std::string>::const_reverse_iterator iter = iterator.m_messages.rbegin(); iter != iterator.m_messages.rend(); iter++, message++) {
            renderTextDebug(iter->c_str(), createTransform(glm::vec3(0.0f, -20.0f - 10.0f * message, 0.0f)), font, camera, getProgram(fontShader));
        }
    }*/

    //{
    //    glm::vec3 drawPoint;

    //    drawPoint[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] = iterator.m_iterator->m_sliceStart;

    //    //the sides
    //    {
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = 
    //            (iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]] 
    //            + iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[Y_DIM]]) * 0.5f;

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];

    //        renderTextDebug("LEFT", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

    //        renderTextDebug("RIGHT", createTransform(drawPoint), font, camera, getProgram(fontShader));
    //    
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = 
    //            (iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]] 
    //            + iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]]) * 0.5f;

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

    //        renderTextDebug("BOTTOM", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

    //        renderTextDebug("TOP", createTransform(drawPoint), font, camera, getProgram(fontShader));
    //    }

    //    drawPoint[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] = iterator.m_iterator->m_sliceStart;

    //    //row rasterizing
    //    {
    //        /*drawPoint[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] = iterator.m_iterator->m_sliceStart - 5;

    //        //row bottom
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_lineBottom;
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];

    //        renderTextDebug("ROW BOTTOM", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

    //        renderTextDebug("ROW BOTTOM", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        //row top
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_lineTop;
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];

    //        renderTextDebug("ROW TOP", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

    //        renderTextDebug("ROW TOP", createTransform(drawPoint), font, camera, getProgram(fontShader));*/

    //        //side bounds

    //        //min
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[Y_DIM]] * iterator.m_sliceMin.x;         

    //        renderTextDebug("SIDE MIN", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

    //        renderTextDebug("SIDE MIN", createTransform(drawPoint), font, camera, getProgram(fontShader));
    //        
    //        //min world
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_leftSlicePoint;         

    //        renderTextDebug("SIDE MIN W", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

    //        renderTextDebug("SIDE MIN W", createTransform(drawPoint), font, camera, getProgram(fontShader));
    //        
    //        //max
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[Y_DIM]] * iterator.m_iterator->m_sliceMax.x;         

    //        renderTextDebug("SIDE MAX", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

    //        renderTextDebug("SIDE MAX", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        //max world
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[X_DIM]];
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_rightSlicePoint;        

    //        renderTextDebug("SIDE MAX W", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[X_DIM]];

    //        renderTextDebug("SIDE MAX W", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        //top and bottom bounds
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] = iterator.m_iterator->m_sliceStart + 5;

    //        //min
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[X_DIM]] * iterator.m_sliceMin.y;
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]];         

    //        renderTextDebug("VERT MIN", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

    //        renderTextDebug("VERT MIN", createTransform(drawPoint), font, camera, getProgram(fontShader));
    //        
    //        //max
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[Y_DIM]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[X_DIM]] * iterator.m_iterator->m_sliceMax.y;
    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_min[iterator.m_iterator->m_dimensionOrder[Y_DIM]];         

    //        renderTextDebug("VERT MAX", createTransform(drawPoint), font, camera, getProgram(fontShader));

    //        drawPoint[iterator.m_iterator->m_dimensionOrder[X_DIM]] = iterator.m_iterator->m_worldRange.m_max[iterator.m_iterator->m_dimensionOrder[Y_DIM]];

    //        renderTextDebug("VERT MAX", createTransform(drawPoint), font, camera, getProgram(fontShader));
    //    }

    //    drawPoint[iterator.m_iterator->m_dimensionOrder[SLICE_DIM]] = iterator.m_iterator->m_sliceStart;
    //}

    glUseProgram(0);
}

void renderMesh(illGraphics::Mesh& mesh, illGraphics::ModelAnimationController& controller, const illGraphics::Camera& camera, const glm::mat4& xform, GLuint prog) {    
    GLint loc = getProgramUniformLocation(prog, "modelViewProjectionMatrix");
    glUniformMatrix4fv(loc, 1, false, glm::value_ptr(camera.getCanonical() * xform));

    loc = getProgramUniformLocation(prog, "modelViewMatrix");
    glUniformMatrix4fv(loc, 1, false, glm::value_ptr(camera.getModelView() * xform));
    
    GLuint buffer = *((GLuint *) mesh.getMeshBackendData() + 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    loc = getProgramUniformLocation(prog, "bones");
    glUniformMatrix4fv(loc, controller.m_skeleton->getNumBones(), false, &controller.m_skelMats[0][0][0]);

    GLint pos = getProgramAttribLocation(prog, "position");
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getPositionOffset());
    glEnableVertexAttribArray(pos);

    GLint tex = getProgramAttribLocation(prog, "texCoords");
    glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getTexCoordOffset());
    glEnableVertexAttribArray(tex);

    GLint norm = getProgramAttribLocation(prog, "normal");
    glVertexAttribPointer(norm, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getNormalOffset());
    glEnableVertexAttribArray(norm);

    GLint tan = getProgramAttribLocation(prog, "tangent");
    glVertexAttribPointer(tan, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getTangentOffset());
    glEnableVertexAttribArray(tan);

    GLint bitan = getProgramAttribLocation(prog, "bitangent");
    glVertexAttribPointer(bitan, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getBitangentOffset());
    glEnableVertexAttribArray(bitan);

    GLint bonei = getProgramAttribLocation(prog, "boneIndices");
    glVertexAttribIPointer(bonei, 4, GL_INT, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getBlendIndexOffset());
    glEnableVertexAttribArray(bonei);

    GLint weights = getProgramAttribLocation(prog, "weights");
    glVertexAttribPointer(weights, 4, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getBlendWeightOffset());
    glEnableVertexAttribArray(weights);

    buffer = *((GLuint *) mesh.getMeshBackendData() + 1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

    glDrawRangeElements(GL_TRIANGLES, 0, mesh.m_meshFrontendData->getNumTri() * 3, mesh.m_meshFrontendData->getNumTri() * 3, GL_UNSIGNED_SHORT, (char *)NULL);

    glDisableVertexAttribArray(pos);
    glDisableVertexAttribArray(norm);
    glDisableVertexAttribArray(tex);
    glDisableVertexAttribArray(tan);
    glDisableVertexAttribArray(bitan);
    glDisableVertexAttribArray(bonei);
    glDisableVertexAttribArray(weights);
}

void debugDrawBone(const glm::mat4& xForm, const glm::mat4& prevXform, bool drawLine) {
    glm::vec4 currPoint(0.0f, 0.0f, 0.0f, 1.0f);
    currPoint = xForm * currPoint;

    glm::vec4 parentPos(0.0f, 0.0f, 0.0f, 1.0f);
    parentPos = prevXform * parentPos;

    //draw line from this bone to the last bone
    glLineWidth(3.0f);

    if(drawLine) {
        glColor4f(1.0f, 1.0f, 0.0f, 0.15f);

        glBegin(GL_LINES);
            glVertex3fv(glm::value_ptr(parentPos));
            glVertex3fv(glm::value_ptr(currPoint));
        glEnd();
    }

    glPointSize(5.0f);
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);

    //draw the bone point
    glBegin(GL_POINTS);
    glVertex3fv(glm::value_ptr(currPoint));
    glEnd();

    //draw the bone orientation
    glLineWidth(3.0f);

    glBegin(GL_LINES);
        //x
        glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
        glVertex3fv(glm::value_ptr(currPoint));
        
        glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
        glVertex3fv(glm::value_ptr(glm::vec3(currPoint) + glm::mat3(xForm) * glm::vec3(1.0f, 0.0f, 0.0f) * 5.0f));

        //y
        glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
        glVertex3fv(glm::value_ptr(currPoint));
        
        glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
        glVertex3fv(glm::value_ptr(glm::vec3(currPoint) + glm::mat3(xForm) * glm::vec3(0.0f, 1.0f, 0.0f) * 5.0f));

        //z
        glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
        glVertex3fv(glm::value_ptr(currPoint));
        
        glColor4f(0.0f, 0.0f, 1.0f, 0.0f);
        glVertex3fv(glm::value_ptr(glm::vec3(currPoint) + glm::mat3(xForm) * glm::vec3(0.0f, 0.0f, 1.0f) * 5.0f));
    glEnd();
    
    glLineWidth(1.0f);
}

void renderSkeleton(const illGraphics::Skeleton& skeleton, const illGraphics::Skeleton::BoneHeirarchy * currNode, const illGraphics::ModelAnimationController& animationController, glm::mat4 currXform, glm::mat4 currBindXform) {
    glm::mat4 prevXform = currXform;
    glm::mat4 prevBindXform = currBindXform;
    
    std::map<unsigned int, illGraphics::ModelAnimationController::BoneInfo>::const_iterator iter = animationController.m_animationTest.find(currNode->m_boneIndex);
        
    if(iter != animationController.m_animationTest.end()) {
        currXform = currXform * iter->second.m_transform;
    }
    else {
        currXform = currXform * skeleton.getBone(currNode->m_boneIndex)->m_transform;
    }

    currBindXform = currBindXform * skeleton.getBone(currNode->m_boneIndex)->m_transform;
           
    debugDrawBone(currXform, prevXform, currNode->m_parent != NULL);
    //debugDrawBone(currBindXform, prevBindXform, currNode->m_parent != NULL);

    for(std::vector<illGraphics::Skeleton::BoneHeirarchy *>::const_iterator iter = currNode->m_children.begin(); iter != currNode->m_children.end(); iter++) {
        renderSkeleton(skeleton, *iter, animationController, currXform, currBindXform);
    }
}

void renderMeshDebug(const illGraphics::Mesh& mesh, const illGraphics::ModelAnimationController& controller, const glm::mat4& xform) {
    glPointSize(5.0f);
    
    for(unsigned int vertex = 0; vertex < mesh.m_meshFrontendData->getNumVert(); vertex++) {
        glm::mat4 transformedMat = controller.m_skelMats[(int) mesh.m_meshFrontendData->getBlendData(vertex).m_blendIndex[0]] * mesh.m_meshFrontendData->getBlendData(vertex).m_blendWeight[0];
        transformedMat += controller.m_skelMats[(int) mesh.m_meshFrontendData->getBlendData(vertex).m_blendIndex[1]] * mesh.m_meshFrontendData->getBlendData(vertex).m_blendWeight[1];
        transformedMat += controller.m_skelMats[(int) mesh.m_meshFrontendData->getBlendData(vertex).m_blendIndex[2]] * mesh.m_meshFrontendData->getBlendData(vertex).m_blendWeight[2];
        transformedMat += controller.m_skelMats[(int) mesh.m_meshFrontendData->getBlendData(vertex).m_blendIndex[3]] * mesh.m_meshFrontendData->getBlendData(vertex).m_blendWeight[3];
        
        glm::vec4 pos = xform * transformedMat * glm::vec4(mesh.m_meshFrontendData->getPosition(vertex), 1.0f);
        
        //transformed point
        glBegin(GL_POINTS);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(pos));

        glEnd();

        glBegin(GL_LINES);

        glm::vec3 tail;

        //normal
        glm::vec3 skinned = glm::mat3(xform) * glm::mat3(transformedMat) * mesh.m_meshFrontendData->getNormal(vertex);
        
        tail = glm::vec3(pos) + glm::vec3(skinned) * 10.0f;

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(pos));
        glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
        glVertex3fv(glm::value_ptr(tail));

        //tangent
        skinned = glm::mat3(xform) * glm::mat3(transformedMat) * mesh.m_meshFrontendData->getTangent(vertex).m_tangent;
        
        tail = glm::vec3(pos) + glm::vec3(skinned) * 10.0f;

        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertex3fv(glm::value_ptr(pos));
        glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
        glVertex3fv(glm::value_ptr(tail));

        //bitangent
        skinned = glm::mat3(xform) * glm::mat3(transformedMat) * mesh.m_meshFrontendData->getTangent(vertex).m_bitangent;
        
        tail = glm::vec3(pos) + glm::vec3(skinned) * 10.0f;

        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(pos));
        glColor4f(0.0f, 0.0f, 1.0f, 0.0f);
        glVertex3fv(glm::value_ptr(tail));

        glEnd();
    }
}

const glm::vec3 LIGHT_POS(0.0, 100.0, 100.0);

namespace Demo {

void MainMenuController::ResetFrustumIterator::onRelease() {
    illGraphics::Camera testCam;
    testCam.setTransform(m_controller->m_camera.getTransform(), m_controller->m_engine->m_window->getAspectRatio(), illGraphics::DEFAULT_FOV, 100.0f, 300.0f);

    //set up test mesh edge list
    m_controller->m_testMeshEdgeList.clear();

    //the edges
    for(unsigned int edge = 0; edge < 12; edge++) {
        m_controller->m_testMeshEdgeList.m_edges.push_back(MeshEdgeList<>::Edge(FRUSTUM_EDGE_LIST[edge][0], FRUSTUM_EDGE_LIST[edge][1]));
    }

    //the points
    for(unsigned int point = 0; point < 8; point++) {
        m_controller->m_testMeshEdgeList.m_points.push_back(testCam.getViewFrustum().m_points[point]);
    }

    m_controller->m_testMeshEdgeList.computePointEdgeMap();

    m_controller->m_testUnclippedMeshEdgeList = m_controller->m_testMeshEdgeList;

    //m_controller->m_planeIndex = 0;

    //clip the mesh edge list against some planes
    m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(1.0f, 0.0f, 0.0f), 60.0f));
    m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 1.0f, 0.0f), 60.0f));
    m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, 1.0f), 60.0f));

    m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(-1.0f, 0.0f, 0.0f), 59.99f));
    m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, -1.0f, 0.0f), 59.99f));
    m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, -1.0f), 59.99f));

    if(!m_controller->m_testMeshEdgeList.m_points.empty()) {
        m_controller->m_testMeshEdgeList.computeBounds();

        //get intersection of frustum and bounds
        Box<int> iterBounds(glm::ivec3(-3), glm::ivec3(3));
        Box<int> frustumGrid(m_controller->m_testMeshEdgeList.m_bounds.grid<int>(glm::vec3(20.0f)));

        if(iterBounds.intersects(frustumGrid)) {
            iterBounds.constrain(frustumGrid);

            m_controller->m_testFrustumIter = new ConvexMeshIterator<>(&m_controller->m_testMeshEdgeList, 
                testCam.getViewFrustum().m_direction, 
                frustumGrid,
                glm::vec3(20.0f));
        }
        else {
            delete m_controller->m_testFrustumIter;
            m_controller->m_testFrustumIter = NULL;
        }
    }
    else {
        delete m_controller->m_testFrustumIter;
        m_controller->m_testFrustumIter = NULL;
    }
}

void MainMenuController::RestartFrustumIterator::onRelease() {
    if(m_controller->m_testFrustumIter) {
        //get intersection of frustum and bounds    
        /*Box<int> iterBounds(glm::ivec3(-3), glm::ivec3(3));  
        Box<int> frustumGrid(m_controller->m_testFrustumIter->m_frustum->m_bounds.grid<int>(glm::vec3(20.0f)));

        iterBounds.constrain(frustumGrid);*/

        //m_controller->m_testFrustumIter = new FrustumIterator<>(m_controller->m_testFrustumIter->m_frustum, frustumGrid, glm::vec3(20.0f));
        
        m_controller->m_testFrustumIter = new ConvexMeshIterator<>(&m_controller->m_testMeshEdgeList, 
            glm::normalize(vec3cast<int8_t, glm::mediump_float>(m_controller->m_testFrustumIter->m_directionSign)), 
            m_controller->m_testFrustumIter->m_range.normalize(),
            glm::vec3(20.0f));
    }
}

void MainMenuController::CompleteFrustumIterator::onRelease() {
    if(m_controller->m_testFrustumIter) {
        while(!m_controller->m_testFrustumIter->atEnd()) {
            m_controller->m_testFrustumIter->forward();
        }
    }
}

MainMenuController::MainMenuController(Engine * engine)
    : GameControllerBase(),
    m_engine(engine),
    m_testFrustumIter(NULL),
    //m_planeIndex(0),
    m_advanceHold(false),
    m_advanceHoldTimer(0.0f),
    m_lightPos(glm::vec3(0.0f)),
    m_forward(false),
    m_back(false),
    m_left(false),
    m_right(false),
    m_up(false),
    m_down(false)
{
    //This is all put together to test some stuff, this is in no way how to normally do these things.  Everything should normally be done through the renderer front end when that's done.

    //debug font
    {
        illGraphics::BitmapFontLoadArgs loadArgs;
        loadArgs.m_path = "prototype12.fnt";

        m_debugFont.load(loadArgs, m_engine->m_rendererBackend);
    }

    //marine body
    {
        IllmeshLoader<> meshLoader("Meshes/Marine/marine8.illmesh");

        m_marine.m_meshFrontendData = new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features);
    
        meshLoader.buildMesh(*m_marine.m_meshFrontendData);
        m_marine.frontendBackendTransfer(m_engine->m_rendererBackend, false);
    }

    //load the diffuse texture
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/marine.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;

        m_marineDiffuse.load(loadArgs, m_engine->m_rendererBackend);
    }

    //marine normal map
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/marine_local.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;

        m_marineNormal.load(loadArgs, m_engine->m_rendererBackend);
    }




    //marine helmet
    {
        IllmeshLoader<> meshLoader("Meshes/Marine/marine.illmesh");

        m_marineHelmet.m_meshFrontendData = new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features);
    
        meshLoader.buildMesh(*m_marineHelmet.m_meshFrontendData);
        m_marineHelmet.frontendBackendTransfer(m_engine->m_rendererBackend, false);
    }

    //helmet normal map
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/helmet_local.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;

        m_helmetNormal.load(loadArgs, m_engine->m_rendererBackend);
    }
    
    //diffuse helmet texture
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/helmet.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;

        m_helmetDiffuse.load(loadArgs, m_engine->m_rendererBackend);
    }




    //load the skeleton
    {
        illGraphics::SkeletonLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/marine.illskel";
        m_marineSkeleton.load(loadArgs, NULL);

        //m_animationTestSkelMats = new glm::mat4[m_marineSkeleton.getNumBones()];

        m_marineController.alloc(m_marineSkeleton.getNumBones());
        m_marineController.m_skeleton = &m_marineSkeleton;
    }

    //load the animation
    {
        illGraphics::SkeletonAnimationLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/marine.illanim";
        m_marineAnimation.load(loadArgs, NULL);

        m_marineController.m_animation = &m_marineAnimation;
    }





    //hellknight
    {
        IllmeshLoader<> meshLoader("Meshes/HellKnight/hellKnight.illmesh");

        m_hellKnight.m_meshFrontendData = new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features);
    
        meshLoader.buildMesh(*m_hellKnight.m_meshFrontendData);
        m_hellKnight.frontendBackendTransfer(m_engine->m_rendererBackend, false);
    }

    //load the diffuse texture
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/HellKnight/hellknight.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;

        m_hellKnightDiffuse.load(loadArgs, m_engine->m_rendererBackend);
    }

    //marine normal map
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/HellKnight/hellknight_local.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;

        m_hellKnightNormal.load(loadArgs, m_engine->m_rendererBackend);
    }

    //load the skeleton
    {
        illGraphics::SkeletonLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/HellKnight/hellknight.illskel";
        m_hellKnightSkeleton.load(loadArgs, NULL);
        
        m_hellKnightController0.alloc(m_hellKnightSkeleton.getNumBones());
        m_hellKnightController0.m_skeleton = &m_hellKnightSkeleton;

        m_hellKnightController1.alloc(m_hellKnightSkeleton.getNumBones());
        m_hellKnightController1.m_skeleton = &m_hellKnightSkeleton;

        m_hellKnightController2.alloc(m_hellKnightSkeleton.getNumBones());
        m_hellKnightController2.m_skeleton = &m_hellKnightSkeleton;
    }

    //load the animation
    {
        illGraphics::SkeletonAnimationLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/HellKnight/hellknight.illanim";
        m_hellKnightAnimation.load(loadArgs, NULL);

        m_hellKnightController0.m_animation = &m_hellKnightAnimation;
        m_hellKnightController1.m_animation = &m_hellKnightAnimation;
        m_hellKnightController2.m_animation = &m_hellKnightAnimation;

        m_hellKnightController1.m_animTime = 1.0f;
        m_hellKnightController2.m_animTime = 1.5f;
    }




    //demon
    {
        IllmeshLoader<> meshLoader("Meshes/Demon/demon.illmesh");

        m_demon.m_meshFrontendData = new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features);
    
        meshLoader.buildMesh(*m_demon.m_meshFrontendData);
        m_demon.frontendBackendTransfer(m_engine->m_rendererBackend, false);
    }

    //load the diffuse texture
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Demon/pinky_d.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;

        m_demonDiffuse.load(loadArgs, m_engine->m_rendererBackend);
    }

    //marine normal map
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Demon/pinky_local.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::W_CLAMP_TO_EDGE;

        m_demonNormal.load(loadArgs, m_engine->m_rendererBackend);
    }
    
    //demon front
    {
        IllmeshLoader<> meshLoader("Meshes/Demon/demon0.illmesh");

        m_demonFront.m_meshFrontendData = new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features);
    
        meshLoader.buildMesh(*m_demonFront.m_meshFrontendData);
        m_demonFront.frontendBackendTransfer(m_engine->m_rendererBackend, false);
    }

    //load the skeleton
    {
        illGraphics::SkeletonLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Demon/demon.illskel";
        m_demonSkeleton.load(loadArgs, NULL);
        
        m_demonController0.alloc(m_demonSkeleton.getNumBones());
        m_demonController0.m_skeleton = &m_demonSkeleton;

        m_demonController1.alloc(m_demonSkeleton.getNumBones());
        m_demonController1.m_skeleton = &m_demonSkeleton;

        m_demonController2.alloc(m_demonSkeleton.getNumBones());
        m_demonController2.m_skeleton = &m_demonSkeleton;

        m_demonController3.alloc(m_demonSkeleton.getNumBones());
        m_demonController3.m_skeleton = &m_demonSkeleton;
    }

    //load the animation
    {
        illGraphics::SkeletonAnimationLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Demon/demon.illanim";
        m_demonAnimation.load(loadArgs, NULL);

        m_demonController0.m_animation = &m_demonAnimation;
        m_demonController1.m_animation = &m_demonAnimation;
        m_demonController2.m_animation = &m_demonAnimation;
        m_demonController3.m_animation = &m_demonAnimation;

        m_demonController1.m_animTime = 0.5f;
        m_demonController2.m_animTime = 0.75f;
        m_demonController3.m_animTime = 1.00f;
    }



    //bill
    {
        IllmeshLoader<> meshLoader("Meshes/Bill/bill.illmesh");

        m_bill.m_meshFrontendData = new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features);
    
        meshLoader.buildMesh(*m_bill.m_meshFrontendData);
        m_bill.frontendBackendTransfer(m_engine->m_rendererBackend, false);
    }
    
    //load the skeleton
    {
        illGraphics::SkeletonLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Bill/bill.illskel";
        m_billSkeleton.load(loadArgs, NULL);
        
        m_billController.alloc(m_billSkeleton.getNumBones());
        m_billController.m_skeleton = &m_billSkeleton;
    }

    //load the animation
    {
        illGraphics::SkeletonAnimationLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Bill/flutter.illanim";
        m_billAnimation.load(loadArgs, NULL);

        m_billController.m_animation = &m_billAnimation;
    }




    //load the test shader
    {
        std::vector<RefCountPtr<illGraphics::Shader> > shaders;

        illGraphics::Shader * shader = new illGraphics::Shader();
        shader->loadInternal(m_engine->m_rendererBackend, "shaders/debugShader.vert", GL_VERTEX_SHADER, "");

        shaders.push_back(RefCountPtr<illGraphics::Shader>(shader));

        shader = new illGraphics::Shader();
        shader->loadInternal(m_engine->m_rendererBackend, "shaders/debugShader.frag", GL_FRAGMENT_SHADER, "");

        shaders.push_back(RefCountPtr<illGraphics::Shader>(shader));

        m_debugShaderLoader = new illGraphics::ShaderProgramLoader(m_engine->m_rendererBackend, NULL);
        m_debugShader.loadInternal(m_debugShaderLoader, shaders);
    }

    //load the temporary font shader
    {
        std::vector<RefCountPtr<illGraphics::Shader> > shaders;

        illGraphics::Shader * shader = new illGraphics::Shader();
        shader->loadInternal(m_engine->m_rendererBackend, "shaders/tempFont.vert", GL_VERTEX_SHADER, "");

        shaders.push_back(RefCountPtr<illGraphics::Shader>(shader));

        shader = new illGraphics::Shader();
        shader->loadInternal(m_engine->m_rendererBackend, "shaders/tempFont.frag", GL_FRAGMENT_SHADER, "");

        shaders.push_back(RefCountPtr<illGraphics::Shader>(shader));

        m_fontShader.loadInternal(m_debugShaderLoader, shaders);
    }

    //initialize the input (this would normally initialize using console variables)
    m_engine->m_inputManager->addPlayer(0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_KEYBOARD, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE_BUTTON, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE_WHEEL, 0);

    m_advanceFrustumIteratorCallback.m_controller = this;
    m_advanceFrustumIteratorHoldCallback.m_controller = this;
    m_resetFrustumIteratorCallback.m_controller = this;
    m_restartFrustumIteratorCallback.m_controller = this;
    m_completeFrustumIteratorCallback.m_controller = this;

    m_advanceFrustumIterator.m_inputCallback = &m_advanceFrustumIteratorCallback;
    m_advanceFrustumIteratorHold.m_inputCallback = &m_advanceFrustumIteratorHoldCallback;
    m_resetFrustumIterator.m_inputCallback = &m_resetFrustumIteratorCallback;
    m_restartFrustumIterator.m_inputCallback = &m_restartFrustumIteratorCallback;
    m_completeFrustumIterator.m_inputCallback = &m_completeFrustumIteratorCallback;

    m_forwardListener.m_state = &m_forward;
    m_backListener.m_state = &m_back;
    m_leftListener.m_state = &m_left;
    m_rightListener.m_state = &m_right;
    m_upListener.m_state = &m_up;
    m_downListener.m_state = &m_down;

    m_forwardInput.m_inputCallback = &m_forwardListener;
    m_backInput.m_inputCallback = &m_backListener;
    m_leftInput.m_inputCallback = &m_leftListener;
    m_rightInput.m_inputCallback = &m_rightListener;
    m_upInput.m_inputCallback = &m_upListener;
    m_downInput.m_inputCallback = &m_downListener;

    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_RIGHT), &m_advanceFrustumIterator);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_UP), &m_advanceFrustumIteratorHold);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_LEFT), &m_restartFrustumIterator);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_DOWN), &m_resetFrustumIterator);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_END), &m_completeFrustumIterator);

    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_8), &m_forwardInput);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_5), &m_backInput);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_4), &m_leftInput);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_6), &m_rightInput);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_0), &m_upInput);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_ENTER), &m_downInput);

    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_cameraController.m_inputContext);
    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_frustumInputContext);

    m_cameraController.m_speed = 50.0f;
    m_cameraController.m_rollSpeed = 50.0f;
}

MainMenuController::~MainMenuController() {
    //delete[] m_animationTestSkelMats;
    delete m_debugShaderLoader;
    delete m_testFrustumIter;
}

void MainMenuController::update(float seconds) {
    m_cameraController.update(seconds);

    /*m_marineController.update(seconds);

    m_hellKnightController0.update(seconds);
    m_hellKnightController1.update(seconds * 0.5f);
    m_hellKnightController2.update(seconds * 0.1f);

    m_demonController0.update(seconds);
    m_demonController1.update(seconds * 0.5f);
    m_demonController2.update(seconds * 0.2f);
    m_demonController3.update(seconds * 0.1f);

    m_billController.update(seconds);*/
    
    m_advanceHoldTimer -= seconds;

    if(m_testFrustumIter && m_advanceHold && m_advanceHoldTimer < 0 && !m_testFrustumIter->atEnd()) {
        m_advanceHoldTimer = 0.0f;//0.05f;
        m_testFrustumIter->forward();
    }

    if(m_forward) {
        m_lightPos.z += -75.0f * seconds;
    }
    else if(m_back) {
        m_lightPos.z += 75.0f * seconds;
    }

    if(m_left) {
        m_lightPos.x += -75.0f * seconds;
    }
    else if(m_right) {
        m_lightPos.x += 75.0f * seconds;
    }

    if(m_up) {
        m_lightPos.y += 75.0f * seconds;
    }
    else if(m_down) {
        m_lightPos.y += -75.0f * seconds;
    }
}

void MainMenuController::updateSound(float seconds) {

}
 
void MainMenuController::render() {
    m_cameraTransform.m_transform = m_cameraController.m_transform;
    m_camera.setTransform(m_cameraTransform, m_engine->m_window->getAspectRatio(), illGraphics::DEFAULT_FOV * m_cameraController.m_zoom, illGraphics::DEFAULT_NEAR, 2000.0f, m_cameraController.m_orthoMode);
    
    m_marineController.computeAnimPose();

    m_hellKnightController0.computeAnimPose();
    m_hellKnightController1.computeAnimPose();
    m_hellKnightController2.computeAnimPose();

    m_demonController0.computeAnimPose();
    m_demonController1.computeAnimPose();
    m_demonController2.computeAnimPose();
    m_demonController3.computeAnimPose();

    m_billController.computeAnimPose();

    //draw the 3d models
    
    //TODO: for now I'm testing a bunch of stuff, normally all rendering is done through the renderer   
    GLuint prog = *((GLuint *) m_debugShader.getShaderProgram());

    glUseProgram(prog);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);    
    glDisable(GL_BLEND);
        
    /*GLint loc = getProgramUniformLocation(prog, "lightPos");
    glUniform3fv(loc, 1, glm::value_ptr(m_camera.getModelView() * glm::vec4(m_lightPos, 1.0f)));

    glm::mat4 xform;

    //draw marine body
    xform = glm::translate(glm::vec3(500.0f, 0.0f, 0.0f));

    loc = getProgramUniformLocation(prog, "diffuseMap");

    glActiveTexture(GL_TEXTURE0);
    GLuint texture = *((GLuint *) m_marineDiffuse.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(loc, 0);

    loc = getProgramUniformLocation(prog, "normalMap");

    glActiveTexture(GL_TEXTURE1);
    texture = *((GLuint *) m_marineNormal.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(loc, 1);

    renderMesh(m_marine, m_marineController, m_camera, xform, prog);

    //draw marine helmet
    glActiveTexture(GL_TEXTURE0);
    texture = *((GLuint *) m_helmetDiffuse.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    texture = *((GLuint *) m_helmetNormal.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);

    renderMesh(m_marineHelmet, m_marineController, m_camera, xform, prog);

    //draw hellknight
    glActiveTexture(GL_TEXTURE0);
    texture = *((GLuint *) m_hellKnightDiffuse.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    texture = *((GLuint *) m_hellKnightNormal.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);

    
    xform = glm::translate(glm::vec3(0.0f, 100.0f, 0.0f));
    renderMesh(m_hellKnight, m_hellKnightController0, m_camera, xform, prog);

    xform = glm::translate(glm::vec3(-20.0f, -200.0f, 0.0f)) * glm::scale(glm::vec3(2.0f));
    renderMesh(m_hellKnight, m_hellKnightController1, m_camera, xform, prog);

    xform = glm::translate(glm::vec3(-500.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(4.0f));
    renderMesh(m_hellKnight, m_hellKnightController2, m_camera, xform, prog);

    //draw demon
    glActiveTexture(GL_TEXTURE0);
    texture = *((GLuint *) m_demonDiffuse.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    texture = *((GLuint *) m_demonNormal.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);
    
    xform = glm::mat4();
    renderMesh(m_demon, m_demonController0, m_camera, xform, prog);
    renderMesh(m_demonFront, m_demonController0, m_camera, xform, prog);

    xform = glm::translate(glm::vec3(0.0f, -100.0f, 0.0f));
    renderMesh(m_demon, m_demonController1, m_camera, xform, prog);
    renderMesh(m_demonFront, m_demonController1, m_camera, xform, prog);

    xform = glm::translate(glm::vec3(0.0f, 300.0f, 0.0f)) * glm::scale(glm::vec3(2.0f));
    renderMesh(m_demon, m_demonController2, m_camera, xform, prog);
    renderMesh(m_demonFront, m_demonController2, m_camera, xform, prog);

    xform = glm::translate(glm::vec3(-400.0f, 600.0f, 0.0f)) * glm::scale(glm::vec3(5.0f));
    renderMesh(m_demon, m_demonController3, m_camera, xform, prog);
    renderMesh(m_demonFront, m_demonController3, m_camera, xform, prog);*/

    //draw bill
    /*xform = glm::mat4();
    static float testAng = 0;
    testAng += 1.0f;
    xform = glm::rotate(xform, testAng, glm::vec3(0.0f, 0.0f, 1.0f));
    renderMesh(m_bill, m_billController, m_camera, xform, prog);*/

    //draw a font
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*glUseProgram(getProgram(m_fontShader));

    {
        GLint diff = getProgramUniformLocation(getProgram(m_fontShader), "diffuseMap");
        glUniform1i(diff, 0);
    }

    renderTextDebug("abcdefghijklmnopqrstuvwxyz\nABCDEFGHIJKLMNOPQRSTUVWXYZ", 
        //createTransform(glm::vec3(5.0f, 5.0f, 5.0f), directionToMat3(glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)))),
        glm::mat4(),
        m_debugFont, m_camera, getProgram(m_fontShader));*/

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

    //debug draw the skeletons
    /*renderSkeleton(m_marineSkeleton, m_marineSkeleton.getRootBoneNode(), m_marineController, 
        glm::translate(glm::vec3(500.0f, 0.0f, 0.0f)), 
        glm::translate(glm::vec3(500.0f, 0.0f, 0.0f)));

    renderSkeleton(m_hellKnightSkeleton, m_hellKnightSkeleton.getRootBoneNode(), m_hellKnightController0, 
        glm::translate(glm::vec3(0.0f, 100.0f, 0.0f)), 
        glm::translate(glm::vec3(0.0f, 100.0f, 0.0f)));
    renderSkeleton(m_hellKnightSkeleton, m_hellKnightSkeleton.getRootBoneNode(), m_hellKnightController1, 
        glm::translate(glm::vec3(-20.0f, -200.0f, 0.0f)) * glm::scale(glm::vec3(2.0f)), 
        glm::translate(glm::vec3(-20.0f, -200.0f, 0.0f)) * glm::scale(glm::vec3(2.0f)));
    renderSkeleton(m_hellKnightSkeleton, m_hellKnightSkeleton.getRootBoneNode(), m_hellKnightController2, 
        glm::translate(glm::vec3(-500.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(4.0f)), 
        glm::translate(glm::vec3(-500.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(4.0f)));

    renderSkeleton(m_demonSkeleton, m_demonSkeleton.getRootBoneNode(), m_demonController0, 
        glm::mat4(), 
        glm::mat4());
    renderSkeleton(m_demonSkeleton, m_demonSkeleton.getRootBoneNode(), m_demonController1, 
        glm::translate(glm::vec3(0.0f, -100.0f, 0.0f)), 
        glm::translate(glm::vec3(0.0f, -100.0f, 0.0f)));
    renderSkeleton(m_demonSkeleton, m_demonSkeleton.getRootBoneNode(), m_demonController2, 
        glm::translate(glm::vec3(0.0f, 300.0f, 0.0f)) * glm::scale(glm::vec3(2.0f)), 
        glm::translate(glm::vec3(0.0f, 300.0f, 0.0f)) * glm::scale(glm::vec3(2.0f)));
    renderSkeleton(m_demonSkeleton, m_demonSkeleton.getRootBoneNode(), m_demonController3, 
        glm::translate(glm::vec3(-400.0f, 600.0f, 0.0f)) * glm::scale(glm::vec3(5.0f)), 
        glm::translate(glm::vec3(-400.0f, 600.0f, 0.0f)) * glm::scale(glm::vec3(5.0f)));*/
    
    //debug draw the meshes
    /*xform = glm::translate(glm::vec3(500.0f, 0.0f, 0.0f));
    renderMeshDebug(m_marine, m_marineController, xform);
    renderMeshDebug(m_marineHelmet, m_marineController, xform);

    xform = glm::translate(glm::vec3(0.0f, 100.0f, 0.0f));
    renderMeshDebug(m_hellKnight, m_hellKnightController0, xform);

    xform = glm::translate(glm::vec3(-20.0f, -200.0f, 0.0f)) * glm::scale(glm::vec3(2.0f));
    renderMeshDebug(m_hellKnight, m_hellKnightController1, xform);

    xform = glm::translate(glm::vec3(-500.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(4.0f));
    renderMeshDebug(m_hellKnight, m_hellKnightController2, xform);*/

    /*xform = glm::mat4();
    xform = glm::rotate(xform, testAng, glm::vec3(0.0f, 0.0f, 1.0f));
    renderMeshDebug(m_bill, m_billController, xform);*/

    //debug draw the frustum iterators
    renderMeshEdgeListDebug(m_testUnclippedMeshEdgeList);

    //renderSceneDebug(Box<>(glm::vec3(0.0f), glm::vec3(5.0f * 100.0f - 0.1f)), glm::vec3(100.0f), glm::uvec3(5));

    if(m_testFrustumIter) {
        renderFrustumIterDebug(m_testFrustumIter->m_debugger, m_camera, m_fontShader, m_debugFont);
    }

    //draw the light position
    /*glBegin(GL_LINES);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(m_lightPos));
        glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
        glVertex3fv(glm::value_ptr(m_lightPos + glm::vec3(15.0f, 0.0f, 0.0f)));

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(m_lightPos));
        glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
        glVertex3fv(glm::value_ptr(m_lightPos + glm::vec3(-15.0f, 0.0f, 0.0f)));

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(m_lightPos));
        glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
        glVertex3fv(glm::value_ptr(m_lightPos + glm::vec3(0.0f, 15.0f, 0.0f)));

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(m_lightPos));
        glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
        glVertex3fv(glm::value_ptr(m_lightPos + glm::vec3(0.0f, -15.0f, 0.0f)));

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(m_lightPos));
        glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
        glVertex3fv(glm::value_ptr(m_lightPos + glm::vec3(0.0f, 0.0f, 15.0f)));

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(m_lightPos));
        glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
        glVertex3fv(glm::value_ptr(m_lightPos + glm::vec3(0.0f, 0.0f, -15.0f)));
    glEnd();*/

    glDepthMask(GL_TRUE);

    ERROR_CHECK_OPENGL;
}

}
