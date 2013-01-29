#include "MainController.h"
#include "SkeletalAnimationDemo/SkeletalAnimationDemoController.h"
#include "FrustumIterVisualizer/FrustumIterVisualizerController.h"
#include "FrustumCullVisualizer/FrustumCullVisualizerController.h"

#include "../Engine.h"

#include "illEngine/Input/serial/InputManager.h"
#include "illEngine/Input/serial/InputBinding.h"
#include "illEngine/Pc/serial/sdlInputEnum.h"

namespace Demo {

MainController::MainController(Engine * engine)
    : GameControllerBase(),
    m_engine(engine),
    m_subGame(NULL)
{
    //initialize input
    m_engine->m_inputManager->addPlayer(0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_KEYBOARD, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE_BUTTON, 0);
    m_engine->m_inputManager->bindDevice(SdlPc::PC_MOUSE_WHEEL, 0);

    m_startSkeletalAnimationDemoCallback.m_controller = this;
    m_startSkeletalAnimationDemoCallback.m_startFunc = &MainController::startSkeletalAnimationDemo;

    m_startFrustumIterVisualizerCallback.m_controller = this;
    m_startFrustumIterVisualizerCallback.m_startFunc = &MainController::startFrustumIterVisualizer;
     
    m_startFrustumCullVisualizerCallback.m_controller = this;
    m_startFrustumCullVisualizerCallback.m_startFunc = &MainController::startFrustumCullVisualizer;
    
    m_startSkeletalAnimationDemoState.m_inputCallback = &m_startSkeletalAnimationDemoCallback;
    m_startFrustumIterVisualizerState.m_inputCallback = &m_startFrustumIterVisualizerCallback;
    m_startFrustumCullVisualizerState.m_inputCallback = &m_startFrustumCullVisualizerCallback;

    m_inputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_F9), &m_startSkeletalAnimationDemoState);
    m_inputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_F10), &m_startFrustumIterVisualizerState);
    m_inputContext.bindInput(Input::InputBinding(SdlPc::PC_KEYBOARD, SDLK_F11), &m_startFrustumCullVisualizerState);

    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_inputContext);

    startSkeletalAnimationDemo();
}

/*void MainController::startMainMenu() {
   m_state = APPST_PREGAME_SPLASH;
   setSubGame(new MainMenuController(m_engine));
}

void MainController::startSinglePlayer() {
   m_state = APPST_SINGLE_PLAYER;
   //setSubGame(new SinglePlayer::SinglePlayerGameController(this));
}*/

void MainController::startSkeletalAnimationDemo() {
    delete m_subGame;
    setSubGame(new SkeletalAnimationDemoController(m_engine));
}

void MainController::startFrustumIterVisualizer() {
    delete m_subGame;
    setSubGame(new FrustumIterVisualizerController(m_engine));
}

void MainController::startFrustumCullVisualizer() {
    delete m_subGame;
    setSubGame(new FrustumCullVisualizerController(m_engine));
}

}