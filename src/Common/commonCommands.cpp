/***************************************************************************
* Vistools
* Matt Rasmussen
* commonCommands.cpp
*
***************************************************************************/

#include "first.h"
#include "commonCommands.h"

namespace Vistools
{

using namespace std;

CommandAttr g_globalAttr;

int CallProcCommand::procid = 0;


void commonCommandsInit()
{
    // global commands
    RegisterGlobalCommand(QuitCommand)
    //RegisterGlobalCommand(HelpCommand) Let python do the help
    RegisterGlobalCommand(ExecFileCommand)
    RegisterScriptCommand(CallProcCommand) AddAttr(g_globalAttr)
    RegisterGlobalCommand(VersionCommand)
}


StringCommand *GetCommand(vector<StringCommand*> &cmds, 
                               int argc, char **argv, int *consume, 
                               bool option, bool showError)
{
    if (argc == 0)
        return new StringCommand();

    // search for command in stringCommandRegistry
    for (vector<StringCommand*>::iterator j=cmds.begin(); 
         j!=cmds.end(); j++)
    {
        if ((option && !strcmp(argv[0], (*j)->GetOptionName())) ||
            (!option && !strcmp(argv[0], (*j)->GetName()))) {
            StringCommand *cmd = (StringCommand*) (*j)->Create();

            *consume = cmd->Setup(argc, (const char**) argv);
            if (*consume > 0) {
                return cmd;
            } else {
                // exit if bad syntax for argument
                delete cmd;
                Error("Error parsing argument '%s'", argv[0]);
                return NULL;
            }
        }
    }

    if (showError)
        Error("Unknown command '%s'", argv[0]);

    return NULL;
}


}

