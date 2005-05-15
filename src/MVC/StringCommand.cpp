/***************************************************************************
* Vistools
* Matt Rasmussen
* StringCommand.cpp
*
***************************************************************************/

#include "first.h"
#include "StringCommand.h"


namespace Vistools
{

using namespace std;

set<int> g_stringCommands;
vector<StringCommand*> g_usageList;


bool IsStringCommand(int cmdId)
{
    return g_stringCommands.find(cmdId) != g_stringCommands.end();
}

vector<StringCommand*> GetAllStringCommands()
{
    vector<StringCommand*> cmds;
    typedef map<CommandId, Command*> Map;
    
    Map commands = g_commandRegistry.GetProducts();
    
    for (Map::iterator i=commands.begin(); i!=commands.end(); i++) {
        if (IsStringCommand((*i).first)) {
            cmds.push_back((StringCommand*) ((*i).second));
        }
    }
    
    return cmds;
}

}

