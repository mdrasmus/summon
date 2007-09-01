/***************************************************************************
* SUMMON
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
#include "SummonWindow.h"



#define MODULE_NAME "summon_core"


// python visible prototypes
extern "C" {
static PyObject *Exec(PyObject *self, PyObject *tup);
static PyObject *SummonMainLoop(PyObject *self, PyObject *tup);
static PyObject *SummonShutdown(PyObject *self, PyObject *tup);
static PyObject *MakeConstruct(PyObject *self, PyObject *args);
static PyObject *IncRefConstruct(PyObject *self, PyObject *args);
static PyObject *DeleteConstruct(PyObject *self, PyObject *args);
static PyObject *GetConstructChildren(PyObject *self, PyObject *args);
static PyObject *GetConstructContents(PyObject *self, PyObject *args);
}


namespace Summon {

using namespace std;


// global prototypes
class SummonModule;
static SummonModule *g_summon;
static int g_hidden_window;
static const int INIT_WINDOW_X = 100;
static const int INIT_WINDOW_Y = 100;


class SummonModule : public CommandExecutor, public GlutViewListener
{
public:
    SummonModule() :
        m_initialized(false),
        m_runtimer(true),
        m_nextWindowId(1),
        m_nextModelId(1),
        
        m_commandWaiting(NULL),
        m_graphicsExec(false),
        m_waiting(false),        
        m_lock(SDL_CreateMutex()),
        m_cond(SDL_CreateCond()),
        m_condlock(SDL_CreateMutex()),

        m_timerCommand(NULL),
        m_timerDelay(0),
        m_windowOffset(0,0),
        m_windowCloseCallback(Scm_EOL)
    {
    }
    
    virtual ~SummonModule()
    {
        // clean up SDL thread management 
        SDL_DestroyMutex(m_lock);
        SDL_DestroyCond(m_cond);
        SDL_DestroyMutex(m_condlock);        
    }
    
    
    // initialization
    bool Init()
    {
        // init commands
        summonCommandsInit();
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
        
        static PyMethodDef *summonMethods = new PyMethodDef [9];

        // install main command
        int table = 0;
        char *mainFunc = "__gatewayFunc";
        summonMethods[table].ml_name  = mainFunc;
        summonMethods[table].ml_meth  = Exec;
        summonMethods[table].ml_flags = METH_VARARGS;
        summonMethods[table].ml_doc   = "";
        table++;

        // summon main loop
        summonMethods[table].ml_name  = "summon_main_loop";
        summonMethods[table].ml_meth  = SummonMainLoop;
        summonMethods[table].ml_flags = METH_VARARGS;
        summonMethods[table].ml_doc   = "";
        table++;

        // gracefully shutdown glut from python
        summonMethods[table].ml_name  = "summon_shutdown";
        summonMethods[table].ml_meth  = SummonShutdown;
        summonMethods[table].ml_flags = METH_VARARGS;
        summonMethods[table].ml_doc   = "";
        table++;

        // make construct
        summonMethods[table].ml_name  = "make_construct";
        summonMethods[table].ml_meth  = MakeConstruct;
        summonMethods[table].ml_flags = METH_VARARGS;
        summonMethods[table].ml_doc   = "";
        table++;
        
        // delete a construct
        summonMethods[table].ml_name  = "delete_construct";
        summonMethods[table].ml_meth  = DeleteConstruct;
        summonMethods[table].ml_flags = METH_VARARGS;
        summonMethods[table].ml_doc   = "";
        table++;
        
        // reference a construct
        summonMethods[table].ml_name  = "incref_construct";
        summonMethods[table].ml_meth  = IncRefConstruct;
        summonMethods[table].ml_flags = METH_VARARGS;
        summonMethods[table].ml_doc   = "";
        table++;

        // get the children of a construct
        summonMethods[table].ml_name  = "get_construct_children";
        summonMethods[table].ml_meth  = GetConstructChildren;
        summonMethods[table].ml_flags = METH_VARARGS;
        summonMethods[table].ml_doc   = "";
        table++;

        // get the contents of a construct
        summonMethods[table].ml_name  = "get_construct_contents";
        summonMethods[table].ml_meth  = GetConstructContents;
        summonMethods[table].ml_flags = METH_VARARGS;
        summonMethods[table].ml_doc   = "";
        table++;

        // cap the methods table with ending method
        summonMethods[table].ml_name  = NULL;
        summonMethods[table].ml_meth  = NULL;
        summonMethods[table].ml_flags = 0;
        summonMethods[table].ml_doc   = NULL;
        table++;

        // register all methods with python
        PyObject *module = Py_InitModule(MODULE_NAME, 
                                         summonMethods);
        

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
        
        return true;
    }
    
    
        
    // Main command execution function
    virtual void ExecCommand(Command &command) {
        switch (command.GetId()) {
            case CALL_PROC_COMMAND: {
                // execute a python function
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
            
            /*
            case REDRAW_CALL_COMMAND: {
                RedrawCallCommand *cmd = (RedrawCallCommand*) &command;
                Scm proc = cmd->proc;                  
                } break;
            */
            
            case GET_WINDOWS_COMMAND: {
                // get a list of all open windows
                Scm lst = Scm_EOL;
                for (WindowIter i=m_windows.begin(); i!=m_windows.end(); i++) {
                    lst = ScmCons(Int2Scm((*i).first), lst);
                }
                ((ScriptCommand*) &command)->SetReturn(lst);
                } break;
            

            case NEW_WINDOW_COMMAND:
                // create a new window
                ((ScriptCommand*) &command)->SetReturn(Int2Scm(NewWindow()));
                break;
                
                        
            case CLOSE_WINDOW_COMMAND: {
                // close a window
                int windowid = ((CloseWindowCommand*)&command)->windowid;
                SummonWindow *window = GetWindow(windowid);
                
                if (window) {
                    CloseWindow(window);
                } else {
                    Error("cannot find window %d", windowid);
                }
                } break;
            
            
            case GET_MODELS_COMMAND: {
                // get a list of all models
                Scm lst = Scm_EOL;
                for (ModelIter i=m_models.begin(); i!=m_models.end(); i++) {
                    lst = ScmCons(Int2Scm((*i).first), lst);
                }
                ((ScriptCommand*) &command)->SetReturn(lst);
                } break;
            
                
            case NEW_MODEL_COMMAND:
                // create a new model
                ((ScriptCommand*) &command)->SetReturn(Int2Scm(NewModel(MODEL_WORLD)));
                break;
            
            case DEL_MODEL_COMMAND: {
                // delete a model
                int modelid  = ((DelModelCommand*)&command)->modelid;
                SummonModel *model = GetModel(modelid);
                
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
                // attach a model to a window
                int windowid = ((AssignModelCommand*)&command)->windowid;
                string kind  =((AssignModelCommand*)&command)->kind;
                int modelid  = ((AssignModelCommand*)&command)->modelid;
                SummonWindow *window = GetWindow(windowid);
                SummonModel *model = GetModel(modelid);
                
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
            
            case GET_WINDOW_DECORATION_COMMAND: {
                // get the size of window decoration
                ((ScriptCommand*) &command)->SetReturn(
                    ScmCons(Int2Scm(m_windowOffset.x),
                            ScmCons(Int2Scm(m_windowOffset.y),
                                    Scm_EOL)));
                } break;
            
            case SET_WINDOW_CLOSE_CALLBACK_COMMAND: {
                // set the callback for window close events
                m_windowCloseCallback = ((SetWindowCloseCallbackCommand*)
                                         &command)->callback;
                
                } break;
            
            case TIMER_CALL_COMMAND: {
                // set the function to call when the timer goes off
                TimerCallCommand *cmd = (TimerCallCommand*) &command;
                SetTimerCommand(cmd->delay, new CallProcCommand(cmd->proc));
                } break;

            
            case APPEND_GROUP_COMMAND: {
                // a group to another
                AppendGroupCommand *cmd = (AppendGroupCommand*) &command;
                Element *elm = Id2Element(cmd->groupid);
                SummonModel *model = GetModelOfElement(elm);
                if (model)
                    model->AddElement(elm, cmd->code);
                } break;
            
            case REMOVE_GROUP_COMMAND2: {
                // a group to another
                RemoveGroupCommand2 *cmd = (RemoveGroupCommand2*) &command;
                Element *elm = Id2Element(cmd->groupid);
                SummonModel *model = GetModelOfElement(elm);

                if (model)
                    for (unsigned int i=0; i<cmd->groupids.size(); i++)
                        model->RemoveElement(Id2Element(cmd->groupids[i]));
                } break;
            
                
            default:
                // do command routing
                
                if (g_globalAttr.Has(&command)) {
                    // global
                    // all global commands should be executed by SUMMON
                    assert(0);
                
                } else if (g_modelAttr.Has(&command)) {
                    // route to model
                    SummonModel *model = 
                        GetModel(((ModelCommand*) &command)->modelid);
                    if (model)
                        model->ExecCommand(command);
                
                } else if (g_viewAttr.Has(&command)) {
                    // route to view
                    int windowid = ((WindowCommand*) &command)->windowid;
                    
                    SummonWindow *window = GetWindow(windowid);
                    if (window) {
                        window->GetView()->ExecCommand(command);
                    } else {
                        Error("unknown window %d", windowid);
                    }
                    
                } else if (g_controllerAttr.Has(&command)) {
                    // route to controller
                    int windowid = ((WindowCommand*) &command)->windowid;
                    
                    SummonWindow *window = GetWindow(windowid);
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
        m_windows[id] = new SummonWindow(id, this, 400, 400, "SUMMON");
        m_windows[id]->GetView()->SetOffset(m_windowOffset);
        m_windows[id]->GetView()->AddListener(this);
        m_closeWaiting[m_windows[id]->GetView()] = m_windows[id];
        m_nextWindowId++;
        m_windows[id]->SetActive();
        return id;
    }
        
    
    SummonWindow *GetWindow(int id)
    {
        if (m_windows.find(id) != m_windows.end())
            return m_windows[id];
        else
            return NULL;
    }
    
    
    void CloseWindow(SummonWindow* window)
    {
        // close the window
        window->Close();
    }
    
    
    void ViewClose(GlutView *view)
    {    
        // GLUT is done working with the window
        // it is now safe to delete the window after this function returns
        SummonWindow *window = m_closeWaiting[view];
        
        // remove window from window list
        m_windows.erase(window->GetId());
        m_closeWaiting.erase(view);
        m_deleteWaiting.push_back(window);
        
        // call the callback, if it exists
        if (m_windowCloseCallback != Scm_EOL) {
            Scm ret = ScmApply(m_windowCloseCallback, 
                               ScmCons(Int2Scm(window->GetId()),
                                       Scm_EOL));
                    
            if (Scm2Py(ret) == NULL)
                //display exceptions, return None
                PyErr_Print();
        }
    }
    
    
    void DeleteClosedWindows()
    {
        if (m_deleteWaiting.size() > 0) {
            for (vector<SummonWindow*>::iterator i = m_deleteWaiting.begin();
                 i != m_deleteWaiting.end(); i++)
            {
                delete (*i);
            }
        
            m_deleteWaiting.clear();
        }
    }
    
        
    
    int NewModel(int kind)
    {
        int id = m_nextModelId;
        m_models[id] = new SummonModel(id, kind);
        m_nextModelId++;
        return id;
    }    
    
    SummonModel *GetModel(int id)
    {
        if (m_models.find(id) != m_models.end())
            return m_models[id];
        else
            return NULL;
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
    
    
    // GLUT first timer
    // initialize window decoration size
    static void FirstTimer(int value)
    {
        // do initialization that can only be done after first pump of the 
        // GLUT event loop
        
        
        // get window offset
        glutSetWindow(g_hidden_window);
        g_summon->m_windowOffset.x = glutGet(GLUT_WINDOW_X) - INIT_WINDOW_X;
        g_summon->m_windowOffset.y = glutGet(GLUT_WINDOW_Y) - INIT_WINDOW_Y;
        glutHideWindow();
        //glutIdleFunc(NULL);
        
        // let the window manager decide window placement
        glutInitWindowSize(-1, -1);
                
        g_summon->m_initialized = true;
        
        glutTimerFunc(0, Timer, 0);
    }
    
    
    // glut timer callback
    // This timer is necessary for frequent processing of queued commands
    static void Timer(int value)
    {
        static int delay = 0;
        
        
        if (!Py_IsInitialized()) {
            // do nothing if python is not initialized
            return;
        }
        
        // delete closed windows
        g_summon->DeleteClosedWindows();
        
        // update window positions
        for (WindowIter i=g_summon->m_windows.begin(); i!=g_summon->m_windows.end(); i++) {
            (*i).second->GetView()->UpdatePosition();
        }
        

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
        
        // set the next
        if (g_summon->m_initialized)
            glutTimerFunc(delay, Timer, 0);
        else
            g_summon->m_runtimer = false;
        
        if (delay < 10)
            delay++;
    }

    
    // execute a commond in a thread safe manner
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
    
    //======================================================================
    // a boolean for whether the summon module is ready for processing commands
    bool m_initialized;
    bool m_runtimer;

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
    typedef map<int, SummonWindow*>::iterator WindowIter;
    typedef map<int, SummonModel*>::iterator ModelIter;
    map<int, SummonWindow*> m_windows;
    map<int, SummonModel*> m_models;
   
    map<GlutView*, SummonWindow*> m_closeWaiting;
    vector<SummonWindow*> m_deleteWaiting;
    Vertex2i m_windowOffset;
    
    Scm m_windowCloseCallback;
};



} // namespace Summon




//=============================================================================
// Python visible functions
//

using namespace Summon;

extern "C" {

// dummy function needed for hidden window display
void HiddenDisplay()
{}


// Begin Summon main loop
// This should be called in a separate thread, because this function will
// never return
static PyObject *
SummonMainLoop(PyObject *self, PyObject *tup)
{
    static bool isGlutInit = false;
    
    // NOTE: not totally thread safe if multiple quick calls are made
    if (isGlutInit) {
        Py_RETURN_NONE;
    }
    
    isGlutInit = true;
    
    // init glut
    int argc = 1;
    char **argv = new char* [1];
    argv[0] = "summon";
    glutInit(&argc, argv);
    
    // create hidden window
    // so that GLUT does not get upset (it always wants one window)
    //glutInitDisplayMode(GLUT_RGBA);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE );
    
    // NOTE: requires freeglut > 2.4.0-1  (2005)
    // or another GLUT implementation with this extension
#ifndef NOGLUTEXT    
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
#endif
    
    // initialize hidden window
    glutInitWindowSize(1, 1);
    glutInitWindowPosition(INIT_WINDOW_X, INIT_WINDOW_Y);
    g_hidden_window = glutCreateWindow("SUMMON");
    glutDisplayFunc(HiddenDisplay);
    
    g_summon->Lock();

    // store summon thread ID
    g_summon->m_threadId = PyThread_get_thread_ident();

    // setup glut timer
    glutTimerFunc(0, Summon::SummonModule::FirstTimer, 0);
    
    
    // begin processing of GLUT events
    Py_BEGIN_ALLOW_THREADS
    glutMainLoop();
    Py_END_ALLOW_THREADS

    
    Py_RETURN_NONE;
}


// This function is called when python is shutting down (atexit)
// Gracefully wait for summon to finish its timers and callbacks.
static PyObject *
SummonShutdown(PyObject *self, PyObject *tup)
{
    if (g_summon) {
        // turn off initizalied flag
        g_summon->m_initialized = false;
        
        // wait for timer to stop
        while (g_summon->m_runtimer)
            SDL_Delay(10);
    }
    Py_RETURN_NONE;
}



// all thread safe command executions are done through this gateway
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
                Py_RETURN_NONE;
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



static PyObject *
MakeConstruct(PyObject *self, PyObject *args)
{
    long elmid;
    PyObject *lst;
    int ok = PyArg_ParseTuple(args, "lO", &elmid, &lst);
        
    if (!ok)
        return NULL;
    Scm code = Py2Scm(lst);
    
    // add factory call
    Element *elm = g_elementFactory.Create(elmid);
    
    if (elm == NULL || !elm->Build(elmid, code)) {
        PyErr_Format(PyExc_Exception, "error constructing element");
        return NULL;
    }
    elm->IncRef();
    
    //printf("new: %p\n", elm);
    
    // return element address
    PyObject *addr = PyInt_FromLong((long) elm);
    return addr;
}


static PyObject *
DeleteConstruct(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = (Element*) addr;
    
    elm->DecRef();
    
    // if element has a parent, then parent owns element and
    // element will be deleted by parent
    if (!elm->IsReferenced() && elm->GetParent() == NULL) {
        //printf("delete: %p\n", elm);
        delete elm;
    }
    
    Py_RETURN_NONE;
}


static PyObject *
IncRefConstruct(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = (Element*) addr;
    
    //printf("incref %p\n", elm);
    elm->IncRef();
    
    Py_RETURN_NONE;
}


static PyObject *
GetConstructChildren(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = (Element*) addr;
    
    int len = elm->NumChildren() * 2;
    
    PyObject *children = PyTuple_New(len);
    int i = 0;
    for (Element::Iterator child=elm->Begin(); child!=elm->End(); child++) {
        PyTuple_SET_ITEM(children, i, PyInt_FromLong((*child)->GetSpecificId()));
        PyTuple_SET_ITEM(children, i+1, PyInt_FromLong((long) (*child)));
        i+=2;
    }
    
    return children;
}


static PyObject *
GetConstructContents(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = (Element*) addr;
    
    Scm contents = elm->GetContents();
    PyObject *pycontents = Scm2Py(contents);
    
    if (pycontents != NULL)
        Py_INCREF(pycontents);
    
    return pycontents;
}




// gateway function to python
PyMODINIT_FUNC
initsummon_core()
{
    // prepare the python environment for summon
    InitPython();
    
    // create Summon module
    g_summon = new Summon::SummonModule();    
    
    // initialize summon
    if (!g_summon->Init())
        printf("summon init error\n");
}
 
 
} // extern "C"
