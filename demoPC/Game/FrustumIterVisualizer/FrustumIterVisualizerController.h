#ifndef ILL_FRUSTUM_ITER_VISUALIZER_CONTROLLER_H__
#define ILL_FRUSTUM_ITER_VISUALIZER_CONTROLLER_H__

#include <map>
#include <glm/glm.hpp>

#include "demoPC/GameControllerBase.h"
#include "demoPC/Game/CameraController.h"

#include "illEngine/Graphics/serial/Camera/Camera.h"
#include "illEngine/Graphics/serial/Camera/CameraTransform.h"
#include "illEngine/Graphics/serial/Material/ShaderProgram.h"
#include "illEngine/Graphics/serial/BitmapFont.h"

#include "illEngine/Util/Geometry/ConvexMeshIterator.h"
#include "illEngine/Util/Geometry/MeshEdgeList.h"

#include "illEngine/Input/serial/InputContext.h"
#include "illEngine/Input/serial/InputListenerState.h"
#include "illEngine/Input/serial/InputListenerRange.h"
#include "illEngine/Input/serial/InputBinding.h"
#include "illEngine/Pc/serial/sdlInputEnum.h"

namespace Demo {
struct Engine;

class FrustumIterVisualizerController : public GameControllerBase {
public:
    FrustumIterVisualizerController(Engine * engine);
    virtual ~FrustumIterVisualizerController();

    void update(float seconds);
    void updateSound(float seconds);
    void render();

private:
    void setupTestFrustumIterator();

    struct AdvanceFrustumIterator : public Input::InputListenerState::InputCallback {
        AdvanceFrustumIterator()
            : Input::InputListenerState::InputCallback()
        {}

        void onRelease() {
            /*switch(++m_controller->m_planeIndex) {
            case 1:
                m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(1.0f, 0.0f, 0.0f), 50.0f));
                break;

            case 2:
                m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 1.0f, 0.0f), 50.0f));
                break;

            case 3:
                m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, 1.0f), 50.0f));
                break;

            case 4:
                m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(-1.0f, 0.0f, 0.0f), 50.0f));
                break;

            case 5:
                m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, -1.0f, 0.0f), 50.0f));
                break;

            case 6:
                m_controller->m_testMeshEdgeList.convexClip(Plane<>(glm::vec3(0.0f, 0.0f, -1.0f), 50.0f));
                break;

            case 7:
                m_controller->m_planeIndex = 0;
                m_controller->m_testMeshEdgeList = m_controller->m_testUnclippedMeshEdgeList;

                break;
            }*/
            
            if(m_controller->m_testFrustumIter && !m_controller->m_testFrustumIter->atEnd()) {
                m_controller->m_testFrustumIter->forward();
            }
        }

        FrustumIterVisualizerController * m_controller;
    };

    struct AdvanceFrustumIteratorHold : public Input::InputListenerState::InputCallback {
        AdvanceFrustumIteratorHold()
            : Input::InputListenerState::InputCallback()
        {}

        void onChange(bool value) {
            m_controller->m_advanceHold = value;
        }

        FrustumIterVisualizerController * m_controller;
    };

    struct MapToWorld : public Input::InputListenerState::InputCallback {
        MapToWorld()
            : Input::InputListenerState::InputCallback()
        {}

        void onRelease() {
            m_controller->m_mapToWorld = !m_controller->m_mapToWorld;
        }

        FrustumIterVisualizerController * m_controller;
    };

    struct ResetFrustumIterator : public Input::InputListenerState::InputCallback {
        ResetFrustumIterator()
            : Input::InputListenerState::InputCallback()
        {}

        void onRelease();

        FrustumIterVisualizerController * m_controller;
    };

    struct RestartFrustumIterator : public Input::InputListenerState::InputCallback {
        RestartFrustumIterator()
            : Input::InputListenerState::InputCallback()
        {}

        void onRelease();

        FrustumIterVisualizerController * m_controller;
    };

    struct CompleteFrustumIterator : public Input::InputListenerState::InputCallback {
        CompleteFrustumIterator()
            : Input::InputListenerState::InputCallback()
        {}

        void onRelease();

        FrustumIterVisualizerController * m_controller;
    };
    
    Engine * m_engine;

    CameraController m_cameraController;

    illGraphics::Camera m_camera;
    illGraphics::CameraTransform m_cameraTransform;
    
    //frustum iterator debugging
    ConvexMeshIterator<> * m_testFrustumIter;
    illGraphics::Camera m_testFrustumCamera;    //for easy resetting
    MeshEdgeList<> m_iteratedMeshEdgeList;
    MeshEdgeList<> m_testMeshEdgeList;
    MeshEdgeList<> m_testUnclippedMeshEdgeList;

    //unsigned int m_planeIndex;

    AdvanceFrustumIterator m_advanceFrustumIteratorCallback;
    AdvanceFrustumIteratorHold m_advanceFrustumIteratorHoldCallback;
    ResetFrustumIterator m_resetFrustumIteratorCallback;
    RestartFrustumIterator m_restartFrustumIteratorCallback;
    CompleteFrustumIterator m_completeFrustumIteratorCallback;
    MapToWorld m_mapToWorldCallback;
    
    Input::InputListenerState m_advanceFrustumIterator;
    Input::InputListenerState m_advanceFrustumIteratorHold;
    Input::InputListenerState m_resetFrustumIterator;
    Input::InputListenerState m_restartFrustumIterator;
    Input::InputListenerState m_completeFrustumIterator;
    Input::InputListenerState m_mapToWorldListener;

    bool m_advanceHold;
    bool m_mapToWorld;
    float m_advanceHoldTimer;

    Input::InputContext m_frustumInputContext;
    
    //debug font
    illGraphics::BitmapFont m_debugFont;

    illGraphics::ShaderProgram m_fontShader;
    illGraphics::ShaderProgramLoader * m_debugShaderLoader;
};
}

#endif
