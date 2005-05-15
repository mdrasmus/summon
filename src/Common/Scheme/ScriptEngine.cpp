#include <algorithm>
#include <vector>
#include <string>
#include "common.h"
#include "commonCommands.h"
#include "ScriptEngine.h"


#include <readline/readline.h>
#include <readline/history.h>
#include <libguile.h>
#include <guile/gh.h>

namespace Vistools {

static vector<StringCommand*> g_terminalCmds;

using namespace std;


static ScriptEngine *g_engine = NULL;


int GetDepth(const char *line)
{
    int len = strlen(line);
    int depth = 0;
    
    for (int i=0; i<len; i++) {
        // count parens
        switch (line[i]) {
        
            // count parens
            case '(':
                depth++;
                break;
            case ')': 
                depth--;
                break;
                
            // skip strings
            case '"': 
                for (i++; i<len && line[i] != '"'; i++) {        
                    // escape sequence, skip next char
                    if (line[i] == '\\')
                        i++;
                }
                break;
                
            // skip character literals
            case '\\':
                if (line[++i] == '#')
                    i++;
        }
    }
    
    return depth;
}



static SCM ExecFunc(SCM vars)
{
    ScriptEngine *engine = g_engine;
    
    // verify the terminal has a listener
    if (!engine->GetListener())
        return SCM_UNSPECIFIED;
    
    
    
    if (IsScriptCommand(gh_scm2int(SCM_CAR(vars))))
    {
        // treat command as a s-expression
        
        ScriptCommand *cmd = GetCommand(vars);
                
        if (cmd) {
            engine->GetListener()->ExecCommand(*cmd);
            SCM ret = Scm2SCM(cmd->GetReturn());
            delete cmd;
            return ret;
        }

    } else {
        // treat command as just a list of strings
        
        vector<string> args = Scheme2Strings(vars);

        // convert strings to char*
        int argc = args.size();
        char **argv = new char* [argc];    
        for (int i=0; i<argc; i++)
            argv[i] = (char*) args[i].c_str(); // NOTE: type loophole

        
        int consume;
        StringCommand *cmd = GetCommand(g_terminalCmds, argc-1, argv+1, 
                                        &consume, false, true);

        if (cmd) {
            engine->GetListener()->ExecCommand(*cmd);
            delete cmd;
        }

        delete [] argv;
    }
    
    return SCM_UNSPECIFIED;
}


static SCM Exec(SCM scm)
{
    // execute the input string in an error handling environment
    return gh_eval_str(g_engine->m_sexp.c_str());
}


int ScriptEngine::EngineRepl(string line)
{
    // include a newline if this is a multi-line sexp
    if (m_sexp.size() > 0)
        m_sexp += "\n";

    // extend existing buffered text
    m_sexp += line;

    // determine if s-expression is complete
    int depth = GetDepth(m_sexp.c_str());
    if (depth > 0)
        return STATUS_CONTINUE;
    
    if (depth < 0) {
        printf("ERROR: Unexpected ')'\n");
        m_sexp = "";
        return STATUS_READY;
    }
    

    // add line to history if it is not blank
    if (m_sexp.size() > 0) {
        // eval s-expression
        SCM result = gh_eval_str("(terminal::env)");
        if (!gh_equal_p(result, SCM_UNSPECIFIED)) {
            SCM output = scm_current_output_port();        
            scm_display(result, output);
            printf("\n");
        }
    }
    
    // clear text buffer
    m_sexp = "";
    
    return STATUS_READY;
}


void ScriptEngine::InstallGuileFunctions()
{
    g_terminalCmds = m_commands;
    g_engine = this;
    
    scm_c_define_gsubr("terminal::exec-in-env", 0, 0, 0, (SCM (*)()) Exec);
    
    // install commands
    for (int i=0; i<m_commands.size(); i++) {
        // get command id
        enum { STR_SIZE = 10 };
        char *idstr = new char [STR_SIZE + 1];
        snprintf(idstr, STR_SIZE, "%d", m_commands[i]->GetId());
        
        // create scheme name for command
        string name = m_commands[i]->GetName();
        string func = string("terminal::") + name;
        
        // install shortcut name for command
        string cmd;
        
        if (IsScriptCommand(m_commands[i]->GetId())) {
            cmd = string("(define (") + name + " . rest) (" + 
                         func + " (cons " + idstr + " rest)))";
        } else {
            cmd = string("(define (") + name + " . rest) (" + 
                         func + " (cons " + idstr + 
                         " (cons \"" + name + "\" rest))))";        
        }
        gh_eval_str(cmd.c_str());
        
        // install interface for command
        scm_c_define_gsubr(func.c_str(), 1, 0, 0, (SCM (*)()) ExecFunc);
        
        delete [] idstr;
    }
}




void ScriptEngine::Init()
{
    // setup guile evironment
    InstallGuileFunctions();

    gh_eval_str("\
    (debug-enable 'debug) \
    (debug-enable 'backtrace) \
    (read-enable 'positions) \
    \
    (define (terminal::handle-error key subr msg args rest) \
              (let ((stack (fluid-ref the-last-stack)) \
                    (port  (current-output-port))) \
                (display-backtrace stack port) \
                (display-error \
                    stack \
                    port \
                    subr msg args rest))) \
    \
    (define (terminal::env) \
        (catch #t \
            (lambda () \
                (lazy-catch #t \
                    (lambda () \
                        (start-stack 'stack-base \
                            (terminal::exec-in-env))) \
                    (lambda (. args) \
                        (fluid-set! the-last-stack (make-stack #t 3)) \
                        (apply throw args)))) \
            terminal::handle-error)) \
    ");
}



ScriptCommand *GetCommand(SCM lst, bool showError)
{
    ScriptCommand *cmd = (ScriptCommand*) 
                         g_commandRegistry.Create((CommandId)
                                                  gh_scm2int(SCM_CAR(lst)));
    
    assert(cmd);
    
    // skip id and name
    if (!cmd->Setup(ScmCdr(SCM2Scm(lst)))) {
        Error("Error processing command '%s'", cmd->GetName());
        return NULL;
    }
    
    return cmd;
}


}
