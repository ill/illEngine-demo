#ifndef ILL_GRAPHICS_VARS_H_
#define ILL_GRAPHICS_VARS_H_

#include "illEngine/Console/serial/VariableManager.h"
#include "illEngine/Console/serial/CommandManager.h"

///The screen width
extern illConsole::ConsoleVariable cv_vid_screenWidth;

///The screen height
extern illConsole::ConsoleVariable cv_vid_screenHeight;

///The color depth bits per pixel.  Nowadays this should always be 32.
extern illConsole::ConsoleVariable cv_vid_screenColors;

///Whether or not the window is windowed or fullscreen.
extern illConsole::ConsoleVariable cv_vid_fullScreen;

///The aspect ratio of the screen
extern illConsole::ConsoleVariable cv_vid_aspect;

///The command to call to apply the resolution changes
extern illConsole::ConsoleCommand cm_vid_applyResolution;

#endif