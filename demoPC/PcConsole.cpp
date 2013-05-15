#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "PcConsole.h"

#include "illEngine/Console/serial/DeveloperConsole.h"
#include "illEngine/Util/Geometry/geomUtil.h"
#include "illEngine/Graphics/serial/Camera/Camera.h"
#include "illEngine/Graphics/Window.h"
#include "illEngine/Input/serial/InputManager.h"

//TODO: move OpenGL code out of here.  That belongs in the renderer backend.
#include "illEngine/GlCommon/glLogging.h"
#include "illEngine/GlCommon/serial/glUtil.h"
#include <GL/glew.h>

#include "Util/CrappyBmFontRenderer.h"

namespace illPc {
PcConsole::PcConsole(Demo::Engine * engine, illConsole::DeveloperConsole * console)
    : m_engine(engine),
    m_console(console),
    m_state(State::HIDDEN),
    m_showOnscreen(false),
    m_openPercentage(0.0f),
    m_cursorVisible(false),
    m_cursorBlinkTimer(0.0f)
{
    m_entry[0] = 0;
    m_commandHistoryIter = m_commandHistory.rend();
}

void PcConsole::init() {
    m_cursorLeftListener.m_typingInfo = &m_typingInfo;
    m_cursorRightListener.m_typingInfo = &m_typingInfo;
    m_commandHistoryUpListener.m_console = this;
    m_commandHistoryDownListener.m_console = this;
    m_commandEnterListener.m_console = this;

    m_inputContext.bindInput("Cons_CursLeft", &m_cursorLeftListener);
    m_inputContext.bindInput("Cons_CursRight", &m_cursorRightListener);
    m_inputContext.bindInput("Cons_HistoryUp", &m_commandHistoryUpListener);
    m_inputContext.bindInput("Cons_HistoryDown", &m_commandHistoryDownListener);
    m_inputContext.bindInput("Cons_CommandEnter", &m_commandEnterListener);
}

void PcConsole::hide() {
    if(m_state != State::HIDDEN) {
        if(m_state == State::VISIBLE) {
            m_engine->m_window->endTypingInput();
            m_engine->m_inputManager->getInputContextStack(0)->popInputContext();
        }

        m_state = State::SLIDE_UP;
    }
}

void PcConsole::show() {
    if(m_state != State::VISIBLE) {
        m_state = State::SLIDE_DOWN;
    }
}

void PcConsole::render() {
    illGraphics::Camera cam;
    cam.setViewport(glm::ivec2(0), glm::ivec2(m_engine->m_window->m_screenWidth, m_engine->m_window->m_screenHeight));
    cam.setOrthoTransform(glm::mat4(), 0.0f, (float) m_engine->m_window->m_screenWidth, (float) m_engine->m_window->m_screenHeight, 0.0f);

    if(m_state != State::HIDDEN) {
        glm::vec2 consoleDimensions((float) m_engine->m_window->m_screenWidth, (float) m_engine->m_window->m_screenHeight * m_openPercentage);

        //for now render stuff with deprecated techniques real quick
        //TODO: do this with a VBO and a shader like a real man!!!!!
        glUseProgram(0);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(cam.getProjection()));

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(cam.getModelView()));

        glBegin(GL_QUADS);
            glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(consoleDimensions.x, 0.0f);
            glColor4f(0.3f, 0.3f, 0.3f, 0.5f);
            glVertex2f(consoleDimensions.x, consoleDimensions.y);
            glVertex2f(0.0f, consoleDimensions.y);
        glEnd();

        glm::mediump_float currY = consoleDimensions.y;
                
        char prompt[CONS_ENTRY_SIZE + 6] = "^2>^7 ";        //lime green prompt arrow followed by white text
        strncat(prompt, m_entry, CONS_ENTRY_SIZE + 6);

        //cursor
        currY -= m_engine->m_crappyFontRenderer->m_font.getPrintDimensions(prompt).y;

        if(m_cursorVisible) {
            size_t currPos = 0;
            glm::vec2 cursorPos = m_engine->m_crappyFontRenderer->m_font.getCharLocation(prompt, m_typingInfo.m_selectionStart + 6, currPos);
            cursorPos.y += currY;// - m_font.getPrintDimensions(prompt).y;
            //cursorPos.x += 1.0f;

            //TROLLOL immediate mode.  TODO: redo later
            glBegin(GL_QUADS);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

                glVertex2f(cursorPos.x, cursorPos.y);
                glVertex2f(cursorPos.x + 2.0f, cursorPos.y);
                glVertex2f(cursorPos.x + 2.0f, cursorPos.y + m_engine->m_crappyFontRenderer->m_font.getLineHeight());
                glVertex2f(cursorPos.x, cursorPos.y + m_engine->m_crappyFontRenderer->m_font.getLineHeight());
            glEnd();
        }

        //TODO setup
        m_engine->m_crappyFontRenderer->setupRender();

        //render prompt        
        m_engine->m_crappyFontRenderer->render(prompt, glm::scale(glm::translate(glm::vec3(0.0f, currY, 0.0f)), glm::vec3(1.0f, -1.0f, 1.0f)), cam);
        
        for(auto lineIter = m_console->getLines().rbegin(); lineIter != m_console->getLines().rend() && currY > 0.0f; lineIter++) {
            currY -= m_engine->m_crappyFontRenderer->m_font.getPrintDimensions(lineIter->c_str()).y;
            m_engine->m_crappyFontRenderer->render(lineIter->c_str(), glm::scale(glm::translate(glm::vec3(0.0f, currY, 0.0f)), glm::vec3(1.0f, -1.0f, 1.0f)), cam);            
        }
    }
}

void PcConsole::update(float seconds) {
    m_cursorBlinkTimer -= seconds;
    
    if(m_cursorBlinkTimer < 0.0f) {
        m_cursorBlinkTimer = 0.25f;
        m_cursorVisible = !m_cursorVisible;
    }

    if(m_state == State::SLIDE_UP) {
        m_openPercentage -= seconds * 4.0f;

        if(m_openPercentage < 0.0f) {
            m_state = State::HIDDEN;
            m_openPercentage = 0.0f;
        }
    }
    else if(m_state == State::SLIDE_DOWN) {
        m_openPercentage += seconds * 4.0f;

        if(m_openPercentage > 1.0f) {
            m_state = State::VISIBLE;
            m_openPercentage = 1.0f;

            m_typingInfo.m_destination = m_entry;
            m_typingInfo.m_editRectCorner = glm::vec2(0.0f, (float) m_engine->m_window->m_screenHeight - m_engine->m_crappyFontRenderer->m_font.getLineHeight());
            m_typingInfo.m_editRectSize = glm::vec2((float) m_engine->m_window->m_screenWidth, m_engine->m_crappyFontRenderer->m_font.getLineHeight());
            m_typingInfo.m_destinationLimit = CONS_ENTRY_SIZE;
            m_typingInfo.m_selectionLength = 0;
            m_typingInfo.m_selectionStart = 0;

            m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_inputContext);

            m_engine->m_window->beginTypingInput(&m_typingInfo);
        }
    }
}
}