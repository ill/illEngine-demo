#ifndef ILL_RENDERER_DEMO_CONTROLLER_H__
#define ILL_RENDERER_DEMO_CONTROLLER_H__

#include <map>
#include <glm/glm.hpp>

#include "demoPC/GameControllerBase.h"
#include "demoPC/Game/CameraController.h"

#include "illEngine/Graphics/serial/Camera/Camera.h"
#include "illEngine/Graphics/serial/Camera/CameraTransform.h"
#include "illEngine/Graphics/serial/Material/ShaderProgram.h"

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
    
    Engine * m_engine;

    CameraController m_cameraController;
};
}

#endif
