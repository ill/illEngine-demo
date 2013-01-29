#include <set>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Engine.h"
#include "illEngine/Graphics/Window.h"

#include "FrustumIterVisualizerController.h"
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

void renderMeshEdgeListDebug(const MeshEdgeList<>& edgeList, const illGraphics::Camera& camera, const illGraphics::ShaderProgram& fontShader, const illGraphics::BitmapFont& font) {
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

     //draw the debug text for various things
    glUseProgram(getProgram(fontShader));

    {
        GLint diff = getProgramUniformLocation(getProgram(fontShader), "diffuseMap");
        glUniform1i(diff, 0);
    }

    //all the points
    /*for(size_t point = 0; point < edgeList.m_points.size(); point++) {
        renderTextDebug(formatString("pidx:%u (%f, %f, %f)", point, edgeList.m_points[point].x, edgeList.m_points[point].y, edgeList.m_points[point].z).c_str(),
            createTransform(edgeList.m_points[point]),
            font, camera, getProgram(fontShader));
    }*/

    //all the lines
    /*for(size_t edge = 0; edge < edgeList.m_edges.size(); edge++) {
        glm::vec3 pos = (edgeList.m_points[edgeList.m_edges[edge].m_point[0]] + edgeList.m_points[edgeList.m_edges[edge].m_point[1]]) * 0.5f;

        renderTextDebug(formatString("eidx:%u pts: %u, %u", edge, edgeList.m_edges[edge].m_point[0], edgeList.m_edges[edge].m_point[1]).c_str(),
            createTransform(pos),
            font, camera, getProgram(fontShader));
    }*/

    glUseProgram(0);
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

void renderFrustumIterDebug(const ConvexMeshIteratorDebug<>::Debugger& iterator, bool mapToWorld, const illGraphics::Camera& camera, const illGraphics::ShaderProgram& fontShader, const illGraphics::BitmapFont& font) {
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
            glVertex3fv(glm::value_ptr(iterator.getPoint(
                iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[edge].m_point[0]], 
                mapToWorld)));

            glVertex3fv(glm::value_ptr(iterator.getPoint(
                iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[edge].m_point[1]],
                mapToWorld)));
        }
    }
    
    //active edges
    glColor4f(0.0f, 1.0f, 0.0f, 0.25f);

    for(std::unordered_map<size_t, int>::const_iterator iter = iterator.m_iterator->m_activeEdges.begin(); iter != iterator.m_iterator->m_activeEdges.end(); iter++) {
        glVertex3fv(glm::value_ptr(iterator.getPoint(
            iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[iter->first].m_point[0]],
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(
            iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[iter->first].m_point[1]],
            mapToWorld)));
    }

    //discarded edges
    glColor4f(1.0f, 0.0f, 0.0f, 0.25f);

    for(std::unordered_set<size_t>::const_iterator iter = iterator.m_discarededEdges.begin(); iter != iterator.m_discarededEdges.end(); iter++) {
        glVertex3fv(glm::value_ptr(iterator.getPoint(
            iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[*iter].m_point[0]], 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(
            iterator.m_meshEdgeList.m_points[iterator.m_meshEdgeList.m_edges[*iter].m_point[1]], 
            mapToWorld)));
    }

    glEnd();

    glLineWidth(1.0f);

    //the frustum bounds
    glColor4f(1.0f, 1.0f, 1.0f, 0.25f);

    glBegin(GL_LINE_LOOP);

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_min.x, 
        iterator.m_meshEdgeList.m_bounds.m_min.y, 
        iterator.m_meshEdgeList.m_bounds.m_min.z), 
        mapToWorld)));

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_max.x, 
        iterator.m_meshEdgeList.m_bounds.m_min.y, 
        iterator.m_meshEdgeList.m_bounds.m_min.z),
        mapToWorld)));

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_max.x, 
        iterator.m_meshEdgeList.m_bounds.m_max.y, 
        iterator.m_meshEdgeList.m_bounds.m_min.z), 
        mapToWorld)));

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_min.x, 
        iterator.m_meshEdgeList.m_bounds.m_max.y, 
        iterator.m_meshEdgeList.m_bounds.m_min.z), 
        mapToWorld)));

    glEnd();

    glBegin(GL_LINE_LOOP);

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_min.x, 
        iterator.m_meshEdgeList.m_bounds.m_min.y, 
        iterator.m_meshEdgeList.m_bounds.m_max.z), 
        mapToWorld)));

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_max.x, 
        iterator.m_meshEdgeList.m_bounds.m_min.y, 
        iterator.m_meshEdgeList.m_bounds.m_max.z), 
        mapToWorld)));

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_max.x, 
        iterator.m_meshEdgeList.m_bounds.m_max.y, 
        iterator.m_meshEdgeList.m_bounds.m_max.z), 
        mapToWorld)));

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_min.x, 
        iterator.m_meshEdgeList.m_bounds.m_max.y, 
        iterator.m_meshEdgeList.m_bounds.m_max.z), 
        mapToWorld)));
    
    glEnd();

    glBegin(GL_LINES);

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_min.x, 
        iterator.m_meshEdgeList.m_bounds.m_min.y, 
        iterator.m_meshEdgeList.m_bounds.m_min.z), 
        mapToWorld)));

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_min.x, 
        iterator.m_meshEdgeList.m_bounds.m_min.y, 
        iterator.m_meshEdgeList.m_bounds.m_max.z), 
        mapToWorld)));


    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_max.x, 
        iterator.m_meshEdgeList.m_bounds.m_min.y, 
        iterator.m_meshEdgeList.m_bounds.m_min.z), 
        mapToWorld)));

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_max.x, 
        iterator.m_meshEdgeList.m_bounds.m_min.y, 
        iterator.m_meshEdgeList.m_bounds.m_max.z), 
        mapToWorld)));


    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_max.x, 
        iterator.m_meshEdgeList.m_bounds.m_max.y, 
        iterator.m_meshEdgeList.m_bounds.m_min.z), 
        mapToWorld)));

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_max.x, 
        iterator.m_meshEdgeList.m_bounds.m_max.y, 
        iterator.m_meshEdgeList.m_bounds.m_max.z), 
        mapToWorld)));


    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_min.x, 
        iterator.m_meshEdgeList.m_bounds.m_max.y, 
        iterator.m_meshEdgeList.m_bounds.m_min.z), 
        mapToWorld)));

    glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
        iterator.m_meshEdgeList.m_bounds.m_min.x, 
        iterator.m_meshEdgeList.m_bounds.m_max.y, 
        iterator.m_meshEdgeList.m_bounds.m_max.z),
        mapToWorld)));
    
    glEnd();
        
    //the direction sign
    {
        glBegin(GL_LINES);
        
        //x
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertex3fv(glm::value_ptr(glm::vec3(0.0f)));

        drawVec = glm::vec3(0.0f);
        drawVec[iterator.m_iterator->m_dimensionOrder[0]] = iterator.m_iterator->m_directionSign[iterator.m_iterator->m_dimensionOrder[0]] * 10.0f;
        glVertex3fv(glm::value_ptr(drawVec));

        glVertex3fv(glm::value_ptr(drawVec));
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(iterator.m_direction * 10.0f));

        //y
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glVertex3fv(glm::value_ptr(glm::vec3(0.0f)));

        drawVec = glm::vec3(0.0f);
        drawVec[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_directionSign[iterator.m_iterator->m_dimensionOrder[1]] * 10.0f;
        glVertex3fv(glm::value_ptr(drawVec));

        glVertex3fv(glm::value_ptr(drawVec));
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(iterator.m_direction * 10.0f));
        
        //z
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(glm::vec3(0.0f)));

        drawVec = glm::vec3(0.0f);
        drawVec[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_directionSign[iterator.m_iterator->m_dimensionOrder[2]] * 10.0f;
        glVertex3fv(glm::value_ptr(drawVec));

        glVertex3fv(glm::value_ptr(drawVec));
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(iterator.m_direction * 10.0f));

        glEnd();
    }

    //the direction
    glBegin(GL_LINES);

    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    glVertex3fv(glm::value_ptr(glm::vec3(0.0f)));

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3fv(glm::value_ptr(iterator.m_direction * 10.0f));

    glEnd();

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
    {
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);

        glBegin(GL_LINE_LOOP);
    
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            0.0f, 
            0.0f), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            0.0f, 
            0.0f),
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            0.0f), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            0.0f), 
            mapToWorld)));
        
        glEnd();

        glBegin(GL_LINE_LOOP);

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            0.0f, 
            iterator.m_iterator->m_algorithmWorldBounds.z), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            0.0f, 
            iterator.m_iterator->m_algorithmWorldBounds.z), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_algorithmWorldBounds.z), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_algorithmWorldBounds.z), 
            mapToWorld)));
                
        glEnd();

        glBegin(GL_LINES);

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            0.0f, 
            0.0f), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            0.0f, 
            iterator.m_iterator->m_algorithmWorldBounds.z), 
            mapToWorld)));


        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            0.0f, 
            0.0f),
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            0.0f, 
            iterator.m_iterator->m_algorithmWorldBounds.z), 
            mapToWorld)));


        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            0.0f),
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_algorithmWorldBounds.z), 
            mapToWorld)));


        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            0.0f), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_algorithmWorldBounds.z), 
            mapToWorld)));
                
        glEnd();
    }

    //the world bounds for real
    {
        glColor4f(1.0f, 1.0f, 1.0f, 0.03f);

        glBegin(GL_LINE_LOOP);
    
        glVertex3fv(glm::value_ptr(iterator.m_iterator->m_worldBounds.m_min));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_max.x, 
            iterator.m_iterator->m_worldBounds.m_min.y, 
            iterator.m_iterator->m_worldBounds.m_min.z)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_max.x, 
            iterator.m_iterator->m_worldBounds.m_max.y, 
            iterator.m_iterator->m_worldBounds.m_min.z)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_min.x, 
            iterator.m_iterator->m_worldBounds.m_max.y, 
            iterator.m_iterator->m_worldBounds.m_min.z)));
        
        glEnd();

        glBegin(GL_LINE_LOOP);

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_min.x, 
            iterator.m_iterator->m_worldBounds.m_min.y, 
            iterator.m_iterator->m_worldBounds.m_max.z)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_max.x, 
            iterator.m_iterator->m_worldBounds.m_min.y, 
            iterator.m_iterator->m_worldBounds.m_max.z)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_max.x, 
            iterator.m_iterator->m_worldBounds.m_max.y, 
            iterator.m_iterator->m_worldBounds.m_max.z)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_min.x, 
            iterator.m_iterator->m_worldBounds.m_max.y, 
            iterator.m_iterator->m_worldBounds.m_max.z)));
                
        glEnd();

        glBegin(GL_LINES);

        glVertex3fv(glm::value_ptr(iterator.m_iterator->m_worldBounds.m_min));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_min.x, 
            iterator.m_iterator->m_worldBounds.m_min.y, 
            iterator.m_iterator->m_worldBounds.m_max.z)));


        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_max.x, 
            iterator.m_iterator->m_worldBounds.m_min.y, 
            iterator.m_iterator->m_worldBounds.m_min.z)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_max.x, 
            iterator.m_iterator->m_worldBounds.m_min.y, 
            iterator.m_iterator->m_worldBounds.m_max.z)));


        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_max.x, 
            iterator.m_iterator->m_worldBounds.m_max.y, 
            iterator.m_iterator->m_worldBounds.m_min.z)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_max.x, 
            iterator.m_iterator->m_worldBounds.m_max.y, 
            iterator.m_iterator->m_worldBounds.m_max.z)));


        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_min.x, 
            iterator.m_iterator->m_worldBounds.m_max.y, 
            iterator.m_iterator->m_worldBounds.m_min.z)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_worldBounds.m_min.x, 
            iterator.m_iterator->m_worldBounds.m_max.y, 
            iterator.m_iterator->m_worldBounds.m_max.z)));
                
        glEnd();
    }

    //the grid bounds
    {
        glBegin(GL_LINES);

        //the x
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            0, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            0, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        glColor4f(1.0f, 0.0f, 0.0f, 0.05f);

        //the y
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            0, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            iterator.m_iterator->m_algorithmBounds.y, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        //the z
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            0, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            0, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        //the rest
        glColor4f(0.0f, 1.0f, 1.0f, 0.2f);

        //x00 - x0z
        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            0, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            0, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        //00z - x0z
        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            0, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            0, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        //xyz - x0z
        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            iterator.m_iterator->m_algorithmBounds.y, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            0, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        //00z - 0yz
        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            0, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            iterator.m_iterator->m_algorithmBounds.y, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        //x00 - xy0
        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            0, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            iterator.m_iterator->m_algorithmBounds.y, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        //xy0 - xyz
        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            iterator.m_iterator->m_algorithmBounds.y, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            iterator.m_iterator->m_algorithmBounds.y, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        //0yz - xyz
        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            iterator.m_iterator->m_algorithmBounds.y, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            iterator.m_iterator->m_algorithmBounds.y, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        //0y0 - 0yz
        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            iterator.m_iterator->m_algorithmBounds.y, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            iterator.m_iterator->m_algorithmBounds.y, 
            iterator.m_iterator->m_algorithmBounds.z)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        //0y0 - xy0
        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            0, 
            iterator.m_iterator->m_algorithmBounds.y, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(vec3cast<int, glm::mediump_float>(glm::ivec3(
            iterator.m_iterator->m_algorithmBounds.x, 
            iterator.m_iterator->m_algorithmBounds.y, 
            0)) 
                * iterator.m_iterator->m_cellDimensions + iterator.m_iterator->m_cellDimensions * 0.5f, 
            mapToWorld)));
        
        glEnd();
    }

    //the surrounding grid in world coords
    /*glBegin(GL_LINES);
    {
        glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
        
        //the xy0 grid

        //y
        for(int line = 0; line <= iterator.m_iterator->m_algorithmBounds.y; line ++) {
            glVertex3fv(glm::value_ptr(glm::vec3(
                iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x], 
                iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] 
                    + line * iterator.m_iterator->m_cellDimensions.y, 
                iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z])));

            glVertex3fv(glm::value_ptr(glm::vec3(
                iterator.m_iterator->m_bounds.m_max.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x], 
                iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] 
                    + line * iterator.m_iterator->m_cellDimensions.y, 
                iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z])));
        }

        //x
        for(int line = 0; line <= iterator.m_iterator->m_algorithmBounds.x; line ++) {
            glVertex3fv(glm::value_ptr(glm::vec3(
                iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                    + line * iterator.m_iterator->m_cellDimensions.x, 
                iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y],
                iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z])));

            glVertex3fv(glm::value_ptr(glm::vec3(
                iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                    + line * iterator.m_iterator->m_cellDimensions.x, 
                iterator.m_iterator->m_bounds.m_max.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y],
                iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z])));
        }

        //the xyz grid

        //y
        for(int line = 0; line <= iterator.m_iterator->m_algorithmBounds.y; line ++) {
            glVertex3fv(glm::value_ptr(glm::vec3(
                iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x], 
                iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] 
                    + line * iterator.m_iterator->m_cellDimensions.y, 
                iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z])));

            glVertex3fv(glm::value_ptr(glm::vec3(
                iterator.m_iterator->m_bounds.m_max.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x], 
                iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] 
                    + line * iterator.m_iterator->m_cellDimensions.y, 
                iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z])));
        }

        //x
        for(int line = 0; line <= iterator.m_iterator->m_algorithmBounds.x; line ++) {
            glVertex3fv(glm::value_ptr(glm::vec3(
                iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                    + line * iterator.m_iterator->m_cellDimensions.x, 
                iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y],
                iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z])));

            glVertex3fv(glm::value_ptr(glm::vec3(
                iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                    + line * iterator.m_iterator->m_cellDimensions.x, 
                iterator.m_iterator->m_bounds.m_max.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y],
                iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z])));
        }
    }
    glEnd();*/

    //the grid bounds for real
    {
        glColor4f(1.0f, 1.0f, 0.0f, 0.1f);

        glBegin(GL_LINE_LOOP);
    
        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f,
            iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_max.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f,
            iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_max.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));
        
        glEnd();

        glBegin(GL_LINE_LOOP);

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_max.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_max.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));
                
        glEnd();

        glBegin(GL_LINES);

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f,
            iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));


        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_max.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_max.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));


        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_max.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_max.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));


        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_min.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));

        glVertex3fv(glm::value_ptr(glm::vec3(
            iterator.m_iterator->m_bounds.m_min.x * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.x] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.y * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.y] * 0.5f, 
            iterator.m_iterator->m_bounds.m_max.z * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z]
                + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrderInverse.z] * 0.5f)));
                
        glEnd();
    }
    
    //the slice plane normal and distance
    /*glBegin(GL_LINES);
    glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
    drawVec = iterator.m_iterator->m_slicePlane.m_normal * -iterator.m_iterator->m_slicePlane.m_distance;
    glVertex3fv(glm::value_ptr(drawVec));

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    drawVec += iterator.m_iterator->m_slicePlane.m_normal * 100.0f;
    glVertex3fv(glm::value_ptr(drawVec));

    glEnd();*/

    //slice planes   
    glBegin(GL_QUADS); 
    {
        glm::vec3 drawPoint;

        //front plane
        glColor4f(1.0f, 1.0f, 1.0f, 0.05f);
        
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            0.0f, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            0.0f, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));

        //back plane
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            0.0f, 
            iterator.m_iterator->m_sliceEnd), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_sliceEnd), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_sliceEnd), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            0.0f, 
            iterator.m_iterator->m_sliceEnd), 
            mapToWorld)));
    }
    glEnd();

    //plane grid
    glBegin(GL_LINES);
    {
        glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
        
        for(int line = 1; line <= iterator.m_iterator->m_algorithmBounds.y; line ++) {
            glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
                0.0f, 
                line * iterator.m_iterator->m_cellDimensions.y, 
                iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
                mapToWorld)));

            glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
                iterator.m_iterator->m_algorithmWorldBounds.x, 
                line * iterator.m_iterator->m_cellDimensions.y, 
                iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
                mapToWorld)));            
        }

        for(int line = 1; line <= iterator.m_iterator->m_algorithmBounds.x; line ++) {
            glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
                line * iterator.m_iterator->m_cellDimensions.x, 
                0.0f, 
                iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
                mapToWorld)));

            glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
                line * iterator.m_iterator->m_cellDimensions.x, 
                iterator.m_iterator->m_algorithmWorldBounds.y, 
                iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
                mapToWorld)));
        }
    }
    glEnd();

    //current row rasterizing debug
    glLineWidth(3.0f);

    glBegin(GL_LINES);
    {
        //row bottom
        glColor4f(1.0f, 1.0f, 0.0f, 0.2f);

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            iterator.m_iterator->m_lineBottom, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            iterator.m_iterator->m_lineBottom, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));
        
        //row top
        glColor4f(1.0f, 1.0f, 0.0f, 0.6f);

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            iterator.m_iterator->m_lineTop, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            iterator.m_iterator->m_lineTop, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));
        
        //side bounds
        glColor4f(0.0f, 1.0f, 1.0f, 0.2f);
        
        //min
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_cellDimensions.x * iterator.m_sliceMin.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
            0.0f, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_cellDimensions.x * iterator.m_sliceMin.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));
                        
        //min world
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_leftSlicePoint, 
            0.0f, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_leftSlicePoint, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));
        
        glColor4f(0.0f, 1.0f, 1.0f, 0.6f);

        //max
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_cellDimensions.x * iterator.m_iterator->m_sliceMax.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
            0.0f, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_cellDimensions.x * iterator.m_iterator->m_sliceMax.x + iterator.m_iterator->m_cellDimensions.x * 0.5f, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));
                        
        //max world
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_rightSlicePoint, 
            0.0f, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_rightSlicePoint, 
            iterator.m_iterator->m_algorithmWorldBounds.y, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));
        
        //top and bottom bounds
        glColor4f(0.0f, 1.0f, 1.0f, 0.2f);

        //min
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            iterator.m_iterator->m_cellDimensions.y * iterator.m_sliceMin.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
            iterator.m_iterator->m_sliceStart),
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            iterator.m_iterator->m_cellDimensions.y * iterator.m_sliceMin.y + iterator.m_iterator->m_cellDimensions.y * 0.5, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));
        
        glColor4f(0.0f, 1.0f, 1.0f, 0.6f);

        //max
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            0.0f, 
            iterator.m_iterator->m_cellDimensions.y * iterator.m_iterator->m_sliceMax.y + iterator.m_iterator->m_cellDimensions.y * 0.5f, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));

        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_algorithmWorldBounds.x, 
            iterator.m_iterator->m_cellDimensions.y * iterator.m_iterator->m_sliceMax.y + iterator.m_iterator->m_cellDimensions.y * 0.5, 
            iterator.m_iterator->m_sliceStart), 
            mapToWorld)));
    }
    glEnd();

    //the 3D line/slice intersection points
    glPointSize(10.0f);

    glBegin(GL_POINTS);

    //current plane (white)
    glColor4f(1.0f, 1.0f, 1.0f, .5f);

    for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_pointList[!iterator.m_iterator->m_currentPointList].size(); elementIndex++) {
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_pointList[!iterator.m_iterator->m_currentPointList][elementIndex], 
            iterator.m_pointListMissingDim[!iterator.m_iterator->m_currentPointList][elementIndex]), 
            mapToWorld)));
    }

    //last plane (yellow)
    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

    for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_pointList[iterator.m_iterator->m_currentPointList].size(); elementIndex++) {
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            iterator.m_iterator->m_pointList[iterator.m_iterator->m_currentPointList][elementIndex], 
            iterator.m_pointListMissingDim[iterator.m_iterator->m_currentPointList][elementIndex]), 
            mapToWorld)));
    }

    glEnd();

    //line connecting polygon points in sorted order

    glBegin(GL_LINES);

    for(std::vector<glm::vec2*>::const_iterator iter = iterator.m_sortedSlicePoints.begin(); iter != iterator.m_sortedSlicePoints.end(); ) {
        glColor4f(1.0f, 0.0f, 1.0f, 0.2f);
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            **iter,
            iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
            mapToWorld)));

        iter++;

        if(iter == iterator.m_sortedSlicePoints.end()) {
            break;
        }

        glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            **iter, 
            iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
            mapToWorld)));
    }

    glEnd();

    glLineWidth(6.0f);
        
    //the clipped convex hull polygon slice
    glLineWidth(20.0f);

    glBegin(GL_LINES);

    //"right"
    for(size_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE].size();) {
        glColor4f(0.7f, 0.7f, 1.0f, 0.3f);
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            *iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE][elementIndex], 
            iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
            mapToWorld)));

        elementIndex++;

        if(elementIndex == iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE].size()) {
            break;
        }

        glColor4f(0.7f, 0.7f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            *iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE][elementIndex], 
            iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
            mapToWorld)));
    }

    glEnd();


    glBegin(GL_LINES);

    //"left"
    for(size_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE].size();) {
        glColor4f(0.3f, 0.3f, 1.0f, 0.3f);
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            *iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE][elementIndex], 
            iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
            mapToWorld)));

        elementIndex++;

        if(elementIndex == iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE].size()) {
            break;
        }

        glColor4f(0.3f, 0.3f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            *iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE][elementIndex], 
            iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
            mapToWorld)));
    }

    glEnd();
    
    //the points
    glPointSize(20.0f);

    glBegin(GL_POINTS);

    //right
    glColor4f(0.7f, 0.7f, 1.0f, 0.3f);
    for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE].size(); elementIndex++) {
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            *iterator.m_iterator->m_sliceRasterizeEdges[RIGHT_SIDE][elementIndex], 
            iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
            mapToWorld)));
    }

    //left
    glColor4f(0.3f, 0.3f, 1.0f, 0.3f);
    for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE].size(); elementIndex++) {
        glVertex3fv(glm::value_ptr(iterator.getPoint(glm::vec3(
            *iterator.m_iterator->m_sliceRasterizeEdges[LEFT_SIDE][elementIndex], 
            iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions.z * 0.5f), 
            mapToWorld)));
    }

    glEnd();

    //draw the so far rasterized points
    glPointSize(8.0f);

    glBegin(GL_POINTS);

    glColor4f(1.0f, 1.0f, 1.0f, 0.2f);

    for(std::vector<glm::vec3>::const_iterator iter = iterator.m_rasterizedCells.begin(); iter != iterator.m_rasterizedCells.end(); iter++) {
        glVertex3fv(glm::value_ptr(iterator.getPoint(
            *iter, 
            mapToWorld)));
    }

    glEnd();

    //draw the current point

    glBegin(GL_POINTS);

    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
        
    glVertex3fv(glm::value_ptr(iterator.getPoint(
        iterator.m_iterator->m_cellDimensions * vec3cast<int, glm::mediump_float>(iterator.m_iterator->m_currentPosition) 
            + (iterator.m_iterator->m_cellDimensions * 0.5f), 
        mapToWorld)));

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
            renderTextDebug(iter->c_str(), createTransform(iterator.getPoint(glm::vec3(0.0f, -20.0f - 10.0f * message, 0.0f), mapToWorld)), font, camera, getProgram(fontShader));
        }
    }*/

    {
        glm::vec3 drawPoint;

        drawPoint.z = iterator.m_iterator->m_sliceStart;

        //the sides
        {
            drawPoint.y = iterator.m_iterator->m_algorithmWorldBounds.y * 0.5f;
            drawPoint.x = 0.0f;

            renderTextDebug("LEFT", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

            drawPoint.x = iterator.m_iterator->m_algorithmWorldBounds.x;

            renderTextDebug("RIGHT", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));
        
            drawPoint.x = iterator.m_iterator->m_algorithmWorldBounds.x * 0.5f;
            drawPoint.y = 0.0f;

            renderTextDebug("BOTTOM", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

            drawPoint.y = iterator.m_iterator->m_algorithmWorldBounds.y;

            renderTextDebug("TOP", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));
        }
        
        //row rasterizing

        //row bottom
        /*drawPoint.y = iterator.m_iterator->m_lineBottom;
        drawPoint.x = 0.0f;

        renderTextDebug("ROW BOTTOM", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

        drawPoint.x = iterator.m_iterator->m_algorithmWorldBounds.x;

        renderTextDebug("ROW BOTTOM", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));
        
        //row top
        drawPoint.y = iterator.m_iterator->m_lineTop;
        drawPoint.x = 0.0f;

        renderTextDebug("ROW TOP", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

        drawPoint.x = iterator.m_iterator->m_algorithmWorldBounds.x;
              
        renderTextDebug("ROW TOP", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));*/

        //side bounds        
        //min
        /*
        drawPoint.x = iterator.m_iterator->m_cellDimensions.x * iterator.m_sliceMin.x + iterator.m_iterator->m_cellDimensions.x * 0.5f;
        drawPoint.y = 0.0f;

        renderTextDebug("SIDE MIN", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

        drawPoint.y = iterator.m_iterator->m_algorithmWorldBounds.y;

        renderTextDebug("SIDE MIN", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));
                        
        //min world
        drawPoint.x = iterator.m_leftSlicePoint;
        drawPoint.y = 0.0f;

        renderTextDebug("SIDE MIN W", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

        drawPoint.y = iterator.m_iterator->m_algorithmWorldBounds.y;
        
        renderTextDebug("SIDE MIN W", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

        //max
        drawPoint.x = iterator.m_iterator->m_cellDimensions.x * iterator.m_iterator->m_sliceMax.x + iterator.m_iterator->m_cellDimensions.x * 0.5f;
        drawPoint.y = 0.0f;

        renderTextDebug("SIDE MAX", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

        drawPoint.y = iterator.m_iterator->m_algorithmWorldBounds.y;
        
        renderTextDebug("SIDE MAX", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

        //max world
        drawPoint.x = iterator.m_rightSlicePoint;
        drawPoint.y = 0.0f;

        renderTextDebug("SIDE MAX W", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

        drawPoint.y = iterator.m_iterator->m_algorithmWorldBounds.y;

        renderTextDebug("SIDE MAX W", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));
        
        //top and bottom bounds
        //min        
        drawPoint.y = iterator.m_iterator->m_cellDimensions.y * iterator.m_sliceMin.y + iterator.m_iterator->m_cellDimensions.y * 0.5f;
        drawPoint.x = 0.0f;

        renderTextDebug("VERT MIN", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

        drawPoint.x = iterator.m_iterator->m_algorithmWorldBounds.x;

        renderTextDebug("VERT MIN", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));
        
        //max
        drawPoint.y = iterator.m_iterator->m_cellDimensions.y * iterator.m_iterator->m_sliceMax.y + iterator.m_iterator->m_cellDimensions.y * 0.5f;
        drawPoint.x = 0.0f;

        renderTextDebug("VERT MAX", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));

        drawPoint.x = iterator.m_iterator->m_algorithmWorldBounds.x;

        renderTextDebug("VERT MAX", createTransform(iterator.getPoint(drawPoint, mapToWorld)), font, camera, getProgram(fontShader));
        */
    }

    glUseProgram(0);
}

namespace Demo {

void FrustumIterVisualizerController::setupTestFrustumIterator() {
    //set up test mesh edge list
    m_testMeshEdgeList.clear();

    //the edges
    for(unsigned int edge = 0; edge < 12; edge++) {
        m_testMeshEdgeList.m_edges.push_back(MeshEdgeList<>::Edge(FRUSTUM_EDGE_LIST[edge][0], FRUSTUM_EDGE_LIST[edge][1]));
    }

    //the points
    for(unsigned int point = 0; point < 8; point++) {
        m_testMeshEdgeList.m_points.push_back(m_testFrustumCamera.getViewFrustum().m_points[point]);
    }

    m_testMeshEdgeList.computePointEdgeMap();

    m_testUnclippedMeshEdgeList = m_testMeshEdgeList;

    //controller.m_planeIndex = 0;

    //clip the mesh edge list against some planes
    m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(1.0f, 0.0f, 0.0f), 500.0f));
    m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 1.0f, 0.0f), 500.0f));
    m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, 1.0f), 500.0f));

    m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(-1.0f, 0.0f, 0.0f), 499.999f));
    m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, -1.0f, 0.0f), 499.999f));
    m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, -1.0f), 499.999f));
    
    if(!m_testMeshEdgeList.m_points.empty()) {
        m_testMeshEdgeList.computeBounds();

        //get intersection of frustum and bounds
        Box<int> iterBounds(glm::ivec3(-10), glm::ivec3(10));
        Box<int> frustumGrid(m_testMeshEdgeList.m_bounds.grid<int>(glm::vec3(50.0f)));

        if(iterBounds.intersects(frustumGrid)) {
            iterBounds.constrain(frustumGrid);

            //create a copy of the clipped mesh
            m_iteratedMeshEdgeList = m_testMeshEdgeList;

            m_testFrustumIter = new ConvexMeshIteratorDebug<>(&m_iteratedMeshEdgeList, 
                m_testFrustumCamera.getViewFrustum().m_direction, 
                frustumGrid,
                glm::vec3(50.0f));
        }
        else {
            delete m_testFrustumIter;
            m_testFrustumIter = NULL;
        }
    }
    else {
        delete m_testFrustumIter;
        m_testFrustumIter = NULL;
    }
}

void FrustumIterVisualizerController::ResetFrustumIterator::onRelease() {
    m_controller->m_testFrustumCamera.setPerspectiveTransform(m_controller->m_camera.getTransform(), m_controller->m_engine->m_window->getAspectRatio(), illGraphics::DEFAULT_FOV, 50.0f, 300.0f);

    m_controller->setupTestFrustumIterator();
}

void FrustumIterVisualizerController::RestartFrustumIterator::onRelease() {
    if(m_controller->m_testFrustumIter) {
        m_controller->setupTestFrustumIterator();
    }
}

void FrustumIterVisualizerController::CompleteFrustumIterator::onRelease() {
    if(m_controller->m_testFrustumIter) {
        while(!m_controller->m_testFrustumIter->atEnd()) {
            m_controller->m_testFrustumIter->forward();
        }
    }
}

FrustumIterVisualizerController::FrustumIterVisualizerController(Engine * engine)
    : GameControllerBase(),
    m_engine(engine),
    m_testFrustumIter(NULL),
    //m_planeIndex(0),
    m_advanceHold(false),
    m_advanceHoldTimer(0.0f),
    m_mapToWorld(true)
{
    //This is all put together to test some stuff, this is in no way how to normally do these things.  Everything should normally be done through the renderer front end when that's done.

    //debug font
    {
        illGraphics::BitmapFontLoadArgs loadArgs;
        loadArgs.m_path = "prototype12.fnt";

        m_debugFont.load(loadArgs, m_engine->m_rendererBackend);
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

        m_debugShaderLoader = new illGraphics::ShaderProgramLoader(m_engine->m_rendererBackend, NULL);
        m_fontShader.loadInternal(m_debugShaderLoader, shaders);
    }

    //initialize the input (this would normally initialize using console variables)    
    m_advanceFrustumIteratorCallback.m_controller = this;
    m_advanceFrustumIteratorHoldCallback.m_controller = this;
    m_resetFrustumIteratorCallback.m_controller = this;
    m_restartFrustumIteratorCallback.m_controller = this;
    m_completeFrustumIteratorCallback.m_controller = this;
    m_mapToWorldCallback.m_controller = this;

    m_advanceFrustumIterator.m_inputCallback = &m_advanceFrustumIteratorCallback;
    m_advanceFrustumIteratorHold.m_inputCallback = &m_advanceFrustumIteratorHoldCallback;
    m_resetFrustumIterator.m_inputCallback = &m_resetFrustumIteratorCallback;
    m_restartFrustumIterator.m_inputCallback = &m_restartFrustumIteratorCallback;
    m_completeFrustumIterator.m_inputCallback = &m_completeFrustumIteratorCallback;
    m_mapToWorldListener.m_inputCallback = &m_mapToWorldCallback;
    
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_RIGHT), &m_advanceFrustumIterator);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_UP), &m_advanceFrustumIteratorHold);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_LEFT), &m_restartFrustumIterator);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_DOWN), &m_resetFrustumIterator);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_END), &m_completeFrustumIterator);
    m_frustumInputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_HOME), &m_mapToWorldListener);
    
    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_cameraController.m_inputContext);
    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_frustumInputContext);

    m_cameraController.m_speed = 50.0f;
    m_cameraController.m_rollSpeed = 50.0f;
}

FrustumIterVisualizerController::~FrustumIterVisualizerController() {
    //delete[] m_animationTestSkelMats;

    m_engine->m_inputManager->getInputContextStack(0)->popInputContext();
    m_engine->m_inputManager->getInputContextStack(0)->popInputContext();

    m_fontShader.unload();
    m_debugFont.unload();

    delete m_debugShaderLoader;
    delete m_testFrustumIter;
}

void FrustumIterVisualizerController::update(float seconds) {
    m_cameraController.update(seconds);
        
    m_advanceHoldTimer -= seconds;

    if(m_testFrustumIter && m_advanceHold && m_advanceHoldTimer < 0) {
        m_advanceHoldTimer = 0.0f;//0.05f;

        for(unsigned int times = 0; times < 1 && !m_testFrustumIter->atEnd(); times++) {
            m_testFrustumIter->forward();
        }
    }
}

void FrustumIterVisualizerController::updateSound(float seconds) {

}
 
void FrustumIterVisualizerController::render() {
    m_camera.setPerspectiveTransform(m_cameraController.m_transform, m_engine->m_window->getAspectRatio(), illGraphics::DEFAULT_FOV * m_cameraController.m_zoom, illGraphics::DEFAULT_NEAR, 2000.0f);
    
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
    
    //debug draw the frustum iterators
    renderMeshEdgeListDebug(m_testMeshEdgeList, m_camera, m_fontShader, m_debugFont);
    renderMeshEdgeListDebug(m_testUnclippedMeshEdgeList, m_camera, m_fontShader, m_debugFont);
    //renderMeshEdgeListDebug(m_testUnclippedMeshEdgeList);

    //renderSceneDebug(Box<>(glm::vec3(3.0f * -20.0f), glm::vec3(3.0f * 20.0f - 0.1f)), glm::vec3(20.0f), glm::uvec3(6));

    if(m_testFrustumIter) {
        renderFrustumIterDebug(m_testFrustumIter->m_debugger, m_mapToWorld, m_camera, m_fontShader, m_debugFont);
    }
    
    glDepthMask(GL_TRUE);

    ERROR_CHECK_OPENGL;
}

}
