#include <cstring>
#include "illEngine/Pc/serial/sdlInputEnum.h"
#include "inputVars.h"

const char * BIND_DESC = "Binds an input to a command name. "
    "A command name may then be put in an input context later. "
    "You can have the same input bound to multiple commands since "
    "you may have multiple contexts when that key can trigger an action. "

    "Example usage bind UP Forward "

    "You can also bind to a console command. bind F10 toggle vid_showFps"
    "bind <input> <command name or console commands>";
const char * BIND_NAME = "bind";

const char * UNBIND_DESC = "Unbinds an input from a command. "
    "unbind <input> <command name or console commands>";
const char * UNBIND_NAME = "unbind";

const char * SAVE_BIND_DESC = "Saves a binding permanently "
    "saveBind <input> <command name or console commands>";
const char * SAVE_BIND_NAME = "saveBind";

const char * PRINT_INPUT_BINDS_DESC = "Prints the input bindings. "
    "You can optionally provide a file to output them to. "
    "printInputBinds <optional file name>";
const char * PRINT_INPUT_BINDS_NAME = "printInputBinds";

illInput::InputBinding consoleInputToBinding(const char * input) {
    //this is most likely an ascii character
    if(strnlen(input, 2) == 1) {        
        if(input[0] >= 33 && input[0] <= 126) {            
            if(input[0] >= 65 && input[0] <= 90) {  //upper case
                return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, input[0] + 32);
            }
            else {                                  //plain ascii
                return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, input[0]);
            }
        }
        else {
            return illInput::InputBinding((int)SdlPc::InputType::INVALID, 0);
        }
    }

    if(strncmp(input, "BACKSPACE", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_BACKSPACE);
    }

    if(strncmp(input, "TAB", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_TAB);
    }

    if(strncmp(input, "CLEAR", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_CLEAR);
    }

    if(strncmp(input, "RETURN", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_RETURN);
    }

    if(strncmp(input, "ENTER", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_RETURN);
    }

    if(strncmp(input, "PAUSE", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_PAUSE);
    }

    if(strncmp(input, "DELETE", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_DELETE);
    }

    if(strncmp(input, "KP_000", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_000);
    }

    if(strncmp(input, "KP_00", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_00);
    }

    if(strncmp(input, "KP_0", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_0);
    }

    if(strncmp(input, "KP_1", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_1);
    }

    if(strncmp(input, "KP_2", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_2);
    }

    if(strncmp(input, "KP_3", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_3);
    }

    if(strncmp(input, "KP_4", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_4);
    }

    if(strncmp(input, "KP_5", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_5);
    }

    if(strncmp(input, "KP_6", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_6);
    }

    if(strncmp(input, "KP_7", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_7);
    }

    if(strncmp(input, "KP_8", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_8);
    }

    if(strncmp(input, "KP_9", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_9);
    }

    if(strncmp(input, "KP_PERIOD", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_PERIOD);
    }

    if(strncmp(input, "KP_DIVIDE", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_DIVIDE);
    }

    if(strncmp(input, "KP_MULTIPLY", 12) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_MULTIPLY);
    }

    if(strncmp(input, "KP_MINUS", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_MINUS);
    }

    if(strncmp(input, "KP_PLUS", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_PLUS);
    }

    if(strncmp(input, "KP_ENTER", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_ENTER);
    }

    if(strncmp(input, "KP_EQUALS", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_KP_EQUALS);
    }
    
    if(strncmp(input, "UP", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_UP);
    }

    if(strncmp(input, "DOWN", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_DOWN);
    }

    if(strncmp(input, "RIGHT", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_RIGHT);
    }

    if(strncmp(input, "LEFT", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_LEFT);
    }

    if(strncmp(input, "INSERT", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_INSERT);
    }

    if(strncmp(input, "HOME", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_HOME);
    }

    if(strncmp(input, "END", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_END);
    }

    if(strncmp(input, "PAGEUP", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_PAGEUP);
    }

    if(strncmp(input, "PAGEDOWN", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_PAGEDOWN);
    }

    if(strncmp(input, "F1", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F1);
    }

    if(strncmp(input, "F2", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F2);
    }

    if(strncmp(input, "F3", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F3);
    }

    if(strncmp(input, "F4", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F4);
    }

    if(strncmp(input, "F5", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F5);
    }

    if(strncmp(input, "F6", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F6);
    }

    if(strncmp(input, "F7", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F7);
    }

    if(strncmp(input, "F8", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F8);
    }

    if(strncmp(input, "F9", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F9);
    }

    if(strncmp(input, "F10", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F10);
    }

    if(strncmp(input, "F11", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F11);
    }

    if(strncmp(input, "F12", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F12);
    }

    if(strncmp(input, "F13", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F13);
    }

    if(strncmp(input, "F14", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F14);
    }

    if(strncmp(input, "F15", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_F15);
    }

    if(strncmp(input, "NUM_LOCK", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_NUMLOCKCLEAR);
    }

    if(strncmp(input, "CAPS_LOCK", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_CAPSLOCK);
    }

    if(strncmp(input, "SCROLL_LOCK", 15) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_SCROLLLOCK);
    }

    if(strncmp(input, "RSHIFT", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_RSHIFT);
    }

    if(strncmp(input, "LSHIFT", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_LSHIFT);
    }

    if(strncmp(input, "RCTRL", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_RCTRL);
    }

    if(strncmp(input, "LCTRL", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_LCTRL);
    }

    if(strncmp(input, "RALT", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_RALT);
    }

    if(strncmp(input, "LALT", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_LALT);
    }
    
    if(strncmp(input, "SPACE", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_SPACE);
    }

    if(strncmp(input, "PRINT_SCREEN", 15) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_PRINTSCREEN);
    }

    if(strncmp(input, "ESC", 10) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_KEYBOARD, SDLK_ESCAPE);
    }

    //TODO: more keyboard stuff to come
    
    if(strncmp(input, "MOUSE_WHEEL_UP", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE_WHEEL, (int)illInput::Axis::AX_Y_POS);
    }

    if(strncmp(input, "MOUSE_WHEEL_DOWN", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE_WHEEL, (int)illInput::Axis::AX_Y_NEG);
    }

    if(strncmp(input, "MOUSE_WHEEL_VERT", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE_WHEEL, (int)illInput::Axis::AX_Y);
    }

    if(strncmp(input, "MOUSE_WHEEL_LEFT", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE_WHEEL, (int)illInput::Axis::AX_X_NEG);
    }

    if(strncmp(input, "MOUSE_WHEEL_RIGHT", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE_WHEEL, (int)illInput::Axis::AX_X_POS);
    }

    if(strncmp(input, "MOUSE_WHEEL_HORZ", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE_WHEEL, (int)illInput::Axis::AX_X);
    }

    if(strncmp(input, "MOUSE_MOVE_UP", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE, (int)illInput::Axis::AX_Y_POS);
    }

    if(strncmp(input, "MOUSE_MOVE_DOWN", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE, (int)illInput::Axis::AX_Y_NEG);
    }

    if(strncmp(input, "MOUSE_MOVE_LEFT", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE, (int)illInput::Axis::AX_X_NEG);
    }

    if(strncmp(input, "MOUSE_MOVE_RIGHT", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE, (int)illInput::Axis::AX_X_POS);
    }

    if(strncmp(input, "MOUSE_MOVE_VERT", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE, (int)illInput::Axis::AX_Y);
    }

    if(strncmp(input, "MOUSE_MOVE_HORZ", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE, (int)illInput::Axis::AX_X);
    }

    if(strncmp(input, "MOUSE_LEFT", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE_BUTTON, 1);
    }

    if(strncmp(input, "MOUSE_RIGHT", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE_BUTTON, 3);
    }

    if(strncmp(input, "MOUSE_MIDDLE", 20) == 0) {
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE_BUTTON, 2);
    }

    //a mouse button
    if(strncmp(input, "MOUSE_", 8) == 1) {
        int button = atoi(input + 6);
        return illInput::InputBinding((int)SdlPc::InputType::PC_MOUSE_BUTTON, button);
    }

    return illInput::InputBinding((int)SdlPc::InputType::INVALID, 0);
}

std::string inputBindingToConsoleInput(const illInput::InputBinding& binding) {
    return "TODO";  //TODO
}