#include <set>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Engine.h"
#include "illEngine/Graphics/Window.h"

#include "MainMenuController.h"
//#include "serial-illUtil/ObjLoader.h"
#include "illEngine/Util/Illmesh/IllmeshLoader.h"
#include "illEngine/Graphics/serial/Material/Shader.h"
#include "illEngine/Input/serial/InputManager.h"

#include "illEngine/GlCommon/glLogging.h"

//TODO: for now I'm testing a bunch of stuff, normally all rendering is done through the renderer
#include <GL/glew.h>

void renderSceneDebug(const Box<>&sceneBounds, const glm::vec3& chunkDimensions, const glm::uvec3& chunkNumber, const illGraphics::Camera& camera) {
   glUseProgram(0);

   glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
   glDepthMask(GL_TRUE);
   glDisable(GL_CULL_FACE);
   glDisable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glMatrixMode(GL_PROJECTION);   
   glLoadMatrixf(glm::value_ptr(camera.getProjection()));

   glMatrixMode(GL_MODELVIEW);
   glLoadMatrixf(glm::value_ptr(camera.getModelView()));

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

/*void renderFrustumIterDebug(const FrustumIterator<>::Debugger& iterator, const illGraphics::Camera& camera) {
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

   //render frustum
   glLineWidth(5.0f);

   glBegin(GL_LINES);

   //inactive edges
   glColor4f(1.0f, 1.0f, 0.0f, 0.25f);

   for(uint8_t edge = 0; edge < FRUSTUM_NUM_EDGES; edge++) {
      if(iterator.m_iterator->m_inactiveEdges[edge]) {
         glVertex3fv(glm::value_ptr(iterator.m_frustum.m_points[FRUSTUM_EDGE_LIST[edge][0]]));
         glVertex3fv(glm::value_ptr(iterator.m_frustum.m_points[FRUSTUM_EDGE_LIST[edge][1]]));
      }
   }

   //active edges
   glColor4f(0.0f, 1.0f, 0.0f, 0.25f);

   for(uint8_t edge = 0; edge < FRUSTUM_NUM_EDGES; edge++) {
      if(iterator.m_iterator->m_activeEdges[edge]) {
         glVertex3fv(glm::value_ptr(iterator.m_frustum.m_points[FRUSTUM_EDGE_LIST[edge][0]]));
         glVertex3fv(glm::value_ptr(iterator.m_frustum.m_points[FRUSTUM_EDGE_LIST[edge][1]]));
      }
   }

   //inactive edges
   glColor4f(1.0f, 0.0f, 0.0f, 0.25f);

   for(uint8_t edge = 0; edge < FRUSTUM_NUM_EDGES; edge++) {
      if(iterator.m_discarededEdges[edge]) {
         glVertex3fv(glm::value_ptr(iterator.m_frustum.m_points[FRUSTUM_EDGE_LIST[edge][0]]));
         glVertex3fv(glm::value_ptr(iterator.m_frustum.m_points[FRUSTUM_EDGE_LIST[edge][1]]));
      }
   }

   glEnd();

   glLineWidth(1.0f);
      
   //the direction
   drawVec = iterator.m_frustum.m_nearTipPoint + iterator.m_frustum.m_direction * 10.0f;

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
   glEnd();

   //the bounds
   glBegin(GL_LINES);
      glColor4f(0.0f, 0.5f, 0.0f, 0.5f);

      if(iterator.m_iterator->m_dimensionOrder[0] == 0) {
         glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
      }
      else if(iterator.m_iterator->m_dimensionOrder[1] == 0) {
         glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
      }
      else if(iterator.m_iterator->m_dimensionOrder[2] == 0) {
         glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
      }

      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);

      if(iterator.m_iterator->m_dimensionOrder[0] == 1) {
         glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
      }
      else if(iterator.m_iterator->m_dimensionOrder[1] == 1) {
         glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
      }
      else if(iterator.m_iterator->m_dimensionOrder[2] == 1) {
         glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
      }

      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);

      if(iterator.m_iterator->m_dimensionOrder[0] == 2) {
         glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
      }
      else if(iterator.m_iterator->m_dimensionOrder[1] == 2) {
         glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
      }
      else if(iterator.m_iterator->m_dimensionOrder[2] == 2) {
         glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
      }

      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);


      glColor4f(0.0f, 1.0f, 1.0f, 0.2f);
      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);

      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);

      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);


      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);

      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);

      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);


      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_min.z * iterator.m_iterator->m_cellDimensions.z);

      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_max.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);

      glVertex3f(iterator.m_iterator->m_range.m_max.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);
      glVertex3f(iterator.m_iterator->m_range.m_min.x * iterator.m_iterator->m_cellDimensions.x, iterator.m_iterator->m_range.m_min.y * iterator.m_iterator->m_cellDimensions.y, iterator.m_iterator->m_range.m_max.z * iterator.m_iterator->m_cellDimensions.z);
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

         drawPoint[iterator.m_iterator->m_dimensionOrder[0]] = iterator.m_iterator->m_sliceStart;
         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[1]];
         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[2]];

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[1]];

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[2]];

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[1]];

            glVertex3fv(glm::value_ptr(drawPoint));

      //back plane
      glColor4f(1.0f, 1.0f, 1.0f, 0.1f);

         drawPoint[iterator.m_iterator->m_dimensionOrder[0]] = iterator.m_iterator->m_sliceStart + iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[0]] * iterator.m_iterator->m_directionSign[iterator.m_iterator->m_dimensionOrder[0]];
         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[1]];
         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[2]];

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[1]];

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[2]];

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[1]];

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
      sign = iterator.m_iterator->m_directionSign[iterator.m_iterator->m_dimensionOrder[1]];

      start = sign >= 0
         ? iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[1]]
         : iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[1]];

      dimensions = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[1]];

      numLines = glm::abs((int) iterator.m_iterator->m_range.m_max[iterator.m_iterator->m_dimensionOrder[1]] - (int) iterator.m_iterator->m_range.m_min[iterator.m_iterator->m_dimensionOrder[1]]);

      for(int line = 1; line <= numLines; line ++) {
         glm::vec3 drawPoint;

         drawPoint[iterator.m_iterator->m_dimensionOrder[0]] = iterator.m_iterator->m_sliceStart;
         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = start + line * dimensions;
         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_range.m_min[iterator.m_iterator->m_dimensionOrder[2]] * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[2]];

         glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_range.m_max[iterator.m_iterator->m_dimensionOrder[2]] * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[2]];

         glVertex3fv(glm::value_ptr(drawPoint));
      }
         
      sign = iterator.m_iterator->m_directionSign[iterator.m_iterator->m_dimensionOrder[2]];

      start = sign >= 0
         ? iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[2]]
         : iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[2]];

      dimensions = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[2]];

      numLines = glm::abs((int) iterator.m_iterator->m_range.m_max[iterator.m_iterator->m_dimensionOrder[2]] - (int) iterator.m_iterator->m_range.m_min[iterator.m_iterator->m_dimensionOrder[2]]);

      for(int line = 1; line <= numLines; line ++) {
         glm::vec3 drawPoint;

         drawPoint[iterator.m_iterator->m_dimensionOrder[0]] = iterator.m_iterator->m_sliceStart;
         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_range.m_min[iterator.m_iterator->m_dimensionOrder[1]] * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[1]];
         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = start + line * dimensions;

         glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_range.m_max[iterator.m_iterator->m_dimensionOrder[1]] * iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[1]];

         glVertex3fv(glm::value_ptr(drawPoint));
      }
   }
   glEnd();
   
   //current row rasterizing debug
   glLineWidth(3.0f);

   glBegin(GL_LINES);
   {
      glm::vec3 drawPoint;

      //row bottom
      glColor4f(1.0f, 1.0f, 0.0f, 0.2f);

         drawPoint[iterator.m_iterator->m_dimensionOrder[0]] = iterator.m_iterator->m_sliceStart;
         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_lineBottom;
         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[2]];

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[2]];

            glVertex3fv(glm::value_ptr(drawPoint));
         
      //row top
      glColor4f(1.0f, 1.0f, 0.0f, 0.6f);

         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_lineTop;
         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[2]];

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[2]];

            glVertex3fv(glm::value_ptr(drawPoint));

      //side bounds
      glColor4f(0.0f, 1.0f, 1.0f, 0.2f);

         drawPoint[iterator.m_iterator->m_dimensionOrder[0]] = iterator.m_iterator->m_sliceStart + 2.0f;

         //min
         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[2]];
         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[1]] * iterator.m_sliceMin.x;         

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[2]];

            glVertex3fv(glm::value_ptr(drawPoint));

      glColor4f(0.0f, 1.0f, 1.0f, 0.6f);

         //max
         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[2]];
         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[1]] * iterator.m_iterator->m_sliceMax.x;         

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[2]];

            glVertex3fv(glm::value_ptr(drawPoint));

      //top and bottom bounds
      glColor4f(0.0f, 1.0f, 1.0f, 0.2f);

         //min
         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[2]] * iterator.m_sliceMin.y;
         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[1]];         

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[1]];

            glVertex3fv(glm::value_ptr(drawPoint));

      glColor4f(0.0f, 1.0f, 1.0f, 0.6f);

         //max
         drawPoint[iterator.m_iterator->m_dimensionOrder[1]] = iterator.m_iterator->m_cellDimensions[iterator.m_iterator->m_dimensionOrder[2]] * iterator.m_iterator->m_sliceMax.y;
         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_min[iterator.m_iterator->m_dimensionOrder[1]];         

            glVertex3fv(glm::value_ptr(drawPoint));

         drawPoint[iterator.m_iterator->m_dimensionOrder[2]] = iterator.m_iterator->m_spaceRange.m_max[iterator.m_iterator->m_dimensionOrder[1]];

            glVertex3fv(glm::value_ptr(drawPoint));
   }
   glEnd();

   //the 3D line/slice intersection points
   glPointSize(10.0f);
   
   glBegin(GL_POINTS);

   glColor4f(1.0f, 1.0f, 1.0f, .5f);

   for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_pointList[!iterator.m_iterator->m_currentPointList].m_size; elementIndex++) {
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(iterator.m_iterator->m_pointList[!iterator.m_iterator->m_currentPointList].m_data[elementIndex], iterator.m_pointListMissingDim[!iterator.m_iterator->m_currentPointList].m_data[elementIndex])));
   }

   glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

   for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_pointList[iterator.m_iterator->m_currentPointList].m_size; elementIndex++) {
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(iterator.m_iterator->m_pointList[iterator.m_iterator->m_currentPointList].m_data[elementIndex], iterator.m_pointListMissingDim[iterator.m_iterator->m_currentPointList].m_data[elementIndex])));
   }

   glEnd();

   //line connecting polygon points in sorted order

   glBegin(GL_LINES);

   for(std::list<glm::vec2*>::const_iterator iter = iterator.m_sortedSlicePoints.begin(); iter != iterator.m_sortedSlicePoints.end(); ) {
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

   //the unclipped convex hull polygon slice
   glBegin(GL_LINES);

   //"right"
   for(uint8_t elementIndex = 0; elementIndex < iterator.m_unclippedRasterizeEdges[0].m_size;) {
      glColor4f(0.7f, 0.7f, 1.0f, 0.3f);
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(*iterator.m_unclippedRasterizeEdges[0].m_data[elementIndex], iterator.m_iterator->m_sliceStart)));

      elementIndex++;

      if(elementIndex == iterator.m_unclippedRasterizeEdges[0].m_size) {
         break;
      }

      glColor4f(0.7f, 0.7f, 1.0f, 0.7f);
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(*iterator.m_unclippedRasterizeEdges[0].m_data[elementIndex], iterator.m_iterator->m_sliceStart)));
   }

   glEnd();


   glBegin(GL_LINES);

   //"left"
   for(int8_t elementIndex = iterator.m_unclippedRasterizeEdges[1].m_size - 1; elementIndex >= 0;) {
      glColor4f(0.3f, 0.3f, 1.0f, 0.3f);
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(*iterator.m_unclippedRasterizeEdges[1].m_data[elementIndex], iterator.m_iterator->m_sliceStart)));

      elementIndex--;

      if(elementIndex == -1) {
         break;
      }

      glColor4f(0.3f, 0.3f, 1.0f, 0.7f);
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(*iterator.m_unclippedRasterizeEdges[1].m_data[elementIndex], iterator.m_iterator->m_sliceStart)));
   }

   glEnd();

   //the clipped convex hull polygon slice
   glLineWidth(20.0f);

   glBegin(GL_LINES);

   //"right"
   for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[0].m_size;) {
      glColor4f(0.7f, 0.7f, 1.0f, 0.3f);
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(iterator.m_iterator->m_sliceRasterizeEdges[0].m_data[elementIndex], iterator.m_iterator->m_sliceStart)));

      elementIndex++;

      if(elementIndex == iterator.m_iterator->m_sliceRasterizeEdges[0].m_size) {
         break;
      }

      glColor4f(0.7f, 0.7f, 1.0f, 1.0f);
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(iterator.m_iterator->m_sliceRasterizeEdges[0].m_data[elementIndex], iterator.m_iterator->m_sliceStart)));
   }

   glEnd();


   glBegin(GL_LINES);

   //"left"
   for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[1].m_size;) {
      glColor4f(0.3f, 0.3f, 1.0f, 0.3f);
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(iterator.m_iterator->m_sliceRasterizeEdges[1].m_data[elementIndex], iterator.m_iterator->m_sliceStart)));

      elementIndex++;

      if(elementIndex == iterator.m_iterator->m_sliceRasterizeEdges[1].m_size) {
         break;
      }

      glColor4f(0.3f, 0.3f, 1.0f, 1.0f);
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(iterator.m_iterator->m_sliceRasterizeEdges[1].m_data[elementIndex], iterator.m_iterator->m_sliceStart)));
   }

   glEnd();

   //some of the clipped points for debugging purposes
   glPointSize(15.0f);

   glBegin(GL_POINTS);

   glColor4f(0.7f, 0.7f, 0.7f, 0.3f);
   for(std::list<glm::detail::tvec2<glm::mediump_float> >::const_iterator iter = iterator.m_clipPoints.begin(); iter != iterator.m_clipPoints.end(); iter++) {
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(*iter, iterator.m_iterator->m_sliceStart)));
   }

   glEnd();

   //the points
   glPointSize(20.0f);

   glBegin(GL_POINTS);

   //right
   glColor4f(0.7f, 0.7f, 1.0f, 0.3f);
   for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[0].m_size; elementIndex++) {      
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(iterator.m_iterator->m_sliceRasterizeEdges[0].m_data[elementIndex], iterator.m_iterator->m_sliceStart)));
   }

   //left
   glColor4f(0.3f, 0.3f, 1.0f, 0.3f);
   for(uint8_t elementIndex = 0; elementIndex < iterator.m_iterator->m_sliceRasterizeEdges[1].m_size; elementIndex++) {      
      glVertex3fv(glm::value_ptr(iterator.sliceTo3D(iterator.m_iterator->m_sliceRasterizeEdges[1].m_data[elementIndex], iterator.m_iterator->m_sliceStart)));
   }

   glEnd();

   //draw the so far rasterized points
   glPointSize(30.0f);

   glBegin(GL_POINTS);

   glColor4f(1.0f, 1.0f, 1.0f, 0.2f);

   for(std::list<glm::detail::tvec3<glm::mediump_float> >::const_iterator iter = iterator.m_rasterizedCells.begin(); iter != iterator.m_rasterizedCells.end(); iter++) {
      glVertex3fv(glm::value_ptr(*iter));
   }

   glEnd();

   //draw the current point

   glBegin(GL_POINTS);

   glColor4f(1.0f, 0.0f, 0.0f, 0.5f);

   glVertex3fv(glm::value_ptr(iterator.m_iterator->m_cellDimensions * vec3cast<int, glm::mediump_float>(iterator.m_iterator->m_currentPosition) + (iterator.m_iterator->m_cellDimensions * 0.5f * vec3cast<int8_t, glm::mediump_float>(iterator.m_iterator->m_directionSign))));

   glEnd();

   glLineWidth(1.0f);
   
   glShadeModel(GL_FLAT);

   glPointSize(1.0f);
}*/

void renderMesh(illGraphics::Mesh& mesh, illGraphics::ModelAnimationController& controller, const illGraphics::Camera& camera, const glm::mat4& xform, GLuint prog) {
    
    GLint loc = glGetUniformLocation(prog, "modelViewProjectionMatrix");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown uniform modelViewProjectionMatrix");
    }
    glUniformMatrix4fv(loc, 1, false, glm::value_ptr(camera.getCanonical() * xform));

    loc = glGetUniformLocation(prog, "modelViewMatrix");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown uniform modelViewMatrix");
    }
    glUniformMatrix4fv(loc, 1, false, glm::value_ptr(camera.getModelView()* xform));
    
    

    GLuint buffer = *((GLuint *) mesh.getMeshBackendData() + 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    loc = glGetUniformLocation(prog, "bones");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown uniform bones");
    }
    glUniformMatrix4fv(loc, controller.m_skeleton->getNumBones(), false, &controller.m_skelMats[0][0][0]);

    loc = glGetAttribLocation(prog, "position");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown attrib position");
    }
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getPositionOffset());
    glEnableVertexAttribArray(loc);

    loc = glGetAttribLocation(prog, "texCoords");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown attrib texCoords");
    }
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getTexCoordOffset());
    glEnableVertexAttribArray(loc);

    loc = glGetAttribLocation(prog, "normal");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown attrib normal");
    }
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getNormalOffset());
    glEnableVertexAttribArray(loc);

    loc = glGetAttribLocation(prog, "tangent");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown attrib tangent");
    }
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getTangentOffset());
    glEnableVertexAttribArray(loc);

    loc = glGetAttribLocation(prog, "bitangent");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown attrib bitangent");
    }
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getBitangentOffset());
    glEnableVertexAttribArray(loc);

    loc = glGetAttribLocation(prog, "boneIndices");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown attrib boneIndices");
    }
    glVertexAttribIPointer(loc, 4, GL_INT, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getBlendIndexOffset());
    glEnableVertexAttribArray(loc);

    loc = glGetAttribLocation(prog, "weights");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown attrib weights");
    }
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, (GLsizei) mesh.m_meshFrontendData->getVertexSize(), (char *)NULL + mesh.m_meshFrontendData->getBlendWeightOffset());
    glEnableVertexAttribArray(loc);

    buffer = *((GLuint *) mesh.getMeshBackendData() + 1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

    glDrawRangeElements(GL_TRIANGLES, 0, mesh.m_meshFrontendData->getNumTri() * 3, mesh.m_meshFrontendData->getNumTri() * 3, GL_UNSIGNED_INT, (char *)NULL);

    /*int start = 2400 * 3;
    int num = 100 * 3;
    int end = start + num;
    glDrawRangeElements(GL_TRIANGLES, start, end, num, GL_UNSIGNED_INT, (char *)NULL + start);*/
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

namespace Demo {

void MainMenuController::ResetFrustumIterator::onRelease() {
   illGraphics::Camera testCam;
   testCam.setTransform(m_controller->m_camera.getTransform(), m_controller->m_engine->m_window->getAspectRatio(), illGraphics::DEFAULT_FOV, 100.0f, 300.0f);
   //m_controller->m_testFrustumIter = new FrustumIterator<>(&testCam.getViewFrustum(), Box<int>(glm::ivec3(0), glm::ivec3(20)), glm::vec3(10.0f));
}

void MainMenuController::RestartFrustumIterator::onRelease() {
   //m_controller->m_testFrustumIter = new FrustumIterator<>(m_controller->m_testFrustumIter->m_frustum, Box<int>(glm::ivec3(0), glm::ivec3(20)), glm::vec3(10.0f));
}

MainMenuController::MainMenuController(Engine * engine)
    : GameControllerBase(),
    m_engine(engine)
{
    //This is all put together to test some stuff, this is in no way how to normally do these things.  Everything should normally be done through the renderer front end when that's done.

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
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

        m_marineDiffuse.load(loadArgs, m_engine->m_rendererBackend);
    }

    //marine normal map
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/marine_local.tga";
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

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
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

        m_helmetNormal.load(loadArgs, m_engine->m_rendererBackend);
    }
    
    //diffuse helmet texture
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/helmet.tga";
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

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
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

        m_hellKnightDiffuse.load(loadArgs, m_engine->m_rendererBackend);
    }

    //marine normal map
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/HellKnight/hellknight_local.tga";
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

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
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

        m_demonDiffuse.load(loadArgs, m_engine->m_rendererBackend);
    }

    //marine normal map
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Demon/pinky_local.tga";
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

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

    //initialize the input (this would normally initialize using console variables)
    m_engine->m_inputManager->addPlayer(0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_KEYBOARD, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE_BUTTON, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE_WHEEL, 0);

    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_cameraController.m_inputContext);
    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_frustumInputContext);

    m_cameraController.m_speed = 50.0f;
    m_cameraController.m_rollSpeed = 50.0f;
}

MainMenuController::~MainMenuController() {
    //delete[] m_animationTestSkelMats;
    delete m_debugShaderLoader;
}

void MainMenuController::update(float seconds) {
    m_cameraController.update(seconds);

    m_marineController.update(seconds);

    m_hellKnightController0.update(seconds);
    m_hellKnightController1.update(seconds * 0.5f);
    m_hellKnightController2.update(seconds * 0.1f);

    m_demonController0.update(seconds);
    m_demonController1.update(seconds * 0.5f);
    m_demonController2.update(seconds * 0.2f);
    m_demonController3.update(seconds * 0.1f);
}

void MainMenuController::updateSound(float seconds) {

}
 
void MainMenuController::render() {
    m_cameraTransform.m_transform = m_cameraController.m_transform;
    m_camera.setTransform(m_cameraTransform, m_engine->m_window->getAspectRatio(), illGraphics::DEFAULT_FOV * m_cameraController.m_zoom, illGraphics::DEFAULT_NEAR, 2000.0f);
    
    m_marineController.computeAnimPose();

    m_hellKnightController0.computeAnimPose();
    m_hellKnightController1.computeAnimPose();
    m_hellKnightController2.computeAnimPose();

    m_demonController0.computeAnimPose();
    m_demonController1.computeAnimPose();
    m_demonController2.computeAnimPose();
    m_demonController3.computeAnimPose();

    //debug drawing
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    //glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(0);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(m_camera.getProjection()));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(m_camera.getModelView()));

    //debug draw the axes
    glBegin(GL_LINES);
    //x Red
        glColor4f(1.0f, 0.0f, 0.0f, 0.1f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(5.0f, 0.0f, 0.0f);

    //y Green
        glColor4f(0.0f, 1.0f, 0.0f, 0.1f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 5.0f, 0.0f);

    //z Blue
        glColor4f(0.0f, 0.0f, 1.0f, 0.1f);
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
        
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);    
    glDisable(GL_BLEND);

    //TODO: for now I'm testing a bunch of stuff, normally all rendering is done through the renderer   
    //render the lil mesh
    //glEnable(GL_DEPTH_TEST);
    GLuint prog = *((GLuint *) m_debugShader.getShaderProgram());

    glUseProgram(prog);

    GLint loc = glGetUniformLocation(prog, "normalMatrix");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown uniform normalMatrix");
    }
    glUniformMatrix3fv(loc, 1, false, glm::value_ptr(m_camera.getNormal()));
    
    loc = glGetUniformLocation(prog, "diffuseMap");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown uniform diffuseMap");
    }

    glm::mat4 xform;

    //draw marine body
    xform = glm::translate(glm::vec3(500.0f, 0.0f, 0.0f));

    glActiveTexture(GL_TEXTURE0);
    GLuint texture = *((GLuint *) m_marineDiffuse.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(loc, 0);

    loc = glGetUniformLocation(prog, "normalMap");
    if(loc == -1) {
        LOG_FATAL_ERROR("Unknown uniform normalMap");
    }

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
    renderMesh(m_demonFront, m_demonController3, m_camera, xform, prog);

    ERROR_CHECK_OPENGL;
}

}
