/***************************************************************************
* Vistools
* Matt Rasmussen
* App.h
*
***************************************************************************/

#ifndef APP_H
#define APP_H

#include <vector>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>
#include <time.h>
#include "Command.h"
#include "commonCommands.h"
#include "Terminal.h"



namespace Vistools {

using namespace std;

class App;

void SetApp(App *app);
App *GetApp();
void Shutdown(int code);
void Timer(int value);


class App : public CommandExecutor, public TerminalListener
{
public:
    App(const char *name = "app");
    virtual ~App();
    
    
    virtual void Usage();
    virtual void ExecCommand(Command &command);
    virtual void TerminalCallback(string line);
    virtual void TerminalQuit();
    virtual bool ExecFile(string filename);
    virtual int ParseArgs(int argc, char **argv);        
    virtual void ProcessLine(string line);
    virtual int Begin(int argc, char **argv) { return 0; }

    
    void Quit();
    void Help(HelpCommand *help);
    
    
    void SetupCommands(vector<StringCommand*> &commands);
    void SetUsage(string usage)
    { m_usage = usage; }
    
    void SetTerminal(Terminal *term);
    inline Terminal *GetTerminal()
    { return m_term; }
    void StartTerminal();        

    void ParseLine(char *line, int *argc, char ***argv);

    
    // synchronization and thread related
    inline SDL_Thread *GetTermThread()
    { return m_termThread; }
    
    inline void Lock()
    {
        assert(SDL_mutexP(m_lock) == 0);
    }
    
    inline void Unlock()
    {
        assert(SDL_mutexV(m_lock) == 0);
    }
    
    inline void WaitForExec()
    {
        assert(SDL_mutexP(m_condlock) == 0);
        assert(SDL_CondWait(m_cond, m_condlock) == 0);
    }
    
    inline void NotifyExecOccurred()
    {
        assert(SDL_CondBroadcast(m_cond) == 0);
    }

    
    // command management between threads
    inline void SetNextLine(string line)
    {
        m_nextLine = line;
        m_terminalReady = true;
    }

    inline string GetNextLine()
    {
        m_terminalReady = false;
        return m_nextLine;
    }
    
    inline bool IsTerminalReady()
    { return m_terminalReady; }

    inline void SetTerminalStatus(int status)
    { m_terminalStatus = status; }
    
    inline int GetTerminalStatus()
    { return m_terminalStatus; }
    
    inline float GetTime()
    { return clock() / float(CLOCKS_PER_SEC) + m_timerOffset; }
    
    inline void SetTimerCommand(float delay, Command *command)
    {
        m_timerDelay = GetTime() + delay;
        m_timerCommand = command;
    }

    
    float m_timerOffset;
    float m_timerDelay;
    Command *m_timerCommand;

protected:    
    string m_appName;
    string m_usage;
    vector<StringCommand*> m_stringCmds;
    SDL_mutex *m_lock;
    SDL_mutex *m_condlock;
    SDL_cond *m_cond;
    Terminal *m_term;
    SDL_Thread *m_termThread;


private:
    bool m_terminalReady;    
    string m_nextLine;
    int m_terminalStatus;
};




}

#endif


