/***************************************************************************
* Vistools
* Matt Rasmussen
* Summon.cpp
*
***************************************************************************/

#include "first.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <list>
#include <map>
#include <vector>
#include <GL/glut.h>
#include <GL/gl.h>


#include "common.h"
#include "DrawWindow.h"
#include "ScriptApp.h"
#include "ScriptTerminal.h"

#define APP_NAME "summon"
#define MODULE_NAME "summon"
#define MODULELIB_NAME "summonlib"
#define VERSION_INFO "\
-----------------------------------------------------------------------------\n\
                                   SUMMON 1.0\n\
                     Large Scale Programmable Visualization\n\
                                 Matt Rasmussen \n\
                             (http://mit.edu/rasmus)\n\
                                 Copyright 2005\n\
-----------------------------------------------------------------------------\n"

namespace Vistools {

using namespace std;




class Summon : public ScriptApp
{
public:
    Summon() :
        ScriptApp(APP_NAME),
        m_window(NULL),
        m_model(NULL),
        m_active(false),
        m_nextWindowId(1),
        m_nextModelId(1)    
    {
    }
    
    virtual ~Summon()
    {
    }
        
    
    virtual void ExecCommand(Command &command) {
        switch (command.GetId()) {
            case CALL_PROC_COMMAND: {
                CallProcCommand *cmd = (CallProcCommand*) &command;
                
                if (cmd->defined) {
                    Scm proc = cmd->GetScmProc();
                    cmd->SetReturn(ScmApply(proc, Scm_EOL));
                }
                
                } break;
            
            case GET_WINDOWS_COMMAND: {
                Scm lst = Scm_EOL;
                for (WindowIter i=m_windows.begin(); i!=m_windows.end(); i++) {
                    lst = ScmCons(Int2Scm((*i).first), lst);
                }
                ((ScriptCommand*) &command)->SetReturn(lst);
                } break;
            
            case GET_WINDOW_COMMAND: {
                if (m_window) {
                    ((ScriptCommand*)
                        &command)->SetReturn(Int2Scm(m_window->GetId()));
                }
                } break;

            case NEW_WINDOW_COMMAND:
                ((ScriptCommand*) &command)->SetReturn(Int2Scm(NewWindow()));
                break;
                
            case SET_WINDOW_COMMAND: {
                int windowid = ((SetWindowCommand*)&command)->windowid;
                DrawWindow *window = GetWindow(windowid);
                
                if (window) {
                    m_window = window;
                } else {
                    Error("cannot find window %d", windowid);
                }
                } break;
            
            case CLOSE_WINDOW_COMMAND: {
                int windowid = ((CloseWindowCommand*)&command)->windowid;
                DrawWindow *window = GetWindow(windowid);
                
                // default window
                if (windowid == -1) {
                    window = m_window;
                }
                
                if (window) {
                    // close old window
                    m_windows.erase(windowid);
                    delete window;
                    
                    if (window == m_window) {
                        // choose new window or null
                        if (m_windows.begin() != m_windows.end()) {
                            m_window = (*m_windows.begin()).second;
                        } else {
                            m_window = NULL;
                        }
                    }
                    
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
            
            case GET_MODEL_COMMAND: {
                int windowid = ((GetModelCommand*)&command)->windowid;
                string kind = ((GetModelCommand*)&command)->kind;
                DrawWindow *window = GetWindow(windowid);
                DrawModel *model = NULL;
            
                if (window) {
                    if (kind == "world")
                        model = window->GetWorldModel();
                    else if (kind == "screen")
                        model = window->GetScreenModel();
                    else
                        Error("unknown model kind '%s'", kind.c_str());
                    
                    if (model)
                        ((ScriptCommand*)
                            &command)->SetReturn(Int2Scm(model->GetId()));
                } else {
                    Error("cannot find window %d", windowid);
                }
                } break;
                
            case NEW_MODEL_COMMAND:
                ((ScriptCommand*) &command)->SetReturn(Int2Scm(NewModel()));
                break;
                
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
            
            case SET_MODEL_COMMAND: {
                int modelid  = ((SetModelCommand*)&command)->modelid;
                DrawModel *model = GetModel(modelid);
                
                // ensure model exists
                if (model) {                       
                    m_model = model;
                } else {
                    Error("cannot find model %d", modelid);
                }
                
                
                } break;
            
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
            
            case VERSION_COMMAND: {
                fprintf(stderr, VERSION_INFO);
                } break;
            
            case TIMER_CALL_COMMAND: {
                TimerCallCommand *cmd = (TimerCallCommand*) &command;
                SetTimerCommand(cmd->delay, new CallProcCommand(cmd->proc));
                } break;
                
            default:
                // ensure a window exists for commands
                if (!m_window) {
                    MakeDefaultWindowModel();
                }
            
                // do command routing
                if (g_globalAttr.Has(&command)) {
                    // global
                    App::ExecCommand(command);
                
                } else if (g_modelAttr.Has(&command)) {
                    // model
                    if (!m_model) {
                        m_model = m_window->GetWorldModel();
                    }
                    m_model->ExecCommand(command);
                
                } else if (g_viewAttr.Has(&command)) {
                    // view
                    m_window->GetView()->ExecCommand(command);
                    
                } else {
                    // controller and default
                    m_window->GetController()->ExecCommand(command);
                }
        }
    }
    
    DrawWindow *GetWindow(int id)
    {
        if (m_windows.find(id) != m_windows.end())
            return m_windows[id];
        else
            return NULL;
    }
    
    DrawModel *GetModel(int id)
    {
        if (m_models.find(id) != m_models.end())
            return m_models[id];
        else
            return NULL;
    }

    int NewWindow()
    {
        int id = m_nextWindowId;
        m_windows[id] = new DrawWindow(id, this);
        m_nextWindowId++;
        if (m_active)
            m_windows[id]->SetActive();
        return id;
    }
    
    int NewModel()
    {
        int id = m_nextModelId;
        m_models[id] = new DrawModel(id);
        m_nextModelId++;
        return id;
    }
    
    void MakeDefaultWindowModel()
    {
        int winId = NewWindow();
        int modelId = NewModel();
        m_window = m_windows[winId];
        m_window->SetWorldModel(m_models[modelId]);
    }
    
    int ParseArgs(int argc, char **argv)
    {
        if (argc > 1) {
            if (!ExecFile(string(argv[1])))
                return 1;
        }
        return 0;
    }
    
    virtual bool ExecFile(string filename)
    {
        ScmEvalFile(filename.c_str());
        return true;
    }
    
    
    // TODO: make less ugly
    void InitDrawEnv()
    {
#       if (VISTOOLS_SCRIPT == VISTOOLS_SCHEME)
#       endif

#       if (VISTOOLS_SCRIPT == VISTOOLS_PYTHON)

            // install group id generator
            ScmEvalStr("import " MODULE_NAME);
            ScmEvalStr(
MODULE_NAME ".groupid = 1 \n\
def __new_groupid(): \n\
   "MODULE_NAME".groupid = "MODULE_NAME".groupid + 1 \n\
   return "MODULE_NAME".groupid\n\
"MODULE_NAME".new_groupid = __new_groupid\n");


            // register constructs
            for (CommandAttr::Iterator i=g_constructAttr.Begin(); 
                 i != g_constructAttr.End(); i++)
            {
                Construct *cmd = (Construct*) *i;
                string str;

                string name = string(cmd->GetName());
                string id = int2string(cmd->GetId());

                // ensure command name does not have dashes
                for (int j=0; j<name.size(); j++) {
                    if (name[j] == '-')
                        name[j] = '_';
                }

                
                if (cmd->GetId() == GROUP_CONSTRUCT) {
                    str = string("") + "\
def __group(* args): return (" + id + ", __new_groupid()) + args\n\
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
def __is_" + name + "(obj): return (obj[0] == " + id + ")\n\
def __" + name + "_contents(obj): return obj[1:]\n\
"MODULE_NAME"." + name + " = __" + name + "\n\
"MODULE_NAME".is_" + name + " = __is_" + name + "\n\
"MODULE_NAME"." + name + "_contents = __" + name + "_contents\n\
";
                }

                ScmEvalStr(str.c_str());

                GetTerminal()->AddToWordCompletion(name.c_str());
            }
#       endif
    }
    
    
    bool LoadStdlib()
    {
#       if (VISTOOLS_SCRIPT == VISTOOLS_SCHEME)
#       endif
#       if (VISTOOLS_SCRIPT == VISTOOLS_PYTHON)
            ScmEvalStr("import "MODULELIB_NAME".summon_config");
#       endif
        return true;
    }
    
    void Usage()
    {
        fprintf(stderr, "usage: %s %s\n", m_appName.c_str(), m_usage.c_str());
    }
    
    virtual int Begin(int argc, char **argv)
    {
        SetUsage("[python file] [options passed to python]");
    
        // init commands
        drawCommandsInit();
        vector<StringCommand*> cmds = GetAllStringCommands();
        
        // remove constructs from cmds 
        // they aren't processed by SchemeTerminal because we process them
        {
            int j=0;
            for (int i=0; i<cmds.size(); i++) {
                cmds[j] = cmds[i];
                if (!g_constructAttr.Has(cmds[i]->GetId()) &&
                    strlen(cmds[i]->GetName()) > 0)
                {
                    j++;
                }
            }
            while (j<cmds.size()) {
                cmds.pop_back();
            }
        }
        
        
        // setup terminal
        SetTerminal(new ScriptTerminal());
        m_term->SetPrompt(APP_NAME"> ");
        SetupCommands(cmds);
        m_stringCmds = GetAllStringCommands();


        // setup scripting engine
        GetEngine()->SetCommands(cmds);
        GetEngine()->SetListener(this);
#       if (VISTOOLS_SCRIPT == VISTOOLS_PYTHON)
            GetEngine()->SetModuleName(MODULE_NAME);
#       endif
        GetEngine()->Init();
        InitDrawEnv();

        if (!LoadStdlib()) {
            return 1;
        }

        // parse arguments
        if (ParseArgs(argc, argv) != 0) {
            return 1;
        }
        
        // activate all windows
        m_active = true;
        for (WindowIter i=m_windows.begin(); i!=m_windows.end(); i++) {
            (*i).second->SetActive();
        }

        // start terminal thread
        StartTerminal();
        
        return 0;
        
    }

    
    bool m_active;

    // current window and model
    DrawWindow *m_window;
    DrawModel *m_model;
    
    // all windows and models
    typedef map<int, DrawWindow*>::iterator WindowIter;
    typedef map<int, DrawModel*>::iterator ModelIter;
    map<int, DrawWindow*> m_windows;
    map<int, DrawModel*> m_models;
    
    // indexes
    int m_nextWindowId;
    int m_nextModelId;
};

}


using namespace Vistools;

#if (VISTOOLS_SCRIPT == VISTOOLS_SCHEME)
#endif


#if (VISTOOLS_SCRIPT == VISTOOLS_PYTHON)

int main(int argc, char **argv)
{
    // init glut
    glutInit(&argc, argv);

    // init python
    Py_SetProgramName(argv[0]);
    Py_Initialize();
    PySys_SetArgv(argc, argv);
    InitPython();
    
        
    // create vismatrix app
    Summon *summon = new Summon();
    SetApp(summon);
    
    // run the actual vismatrix app
    if (summon->Begin(argc, argv) == 0) {
        // start main loop
        glutMainLoop();
    } else {
        // destroy python
        DestroyPython();
        Py_Finalize();
    
        Shutdown(1);
    }
    
    // should never get here
    assert(0);
}

#endif

