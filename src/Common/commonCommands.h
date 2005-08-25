/***************************************************************************
* Vistools
* Matt Rasmussen
* commonCommands.h
*
***************************************************************************/

#ifndef COMMON_COMMANDS_H
#define COMMON_COMMANDS_H

#include <set>
#include <vector>
#include "common.h"
#include "StringCommand.h"
#include "glut2DCommands.h"
#include "ScriptCommand.h"

namespace Vistools
{

using namespace std;

void commonCommandsInit();

StringCommand *GetCommand(vector<StringCommand*> &cmds, 
                               int argc, char **argv, int *consume, 
                               bool option = false, bool showError = true);

// global attribute
extern CommandAttr g_globalAttr;


#define RegisterGlobalCommand(cmd) \
    RegisterStringCommand(cmd) \
    AddAttr(g_globalAttr)


enum {
    COMMON_COMMANDS_BEGIN = GLUT_COMMANDS_END + 1,
    QUIT_COMMAND,
    HELP_COMMAND,
    EXEC_FILE_COMMAND,
    CALL_PROC_COMMAND,    
    VERSION_COMMAND,
    COMMON_COMMANDS_END
};



class QuitCommand : public StringCommand
{
public:
    virtual Command* Create() { return new QuitCommand(); }
    virtual int GetId() { return QUIT_COMMAND; }

    virtual const char *GetOptionName() { return ""; }
    virtual const char *GetName() { return "quit"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "quits program"; }
};


class HelpCommand : public StringCommand
{
public:
    virtual Command* Create() { return new HelpCommand(); }
    virtual int GetId() { return HELP_COMMAND; }

    virtual const char *GetOptionName() { return "-help"; }
    virtual const char *GetName() { return "get_help"; }
    virtual const char *GetUsage() { return "command"; }
    virtual const char *GetDescription() 
    { return "gives the usage for a command"; }
    
    
    virtual int Setup(int argc, const char **argv)
    {
        // note whether to give option or command line format
        isOption = (!strcmp(argv[0], GetOptionName()));
    
        if (argc >= 2) {
            command = string(argv[1]);
            return 2;
        } else if (argc == 1) {
            command = "";
            return 1;
        } else {
            Error("must specify a command for help");
            command = argv[0];
        }
        
        return -1;
    }
    
    bool isOption;
    string command;
};


class ExecFileCommand : public StringCommand
{
public:
    virtual Command* Create() { return new ExecFileCommand(); }
    virtual int GetId() { return EXEC_FILE_COMMAND; }

    virtual const char *GetOptionName() { return "-exec"; }
    virtual const char *GetName() { return ""; }
    virtual const char *GetUsage() { return "file"; }
    virtual const char *GetDescription() 
    { return "executes commands stored in a file"; }
    
    virtual int Setup(int argc, const char **argv)
    {
        if (argc >= 2) {
            filename = string(argv[1]);
            return 2;
        }
        
        return -1;
    }
    
    string filename;
};


class CallProcCommand : public ScriptCommand
{
public:
    CallProcCommand(Scm code = Scm_UNDEFINED) : defined(false)
    {
        if (code != Scm_UNDEFINED) {
            Setup(ScmCons(code, Scm_EOL));
        }
    }
    
    virtual ~CallProcCommand()
    {
        if (defined)  {
#           if (VISTOOLS_SCRIPT == VISTOOLS_SCHEME)       
                ScmDefine(name.c_str(), Scm_UNDEFINED);
#           endif
#           if (VISTOOLS_SCRIPT == VISTOOLS_PYTHON)
                ScmUngaurd(proc);
#           endif
        }
    }
    
    virtual Command* Create() { 
        if (!defined) {
            return new CallProcCommand();
        } else {
            Scm proc = GetScmProc();
            return new CallProcCommand(proc);
        }
    }
    virtual int GetId() { return CALL_PROC_COMMAND; }

    virtual const char *GetName() { return "call_proc"; }
    virtual const char *GetUsage() { return "proc"; }
    virtual const char *GetDescription() 
    { return "executes a procedure that takes no arguments"; }
    
    virtual bool Setup(Scm lst)
    {
        if (ScmProcedurep(ScmCar(lst))) {
            proc = ScmCar(lst);

#           if (VISTOOLS_SCRIPT == VISTOOLS_SCHEME)            
                size_t len;
                Scm sym = scm_gensym(gh_str02scm(" "));
                char *str = gh_symbol2newstr(sym, &len);
                name = string(str);
                scm_define(sym, proc);
#           endif
#           if (VISTOOLS_SCRIPT == VISTOOLS_PYTHON)
                ScmGaurd(proc);
#           endif
            
            defined = true;
            return true;            
        } else {
            Error("argument must be a procedure");
            return false;
        }
    }
    
    inline Scm GetScmProc()
    {
#       if (VISTOOLS_SCRIPT == VISTOOLS_SCHEME)   
            Scm var = ScmLookup(name.c_str());
            return ScmVariableRef(var);
#       endif
#       if (VISTOOLS_SCRIPT == VISTOOLS_PYTHON)   
            return proc;
#       endif
    }
    
    Scm proc;
    bool defined;
    string name;
    static int procid;
};

class VersionCommand : public StringCommand
{
public:
    virtual Command* Create() { return new VersionCommand(); }
    virtual int GetId() { return VERSION_COMMAND; }

    virtual const char *GetOptionName() { return "-v"; }
    virtual const char *GetName() { return "version"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "prints the current version"; }
};


}


#endif
