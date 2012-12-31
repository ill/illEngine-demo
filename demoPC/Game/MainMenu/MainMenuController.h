#ifndef __MAIN_MENU_CONTROLLER_H__
#define __MAIN_MENU_CONTROLLER_H__

#include <map>
#include <glm/glm.hpp>

#include "demoPC/GameControllerBase.h"
#include "demoPC/Game/CameraController.h"

#include "illEngine/Graphics/serial/Camera/Camera.h"
#include "illEngine/Graphics/serial/Camera/CameraTransform.h"
#include "illEngine/Graphics/serial/Model/Mesh.h"
#include "illEngine/Graphics/serial/Material/Texture.h"
#include "illEngine/Graphics/serial/Model/Skeleton.h"
#include "illEngine/Graphics/serial/Model/SkeletonAnimation.h"
#include "illEngine/Graphics/serial/Model/ModelAnimationController.h"
#include "illEngine/Graphics/serial/Material/ShaderProgram.h"

namespace Demo {
struct Engine;

class MainMenuController : public GameControllerBase {
public:
    MainMenuController(Engine * engine);
    virtual ~MainMenuController();

    void update(float seconds);
    void updateSound(float seconds);
    void render();

private:
    Engine * m_engine;

    CameraController m_cameraController;

    illGraphics::Camera m_camera;
    illGraphics::CameraTransform m_cameraTransform;
    
    //marine

    illGraphics::Mesh m_marine;
    illGraphics::Mesh m_marineHelmet;
    illGraphics::Texture m_marineDiffuse;
    illGraphics::Texture m_helmetDiffuse;
    illGraphics::Texture m_marineNormal;
    illGraphics::Texture m_helmetNormal;
    illGraphics::Skeleton m_marineSkeleton;
    illGraphics::SkeletonAnimation m_marineAnimation;
    illGraphics::ModelAnimationController m_marineController;

    //hell knight

    illGraphics::Mesh m_hellKnight;
    illGraphics::Texture m_hellKnightDiffuse;
    illGraphics::Texture m_hellKnightNormal;
    illGraphics::Skeleton m_hellKnightSkeleton;
    illGraphics::SkeletonAnimation m_hellKnightAnimation;

    illGraphics::ModelAnimationController m_hellKnightController0;
    illGraphics::ModelAnimationController m_hellKnightController1;
    illGraphics::ModelAnimationController m_hellKnightController2;

    //demon

    illGraphics::Mesh m_demon;
    illGraphics::Texture m_demonDiffuse;
    illGraphics::Texture m_demonNormal;
    illGraphics::Skeleton m_demonSkeleton;
    illGraphics::SkeletonAnimation m_demonAnimation;

    illGraphics::ModelAnimationController m_demonController0;
    illGraphics::ModelAnimationController m_demonController1;
    illGraphics::ModelAnimationController m_demonController2;
    illGraphics::ModelAnimationController m_demonController3;

    //demon front

    illGraphics::Mesh m_demonFront;

    //the skinning shader

    illGraphics::ShaderProgram m_debugShader;
    illGraphics::ShaderProgramLoader * m_debugShaderLoader;

    /*std::map<unsigned int, glm::mat4> m_animationTest;    //temporarily testing animations manually without the animation controller
    glm::mat4 * m_animationTestSkelMats;*/
};
}

#endif
