/***************************************************************************
* Vistools
* Matt Rasmussen
* Terminal.cpp
*
***************************************************************************/

#include "first.h"
#include <algorithm>
#include <vector>
#include <string>
#include "common.h"
#include "Terminal.h"

#include <readline/readline.h>
#include <readline/history.h>

namespace Vistools {

using namespace std;

Terminal *g_term = NULL;


Terminal *GetTerminal()
{
    return g_term;
}


Terminal::Terminal() :
    m_listener(NULL),
    m_termRunning(true),
    m_prompt("> ")
{
    g_term = this;
}


void Terminal::TerminalCleanup()
{
    // tell terminal thread to restore terminal to original state
    rl_free_line_state();
    rl_cleanup_after_signal();
}

void Terminal::AddToWordCompletion(const char *word)
{
    if (strcmp(word, ""))
        m_words.push_back(word);
}


char *Terminal::CompleteWord(const char *text, int state)
{
    static int cmd = 0;
    int len = strlen(text);
    string word(text);
    
    // reset cmd index when given new word
    if (state == 0) {
        cmd = 0;
    }
    
    // search for next matching command
    for (; cmd < m_words.size() && 
           m_words[cmd].substr(0, len) != word; cmd++);
    
    // if command found return it, otherwise return null    
    if (cmd < m_words.size()) {
        cmd++;
        return strdup(m_words[cmd-1].c_str());
    } else {
        return NULL;
    }
}


char *ReadlineCompleteWord(const char *text, int state)
{
    g_term->CompleteWord(text, state);
}


char **Terminal::AttemptComplete(const char *text, int start, int end)
{
    char **matches = NULL;

    // if first word in text then try to complete it as a command
    if (start == 0)
        matches = rl_completion_matches(text, ReadlineCompleteWord);

    return matches;
}


int Terminal::TerminalThread(void *_term)
{
    Terminal *term = (Terminal*) _term;
    return term->Begin();
}


void Terminal::SetupComplete()
{
    // tell readline to use our word completion
    rl_attempted_completion_function = Terminal::AttemptComplete;    
}


int Terminal::Begin()
{
    SetupComplete();
    
    // begin terminal reading loop
    while (m_termRunning) {
        char *line = readline(m_prompt.c_str());
        
        // quit if end of file
        if (line == NULL) {
            if (m_listener) {
                m_listener->TerminalQuit();
            }
        }
        
        // add line to history if it is not blank
        if (*line)
            add_history(line);
        
        if (m_termRunning) {
            if (m_listener) {
                m_listener->TerminalCallback(string(line));
            }
        }
        
        free(line);
    }
    
    return 0;
}

}
