#ifndef ILL_INPUT_VARS_H_
#define ILL_INPUT_VARS_H_

#include "illEngine/Console/serial/VariableManager.h"
#include "illEngine/Console/serial/CommandManager.h"
#include "illEngine/Input/serial/InputBinding.h"

illInput::InputBinding consoleInputToBinding(const char * input);
std::string inputBindingToConsoleInput(const illInput::InputBinding& binding);

extern const char * BIND_DESC;
extern const char * BIND_NAME;
extern illConsole::ConsoleCommand cm_bind;

extern const char * UNBIND_DESC;
extern const char * UNBIND_NAME;
extern illConsole::ConsoleCommand cm_unbind;

extern const char * SAVE_BIND_DESC;
extern const char * SAVE_BIND_NAME;
extern illConsole::ConsoleCommand cm_saveBind;

extern const char * PRINT_INPUT_BINDS_DESC;
extern const char * PRINT_INPUT_BINDS_NAME;
extern illConsole::ConsoleCommand cm_printInputBinds;

#endif