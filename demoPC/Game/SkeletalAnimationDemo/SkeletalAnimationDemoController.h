#ifndef ILL_SKELETAL_ANIMATION_DEMO_CONTROLLER_H__
#define ILL_SKELETAL_ANIMATION_DEMO_CONTROLLER_H__

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

#include "illEngine/Input/serial/InputContext.h"
#include "illEngine/Input/serial/InputListenerState.h"
#include "illEngine/Input/serial/InputListenerRange.h"
#include "illEngine/Input/serial/InputBinding.h"
#include "illEngine/Pc/serial/sdlInputEnum.h"

namespace Demo {
struct Engine;

class SkeletalAnimationDemoController : public GameControllerBase {
public:
    SkeletalAnimationDemoController(Engine * engine);
    virtual ~SkeletalAnimationDemoController();

    void update(float seconds);
    void updateSound(float seconds);
    void render();

private:

    //TODO: the state and toggle listeners look like they'd be useful everywhere, move them to the Input project?
    struct State : public Input::InputListenerState::InputCallback {
        State()
            : Input::InputListenerState::InputCallback()
        {}

        virtual ~State() {}

        void onChange(bool value) {
            *m_state = value;
        }

        bool* m_state;
    };

    Engine * m_engine;

    CameraController m_cameraController;

    illGraphics::Camera m_camera;
    illGraphics::CameraTransform m_cameraTransform;
    
    Input::InputContext m_inputContext;

    //debug light
    glm::vec3 m_lightPos;

    bool m_forward;
    bool m_back;
    bool m_left;
    bool m_right;
    bool m_up;
    bool m_down;

    State m_forwardListener;
    State m_backListener;
    State m_leftListener;
    State m_rightListener;
    State m_upListener;
    State m_downListener;

    Input::InputListenerState m_forwardInput;
    Input::InputListenerState m_backInput;
    Input::InputListenerState m_leftInput;
    Input::InputListenerState m_rightInput;
    Input::InputListenerState m_upInput;
    Input::InputListenerState m_downInput;
    
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

    //bill
    illGraphics::Mesh m_bill;
    illGraphics::Skeleton m_billSkeleton;
    illGraphics::SkeletonAnimation m_billAnimation;

    illGraphics::ModelAnimationController m_billController;

    //the skinning shader

    illGraphics::ShaderProgram m_debugShader;
    illGraphics::ShaderProgramLoader * m_debugShaderLoader;
};
}

#endif