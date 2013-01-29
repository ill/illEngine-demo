#ifndef ILL_FRUSTUM_CULL_VISUALIZER_CONTROLLER_H__
#define ILL_FRUSTUM_CULL_VISUALIZER_CONTROLLER_H__

#include <map>
#include <glm/glm.hpp>

#include "demoPC/GameControllerBase.h"
#include "demoPC/Game/CameraController.h"

#include "illEngine/Graphics/serial/Camera/Camera.h"
#include "illEngine/Graphics/serial/Camera/CameraTransform.h"
#include "illEngine/Graphics/serial/Material/ShaderProgram.h"

#include "illEngine/Util/Geometry/ConvexMeshIterator.h"

#include "illEngine/Input/serial/InputContext.h"
#include "illEngine/Input/serial/InputListenerState.h"
#include "illEngine/Input/serial/InputListenerRange.h"
#include "illEngine/Input/serial/InputBinding.h"
#include "illEngine/Pc/serial/sdlInputEnum.h"

namespace Demo {
struct Engine;

class FrustumCullVisualizerController : public GameControllerBase {
public:
    FrustumCullVisualizerController(Engine * engine);
    virtual ~FrustumCullVisualizerController();

    void update(float seconds);
    void updateSound(float seconds);
    void render();

private:
    void setupFrustumIterator();
    
    struct HoldFrustumIterator : public Input::InputListenerState::InputCallback {
        HoldFrustumIterator()
            : Input::InputListenerState::InputCallback()
        {}

        void onRelease() {
            if(!m_controller->m_hold) {
                m_controller->m_hold = true;
                m_controller->m_testFrustumCamera = m_controller->m_camera; //preserve the camera
            }
            else {
                m_controller->m_hold = false;
            }
        }

        FrustumCullVisualizerController * m_controller;
    };
    
    Engine * m_engine;

    CameraController m_cameraController;

    illGraphics::Camera m_camera;
    illGraphics::CameraTransform m_cameraTransform;
    
    //frustum iterator debugging
    illGraphics::Camera m_testFrustumCamera;    //for holding

    //unsigned int m_planeIndex;

    HoldFrustumIterator m_holdFrustumIteratorCallback;
    
    Input::InputListenerState m_holdFrustumIterator;

    bool m_hold;

    Input::InputContext m_frustumInputContext;    
};
}

#endif
