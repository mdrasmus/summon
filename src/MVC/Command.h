/***************************************************************************
* Vistools
* Matt Rasmussen
* Command.h
*
***************************************************************************/

#ifndef COMMAND_H
#define COMMAND_H

#include <set>
#include <vector>
#include "Factory.h"
#include "Input.h"

namespace Vistools
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
typedef Factory<CommandId, Command> CommandRegistry;

extern CommandRegistry g_commandRegistry;
extern Command *g_lastRegisteredCommand;

// command attributes
class CommandAttr
{
public:
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
