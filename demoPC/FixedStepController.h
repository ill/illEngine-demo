#ifndef __FIXED_STEP_CONTROLLER_H__
#define __FIXED_STEP_CONTROLLER_H__

#include "Util/Graph.h"

namespace illPc {
class PcConsole;
}

namespace Demo {
class GameControllerBase;
struct Engine;

class FixedStepController {
public:
    FixedStepController(GameControllerBase * gameController, Engine * engine, illPc::PcConsole * pcConsole);
    ~FixedStepController() {}

    void appLoop();

    inline void exitApp() {
        m_state = APPST_EXITING;
    }

    //TODO: take this out for a real game and use the renderer backend, this makes raw GL calls
    Graph m_fpsGraph;

private:
    enum State {
        APPST_INITIALIZED,
        APPST_RUNNING,
        APPST_EXITING
    };

    //TODO: take this out for a real game and use the renderer backend, this makes raw GL calls
    void renderFpsGraph();

    State m_state;
    GameControllerBase * m_gameController;
    Engine * m_engine;
    illPc::PcConsole * m_console;
};

}

#endif
