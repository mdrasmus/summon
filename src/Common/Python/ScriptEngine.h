#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include "ScriptCommand.h"
#include "Script.h"

namespace Vistools {

using namespace std;



class ScriptEngine
{
public:
    ScriptEngine() : 
        m_cont(false),
        m_multiline(false),
        m_listener(NULL), 
        m_moduleName(string("vistools"))
    {}

    virtual void Init();
    virtual int EngineRepl(string line);
    
    virtual CommandExecutor *GetListener() { return m_listener; }
    virtual void SetListener(CommandExecutor *listener)
    { m_listener = listener; }
    
    virtual void SetCommands(vector<StringCommand*> &cmds)
    { m_commands = cmds; }

    enum {
        STATUS_READY,
        STATUS_CONTINUE
    };

    inline void SetModuleName(string name)
    { m_moduleName = name; }
    
protected:
    string m_line;
    bool m_cont;    
    bool m_multiline;

    CommandExecutor *m_listener;
    string m_moduleName;    
    vector<StringCommand*> m_commands;
};


}

#endif

