#include "first.h"
#include <algorithm>
#include <vector>
#include <string>
#include "common.h"
#include "commonCommands.h"
#include "ScriptEngine.h"


namespace Vistools {

using namespace std;

static vector<StringCommand*> g_terminalCmds;
static ScriptEngine *g_engine = NULL;


extern "C" {

ScriptCommand *GetCommand(int commandid, Scm args)
{
    ScriptCommand *cmd = (ScriptCommand*) 
                         g_commandRegistry.Create((CommandId) commandid);
    
    assert(cmd);
    
    if (!cmd->Setup(args)) {
        Error("Error processing command '%s'", cmd->GetName());
        return NULL;
    }
    
    return cmd;
}


static PyObject *
Exec(PyObject *self, PyObject *tup)
{
    // hold reference to tup for safety
    Scm scmtup(tup);

    if (ScmIntp(Py2Scm(PyTuple_GET_ITEM(tup, 0)))) {
        // get command id
        int commandid = (int) PyInt_AsLong(PyTuple_GET_ITEM(tup, 0));
    
        // get command arguments
        Scm args = Py2Scm(PyTuple_GET_ITEM(tup, 1));
    
        // treat command as a s-expression        
        ScriptCommand *cmd = GetCommand(commandid, args);
        
        
        if (cmd) {
            g_engine->GetListener()->ExecCommand(*cmd);
            PyObject *ret = Scm2Py(cmd->GetReturn());
            Py_INCREF(ret);
            delete cmd;
            return ret;
        }
        
    } else if (ScmStringp(Py2Scm(PyTuple_GET_ITEM(tup, 0)))) {
        vector<string> args;
        
        args.push_back(Scm2String(Py2Scm(PyTuple_GET_ITEM(tup, 0))));
        
        PyObject *tupargs = PyTuple_GET_ITEM(tup, 1);
        
        // treat command as just a list of strings        
        for (int i=0; i<PyTuple_GET_SIZE(tupargs); i++) {
            Scm arg = Py2Scm(PyTuple_GET_ITEM(tupargs, i));
            
            if (ScmIntp(arg)) {
                int num = Scm2Int(arg);
                args.push_back(int2string(num));
            } else if (ScmFloatp(arg)) {
                float num = Scm2Float(arg);
                char str[10];
                snprintf(str, 10, "%f", num);
                args.push_back(string(str));
            } else if (ScmStringp(arg)) {
                args.push_back(Scm2String(arg));
            } else {
                Error("unknown argument type");
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        // convert strings to char*
        int argc = args.size();
        char **argv = new char* [argc];    
        for (int i=0; i<argc; i++)
            argv[i] = (char*) args[i].c_str(); // NOTE: type loophole

        
        int consume;
        StringCommand *cmd = GetCommand(g_terminalCmds, argc, argv, 
                                        &consume, false, true);

        if (cmd) {
            g_engine->GetListener()->ExecCommand(*cmd);
            delete cmd;
        }

        delete [] argv;
    } else {
        assert(0);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}


PyMethodDef *g_vistoolsMethods;




int ScriptEngine::EngineRepl(string line)
{
    // interpret empty lines
    if (line.size() == 0) {
        if (!m_cont) 
            return STATUS_READY;  // ignore empty lines
        else
            m_cont = false;       // exit continue mode
    }

    // add line to growing m_line
    m_line += line + "\n";

    // enter continue mode if line ends with colon
    if (line[line.size() - 1] == ':') {
        m_cont = true;
        m_multiline = true;
    }

    // don't process line if in continue mode
    if (m_cont) {
        return STATUS_CONTINUE;
    } else {
        // get global dictionary
        PyObject *globals = PyModule_GetDict(PyImport_AddModule("__main__"));
        Py_INCREF(globals);

        PyObject *result;
        if (m_multiline) {
            result = PyRun_String(m_line.c_str(), Py_file_input, 
                                        globals, globals);
        } else {
            result = PyRun_String(m_line.c_str(), Py_single_input, 
                                        globals, globals);
        }
        
        if (result) {
            Py_DECREF(result);
        } else {
            // handle error
            PyErr_Print();
            PyErr_Clear();
        }

        Py_DECREF(globals);

        // clear m_line
        m_line = "";
        m_multiline = false;
    }

    return STATUS_READY;
}


void ScriptEngine::Init()
{
    g_engine = this;
    g_terminalCmds = m_commands;
    g_vistoolsMethods = new PyMethodDef [m_commands.size() + 1];
    
    

    // install main command
    int table = 0;
    char *mainFunc = "__gatewayFunc";
    g_vistoolsMethods[table].ml_name  = mainFunc;
    g_vistoolsMethods[table].ml_meth  = Exec;
    g_vistoolsMethods[table].ml_flags = METH_VARARGS;
    g_vistoolsMethods[table].ml_doc   = "";
    table++;
    
    // cap the methods table with ending method
    g_vistoolsMethods[table].ml_name  = NULL;
    g_vistoolsMethods[table].ml_meth  = NULL;
    g_vistoolsMethods[table].ml_flags = 0;
    g_vistoolsMethods[table].ml_doc   = NULL;

    // register all methods with python
    PyObject *module = Py_InitModule(strdup(m_moduleName.c_str()), 
                                     g_vistoolsMethods);
    PyImport_AddModule(strdup(m_moduleName.c_str()));    
    
    for (int i=0; i<m_commands.size(); i++) {
        // get command id
        string idstr = int2string(m_commands[i]->GetId());
        
        // create python name for command
        string name = m_commands[i]->GetName();
        string help = string(m_commands[i]->GetUsage()) + " " +
                      m_commands[i]->GetDescription();
        
        // create wrapper function
        string pyCommands =  "import " + m_moduleName + "\n" +
            "def __helper_" + name +  "(* args): " +
            "  return " + m_moduleName + "." + mainFunc + "(" + 
                (m_commands[i]->HasAttr(&g_scriptAttr) ?
                    idstr :  string("'") + m_commands[i]->GetName() + "'") +
            ", args)\n" + 
            m_moduleName + "." + name + " = __helper_" + name + "\n" + 
            "__helper_" + name + ".func_name = \"" + name + "\"\n" +
            "__helper_" + name + ".func_doc = \"" + help + "\"\n";
        ScmEvalStr(pyCommands.c_str());
        printf(pyCommands.c_str());
    }
}

}


}
