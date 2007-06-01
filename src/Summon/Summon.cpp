/***************************************************************************
* Vistools
* Matt Rasmussen
* Summon.cpp
*
***************************************************************************/

#include "first.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <vector>
#include <GL/glut.h>
#include <GL/gl.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>
#include <SDL/SDL_timer.h>

#include "common.h"
#include "DrawWindow.h"



#define MODULE_NAME_STRICT "summon_core"
#define MODULE_NAME "summon_core"

#define VERSION_INFO "\
-----------------------------------------------------------------------------\n\
                                   SUMMON 1.7.1\n\
                       Large Scale Visualization Scripting\n\
                                 Matt Rasmussen \n\
                             (http://mit.edu/rasmus)\n\
                                 Copyright 2007\n\
-----------------------------------------------------------------------------\n"


// python visible prototypes
extern "C" {
static PyObject *Exec(PyObject *self, PyObject *tup);
static PyObject *SummonMainLoop(PyObject *self, PyObject *tup);
}


namespace Vistools {

using namespace std;


// global prototypes
class Summon;
static Summon *g_summon;



class Summon : public CommandExecutor, GlutViewListener
{
public:
    Summon() :
        m_initialized(false),
        m_nextWindowId(1),
        m_nextModelId(1),
        
        m_commandWaiting(NULL),
        m_graphicsExec(false),
        m_waiting(false),        
        m_lock(SDL_CreateMutex()),
        m_cond(SDL_CreateCond()),
        m_condlock(SDL_CreateMutex()),

        m_timerCommand(NULL),
        m_timerDelay(0)
    {
    }
    
    virtual ~Summon()
    {
        SDL_DestroyMutex(m_lock);
        SDL_DestroyCond(m_cond);
        SDL_DestroyMutex(m_condlock);        
    }
        
    
    virtual void ExecCommand(Command &command) {
        switch (command.GetId()) {
            case CALL_PROC_COMMAND: {
                CallProcCommand *cmd = (CallProcCommand*) &command;
                
                if (cmd->defined) {
                    Scm proc = cmd->GetScmProc();
                    Scm ret = ScmApply(proc, Scm_EOL);
                    
                    if (Scm2Py(ret) == NULL)
                        //display exceptions, return None
                        PyErr_Print();
                    else 
                        cmd->SetReturn(ret);
                }
                
                } break;
            
            case REDRAW_CALL_COMMAND: {
                RedrawCallCommand *cmd = (RedrawCallCommand*) &command;
                
                Scm proc = cmd->proc;
                                
                } break;
            
            case GET_WINDOWS_COMMAND: {
                Scm lst = Scm_EOL;
                for (WindowIter i=m_windows.begin(); i!=m_windows.end(); i++) {
                    lst = ScmCons(Int2Scm((*i).first), lst);
                }
                ((ScriptCommand*) &command)->SetReturn(lst);
                } break;
            

            case NEW_WINDOW_COMMAND:
                ((ScriptCommand*) &command)->SetReturn(Int2Scm(NewWindow()));
                break;
                
                        
            case CLOSE_WINDOW_COMMAND: {
                int windowid = ((CloseWindowCommand*)&command)->windowid;
                DrawWindow *window = GetWindow(windowid);
                
                if (window) {
                    CloseWindow(window);
                } else {
                    Error("cannot find window %d", windowid);
                }
                } break;
            
            
            case GET_MODELS_COMMAND: {
                Scm lst = Scm_EOL;
                for (ModelIter i=m_models.begin(); i!=m_models.end(); i++) {
                    lst = ScmCons(Int2Scm((*i).first), lst);
                }
                ((ScriptCommand*) &command)->SetReturn(lst);
                } break;
            
                
            case NEW_MODEL_COMMAND:
                ((ScriptCommand*) &command)->SetReturn(Int2Scm(NewModel(MODEL_WORLD)));
                break;
            
            case DEL_MODEL_COMMAND: {
                int modelid  = ((DelModelCommand*)&command)->modelid;
                DrawModel *model = GetModel(modelid);
                
                if (!model) {
                    Error("cannot find model %d", modelid);
                    break;
                }
                
                // remove model from any windows that are using it
                for (WindowIter i=m_windows.begin(); i!=m_windows.end(); i++) {
                    if ((*i).second->GetWorldModel() == model)
                        (*i).second->SetWorldModel(NULL);
                    if ((*i).second->GetScreenModel() == model)
                        (*i).second->SetScreenModel(NULL);
                }
                
                // delete model
                m_models.erase(modelid);
                delete model;
                
            } break;
            
            case ASSIGN_MODEL_COMMAND: {
                int windowid = ((AssignModelCommand*)&command)->windowid;
                string kind  =((AssignModelCommand*)&command)->kind;
                int modelid  = ((AssignModelCommand*)&command)->modelid;
                DrawWindow *window = GetWindow(windowid);
                DrawModel *model = GetModel(modelid);
                
                // ensure window and model exists
                if (window) {
                    if (model) {                       
                        // set window's model to model
                        if (kind == "world") {
                            window->SetWorldModel(model);
                        } else if (kind == "screen") {
                            window->SetScreenModel(model);
                        } else {
                            Error("unknown model kind '%s'", kind.c_str());
                        }
                    } else {
                        Error("cannot find model %d", modelid);
                    }
                } else {
                    Error("cannot find window %d", windowid);
                }
                
                } break;
            
            
            
            
            case VERSION_COMMAND: {
                fprintf(stderr, VERSION_INFO);
                } break;
            
            
            case TIMER_CALL_COMMAND: {
                TimerCallCommand *cmd = (TimerCallCommand*) &command;
                SetTimerCommand(cmd->delay, new CallProcCommand(cmd->proc));
                } break;
                
                
            default:
                // do command routing
                if (g_globalAttr.Has(&command)) {
                    // global
                    // all global commands should be executed by SUMMON
                    assert(0);
                
                } else if (g_modelAttr.Has(&command)) {
                    // model
                    DrawModel *model = 
                        GetModel(((ModelCommand*) &command)->modelid);
                    if (model)
                        model->ExecCommand(command);
                
                } else if (g_viewAttr.Has(&command)) {
                    // view
                    int windowid = ((WindowCommand*) &command)->windowid;
                    
                    DrawWindow *window = GetWindow(windowid);
                    if (window) {
                        window->GetView()->ExecCommand(command);
                    } else {
                        Error("unknown window %d", windowid);
                    }
                    
                } else if (g_controllerAttr.Has(&command)) {
                    // controller
                    int windowid = ((WindowCommand*) &command)->windowid;
                    
                    DrawWindow *window = GetWindow(windowid);
                    if (window) {
                        window->GetController()->ExecCommand(command);
                    } else {
                        Error("unknown window %d", windowid);
                    }
                }
        }
    }
    
    int NewWindow()
    {
        int id = m_nextWindowId;
        m_windows[id] = new DrawWindow(id, this, 400, 400, "SUMMON");
        m_nextWindowId++;
        m_windows[id]->SetActive();
        return id;
    }
        
    
    DrawWindow *GetWindow(int id)
    {
        if (m_windows.find(id) != m_windows.end())
            return m_windows[id];
        else
            return NULL;
    }
    
    
    void CloseWindow(DrawWindow* window)
    {
        // remove window from window list
        m_windows.erase(window->GetId());
        
        // close the window
        window->Close();
        
        // put the window on a queue to be deleted
        m_closeWaiting[window->GetView()] = window;
    }
    
    
    void OnClose(GlutView *view)
    {
        // it is now safe to delete waiting window 
        // (GLUT will make no further calls)
        delete m_closeWaiting[view];
        m_closeWaiting.erase(view);
    }
        
    
    int NewModel(int kind)
    {
        int id = m_nextModelId;
        m_models[id] = new DrawModel(id, kind);
        m_nextModelId++;
        return id;
    }    
    
    DrawModel *GetModel(int id)
    {
        if (m_models.find(id) != m_models.end())
            return m_models[id];
        else
            return NULL;
    }
    

    
    
    bool Init()
    {    
        ModuleInit();
        InitDrawEnv();    
        
        return true;
    }
    
    
    void ModuleInit()
    {
        // init commands
        drawCommandsInit();
        m_summonCommands.clear();
        
        for (CommandAttr::Iterator i=g_scriptAttr.Begin();
             i != g_scriptAttr.End(); i++)
        {
            if (!g_constructAttr.Has((*i)->GetId()) &&
                strlen(((ScriptCommand*) *i)->GetName()) > 0)
            {
                m_summonCommands.push_back((ScriptCommand*) *i);
            }
        }
        
        static PyMethodDef *g_vistoolsMethods = new PyMethodDef [m_summonCommands.size() + 1];

        // install main command
        int table = 0;
        char *mainFunc = "__gatewayFunc";
        g_vistoolsMethods[table].ml_name  = mainFunc;
        g_vistoolsMethods[table].ml_meth  = Exec;
        g_vistoolsMethods[table].ml_flags = METH_VARARGS;
        g_vistoolsMethods[table].ml_doc   = "";
        table++;

        // summon main loop
        g_vistoolsMethods[table].ml_name  = "summon_main_loop";
        g_vistoolsMethods[table].ml_meth  = SummonMainLoop;
        g_vistoolsMethods[table].ml_flags = METH_VARARGS;
        g_vistoolsMethods[table].ml_doc   = "";
        table++;

        // cap the methods table with ending method
        g_vistoolsMethods[table].ml_name  = NULL;
        g_vistoolsMethods[table].ml_meth  = NULL;
        g_vistoolsMethods[table].ml_flags = 0;
        g_vistoolsMethods[table].ml_doc   = NULL;

        // register all methods with python
        PyObject *module = Py_InitModule(MODULE_NAME_STRICT, 
                                         g_vistoolsMethods);
        

        for (unsigned int i=0; i<m_summonCommands.size(); i++) {
            // get command id
            string idstr = int2string(m_summonCommands[i]->GetId());

            // create python name for command
            string name = m_summonCommands[i]->GetName();
            string help = string("(") + string(m_summonCommands[i]->GetUsage()) + 
                          ")\\n" + m_summonCommands[i]->GetDescription();

            // create wrapper function
            string pyCommands =  "import " MODULE_NAME "\n"
                "def " + name +  "(* args): " +
                "  return " MODULE_NAME "." + mainFunc + "(" + 
                    (m_summonCommands[i]->HasAttr(&g_scriptAttr) ?
                        idstr :  string("'") + m_summonCommands[i]->GetName() + "'") +
                ", args)\n" + 
                name + ".func_doc = \"" + help + "\"\n"
                MODULE_NAME "." + name + " = " + name + "\n" +
                "del " + name + "\n";
                //"__helper_" + name + ".func_name = \"" + name + "\"\n";
            ScmEvalStr(pyCommands.c_str());
        }
    }
    
    // TODO: make less ugly
    void InitDrawEnv()
    {
        // install group id generator
        ScmEvalStr("import " MODULE_NAME);
        ScmEvalStr(
MODULE_NAME ".__groupid = 1 \n\
def __new_groupid(): \n\
    "MODULE_NAME".__groupid = "MODULE_NAME".__groupid + 1 \n\
    return "MODULE_NAME".__groupid\n\
"MODULE_NAME".new_groupid = __new_groupid\n");


        // register constructs
        for (CommandAttr::Iterator i=g_constructAttr.Begin(); 
             i != g_constructAttr.End(); i++)
        {
            Construct *cmd = (Construct*) *i;
            string str;

            string name = string(cmd->GetName());
            string id = int2string(cmd->GetId());
            string help = string("(") + string(cmd->GetUsage()) + 
                  ")\\n" + cmd->GetDescription();

            if (cmd->GetId() == GROUP_CONSTRUCT) {
                str = string("") + "\
def __group(* args): return (" + id + ", __new_groupid()) + args\n\
__group.func_doc = \"" + help + "\"\n\
def __list2group(lst):\n\
    return ("+ id +", __new_groupid()) + tuple(lst)\n\
def __is_group(obj): return (obj[0] == " + id + ")\n\
def __group_contents(obj): return obj[2:]\n\
def __get_group_id(obj): return obj[1]\n\
"MODULE_NAME".group = __group\n\
"MODULE_NAME".list2group = __list2group\n\
"MODULE_NAME".is_group = __is_group\n\
"MODULE_NAME".group_contents = __group_contents\n\
"MODULE_NAME".get_group_id = __get_group_id\n";
            } else {
                str = string("") + "\
def __" + name + "(* args): return (" + id + ",) + args\n\
__" + name + ".func_doc = \"" + help + "\"\n\
def __is_" + name + "(obj): return (obj[0] == " + id + ")\n\
def __" + name + "_contents(obj): return obj[1:]\n\
"MODULE_NAME"." + name + " = __" + name + "\n\
"MODULE_NAME".is_" + name + " = __is_" + name + "\n\
"MODULE_NAME"." + name + "_contents = __" + name + "_contents\n\
";
            }

            ScmEvalStr(str.c_str());
        }
    }
    
    
    

    // synchronization and thread management    
    inline void Lock()
    {
        assert(SDL_mutexP(m_lock) == 0);
    }
    
    inline void Unlock()
    {
        assert(SDL_mutexV(m_lock) == 0);
    }
    
    inline bool IsCommandWaiting()
    {
        return m_commandWaiting != NULL;
    }
    
    inline void WaitForExec()
    {
        assert(SDL_mutexP(m_condlock) == 0);
        assert(SDL_CondWait(m_cond, m_condlock) == 0);
    }
    
    inline void NotifyExecOccurred()
    {
        m_commandWaiting = NULL;
        assert(SDL_CondBroadcast(m_cond) == 0);
    }
    
    // get number of milliseconds since program started
    inline int GetTime()
    { return SDL_GetTicks(); }
    
    inline void SetTimerCommand(float delay, Command *command)
    {
        m_timerDelay = GetTime() + int(delay * 1000);
        m_timerCommand = command;
    }

    // glut timer callback
    static void Timer(int value)
    {
        static int delay = 0;
                

        if (g_summon->IsCommandWaiting()) {
            if (g_summon->m_graphicsExec) {
                // graphics command is executed in this thread
                
                // command may also call python so aquire the GIL
                PyGILState_STATE gstate = PyGILState_Ensure();
                
                g_summon->ExecCommand(*g_summon->m_commandWaiting);
                g_summon->m_commandWaiting = NULL;
                g_summon->m_graphicsExec = false;
                
                PyGILState_Release(gstate);
                
                delay = 0;
            } else {
                // non-graphic command will be execute in other thread
                // release lock on SUMMON
                g_summon->Unlock();        

                // sleep for  awhile to allow multiple non-graphics 
                // commands to execute
                while (g_summon->IsCommandWaiting() && 
                       !g_summon->m_graphicsExec)
                {
                    if (g_summon->m_timerCommand) {
                        // calculate time until timer goes off
                        int remaining = g_summon->m_timerDelay -
                                        g_summon->GetTime();
                        if (remaining > 0) {
                            SDL_Delay(remaining);
                        }
                        break;
                    } else {
                        SDL_Delay(10);
                    }
                }
            
                g_summon->Lock();
                
            }
        }
        
        
        // wake up python thread if it is waiting for a graphic command to
        // complete
        if (g_summon->m_waiting && !g_summon->m_graphicsExec)
            g_summon->NotifyExecOccurred();
        
        // look at timer-delay function
        if (g_summon->m_timerCommand && 
            g_summon->GetTime() > g_summon->m_timerDelay) 
        {
            Command *cmd = g_summon->m_timerCommand;
            g_summon->m_timerCommand = NULL;
            
            PyGILState_STATE gstate = PyGILState_Ensure();
            g_summon->ExecCommand(*cmd);
            PyGILState_Release(gstate);
            
            delete cmd;
        }
        
        glutTimerFunc(delay, Timer, 0);
        
        if (delay < 10)
            delay++;
    }


    inline void ThreadSafeExecCommand(Command *command)
    {
        if (!m_initialized)
            return;    
    
        int curThreadId = PyThread_get_thread_ident();
    
        if (curThreadId == m_threadId) {
            // execute command in this thread if we are in the summon thread
            ExecCommand(*command);
        } else {
            // we are the python thread, more care is needed
            
            // commands that manipulate OpenGL must be passed to the other thread
            if (command->HasAttr(&g_glAttr)) {
                // pass command to other thread
                assert(m_commandWaiting == NULL);
                m_graphicsExec = true;
                m_waiting = true;
                m_commandWaiting = command;

                // wait for graphics thread to exec command
                // graphics thread will reset m_commandWaiting and m_graphicsExec
                Py_BEGIN_ALLOW_THREADS
                WaitForExec();
                Py_END_ALLOW_THREADS
                
                m_waiting = false;
                
                assert(m_commandWaiting == NULL);

            } else {
                // execute command in this thread once exclusive lock is obtained
                assert(m_commandWaiting == NULL);
                m_commandWaiting = command;

                Py_BEGIN_ALLOW_THREADS
                Lock();
                Py_END_ALLOW_THREADS

                ExecCommand(*command);
                m_commandWaiting = NULL;
                Unlock();
            }
        }
    }
    
    
    // a boolean for whether the summon module is ready for processing commands
    bool m_initialized;

    // indexes
    int m_nextWindowId;
    int m_nextModelId;

    // thread management
    Command *m_commandWaiting;
    bool m_graphicsExec;
    bool m_waiting;
    int m_threadId;
    SDL_mutex *m_lock;
    SDL_cond *m_cond;    
    SDL_mutex *m_condlock;    

        
    // time-delay command execution
    Command *m_timerCommand;    
    int m_timerDelay;
    
    
    // list of commands directly visible in python
    vector<StringCommand*> m_summonCommands;
    
    // all windows and models
    typedef map<int, DrawWindow*>::iterator WindowIter;
    typedef map<int, DrawModel*>::iterator ModelIter;
    map<int, DrawWindow*> m_windows;
    map<int, DrawModel*> m_models;
   
    map<GlutView*, DrawWindow*> m_closeWaiting;
};



} // namespace Vistools




//=============================================================================
// Python visible functions
//

using namespace Vistools;

extern "C" {

// Begin Summon main loop
// This should be called in a separate thread, because this function will
// never return
static PyObject *
SummonMainLoop(PyObject *self, PyObject *tup)
{
    static bool isGlutInit = false;
    
    // NOTE: not totally thread safe
    if (!isGlutInit) {
        isGlutInit = true;
        g_summon->Lock();
        
        // store summon thread ID
        g_summon->m_threadId = PyThread_get_thread_ident();
        g_summon->m_initialized = true;
        
        // setup glut timer
        glutTimerFunc(10, Summon::Timer, 0);
        
        Py_BEGIN_ALLOW_THREADS
        glutMainLoop();
        Py_END_ALLOW_THREADS
    }
    
    Py_RETURN_NONE;
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
        ScriptCommand *cmd = (ScriptCommand*) 
                         g_commandRegistry.Create((CommandId) commandid);
        assert(cmd);
    
        if (cmd->Setup(args)) {
            // execute command
            g_summon->ThreadSafeExecCommand(cmd);
            PyObject *ret = Scm2Py(cmd->GetReturn());
            Py_INCREF(ret);
            delete cmd;
            return ret;
        } else {
            Error("Error processing command '%s'", cmd->GetName());
            delete cmd;
        }
        
    } else if (ScmStringp(Py2Scm(PyTuple_GET_ITEM(tup, 0)))) {
        // old style? what about "trans"
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
        StringCommand *cmd = GetCommand(g_summon->m_summonCommands, argc, argv, 
                                        &consume, false, true);

        if (cmd) {
            g_summon->ThreadSafeExecCommand(cmd);
            delete cmd;
        }

        delete [] argv;
    } else {
        assert(0);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}


PyMODINIT_FUNC
initsummon_core()
{
    // init glut
    int argc = 1;
    char *argv[1] = {"summon"};
    glutInit(&argc, argv);
    
    // create hidden window
    // so that GLUT does not get upset (it always wants one window)
    //glutInitDisplayMode(GLUT_RGBA);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE );
    
    // NOTE: requires freeglut > 2.4.0-1  (2005)
    // or another GLUT implementation with this extension
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    
    glutInitWindowSize(1, 1);
    int hidden_window = glutCreateWindow("SUMMON");
    glutHideWindow();
    
    InitPython();
    
    // create Summon app
    g_summon = new Summon();    
    
    // run the actual Summon app
    if (!g_summon->Init())
        printf("error\n");
}
 
 
} // extern "C"
