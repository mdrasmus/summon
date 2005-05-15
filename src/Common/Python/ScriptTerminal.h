/***************************************************************************
* Vistools
* Matt Rasmussen
* ScriptTerminal.h
*
***************************************************************************/

#ifndef SCRIPT_TERMINAL_H
#define SCRIPT_TERMINAL_H

#include <readline/readline.h>
#include <readline/history.h>

#include "Terminal.h"


namespace Vistools {

using namespace std;

class ScriptTerminal : public Terminal
{
public:
    /*
    static char **AttemptComplete(const char *text, int start, int end) {
        char **matches = NULL;
    
        // if first word in list then try to complete it as a command
        if (start > 0 && rl_line_buffer[start-1] == '(')
            matches = rl_completion_matches(text, ReadlineCompleteWord);

        return matches;    
    }

    virtual void SetupComplete()
    {
        // tell readline to use our word completion
        rl_attempted_completion_function = ScriptTerminal::AttemptComplete;    
    }
    */
};

}

#endif

