#include <stdint.h>
#include <SDL.h>

#include "FixedStepController.h"
#include "GameControllerBase.h"

#include "Engine.h"
#include "illEngine/Graphics/Window.h"
#include "illEngine/Graphics/GraphicsBackend.h"
#include "illEngine/Logging/logging.h"
#include "PcConsole.h"

//TODO: some stuff for the FPS graph that should be moved later
#include "illEngine/Graphics/serial/Camera/Camera.h"

namespace Demo {

const float STEP_SIZE = 1000.0f / 60.0f;     //makes application run at a base FPS of 60
const int MAX_STEPS = 10;

FixedStepController::FixedStepController(GameControllerBase * gameController, Engine * engine, illPc::PcConsole * pcConsole) 
    : m_state(APPST_INITIALIZED),
    m_gameController(gameController),
    m_engine(engine),
    m_console(pcConsole)
{
    m_fpsGraph.m_fontRenderer = m_engine->m_crappyFontRenderer;
    m_fpsGraph.m_name.assign("FPS");
}

void FixedStepController::renderFpsGraph() {
    illGraphics::Camera cam;
    cam.setOrthoTransform(glm::mat4(), 0.0f, m_engine->m_window->m_screenWidth, 0.0f, m_engine->m_window->m_screenHeight);
    m_fpsGraph.render(glm::mat4(), cam);
}

void FixedStepController::appLoop() {      
    //if the application controller isn't in the expected state error
    if (m_state != APPST_INITIALIZED) {
        LOG_FATAL_ERROR("Application in unexpected state.");
    }

    m_state = APPST_RUNNING;

    float timeAccumulator = 0.0f;
    int32_t lastLoopTime = SDL_GetTicks();

    while (m_state == APPST_RUNNING) {
        //poll input events from the game's window
        m_engine->m_window->pollEvents();

        //compute time since last game loop
        int32_t currentLoopTime = SDL_GetTicks();
        int32_t milliseconds = currentLoopTime - lastLoopTime;
        lastLoopTime = currentLoopTime;
        timeAccumulator += milliseconds;
        
        int steps = 0;

        float seconds = (float) milliseconds / 1000.0f;

        if(m_engine->m_showingFps && seconds > 0.0f) {
            m_fpsGraph.addDataPoint(1.0f / seconds);
        }

        //update developer console
        m_console->update(seconds);

        //run the game loop with a fixed step
        while(timeAccumulator > STEP_SIZE && steps++ < MAX_STEPS) {
            timeAccumulator -= STEP_SIZE;

            m_gameController->update(STEP_SIZE / 1000.0f);
        }
        
        /////////////////////
        //update sound
        m_gameController->updateSound(seconds);

        /////////////////////
        //draw screen
        m_engine->m_window->beginFrame();
        m_engine->m_graphicsBackend->beginFrame();
                
        m_gameController->render();
                
        m_console->render();        
        
        if(m_engine->m_showingFps) {
            renderFpsGraph();
        }

        m_engine->m_graphicsBackend->endFrame();
        m_engine->m_window->endFrame();

        //force delay 1 ms to avoid precision issues
        SDL_Delay(1);
    }
}

}
