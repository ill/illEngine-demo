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

//void debugDrawBone(const glm::mat4& xForm, const glm::mat4& prevXform, bool drawLine) {
//    glm::vec4 currPoint(0.0f, 0.0f, 0.0f, 1.0f);
//    currPoint = xForm * currPoint;
//
//    glm::vec4 parentPos(0.0f, 0.0f, 0.0f, 1.0f);
//    parentPos = prevXform * parentPos;
//
//    //draw line from this bone to the last bone
//    glLineWidth(3.0f);
//
//    if(drawLine) {
//        glColor4f(1.0f, 1.0f, 0.0f, 0.15f);
//
//        glBegin(GL_LINES);
//            glVertex3fv(glm::value_ptr(parentPos));
//            glVertex3fv(glm::value_ptr(currPoint));
//        glEnd();
//    }
//
//    glPointSize(5.0f);
//    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
//
//    //draw the bone point
//    glBegin(GL_POINTS);
//    glVertex3fv(glm::value_ptr(currPoint));
//    glEnd();
//
//    //draw the bone orientation
//    glLineWidth(3.0f);
//
//    glBegin(GL_LINES);
//        //x
//        glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
//        glVertex3fv(glm::value_ptr(currPoint));
//        
//        glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
//        glVertex3fv(glm::value_ptr(glm::vec3(currPoint) + glm::mat3(xForm) * glm::vec3(1.0f, 0.0f, 0.0f) * 5.0f));
//
//        //y
//        glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
//        glVertex3fv(glm::value_ptr(currPoint));
//        
//        glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
//        glVertex3fv(glm::value_ptr(glm::vec3(currPoint) + glm::mat3(xForm) * glm::vec3(0.0f, 1.0f, 0.0f) * 5.0f));
//
//        //z
//        glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
//        glVertex3fv(glm::value_ptr(currPoint));
//        
//        glColor4f(0.0f, 0.0f, 1.0f, 0.0f);
//        glVertex3fv(glm::value_ptr(glm::vec3(currPoint) + glm::mat3(xForm) * glm::vec3(0.0f, 0.0f, 1.0f) * 5.0f));
//    glEnd();
//    
//    glLineWidth(1.0f);
//}
//
//void debugDrawSkeleton(const Graphics::Skeleton * skeleton, const Graphics::Skeleton::BoneHeirarchy * currNode, glm::mat4 currXform, glm::mat4 currBindXform, glm::mat4 currAnimXform, std::map<unsigned int, glm::mat4>& animTransforms, glm::mat4 * animationTestSkelMats) {
//    glm::mat4 prevXform = currXform;
//    glm::mat4 prevBindXform = currBindXform;
//    
//    //currXform = currXform * skeleton->getBone(currNode->m_boneIndex)->m_transform * animTransforms[currNode->m_boneIndex];
//
//    std::map<unsigned int, glm::mat4>::iterator iter = animTransforms.find(currNode->m_boneIndex);
//
//    /*glm::mat4 relXform; //TODO: This should go into the animation file instead of absolute transforms from the bind pose, there's currently a limitation though so it's computed here
//
//    if(iter != animTransforms.end()) {
//        relXform = glm::inverse(skeleton->getBone(currNode->m_boneIndex)->m_transform) * animTransforms[currNode->m_boneIndex];
//    }
//
//    currXform = currXform * skeleton->getBone(currNode->m_boneIndex)->m_transform * relXform;
//    currBindXform = currBindXform * skeleton->getBone(currNode->m_boneIndex)->m_transform;
//    currAnimXform = currAnimXform * relXform;*/
//    
//    if(iter != animTransforms.end()) {
//        currXform = currXform * animTransforms[currNode->m_boneIndex];
//    }
//    else {
//        currXform = currXform * skeleton->getBone(currNode->m_boneIndex)->m_transform;
//    }
//
//    currBindXform = currBindXform * skeleton->getBone(currNode->m_boneIndex)->m_transform;
//
//    animationTestSkelMats[currNode->m_boneIndex] = 
//        //glm::inverse(currBindXform) * currXform;
//        currXform * glm::inverse(currBindXform) * glm::rotate(-90.0f, glm::vec3(1.0f, 0.0, 0.0f));      //TODO: for now hardcoded to rotate this -90 degrees around x since all md5s seem to be flipped
//                                                                                                        //figure out how to export models in the right orientation
//                                                                                                        //THIS!  is why there's the horrible hack code below and why it took me days to get this working, 1 tiny mistake and it all explodes
//        //currAnimXform;
//        //glm::mat4();
//        
//        /*skeleton->getBone(currNode->m_boneIndex)->m_boneOffsetHack
//            ? currXform * *skeleton->getBone(currNode->m_boneIndex)->m_boneOffsetHack
//            : glm::mat4();*/
//
//    /*if(skeleton->getBone(currNode->m_boneIndex)->m_boneOffsetHack) {
//        glm::mat4 debMat = glm::inverse(currBindXform);
//    
//        LOG_DEBUG("\nBind inverse:\n\t%3.4f %3.4f %3.4f %3.4f\n\t%3.4f %3.4f %3.4f %3.4f\n\t%3.4f %3.4f %3.4f %3.4f\n\t%3.4f %3.4f %3.4f %3.4f\n",
//            debMat[0][0], debMat[0][1], debMat[0][2], debMat[0][3],
//            debMat[1][0], debMat[1][1], debMat[1][2], debMat[1][3],
//            debMat[2][0], debMat[2][1], debMat[2][2], debMat[2][3],
//            debMat[3][0], debMat[3][1], debMat[3][2], debMat[3][3]);
//
//        debMat = *skeleton->getBone(currNode->m_boneIndex)->m_boneOffsetHack;
//
//        LOG_DEBUG("\nBone Offset:\n\t%3.4f %3.4f %3.4f %3.4f\n\t%3.4f %3.4f %3.4f %3.4f\n\t%3.4f %3.4f %3.4f %3.4f\n\t%3.4f %3.4f %3.4f %3.4f\n",
//            debMat[0][0], debMat[0][1], debMat[0][2], debMat[0][3],
//            debMat[1][0], debMat[1][1], debMat[1][2], debMat[1][3],
//            debMat[2][0], debMat[2][1], debMat[2][2], debMat[2][3],
//            debMat[3][0], debMat[3][1], debMat[3][2], debMat[3][3]);
//
//        debMat = currBindXform * *skeleton->getBone(currNode->m_boneIndex)->m_boneOffsetHack;
//
//        LOG_DEBUG("\Difference Offset:\n\t%3.4f %3.4f %3.4f %3.4f\n\t%3.4f %3.4f %3.4f %3.4f\n\t%3.4f %3.4f %3.4f %3.4f\n\t%3.4f %3.4f %3.4f %3.4f\n",
//            debMat[0][0], debMat[0][1], debMat[0][2], debMat[0][3],
//            debMat[1][0], debMat[1][1], debMat[1][2], debMat[1][3],
//            debMat[2][0], debMat[2][1], debMat[2][2], debMat[2][3],
//            debMat[3][0], debMat[3][1], debMat[3][2], debMat[3][3]);
//
//        LOG_DEBUG("\n\n\n");
//    }*/
//
//    debugDrawBone(currXform, prevXform, currNode->m_parent != NULL);
//    debugDrawBone(currBindXform, prevBindXform, currNode->m_parent != NULL);
//
//    for(std::vector<Graphics::Skeleton::BoneHeirarchy *>::const_iterator iter = currNode->m_children.begin(); iter != currNode->m_children.end(); iter++) {
//        debugDrawSkeleton(skeleton, *iter, currXform, currBindXform, currAnimXform, animTransforms, animationTestSkelMats);
//    }
//}

void renderMesh(Graphics::Mesh& mesh, Graphics::ModelAnimationController& controller, const Graphics::Camera& camera, const glm::mat4& xform, GLuint prog) {
    
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

namespace Demo {

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
        Graphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/marine.tga";
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

        m_marineDiffuse.load(loadArgs, m_engine->m_rendererBackend);
    }

    //marine normal map
    {
        Graphics::TextureLoadArgs loadArgs;
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
        Graphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/helmet_local.tga";
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

        m_helmetNormal.load(loadArgs, m_engine->m_rendererBackend);
    }
    
    //diffuse helmet texture
    {
        Graphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/helmet.tga";
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

        m_helmetDiffuse.load(loadArgs, m_engine->m_rendererBackend);
    }




    //load the skeleton
    {
        Graphics::SkeletonLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Marine/marine.illskel";
        m_marineSkeleton.load(loadArgs, NULL);

        //m_animationTestSkelMats = new glm::mat4[m_marineSkeleton.getNumBones()];

        m_marineController.alloc(m_marineSkeleton.getNumBones());
        m_marineController.m_skeleton = &m_marineSkeleton;
    }

    //load the animation
    {
        Graphics::SkeletonAnimationLoadArgs loadArgs;
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
        Graphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/HellKnight/hellknight.tga";
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

        m_hellKnightDiffuse.load(loadArgs, m_engine->m_rendererBackend);
    }

    //marine normal map
    {
        Graphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/HellKnight/hellknight_local.tga";
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

        m_hellKnightNormal.load(loadArgs, m_engine->m_rendererBackend);
    }

    //load the skeleton
    {
        Graphics::SkeletonLoadArgs loadArgs;
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
        Graphics::SkeletonAnimationLoadArgs loadArgs;
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
        Graphics::TextureLoadArgs loadArgs;
        loadArgs.m_path = "Meshes/Demon/pinky_d.tga";
        loadArgs.m_wrapS = GL_CLAMP;
        loadArgs.m_wrapT = GL_CLAMP;

        m_demonDiffuse.load(loadArgs, m_engine->m_rendererBackend);
    }

    //marine normal map
    {
        Graphics::TextureLoadArgs loadArgs;
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
        Graphics::SkeletonLoadArgs loadArgs;
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
        Graphics::SkeletonAnimationLoadArgs loadArgs;
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
        std::vector<RefCountPtr<Graphics::Shader> > shaders;

        Graphics::Shader * shader = new Graphics::Shader();
        shader->loadInternal(m_engine->m_rendererBackend, "shaders/debugShader.vert", GL_VERTEX_SHADER, "");

        shaders.push_back(RefCountPtr<Graphics::Shader>(shader));

        shader = new Graphics::Shader();
        shader->loadInternal(m_engine->m_rendererBackend, "shaders/debugShader.frag", GL_FRAGMENT_SHADER, "");

        shaders.push_back(RefCountPtr<Graphics::Shader>(shader));

        m_debugShaderLoader = new Graphics::ShaderProgramLoader(m_engine->m_rendererBackend, NULL);
        m_debugShader.loadInternal(m_debugShaderLoader, shaders);
    }

    //initialize the input (this would normally initialize using console variables)
    m_engine->m_inputManager->addPlayer(0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_KEYBOARD, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE_BUTTON, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE_WHEEL, 0);

    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_cameraController.m_inputContext);

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

    /*static float animTime = 0.0f;

    animTime += seconds * 0.01f;
    
    Graphics::SkeletonAnimation::InterpInfo interpInfo = m_animation.getFrames(animTime);

    //get transforms for all the bones
    for(std::map<std::string, unsigned int>::const_iterator iter = m_skeleton.getBoneNameMap().begin(); iter != m_skeleton.getBoneNameMap().end(); iter++) {
        std::map<std::string, Graphics::SkeletonAnimation::Transform*>::const_iterator animIter = m_animation.getAnimations().find(iter->first);
        
        if(animIter == m_animation.getAnimations().end()) {
            continue;
        }

        Graphics::SkeletonAnimation::Transform* transforms = animIter->second;

        Graphics::SkeletonAnimation::Transform currentTransform;

        currentTransform.m_position = glm::mix(transforms[interpInfo.m_frame1].m_position, transforms[interpInfo.m_frame2].m_position, interpInfo.m_delta);
        currentTransform.m_rotation = glm::fastMix(transforms[interpInfo.m_frame1].m_rotation, transforms[interpInfo.m_frame2].m_rotation, interpInfo.m_delta);
        currentTransform.m_scale = glm::mix(transforms[interpInfo.m_frame1].m_scale, transforms[interpInfo.m_frame2].m_scale, interpInfo.m_delta);

        //compute the matrix
        //Oh god!  It took me 2 days of trying to debug math problems to fix this.
        glm::mat4 transform = glm::translate(currentTransform.m_position) * glm::mat4_cast(currentTransform.m_rotation) * glm::scale(currentTransform.m_scale);
                
        //place the transform into the thing
        m_animationTest[iter->second] = transform;
    }*/
}

void MainMenuController::updateSound(float seconds) {

}
 
void MainMenuController::render() {
    m_cameraTransform.m_transform = m_cameraController.m_transform;
    m_camera.setTransform(m_cameraTransform, m_engine->m_window->getAspectRatio(), Graphics::DEFAULT_FOV * m_cameraController.m_zoom, Graphics::DEFAULT_NEAR, 2000.0f);
    
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

    //debug draw the skeleton
    //debugDrawSkeleton(&m_skeleton, m_skeleton.getRootBoneNode(), glm::mat4(), glm::mat4(), glm::mat4(), m_animationTest, m_animationTestSkelMats);

    ////debug draw the mesh stuff
    //glPointSize(5.0f);

    ////make all the computed poses be the relative xform

    //for(unsigned int vertex = 0; vertex < m_mesh.m_meshFrontendData->getNumVert(); vertex++) {
    //    glm::vec3 thisPos = m_mesh.m_meshFrontendData->getPosition(vertex);
    //    glm::vec4 pos(0.0f);
    //    
    //    for(unsigned int weight = 0; weight < 4; weight++) {
    //        //bone xform * this position * bone weight
    //        glm::mat4 xform = m_animationTestSkelMats[(int) m_mesh.m_meshFrontendData->getBlendData(vertex).m_blendIndex[weight]];
    //        glm::vec4 weightsArray = m_mesh.m_meshFrontendData->getBlendData(vertex).m_blendWeight;
    //        glm::mediump_float wgt = m_mesh.m_meshFrontendData->getBlendData(vertex).m_blendWeight[weight];

    //        pos += m_animationTestSkelMats[(int) m_mesh.m_meshFrontendData->getBlendData(vertex).m_blendIndex[weight]]
    //            * glm::vec4(thisPos, 1.0f) 
    //            * m_mesh.m_meshFrontendData->getBlendData(vertex).m_blendWeight[weight];
    //    }

    //    //transformed point
    //    glBegin(GL_POINTS);

    //    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    //    glVertex3fv(glm::value_ptr(pos));

    //    glEnd();

    //    glBegin(GL_LINES);

    //    glm::vec3 tail;

    //    //normal
    //    glm::vec3 thisNormal = m_mesh.m_meshFrontendData->getNormal(vertex);
    //    glm::vec3 skinned(0.0f);

    //    for(unsigned int weight = 0; weight < 4; weight++) {
    //        //bone xform * this position * bone weight
    //        skinned += glm::mat3(m_animationTestSkelMats[(int) m_mesh.m_meshFrontendData->getBlendData(vertex).m_blendIndex[weight]])
    //            * thisNormal
    //            * m_mesh.m_meshFrontendData->getBlendData(vertex).m_blendWeight[weight];
    //    }

    //    tail = glm::vec3(pos) + glm::vec3(skinned);

    //    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    //    glVertex3fv(glm::value_ptr(pos));
    //    glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
    //    glVertex3fv(glm::value_ptr(tail));

    //    //tangent
    //    glm::vec3 thisTangent = m_mesh.m_meshFrontendData->getTangent(vertex).m_tangent;
    //    skinned = glm::vec3(0.0f);

    //    for(unsigned int weight = 0; weight < 4; weight++) {
    //        //bone xform * this position * bone weight
    //        skinned += glm::mat3(m_animationTestSkelMats[(int) m_mesh.m_meshFrontendData->getBlendData(vertex).m_blendIndex[weight]])
    //            * thisTangent
    //            * m_mesh.m_meshFrontendData->getBlendData(vertex).m_blendWeight[weight];
    //    }

    //    tail = glm::vec3(pos) + glm::vec3(skinned);

    //    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    //    glVertex3fv(glm::value_ptr(pos));
    //    glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
    //    glVertex3fv(glm::value_ptr(tail));

    //    //bitangent
    //    glm::vec3 thisBitangent = m_mesh.m_meshFrontendData->getTangent(vertex).m_bitangent;
    //    skinned = glm::vec3(0.0f);

    //    for(unsigned int weight = 0; weight < 4; weight++) {
    //        //bone xform * this position * bone weight
    //        skinned += glm::mat3(m_animationTestSkelMats[(int) m_mesh.m_meshFrontendData->getBlendData(vertex).m_blendIndex[weight]])
    //            * thisBitangent
    //            * m_mesh.m_meshFrontendData->getBlendData(vertex).m_blendWeight[weight];
    //    }

    //    tail = glm::vec3(pos) + glm::vec3(skinned);

    //    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    //    glVertex3fv(glm::value_ptr(pos));
    //    glColor4f(0.0f, 0.0f, 1.0f, 0.0f);
    //    glVertex3fv(glm::value_ptr(tail));

    //    glEnd();
    //}
    
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
