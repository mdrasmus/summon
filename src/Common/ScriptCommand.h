/***************************************************************************
* Vistools
* Matt Rasmussen
* ScriptCommand.h
*
***************************************************************************/

#ifndef SCRIPT_COMMAND_H
#define SCRIPT_COMMAND_H

#include <set>
#include "StringCommand.h"
#include "Script.h"

namespace Vistools
{


class ScriptCommand : public StringCommand
{
public:
    ScriptCommand() : m_return(Scm_UNSPECIFIED) {}
    virtual ~ScriptCommand() {}

    virtual bool Setup(Scm lst) { return true; }
    
    inline void SetReturn(Scm scm) { m_return = scm; }
    inline Scm GetReturn() { return m_return; }
    
protected:
    Scm m_return;
};


bool IsScriptCommand(int cmdId);
extern set<int> g_scriptCommands;

#define RegisterScriptCommand(cmd) \
    RegisterStringCommand(cmd) \
    g_scriptCommands.insert(cmd ## _c.GetId());

}


#endif
