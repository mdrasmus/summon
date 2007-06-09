/***************************************************************************
* Summon
* Matt Rasmussen
* StringCommand.cpp
*
***************************************************************************/

#include "first.h"
#include "StringCommand.h"
#include "common.h"

namespace Summon
{

using namespace std;

CommandAttr g_stringAttr;

vector<StringCommand*> g_usageList;

vector<StringCommand*> GetAllStringCommands()
{
    vector<StringCommand*> cmds;
   
    for (CommandAttr::Iterator i=g_stringAttr.Begin();
         i != g_stringAttr.End(); i++)
    {
        cmds.push_back((StringCommand*) *i);
    }
    
    return cmds;
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

