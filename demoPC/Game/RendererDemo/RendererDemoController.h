#ifndef ILL_RENDERER_DEMO_CONTROLLER_H__
#define ILL_RENDERER_DEMO_CONTROLLER_H__

#include <fstream>
#include <chrono>
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

#include "illEngine/Util/Geometry/Transform.h"

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
    enum class Mode {
        NONE,
        RECORDING,
        PLAYING
    };

    struct Recorder {
        std::chrono::system_clock::time_point m_startTime;
        std::ofstream m_recordFile;
    };

    struct Player {
        struct Keyframe {
            glm::mediump_float m_time;
            Transform<> m_transform;            
        };

        std::vector<Keyframe> m_transformList;

        size_t m_keyframe;
        glm::mediump_float m_delta;
        glm::mediump_float m_t;

        inline void computeDelta() {
            if(m_keyframe < m_transformList.size() - 1) {
                m_delta = 1.0f / (m_transformList[m_keyframe + 1].m_time - m_transformList[m_keyframe].m_time);
            }
            else {
                m_delta = 0.0f;
            }
        }
    };

    void beginRecord(const char * fileName);
    void recordTransform(const Transform<>& xform);
    void endRecord();

    void beginPlayback(const char * fileName);
    void endPlayback();

    Recorder m_recorder;
    Player m_player;
    Mode m_mode;

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

    illConsole::ConsoleCommand * m_cm_ren_freezeFrustum;
    illConsole::ConsoleCommand * m_cm_ren_unfreezeFrustum;
    illConsole::ConsoleCommand * m_cm_ren_advanceFrustum;
    illConsole::ConsoleCommand * m_cm_ren_fastAdvanceFrustum;

    illConsole::ConsoleCommand * m_cm_demo_beginRecord;
    illConsole::ConsoleCommand * m_cm_demo_recordPos;
    illConsole::ConsoleCommand * m_cm_demo_endRecord;
    illConsole::ConsoleCommand * m_cm_demo_play;
    illConsole::ConsoleCommand * m_cm_demo_stop;

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
