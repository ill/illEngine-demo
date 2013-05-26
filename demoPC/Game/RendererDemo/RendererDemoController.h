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

#include "illEngine/Console/serial/VariableManager.h"
#include "illEngine/Console/serial/CommandManager.h"

#include "../../Util/Graph.h"

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
    Engine * m_engine;

    size_t m_viewport;

    illGraphics::Camera m_camera;
    illGraphics::Camera m_occlusionCamera;

	illRendererCommon::GraphicsScene * m_graphicsScene;
    illRendererCommon::RendererBackend * m_rendererBackend;

    CameraController m_cameraController;
    CameraController m_occlusionCameraController;    
    bool m_whichCamera;
    
    illInput::InputContext m_inputContext;
    
    illConsole::ConsoleVariable * m_cv_ren_deferredDebugMode;
    illConsole::ConsoleVariable * m_cv_ren_showPerf;
    illConsole::ConsoleVariable * m_cv_ren_stencilLighting;    
    illConsole::ConsoleVariable * m_cv_ren_occlusionCull;
    illConsole::ConsoleVariable * m_cv_ren_showLights;
    illConsole::ConsoleVariable * m_cv_ren_showBounds;

    illConsole::ConsoleVariable * m_cv_ren_showGrid;
    illConsole::ConsoleVariable * m_cv_ren_showCullDebug;
    illConsole::ConsoleVariable * m_cv_ren_controlCullCamera;
    illConsole::ConsoleVariable * m_cv_ren_showFrustum;

    bool m_showPerformance;
    Graph m_numTraversedCellsGraph;    
    Graph m_numEmptyCellsGraph;
    Graph m_numNonEmptyCellsGraph;
    Graph m_numCellQueriesGraph;
    Graph m_numCellsUnqueriedGraph;
    Graph m_numRenderedCellsGraph;    
    Graph m_numCulledCellsGraph;
    Graph m_cellRequeryDurationGraph;
    Graph m_numProcessedNodesGraph;
    Graph m_numOverflowedQueriesGraph;

    bool m_performCull;
    bool m_occlusionDebug;
    bool m_perObjectOcclusion;

    bool m_topDown;
    bool m_drawFrustum;
    bool m_drawGrid;
};
}

#endif
