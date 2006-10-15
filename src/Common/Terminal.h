/***************************************************************************
* Vistools
* Matt Rasmussen
* Terminal.h
*
***************************************************************************/


#ifndef TERMINAL_H
#define TERMINAL_H


#include "StringCommand.h"

#include <string>
#include <vector>

namespace Vistools {

using namespace std;


char *ReadlineCompleteWord(const char *text, int state);


class TerminalListener {
public:
    virtual void TerminalCallback(string str) {}
    virtual void TerminalQuit() {}
};

class Terminal
{
public:
    Terminal();
    
    static int TerminalThread(void *term);

    virtual int Begin();
    virtual void TerminalCleanup();
    virtual void AddToWordCompletion(const char *word);
    virtual void SetupComplete();

    inline void SetListener(TerminalListener *listener)
    { m_listener = listener; }    
    inline TerminalListener *GetListener()
    { return m_listener; }
    
    inline void SetPrompt(string prompt)
    { m_prompt = prompt; }
    inline string GetPrompt()
    { return m_prompt; }
    
    

    inline void SetTerminalRunning(bool running)
    { m_termRunning = running; }
    inline bool GetTerminalRunning()
    { return m_termRunning; } 
    
    inline vector<string> GetWords()
    { return m_words; }
    
    inline void AddCommand(StringCommand *command)
    { m_commands.push_back(command); }
    
    inline vector<StringCommand*> GetCommands()
    { return m_commands; }
    

    virtual char *CompleteWord(const char *text, int state);
    static char **AttemptComplete(const char *text, int start, int end);


protected:
    TerminalListener *m_listener;
    bool m_termRunning;
    vector<string> m_words;
    vector<StringCommand*> m_commands;
    string m_prompt;
};


Terminal *GetTerminal();

}

#endif

