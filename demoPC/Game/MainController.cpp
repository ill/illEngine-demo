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
    m_startSkeletalAnimationDemoListener.m_controller = this;
    m_startSkeletalAnimationDemoListener.m_startFunc = &MainController::startSkeletalAnimationDemo;

    m_startFrustumIterVisualizerListener.m_controller = this;
    m_startFrustumIterVisualizerListener.m_startFunc = &MainController::startFrustumIterVisualizer;
     
    m_startFrustumCullVisualizerListener.m_controller = this;
    m_startFrustumCullVisualizerListener.m_startFunc = &MainController::startFrustumCullVisualizer;

    m_startRendererDemoTheGridListener.m_controller = this;
    m_startRendererDemoTheGridListener.m_startFunc = &MainController::startRendererDemoTheGrid;

    m_startRendererDemoOrganizedListener.m_controller = this;
    m_startRendererDemoOrganizedListener.m_startFunc = &MainController::startRendererDemoOrganized;

    m_startRendererDemoShortChaosListener.m_controller = this;
    m_startRendererDemoShortChaosListener.m_startFunc = &MainController::startRendererDemoShortChaos;

    m_startRendererDemoChaosListener.m_controller = this;
    m_startRendererDemoChaosListener.m_startFunc = &MainController::startRendererDemoChaos;
    
    m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_inputContext);

    startRendererDemoTheGrid();
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

void MainController::startRendererDemoTheGrid() {
    delete m_subGame;
    setSubGame(new RendererDemoController(m_engine, RendererDemoController::Scene::THE_GRID));
}

void MainController::startRendererDemoOrganized() {
    delete m_subGame;
    setSubGame(new RendererDemoController(m_engine, RendererDemoController::Scene::ORGANIZED));
}

void MainController::startRendererDemoShortChaos() {
    delete m_subGame;
    setSubGame(new RendererDemoController(m_engine, RendererDemoController::Scene::SHORT_CHAOS));
}

void MainController::startRendererDemoChaos() {
    delete m_subGame;
    setSubGame(new RendererDemoController(m_engine, RendererDemoController::Scene::CHAOS));
}

}