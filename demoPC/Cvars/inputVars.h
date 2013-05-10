#ifndef ILL_INPUT_VARS_H_
#define ILL_INPUT_VARS_H_

#include "illEngine/Console/serial/VariableManager.h"
#include "illEngine/Console/serial/CommandManager.h"

/**
Binds an input to a command name.
A command name may then be put in an input context later.
You can have the same input bound to multiple commands since
you may have multiple contexts when that key can trigger an action.

Example usage bind UP_ARROW Forward

@param input
@param command string
*/
extern illConsole::ConsoleCommand cm_bind;

/**
Unbinds an input from a command.

@param input
@param command string
*/
extern illConsole::ConsoleCommand cm_unbind;

/**
Saves an input to permanent setting storage.
*/
extern illConsole::ConsoleCommand cm_saveBind;

/**
Prints bindings to an input.
*/
extern illConsole::ConsoleCommand cm_printInputBinds;

#endif