/***************************************************************************
* SUMMON
* Matt Rasmussen
* Summon.cpp
*
* This file creates main python interface to the SUMMON module.  The SUMMON 
* module itself, it represented by a singleton class SummonModule.
*
*
* This file is part of SUMMON.
* 
* SUMMON is free software: you can redistribute it and/or modify
* it under the terms of the Lesser GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
* 
* SUMMON is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
* 
* You should have received a copy of the Lesser GNU General Public License
* along with SUMMON.  If not, see <http://www.gnu.org/licenses/>.
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
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>
#include <SDL/SDL_timer.h>


// summon headers
#include "common.h"
#include "SummonWindow.h"
#include "summon_scene_graph.h"


// constants
#define MODULE_NAME "summon_core"


#ifdef DEBUG_X
#include <X11/Xlib.h>

static int AbortOnXError(Display *display, XErrorEvent *theEvent)
{
    abort();
    return 0 ;
}
#endif


//=============================================================================
// python visible prototypes
extern "C" {

// functions directly visible in python
static PyObject *Exec(PyObject *self, PyObject *tup);
static PyObject *SummonMainLoop(PyObject *self, PyObject *tup);
static PyObject *SummonShutdown(PyObject *self, PyObject *tup);
static PyObject *GetTextWidth(PyObject *self, PyObject *args);
static PyObject *GetTextHeight(PyObject *self, PyObject *args);

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

    // make element
    {(char*) "make_element", MakeElement, METH_VARARGS, (char*) ""},

    // delete an element
    {(char*) "delete_element", DeleteElement, METH_VARARGS, (char*) ""},

    // reference an element
    {(char*) "incref_element", IncRefElement, METH_VARARGS, (char*) ""},

    // get the children of an element
    {(char*) "get_element_children", GetElementChildren, METH_VARARGS, (char*) ""},

    // get the contents of an element
    {(char*) "get_element_contents", GetElementContents, METH_VARARGS, (char*) ""},

    // get the parent of an element
    {(char*) "get_element_parent", GetElementParent, METH_VARARGS, (char*) ""},

    // get the width of a text string
    {(char*) "get_text_width", GetTextWidth, METH_VARARGS, (char*) ""},

    // get the height of a text string
    {(char*) "get_text_height", GetTextHeight, METH_VARARGS, (char*) ""},
    
    // cap the methods table with NULL method
    {NULL, NULL, 0, NULL}
};

} // extern "C"



//=============================================================================
namespace Summon {

using namespace std;


// global prototypes
class SummonModule;
static SummonModule *g_summon;  // global singleton instance of module


// NOTE: don't make this too small, some window managers don't allow windows
// to be opened with their left-top corner off of the desktop
static const int INIT_WINDOW_X = 100;
static const int INIT_WINDOW_Y = 100;

// NOTE: don't make this too small, some window mangers don't allow
// values less than 20 (or so) thus the window will never attain the
// correct size
const static int INIT_WINDOW_SIZE = 50;


// module state
typedef enum {
    SUMMON_STATE_UNINITIALIZED,
    SUMMON_STATE_RUNNING,
    SUMMON_STATE_STOPPING,
    SUMMON_STATE_STOPPED
} SummonState;


// glut callbacks
static void FirstDisplay();
static void FirstReshape(int width, int height);
static void FirstTimer(int value);
static void MenuCallback(int item);
static void Timer(int value);



class SummonModule : public CommandExecutor, public GlutViewListener
{
public:
    SummonModule() :
        m_state(SUMMON_STATE_UNINITIALIZED),
        m_nextModelId(1),
        m_nextWindowId(1),
        m_windowOffset(0,0),
        m_windowCloseCallback(Scm_EOL),
        m_nextMenuItem(1),
        
        m_waiting(false),
        m_cond(SDL_CreateCond()),
        m_condlock(SDL_CreateMutex()),

        m_timerCommand(NULL),
        m_timerDelay(0)

    {
    }
    
    virtual ~SummonModule()
    {
        // clean up SDL thread management 
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
        
        for (CommandAttr::Iterator i=g_scriptAttr.Begin();
             i != g_scriptAttr.End(); i++)
        {
            ScriptCommand* cmd = (ScriptCommand*) *i;
        
            // get command id
            string idstr = int2string(cmd->GetId());

            // create python name for command
            string name = cmd->GetName();
            string help = string("(") + string(cmd->GetUsage()) + 
                          ")\\n" + cmd->GetDescription();

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
            case CALL_PROC_COMMAND:
            case CALL_THREAD_PROC_COMMAND: {
                // execute a python function
                CallProcCommand *cmd = (CallProcCommand*) &command;
                
                if (cmd->defined) {
                    Scm ret = ScmApply(cmd->proc, cmd->args);
                    
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
                    BuildScm("dd", m_windowOffset.x, m_windowOffset.y));
                } break;
            
            case SET_WINDOW_CLOSE_CALLBACK_COMMAND: {
                // set the callback for window close events
                m_windowCloseCallback = ((SetWindowCloseCallbackCommand*)
                                         &command)->callback;
                
                } break;
            
            case SCREENSHOT_WINDOW_COMMAND: {
                ScreenshotWindowCommand *cmd = 
                    (ScreenshotWindowCommand *) &command;
                    
                SummonWindow *window = GetWindow(cmd->winid);
                if (!window)
                    Error("unknown window id '%d'", cmd->winid);
                
                ScreenshotWindow(window, cmd->filename);
                
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
                int menuid = glutCreateMenu(Summon::MenuCallback);
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
                    newelm = model->ReplaceElement(oldelm, ScmCar(newcode));
                } else {
                    newelm = elm->ReplaceChild(oldelm, ScmCar(newcode));
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
            
                    cmd->SetReturn(BuildScm("ffff", pos1.x, pos1.y, pos2.x, pos2.y));
                } else {
                    Error("element must be added to a model to find its bounding box");
                }
                
                } break;
            
            
            case SET_CONTENTS_COMMAND: {
                SetContentsCommand *cmd = (SetContentsCommand*) &command;
                Element *elm = Id2Element(cmd->groupid);
                SummonModel *model = GetModelOfElement(elm);                

                if (!elm->SetContents(cmd->code))
                    Error("Error setting contents");
                
                if (model)
                    model->Update(elm);
                    
                } break;
                
            //==============================================
            // do command routing
            default:
                
                
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
    void MenuCallback(int item)
    {
        PyGILState_STATE gstate = PyGILState_Ensure();
        
        // look up python function and call it
        Scm proc = m_menuItems[item];
        Scm ret = ScmApply(proc, Scm_EOL);
        
        //display exceptions
        if (Scm2Py(ret) == NULL)
            PyErr_Print();
        
        PyGILState_Release(gstate);    
    }
    
    // Callback from GLUT notifying us that the window is closed and is ready
    // to be deleted.  However, we cannot delete the window until one pump
    // of the GLUT event queue.  Thus, we place the window on a delete waiting
    // list, and delete the window in the GLUT timer function
    void ViewClose(GlutView *view)
    {    
        // GLUT is done working with the window
        // it is now safe to delete the window after this function returns
        SummonWindow *window = m_closeWaiting[view];
        
        // remove window from window list        
        m_closeWaiting.erase(view);
        
        // also make sure to remove window from set of open windows
        m_windows.erase(window->GetId());
        
        // add this window to the deletion waiting list
        m_deleteWaiting.push_back(window);
        
        // call the user-defined callback, if it exists
        if (m_windowCloseCallback != Scm_EOL) {
            // let python know window has been closed
            Scm ret = ScmApply(m_windowCloseCallback, 
                               BuildScm("d", window->GetId()));
                    
            if (Scm2Py(ret) == NULL)
                //display exceptions, return None
                PyErr_Print();
        }
    }
    
    bool ScreenshotWindow(SummonWindow *window, string filename)
    {
        Vertex2i winsize = window->GetView()->GetWindowSize();
        
        // allocate pixel data
        GLubyte *pixels = new GLubyte[winsize.x * winsize.y * 3];
        
        // read gl screen
        window->GetView()->CopyPixels(pixels);

        // create SDL surface
        Uint32 rmask, gmask, bmask, amask;
#       if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
#       else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
#       endif
        
        static const int bpp = 32;
        //SDL_Surface *s = SDL_SetVideoMode(winsize.x, winsize.y, bpp, 
        //    SDL_SWSURFACE);
        SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
            winsize.x, winsize.y, 
            bpp, rmask, gmask, bmask, amask);
        
        if (!surface) {
            Error("Internal error: cannot allocate SDL surface: %s",
                  SDL_GetError());
            delete [] pixels;
            return false;
        }
        
        SDL_LockSurface(surface);
        Uint32 *pdata = (Uint32 *) surface->pixels;
        
        // copy into SDL surface
        for (int y = 0; y < winsize.y; y++)
        {
            for (int x = 0; x < winsize.x; x++) {
                int p = (y * winsize.x + x) * 3;
                pdata[(winsize.y - 1 - y) * winsize.x + x] = 
                    SDL_MapRGBA(surface->format,
                                pixels[p + 0], pixels[p + 1], 
                                pixels[p + 2], 255);
            }
        }

        SDL_UnlockSurface(surface);

        // save the surface
        if (SDL_SaveBMP(surface, filename.c_str()) != 0) {
            Error("Error saving screenshot '%s'", filename.c_str());
            SDL_FreeSurface(surface);
            delete [] pixels;
            return false;
        }

        // cleanup
        SDL_FreeSurface(surface);
        delete [] pixels;
        
        return true;
    }
       
    
private:    
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
        
        // NOTE: ViewClose will be called-back when the close has been processed
        // by GLUT
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
    
    
    
    void ProcessWindowEvents()
    {
        DeleteClosedWindows();
        
        PyGILState_STATE gstate = PyGILState_Ensure();    
        for (WindowIter i=m_windows.begin(); i!=m_windows.end(); i++) {
            SummonView *view = (*i).second->GetView();
            
            // Take note of the new window positions
            view->UpdatePosition();
            
            view->ProcessRedisplay();
        }
        PyGILState_Release(gstate);
    }
    
    // Create a new model (kind = WORLD, SCREEN)
    int NewModel(ModelKind kind)
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
    

    
    
    //==================================================
    // synchronization and thread management    

    
    // Returns whether a command waiting to be executed
    inline bool IsCommandWaiting()
    {
        return m_queuedCommands.size() > 0;
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
        assert(SDL_CondBroadcast(m_cond) == 0);
    }

    
    // get number of milliseconds since program started
    inline int GetTime()
    { 
        return SDL_GetTicks(); 
    }

public:
    
    //====================================================
    // manage module state
    inline void Start()
    {
        assert(m_state == SUMMON_STATE_UNINITIALIZED);
        m_state = SUMMON_STATE_RUNNING;
    }
    
    inline bool IsRunning()
    {
        return m_state == SUMMON_STATE_RUNNING;
    }
    
    inline void Stop()
    {
        assert(m_state == SUMMON_STATE_RUNNING);
        m_state = SUMMON_STATE_STOPPING;
    }
    
    inline bool IsStopped()
    {
        return m_state == SUMMON_STATE_STOPPED;
    }
    
private:    
    inline void ConfirmStop()
    {
        //assert(m_state == SUMMON_STATE_STOPPING);
        m_state = SUMMON_STATE_STOPPED;        
    }
    

public:
    
    //=======================================================
    // GLUT startup and initial window creation

    // Starts GLUT processing
    void StartGlut()
    {
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
#       ifdef GLUT_ACTION_ON_WINDOW_CLOSE
            glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
#       endif

        // initialize first hidden window
        // NOTE: this is needed in order to estimate window decoration size.
        // make the window a size that we have to change later on (this is the
        // reason for the 2*INIT_WINDOW_SIZE).
        glutInitWindowSize(2*INIT_WINDOW_SIZE, 2*INIT_WINDOW_SIZE);
        glutInitWindowPosition(INIT_WINDOW_X, INIT_WINDOW_Y);
        m_initWindow = glutCreateWindow("SUMMON");
        glutDisplayFunc(Summon::FirstDisplay);
        glutReshapeFunc(Summon::FirstReshape);
        glutTimerFunc(100, Summon::FirstTimer, 0);
        
        // aquire the SUMMON lock for this thread
        // Lock();
        
        // store summon thread ID
        m_threadId = PyThread_get_thread_ident();


        // begin processing of GLUT events
        Py_BEGIN_ALLOW_THREADS
        glutMainLoop();
        Py_END_ALLOW_THREADS
        
    }

    // used to help initialize window decoration    
    void FirstReshape(int width, int height)
    {
        static int tries = 0;
        static const int maxtries = 10;
        
        // only try to get consistent window size about 10 times 
        // and then give up
        tries += 1;
        
        // if SUMMON is already initialized, then do not do any more 
        // window decoration processing
        if (!IsRunning()) {
            if (tries < maxtries && 
                (glutGet(GLUT_WINDOW_WIDTH) != INIT_WINDOW_SIZE ||
                 glutGet(GLUT_WINDOW_HEIGHT) != INIT_WINDOW_SIZE))
            {
                glutReshapeWindow(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);
                glutPositionWindow(INIT_WINDOW_X, INIT_WINDOW_Y);
            } else {
                // get window offset
                m_windowOffset.x = glutGet(GLUT_WINDOW_X) - INIT_WINDOW_X;
                m_windowOffset.y = glutGet(GLUT_WINDOW_Y) - INIT_WINDOW_Y;

                // offset is now consistent, start the real timer
                glutHideWindow();                
                Start();
                glutTimerFunc(0, Summon::Timer, 0);
            }
        }
    }
    
    
    void FirstDisplay()
    {
        static bool first = true;
        
        if (first) {
            first = false;
            glutReshapeWindow(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);
        }
    }
    
    // this timer checks in on initial window to make sure it was propperly
    // initialized
    void FirstTimer(int value)
    {
        if (!IsRunning()) {
            glutSetWindow(m_initWindow);
            FirstReshape(glutGet(GLUT_WINDOW_WIDTH),
                         glutGet(GLUT_WINDOW_HEIGHT));
            if (!IsRunning()) {
                glutTimerFunc(100, Summon::FirstTimer, 0);
            }
        }
    }
    
    
    
    //===========================================================
    // GLUT timer callback
    // This function executes periodically to process queued commands
    // and to handle window operations
    void Timer(int value)
    {
        static int delay = 0;
        static int delayTime = 0;
        
        // do nothing if python is not initialized        
        if (!Py_IsInitialized() && IsRunning())
            return;        

        if (!IsRunning()) {
            ConfirmStop();
        }

        // get python GIL
        m_gil = PyGILState_Ensure();
        
        // process window events
        ProcessWindowEvents();
        
        // process waiting commands commands
        if (ExecWaitingCommands())
            delay = 0;
        
        

        // process user-defined timer function
        SummonTimer();
        
        if (m_timerCommand) {
            // sleep up until timer goes off
            int remaining = m_timerDelay - GetTime();
            
            if (remaining < 10)
                delayTime = 0;
            else if (remaining/2 < 100)
                delayTime = remaining/2;
            else
                delayTime = 100;
        }

        // set the next timer
        if (IsRunning())
            glutTimerFunc(delayTime, Summon::Timer, 0);
        else
            ConfirmStop();
        
        if (delay < 10) {
            delay++;
            delayTime = 0;
        } else {
            delayTime = 10;
        }
        
        PyGILState_Release(m_gil);
    }
    
private:   

    // Set the user-defined timer command
    inline void SetTimerCommand(float delay, Command *command)
    {
        m_timerDelay = GetTime() + int(delay * 1000);
        m_timerCommand = command;
    }
 
    // execute the user-defined timer function
    inline void SummonTimer()
    {
        if (m_timerCommand && GetTime() > m_timerDelay) {
            Command *cmd = m_timerCommand;
            m_timerCommand = NULL;
            ExecCommand(*cmd);
            delete cmd;
        }
    }
    
    
    // Execute any commands waiting to be executed in the SUMMON/GLUT thread
    // This function is periodically called by the GLUT timer.
    // Returns true if command was executed
    inline bool ExecWaitingCommands()
    {
        bool nodelay;
    
        if (IsCommandWaiting()) {
            // graphics command is executed in this thread
            
            do {
                ScriptCommand *cmd = m_queuedCommands.front();
                m_queuedCommands.pop_front();
                ExecCommand(*cmd);
                
                // if command is owned by queue, delete it
                if (!cmd->NeedsReturn())
                    delete cmd;
            } while (m_queuedCommands.size() > 0);
            
            nodelay = true;
        } else
            nodelay = false;
            
        // wake up python thread if it is waiting for a graphic command to
        // complete
        if (m_waiting)
            NotifyExecOccurred();
        
        return nodelay;          
    }

public:    
    // Execute a command in a thread safe manner
    // This function is called from python code.  Most of the time this is 
    // call is from the non-GLUT thread, however if a GLUT-callback calls 
    // python code that then calls a SUMMON command, then this could also be
    // called in the GLUT thread.
    // Returns true if command has been queued
    inline bool ThreadSafeExecCommand(ScriptCommand *command)
    {
        // do nothing until summon is initialized, shouldn't happen anyway
        // NOTE: Maybe I could make this an assert instead
        if (!IsRunning())
            return false;    

        // determine which thread we are in

        int curThreadId = PyThread_get_thread_ident();
    
        if (curThreadId == m_threadId) {
            // execute command in this thread if we are in the summon/glut thread
            ExecCommand(*command);
        } else {
            // we are the python/non-glut thread, more care is needed
            // commands that manipulate OpenGL must be passed to the 
            // other thread
        
            if (command->HasAttr(&g_glAttr)) {
                // pass command to other thread
                m_queuedCommands.push_back(command);

                if (command->NeedsReturn()) {
                    m_waiting = true;

                    // wait for graphics thread to exec command
                    Py_BEGIN_ALLOW_THREADS
                    WaitForExec();
                    Py_END_ALLOW_THREADS

                    m_waiting = false;

                    // all commands on queue should be executed
                    assert(m_queuedCommands.size() == 0);

                    // command is still owned by caller
                    return false;
                } else {
                    // command is owned by queued
                    return true;
                }
            } else {
                // execute command in this thread
                ExecCommand(*command);
            }
        }
        
        return false;
    }

//======================================================================
private:    

    // the state of the SUMMON module indicating whether it is ready for 
    // processing commands from python
    SummonState m_state;
    
    
    // model data structures
    int m_nextModelId;  // next id number to issue
    typedef map<int, SummonModel*>::iterator ModelIter;
    map<int, SummonModel*> m_models;
    
    
    // window data structures
    int m_initWindow;   // initial hidden window id
    int m_nextWindowId; // next id number to issue
    typedef map<int, SummonWindow*>::iterator WindowIter;
    map<int, SummonWindow*> m_windows;
    Vertex2i m_windowOffset; // SUMMON window decoration size
    
    // waiting queues for closing and deleting a window
    map<GlutView*, SummonWindow*> m_closeWaiting;
    vector<SummonWindow*> m_deleteWaiting;

    // user-defined python function to call whenever a window closes    
    Scm m_windowCloseCallback;

    // menus
    int m_nextMenuItem;
    map<int, Scm> m_menuItems;
    
    
    // thread management
    bool m_waiting;
    int m_threadId;
    SDL_cond *m_cond;    
    SDL_mutex *m_condlock;    
    list<ScriptCommand*> m_queuedCommands;
    PyGILState_STATE m_gil;
    
    
    // user-defined time-delay command execution
    Command *m_timerCommand;    
    int m_timerDelay;
};


static void FirstDisplay()
{
    g_summon->FirstDisplay();
}

static void FirstReshape(int width, int height)
{    
    g_summon->FirstReshape(width, height);
}

static void FirstTimer(int value)
{
    g_summon->FirstTimer(value);
}


static void MenuCallback(int item)
{
    g_summon->MenuCallback(item);
}

static void Timer(int value)
{
    g_summon->Timer(value);
}

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
    
    g_summon->StartGlut();
    
    Py_RETURN_NONE;
}


// This function is called when python is shutting down (atexit)
// Gracefully wait for summon to finish its timers and callbacks.
static PyObject *
SummonShutdown(PyObject *self, PyObject *tup)
{
    if (g_summon) {
        // turn off initizalied flag
        g_summon->Stop();
        
        // wait for timer to stop
        while (!g_summon->IsStopped()) {
            SDL_Delay(10);
        }
    }
    Py_RETURN_NONE;
}



// All thread safe command executions from python pass through this gateway
// function
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
        bool queued = g_summon->ThreadSafeExecCommand(cmd);
        
        if (!queued) {

            // handle return value
            PyObject *ret = Scm2Py(cmd->GetReturn());
            if (ret && !HasError())
                Py_INCREF(ret);
            else {
                // set python exception
                SetException();
            }

            // clean up and return result
            delete cmd;
            return ret;
        }
    } else {
        // populating command failed, raise exception
        Error("error processing command '%s'", cmd->GetName());
        SetException();
        delete cmd;
    }    
    
    Py_RETURN_NONE;
}


static PyObject *GetTextWidth(PyObject *self, PyObject *args)
{
    void *font = GLUT_STROKE_MONO_ROMAN;
    
    //long font2 = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    const unsigned char *text = (const unsigned char *) 
        PyString_AS_STRING(PyTuple_GET_ITEM(args, 1));
    
    return PyFloat_FromDouble(getTextWidth(font, text));
}

static PyObject *GetTextHeight(PyObject *self, PyObject *args)
{
    void *font = GLUT_STROKE_MONO_ROMAN;
    return PyFloat_FromDouble(getTextHeight(font));
}



// Module initialization for python
PyMODINIT_FUNC
initsummon_core()
{
    // prepare the python environment for summon
    InitPython();

#   ifdef DEBUG_X
        // catch and die on X errors
        XSetErrorHandler(AbortOnXError);
#   endif
    
    // create Summon module (global singleton)
    g_summon = new Summon::SummonModule();    
    
    // initialize summon
    if (!g_summon->Init())
        printf("summon init error\n");
}
 
 
} // extern "C"
