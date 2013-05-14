#ifndef ILL_CONSOLE_VARS_H_
#define ILL_CONSOLE_VARS_H_

#include "illEngine/Console/serial/VariableManager.h"
#include "illEngine/Console/serial/CommandManager.h"

extern const char * CON_VISIBLE_NAME;
extern const char * CON_VISIBLE_DESC;
extern illConsole::ConsoleVariable cv_con_visible;

extern const char * CON_OUTPUT_NAME;
extern const char * CON_OUTPUT_DESC;
extern illConsole::ConsoleVariable cv_con_output;

extern const char * CON_MAX_LINES_NAME;
extern const char * CON_MAX_LINES_DESC;
extern illConsole::ConsoleVariable cv_con_maxLines;

extern const char * CON_LOG_SCREEN_NAME;
extern const char * CON_LOG_SCREEN_DESC;
extern illConsole::ConsoleVariable cv_con_logScreen;

/**
The command for setting a console variable.  It's up to the callback to handle invalid values
and set the variable back to the proper value using setValueNoCallback().

@param varName
@param varValue
*/
extern const char * SET_NAME;
extern const char * SET_DESC;
extern illConsole::ConsoleCommand cm_set;

/**
The command for saving a console variable to the settings file.

@param varName
*/
extern const char * SAVE_NAME;
extern const char * SAVE_DESC;
extern illConsole::ConsoleCommand cm_save;

extern const char * DESCRIPTION_NAME;
extern const char * HELP_NAME;
extern const char * DESCRIPTION_DESC;
extern illConsole::ConsoleCommand cm_description;

/**
The command for toggling the boolean value of a console variable.
This basically means a variable set to anything other than 0 is now set to 0, and 0 is set to 1.
This may be completely useless depending on the variable but nothing stops you from calling it
just like nothing would stop you from setting a variable to an invalid value anyway.

@param varName
*/
extern const char * TOGGLE_NAME;
extern const char * TOGGLE_DESC;
extern illConsole::ConsoleCommand cm_toggle;

extern const char * CON_DUMP_NAME;
extern const char * CON_DUMP_DESC;
extern illConsole::ConsoleCommand cm_conDump;

extern const char * EXEC_NAME;
extern const char * EXEC_DESC;
extern illConsole::ConsoleCommand cm_exec;

extern const char * CLEAR_NAME;
extern const char * CLEAR_DESC;
extern illConsole::ConsoleCommand cm_clear;

extern const char * ECHO_NAME;
extern const char * ECHO_DESC;
extern illConsole::ConsoleCommand cm_echo;

#endif