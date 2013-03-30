#ifndef ILL_RENDERER_DEMO_CONTROLLER_H__
#define ILL_RENDERER_DEMO_CONTROLLER_H__

#include <map>
#include <glm/glm.hpp>

#include "demoPC/GameControllerBase.h"
#include "demoPC/Game/CameraController.h"

#include "illEngine/Input/serial/InputContext.h"
#include "illEngine/Input/serial/Listeners/StateListener.h"
#include "illEngine/Input/serial/Listeners/StateReleaseToggleListener.h"

#include "illEngine/Graphics/serial/Camera/Camera.h"
#include "illEngine/Graphics/serial/Camera/CameraTransform.h"
#include "illEngine/Graphics/serial/Material/ShaderProgram.h"

namespace illRendererCommon {
class GraphicsScene;
class RendererBackend;
}

namespace Demo {
struct Engine;

class RendererDemoController : public GameControllerBase {
public:
    enum class Scene {
        THE_GRID,
        CHAOS,
        SHORT_CHAOS,
        ORGANIZED
    };

    RendererDemoController(Engine * engine, Scene scene);
    virtual ~RendererDemoController();

    void update(float seconds);
    void updateSound(float seconds);
    void render();

private:
    struct ChangeDebugMode : public illInput::StateListener {
        ChangeDebugMode()
            : illInput::StateListener()
        {}

        void onRelease();

        RendererDemoController * m_controller;
        int m_mode;
    };

    struct ToggleCamera : public illInput::StateListener {
        ToggleCamera()
            : illInput::StateListener()
        {}

        void onRelease();

        RendererDemoController * m_controller;
    };

    Engine * m_engine;

    size_t m_viewport;

    illGraphics::Camera m_camera;
    illGraphics::Camera m_occlusionCamera;

	illRendererCommon::GraphicsScene * m_graphicsScene;
    illRendererCommon::RendererBackend * m_rendererBackend;

    CameraController m_cameraController;
    CameraController m_occlusionCameraController;

    ChangeDebugMode m_noneDebugMode;

    ChangeDebugMode m_depthDebugMode;
    ChangeDebugMode m_normalDebugMode;
    ChangeDebugMode m_diffuseDebugMode;
    ChangeDebugMode m_specularDebugMode;

    ChangeDebugMode m_diffuseAccumulationDebugMode;
    ChangeDebugMode m_specularAccumulationDebugMode;

    illInput::StateReleaseToggleListener m_occlusionDebugToggle;
    ToggleCamera m_toggleCamera;
    bool m_whichCamera;

    illInput::StateReleaseToggleListener m_topDownToggle;
    illInput::StateReleaseToggleListener m_drawFrustumToggle;
    illInput::StateReleaseToggleListener m_drawGridToggle;
    illInput::StateReleaseToggleListener m_performOcclusionToggle;

    illInput::StateReleaseToggleListener m_drawLightsToggle;
    illInput::StateReleaseToggleListener m_drawBoundsToggle;

    illInput::InputContext m_inputContext;

    bool m_performCull;
    bool m_occlusionDebug;

    bool m_topDown;
    bool m_drawFrustum;
    bool m_drawGrid;
};
}

#endif
