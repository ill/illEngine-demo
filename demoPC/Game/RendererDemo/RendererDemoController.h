#ifndef ILL_RENDERER_DEMO_CONTROLLER_H__
#define ILL_RENDERER_DEMO_CONTROLLER_H__

#include <map>
#include <glm/glm.hpp>

#include "demoPC/GameControllerBase.h"
#include "demoPC/Game/CameraController.h"

#include "illEngine/Input/serial/InputContext.h"
#include "illEngine/Input/serial/Listeners/StateListener.h"

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
    RendererDemoController(Engine * engine);
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

    Engine * m_engine;

    illGraphics::Camera m_camera;

	illRendererCommon::GraphicsScene * m_graphicsScene;
    illRendererCommon::RendererBackend * m_rendererBackend;

    CameraController m_cameraController;

    ChangeDebugMode m_noneDebugMode;
    ChangeDebugMode m_lightPosDebugMode;
    ChangeDebugMode m_wireDebugMode;
    ChangeDebugMode m_solidDebugMode;

    ChangeDebugMode m_depthDebugMode;
    ChangeDebugMode m_normalDebugMode;
    ChangeDebugMode m_diffuseDebugMode;
    ChangeDebugMode m_specularDebugMode;

    ChangeDebugMode m_diffuseAccumulationDebugMode;
    ChangeDebugMode m_specularAccumulationDebugMode;

    illInput::InputContext m_inputContext;
};
}

#endif
