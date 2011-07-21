/***************************************************************************
* Summon
* Matt Rasmussen
* Command.h
*
*
* This file is part of SUMMON.
* 
* SUMMON is free software: you can redistribute it and/or modify
* it under the terms of the Lesser GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
* 
* SUMMON is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
* 
* You should have received a copy of the Lesser GNU General Public License
* along with SUMMON.  If not, see <http://www.gnu.org/licenses/>.
* 
***************************************************************************/

#ifndef SUMMON_COMMAND_H
#define SUMMON_COMMAND_H

#include <set>
#include <vector>
#include "Factory.h"
#include "Input.h"

namespace Summon
{

typedef int CommandId;

enum {
    NULL_COMMAND = -1
};



// forward declaration
class CommandAttr;

class Command
{
public:
    Command() {}
    virtual ~Command() {}
    
    virtual Command* Create() { return new Command(); }
    virtual CommandId GetId() { return NULL_COMMAND; }
    virtual void Setup(Input &input) {}
    
    bool HasAttr(CommandAttr *attr);
};

class CommandExecutor {
public:
    virtual ~CommandExecutor() {}
    virtual void ExecCommand(Command &command) {}
};


// command factory and registration
typedef FactoryArray<CommandId, Command> CommandRegistry;

extern CommandRegistry g_commandRegistry;
extern Command *g_lastRegisteredCommand;

// command attributes
class CommandAttr
{
public:
    virtual ~CommandAttr()
    {}

    inline bool Has(CommandId cmdId) {
        return m_set.find(cmdId) != m_set.end();
    }
    
    inline bool Has(Command *cmd) {
        return m_set.find(cmd->GetId()) != m_set.end();
    }
    
    inline void Add(Command *cmd) {
        m_set.insert(cmd->GetId());
        m_cmds.push_back(cmd);
    }

    typedef vector<Command*>::iterator Iterator;
    
    inline Iterator Begin() { return m_cmds.begin(); }
    inline Iterator End() { return m_cmds.end(); }
    
protected:
    set<int> m_set;
    vector<Command*> m_cmds;
};

inline bool Command::HasAttr(CommandAttr *attr) { return attr->Has(GetId()); }



// macros
#define RegisterCommand(cmd) \
    static cmd cmd ## _c; \
    static CommandRegistry::Registrator \
        cmd ## _r(g_commandRegistry, &cmd ## _c, cmd ## _c.GetId()); \
    g_lastRegisteredCommand = &cmd ## _c;

#define AddAttr(attr) \
    attr.Add(g_lastRegisteredCommand);

}


#endif
