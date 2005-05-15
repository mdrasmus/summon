#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include "ScriptCommand.h"
#include "Script.h"

namespace Vistools {

using namespace std;

ScriptCommand *GetCommand(SCM lst, bool showError = true);


class ScriptEngine
{
public:
    ScriptEngine() : m_listener(NULL) {}

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

    string m_sexp;
    
protected:
    virtual void InstallGuileFunctions();

    CommandExecutor *m_listener;
    vector<StringCommand*> m_commands;
};


}

#endif

