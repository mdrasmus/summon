/***************************************************************************
* Vistools
* Matt Rasmussen
* ScriptCommand.cpp
*
***************************************************************************/

#include "first.h"
#include "ScriptCommand.h"


namespace Vistools
{

set<int> g_scriptCommands;


bool IsScriptCommand(int cmdId)
{
    return g_scriptCommands.find(cmdId) != g_scriptCommands.end();
}


}

