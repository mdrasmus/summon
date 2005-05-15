/***************************************************************************
* Vistools
* Matt Rasmussen
* ScriptApp.h
*
***************************************************************************/

#ifndef SCRIPT_APP_H
#define SCRIPT_APP_H


#include "App.h"
#include "ScriptEngine.h"


namespace Vistools {

using namespace std;

class ScriptApp : public App
{
public:
    ScriptApp(const char *name = "app") :
        App(name),
        m_initPrompt(true)
    {}

    virtual void ProcessLine(string line)
    {
        if (m_initPrompt) {
            m_prompt = GetTerminal()->GetPrompt();
            m_initPrompt = false;
        }
    
        switch (m_engine.EngineRepl(line)) {
            case ScriptEngine::STATUS_READY:
                GetTerminal()->SetPrompt(m_prompt);
                break;
            case ScriptEngine::STATUS_CONTINUE:
                GetTerminal()->SetPrompt("");
                break;
        }
    }
    
    inline ScriptEngine *GetEngine()
    { return &m_engine; }

protected:
    ScriptEngine m_engine;
    bool m_initPrompt;
    string m_prompt;
};




}

#endif


