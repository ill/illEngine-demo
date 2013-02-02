#include "MainController.h"
#include "SkeletalAnimationDemo/SkeletalAnimationDemoController.h"
#include "FrustumIterVisualizer/FrustumIterVisualizerController.h"
#include "FrustumCullVisualizer/FrustumCullVisualizerController.h"
#include "RendererDemo/RendererDemoController.h"

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

    m_startSkeletalAnimationDemoListener.m_controller = this;
    m_startSkeletalAnimationDemoListener.m_startFunc = &MainController::startSkeletalAnimationDemo;

    m_startFrustumIterVisualizerListener.m_controller = this;
    m_startFrustumIterVisualizerListener.m_startFunc = &MainController::startFrustumIterVisualizer;
     
    m_startFrustumCullVisualizerListener.m_controller = this;
    m_startFrustumCullVisualizerListener.m_startFunc = &MainController::startFrustumCullVisualizer;

    m_startRendererDemoListener.m_controller = this;
    m_startRendererDemoListener.m_startFunc = &MainController::startRendererDemo;

    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_F9), &m_startSkeletalAnimationDemoListener);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_F10), &m_startFrustumIterVisualizerListener);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_F11), &m_startFrustumCullVisualizerListener);
    m_inputContext.bindInput(illInput::InputBinding(SdlPc::PC_KEYBOARD, SDLK_F8), &m_startRendererDemoListener);

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

void MainController::startRendererDemo() {
    delete m_subGame;
    setSubGame(new RendererDemoController(m_engine));
}

}