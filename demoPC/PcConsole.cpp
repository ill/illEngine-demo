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
    {
        illGraphics::BitmapFontLoadArgs loadArgs;
        loadArgs.m_path = "prototype12.fnt";

        m_font.load(loadArgs, m_engine->m_graphicsBackend);
    }

    //load the temporary font shader
    {
        std::vector<RefCountPtr<illGraphics::Shader> > shaders;

        illGraphics::Shader * shader = new illGraphics::Shader();
        shader->loadInternal(m_engine->m_graphicsBackend, "shaders/tempFont.vert", GL_VERTEX_SHADER, "");

        shaders.push_back(RefCountPtr<illGraphics::Shader>(shader));

        shader = new illGraphics::Shader();
        shader->loadInternal(m_engine->m_graphicsBackend, "shaders/tempFont.frag", GL_FRAGMENT_SHADER, "");

        shaders.push_back(RefCountPtr<illGraphics::Shader>(shader));

        m_internalShaderProgramLoader = new illGraphics::ShaderProgramLoader(m_engine->m_graphicsBackend, NULL);
        m_fontShader.loadInternal(m_internalShaderProgramLoader, shaders);
    }

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

void PcConsole::uninit() {
    m_font.unload();
    m_fontShader.unload();
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

//TODO: move this code out of here and into a thing that helps setup the draw calls for bitmap fonts
void tempConsTextRender(const char * text, const glm::mat4& transform, const illGraphics::BitmapFont& font, illGraphics::Camera& camera, GLuint prog) {
    glm::vec4 currColor = glm::vec4(1.0f);  //set the color to white initially

    glm::mat4 currentTransform = transform;

    GLuint buffer = *((GLuint *) font.getMesh().getMeshBackendData() + 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    GLint pos = getProgramAttribLocation(prog, "position");
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, (GLsizei) font.getMesh().getMeshFrontentData()->getVertexSize(), (char *)NULL + font.getMesh().getMeshFrontentData()->getPositionOffset());
    glEnableVertexAttribArray(pos);

    GLint tex = getProgramAttribLocation(prog, "texCoords");
    glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, (GLsizei) font.getMesh().getMeshFrontentData()->getVertexSize(), (char *)NULL + font.getMesh().getMeshFrontentData()->getTexCoordOffset());
    glEnableVertexAttribArray(tex);

    buffer = *((GLuint *) font.getMesh().getMeshBackendData() + 1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    
    while(*text) {
        //check if color code
        text = font.getColorCode(text, currColor);

        //parse special characters
        switch (*text) {
        case '\n':    //newline
            currentTransform = glm::translate(transform, glm::vec3(0.0f, (getTransformPosition(transform).y - getTransformPosition(currentTransform).y) - font.getLineHeight(), 0.0f));
            text++;
            continue;

        case ' ': //space
            currentTransform = glm::translate(currentTransform, glm::vec3(/*font.getSpacingHorz()*/5.0f, 0.0f, 0.0f));
            text++;
            continue;

        case '\t': //tab
            currentTransform = glm::translate(currentTransform, glm::vec3(font.getSpacingHorz() * 4.0f, 0.0f, 0.0f));
            text++;
            continue;
        }

        {
            GLint mvp = getProgramUniformLocation(prog, "modelViewProjectionMatrix");
            glUniformMatrix4fv(mvp, 1, false, glm::value_ptr(camera.getModelViewProjection() * currentTransform));
        }

        {
            GLint color = getProgramUniformLocation(prog, "color");
            glUniform4fv(color, 1, glm::value_ptr(currColor));
        }

        {
            GLuint texture = *((GLuint *) font.getPageTexture(font.getCharData(*text).m_texturePage).getTextureData());
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        
        if(font.getCharData(*text).m_advance != 0.0f) {
            glDrawRangeElements(GL_TRIANGLE_FAN, 
                font.getCharData(*text).m_meshIndex, font.getCharData(*text).m_meshIndex + 4, 4, 
                GL_UNSIGNED_SHORT, (char *)NULL + font.getCharData(*text).m_meshIndex * sizeof(uint16_t));
            
            currentTransform = glm::translate(currentTransform, glm::vec3(font.getCharData(*text).m_advance, 0.0f, 0.0f));
        }

        text++;
    }
    
    glDisableVertexAttribArray(pos);
    glDisableVertexAttribArray(tex);
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
                
        char prompt[CONS_ENTRY_SIZE + 6] = "^3>^7 ";        //yellow prompt arrow followed by white text
        strncat(prompt, m_entry, CONS_ENTRY_SIZE + 6);

        //cursor
        currY -= m_font.getPrintDimensions(prompt).y;

        if(m_cursorVisible) {
            size_t currPos = 0;
            glm::vec2 cursorPos = m_font.getCharLocation(prompt, m_typingInfo.m_selectionStart + 6, currPos);
            cursorPos.y += currY;// - m_font.getPrintDimensions(prompt).y;
            //cursorPos.x += 1.0f;

            //TROLLOL immediate mode.  TODO: redo later
            glBegin(GL_QUADS);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

                glVertex2f(cursorPos.x, cursorPos.y);
                glVertex2f(cursorPos.x + 2.0f, cursorPos.y);
                glVertex2f(cursorPos.x + 2.0f, cursorPos.y + m_font.getLineHeight());
                glVertex2f(cursorPos.x, cursorPos.y + m_font.getLineHeight());
            glEnd();
        }

        //this isn't really a very good way
        glUseProgram(getProgram(m_fontShader));

        {
            GLint diff = getProgramUniformLocation(getProgram(m_fontShader), "diffuseMap");
            glUniform1i(diff, 0);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getTexture(m_font.getPageTexture(0)));

        //render prompt        
        tempConsTextRender(prompt, glm::scale(glm::translate(glm::vec3(0.0f, currY, 0.0f)), glm::vec3(1.0f, -1.0f, 1.0f)), m_font, cam, getProgram(m_fontShader));
        
        for(auto lineIter = m_console->getLines().rbegin(); lineIter != m_console->getLines().rend() && currY > 0.0f; lineIter++) {
            currY -= m_font.getPrintDimensions(lineIter->c_str()).y;
            tempConsTextRender(lineIter->c_str(), glm::scale(glm::translate(glm::vec3(0.0f, currY, 0.0f)), glm::vec3(1.0f, -1.0f, 1.0f)), m_font, cam, getProgram(m_fontShader));            
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
            m_typingInfo.m_editRectCorner = glm::vec2(0.0f, (float) m_engine->m_window->m_screenHeight - m_font.getLineHeight());
            m_typingInfo.m_editRectSize = glm::vec2((float) m_engine->m_window->m_screenWidth, m_font.getLineHeight());
            m_typingInfo.m_destinationLimit = CONS_ENTRY_SIZE;
            m_typingInfo.m_selectionLength = 0;
            m_typingInfo.m_selectionStart = 0;

            m_engine->m_inputManager->getInputContextStack(0)->pushInputContext(&m_inputContext);

            m_engine->m_window->beginTypingInput(&m_typingInfo);
        }
    }
}
}