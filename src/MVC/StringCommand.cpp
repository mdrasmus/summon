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

}

