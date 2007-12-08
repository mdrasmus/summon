/***************************************************************************
* SUMMON
* Matt Rasmussen
* Summon.cpp
*
***************************************************************************/
 
#include "first.h"

// c/c++ headers
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <vector>

// opengl headers
#include <GL/glut.h>
#include <GL/gl.h>

// SDL headers
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>
#include <SDL/SDL_timer.h>

// summon headers
#include "common.h"
#include "SummonWindow.h"


// constants
#define MODULE_NAME "summon_core"


// python visible prototypes
extern "C" {

// functions directly visible in python
static PyObject *Exec(PyObject *self, PyObject *tup);
static PyObject *SummonMainLoop(PyObject *self, PyObject *tup);
static PyObject *SummonShutdown(PyObject *self, PyObject *tup);
static PyObject *MakeConstruct(PyObject *self, PyObject *args);
static PyObject *IncRefConstruct(PyObject *self, PyObject *args);
static PyObject *DeleteConstruct(PyObject *self, PyObject *args);
static PyObject *GetConstructChildren(PyObject *self, PyObject *args);
static PyObject *GetConstructContents(PyObject *self, PyObject *args);
static PyObject *GetConstructParent(PyObject *self, PyObject *args);

// python module method table
// NOTE: the (char*) casts are needed to avoid compiler warnings.
static const char *g_gatewayFunc = "__gatewayFunc";        
static PyMethodDef g_summonMethods[] = {
    // install main command
    {(char*) g_gatewayFunc,  Exec, METH_VARARGS, (char*) ""},

    // summon main loop
    {(char*) "summon_main_loop", SummonMainLoop, METH_VARARGS, (char*) ""},

    // gracefully shutdown glut from python
    {(char*) "summon_shutdown", SummonShutdown, METH_VARARGS, (char*) ""},

    // make construct
    {(char*) "make_construct", MakeConstruct, METH_VARARGS, (char*) ""},

    // delete a construct
    {(char*) "delete_construct", DeleteConstruct, METH_VARARGS, (char*) ""},

    // reference a construct
    {(char*) "incref_construct", IncRefConstruct, METH_VARARGS, (char*) ""},

    // get the children of a construct
    {(char*) "get_construct_children", GetConstructChildren, METH_VARARGS, (char*) ""},

    // get the contents of a construct
    {(char*) "get_construct_contents", GetConstructContents, METH_VARARGS, (char*) ""},

    // get the parent of a construct
    {(char*) "get_construct_parent", GetConstructParent, METH_VARARGS, (char*) ""},

    // cap the methods table with ending method
    {NULL, NULL, 0, NULL}
};

} // extern "C"


namespace Summon {

using namespace std;


// global prototypes
class SummonModule;
static SummonModule *g_summon;
static int g_hidden_window;

// NOTE: don't make this too small, some window managers don't allow windows
// to be opened with their left-top corner off of the desktop
static const int INIT_WINDOW_X = 100;
static const int INIT_WINDOW_Y = 100;

// NOTE: don't make this too small, some window mangers don't allow
// values less than 20 (or so) thus the window will never attain the
// correct size
const static int INIT_WINDOW_SIZE = 50;


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
        m_windowCloseCallback(Scm_EOL),
        
        m_nextMenuItem(1)
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
        // register all direction functions with python
        Py_InitModule((char*) MODULE_NAME, g_summonMethods);
        
        // init summon commands
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
        
        // register summon commands through gateway function
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
                "  return " MODULE_NAME "." + g_gatewayFunc + 
                "(" + idstr + ", args)\n" + 
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
            

            case NEW_WINDOW_COMMAND: {
                // create a new window
                NewWindowCommand *cmd = (NewWindowCommand*) &command;
                
                cmd->SetReturn(Int2Scm(NewWindow(cmd->name, cmd->size,
                                                 cmd->position)));
                } break;
                
                        
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


            //=============================================================
            // menu commands
            
            case NEW_MENU_COMMAND: {
                // create a new menu
                int menuid = glutCreateMenu(SummonModule::MenuCallback);
                ((DelMenuCommand*) &command)->SetReturn(Int2Scm(menuid));
                
                } break;
                
            case DEL_MENU_COMMAND: {
                // delete a menu
                glutDestroyMenu(((DelMenuCommand*) &command)->menuid);
                } break;
            
            case ADD_MENU_ENTRY_COMMAND: {
                // add an entry to a menu
                AddMenuEntryCommand *cmd = (AddMenuEntryCommand*) &command;
                
                glutSetMenu(cmd->menuid);
                glutAddMenuEntry(cmd->text.c_str(), m_nextMenuItem);
                m_menuItems[m_nextMenuItem] = cmd->func;
                m_nextMenuItem++;
                
                } break;
                
            case ADD_SUBMENU_COMMAND: {
                AddSubmenuCommand *cmd = (AddSubmenuCommand*) &command;
                
                glutSetMenu(cmd->menuid);
                glutAddSubMenu(cmd->text.c_str(), cmd->submenuid);
                
                } break;
            
            case REMOVE_MENU_ITEM_COMMAND: {
                RemoveMenuItemCommand *cmd = (RemoveMenuItemCommand*) &command;
            
                glutSetMenu(cmd->menuid);
                glutRemoveMenuItem(cmd->index);
                m_menuItems.erase(cmd->index);
                
                } break;
            
            case SET_MENU_ENTRY_COMMAND: {
                SetMenuEntryCommand *cmd = (SetMenuEntryCommand*) &command;
                
                glutSetMenu(cmd->menuid);
                glutChangeToMenuEntry(cmd->index, cmd->text.c_str(), m_nextMenuItem);
                m_menuItems[m_nextMenuItem] = cmd->func;
                m_nextMenuItem++;
                } break;
                
            
            case SET_SUBMENU_COMMAND: {
                SetSubmenuCommand *cmd = (SetSubmenuCommand*) &command;
                
                glutSetMenu(cmd->menuid);
                glutChangeToSubMenu(cmd->index, cmd->text.c_str(), cmd->submenuid);
                } break;
            
            
            //=============================================================
            // element commands
            // TODO: can I simplify this code?
            
            
            case APPEND_GROUP_COMMAND: {
                // append a group to another
                AppendGroupCommand *cmd = (AppendGroupCommand*) &command;
                Element *elm = Id2Element(cmd->groupid);
                SummonModel *model = GetModelOfElement(elm);
                if (model)
                    model->AddElement(elm, cmd->code);
                else {
                    elm->AddChild(cmd->code);
                }
                    
                } break;
            
            case REMOVE_GROUP_COMMAND2: {
                // remove a group
                RemoveGroupCommand2 *cmd = (RemoveGroupCommand2*) &command;
                Element *elm = Id2Element(cmd->groupid);
                SummonModel *model = GetModelOfElement(elm);

                if (model) {
                    for (unsigned int i=0; i<cmd->groupids.size(); i++) {
                        if (!model->RemoveElement(Id2Element(cmd->groupids[i])))
                            break;
                    }
                } else {
                    for (unsigned int i=0; i<cmd->groupids.size(); i++) {
                        Element *child = Id2Element(cmd->groupids[i]);
                        if (child->GetParent() == elm)
                            elm->RemoveChild(child);
                        else
                            Error("element is not a child");
                    }
                }
                } break;
            
            case REPLACE_GROUP_COMMAND2: {
                ReplaceGroupCommand2 *cmd = (ReplaceGroupCommand2*) &command;
                Element *elm = Id2Element(cmd->groupid);
                Element *oldelm = Id2Element(cmd->oldgroupid);
                Scm newcode = cmd->code;
                SummonModel *model = GetModelOfElement(elm);
                Element *newelm = NULL;
                
                if (model) {
                    newelm = model->ReplaceElement(oldelm, newcode);
                } else {
                    newelm = elm->ReplaceChild(oldelm, newcode);
                }
                
                } break;
                
            case SHOW_GROUP_COMMAND2: {
                ShowGroupCommand2 *cmd = (ShowGroupCommand2*) &command;
                Element *elm = Id2Element(cmd->groupid);
                SummonModel *model = GetModelOfElement(elm);
                
                elm->SetVisible(cmd->visible); 
                
                if (model)
                    model->Update(elm);
                
                } break;               
            
            case GET_BOUNDING_COMMAND2: {
                GetBoundingCommand2 *cmd = (GetBoundingCommand2*) &command;
                Element *elm = Id2Element(cmd->groupid);
                SummonModel *model = GetModelOfElement(elm);                
                
                if (model) {
                    Vertex2f pos1, pos2;
                    model->FindBounding(elm, &pos1, &pos2);
            
                    cmd->SetReturn(ScmCons(Float2Scm(pos1.x),
                                 ScmCons(Float2Scm(pos1.y),
                                   ScmCons(Float2Scm(pos2.x),
                                     ScmCons(Float2Scm(pos2.y), Scm_EOL)))));
                } else {
                    Error("element must be added to a model to find its bounding box");
                }
                
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
    
    
    // Callback for GLUT pop-up menu item click
    static void MenuCallback(int item)
    {
        PyGILState_STATE gstate = PyGILState_Ensure();
        
        Scm proc = g_summon->m_menuItems[item];
        Scm ret = ScmApply(proc, Scm_EOL);

        //display exceptions
        if (Scm2Py(ret) == NULL)
            PyErr_Print();
        
        PyGILState_Release(gstate);    
    }
    
    
    // Creates a new SUMMON window
    //  name:       title of window
    //  size:       size of window
    //  position:   position of window on desktop
    int NewWindow(string name, Vertex2i size, Vertex2i position)
    {
        int id = m_nextWindowId;
        m_windows[id] = new SummonWindow(id, this, size.x, size.y, 
                                         name.c_str(), position.x, position.y);
        m_windows[id]->GetView()->SetOffset(m_windowOffset);
        m_windows[id]->GetView()->AddListener(this);
        m_closeWaiting[m_windows[id]->GetView()] = m_windows[id];
        m_nextWindowId++;
        m_windows[id]->SetActive();
        return id;
    }
        
    
    // Gets the SUMMON window object associated with an window id
    SummonWindow *GetWindow(int id)
    {
        if (m_windows.find(id) != m_windows.end())
            return m_windows[id];
        else
            return NULL;
    }
    
    
    // Closes a SUMMON window
    void CloseWindow(SummonWindow* window)
    {
        // remove window from set of open windows
        m_windows.erase(window->GetId());
        
        // close the window
        window->Close();
    }
    
    
    // Callback from GLUT notifying us that the window is closed and is ready
    // to be deleted.  However, we cannot delete the window until one pump
    // of the GLUT event queue.  Thus, we place the window on a delete waiting
    // list
    void ViewClose(GlutView *view)
    {    
        // GLUT is done working with the window
        // it is now safe to delete the window after this function returns
        SummonWindow *window = m_closeWaiting[view];
        
        // remove window from window list        
        m_closeWaiting.erase(view);
        
        // add this window to the deletion waiting list
        m_deleteWaiting.push_back(window);
        
        // call the user-defined callback, if it exists
        if (m_windowCloseCallback != Scm_EOL) {
            // let python know window has been closed
            Scm ret = ScmApply(m_windowCloseCallback, 
                               ScmCons(Int2Scm(window->GetId()),
                                       Scm_EOL));
                    
            if (Scm2Py(ret) == NULL)
                //display exceptions, return None
                PyErr_Print();
        }
    }
    
    
    // Delete any windows that have been recently closed and are ready to be 
    // deleted
    inline void DeleteClosedWindows()
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
    
    
    // Take note of the new window positions
    inline void UpdateWindowPositions()
    {
        PyGILState_STATE gstate = PyGILState_Ensure();    
        for (WindowIter i=m_windows.begin(); i!=m_windows.end(); i++) {
            SummonView *view = (*i).second->GetView();            
            view->UpdatePosition();
        }
        PyGILState_Release(gstate);
    }    
        
    
    // Create a new model (kind = WORLD, SCREEN)
    int NewModel(int kind)
    {
        int id = m_nextModelId;
        m_models[id] = new SummonModel(id, kind);
        m_nextModelId++;
        return id;
    }    
    
    // Get a SUMMON model by its id
    SummonModel *GetModel(int id)
    {
        if (m_models.find(id) != m_models.end())
            return m_models[id];
        else
            return NULL;
    }
    
    // Set the user-defined timer command
    inline void SetTimerCommand(float delay, Command *command)
    {
        m_timerDelay = GetTime() + int(delay * 1000);
        m_timerCommand = command;
    }
    
    
    //==================================================
    // synchronization and thread management    
    
    // Lock the SUMMON module for exclusive command execution
    inline void Lock()
    {
        assert(SDL_mutexP(m_lock) == 0);
    }
    
    // Unlock the SUMMON model to allow other threads to execute commands
    inline void Unlock()
    {
        assert(SDL_mutexV(m_lock) == 0);
    }
    
    // Returns whether a command waiting to be executed
    inline bool IsCommandWaiting()
    {
        return m_commandWaiting != NULL;
    }
    
    // Wait for a submitted command to be executed
    inline void WaitForExec()
    {
        assert(SDL_mutexP(m_condlock) == 0);
        assert(SDL_CondWait(m_cond, m_condlock) == 0);
    }
    
    // Notify those waiting that a command has been executed
    inline void NotifyExecOccurred()
    {
        m_commandWaiting = NULL;
        assert(SDL_CondBroadcast(m_cond) == 0);
    }
    
    // get number of milliseconds since program started
    inline int GetTime()
    { return SDL_GetTicks(); }
    
    
    //=======================================================
    // GLUT first timer
    // dummy function needed for hidden window display

    static void FirstDisplay()
    {}

    // used to help initialize window decoration    
    static void FirstReshape(int width, int height)
    {    
        if (!g_summon->m_initialized) {
            if (glutGet(GLUT_WINDOW_WIDTH) != INIT_WINDOW_SIZE ||
                glutGet(GLUT_WINDOW_HEIGHT) != INIT_WINDOW_SIZE)
            {
                glutReshapeWindow(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);
            } else {
                // get window offset
                g_summon->m_windowOffset.x = glutGet(GLUT_WINDOW_X) - INIT_WINDOW_X;
                g_summon->m_windowOffset.y = glutGet(GLUT_WINDOW_Y) - INIT_WINDOW_Y;

                // offset is now consistent, start the real timer
                glutHideWindow();                
                g_summon->m_initialized = true;
                glutTimerFunc(0, Timer, 0);
            }
        }
    }
    
    //===========================================================
    // GLUT timer callback
    // This function executes periodically to process queued commands
    // and to handle window operations
    static void Timer(int value)
    {
        static int delay = 0;
        
        // do nothing if python is not initialized        
        if (!Py_IsInitialized())
            return;
        
        // delete closed windows
        g_summon->DeleteClosedWindows();
        
        // update window positions
        g_summon->UpdateWindowPositions();
        
        // process waiting commands commands
        if (g_summon->ExecWaitingCommands())
            delay = 0;
        
        // look at timer-delay function
        g_summon->SummonTimer();
        
        // set the next
        if (g_summon->m_initialized)
            glutTimerFunc(delay, Timer, 0);
        else
            g_summon->m_runtimer = false;
        
        if (delay < 10)
            delay++;
    }
    
    // execute the user-defined timer function
    inline void SummonTimer()
    {
        if (m_timerCommand && GetTime() > m_timerDelay) {
            Command *cmd = m_timerCommand;
            m_timerCommand = NULL;
            
            PyGILState_STATE gstate = PyGILState_Ensure();
            ExecCommand(*cmd);
            PyGILState_Release(gstate);
            
            delete cmd;
        }
    }
    
    
    inline bool ExecWaitingCommands()
    {
        bool nodelay = false;
    
        if (IsCommandWaiting()) {
            if (m_graphicsExec) {
                // graphics command is executed in this thread
                
                // command may also call python so aquire the GIL
                PyGILState_STATE gstate = PyGILState_Ensure();
                
                ExecCommand(*m_commandWaiting);
                m_commandWaiting = NULL;
                m_graphicsExec = false;
                
                PyGILState_Release(gstate);
                
                nodelay = true;
            } else {
                // non-graphic command will be execute in other thread
                // release lock on SUMMON
                Unlock();        

                // sleep for  awhile to allow multiple non-graphics 
                // commands to execute
                while (IsCommandWaiting() && !m_graphicsExec)
                {
                    if (m_timerCommand) {
                        // calculate time until timer goes off
                        int remaining = m_timerDelay - GetTime();
                        if (remaining > 0) {
                            SDL_Delay(remaining);
                        }
                        break;
                    } else {
                        SDL_Delay(10);
                    }
                }
            
                Lock();
            }
        }
        
        // wake up python thread if it is waiting for a graphic command to
        // complete
        if (g_summon->m_waiting && !g_summon->m_graphicsExec)
            g_summon->NotifyExecOccurred();        
        
        return nodelay;
    }

    
    // Execute a command in a thread safe manner
    inline void ThreadSafeExecCommand(Command *command)
    {
        // do nothing until summon is initialized
        if (!m_initialized)
            return;    
    
        int curThreadId = PyThread_get_thread_ident();
        
        if (curThreadId == m_threadId) {
            // execute command in this thread if we are in the summon thread
            ExecCommand(*command);
        } else {
            // we are the python thread, more care is needed
            
            // try to execute element commands directly
            if (command->HasAttr(&g_elementAttr)) {
                ElementCommand *elmCmd = (ElementCommand*) command;
                Element *elm = Id2Element(elmCmd->groupid);
                SummonModel *model = GetModelOfElement(elm);                
                
                if (!model) {
                    // direct execution
                    ExecCommand(*command);
                    return;
                }
                // element is attached to model, so fall through
            }

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
    
    // menus
    int m_nextMenuItem;
    map<int, Scm> m_menuItems;
};



} // namespace Summon




//=============================================================================
// Python visible functions
//

using namespace Summon;

extern "C" {



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
    argv[0] = (char*) "summon"; // avoid compiler warning (GLUT's fault)
    glutInit(&argc, argv);
    
    // create hidden window
    // so that GLUT does not get upset (it always wants one window)
    //glutInitDisplayMode(GLUT_RGBA);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE );
    
    // NOTE: requires freeglut > 2.4.0-1  (2005)
    // or another GLUT implementation with this extension
#ifdef GLUT_ACTION_ON_WINDOW_CLOSE
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
#endif
    
    // initialize first hidden window
    // NOTE: this is needed in order to estimate window decoration size
    glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);
    glutInitWindowPosition(INIT_WINDOW_X, INIT_WINDOW_Y);
    g_hidden_window = glutCreateWindow("SUMMON");
    glutDisplayFunc(Summon::SummonModule::FirstDisplay);
    glutReshapeFunc(Summon::SummonModule::FirstReshape);
    
    g_summon->Lock();

    // store summon thread ID
    g_summon->m_threadId = PyThread_get_thread_ident();


    
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
    
    // get command id
    PyObject *header = PyTuple_GET_ITEM(tup, 0);
    assert (ScmIntp(Py2Scm(header)));    
    int commandid = (int) PyInt_AsLong(header);

    // get command arguments
    Scm args = Py2Scm(PyTuple_GET_ITEM(tup, 1));

    // create command object
    ScriptCommand *cmd = (ScriptCommand*) 
                     g_commandRegistry.Create((CommandId) commandid);
    assert(cmd);

    // populate command and execute
    if (cmd->Setup(args)) {
        // execute command
        g_summon->ThreadSafeExecCommand(cmd);
        
        // handle return value
        PyObject *ret = Scm2Py(cmd->GetReturn());
        if (ret)
            Py_INCREF(ret);
        else {
            // set error
            PyErr_Format(PyExc_Exception, GetError());
            ClearError();
        }
        delete cmd;
        return ret;
    } else {
        // populating command failed, raise exception
        PyErr_Format(PyExc_Exception, "error processing command '%s'", 
                     cmd->GetName());
        delete cmd;
    }    
    
    Py_RETURN_NONE;
}



static PyObject *
MakeConstruct(PyObject *self, PyObject *args)
{
    long elmid;
    PyObject *lst;
    int ok = PyArg_ParseTuple(args, (char*) "lO", &elmid, &lst); // avoid compiler warning (python's fault)
        
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
    PyObject *addr = PyInt_FromLong(Element2Id(elm));
    return addr;
}


static PyObject *
DeleteConstruct(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = Id2Element(addr);
    
    elm->DecRef();
    
    // if element has a parent, then parent owns element and
    // element will be deleted by parent
    if (!elm->IsReferenced()) { // && elm->GetParent() == NULL) {
        //printf("delete: %p\n", elm);
        delete elm;
    }
    
    Py_RETURN_NONE;
}


static PyObject *
IncRefConstruct(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = Id2Element(addr);
    
    //printf("incref %p\n", elm);
    elm->IncRef();
    
    Py_RETURN_NONE;
}


static PyObject *
GetConstructChildren(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = Id2Element(addr);
    
    int len = elm->NumChildren() * 2;
    
    PyObject *children = PyTuple_New(len);
    int i = 0;
    for (Element::Iterator child=elm->Begin(); child!=elm->End(); child++) {
        PyTuple_SET_ITEM(children, i, PyInt_FromLong((*child)->GetSpecificId()));
        PyTuple_SET_ITEM(children, i+1, PyInt_FromLong(Element2Id(*child)));
        i+=2;
    }
    
    return children;
}


static PyObject *
GetConstructContents(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = Id2Element(addr);
    
    Scm contents = elm->GetContents();
    PyObject *pycontents = Scm2Py(contents);
    
    if (pycontents != NULL)
        Py_INCREF(pycontents);
    
    return pycontents;
}


static PyObject *
GetConstructParent(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = Id2Element(addr);
    
    PyObject *parent = PyTuple_New(2);
    PyTuple_SET_ITEM(parent, 0, PyInt_FromLong(elm->GetSpecificId()));
    PyTuple_SET_ITEM(parent, 1, PyInt_FromLong(Element2Id(elm->GetParent())));
    
    return parent;
}




// gateway function to python
PyMODINIT_FUNC
initsummon_core()
{
    // prepare the python environment for summon
    InitPython();
    
    // create Summon module (global singleton)
    g_summon = new Summon::SummonModule();    
    
    // initialize summon
    if (!g_summon->Init())
        printf("summon init error\n");
}
 
 
} // extern "C"
