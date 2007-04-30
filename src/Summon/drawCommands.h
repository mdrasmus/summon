/***************************************************************************
* Vistools
* Matt Rasmussen
* drawCommands.h
*
***************************************************************************/

#ifndef DRAW_COMMANDS_H
#define DRAW_COMMANDS_H

#include "Script.h"
#include <list>
#include <set>
#include "common.h"
#include "Color.h"
#include "glutInputs.h"
#include "glut2DCommands.h"
#include "ScriptCommand.h"
#include "types.h"



namespace Vistools
{

using namespace std;



// command ids
enum {
    VISDRAW_COMMANDS_BEGIN = GLUT_COMMANDS_END + 1,
    
    // global commands
    GET_WINDOWS_COMMAND,
    GET_WINDOW_COMMAND,
    NEW_WINDOW_COMMAND,
    SET_WINDOW_COMMAND,
    SET_WINDOW_NAME_COMMAND,
    GET_WINDOW_NAME_COMMAND,
    CLOSE_WINDOW_COMMAND,
    GET_MODELS_COMMAND,
    GET_MODEL_COMMAND,
    NEW_MODEL_COMMAND,
    SET_MODEL_COMMAND,
    ASSIGN_MODEL_COMMAND,
    DEL_MODEL_COMMAND,
    TIMER_CALL_COMMAND,
    REDRAW_CALL_COMMAND,
    VERSION_COMMAND,
    QUIT_COMMAND,
    
    // model commands
    ADD_GROUP_COMMAND,
    INSERT_GROUP_COMMAND,
    REMOVE_GROUP_COMMAND,
    REPLACE_GROUP_COMMAND,
    CLEAR_GROUPS_COMMAND,
    SHOW_GROUP_COMMAND,
    GET_GROUP_COMMAND,    
    GET_ROOT_ID_COMMAND,
    
    // view commands
    HOME_COMMAND,
    SET_BGCOLOR_COMMAND,
    GET_BGCOLOR_COMMAND,
    SET_VISIBLE_COMMAND,
    GET_VISIBLE_COMMAND,    
    SET_WINDOW_SIZE_COMMAND,
    GET_WINDOW_SIZE_COMMAND,
    SET_ANTIALIAS_COMMAND,
    SHOW_CROSSHAIR_COMMAND,
    SET_CROSSHAIR_COLOR_COMMAND,
    MODEL_CHANGED_COMMAND,
    
    // controller commands
    SET_BINDING_COMMAND,
    CLEAR_BINDING_COMMAND,
    CLEAR_ALL_BINDINGS_COMMAND,
    HOTSPOT_CLICK_COMMAND,
    GET_MOUSE_POS_COMMAND,
    
    // constructs
    
    // structure
    GROUP_CONSTRUCT,
    DYNAMIC_GROUP_CONSTRUCT,
    HOTSPOT_CONSTRUCT,
    
    // graphics
    POINTS_CONSTRUCT,
    LINES_CONSTRUCT,
    LINE_STRIP_CONSTRUCT,
    TRIANGLES_CONSTRUCT,
    TRIANGLE_STRIP_CONSTRUCT,
    TRIANGLE_FAN_CONSTRUCT,
    QUADS_CONSTRUCT,
    QUAD_STRIP_CONSTRUCT,
    POLYGON_CONSTRUCT,
    TEXT_CONSTRUCT,
    TEXT_SCALE_CONSTRUCT,
    TEXT_CLIP_CONSTRUCT,
    
    // primitives
    VERTICES_CONSTRUCT,
    COLOR_CONSTRUCT,
    
    // transforms
    TRANSFORM_CONSTRUCT, // internal use only
    TRANSLATE_CONSTRUCT,
    ROTATE_CONSTRUCT,
    SCALE_CONSTRUCT,
    FLIP_CONSTRUCT,
    NOZOOM_CONSTRUCT,
    SIDE_ALIGN_CONSTRUCT,
    
    // inputs
    INPUT_KEY_CONSTRUCT,
    INPUT_CLICK_CONSTRUCT,
    INPUT_MOTION_CONSTRUCT,
    
    // misc
    CALL_PROC_COMMAND,    
        
    VISDRAW_COMMANDS_END
};




void drawCommandsInit();

// common parsing functions
bool ParseScm(string errorMsg, Scm lst, const char *fmt, ...);
bool ParseCommand(Scm procScm, Command **command);
bool ParseInput(Scm lst, Input **input);
bool ParseKeyInput(Scm lst, Input **input);
bool ParseMouse(bool isClick, Scm lst, Input **input);
bool ParseMod(Scm lst, int *mod);


// attributes
extern CommandAttr g_modelAttr;
extern CommandAttr g_viewAttr;
extern CommandAttr g_controllerAttr;
extern CommandAttr g_globalAttr;
extern CommandAttr g_glAttr;




// constructs
class Construct : public StringCommand
{
public:
};

extern CommandAttr g_constructAttr;

#define RegisterConstruct(cmd) \
    RegisterStringCommand(cmd) \
    AddAttr(g_constructAttr)


#define RegisterGlobalCommand(cmd) \
    RegisterStringCommand(cmd) \
    AddAttr(g_globalAttr)


// -----------------------------------------------------------------------------
// global commands
// 

class GetWindowsCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new GetWindowsCommand(); }
    virtual int GetId() { return GET_WINDOWS_COMMAND; }

    virtual const char *GetName() { return "get_windows"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "gets a list of ids for all open windows"; }
};

class GetWindowCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new GetWindowCommand(); }
    virtual int GetId() { return GET_WINDOW_COMMAND; }

    virtual const char *GetName() { return "get_window"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "gets the id of the current window"; }
};

class NewWindowCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new NewWindowCommand(); }
    virtual int GetId() { return NEW_WINDOW_COMMAND; }

    virtual const char *GetName() { return "new_window"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "creates a new window and returns its id"; }
};

class SetWindowCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new SetWindowCommand(); }
    virtual int GetId() { return SET_WINDOW_COMMAND; }

    virtual const char *GetName() { return "set_window"; }
    virtual const char *GetUsage() { return "id"; }
    virtual const char *GetDescription() 
    { return "sets the current window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "d", &windowid);
    }
    
    int windowid;
};


class SetWindowNameCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new SetWindowNameCommand(); }
    virtual int GetId() { return SET_WINDOW_NAME_COMMAND; }

    virtual const char *GetName() { return "set_window_name"; }
    virtual const char *GetUsage() { return "id name"; }
    virtual const char *GetDescription() 
    { return "sets the name of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "ds", &windowid, &name);
    }
    
    int windowid;
    string name;
};


class GetWindowNameCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new GetWindowNameCommand(); }
    virtual int GetId() { return GET_WINDOW_NAME_COMMAND; }

    virtual const char *GetName() { return "get_window_name"; }
    virtual const char *GetUsage() { return "id"; }
    virtual const char *GetDescription() 
    { return "get the name of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "d", &windowid);
    }
    
    int windowid;
};

class CloseWindowCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new CloseWindowCommand(); }
    virtual int GetId() { return CLOSE_WINDOW_COMMAND; }

    virtual const char *GetName() { return "close_window"; }
    virtual const char *GetUsage() { return "[id]"; }
    virtual const char *GetDescription() 
    { return "closes a window"; }
    
    virtual bool Setup(Scm lst)
    {
        if (ScmLength(lst) > 0)
            return ParseScm(ErrorHelp(), lst, "d", &windowid);
        else {
            windowid = -1;
            return true;
        }
    }
    
    int windowid;
};


class GetModelsCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new GetModelsCommand(); }
    virtual int GetId() { return GET_MODELS_COMMAND; }

    virtual const char *GetName() { return "get_models"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "gets a list of ids for all models"; }
};

class GetModelCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new GetModelCommand(); }
    virtual int GetId() { return GET_MODEL_COMMAND; }

    virtual const char *GetName() { return "get_model"; }
    virtual const char *GetUsage() { return "windowid, ['world'|'screen']"; }
    virtual const char *GetDescription() 
    { return "gets the model id of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "ds", &windowid, &kind);
    }
    
    int windowid;
    string kind;
};

class NewModelCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new NewModelCommand(); }
    virtual int GetId() { return NEW_MODEL_COMMAND; }

    virtual const char *GetName() { return "new_model"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "creates a new model and returns its id"; }
};

class SetModelCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new SetModelCommand(); }
    virtual int GetId() { return SET_MODEL_COMMAND; }

    virtual const char *GetName() { return "set_model"; }
    virtual const char *GetUsage() 
    { return "modelid"; }
    virtual const char *GetDescription() 
    { return "sets the current model"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "d", &modelid);
    }
    
    int modelid;
};

class AssignModelCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new AssignModelCommand(); }
    virtual int GetId() { return ASSIGN_MODEL_COMMAND; }

    virtual const char *GetName() { return "assign_model"; }
    virtual const char *GetUsage() 
    { return "windowid, 'world'|'screen', modelid"; }
    virtual const char *GetDescription() 
    { return "assigns a model to a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "dsd", &windowid, &kind, &modelid);
    }
    
    int windowid;
    string kind;
    int modelid;
};


class DelModelCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new DelModelCommand(); }
    virtual int GetId() { return DEL_MODEL_COMMAND; }

    virtual const char *GetName() { return "del_model"; }
    virtual const char *GetUsage() 
    { return "modelid"; }
    virtual const char *GetDescription() 
    { return "deletes a model"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "d", &modelid);
    }
    
    int modelid;
};


class TimerCallCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new TimerCallCommand(); }
    virtual int GetId() { return TIMER_CALL_COMMAND; }

    virtual const char *GetName() { return "timer_call"; }
    virtual const char *GetUsage() 
    { return "delay, func"; }
    virtual const char *GetDescription() 
    { return "calls a function 'func' after a delay in seconds"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "fp", &delay, &proc);
    }
    
    float delay;
    Scm proc;
};


class RedrawCallCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new RedrawCallCommand(); }
    virtual int GetId() { return REDRAW_CALL_COMMAND; }

    virtual const char *GetName() { return "redraw_call"; }
    virtual const char *GetUsage() 
    { return "func"; }
    virtual const char *GetDescription() 
    { return "calls function 'func' on every redraw"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "p", &proc);
    }
    
    Scm proc;
};


class VersionCommand : public ScriptCommand
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


class QuitCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new QuitCommand(); }
    virtual int GetId() { return QUIT_COMMAND; }

    virtual const char *GetOptionName() { return ""; }
    virtual const char *GetName() { return "quit"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "quits summon"; }
};



// -----------------------------------------------------------------------------
// model commands
//


class ModelCommand : public ScriptCommand
{
public:
    int modelid;
};


class AddGroupCommand : public ModelCommand
{
public:
    virtual Command* Create() { return new AddGroupCommand(); }
    virtual int GetId() { return ADD_GROUP_COMMAND; }

    virtual const char *GetName() { return "add_group"; }
    virtual const char *GetUsage() { return "modelid, group"; }
    virtual const char *GetDescription() 
    { return "adds drawing groups to the current model"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "dc", &modelid, &code);
    }
    
    Scm code;
};

class InsertGroupCommand : public ModelCommand
{
public:
    virtual Command* Create() { return new InsertGroupCommand(); }
    virtual int GetId() { return INSERT_GROUP_COMMAND; }

    virtual const char *GetName() { return "insert_group"; }
    virtual const char *GetUsage() { return "modelid, groupid, group"; }
    virtual const char *GetDescription() 
    { return "inserts drawing groups under an existing group"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "ddc", &modelid, &groupid, &code);
    }
    
    int groupid;
    Scm code;
};


class RemoveGroupCommand : public ModelCommand
{
public:
    virtual Command* Create() { return new RemoveGroupCommand(); }
    virtual int GetId() { return REMOVE_GROUP_COMMAND; }

    virtual const char *GetName() { return "remove_group"; }
    virtual const char *GetUsage() { return "modelid, *groups"; }
    virtual const char *GetDescription() 
    { return "removes drawing groups from the current display"; }
    
    virtual bool Setup(Scm lst)
    {
        // parse modelid
        if (ScmConsp(lst) && ScmIntp(ScmCar(lst))) {
            modelid = Scm2Int(ScmCar(lst));
            lst = ScmCdr(lst);
        } else {
            return false;
        }
    
        // parse group ids
        for (; ScmConsp(lst); lst = ScmCdr(lst)) {
            if (ScmIntp(ScmCar(lst))) {
                groupids.push_back(Scm2Int(ScmCar(lst)));
            } else {
                Error("Can only remove with group id");
                return false;
            }
        }
        
        return true;
    }
    
    vector<int> groupids;
};


class ReplaceGroupCommand : public ModelCommand
{
public:
    virtual Command* Create() { return new ReplaceGroupCommand(); }
    virtual int GetId() { return REPLACE_GROUP_COMMAND; }

    virtual const char *GetName() { return "replace_group"; }
    virtual const char *GetUsage() { return "modelid, groupid, group"; }
    virtual const char *GetDescription() 
    { return "replaces a drawing group on the current display"; }
    
    virtual bool Setup(Scm lst)
    {
        if (ParseScm(ErrorHelp(), lst, "dd", &modelid, &groupid)) {
            code = ScmCddr(lst);
            return true;
        } else {
            return false;
        }
    }
    
    int groupid;
    Scm code;
};


class ClearGroupsCommand : public ModelCommand
{
public:
    virtual Command* Create() { return new ClearGroupsCommand(); }
    virtual int GetId() { return CLEAR_GROUPS_COMMAND; }

    virtual const char *GetName() { return "clear_groups"; }
    virtual const char *GetUsage() { return "modelid"; }
    virtual const char *GetDescription() 
    { return "removes all drawing groups from the current display"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "d", &modelid);
    }
};


class ShowGroupCommand : public ModelCommand
{
public:
    virtual Command* Create() { return new ShowGroupCommand(); }
    virtual int GetId() { return SHOW_GROUP_COMMAND; }

    virtual const char *GetName() { return "show_group"; }
    virtual const char *GetUsage() { return "modelid, groupid, True|False"; }
    virtual const char *GetDescription() 
    { return "sets the visibilty of a group"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "ddb", &modelid, &groupid, &visible);
    }
    
    int groupid;
    bool visible;
};


class GetGroupCommand : public ModelCommand
{
public:
    virtual Command* Create() { return new GetGroupCommand(); }
    virtual int GetId() { return GET_GROUP_COMMAND; }

    virtual const char *GetName() { return "get_group"; }
    virtual const char *GetUsage() { return "modelid, groupid"; }
    virtual const char *GetDescription() 
    { return "creates a tuple object that represents a group"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "dd", &modelid, &groupid);
    }
    
    int groupid;
};


class GetRootIdCommand : public ModelCommand
{
public:
    virtual Command* Create() { return new GetRootIdCommand(); }
    virtual int GetId() { return GET_ROOT_ID_COMMAND; }

    virtual const char *GetName() { return "get_root_id"; }
    virtual const char *GetUsage() { return "modelid"; }
    virtual const char *GetDescription() 
    { return "gets the group id of the root group"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "d", &modelid);
    }
};



// -----------------------------------------------------------------------------
// view commands
//

class HomeCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new HomeCommand(); }
    virtual int GetId() { return HOME_COMMAND; }

    virtual const char *GetName() { return "home"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "adjust view to show all graphics"; }
};


class SetBgColorCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new SetBgColorCommand(); }
    virtual int GetId() { return SET_BGCOLOR_COMMAND; }

    virtual const char *GetName() { return "set_bgcolor"; }
    virtual const char *GetUsage() { return "red, green, blue"; }
    virtual const char *GetDescription() 
    { return "sets background color"; }
    
    virtual bool Setup(Scm lst)
    {
        float r, g, b;
        
        if (ParseScm(ErrorHelp(), lst, "fff", &r, &g, &b)) {
            color = Color(r, g, b);
            return true;
        } else
            return false;
    }
    
    Color color;
};


class GetBgColorCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new GetBgColorCommand(); }
    virtual int GetId() { return GET_BGCOLOR_COMMAND; }

    virtual const char *GetName() { return "get_bgcolor"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "gets background color"; }
};


class SetVisibleCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new SetVisibleCommand(); }
    virtual int GetId() { return SET_VISIBLE_COMMAND; }

    virtual const char *GetName() { return "set_visible"; }
    virtual const char *GetUsage() { return "x1, y1, x2, y2"; }
    virtual const char *GetDescription() 
    { return "change display to contain region (x1,y1)-(x2,y2)"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "ffff", 
                        &data[0], &data[1], &data[2], &data[3]);
    }
    
    float data[4];
};


class GetVisibleCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new GetVisibleCommand(); }
    virtual int GetId() { return GET_VISIBLE_COMMAND; }

    virtual const char *GetName() { return "get_visible"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "gets visible bounding box"; }
};


class SetWindowSizeCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new SetWindowSizeCommand(); }
    virtual int GetId() { return SET_WINDOW_SIZE_COMMAND; }

    virtual const char *GetName() { return "set_window_size"; }
    virtual const char *GetUsage() { return "x, y"; }
    virtual const char *GetDescription() 
    { return "sets current window's size"; }
    
    bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "dd", &width, &height);
    }
    
    int width;
    int height;
};


class GetWindowSizeCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new GetWindowSizeCommand(); }
    virtual int GetId() { return GET_WINDOW_SIZE_COMMAND; }

    virtual const char *GetName() { return "get_window_size"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "gets current window's size"; }
};


class SetAntialiasCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new SetAntialiasCommand(); }
    virtual int GetId() { return SET_ANTIALIAS_COMMAND; }

    virtual const char *GetName() { return "set_antialias"; }
    virtual const char *GetUsage() { return "True|False"; }
    virtual const char *GetDescription() 
    { return "sets anti-aliasing status"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "b", &enabled);
    }
    
    bool enabled;
};


class ShowCrosshairCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new ShowCrosshairCommand(); }
    virtual int GetId() { return SHOW_CROSSHAIR_COMMAND; }

    virtual const char *GetName() { return "show_crosshair"; }
    virtual const char *GetUsage() { return "True|False"; }
    virtual const char *GetDescription() 
    { return "shows and hides the mouse crosshair"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "b", &enabled);
    }
    
    bool enabled;
};


class SetCrosshairColorCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new SetCrosshairColorCommand(); }
    virtual int GetId() { return SET_CROSSHAIR_COLOR_COMMAND; }

    virtual const char *GetName() { return "set_crosshair_color"; }
    virtual const char *GetUsage() { return "red, green, blue[, alpha]"; }
    virtual const char *GetDescription() 
    { return "sets mouse crosshair color"; }
    
    virtual bool Setup(Scm lst)
    {
        float r, g, b, a;
        
        if (ScmLength(lst) == 3 && 
            ParseScm(ErrorHelp(), lst, "fff", &r, &g, &b))
        {
            color = Color(r, g, b, 1);
            return true;
        } else if (ScmLength(lst) == 4 && 
                   ParseScm(ErrorHelp(), lst, "ffff", &r, &g, &b, &a))
        {
            color = Color(r, g, b, a);
            return true;
        }
            return false;
    }
    
    Color color;
};

// forward declaration
class Group;
class ModelChangedCommand : public Command
{
public:
    virtual Command* Create() { return new ModelChangedCommand(); }
    virtual int GetId() { return MODEL_CHANGED_COMMAND; }
    
    list<Group*> changedGroups;
};


//----------------------------------------------------------------------------
// controller commands
//

class SetBindingCommand : public ScriptCommand
{
public:
    SetBindingCommand() : 
        input(NULL), 
        command(NULL)
    {}

    virtual Command* Create() { return new SetBindingCommand(); }
    virtual int GetId() { return SET_BINDING_COMMAND; }

    virtual const char *GetName() { return "set_binding"; }
    virtual const char *GetUsage() { return "input, proc|command_name"; }
    virtual const char *GetDescription() 
    { return "bind an input to a command or procedure"; }
    
    virtual bool Setup(Scm lst)
    {
        // ensure basic structure of lst
        if (!ScmConsp(lst) || !ScmConsp(ScmCdr(lst))) {
            Error("must specify an input and command/procedure");
            return false;
        }
        
        // ensure member vars are NULL before parsing
        input = NULL;
        command = NULL;
        
        // parse input and command from lst
        if (ParseInput(ScmCar(lst), &input) &&
            ParseCommand(ScmCadr(lst), &command))
        {
            return true;
        } else {
            if (input)
                delete input;
            return false;
        }
    }
        
    Input *input;
    Command *command;
};


class ClearBindingCommand : public ScriptCommand
{
public:
    ClearBindingCommand() : 
        input(NULL)
    {}

    virtual Command* Create() { return new ClearBindingCommand(); }
    virtual int GetId() { return CLEAR_BINDING_COMMAND; }

    virtual const char *GetName() { return "clear_binding"; }
    virtual const char *GetUsage() { return "input"; }
    virtual const char *GetDescription() 
    { return "clear all bindings for an input"; }
    
    virtual bool Setup(Scm lst)
    {
        // ensure member vars are NULL before parsing
        input = NULL;
        
        // parse input and command from lst
        return ParseInput(ScmCar(lst), &input);
    }
    
    Input *input;
};


class ClearAllBindingsCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new ClearAllBindingsCommand(); }
    virtual int GetId() { return CLEAR_ALL_BINDINGS_COMMAND; }

    virtual const char *GetName() { return "clear_all_bindings"; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() 
    { return "clear all bindings for all input"; }    
};


class HotspotClickCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new HotspotClickCommand(); }
    virtual int GetId() { return HOTSPOT_CLICK_COMMAND; }

    virtual const char *GetName() { return "hotspot_click"; }
    virtual const char *GetUsage() { return "cannot be invoked on commandline"; }
    virtual const char *GetDescription() 
    { return "activates a hotspot with a 'click' action"; }

    virtual void Setup(Input &input)
    {
        if (input.GetId() == MOUSE_CLICK_INPUT) {
            pos = ((MouseClickInput*) &input)->pos;
        } else if (input.GetId() == KEYBOARD_INPUT) {
            pos = ((KeyboardInput*) &input)->pos;
        } else if (input.GetId() == SPECIAL_KEY_INPUT) {
            pos = ((SpecialKeyInput*) &input)->pos;
        }
    }
    
    Vertex2i pos;
};


class GetMousePosCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new GetMousePosCommand(); }
    virtual int GetId() { return GET_MOUSE_POS_COMMAND; }

    virtual const char *GetName() { return "get_mouse_pos"; }
    virtual const char *GetUsage() { return "'world'|'screen'|'window'"; }
    virtual const char *GetDescription() 
    { return "gets the current mouse position in the requested coordinates"; }

    virtual bool Setup(Scm lst)
    {
        return ParseScm(ErrorHelp(), lst, "s", &kind);
    }
    
    string kind;
};


// ----------------------------------------------------------------------------
// Constructs
//

class GroupConstruct : public Construct
{
public:
    virtual Command* Create() { return new GroupConstruct(); }
    virtual int GetId() { return GROUP_CONSTRUCT; }

    virtual const char *GetName() { return "group"; }
    virtual const char *GetUsage() { return "* elements"; }
    virtual const char *GetDescription() 
    { return "constructs a group of elements"; }
};

class DynamicGroupConstruct : public Construct
{
public:
    virtual Command* Create() { return new DynamicGroupConstruct(); }
    virtual int GetId() { return DYNAMIC_GROUP_CONSTRUCT; }

    virtual const char *GetName() { return "dynamic_group"; }
    virtual const char *GetUsage() { return "procedure"; }
    virtual const char *GetDescription() 
    { return "constructs a group whose contents is determined by a procedure"; }
};


class HotspotConstruct : public Construct
{
public:
    virtual Command* Create() { return new HotspotConstruct(); }
    virtual int GetId() { return HOTSPOT_CONSTRUCT; }

    virtual const char *GetName() { return "hotspot"; }
    virtual const char *GetUsage() 
    { return "'over'|'out'|'click', x1, y1, x2, y2, proc"; }
    virtual const char *GetDescription() 
    { return "constructs hotspot for a region that activates a python procedure 'proc'"; }
};



class PointsConstruct : public Construct
{
public:
    virtual Command* Create() { return new PointsConstruct(); }
    virtual int GetId() { return POINTS_CONSTRUCT; }

    virtual const char *GetName() { return "points"; }
    virtual const char *GetUsage() { return "* vertices|colors"; }
    virtual const char *GetDescription() 
    { return "plots vertices as points"; }
};

class LinesConstruct : public Construct
{
public:
    virtual Command* Create() { return new LinesConstruct(); }
    virtual int GetId() { return LINES_CONSTRUCT; }

    virtual const char *GetName() { return "lines"; }
    virtual const char *GetUsage() { return "* vertices|colors"; }
    virtual const char *GetDescription() 
    { return "plots vertices as lines"; }
};
class LineStripConstruct : public Construct
{
public:
    virtual Command* Create() { return new LineStripConstruct(); }
    virtual int GetId() { return LINE_STRIP_CONSTRUCT; }

    virtual const char *GetName() { return "line_strip"; }
    virtual const char *GetUsage() { return "* vertices|colors"; }
    virtual const char *GetDescription() 
    { return "plots vertices as connected lines"; }
};


class TrianglesConstruct : public Construct
{
public:
    virtual Command* Create() { return new TrianglesConstruct(); }
    virtual int GetId() { return TRIANGLES_CONSTRUCT; }

    virtual const char *GetName() { return "triangles"; }
    virtual const char *GetUsage() { return "* vertices|colors"; }
    virtual const char *GetDescription() 
    { return "plots vertices as triangles"; }
};

class TriangleStripConstruct : public Construct
{
public:
    virtual Command* Create() { return new TriangleStripConstruct(); }
    virtual int GetId() { return TRIANGLE_STRIP_CONSTRUCT; }

    virtual const char *GetName() { return "triangle_strip"; }
    virtual const char *GetUsage() { return "* vertices|colors"; }
    virtual const char *GetDescription() 
    { return "plots vertices as connected triangles"; }
};

class TriangleFanConstruct : public Construct
{
public:
    virtual Command* Create() { return new TriangleFanConstruct(); }
    virtual int GetId() { return TRIANGLE_FAN_CONSTRUCT; }

    virtual const char *GetName() { return "triangle_fan"; }
    virtual const char *GetUsage() { return "* vertices|colors"; }
    virtual const char *GetDescription() 
    { return "plots vertices as triangles in a fan"; }
};


class QuadsConstruct : public Construct
{
public:
    virtual Command* Create() { return new QuadsConstruct(); }
    virtual int GetId() { return QUADS_CONSTRUCT; }

    virtual const char *GetName() { return "quads"; }
    virtual const char *GetUsage() { return "* vertices|colors"; }
    virtual const char *GetDescription() 
    { return "plots vertices as quads"; }
};

class QuadStripConstruct : public Construct
{
public:
    virtual Command* Create() { return new QuadStripConstruct(); }
    virtual int GetId() { return QUAD_STRIP_CONSTRUCT; }

    virtual const char *GetName() { return "quad_strip"; }
    virtual const char *GetUsage() { return "* vertices|colors"; }
    virtual const char *GetDescription() 
    { return "plots vertices as connected quads"; }
};

class PolygonConstruct : public Construct
{
public:
    virtual Command* Create() { return new PolygonConstruct(); }
    virtual int GetId() { return POLYGON_CONSTRUCT; }

    virtual const char *GetName() { return "polygon"; }
    virtual const char *GetUsage() { return "* vertices|colors"; }
    virtual const char *GetDescription() 
    { return "plots vertices as a convex polygon"; }
};

class TextConstruct : public Construct
{
public:
    virtual Command* Create() { return new TextConstruct(); }
    virtual int GetId() { return TEXT_CONSTRUCT; }

    virtual const char *GetName() { return "text"; }
    virtual const char *GetUsage() 
    { return "string, x1, y1, x2, y2, ['left'|'center'|'right'], \
['top'|'middle'|'bottom']"; }
    virtual const char *GetDescription() 
    { return "draws text justified within a bounding box"; }
};


class TextScaleConstruct : public Construct
{
public:
    virtual Command* Create() { return new TextScaleConstruct(); }
    virtual int GetId() { return TEXT_SCALE_CONSTRUCT; }

    virtual const char *GetName() { return "text_scale"; }
    virtual const char *GetUsage() 
    { return "string, x1, y1, x2, y2, ['left'|'center'|'right'], \
['top'|'middle'|'bottom']"; }
    virtual const char *GetDescription() 
    { return "draws stroked text within a bounding box"; }
};


class TextClipConstruct : public Construct
{
public:
    virtual Command* Create() { return new TextClipConstruct(); }
    virtual int GetId() { return TEXT_CLIP_CONSTRUCT; }

    virtual const char *GetName() { return "text_clip"; }
    virtual const char *GetUsage() 
    { return "string, x1, y1, x2, y2, minheight, maxheight, ['left'|'center'|'right'], \
['top'|'middle'|'bottom']"; }
    virtual const char *GetDescription() 
    { return "draws stroked text within a bounding box and height restrictions"; }
};


//---------------------------------------------------------------------------
// primitives

class VerticesConstruct : public Construct
{
public:
    virtual Command* Create() { return new VerticesConstruct(); }
    virtual int GetId() { return VERTICES_CONSTRUCT; }

    virtual const char *GetName() { return "vertices"; }
    virtual const char *GetUsage() { return "x, y, * more"; }
    virtual const char *GetDescription() 
    { return "creates a list of vertices"; }
};

class ColorConstruct : public Construct
{
public:
    virtual Command* Create() { return new ColorConstruct(); }
    virtual int GetId() { return COLOR_CONSTRUCT; }

    virtual const char *GetName() { return "color"; }
    virtual const char *GetUsage() { return "red, green, blue, [alpha]"; }
    virtual const char *GetDescription() 
    { return "creates a color from 3 or 4 values in [0,1]"; }
};

class TranslateConstruct : public Construct
{
public:
    virtual Command* Create() { return new TranslateConstruct(); }
    virtual int GetId() { return TRANSLATE_CONSTRUCT; }

    virtual const char *GetName() { return "translate"; }
    virtual const char *GetUsage() { return "x, y, * elements"; }
    virtual const char *GetDescription() 
    { return "translates the coordinate system of enclosed elements"; }
};

class RotateConstruct : public Construct
{
public:
    virtual Command* Create() { return new RotateConstruct(); }
    virtual int GetId() { return ROTATE_CONSTRUCT; }

    virtual const char *GetName() { return "rotate"; }
    virtual const char *GetUsage() { return "angle, * elements"; }
    virtual const char *GetDescription() 
    { return "rotates the coordinate system of enclosed elements"; }
};

class ScaleConstruct : public Construct
{
public:
    virtual Command* Create() { return new ScaleConstruct(); }
    virtual int GetId() { return SCALE_CONSTRUCT; }

    virtual const char *GetName() { return "scale"; }
    virtual const char *GetUsage() { return "x, y, * elements"; }
    virtual const char *GetDescription() 
    { return "scales the coordinate system of enclosed elements"; }
};

class FlipConstruct : public Construct
{
public:
    virtual Command* Create() { return new FlipConstruct(); }
    virtual int GetId() { return FLIP_CONSTRUCT; }

    virtual const char *GetName() { return "flip"; }
    virtual const char *GetUsage() { return "x, y, * elements"; }
    virtual const char *GetDescription() 
    { return "flips the coordinate system of enclosed elements over (x,y)"; }
};


class NozoomConstruct : public Construct
{
public:
    virtual Command* Create() { return new NozoomConstruct(); }
    virtual int GetId() { return NOZOOM_CONSTRUCT; }

    virtual const char *GetName() { return "nozoom"; }
    virtual const char *GetUsage() { return "zoomx, zoomy, * elements"; }
    virtual const char *GetDescription() 
    { return "Prevents zooming along either the x or y coordinates for the enclosed elements"; }
};


class SideAlignConstruct : public Construct
{
public:
    virtual Command* Create() { return new SideAlignConstruct(); }
    virtual int GetId() { return SIDE_ALIGN_CONSTRUCT; }

    virtual const char *GetName() { return "side_align"; }
    virtual const char *GetUsage() { return "['top'], ['bottom'], ['left'], ['right'], * elements"; }
    virtual const char *GetDescription() 
    { return "Prevents the origin of the elements from leaving the window via the specified sides"; }
};


class InputKeyConstruct : public Construct
{
public:
    virtual Command* Create() { return new InputKeyConstruct(); }
    virtual int GetId() { return INPUT_KEY_CONSTRUCT; }

    virtual const char *GetName() { return "input_key"; }
    virtual const char *GetUsage() { return "key, ['shift'], ['ctrl'], ['alt']"; }
    virtual const char *GetDescription() 
    { return "specifies a keyboard input"; }
};

class InputClickConstruct : public Construct
{
public:
    virtual Command* Create() { return new InputClickConstruct(); }
    virtual int GetId() { return INPUT_CLICK_CONSTRUCT; }

    virtual const char *GetName() { return "input_click"; }
    virtual const char *GetUsage() 
    { return "'left'|'middle'|'right', 'up'|'down', \
['shift'], ['ctrl'], ['alt']"; }
    virtual const char *GetDescription() 
    { return "specifies a mouse click input"; }
};

class InputMotionConstruct : public Construct
{
public:
    virtual Command* Create() { return new InputMotionConstruct(); }
    virtual int GetId() { return INPUT_MOTION_CONSTRUCT; }

    virtual const char *GetName() { return "input_motion"; }
    virtual const char *GetUsage() 
    { return "'left'|'middle'|'right', 'up'|'down', \
['shift'], ['ctrl'], ['alt']"; }
    virtual const char *GetDescription() 
    { return "specifies a mouse motion input"; }
};



// --------------

class CallProcCommand : public ScriptCommand
{
public:
    CallProcCommand(Scm code = Scm_UNDEFINED) : defined(false)
    {
        if (code != Scm_UNDEFINED) {
            Setup(ScmCons(code, Scm_EOL));
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
            defined = true;
            return true;            
        } else {
            Error("argument must be a procedure");
            return false;
        }
    }
    
    inline Scm GetScmProc()
    {
        return proc;
    }
    
    Scm proc;
    bool defined;
    string name;
    static int procid;
};



}

#endif
