/***************************************************************************
* Summon
* Matt Rasmussen
* summonCommands.cpp
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
#include "summonCommands.h"
#include "glut2DCommands.h"
#include "Element.h"
#include "Graphic.h"
#include "Group.h"
#include "Hotspot.h"
#include "Transform.h"
#include "TextElement.h"
#include "ZoomClamp.h"


namespace Summon
{

using namespace std;


CommandAttr g_elementAttr;
CommandAttr g_globalAttr;
CommandAttr g_constructAttr;
CommandAttr g_modelAttr;
CommandAttr g_viewAttr;
CommandAttr g_controllerAttr;
CommandAttr g_glAttr;


ElementFactory g_elementFactory;


void summonCommandsInit()
{
    glutCommandsInit();
    
#   define e() AddAttr(g_elementAttr)
#   define g() AddAttr(g_globalAttr)
#   define m() AddAttr(g_modelAttr)    
#   define v() AddAttr(g_viewAttr)    
#   define c() AddAttr(g_controllerAttr)
#   define gl() AddAttr(g_glAttr)

    // add glAttr to glut commands
    g_glAttr.Add(new TranslateCommand());
    g_glAttr.Add(new ZoomCommand());
    g_glAttr.Add(new ZoomXCommand());
    g_glAttr.Add(new ZoomYCommand());
    g_glAttr.Add(new FocusCommand());
    
    // global commands
    RegisterScriptCommand(GetWindowsCommand)    g()
    RegisterScriptCommand(NewWindowCommand)     g() gl()
    RegisterScriptCommand(CloseWindowCommand)   g() gl()
    RegisterScriptCommand(GetModelsCommand)     g()
    RegisterScriptCommand(NewModelCommand)      g()
    RegisterScriptCommand(AssignModelCommand)   g()
    RegisterScriptCommand(DelModelCommand)      g()
    RegisterScriptCommand(GetWindowDecorationCommand) g()
    RegisterScriptCommand(SetWindowCloseCallbackCommand) g()
    RegisterScriptCommand(TimerCallCommand)     g() gl()
    RegisterScriptCommand(RedrawCallCommand)    g()
    
    // menu commands
    RegisterScriptCommand(NewMenuCommand)         g() gl()
    RegisterScriptCommand(DelMenuCommand)         g() gl()
    RegisterScriptCommand(AddMenuEntryCommand)    g() gl()
    RegisterScriptCommand(AddSubmenuCommand)      g() gl()
    RegisterScriptCommand(RemoveMenuItemCommand)  g() gl()
    RegisterScriptCommand(SetMenuEntryCommand)    g() gl()
    RegisterScriptCommand(SetSubmenuCommand)      g() gl()
    RegisterScriptCommand(AttachMenuCommand)      v() gl()
    RegisterScriptCommand(DetachMenuCommand)      v() gl()

    
    RegisterScriptCommand(AppendGroupCommand)   e() g()
    RegisterScriptCommand(RemoveGroupCommand2)  e() g()
    RegisterScriptCommand(ReplaceGroupCommand2) e() g()
    RegisterScriptCommand(ShowGroupCommand2)    e() g()
    RegisterScriptCommand(GetBoundingCommand2)  e() g()
    RegisterScriptCommand(SetContentsCommand)   e() g()
    
    // model commands
    RegisterScriptCommand(AddGroupCommand)      m()
    RegisterScriptCommand(InsertGroupCommand)   m()
    RegisterScriptCommand(RemoveGroupCommand)   m()
    RegisterScriptCommand(ReplaceGroupCommand)  m()
    RegisterScriptCommand(ClearGroupsCommand)   m()
    RegisterScriptCommand(ShowGroupCommand)     m()
    RegisterScriptCommand(GetRootIdCommand)     m()
    RegisterScriptCommand(GetBoundingCommand)   m()
    
    // view commands
    RegisterScriptCommand(SetWindowNameCommand)     v() gl()
    RegisterScriptCommand(GetWindowNameCommand)     v()   
    RegisterScriptCommand(SetWindowPositionCommand) v() gl()
    RegisterScriptCommand(GetWindowPositionCommand) v()          
    RegisterScriptCommand(SetWindowSizeCommand)     v() gl()
    RegisterScriptCommand(GetWindowSizeCommand)     v()
    RegisterScriptCommand(SetWindowBoundaryCommand) v() gl()
    RegisterScriptCommand(GetWindowBoundaryCommand) v()    
    RegisterScriptCommand(RaiseWindowCommand)       v() gl()
    RegisterScriptCommand(SetTransCommand)          v() gl()
    RegisterScriptCommand(GetTransCommand)          v()
    RegisterScriptCommand(SetZoomCommand)           v() gl()
    RegisterScriptCommand(GetZoomCommand)           v()
    RegisterScriptCommand(SetFocusCommand)          v()
    RegisterScriptCommand(GetFocusCommand)          v()
    RegisterScriptCommand(SetBgColorCommand)        v() gl()
    RegisterScriptCommand(GetBgColorCommand)        v()
    RegisterScriptCommand(SetVisibleCommand)        v() gl()
    RegisterScriptCommand(GetVisibleCommand)        v()    
    RegisterScriptCommand(SetAntialiasCommand)      v() gl()
    RegisterScriptCommand(ShowCrosshairCommand)     v()
    RegisterScriptCommand(SetCrosshairColorCommand) v()
    RegisterCommand(ModelChangedCommand)

    // view basics commands
    RegisterScriptCommand(TranslateScriptCommand)   v() gl()
    RegisterScriptCommand(ZoomScriptCommand)        v() gl()
    RegisterScriptCommand(ZoomXScriptCommand)       v() gl()
    RegisterScriptCommand(ZoomYScriptCommand)       v() gl()
    RegisterScriptCommand(FocusScriptCommand)       v()

    // controller commands    
    RegisterScriptCommand(SetBindingCommand)        c()
    RegisterScriptCommand(ClearBindingCommand)      c()
    RegisterScriptCommand(ClearAllBindingsCommand)  c()
    RegisterScriptCommand(HotspotClickCommand)      c() gl()
    RegisterScriptCommand(HotspotDragCommand)       c() gl()
    RegisterScriptCommand(HotspotDragStartCommand)  c() gl()
    RegisterScriptCommand(HotspotDragStopCommand)   c() gl()
    RegisterScriptCommand(GetMousePosCommand)       c()
    RegisterScriptCommand(SetWindowOnResizeCommand) c()
    RegisterScriptCommand(SetWindowOnMoveCommand)   c()
    
    RegisterScriptCommand(CallProcCommand) g()

#   undef e
#   undef g
#   undef m
#   undef v
#   undef c
#   undef gl


    // structure
    g_elementFactory.Register(new Group(), GROUP_CONSTRUCT);
    g_elementFactory.Register(new Hotspot(), HOTSPOT_CONSTRUCT);
    
    // graphics
    g_elementFactory.Register(new Graphic(), POINTS_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), LINES_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), LINE_STRIP_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), TRIANGLES_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), TRIANGLE_STRIP_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), TRIANGLE_FAN_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), QUADS_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), QUAD_STRIP_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), POLYGON_CONSTRUCT);
    g_elementFactory.Register(new TextElement(), TEXT_CONSTRUCT);
    g_elementFactory.Register(new TextElement(), TEXT_SCALE_CONSTRUCT);
    g_elementFactory.Register(new TextElement(), TEXT_CLIP_CONSTRUCT);
    
    // primitives
    //g_elementFactory.Register(new (VerticesConstruct)
    g_elementFactory.Register(new Graphic(), COLOR_CONSTRUCT);
    
    // transforms
    g_elementFactory.Register(new Transform(), TRANSLATE_CONSTRUCT);
    g_elementFactory.Register(new Transform(), ROTATE_CONSTRUCT);
    g_elementFactory.Register(new Transform(), SCALE_CONSTRUCT);
    g_elementFactory.Register(new Transform(), FLIP_CONSTRUCT);
    g_elementFactory.Register(new ZoomClamp(), ZOOM_CLAMP_CONSTRUCT);
}



bool ParseCommand(Scm procScm, Command **command)
{        
    if (ScmStringp(procScm)) {
        string name = Scm2String(procScm);
        
        for (CommandAttr::Iterator i=g_stringAttr.Begin();
             i != g_stringAttr.End(); i++)
        {
            if (name == ((StringCommand*) *i)->GetName()) {
                *command = (*i)->Create();
                return true;
            }
        }

        Error("unknown command '%s'", name.c_str());
        return false;

    } else if (ScmProcedurep(procScm)) {
        *command = new CallProcCommand(procScm);
    } else {
        Error("command must be command name or procedure");
        return false;
    }

    return true;
}


bool ParseInput(Scm lst, Input **input)
{
    if (!ScmIsList(lst) || !ScmIsInt(lst.GetScm(0))) {
        Error("Bad input");
        return false;
    }

    int header = Scm2Int(lst.GetScm(0));
    switch (header) {
        case INPUT_KEY_CONSTRUCT: 
            return ParseKeyInput(lst.Slice(1), input);
        case INPUT_CLICK_CONSTRUCT:
            return ParseMouse(true, lst.Slice(1), input);
        case INPUT_MOTION_CONSTRUCT:
            return ParseMouse(false, lst.Slice(1), input);
        default:
            Error("unknown input");
            return false;
    }
}

bool ParseKeyInput(Scm lst, Input **input)
{
    if (!ScmIsList(lst) || !lst.GetScm(0).IsString()) {
        Error("input key must be given as a string");
        return false;
    }

    string key = Scm2String(lst.GetScm(0));

    if (key.size() == 1) {
        KeyboardInput *keyInput = new KeyboardInput();
        keyInput->key = key[0];

        if (!ParseMod(lst.Slice(1), &(keyInput->mod))) {
            delete keyInput;
            return false;
        }
        *input = keyInput;
        return true;
    } else {
        SpecialKeyInput *keyInput = new SpecialKeyInput();

        if (key == "F1")            keyInput->key = GLUT_KEY_F1;
        else if (key == "F2")       keyInput->key = GLUT_KEY_F2;
        else if (key == "F3")       keyInput->key = GLUT_KEY_F3;
        else if (key == "F4")       keyInput->key = GLUT_KEY_F4;
        else if (key == "F5")       keyInput->key = GLUT_KEY_F5;
        else if (key == "F6")       keyInput->key = GLUT_KEY_F6;                
        else if (key == "F7")       keyInput->key = GLUT_KEY_F7;
        else if (key == "F8")       keyInput->key = GLUT_KEY_F8;
        else if (key == "F9")       keyInput->key = GLUT_KEY_F9;
        else if (key == "F10")      keyInput->key = GLUT_KEY_F10;
        else if (key == "F11")      keyInput->key = GLUT_KEY_F11;
        else if (key == "F12")      keyInput->key = GLUT_KEY_F12;
        else if (key == "left")     keyInput->key = GLUT_KEY_LEFT;
        else if (key == "up")       keyInput->key = GLUT_KEY_UP;
        else if (key == "right")    keyInput->key = GLUT_KEY_RIGHT;
        else if (key == "down")     keyInput->key = GLUT_KEY_DOWN;
        else if (key == "pageup")   keyInput->key = GLUT_KEY_PAGE_UP;
        else if (key == "pagedown") keyInput->key = GLUT_KEY_PAGE_DOWN;
        else if (key == "home")     keyInput->key = GLUT_KEY_HOME;
        else if (key == "end")      keyInput->key = GLUT_KEY_END;
        else if (key == "insert")   keyInput->key = GLUT_KEY_INSERT;
        else {
            Error("unknown key '%s'", key.c_str());
            return false;
        }

        if (!ParseMod(lst.Slice(1), &(keyInput->mod))) {
            delete keyInput;
            return false;
        }
        *input = keyInput;
        return true;
    }
}


bool ParseMouse(bool isClick, Scm lst, Input **input)
{
    int button;
    int state;
    bool drag;

    // check for correct types
    if (!ScmIsList(lst) || lst.Size() < 2 || !lst.GetScm(0).IsString() ||
        !lst.GetScm(1).IsString())
    {
        Error("bad mouse input");
        return false;
    }

    string buttonStr = Scm2String(lst.GetScm(0));
    string stateStr  = Scm2String(lst.GetScm(1));

    if (buttonStr == "left")        button = GLUT_LEFT_BUTTON;
    else if (buttonStr == "middle") button = GLUT_MIDDLE_BUTTON;
    else if (buttonStr == "right")  button = GLUT_RIGHT_BUTTON;
    else {
        Error("unknown button '%s'", buttonStr.c_str());
        return false;
    }

    if (stateStr == "up") {
        state = GLUT_UP;
        drag = true;
    } else if (stateStr == "down") {
        state = GLUT_DOWN;
        drag = false;
    } else if (stateStr == "click") {
        state = GLUT_UP;
        drag = false;
    } else {
        Error("unknown state '%s'", stateStr.c_str());
        return false;
    }


    if (isClick) {
        MouseClickInput *mouse = new MouseClickInput();
        mouse->button = button;
        mouse->state  = state;
        mouse->drag   = drag;

        if (!ParseMod(lst.Slice(2), &(mouse->mod))) {
            delete mouse;
            return false;
        }
        *input = mouse;
    } else {
        MouseMotionInput *mouse = new MouseMotionInput();
        mouse->button = button;
        mouse->state  = state;

        if (!ParseMod(lst.Slice(2), &(mouse->mod))) {
            delete mouse;
            return false;
        }
        *input = mouse;
    }
    
    return true;
}


bool ParseMod(Scm lst, int *mod)
{
    *mod = 0;
    
    if (!ScmIsList(lst))
        return true;
    
    for (int i=0; i<lst.Size(); i++) {
        if (!lst.GetScm(i).IsString()) {
            Error("modifier must be a string");
            return false;
        }

        string str = Scm2String(lst.GetScm(i));

        if (str == "shift")     *mod = *mod | GLUT_ACTIVE_SHIFT;
        else if (str == "ctrl") *mod = *mod | GLUT_ACTIVE_CTRL;
        else if (str == "alt")  *mod = *mod | GLUT_ACTIVE_ALT;
        else {
            Error("unknown modifer '%s'", str.c_str());
            return false;
        }
    }

    return true;
}



}

