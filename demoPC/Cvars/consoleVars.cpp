#include "consoleVars.h"

const char * CON_VISIBLE_NAME = "con_visible";
const char * CON_VISIBLE_DESC = "Whether or not the console is visible or hidden.";

const char * CON_OUTPUT_NAME = "con_output";
const char * CON_OUTPUT_DESC = "If set, the console will print all output to a file in real time.";

const char * CON_MAX_LINES_NAME = "con_maxLines";
const char * CON_MAX_LINES_DESC = "The maximum number of lines the console will remember at a time. "
    "It may not necessarily have room to display all of these lines, "
    "but dumping the console to a file with conDump will dump all of the lines that are remembered.";

const char * CON_LOG_SCREEN_NAME = "con_logScreen";
const char * CON_LOG_SCREEN_DESC = "If set, all messages printed to the console will be displayed on the screen as well "
    "even when the console is closed.";

const char * SET_NAME = "set";
const char * SET_DESC = "Sets a console variable. "
    "set <variable name> <value>";

const char * SAVE_NAME = "save";
const char * SAVE_DESC = "Saves a console variable permanently. "
    "save <variable name>";

const char * DESCRIPTION_NAME = "description";
const char * HELP_NAME = "help";
const char * DESCRIPTION_DESC = "Prints the description of a variable or command. Inception bro. "
    "description <command or variable name>";

const char * TOGGLE_NAME = "toggle";
const char * TOGGLE_DESC = "The command for toggling the boolean value of a console variable. "
    "This basically means a variable set to anything other than 0 is now set to 0, and 0 is set to 1. "
    "This may be completely useless depending on the variable. "
    "toggle <variable name>";

const char * CON_DUMP_NAME = "conDump";
const char * CON_DUMP_DESC = "The command for dumping all remembered lines in the developer console to a file. "
    "conDump <file name>";

const char * EXEC_NAME = "exec";
const char * EXEC_DESC = "Executes the commands in a file. "
    "exec <file name>";

const char * CLEAR_NAME = "clear";
const char * CLEAR_DESC = "Clears all lines in the developer console.";

const char * ECHO_NAME = "echo";
const char * ECHO_DESC = "Prints a message to the console. echo <some message>";