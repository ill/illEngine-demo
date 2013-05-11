#ifndef ILL_CONSOLE_VARS_H_
#define ILL_CONSOLE_VARS_H_

#include "illEngine/Console/serial/VariableManager.h"
#include "illEngine/Console/serial/CommandManager.h"

const char * CON_CONSOLE_OUTPUT_NAME = "con_consoleOutput";
const char * CON_CONSOLE_OUTPUT_DESC = "If set, the console will print all output to a file in real time.";
extern illConsole::ConsoleVariable cv_con_consoleOutput;

const char * CON_MAX_LINES_NAME = "con_maxLines";
const char * CON_MAX_LINES_DESC = "The maximum number of lines the console will remember at a time. "
    "It may not necessarily have room to display all of these lines, "
    "but dumping the console to a file with conDump will dump all of the lines that are remembered.";
extern illConsole::ConsoleVariable cv_con_maxLines;

const char * CON_LOG_SCREEN_NAME = "con_logScreen";
const char * CON_LOG_SCREEN_DESC = "If set, all messages printed to the console will be displayed on the screen as well "
    "even when the console is closed.";
extern illConsole::ConsoleVariable cv_con_logScreen;

/**
The command for setting a console variable.  It's up to the callback to handle invalid values
and set the variable back to the proper value using setValueNoCallback().

@param varName
@param varValue
*/
const char * SET_NAME = "set";
const char * SET_DESC = "Sets a console variable. "
    "set <variable name> <value>";
extern illConsole::ConsoleCommand cm_set;

/**
The command for saving a console variable to the settings file.

@param varName
*/
const char * SAVE_NAME = "save";
const char * SAVE_DESC = "Saves a console variable permanently. "
    "save <variable name>";
extern illConsole::ConsoleCommand cm_save;

const char * DESCRIPTION_NAME = "description";
const char * HELP_NAME = "help";
const char * DESCRIPTION_DESC = "Prints the description for a variable or command. "
    "description <variable name/command name>";
extern illConsole::ConsoleCommand cm_description;

/**
The command for toggling the boolean value of a console variable.
This basically means a variable set to anything other than 0 is now set to 0, and 0 is set to 1.
This may be completely useless depending on the variable but nothing stops you from calling it
just like nothing would stop you from setting a variable to an invalid value anyway.

@param varName
*/
const char * TOGGLE_NAME = "toggle";
const char * TOGGLE_DESC = "The command for toggling the boolean value of a console variable. "
    "This basically means a variable set to anything other than 0 is now set to 0, and 0 is set to 1. "
    "This may be completely useless depending on the variable. "
    "toggle <variable name>";
extern illConsole::ConsoleCommand cm_toggle;

const char * CON_DUMP_NAME = "conDump";
const char * CON_DUMP_DESC = "The command for dumping all remembered lines in the developer console to a file. "
    "conDump <file name>";
extern illConsole::ConsoleCommand cm_conDump;

const char * EXEC_NAME = "exec";
const char * EXEC_DESC = "Executes the commands in a file. "
    "exec <file name>";
extern illConsole::ConsoleCommand cm_exec;

const char * CLEAR_NAME = "clear";
const char * CLEAR_DESC = "Clears all lines in the developer console.";
extern illConsole::ConsoleCommand cm_clear;

const char * ECHO_NAME = "echo";
const char * ECHO_DESC = "Prints a message to the console. echo <some message>";
extern illConsole::ConsoleCommand cm_echo;

#endif