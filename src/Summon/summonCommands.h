/***************************************************************************
* Summon
* Matt Rasmussen
* summonCommands.h
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



namespace Summon
{

using namespace std;



// command ids
enum {
    SUMMON_COMMANDS_BEGIN = GLUT_COMMANDS_END + 1,
    
    // global commands
    GET_WINDOWS_COMMAND,
    NEW_WINDOW_COMMAND,
    CLOSE_WINDOW_COMMAND,
    GET_MODELS_COMMAND,
    NEW_MODEL_COMMAND,
    ASSIGN_MODEL_COMMAND,
    DEL_MODEL_COMMAND,
    GET_WINDOW_DECORATION_COMMAND,
    SET_WINDOW_CLOSE_CALLBACK_COMMAND,
    TIMER_CALL_COMMAND,
    REDRAW_CALL_COMMAND,
    
    // menu commands
    NEW_MENU_COMMAND,
    DEL_MENU_COMMAND,
    ADD_MENU_ENTRY_COMMAND,
    ADD_SUBMENU_COMMAND,
    REMOVE_MENU_ITEM_COMMAND,
    SET_MENU_ENTRY_COMMAND,
    SET_SUBMENU_COMMAND,
    ATTACH_MENU_COMMAND,
    DETACH_MENU_COMMAND,
    
    APPEND_GROUP_COMMAND,
    REMOVE_GROUP_COMMAND2,
    REPLACE_GROUP_COMMAND2,
    SHOW_GROUP_COMMAND2,
    GET_BOUNDING_COMMAND2,    
    
    // model commands
    ADD_GROUP_COMMAND,
    INSERT_GROUP_COMMAND,
    REMOVE_GROUP_COMMAND,
    REPLACE_GROUP_COMMAND,
    CLEAR_GROUPS_COMMAND,
    SHOW_GROUP_COMMAND,   
    GET_ROOT_ID_COMMAND,
    GET_BOUNDING_COMMAND,
    
    // view commands
    SET_WINDOW_NAME_COMMAND,
    GET_WINDOW_NAME_COMMAND,
    SET_WINDOW_POSITION_COMMAND,
    GET_WINDOW_POSITION_COMMAND,
    SET_WINDOW_SIZE_COMMAND,
    GET_WINDOW_SIZE_COMMAND,
    SET_WINDOW_BOUNDARY_COMMAND,
    GET_WINDOW_BOUNDARY_COMMAND,
    RAISE_WINDOW_COMMAND,
    SET_TRANS_COMMAND,
    GET_TRANS_COMMAND,
    SET_ZOOM_COMMAND,
    GET_ZOOM_COMMAND,
    SET_FOCUS_COMMAND,
    GET_FOCUS_COMMAND,
    SET_BGCOLOR_COMMAND,
    GET_BGCOLOR_COMMAND,
    SET_VISIBLE_COMMAND,
    GET_VISIBLE_COMMAND,
    SET_ANTIALIAS_COMMAND,
    SHOW_CROSSHAIR_COMMAND,
    SET_CROSSHAIR_COLOR_COMMAND,
    MODEL_CHANGED_COMMAND,    
    
    // view basic commands
    TRANSLATE_SCRIPT_COMMAND,
    ZOOM_SCRIPT_COMMAND,
    ZOOM_X_SCRIPT_COMMAND,
    ZOOM_Y_SCRIPT_COMMAND,
    FOCUS_SCRIPT_COMMAND,
    
    // controller commands
    SET_BINDING_COMMAND,
    CLEAR_BINDING_COMMAND,
    CLEAR_ALL_BINDINGS_COMMAND,
    HOTSPOT_CLICK_COMMAND,
    GET_MOUSE_POS_COMMAND,
    SET_WINDOW_ON_RESIZE_COMMAND,
    SET_WINDOW_ON_MOVE_COMMAND,
    
    // constructs
    
    // structure
    GROUP_CONSTRUCT = 2000,
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
    ZOOM_CLAMP_CONSTRUCT,
    NOZOOM_CONSTRUCT,
    SIDE_ALIGN_CONSTRUCT,
    
    // inputs
    INPUT_KEY_CONSTRUCT,
    INPUT_CLICK_CONSTRUCT,
    INPUT_MOTION_CONSTRUCT,
    
    // misc
    CALL_PROC_COMMAND,    
        
    SUMMON_COMMANDS_END
};




void summonCommandsInit();

// common parsing functions
bool ParseCommand(Scm procScm, Command **command);
bool ParseInput(Scm lst, Input **input);
bool ParseKeyInput(Scm lst, Input **input);
bool ParseMouse(bool isClick, Scm lst, Input **input);
bool ParseMod(Scm lst, int *mod);


// attributes
extern CommandAttr g_elementAttr;
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

class WindowCommand : public ScriptCommand
{
public:
    int windowid;
};

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


class NewWindowCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new NewWindowCommand(); }
    virtual int GetId() { return NEW_WINDOW_COMMAND; }

    virtual const char *GetName() { return "new_window"; }
    virtual const char *GetUsage() { return "name, width, height, left, top"; }
    virtual const char *GetDescription() 
    { return "creates a new window and returns its id"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "sdddd", &name, &size.x, &size.y,
                                                     &position.x, &position.y);
    }
    
    string name;
    Vertex2i size;
    Vertex2i position;
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
            return ParseScm(lst, "d", &windowid);
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
        return ParseScm(lst, "dsd", &windowid, &kind, &modelid);
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
        return ParseScm(lst, "d", &modelid);
    }
    
    int modelid;
};



class GetWindowDecorationCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new GetWindowDecorationCommand(); }
    virtual int GetId() { return GET_WINDOW_DECORATION_COMMAND; }

    virtual const char *GetName() { return "get_window_decoration"; }
    virtual const char *GetUsage() 
    { return ""; }
    virtual const char *GetDescription() 
    { return "get a window decoration size"; }
};




class SetWindowCloseCallbackCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new SetWindowCloseCallbackCommand(); }
    virtual int GetId() { return SET_WINDOW_CLOSE_CALLBACK_COMMAND; }

    virtual const char *GetName() { return "set_window_close_callback"; }
    virtual const char *GetUsage() 
    { return ""; }
    virtual const char *GetDescription() 
    { return "set the callback for a window close"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "p", &callback);
    }
    
    Scm callback;
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
        return ParseScm(lst, "fp", &delay, &proc);
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
        return ParseScm(lst, "p", &proc);
    }
    
    Scm proc;
};


//=============================================================================
// menu commands

class NewMenuCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new NewMenuCommand(); }
    virtual int GetId() { return NEW_MENU_COMMAND; }

    virtual const char *GetName() { return "new_menu"; }
    virtual const char *GetUsage() 
    { return ""; }
    virtual const char *GetDescription() 
    { return "creates a new menu"; }   
};

class DelMenuCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new DelMenuCommand(); }
    virtual int GetId() { return DEL_MENU_COMMAND; }

    virtual const char *GetName() { return "del_menu"; }
    virtual const char *GetUsage() 
    { return "menuid"; }
    virtual const char *GetDescription() 
    { return "deletes a menu"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "d", &menuid);
    }
    
    int menuid;
};

class AddMenuEntryCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new AddMenuEntryCommand(); }
    virtual int GetId() { return ADD_MENU_ENTRY_COMMAND; }

    virtual const char *GetName() { return "add_menu_entry"; }
    virtual const char *GetUsage() 
    { return "menuid, text, func"; }
    virtual const char *GetDescription() 
    { return "adds an entry to a menu"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dsp", &menuid, &text, &func);
    }
    
    int menuid;
    string text;
    Scm func;
};


class AddSubmenuCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new AddSubmenuCommand(); }
    virtual int GetId() { return ADD_SUBMENU_COMMAND; }

    virtual const char *GetName() { return "add_submenu"; }
    virtual const char *GetUsage() 
    { return "menuid, text, submenuid"; }
    virtual const char *GetDescription() 
    { return "adds a submenu to a menu"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dsd", &menuid, &text, &submenuid);
    }
    
    int menuid;
    string text;
    int submenuid;
};


class RemoveMenuItemCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new RemoveMenuItemCommand(); }
    virtual int GetId() { return REMOVE_MENU_ITEM_COMMAND; }

    virtual const char *GetName() { return "remove_menu_item"; }
    virtual const char *GetUsage() 
    { return "menuid, index"; }
    virtual const char *GetDescription() 
    { return "remove a menu item from a menu"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dd", &menuid, &index);
    }
    
    int menuid;
    int index;
};


class SetMenuEntryCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new SetMenuEntryCommand(); }
    virtual int GetId() { return SET_MENU_ENTRY_COMMAND; }

    virtual const char *GetName() { return "set_menu_entry"; }
    virtual const char *GetUsage() 
    { return "menuid, index, text, func"; }
    virtual const char *GetDescription() 
    { return "set a menu item to a menu entry"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "ddsp", &menuid, &index, &text, &func);
    }
    
    int menuid;
    int index;
    string text;
    Scm func;
};

class SetSubmenuCommand : public ScriptCommand
{
public:
    virtual Command* Create() { return new SetSubmenuCommand(); }
    virtual int GetId() { return SET_SUBMENU_COMMAND; }

    virtual const char *GetName() { return "set_submenu"; }
    virtual const char *GetUsage() 
    { return "menuid, index, text, submenuid"; }
    virtual const char *GetDescription() 
    { return "set a menu item to a menu entry"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "ddsd", &menuid, &index, &text, &submenuid);
    }
    
    int menuid;
    int index;
    string text;
    int submenuid;
};


class AttachMenuCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new AttachMenuCommand(); }
    virtual int GetId() { return ATTACH_MENU_COMMAND; }

    virtual const char *GetName() { return "attach_menu"; }
    virtual const char *GetUsage() 
    { return "windowid, menuid, button"; }
    virtual const char *GetDescription() 
    { return "attach a menu to a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "ddd", &windowid, &menuid, &button);
    }
    
    int menuid;
    int button;
};


class DetachMenuCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new DetachMenuCommand(); }
    virtual int GetId() { return DETACH_MENU_COMMAND; }

    virtual const char *GetName() { return "detach_menu"; }
    virtual const char *GetUsage() 
    { return "windowid, menuid, button"; }
    virtual const char *GetDescription() 
    { return "detach a menu from a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "ddd", &windowid, &menuid, &button);
    }
    
    int menuid;
    int button;
};

//=============================================================================
// element commands


class ElementCommand : public ScriptCommand
{
public:
    int groupid;    
};


class AppendGroupCommand : public ElementCommand
{
public:
    virtual Command* Create() { return new AppendGroupCommand(); }
    virtual int GetId() { return APPEND_GROUP_COMMAND; }

    virtual const char *GetName() { return "append_group"; }
    virtual const char *GetUsage() { return "groupid, group"; }
    virtual const char *GetDescription() 
    { return "adds drawing groups as children to another group"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dc", &groupid, &code);
    }
    
    Scm code;
};

class RemoveGroupCommand2 : public ElementCommand
{
public:
    virtual Command* Create() { return new RemoveGroupCommand2(); }
    virtual int GetId() { return REMOVE_GROUP_COMMAND2; }

    virtual const char *GetName() { return "remove_group2"; }
    virtual const char *GetUsage() { return "groupid, *groups"; }
    virtual const char *GetDescription() 
    { return "removes drawing groups from another group"; }
    
    virtual bool Setup(Scm lst)
    {
        // parse groupid
        if (ScmIsList(lst) && ScmIsInt(lst.GetScm(0))) {
            groupid = Scm2Int(lst.GetScm(0));
            lst.Pop();
        } else {
            return false;
        }
    
        // parse group ids
        for (int i=0; i<lst.Size(); i++) {
            Scm elmid = lst.GetScm(i);
            if (ScmIsInt(elmid)) {
                groupids.push_back(Scm2Int(elmid));
            } else {
                Error("Can only remove with group id");
                return false;
            }
        }
        
        return true;
    }
    
    vector<int> groupids;
};


class ReplaceGroupCommand2 : public ElementCommand
{
public:
    virtual Command* Create() { return new ReplaceGroupCommand2(); }
    virtual int GetId() { return REPLACE_GROUP_COMMAND2; }

    virtual const char *GetName() { return "replace_group2"; }
    virtual const char *GetUsage() { return "oldgroup, newgroup"; }
    virtual const char *GetDescription() 
    { return "replaces a drawing group"; }
    
    virtual bool Setup(Scm lst)
    {
        if (ParseScm(lst, "dd", &groupid, &oldgroupid)) {
            code = ScmCddr(lst);
            return true;
        } else {
            return false;
        }
    }
    
    int oldgroupid;
    Scm code;
};


class ShowGroupCommand2 : public ElementCommand
{
public:
    virtual Command* Create() { return new ShowGroupCommand2(); }
    virtual int GetId() { return SHOW_GROUP_COMMAND2; }

    virtual const char *GetName() { return "show_group2"; }
    virtual const char *GetUsage() { return "groupid, True|False"; }
    virtual const char *GetDescription() 
    { return "sets the visibilty of a group"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "db", &groupid, &visible);
    }
    
    bool visible;
};

class GetBoundingCommand2 : public ElementCommand
{
public:
    virtual Command* Create() { return new GetBoundingCommand2(); }
    virtual int GetId() { return GET_BOUNDING_COMMAND2; }

    virtual const char *GetName() { return "get_bounding2"; }
    virtual const char *GetUsage() { return "groupid"; }
    virtual const char *GetDescription() 
    { return "returns a bounding box for a group and its contents"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "d", &groupid);
    }
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
        return ParseScm(lst, "dc", &modelid, &code);
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
        return ParseScm(lst, "ddc", &modelid, &groupid, &code);
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
        if (ScmIsList(lst) && ScmIsInt(lst.GetScm(0))) {
            modelid = Scm2Int(lst.GetScm(0));
            lst.Pop();
        } else {
            return false;
        }
    
        // parse group ids
        for (int i=0; i<lst.Size(); i++) {
            Scm elmid = lst.GetScm(i);
            if (ScmIsInt(elmid)) {
                groupids.push_back(Scm2Int(elmid));
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
        if (ParseScm(lst, "dd", &modelid, &groupid)) {
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
        return ParseScm(lst, "d", &modelid);
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
        return ParseScm(lst, "ddb", &modelid, &groupid, &visible);
    }
    
    int groupid;
    bool visible;
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
        return ParseScm(lst, "d", &modelid);
    }
};



class GetBoundingCommand : public ModelCommand
{
public:
    virtual Command* Create() { return new GetBoundingCommand(); }
    virtual int GetId() { return GET_BOUNDING_COMMAND; }

    virtual const char *GetName() { return "get_bounding"; }
    virtual const char *GetUsage() { return "modelid, groupid"; }
    virtual const char *GetDescription() 
    { return "returns a bounding box for a group and its contents"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dd", &modelid, &groupid);
    }
    
    int groupid;
};



// -----------------------------------------------------------------------------
// view commands
//


class SetWindowNameCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetWindowNameCommand(); }
    virtual int GetId() { return SET_WINDOW_NAME_COMMAND; }

    virtual const char *GetName() { return "set_window_name"; }
    virtual const char *GetUsage() { return "id, name"; }
    virtual const char *GetDescription() 
    { return "sets the name of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "ds", &windowid, &name);
    }
    
    string name;
};


class GetWindowNameCommand : public WindowCommand
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
        return ParseScm(lst, "d", &windowid);
    }
};


class SetWindowPositionCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetWindowPositionCommand(); }
    virtual int GetId() { return SET_WINDOW_POSITION_COMMAND; }

    virtual const char *GetName() { return "set_window_position"; }
    virtual const char *GetUsage() { return "id, x, y"; }
    virtual const char *GetDescription() 
    { return "sets the position of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "ddd", &windowid, &x, &y);
    }
    
    int x, y;
};

class GetWindowPositionCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new GetWindowPositionCommand(); }
    virtual int GetId() { return GET_WINDOW_POSITION_COMMAND; }

    virtual const char *GetName() { return "get_window_position"; }
    virtual const char *GetUsage() { return "id"; }
    virtual const char *GetDescription() 
    { return "gets the position of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "d", &windowid);
    }
};


class SetWindowSizeCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetWindowSizeCommand(); }
    virtual int GetId() { return SET_WINDOW_SIZE_COMMAND; }

    virtual const char *GetName() { return "set_window_size"; }
    virtual const char *GetUsage() { return "id, x, y"; }
    virtual const char *GetDescription() 
    { return "sets current window's size"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "ddd", &windowid, &width, &height);
    }
    
    
    int width;
    int height;
};


class GetWindowSizeCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new GetWindowSizeCommand(); }
    virtual int GetId() { return GET_WINDOW_SIZE_COMMAND; }

    virtual const char *GetName() { return "get_window_size"; }
    virtual const char *GetUsage() { return "id"; }
    virtual const char *GetDescription() 
    { return "gets current window's size"; }

    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "d", &windowid);
    }    
};

class SetWindowBoundaryCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetWindowBoundaryCommand(); }
    virtual int GetId() { return SET_WINDOW_BOUNDARY_COMMAND; }

    virtual const char *GetName() { return "set_window_boundary"; }
    virtual const char *GetUsage() { return "id, x1, y1, x2, y2"; }
    virtual const char *GetDescription() 
    { return "sets current window's boundary"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dffff", &windowid, &pos1.x, &pos1.y, 
                        &pos2.x, &pos2.y);
    }
    
    Vertex2f pos1;
    Vertex2f pos2;
};


class GetWindowBoundaryCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new GetWindowBoundaryCommand(); }
    virtual int GetId() { return GET_WINDOW_BOUNDARY_COMMAND; }

    virtual const char *GetName() { return "get_window_boundary"; }
    virtual const char *GetUsage() { return "id"; }
    virtual const char *GetDescription() 
    { return "gets current window's boundary"; }

    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "d", &windowid);
    }
};



class RaiseWindowCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new RaiseWindowCommand(); }
    virtual int GetId() { return RAISE_WINDOW_COMMAND; }

    virtual const char *GetName() { return "raise_window"; }
    virtual const char *GetUsage() { return "id, raise"; }
    virtual const char *GetDescription() 
    { return "raises or lowers a window"; }

    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "db", &windowid, &raise);
    }
    
    bool raise;
};


class SetTransCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetTransCommand(); }
    virtual int GetId() { return SET_TRANS_COMMAND; }

    virtual const char *GetName() { return "set_trans"; }
    virtual const char *GetUsage() { return "id, x, y"; }
    virtual const char *GetDescription() 
    { return "sets the translation of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dff", &windowid, &x, &y);
    }
    
    float x, y;
};

class GetTransCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new GetTransCommand(); }
    virtual int GetId() { return GET_TRANS_COMMAND; }

    virtual const char *GetName() { return "get_trans"; }
    virtual const char *GetUsage() { return "id"; }
    virtual const char *GetDescription() 
    { return "gets the translation of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "d", &windowid);
    }
};


class SetZoomCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetZoomCommand(); }
    virtual int GetId() { return SET_ZOOM_COMMAND; }

    virtual const char *GetName() { return "set_zoom"; }
    virtual const char *GetUsage() { return "id, zoomx, zoomy"; }
    virtual const char *GetDescription() 
    { return "sets the zoom of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dff", &windowid, &x, &y);
    }
    
    float x, y;
};

class GetZoomCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new GetZoomCommand(); }
    virtual int GetId() { return GET_ZOOM_COMMAND; }

    virtual const char *GetName() { return "get_zoom"; }
    virtual const char *GetUsage() { return "id"; }
    virtual const char *GetDescription() 
    { return "gets the zoom of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "d", &windowid);
    }
};


class SetFocusCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetFocusCommand(); }
    virtual int GetId() { return SET_FOCUS_COMMAND; }

    virtual const char *GetName() { return "set_focus"; }
    virtual const char *GetUsage() { return "id, x, y"; }
    virtual const char *GetDescription() 
    { return "sets the focus point of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dff", &windowid, &x, &y);
    }
    
    float x, y;
};

class GetFocusCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new GetFocusCommand(); }
    virtual int GetId() { return GET_FOCUS_COMMAND; }

    virtual const char *GetName() { return "get_focus"; }
    virtual const char *GetUsage() { return "id"; }
    virtual const char *GetDescription() 
    { return "gets the focus point of a window"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "d", &windowid);
    }
};


class SetBgColorCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetBgColorCommand(); }
    virtual int GetId() { return SET_BGCOLOR_COMMAND; }

    virtual const char *GetName() { return "set_bgcolor"; }
    virtual const char *GetUsage() { return "id, red, green, blue"; }
    virtual const char *GetDescription() 
    { return "sets background color"; }
    
    virtual bool Setup(Scm lst)
    {
        float r, g, b;
        
        if (ParseScm(lst, "dfff", &windowid, &r, &g, &b)) {
            color = Color(r, g, b);
            return true;
        } else
            return false;
    }
    
    Color color;
};


class GetBgColorCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new GetBgColorCommand(); }
    virtual int GetId() { return GET_BGCOLOR_COMMAND; }

    virtual const char *GetName() { return "get_bgcolor"; }
    virtual const char *GetUsage() { return "id"; }
    virtual const char *GetDescription() 
    { return "gets background color"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "d", &windowid);
    }
};


class SetVisibleCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetVisibleCommand(); }
    virtual int GetId() { return SET_VISIBLE_COMMAND; }

    virtual const char *GetName() { return "set_visible"; }
    virtual const char *GetUsage() { return "id, x1, y1, x2, y2"; }
    virtual const char *GetDescription() 
    { return "change display to contain region (x1,y1)-(x2,y2)"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dffff", &windowid,
                        &data[0], &data[1], &data[2], &data[3]);
    }
    
    float data[4];
};


class GetVisibleCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new GetVisibleCommand(); }
    virtual int GetId() { return GET_VISIBLE_COMMAND; }

    virtual const char *GetName() { return "get_visible"; }
    virtual const char *GetUsage() { return "id"; }
    virtual const char *GetDescription() 
    { return "gets visible bounding box"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "d", &windowid);
    }
};


class SetAntialiasCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetAntialiasCommand(); }
    virtual int GetId() { return SET_ANTIALIAS_COMMAND; }

    virtual const char *GetName() { return "set_antialias"; }
    virtual const char *GetUsage() { return "id, True|False"; }
    virtual const char *GetDescription() 
    { return "sets anti-aliasing status"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "db", &windowid, &enabled);
    }
    
    bool enabled;
};


class ShowCrosshairCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new ShowCrosshairCommand(); }
    virtual int GetId() { return SHOW_CROSSHAIR_COMMAND; }

    virtual const char *GetName() { return "show_crosshair"; }
    virtual const char *GetUsage() { return "id, True|False"; }
    virtual const char *GetDescription() 
    { return "shows and hides the mouse crosshair"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "db", &windowid, &enabled);
    }
    
    bool enabled;
};


class SetCrosshairColorCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetCrosshairColorCommand(); }
    virtual int GetId() { return SET_CROSSHAIR_COLOR_COMMAND; }

    virtual const char *GetName() { return "set_crosshair_color"; }
    virtual const char *GetUsage() { return "id, red, green, blue[, alpha]"; }
    virtual const char *GetDescription() 
    { return "sets mouse crosshair color"; }
    
    virtual bool Setup(Scm lst)
    {
        float r, g, b, a;
        
        if (ScmLength(lst) == 4 && 
            ParseScm(lst, "dfff", &windowid, &r, &g, &b))
        {
            color = Color(r, g, b, 1);
            return true;
        } else if (ScmLength(lst) == 5 && 
                   ParseScm(lst, "dffff", &windowid, &r, &g, &b, &a))
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
// View basic commands
//

class TranslateScriptCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new TranslateScriptCommand(); }
    virtual CommandId GetId() { return TRANSLATE_SCRIPT_COMMAND; }
    
    virtual void Setup(Input &input) 
    {
        if (input.GetId() == MOUSE_MOTION_INPUT) {
            trans = ((MouseMotionInput*)(&input))->vel;
            trans.y *= -1;
        }
    }

    virtual const char *GetName() { return "trans"; }
    virtual const char *GetUsage() { return "id, x, y"; }
    virtual const char *GetDescription() 
    { return "translate the view by (x,y)"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dff", &windowid, &trans.x, &trans.y);
    }
    
    Vertex2f trans;
};


class ZoomScriptCommand : public WindowCommand
{
public:
    ZoomScriptCommand() : zoom(1.0, 1.0) {}

    virtual Command* Create() { return new ZoomScriptCommand(); }
    virtual CommandId GetId() { return ZOOM_SCRIPT_COMMAND; }
    
    virtual void Setup(Input &input) 
    {
        if (input.GetId() == MOUSE_MOTION_INPUT) {
            float num = (float) ((MouseMotionInput*)(&input))->vel.y;
            
            // clamp zoom
            if (num > MAX_ZOOM)  num = MAX_ZOOM;
            if (num < -MAX_ZOOM) num = -MAX_ZOOM;
            
            zoom.x = 1 + num / (MAX_ZOOM + 1);
            zoom.y = 1 + num / (MAX_ZOOM + 1);
        }
    }
    
    virtual const char *GetName() { return "zoom"; }
    virtual const char *GetUsage() { return "id, factorX, factorY"; }
    virtual const char *GetDescription() { return "zoom view by a factor"; }
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dff", &windowid, &zoom.x, &zoom.y);
    }
    
    enum { MAX_ZOOM = 20 };
    
    Vertex2f zoom;
};


class ZoomXScriptCommand : public WindowCommand
{
public:
    ZoomXScriptCommand() : zoom(1.0, 1.0) {}

    virtual Command* Create() { return new ZoomXScriptCommand(); }
    virtual CommandId GetId() { return ZOOM_X_SCRIPT_COMMAND; }
    
    virtual void Setup(Input &input) 
    {
        if (input.GetId() == MOUSE_MOTION_INPUT) {
            float num = (float) ((MouseMotionInput*)(&input))->vel.y;
            
            // clamp zoom
            if (num > MAX_ZOOM)  num = MAX_ZOOM;
            if (num < -MAX_ZOOM) num = -MAX_ZOOM;
            
            zoom.x = 1 + num / (MAX_ZOOM + 1);
            zoom.y = 1.0;
        }
    }
    
    virtual const char *GetName() { return "zoomx"; }
    virtual const char *GetUsage() { return "id, factorX"; }
    virtual const char *GetDescription() 
    { return "zoom x-axis by a factor"; }
    
    virtual bool Setup(Scm lst)
    {
        zoom.y = 1.0;
        return ParseScm(lst, "df", &windowid, &zoom.x);
    }
    
    enum { MAX_ZOOM = 20 };
    
    Vertex2f zoom;    
};

class ZoomYScriptCommand : public WindowCommand
{
public:
    ZoomYScriptCommand() : zoom(1.0, 1.0) {}

    virtual Command* Create() { return new ZoomYScriptCommand(); }
    virtual CommandId GetId() { return ZOOM_Y_SCRIPT_COMMAND; }
    
    virtual void Setup(Input &input) 
    {
        if (input.GetId() == MOUSE_MOTION_INPUT) {
            float num = (float) ((MouseMotionInput*)(&input))->vel.y;
            
            // clamp zoom
            if (num > MAX_ZOOM)  num = MAX_ZOOM;
            if (num < -MAX_ZOOM) num = -MAX_ZOOM;

            zoom.x = 1.0;            
            zoom.y = 1 + num / (MAX_ZOOM + 1);
        }
    }
    
    virtual const char *GetName() { return "zoomy"; }
    virtual const char *GetUsage() { return "id, factorY"; }
    virtual const char *GetDescription() 
    { return "zoom y-axis by a factor"; }

    
    virtual bool Setup(Scm lst)
    {
        zoom.x = 1.0;
        return ParseScm(lst, "df", &windowid, &zoom.y);
    }
    
    enum { MAX_ZOOM = 20 };
    
    Vertex2f zoom;    
};


class FocusScriptCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new FocusScriptCommand(); }
    virtual CommandId GetId() { return FOCUS_SCRIPT_COMMAND; }
    
    virtual void Setup(Input &input) 
    {
        if (input.GetId() == MOUSE_CLICK_INPUT) {
            focus = ((MouseClickInput*)&input)->pos;
        }
    }
    
    virtual const char *GetName() { return "focus"; }
    virtual const char *GetUsage() { return "id, x, y"; }
    virtual const char *GetDescription() { return "focus the view on (x,y)"; }
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "ddd", &windowid, &focus.x, &focus.y);
    }
    
    Vertex2i focus;
};

//----------------------------------------------------------------------------
// controller commands
//

class SetBindingCommand : public WindowCommand
{
public:
    SetBindingCommand() : 
        input(NULL), 
        command(NULL)
    {}

    virtual Command* Create() { return new SetBindingCommand(); }
    virtual int GetId() { return SET_BINDING_COMMAND; }

    virtual const char *GetName() { return "set_binding"; }
    virtual const char *GetUsage() { return "id, input, proc|command_name"; }
    virtual const char *GetDescription() 
    { return "bind an input to a command or procedure"; }
    
    virtual bool Setup(Scm lst)
    {
        Scm inputArg;
        Scm commandArg;
        
        if (!ParseScm(lst, "dcc", &windowid, &inputArg, &commandArg)) {
            Error("must specify an input and command/procedure");
            return false;
        }
        
        // ensure member vars are NULL before parsing
        input = NULL;
        command = NULL;
        
        // parse input and command from lst
        if (ParseInput(inputArg, &input) &&
            ParseCommand(commandArg, &command))
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


class ClearBindingCommand : public WindowCommand
{
public:
    ClearBindingCommand() : 
        input(NULL)
    {}

    virtual Command* Create() { return new ClearBindingCommand(); }
    virtual int GetId() { return CLEAR_BINDING_COMMAND; }

    virtual const char *GetName() { return "clear_binding"; }
    virtual const char *GetUsage() { return "id, input"; }
    virtual const char *GetDescription() 
    { return "clear all bindings for an input"; }
    
    virtual bool Setup(Scm lst)
    {
        Scm inputArg;
    
        if (!ParseScm(lst, "dc", &windowid, &inputArg)) {
            Error("must specify an input");
            return false;
        }
    
        // ensure member vars are NULL before parsing
        input = NULL;
        
        // parse input and command from lst
        return ParseInput(inputArg, &input);
    }
    
    Input *input;
};


class ClearAllBindingsCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new ClearAllBindingsCommand(); }
    virtual int GetId() { return CLEAR_ALL_BINDINGS_COMMAND; }

    virtual const char *GetName() { return "clear_all_bindings"; }
    virtual const char *GetUsage() { return "id"; }
    virtual const char *GetDescription() 
    { return "clear all bindings for all input"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "d", &windowid);
    }
};


class HotspotClickCommand : public WindowCommand
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
        } else if (input.GetId() == MOUSE_MOTION_INPUT) {
            pos = ((MouseMotionInput*) &input)->pos;            
        } else if (input.GetId() == KEYBOARD_INPUT) {
            pos = ((KeyboardInput*) &input)->pos;
        } else if (input.GetId() == SPECIAL_KEY_INPUT) {
            pos = ((SpecialKeyInput*) &input)->pos;
        }
    }
    
    Vertex2i pos;
};


class GetMousePosCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new GetMousePosCommand(); }
    virtual int GetId() { return GET_MOUSE_POS_COMMAND; }

    virtual const char *GetName() { return "get_mouse_pos"; }
    virtual const char *GetUsage() { return "id, 'world'|'screen'|'window'"; }
    virtual const char *GetDescription() 
    { return "gets the current mouse position in the requested coordinates"; }

    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "ds", &windowid, &kind);
    }
    
    string kind;
};



class SetWindowOnResizeCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetWindowOnResizeCommand(); }
    virtual int GetId() { return SET_WINDOW_ON_RESIZE_COMMAND; }

    virtual const char *GetName() { return "set_window_on_resize"; }
    virtual const char *GetUsage() { return "id, func"; }
    virtual const char *GetDescription() 
    { return "registers a callback for window resize"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dc", &windowid, &proc);
    }
    
    Scm proc;
};


class SetWindowOnMoveCommand : public WindowCommand
{
public:
    virtual Command* Create() { return new SetWindowOnMoveCommand(); }
    virtual int GetId() { return SET_WINDOW_ON_MOVE_COMMAND; }

    virtual const char *GetName() { return "set_window_on_move"; }
    virtual const char *GetUsage() { return "id, func"; }
    virtual const char *GetDescription() 
    { return "registers a callback for window move"; }
    
    virtual bool Setup(Scm lst)
    {
        return ParseScm(lst, "dc", &windowid, &proc);
    }
    
    Scm proc;
};


class CallProcCommand : public ScriptCommand
{
public:
    CallProcCommand(Scm code=Scm_NONE, Scm args=Scm_EOL) : 
        args(Scm_EOL),    
        defined(false)
    {
        if (code != Scm_NONE) {
            Setup(ScmCons(code, Scm_EOL));
        }
    }
    
    virtual Command* Create() { 
        if (!defined) {
            return new CallProcCommand();
        } else {
            return new CallProcCommand(proc, args);
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
    
    Scm proc;
    Scm args;
    bool defined;
};



}

#endif
