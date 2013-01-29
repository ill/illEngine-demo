#ifndef __MAIN_CONTROLLER_H__
#define __MAIN_CONTROLLER_H__

#include <cstdlib>
#include "../GameControllerBase.h"

#include "illEngine/Input/serial/InputContext.h"
#include "illEngine/Input/serial/InputListenerState.h"

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

    void startFrustumIterVisualizer();
    void startSkeletalAnimationDemo();

private:
    inline void setSubGame(GameControllerBase * subGame) {        
        m_subGame = subGame;
        update(1.0f);
    }

    struct SetGame : public Input::InputListenerState::InputCallback {
        SetGame()
            : Input::InputListenerState::InputCallback()
        {}

        virtual ~SetGame() {}

        void onRelease() {
            (m_controller->*m_startFunc)();
        }

        void (MainController::*m_startFunc)();
        MainController * m_controller;
    };

    Input::InputContext m_inputContext;

    Input::InputListenerState m_startFrustumIterVisualizerState;
    Input::InputListenerState m_startSkeletalAnimationDemoState;

    SetGame m_startFrustumIterVisualizerCallback;
    SetGame m_startSkeletalAnimationDemoCallback;

    Engine * m_engine;
    State m_state;
    GameControllerBase * m_subGame;
};

}

#endif