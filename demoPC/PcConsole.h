#ifndef ILL_PC_CONSOLE_H_
#define ILL_PC_CONSOLE_H_

#include <map>
#include <cstring>
#include <string>

#include "illEngine/Graphics/serial/BitmapFont.h"
#include "illEngine/Graphics/serial/material/ShaderProgram.h"
#include "Engine.h"
#include "illEngine/Graphics/Window.h"

#include "illEngine/Input/serial/InputContext.h"
#include "illEngine/Input/serial/listeners/StateListener.h"
#include "illEngine/Console/serial/DeveloperConsole.h"

namespace illPc {
const size_t CONS_ENTRY_SIZE = 256;
const size_t CONS_HISTORY_SIZE = 16;

class PcConsole {
public:
    enum class State {
        HIDDEN,         //< Console is hidden
        SLIDE_DOWN,     //< Console is animating down to be visible
        VISIBLE,        //< Console is visible
        SLIDE_UP        //< Console is animating up to hide
    };

    PcConsole(Demo::Engine * engine, illConsole::DeveloperConsole * console);

    ~PcConsole() {
        delete m_internalShaderProgramLoader;
    }

    void init();
    void uninit();

    void hide();
    void show();

    void render();
    void update(float seconds);
     
    ///Whether or not the console messages still show up even when hidden
    bool m_showOnscreen;

private:    
    illGraphics::BitmapFont m_font;
    illGraphics::ShaderProgram m_fontShader;
    illGraphics::ShaderProgramLoader * m_internalShaderProgramLoader;

    std::list<std::string>::iterator m_commandHistoryIter;
    std::list<std::string> m_commandHistory;
    char m_entry[CONS_ENTRY_SIZE];
    
    float m_cursorBlinkTimer;
    bool m_cursorVisible;
    illGraphics::Window::TypingInfo m_typingInfo;

    Demo::Engine * m_engine;
    illConsole::DeveloperConsole * m_console;

    State m_state;

    //TODO: switch to using Lambdas for input listeners, this seriously sucks and is cumbersome!
    struct CursorLeftListener : public illInput::StateListener {
        virtual void onPress() {
            if(m_typingInfo->m_selectionStart > 0) {
                --m_typingInfo->m_selectionStart;
            }
        }

        illGraphics::Window::TypingInfo * m_typingInfo;
    };

    struct CursorRightListener : public illInput::StateListener {
        virtual void onPress() {
            size_t textLen = strlen(m_typingInfo->m_destination);

            if(m_typingInfo->m_selectionStart < textLen) {
                ++m_typingInfo->m_selectionStart;
            }
        }

        illGraphics::Window::TypingInfo * m_typingInfo;
    };

    struct CommandHistoryUpListener : public illInput::StateListener {
        virtual void onPress() {
            if(m_console->m_commandHistory.empty()) {
                return;
            }

            if(++m_console->m_commandHistoryIter == m_console->m_commandHistory.end()) {
                --m_console->m_commandHistoryIter;
            }

            const std::string& commandText = *m_console->m_commandHistoryIter;
            memcpy(m_console->m_typingInfo.m_destination, commandText.c_str(), commandText.length());
            m_console->m_typingInfo.m_destination[commandText.length()] = 0;
        }

        PcConsole * m_console;
    };

    struct CommandHistoryDownListener : public illInput::StateListener {
        virtual void onPress() {
            if(m_console->m_commandHistory.empty()) {
                return;
            }

            if(m_console->m_commandHistoryIter != m_console->m_commandHistory.begin()) {
                m_console->m_commandHistoryIter--;
            }

            const std::string& commandText = *m_console->m_commandHistoryIter;
            memcpy(m_console->m_typingInfo.m_destination, commandText.c_str(), commandText.length());
            m_console->m_typingInfo.m_destination[commandText.length()] = 0;
        }

        PcConsole * m_console;
    };

    struct CommandEnterListener : public illInput::StateListener {
        virtual void onPress() {
            //don't do anything if the string is empty
            if(m_console->m_typingInfo.m_destination[0]) {
                if(m_console->m_commandHistory.size() > CONS_HISTORY_SIZE) {
                    m_console->m_commandHistory.pop_back();
                }

                //RUN!!!!
                m_console->m_console->parseInput(m_console->m_typingInfo.m_destination);

                m_console->m_commandHistory.emplace_front(m_console->m_typingInfo.m_destination);
                m_console->m_commandHistoryIter = m_console->m_commandHistory.begin();
                m_console->m_typingInfo.m_destination[0] = 0;
                m_console->m_typingInfo.m_selectionStart = 0;
            }
        }

        PcConsole * m_console;
    };

    CursorLeftListener m_cursorLeftListener;
    CursorRightListener m_cursorRightListener;
    CommandHistoryUpListener m_commandHistoryUpListener;
    CommandHistoryDownListener m_commandHistoryDownListener;
    CommandEnterListener m_commandEnterListener;

    illInput::InputContext m_inputContext;

    ///As the console is animating sliding up or sliding down, what percentage done is it
    float m_openPercentage;
};
}

#endif