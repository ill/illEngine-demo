#ifndef ILL_GRAPHICS_VARS_H_
#define ILL_GRAPHICS_VARS_H_

#include "illEngine/Console/serial/VariableManager.h"
#include "illEngine/Console/serial/CommandManager.h"

extern const char * VID_SCREEN_WIDTH_DESC;
extern const char * VID_SCREEN_WIDTH_NAME;
extern illConsole::ConsoleVariable cv_vid_screenWidth;

extern const char * VID_SCREEN_HEIGHT_DESC;
extern const char * VID_SCREEN_HEIGHT_NAME;
extern illConsole::ConsoleVariable cv_vid_screenHeight;

extern const char * VID_COLOR_DEPTH_DESC;
extern const char * VID_COLOR_DEPTH_NAME;
extern illConsole::ConsoleVariable cv_vid_colorDepth;

extern const char * VID_FULL_SCREEN_DESC;
extern const char * VID_FULL_SCREEN_NAME;
extern illConsole::ConsoleVariable cv_vid_fullScreen;

extern const char * VID_ASPECT_DESC;
extern const char * VID_ASPECT_NAME;
extern illConsole::ConsoleVariable cv_vid_aspect;

extern const char * VID_APPLY_RESOLUTION_DESC;
extern const char * VID_APPLY_RESOLUTION_NAME;
extern illConsole::ConsoleCommand cm_vid_applyResolution;

#endif