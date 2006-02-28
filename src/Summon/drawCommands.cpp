/***************************************************************************
* Vistools
* Matt Rasmussen
* drawCommands.cpp
*
***************************************************************************/

#include "first.h"
#include "drawCommands.h"
#include "glut2DCommands.h"


namespace Vistools
{

using namespace std;

CommandAttr g_constructAttr;
CommandAttr g_modelAttr;
CommandAttr g_viewAttr;
CommandAttr g_controllerAttr;



void drawCommandsInit()
{
    commonCommandsInit();
    glutCommandsInit();
    
#   define g() AddAttr(g_globalAttr)
#   define m() AddAttr(g_modelAttr)    
#   define v() AddAttr(g_viewAttr)    
#   define c() AddAttr(g_controllerAttr)
    
    // global commands
    RegisterScriptCommand(GetWindowsCommand)    g()
    RegisterScriptCommand(GetWindowCommand)     g()
    RegisterScriptCommand(NewWindowCommand)     g()
    RegisterScriptCommand(SetWindowCommand)     g()
    RegisterScriptCommand(CloseWindowCommand)   g()
    RegisterScriptCommand(GetModelsCommand)     g()
    RegisterScriptCommand(GetModelCommand)      g()
    RegisterScriptCommand(NewModelCommand)      g()
    RegisterScriptCommand(AssignModelCommand)   g()
    RegisterScriptCommand(SetModelCommand)      g()
    RegisterScriptCommand(DelModelCommand)      g()
    RegisterScriptCommand(TimerCallCommand)     g()
    RegisterScriptCommand(RedrawCallCommand)    g()
    
    // model commands
    RegisterScriptCommand(AddGroupCommand)      m()
    RegisterScriptCommand(InsertGroupCommand)   m()
    RegisterScriptCommand(RemoveGroupCommand)   m()
    RegisterScriptCommand(ReplaceGroupCommand)  m()
    RegisterScriptCommand(ClearGroupsCommand)   m()
    RegisterScriptCommand(ShowGroupCommand)     m()
    RegisterScriptCommand(GetGroupCommand)      m()
    RegisterScriptCommand(GetRootIdCommand)     m()
    
    // view commands                      
    RegisterScriptCommand(HomeCommand)          v()
    RegisterScriptCommand(SetBgColorCommand)    v()
    RegisterScriptCommand(GetBgColorCommand)    v()
    RegisterScriptCommand(SetVisibleCommand)    v()
    RegisterScriptCommand(GetVisibleCommand)    v()
    RegisterScriptCommand(SetWindowSizeCommand) v()
    RegisterScriptCommand(GetWindowSizeCommand) v()
    RegisterScriptCommand(SetAntialiasCommand)  v()    

    // controller commands    
    RegisterScriptCommand(SetBindingCommand)       c()
    RegisterScriptCommand(ClearBindingCommand)     c()
    RegisterScriptCommand(ClearAllBindingsCommand) c()
    RegisterScriptCommand(HotspotClickCommand)     c()
    RegisterScriptCommand(GetMousePosCommand)      c()
    
    // constructs
    
    // structure
    RegisterConstruct(GroupConstruct)
    RegisterConstruct(DynamicGroupConstruct)
    RegisterConstruct(HotspotConstruct)
    
    // graphics
    RegisterConstruct(PointsConstruct)
    RegisterConstruct(LinesConstruct)
    RegisterConstruct(LineStripConstruct)    
    RegisterConstruct(TrianglesConstruct)
    RegisterConstruct(TriangleStripConstruct)
    RegisterConstruct(TriangleFanConstruct)    
    RegisterConstruct(QuadsConstruct)
    RegisterConstruct(QuadStripConstruct)
    RegisterConstruct(PolygonConstruct)
    RegisterConstruct(TextConstruct)
    RegisterConstruct(TextScaleConstruct)    
    RegisterConstruct(TextClipConstruct)
    
    // primitives
    RegisterConstruct(VerticesConstruct)
    RegisterConstruct(ColorConstruct)
    
    // transforms
    RegisterConstruct(TranslateConstruct)
    RegisterConstruct(RotateConstruct)
    RegisterConstruct(ScaleConstruct)
    RegisterConstruct(FlipConstruct)
    
    // inputs
    RegisterConstruct(InputKeyConstruct)
    RegisterConstruct(InputClickConstruct)
    RegisterConstruct(InputMotionConstruct)

#   undef g
#   undef m
#   undef v
#   undef c

}

bool ParseScm(string errorMsg, Scm lst, const char *fmt, ...)
{
    va_list ap;   
    
    bool status = true;
    int *d;
    float *f;
    string *str;
    Scm *proc;
    Scm *code;
    bool *b;
    
    va_start(ap, fmt);

       
    // loop through format string
    int i=1;
    for (const char *argtype = fmt; *argtype; argtype++, i++) {
        // ensure lst is a list
        if (!ScmConsp(lst)) {
            status = false;
            break;
        }
        
        
        // get next arg in lst
        Scm arg = ScmCar(lst);
        lst = ScmCdr(lst);
        
        switch (*argtype) {
            case 'd':
                if (!ScmIntp(arg)) {
                    Error("expected integer for argument %d", i);
                    status = false;
                    break;
                }
                
                d = va_arg(ap, int *);
                *d = Scm2Int(arg);
                break;
            case 'f':
                if (!ScmFloatp(arg)) {
                    Error("expected float for argument %d", i);
                    status = false;
                    break;
                }
                
                f = va_arg(ap, float *);
                *f = Scm2Float(arg);
                break;
            case 's':
                if (!ScmStringp(arg)) {
                    Error("expected string for argument %d", i);
                    status = false;
                    break;
                }
                
                str = va_arg(ap, string *);
                *str = Scm2String(arg);
                break;
            case 'b':
                b = va_arg(ap, bool *);
                *b = (arg != Scm_FALSE);
                break;
            case 'p':
                if (!ScmProcedurep(arg)) {
                    Error("expected procedure for argument %d", i);
                    status = false;
                    break;
                }
                
                proc = va_arg(ap, Scm *);
                *proc = arg;
                break;
            case 'c':
                code = va_arg(ap, Scm *);
                *code = arg;
                break;
        }
    }
    
    va_end(ap);
    
    if (!status) {
        Error(errorMsg.c_str());
    }
    
    return status;
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
        Error("command must be command name or scheme procedure");
        return false;
    }

    return true;
}


bool ParseInput(Scm lst, Input **input)
{
    if (!ScmConsp(lst) || !ScmIntp(ScmCar(lst))) {
        Error("Bad input");
        return false;
    }

    int header = Scm2Int(ScmCar(lst));
    switch (header) {
        case INPUT_KEY_CONSTRUCT: 
            return ParseKeyInput(ScmCdr(lst), input);
        case INPUT_CLICK_CONSTRUCT:
            return ParseMouse(true, ScmCdr(lst), input);
        case INPUT_MOTION_CONSTRUCT:
            return ParseMouse(false, ScmCdr(lst), input);
        default:
            Error("unknown input");
            return false;
    }
}

bool ParseKeyInput(Scm lst, Input **input)
{
    if (!ScmConsp(lst) || !ScmStringp(ScmCar(lst))) {
        Error("input key must be given as a string");
        return false;
    }

    string key = Scm2String(ScmCar(lst));

    if (key.size() == 1) {
        KeyboardInput *keyInput = new KeyboardInput();
        keyInput->key = key[0];

        if (!ParseMod(ScmCdr(lst), &(keyInput->mod))) {
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

        if (!ParseMod(ScmCdr(lst), &(keyInput->mod))) {
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

    // check for correct types
    if (!ScmConsp(lst) || !ScmStringp(ScmCar(lst)) ||
        !ScmConsp(ScmCdr(lst)) || !ScmStringp(ScmCadr(lst)))
    {
        Error("bad mouse input");
        return false;
    }

    string buttonStr = Scm2String(ScmCar(lst));
    string stateStr  = Scm2String(ScmCadr(lst));

    if (buttonStr == "left")        button = GLUT_LEFT_BUTTON;
    else if (buttonStr == "middle") button = GLUT_MIDDLE_BUTTON;
    else if (buttonStr == "right")  button = GLUT_RIGHT_BUTTON;
    else {
        Error("unknown button '%s'", buttonStr.c_str());
        return false;
    }

    if (stateStr == "up")        state = GLUT_UP;
    else if (stateStr == "down") state = GLUT_DOWN;
    else {
        Error("unknown state '%s'", stateStr.c_str());
        return false;
    }


    if (isClick) {
        MouseClickInput *mouse = new MouseClickInput();
        mouse->button = button;
        mouse->state  = state;

        if (!ParseMod(ScmCddr(lst), &(mouse->mod))) {
            delete mouse;
            return false;
        }
        *input = mouse;
    } else {
        MouseMotionInput *mouse = new MouseMotionInput();
        mouse->button = button;
        mouse->state  = state;

        if (!ParseMod(ScmCddr(lst), &(mouse->mod))) {
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

    for (; ScmConsp(lst); lst = ScmCdr(lst)) {
        if (!ScmStringp(ScmCar(lst))) {
            Error("modifier must be a string");
            return false;
        }

        string str = Scm2String(ScmCar(lst));

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

