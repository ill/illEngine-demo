#include <set>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Engine.h"
#include "illEngine/Graphics/Window.h"

#include "SkeletalAnimationDemoController.h"
#include "illEngine/Util/Illmesh/IllmeshLoader.h"
#include "illEngine/Graphics/serial/Material/Shader.h"
#include "illEngine/Graphics/serial/BitmapFont.h"
#include "illEngine/Input/serial/InputManager.h"

#include "illEngine/GlCommon/glLogging.h"
#include "illEngine/GlCommon/serial/glUtil.h"

//TODO: for now I'm testing a bunch of stuff, normally all rendering is done through the renderer
#include <GL/glew.h>

void renderMesh(illGraphics::Mesh& mesh, illGraphics::ModelAnimationController& controller, const glm::mat4 * boneTransforms, const illGraphics::Camera& camera, const glm::mat4& xform, GLuint prog) {    
    GLint loc = getProgramUniformLocation(prog, "modelViewProjectionMatrix");
    glUniformMatrix4fv(loc, 1, false, glm::value_ptr(camera.getModelViewProjection() * xform));

    loc = getProgramUniformLocation(prog, "modelViewMatrix");
    glUniformMatrix4fv(loc, 1, false, glm::value_ptr(camera.getModelView() * xform));
    
    GLuint buffer = *((GLuint *) mesh.getMeshBackendData() + 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    loc = getProgramUniformLocation(prog, "bones");
	glUniformMatrix4fv(loc, (GLsizei) controller.getNumBones(), false, &boneTransforms[0][0][0]);

    GLint pos = getProgramAttribLocation(prog, "position");
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.getMeshFrontentData()->getVertexSize(), (char *)NULL + mesh.getMeshFrontentData()->getPositionOffset());
    glEnableVertexAttribArray(pos);

    GLint tex = getProgramAttribLocation(prog, "texCoords");
    glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, (GLsizei) mesh.getMeshFrontentData()->getVertexSize(), (char *)NULL + mesh.getMeshFrontentData()->getTexCoordOffset());
    glEnableVertexAttribArray(tex);

    GLint norm = getProgramAttribLocation(prog, "normal");
    glVertexAttribPointer(norm, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.getMeshFrontentData()->getVertexSize(), (char *)NULL + mesh.getMeshFrontentData()->getNormalOffset());
    glEnableVertexAttribArray(norm);

    GLint tan = getProgramAttribLocation(prog, "tangent");
    glVertexAttribPointer(tan, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.getMeshFrontentData()->getVertexSize(), (char *)NULL + mesh.getMeshFrontentData()->getTangentOffset());
    glEnableVertexAttribArray(tan);

    GLint bitan = getProgramAttribLocation(prog, "bitangent");
    glVertexAttribPointer(bitan, 3, GL_FLOAT, GL_FALSE, (GLsizei) mesh.getMeshFrontentData()->getVertexSize(), (char *)NULL + mesh.getMeshFrontentData()->getBitangentOffset());
    glEnableVertexAttribArray(bitan);

    GLint bonei = getProgramAttribLocation(prog, "boneIndices");
    glVertexAttribIPointer(bonei, 4, GL_INT, (GLsizei) mesh.getMeshFrontentData()->getVertexSize(), (char *)NULL + mesh.getMeshFrontentData()->getBlendIndexOffset());
    glEnableVertexAttribArray(bonei);

    GLint weights = getProgramAttribLocation(prog, "weights");
    glVertexAttribPointer(weights, 4, GL_FLOAT, GL_FALSE, (GLsizei) mesh.getMeshFrontentData()->getVertexSize(), (char *)NULL + mesh.getMeshFrontentData()->getBlendWeightOffset());
    glEnableVertexAttribArray(weights);

    buffer = *((GLuint *) mesh.getMeshBackendData() + 1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

    glDrawRangeElements(GL_TRIANGLES, 0, mesh.getMeshFrontentData()->getNumTri() * 3, mesh.getMeshFrontentData()->getNumTri() * 3, GL_UNSIGNED_SHORT, (char *)NULL);

    glDisableVertexAttribArray(pos);
    glDisableVertexAttribArray(norm);
    glDisableVertexAttribArray(tex);
    glDisableVertexAttribArray(tan);
    glDisableVertexAttribArray(bitan);
    //glDisableVertexAttribArray(bonei);
    //glDisableVertexAttribArray(weights);
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

//TODO: this doesn't work at all right now
void renderSkeleton(const illGraphics::Skeleton& skeleton, const illGraphics::Skeleton::BoneHeirarchy * currNode, const illGraphics::ModelAnimationController& animationController, glm::mat4 currXform, glm::mat4 currBindXform) {
	Transform<> transform[2];

	//for the primary animation
	if(animationController.m_animations[animationController.m_currentAnimation].m_animation) {
		transform[0] = animationController.m_animations[animationController.m_currentAnimation].m_animation->getTransform(currNode->m_boneIndex,
			animationController.m_animations[animationController.m_currentAnimation].m_animTime);
	}
	else {
		new(&transform[0]) Transform<>();
	}

	//for the secondary animation, see if the bone is in the animation
	if(animationController.m_transitionWeight > 0.0f) {
		if(animationController.m_animations[!animationController.m_currentAnimation].m_animation) {
			transform[1] = animationController.m_animations[!animationController.m_currentAnimation].m_animation->getTransform(currNode->m_boneIndex,
				animationController.m_animations[!animationController.m_currentAnimation].m_animTime);
		}
		else {
			new(&transform[1]) Transform<>();
		}

		//now blend them
		transform[0] = transform[0].interpolate(transform[1], animationController.m_transitionWeight);
	}
	
	
	
	glm::mat4 prevXform = currXform;
	glm::mat4 prevBindXform = currBindXform;

	currXform = currXform * skeleton.getBone(currNode->m_boneIndex).m_relativeTransform * transform[0].getMatrix();
	currBindXform = currBindXform * skeleton.getBone(currNode->m_boneIndex).m_relativeTransform;
	
	debugDrawBone(currXform, prevXform, currNode->m_parent != NULL);
	debugDrawBone(currBindXform, prevBindXform, currNode->m_parent != NULL);

    for(std::vector<illGraphics::Skeleton::BoneHeirarchy *>::const_iterator iter = currNode->m_children.begin(); iter != currNode->m_children.end(); iter++) {
        renderSkeleton(skeleton, *iter, animationController, currXform, currBindXform);
    }
}

void renderMeshDebug(const illGraphics::Mesh& mesh, const glm::mat4 * boneTransforms, const illGraphics::ModelAnimationController& controller, const glm::mat4& xform) {
    glPointSize(5.0f);
    
    for(unsigned int vertex = 0; vertex < mesh.getMeshFrontentData()->getNumVert(); vertex++) {
        glm::mat4 transformedMat = boneTransforms[(int) mesh.getMeshFrontentData()->getBlendData(vertex).m_blendIndex[0]] * mesh.getMeshFrontentData()->getBlendData(vertex).m_blendWeight[0];
        transformedMat += boneTransforms[(int) mesh.getMeshFrontentData()->getBlendData(vertex).m_blendIndex[1]] * mesh.getMeshFrontentData()->getBlendData(vertex).m_blendWeight[1];
        transformedMat += boneTransforms[(int) mesh.getMeshFrontentData()->getBlendData(vertex).m_blendIndex[2]] * mesh.getMeshFrontentData()->getBlendData(vertex).m_blendWeight[2];
        transformedMat += boneTransforms[(int) mesh.getMeshFrontentData()->getBlendData(vertex).m_blendIndex[3]] * mesh.getMeshFrontentData()->getBlendData(vertex).m_blendWeight[3];
        
        glm::vec4 pos = xform * transformedMat * glm::vec4(mesh.getMeshFrontentData()->getPosition(vertex), 1.0f);
        
        //transformed point
        glBegin(GL_POINTS);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(pos));

        glEnd();

        glBegin(GL_LINES);

        glm::vec3 tail;

        //normal
        glm::vec3 skinned = glm::mat3(xform) * glm::mat3(transformedMat) * mesh.getMeshFrontentData()->getNormal(vertex);
        
        tail = glm::vec3(pos) + glm::vec3(skinned) * 10.0f;

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3fv(glm::value_ptr(pos));
        glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
        glVertex3fv(glm::value_ptr(tail));

        //tangent
        skinned = glm::mat3(xform) * glm::mat3(transformedMat) * mesh.getMeshFrontentData()->getTangent(vertex).m_tangent;
        
        tail = glm::vec3(pos) + glm::vec3(skinned) * 10.0f;

        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertex3fv(glm::value_ptr(pos));
        glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
        glVertex3fv(glm::value_ptr(tail));

        //bitangent
        skinned = glm::mat3(xform) * glm::mat3(transformedMat) * mesh.getMeshFrontentData()->getTangent(vertex).m_bitangent;
        
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

SkeletalAnimationDemoController::SkeletalAnimationDemoController(Engine * engine)
    : GameControllerBase(),
    m_engine(engine),
    m_lightPos(glm::vec3(0.0f)),
    m_forward(false),
    m_back(false),
    m_left(false),
    m_right(false),
    m_up(false),
    m_down(false)
{
    //This is all put together to test some stuff, this is in no way how to normally do these things.  Everything should normally be done through the renderer front end when that's done.
    
    //marine body
    {
        IllmeshLoader meshLoader("Meshes/Marine/marine8.illmesh");

        m_marine.setFrontentDataInternal(new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features));
    
        meshLoader.buildMesh(*m_marine.getMeshFrontentData());
        m_marine.frontendBackendTransferInternal(m_engine->m_graphicsBackend, false);
    }

    //load the diffuse texture
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/marine.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;

        m_marineDiffuse.load(loadArgs, m_engine->m_graphicsBackend);
    }

    //marine normal map
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/marine_local.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;

        m_marineNormal.load(loadArgs, m_engine->m_graphicsBackend);
    }




    //marine helmet
    {
        IllmeshLoader meshLoader("Meshes/Marine/marine.illmesh");

        m_marineHelmet.setFrontentDataInternal(new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features));
    
        meshLoader.buildMesh(*m_marineHelmet.getMeshFrontentData());
        m_marineHelmet.frontendBackendTransferInternal(m_engine->m_graphicsBackend, false);
    }

    //helmet normal map
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/helmet_local.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;

        m_helmetNormal.load(loadArgs, m_engine->m_graphicsBackend);
    }
    
    //diffuse helmet texture
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/helmet.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;

        m_helmetDiffuse.load(loadArgs, m_engine->m_graphicsBackend);
    }




    //load the skeleton
    {
        illGraphics::SkeletonLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/marine.illskel";
        m_marineSkeleton.load(loadArgs, NULL);
		//m_boneMatrixBuffer.reserve(m_marineSkeleton.getNumBones());
    }

    //load the animation
    {
        illGraphics::SkeletonAnimationLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/run.illanim";
        m_marineAnimation.load(loadArgs, NULL);

        loadArgs.m_path = "Meshes/Marine/strafeLeft.illanim";
        m_marineStrafeAnimation.load(loadArgs, NULL);

        loadArgs.m_path = "Meshes/Marine/plasmaAim.illanim";
        m_marineAimAnimation.load(loadArgs, NULL);

        loadArgs.m_path = "Meshes/Marine/plasmaAimUp.illanim";
        m_marineAimUpAnimation.load(loadArgs, NULL);

        loadArgs.m_path = "Meshes/Marine/plasmaAimDown.illanim";
        m_marineAimDownAnimation.load(loadArgs, NULL);

        m_marineController.setSkeleton(&m_marineSkeleton);
        m_marineController.queueTransition(&m_marineAimUpAnimation, 0.0f, 0.0f);
        m_marineController.queueTransition(&m_marineAimAnimation, 1.0f, 5.0f);
        m_marineController.queueTransition(&m_marineAimDownAnimation, 1.0f, 2.0f);
        m_marineController.queueTransition(&m_marineAimAnimation, 1.0f, 2.0f);

        m_marineController.queueTransition(&m_marineAimUpAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimDownAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimAnimation, 10.0f, 10.0f);

		m_marineController.queueTransition(&m_marineAimUpAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimDownAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimAnimation, 10.0f, 10.0f);

		m_marineController.queueTransition(&m_marineAimUpAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimDownAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimAnimation, 10.0f, 10.0f);

		m_marineController.queueTransition(&m_marineAimUpAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimDownAnimation, 10.0f, 10.0f);
        m_marineController.queueTransition(&m_marineAimAnimation, 10.0f, 10.0f);


        //m_marineController.queueTransition(&m_marineAnimation, 2.0f, 5.0f);
        //m_marineController.queueTransition(&m_marineStrafeAnimation, 10.0f, 10.0f);

		/*m_marineController.init(&m_marineSkeleton);
        m_marineController.queueTransition(&m_marineAnimation, 10.0f, 0.0f);
		m_marineController.queueTransition(NULL, 10.0f, 20.0f);
		m_marineController.queueTransition(&m_marineAnimation, 10.0f, 20.0f);
		m_marineController.queueTransition(NULL, 10.0f, 20.0f);
		m_marineController.queueTransition(&m_marineAnimation, 10.0f, 20.0f);
		m_marineController.queueTransition(NULL, 10.0f, 20.0f);
		m_marineController.queueTransition(&m_marineAnimation, 10.0f, 20.0f);
		m_marineController.queueTransition(NULL, 10.0f, 20.0f);*/
    }





    //hellknight
    {
        IllmeshLoader meshLoader("Meshes/HellKnight/hellknight.illmesh");

        m_hellKnight.setFrontentDataInternal(new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features));
    
        meshLoader.buildMesh(*m_hellKnight.getMeshFrontentData());
        m_hellKnight.frontendBackendTransferInternal(m_engine->m_graphicsBackend, false);
    }

    //load the diffuse texture
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/HellKnight/hellknight.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;

        m_hellKnightDiffuse.load(loadArgs, m_engine->m_graphicsBackend);
    }

    //marine normal map
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/HellKnight/hellknight_local.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;

        m_hellKnightNormal.load(loadArgs, m_engine->m_graphicsBackend);
    }

    //load the skeleton
    {
        illGraphics::SkeletonLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/HellKnight/hellknight.illskel";
        m_hellKnightSkeleton.load(loadArgs, NULL);
		//m_boneMatrixBuffer.reserve(m_hellKnightSkeleton.getNumBones());
    }

    //load the animation
    {
        illGraphics::SkeletonAnimationLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/HellKnight/hellknight.illanim";
        m_hellKnightAnimation.load(loadArgs, NULL);

		m_hellKnightController0.setSkeleton(&m_hellKnightSkeleton);
		m_hellKnightController1.setSkeleton(&m_hellKnightSkeleton);
		m_hellKnightController2.setSkeleton(&m_hellKnightSkeleton);

        m_hellKnightController0.queueTransition(&m_hellKnightAnimation, 0.0f, 0.0f);
        m_hellKnightController1.queueTransition(&m_hellKnightAnimation, 0.0f, 0.0f, 1.0f);
        m_hellKnightController2.queueTransition(&m_hellKnightAnimation, 0.0f, 0.0f, 1.5f);
    }




    //demon
    {
        IllmeshLoader meshLoader("Meshes/Demon/demon.illmesh");

        m_demon.setFrontentDataInternal(new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features));
    
        meshLoader.buildMesh(*m_demon.getMeshFrontentData());
        m_demon.frontendBackendTransferInternal(m_engine->m_graphicsBackend, false);
    }

    //load the diffuse texture
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Demon/pinky_d.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;

        m_demonDiffuse.load(loadArgs, m_engine->m_graphicsBackend);
    }

    //demon normal map
    {
        illGraphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Demon/pinky_local.tga";
        loadArgs.m_wrapS = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;
        loadArgs.m_wrapT = illGraphics::TextureLoadArgs::Wrap::W_CLAMP_TO_EDGE;

        m_demonNormal.load(loadArgs, m_engine->m_graphicsBackend);
    }
    
    //demon front
    {
        IllmeshLoader meshLoader("meshes/demon/demon0.illmesh");

        m_demonFront.setFrontentDataInternal(new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features));
    
        meshLoader.buildMesh(*m_demonFront.getMeshFrontentData());
        m_demonFront.frontendBackendTransferInternal(m_engine->m_graphicsBackend, false);
    }

    //load the skeleton
    {
        illGraphics::SkeletonLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Demon/demon.illskel";
        m_demonSkeleton.load(loadArgs, NULL);
		m_boneMatrixBuffer.reserve(m_demonSkeleton.getNumBones());
    }

    //load the animation
    {
        illGraphics::SkeletonAnimationLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Demon/demon.illanim";
        m_demonAnimation.load(loadArgs, NULL);

		m_demonController0.setSkeleton(&m_demonSkeleton);
		m_demonController1.setSkeleton(&m_demonSkeleton);
		m_demonController2.setSkeleton(&m_demonSkeleton);
		m_demonController3.setSkeleton(&m_demonSkeleton);

		m_demonController0.queueTransition(&m_demonAnimation, 0.0f, 0.0f);
        m_demonController1.queueTransition(&m_demonAnimation, 0.0f, 0.0f, 0.5f);
        m_demonController2.queueTransition(&m_demonAnimation, 0.0f, 0.0f, 0.75f);
        m_demonController3.queueTransition(&m_demonAnimation, 0.0f, 0.0f, 1.00f);
    }



    //bill
    /*{
        IllmeshLoader meshLoader("Meshes/Bill/bill.illmesh");

        m_bill.m_meshFrontendData = new MeshData<>(meshLoader.m_numInd / 3, meshLoader.m_numVert, meshLoader.m_features);
    
        meshLoader.buildMesh(*m_bill.m_meshFrontendData);
        m_bill.frontendBackendTransfer(m_engine->m_rendererBackend, false);
    }*/
    
    //load the skeleton
  //  {
  //      illGraphics::SkeletonLoadArgs loadArgs;
  //      loadArgs.m_path = "Meshes/Bill/bill.illskel";
  //      m_billSkeleton.load(loadArgs, NULL);
		////m_boneMatrixBuffer.reserve(m_billSkeleton.getNumBones());
  //  }

  //  //load the animation
  //  {
  //      illGraphics::SkeletonAnimationLoadArgs loadArgs;
  //      loadArgs.m_path = "Meshes/Bill/flutter.illanim";
  //      m_billAnimation.load(loadArgs, NULL);

		//m_billController.setSkeleton(&m_billSkeleton);
		//m_billController.queueTransition(&m_billAnimation, 0.0f, 0.0f);
  //  }

	m_boneMatrixBuffer.reserve(
		glm::max(
			glm::max(
				glm::max(
					m_marineSkeleton.getNumBones(), 
					m_hellKnightSkeleton.getNumBones()), 
					m_demonSkeleton.getNumBones()), 
					m_billSkeleton.getNumBones()));


    //load the test shader
    {
        std::vector<RefCountPtr<illGraphics::Shader> > shaders;

        illGraphics::Shader * shader = new illGraphics::Shader();
        shader->loadInternal(m_engine->m_graphicsBackend, "shaders/debugShader.vert", GL_VERTEX_SHADER, "");

        shaders.push_back(RefCountPtr<illGraphics::Shader>(shader));

        shader = new illGraphics::Shader();
        shader->loadInternal(m_engine->m_graphicsBackend, "shaders/debugShader.frag", GL_FRAGMENT_SHADER, "");

        shaders.push_back(RefCountPtr<illGraphics::Shader>(shader));

        m_debugShaderLoader = new illGraphics::ShaderProgramLoader(m_engine->m_graphicsBackend, NULL);
        m_debugShader.loadInternal(m_debugShaderLoader, shaders);
    }
    
    //initialize the input (this would normally initialize using console variables)
    m_forwardListener.m_value = &m_forward;
    m_backListener.m_value = &m_back;
    m_leftListener.m_value = &m_left;
    m_rightListener.m_value = &m_right;
    m_upListener.m_value = &m_up;
    m_downListener.m_value = &m_down;

    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_8), &m_forwardListener);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_5), &m_backListener);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_4), &m_leftListener);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_6), &m_rightListener);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_0), &m_upListener);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_KP_ENTER), &m_downListener);

    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_cameraController.m_inputContext);
    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_inputContext);

    m_cameraController.m_speed = 50.0f;
    m_cameraController.m_rollSpeed = 50.0f;
}

SkeletalAnimationDemoController::~SkeletalAnimationDemoController() {
    //delete[] m_animationTestSkelMats;

    m_engine->m_inputManager->getInputContextStack(0)->popInputContext();
    m_engine->m_inputManager->getInputContextStack(0)->popInputContext();

    m_debugShader.unload();
    delete m_debugShaderLoader;
}

void SkeletalAnimationDemoController::update(float seconds) {
    m_cameraController.update(seconds);

    m_marineController.update(seconds);

    m_hellKnightController0.update(seconds);
    m_hellKnightController1.update(seconds * 0.5f);
    m_hellKnightController2.update(seconds * 0.1f);

    m_demonController0.update(seconds);
    m_demonController1.update(seconds * 0.5f);
    m_demonController2.update(seconds * 0.2f);
    m_demonController3.update(seconds * 0.1f);

    m_billController.update(seconds);
    
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

void SkeletalAnimationDemoController::updateSound(float seconds) {

}
 
void SkeletalAnimationDemoController::render() {
    m_camera.setPerspectiveTransform(m_cameraController.m_transform, m_engine->m_window->getAspectRatio(), illGraphics::DEFAULT_FOV * m_cameraController.m_zoom, illGraphics::DEFAULT_NEAR, 2000.0f);
    
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
        
    GLint loc = getProgramUniformLocation(prog, "lightPos");
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
		
    m_marineController.computeAnimPose(&m_boneMatrixBuffer[0]);
    renderMesh(m_marine, m_marineController, &m_boneMatrixBuffer[0], m_camera, xform, prog);

    //draw marine helmet
    glActiveTexture(GL_TEXTURE0);
    texture = *((GLuint *) m_helmetDiffuse.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    texture = *((GLuint *) m_helmetNormal.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);

    renderMesh(m_marineHelmet, m_marineController, &m_boneMatrixBuffer[0], m_camera, xform, prog);

    //draw hellknight
    glActiveTexture(GL_TEXTURE0);
    texture = *((GLuint *) m_hellKnightDiffuse.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    texture = *((GLuint *) m_hellKnightNormal.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);

    
    xform = glm::translate(glm::vec3(0.0f, 100.0f, 0.0f));
	m_hellKnightController0.computeAnimPose(&m_boneMatrixBuffer[0]);
    renderMesh(m_hellKnight, m_hellKnightController0, &m_boneMatrixBuffer[0], m_camera, xform, prog);

    xform = glm::translate(glm::vec3(-20.0f, -200.0f, 0.0f)) * glm::scale(glm::vec3(2.0f));	
    m_hellKnightController1.computeAnimPose(&m_boneMatrixBuffer[0]);
    renderMesh(m_hellKnight, m_hellKnightController1, &m_boneMatrixBuffer[0], m_camera, xform, prog);

    xform = glm::translate(glm::vec3(-500.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(4.0f));
    m_hellKnightController2.computeAnimPose(&m_boneMatrixBuffer[0]);
    renderMesh(m_hellKnight, m_hellKnightController2, &m_boneMatrixBuffer[0], m_camera, xform, prog);

    //draw demon
    glActiveTexture(GL_TEXTURE0);
    texture = *((GLuint *) m_demonDiffuse.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    texture = *((GLuint *) m_demonNormal.getTextureData());
    glBindTexture(GL_TEXTURE_2D, texture);
    
    xform = glm::mat4();
	m_demonController0.computeAnimPose(&m_boneMatrixBuffer[0]);
    renderMesh(m_demon, m_demonController0, &m_boneMatrixBuffer[0], m_camera, xform, prog);
    renderMesh(m_demonFront, m_demonController0, &m_boneMatrixBuffer[0], m_camera, xform, prog);

    xform = glm::translate(glm::vec3(0.0f, -100.0f, 0.0f));	
    m_demonController1.computeAnimPose(&m_boneMatrixBuffer[0]);
    renderMesh(m_demon, m_demonController1, &m_boneMatrixBuffer[0], m_camera, xform, prog);
    renderMesh(m_demonFront, m_demonController1, &m_boneMatrixBuffer[0], m_camera, xform, prog);

    xform = glm::translate(glm::vec3(0.0f, 300.0f, 0.0f)) * glm::scale(glm::vec3(2.0f));	
    m_demonController2.computeAnimPose(&m_boneMatrixBuffer[0]);
    renderMesh(m_demon, m_demonController2, &m_boneMatrixBuffer[0], m_camera, xform, prog);
    renderMesh(m_demonFront, m_demonController2, &m_boneMatrixBuffer[0], m_camera, xform, prog);

    xform = glm::translate(glm::vec3(-400.0f, 600.0f, 0.0f)) * glm::scale(glm::vec3(5.0f));	
    m_demonController3.computeAnimPose(&m_boneMatrixBuffer[0]);
    renderMesh(m_demon, m_demonController3, &m_boneMatrixBuffer[0], m_camera, xform, prog);
    renderMesh(m_demonFront, m_demonController3, &m_boneMatrixBuffer[0], m_camera, xform, prog);

    //draw bill
	//m_billController.computeAnimPose(&m_boneMatrixBuffer[0]);
    //renderMesh(m_bill, m_billController, &m_boneMatrixBuffer[0], m_camera, glm::mat4(), prog);
    
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
        glm::translate(glm::vec3(500.0f, 0.0f, 0.0f)), glm::translate(glm::vec3(500.0f, 0.0f, 0.0f)));*/

    /*renderSkeleton(m_hellKnightSkeleton, m_hellKnightSkeleton.getRootBoneNode(), m_hellKnightController0, 
        glm::translate(glm::vec3(0.0f, 100.0f, 0.0f)));
    renderSkeleton(m_hellKnightSkeleton, m_hellKnightSkeleton.getRootBoneNode(), m_hellKnightController1, 
        glm::translate(glm::vec3(-20.0f, -200.0f, 0.0f)) * glm::scale(glm::vec3(2.0f)));
    renderSkeleton(m_hellKnightSkeleton, m_hellKnightSkeleton.getRootBoneNode(), m_hellKnightController2, 
        glm::translate(glm::vec3(-500.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(4.0f)));

    renderSkeleton(m_demonSkeleton, m_demonSkeleton.getRootBoneNode(), m_demonController0, 
        glm::mat4());
    renderSkeleton(m_demonSkeleton, m_demonSkeleton.getRootBoneNode(), m_demonController1, 
        glm::translate(glm::vec3(0.0f, -100.0f, 0.0f)));
    renderSkeleton(m_demonSkeleton, m_demonSkeleton.getRootBoneNode(), m_demonController2, 
        glm::translate(glm::vec3(0.0f, 300.0f, 0.0f)) * glm::scale(glm::vec3(2.0f)));
    renderSkeleton(m_demonSkeleton, m_demonSkeleton.getRootBoneNode(), m_demonController3, 
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
    
    //draw the light position
    glBegin(GL_LINES);
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
    glEnd();

    glDepthMask(GL_TRUE);

    ERROR_CHECK_OPENGL;
}

}
