#include "CrappyBmFontRenderer.h"
#include "../Engine.h"
#include "illEngine/Graphics/serial/Camera/Camera.h"
#include "illEngine/GlCommon/serial/glUtil.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

CrappyBmFontRenderer::CrappyBmFontRenderer(Demo::Engine * engine) 
    : m_engine(engine)
{
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
}

CrappyBmFontRenderer::~CrappyBmFontRenderer() {
    m_font.unload();
    m_fontShader.unload();
    delete m_internalShaderProgramLoader;
}

void CrappyBmFontRenderer::setupRender() {
    //this isn't really a very good way
    glUseProgram(getProgram(m_fontShader));

    {
        GLint diff = getProgramUniformLocation(getProgram(m_fontShader), "diffuseMap");
        glUniform1i(diff, 0);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, getTexture(m_font.getPageTexture(0)));
}

void CrappyBmFontRenderer::render(const char * text, const glm::mat4& transform, illGraphics::Camera& camera) {
    glm::vec4 currColor = glm::vec4(1.0f);  //set the color to white initially

    glm::mat4 currentTransform = transform;

    GLuint buffer = *((GLuint *) m_font.getMesh().getMeshBackendData() + 0);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    GLint pos = getProgramAttribLocation(getProgram(m_fontShader), "position");
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, (GLsizei) m_font.getMesh().getMeshFrontentData()->getVertexSize(), (char *)NULL + m_font.getMesh().getMeshFrontentData()->getPositionOffset());
    glEnableVertexAttribArray(pos);

    GLint tex = getProgramAttribLocation(getProgram(m_fontShader), "texCoords");
    glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, (GLsizei) m_font.getMesh().getMeshFrontentData()->getVertexSize(), (char *)NULL + m_font.getMesh().getMeshFrontentData()->getTexCoordOffset());
    glEnableVertexAttribArray(tex);

    buffer = *((GLuint *) m_font.getMesh().getMeshBackendData() + 1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    
    while(*text) {
        //check if color code
        text = m_font.getColorCode(text, currColor);

        //parse special characters
        switch (*text) {
        case '\n':    //newline
            currentTransform = glm::translate(transform, glm::vec3(0.0f, (getTransformPosition(transform).y - getTransformPosition(currentTransform).y) - m_font.getLineHeight(), 0.0f));
            text++;
            continue;

        case ' ': //space
            currentTransform = glm::translate(currentTransform, glm::vec3(/*font.getSpacingHorz()*/5.0f, 0.0f, 0.0f));
            text++;
            continue;

        case '\t': //tab
            currentTransform = glm::translate(currentTransform, glm::vec3(m_font.getSpacingHorz() * 4.0f, 0.0f, 0.0f));
            text++;
            continue;
        }

        {
            GLint mvp = getProgramUniformLocation(getProgram(m_fontShader), "modelViewProjectionMatrix");
            glUniformMatrix4fv(mvp, 1, false, glm::value_ptr(camera.getModelViewProjection() * currentTransform));
        }

        {
            GLint color = getProgramUniformLocation(getProgram(m_fontShader), "color");
            glUniform4fv(color, 1, glm::value_ptr(currColor));
        }

        {
            GLuint texture = *((GLuint *) m_font.getPageTexture(m_font.getCharData(*text).m_texturePage).getTextureData());
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        
        if(m_font.getCharData(*text).m_advance != 0.0f) {
            glDrawRangeElements(GL_TRIANGLE_FAN, 
                m_font.getCharData(*text).m_meshIndex, m_font.getCharData(*text).m_meshIndex + 4, 4, 
                GL_UNSIGNED_SHORT, (char *)NULL + m_font.getCharData(*text).m_meshIndex * sizeof(uint16_t));
            
            currentTransform = glm::translate(currentTransform, glm::vec3(m_font.getCharData(*text).m_advance, 0.0f, 0.0f));
        }

        text++;
    }
    
    glDisableVertexAttribArray(pos);
    glDisableVertexAttribArray(tex);
}