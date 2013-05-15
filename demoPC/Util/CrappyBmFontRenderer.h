#ifndef ILL_CRAPPY_BM_FONT_RENDERER_H_
#define ILL_CRAPPY_BM_FONT_RENDERER_H_

#include <glm/glm.hpp>
#include <GL/glew.h>
#include "illEngine/Graphics/serial/BitmapFont.h"
#include "illEngine/Graphics/serial/Camera/Camera.h"
#include "illEngine/Graphics/serial/material/ShaderProgram.h"

namespace Demo {
struct Engine;
}

/**
The reason I call this the crappy BmFont renderer is because it uses immediate mode and doesn't make use of
the actual rendering pipeline by making direct GL calls.  The reason is some of the demos
don't use the renderer and also render things with direct GL calls.

A real game wouldn't do this and would be using the renderer backend to do the actual calls, so I'm adding this for convenience here.
*/
struct CrappyBmFontRenderer {
    CrappyBmFontRenderer(Demo::Engine * engine);
    ~CrappyBmFontRenderer();

    /**
    Call this once before making a series of font render calls.
    */
    void setupRender();

    /*
    @param text The text to render
    @param transform The transform of the text, this means you can render text in ortho or 3D
    @param camera The camera view used to render the font

    Coming soon, Horizontal and vertical alignment.
    */
    void render(const char * text, const glm::mat4& transform, illGraphics::Camera& camera);

    Demo::Engine * m_engine;
    illGraphics::BitmapFont m_font;
    illGraphics::ShaderProgram m_fontShader;
    illGraphics::ShaderProgramLoader * m_internalShaderProgramLoader;
};

#endif