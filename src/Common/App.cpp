/***************************************************************************
* Vistools
* Matt Rasmussen
* App.cpp
*
***************************************************************************/

#include "first.h"
#include <stdio.h>
#include <GL/glut.h>
#include "App.h"
#include "commonCommands.h"



namespace Vistools {

using namespace std;


class App;
static App *g_app = NULL;


App::App(const char *name) :
    m_appName(name),
    m_lock(SDL_CreateMutex()),
    m_cond(SDL_CreateCond()),
    m_condlock(SDL_CreateMutex()),
    m_termThread(NULL),
    m_terminalReady(false),
    m_timerCommand(NULL),
    m_timerOffset(0)
{        
}

App::~App()
{
    SDL_DestroyMutex(m_lock);
    SDL_DestroyCond(m_cond);
    SDL_DestroyMutex(m_condlock);
}


void App::SetTerminal(Terminal *term)
{
    m_term = term;
    m_term->SetListener(this);
}
    
    
void App::Usage()
{
    fprintf(stderr, "usage: %s %s\noptions:\n", m_appName.c_str(),
            m_usage.c_str());

    int maxlen = 0;
    int spaces = 6;

    // find maximum length usage
    for (vector<StringCommand*>::iterator j=g_usageList.begin(); 
         j!=g_usageList.end(); j++)
    {
        if (!strcmp((*j)->GetOptionName(), ""))
            continue;

        int len = strlen((*j)->GetOptionName()) + 
                  strlen((*j)->GetUsage()) + spaces;

        if (len > maxlen)
            maxlen = len;
    }

    // print usage
    for (vector<StringCommand*>::iterator j=g_usageList.begin(); 
         j!=g_usageList.end(); j++)
    {
        if ((*j)->GetId() == COMMENT_COMMAND) {
            fprintf(stderr, (*j)->GetComment());
        }

        if (!strcmp((*j)->GetOptionName(), ""))
            continue;

        fprintf(stderr, "  %s %s   ",
                (*j)->GetOptionName(),
                (*j)->GetUsage());

        int len = strlen((*j)->GetOptionName()) +
                  strlen((*j)->GetUsage()) + spaces;
        for (int i=0; i<maxlen - len; i++)
            fprintf(stderr, " ");

        fprintf(stderr, "%s\n", (*j)->GetDescription());
    }
}
    
    
void App::ExecCommand(Command &command) {
    switch (command.GetId()) {
        case EXEC_FILE_COMMAND:
            ExecFile(((ExecFileCommand*)&command)->filename);
            break;

        case QUIT_COMMAND:
            Quit();
            break;

        case HELP_COMMAND:
            Help((HelpCommand*) &command);
            break;            
    }
}
    
    
bool App::ExecFile(string filename)
{
    FILE *file;

    if ((file = fopen(filename.c_str(), "r")) == NULL) {
        Error("Cannot open file '%s'", filename.c_str());
        return false;
    }

    char *line = new char [MAX_LINE];

    int lineno = 1;
    while (fgets(line, MAX_LINE, file)) {
        int argc, consume;
        char **argv;

        Chomp(line);

        ParseLine(line, &argc, &argv);
        Command *cmd = GetCommand(m_stringCmds, argc, argv, &consume, false);

        if (cmd) {
            ExecCommand(*cmd);
            delete cmd;
        } else {
            Error("Error in file '%s' on line %d", filename.c_str(), lineno);
            break;
        }

        lineno++;
    }

    // clean up
    delete [] line;
    fclose(file);

    return true;
}
    
    
void App::Quit()
{
    printf("quiting...\n");

    Shutdown(0);
}

    
void App::Help(HelpCommand *help)
{
    StringCommand* cmd = NULL;

    if (help->isOption) {
        Usage();
        exit(1);
    }

    // default message
    if (help->command == "") {
        for (vector<StringCommand*>::iterator j=m_stringCmds.begin(); 
             j!=m_stringCmds.end(); j++)
        {
            const char *str = help->isOption ?
                                (*j)->GetOptionName() :
                                (*j)->GetName();

            if (strcmp(str, "")) {
                printf("  %s\n", str);
            }
        }

        
        printf("use help <command> to get help on a command\n");        

    } else {
        // find command
        for (vector<StringCommand*>::iterator j=m_stringCmds.begin(); 
                 j!=m_stringCmds.end(); j++)
        {
            const char *str = help->isOption ?
                            (*j)->GetOptionName() :
                            (*j)->GetName();

            if (!strcmp(str, help->command.c_str()))
            {
                cmd = *j;
                break;
            }
        }

        // print command                
        if (cmd) {
            printf("command usage: ");

            if (help->isOption)
                printf(cmd->GetOptionName());
            else
                printf(cmd->GetName());

            printf(" %s   %s\n", cmd->GetUsage(),
                                 cmd->GetDescription());
        } else {
            printf("command '%s' unknown\n", help->command.c_str());
        }
    }
}

    
    
int App::ParseArgs(int argc, char **argv)
{
    // execute args
    for (int i=1; i<argc;) {
        int consume;
        Command *cmd = GetCommand(m_stringCmds, argc-i, argv+i, &consume, true);

        if (cmd) {
            // execute the argument
            ExecCommand(*cmd);
            i += consume;
            delete cmd;
        } else {
            i++;
        }
    }
    return 0;
}


void App::ParseLine(char *line, int *argc, char ***argv)
{
    char *delim = " \t";

    // parse line into tokens
    vector<char*> tokens = Split(line, delim);
    *argc = tokens.size();
    *argv = new char* [*argc];

    for (int i=0; i<*argc; i++) {
        (*argv)[i] = tokens[i];
    }
}


void App::ProcessLine(string line)
{
    int argc, consume;
    char **argv;
    char *str = strdup(line.c_str());
    ParseLine(str, &argc, &argv);
    
    Command *cmd = GetCommand(m_stringCmds, argc, argv, &consume, true);
    if (cmd) {
        ExecCommand(*cmd);
    }
    
    delete [] argv;
    free(str);
}
    
    
// note: this function will be called in another thread
// it should not modify any state unless it attains the Lock()
void App::TerminalCallback(string line)
{
    // execute the command in another thread
    Lock();
    SetNextLine(line);
    Unlock();

    // pause until command is executed
    WaitForExec();
}

void App::TerminalQuit()
{
    Shutdown(0);
}
    
    
void App::SetupCommands(vector<StringCommand*> &commands)
{
    // setup commands
    m_stringCmds = commands;
    for (int i=0; i<m_stringCmds.size(); i++) {
        if (m_stringCmds[i]->GetName() != "") {
            m_term->AddCommand(m_stringCmds[i]);
            m_term->AddToWordCompletion(m_stringCmds[i]->GetName());
        }
    }
}

    
void App::StartTerminal()
{
    // setup timer
    glutTimerFunc(10, Timer, 0);

    // start terminal thread
    m_termThread = SDL_CreateThread(Terminal::TerminalThread, (void*) m_term);
}


void Shutdown(int code)
{
    // kill the terminal forcefully
    if (g_app->GetTermThread()) {
        if (SDL_ThreadID() != SDL_GetThreadID(g_app->GetTermThread()))
            SDL_KillThread(g_app->GetTermThread());
        g_app->GetTerminal()->TerminalCleanup();
    }
    
    // delete the app
    delete g_app;
    
    // quit with code
    exit(code);
}


void Timer(int value)
{
    // exec next command if available
    if (g_app->IsTerminalReady()) {
        g_app->Lock();
        
        g_app->ProcessLine(g_app->GetNextLine());
        g_app->NotifyExecOccurred();
        
        g_app->Unlock();
        
        // immediately try to execute next instruction
        glutTimerFunc(0, Timer, 0);
    } else {
        
        // poll slowly since nothing is happening
        glutTimerFunc(10, Timer, 0);
        g_app->m_timerOffset += .010;
    }
    
    // look at timer
    if (g_app->m_timerCommand && g_app->GetTime() > g_app->m_timerDelay) {
        Command *cmd = g_app->m_timerCommand;
        g_app->m_timerCommand = NULL;
        g_app->ExecCommand(*cmd);
        delete cmd;
    }
}

void SetApp(App *app)
{
    g_app = app;
}

App *GetApp()
{
    return g_app;
}

}



