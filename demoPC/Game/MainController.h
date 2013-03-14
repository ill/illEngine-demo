#ifndef __MAIN_CONTROLLER_H__
#define __MAIN_CONTROLLER_H__

#include <cstdlib>
#include "../GameControllerBase.h"

#include "illEngine/Input/serial/InputContext.h"
#include "illEngine/Input/serial/Listeners/StateListener.h"

namespace Demo {

struct Engine;

class MainController : public GameControllerBase {
public:
    enum State {
        APPST_PREGAME_SPLASH,
        APPST_SINGLE_PLAYER
    };

    MainController(Engine * engine);

    ~MainController() {
        delete m_subGame;
    }

    void update(float seconds) {
        m_subGame->update(seconds);
    }

    void updateSound(float seconds) {
        m_subGame->updateSound(seconds);
    }

    void render() {
        m_subGame->render();
    }

    void startSkeletalAnimationDemo();
    void startFrustumIterVisualizer();
    void startFrustumCullVisualizer();
    void startRendererDemoOrganized();
    void startRendererDemoShortChaos();
    void startRendererDemoChaos();

private:
    inline void setSubGame(GameControllerBase * subGame) {        
        m_subGame = subGame;
        update(1.0f);
    }

    struct SetGame : public illInput::StateListener {
        SetGame()
            : illInput::StateListener()
        {}

        virtual ~SetGame() {}

        void onRelease() {
            (m_controller->*m_startFunc)();
        }

        void (MainController::*m_startFunc)();
        MainController * m_controller;
    };

    illInput::InputContext m_inputContext;
    
    SetGame m_startSkeletalAnimationDemoListener;    
    SetGame m_startFrustumIterVisualizerListener;
    SetGame m_startFrustumCullVisualizerListener;
    SetGame m_startRendererDemoOrganizedListener;
    SetGame m_startRendererDemoShortChaosListener;
    SetGame m_startRendererDemoChaosListener;

    Engine * m_engine;
    State m_state;
    GameControllerBase * m_subGame;
};

}

#endif