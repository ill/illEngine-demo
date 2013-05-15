#include "graphicsVars.h"

const char * VID_SHOW_FPS_DESC = "Shows a frames per second graph if enabled";
const char * VID_SHOW_FPS_NAME = "vid_showFPS";

const char * VID_SCREEN_WIDTH_DESC = "Screen resolution width in pixels. Call vid_applyResolution to make changes take place after setting.";
const char * VID_SCREEN_WIDTH_NAME = "vid_screenWidth";

const char * VID_SCREEN_HEIGHT_DESC = "Screen resolution height in pixels. Call vid_applyResolution to make changes take place after setting.";
const char * VID_SCREEN_HEIGHT_NAME = "vid_screenHeight";

const char * VID_COLOR_DEPTH_DESC = "The color depth bits per pixel. Nowadays this should pretty much always be 32. Call vid_applyResolution to make changes take place after setting.";
const char * VID_COLOR_DEPTH_NAME = "vid_colorDepth";

const char * VID_FULL_SCREEN_DESC = "Whether or not windowed or full screen. Call vid_applyResolution to make changes take place after setting.";
const char * VID_FULL_SCREEN_NAME = "vid_fullScreen";

const char * VID_ASPECT_DESC = "The aspect ratio of the screen. "
    "0 to automatically base on screen resolution."
    "4:3, 16:9, or 16:10 for those standard ratios. "
    "You can also type in a random fractional number for some custom aspect ratio.";
const char * VID_ASPECT_NAME = "vid_aspect";

const char * VID_APPLY_RESOLUTION_DESC = "Applies the screen resolution changes as set in the variables vid_screenWidth, vid_screenHeight, vid_colorDepth, vid_fullScreen.";
const char * VID_APPLY_RESOLUTION_NAME = "vid_applyResolution";