/***************************************************************************
* Vistools
* Matt Rasmussen
* DrawController.cpp
*
***************************************************************************/

#include "first.h"
#include "Color.h"
#include "DrawController.h"
#include "DrawWindow.h"

namespace Vistools
{


DrawController::DrawController(CommandExecutor *global, DrawView *view, 
                               DrawModel *model) :
    Glut2DController(view),
    m_global(global),
    m_view(view),
    m_model(model)
{
    m_binding->Clear();
}


DrawController::~DrawController()
{}





void DrawController::ExecCommand(Command &command)
{
    // route view commands to connected view
    // XXX: why did I have this greater than GLUT_COMMANDS_BEGIN
    //    To prevent an infinite loop (GLUT commands are processed here)
    if (command.GetId() > GLUT_COMMANDS_END &&
        g_viewAttr.Has(&command))
    {
        m_view->ExecCommand(command);
        return;
    }

    // send visdraw commands that are not for controller to global
    if (command.GetId() > GLUT_COMMANDS_END &&
        !g_controllerAttr.Has(&command))
    {
        m_global->ExecCommand(command);
        return;
    }
    
    // controller commands
    switch (command.GetId()) {
        case SET_BINDING_COMMAND:
            m_binding->AddBinding(((SetBindingCommand*)&command)->input,
                                  ((SetBindingCommand*)&command)->command);
            break;
        
        case CLEAR_BINDING_COMMAND: {
            Input *input = ((SetBindingCommand*)&command)->input;
            m_binding->ClearBinding(*input);
            delete input;
            } break;
        
        case CLEAR_ALL_BINDINGS_COMMAND: {
            m_binding->Clear();
            } break;
        
        case HOTSPOT_CLICK_COMMAND:
            HotspotClick(((HotspotClickCommand*) &command)->pos);
            break;
        
                
        case GET_MOUSE_POS_COMMAND: {
            Vertex2i pos = GetMousePos();
            Vertex2f pos2(-1,-1);
            GetMousePosCommand* cmd = (GetMousePosCommand*) &command;
            
            if (cmd->kind == "world") {
                pos2 = m_view->WindowToWorld(pos.x, pos.y);
            } else if (cmd->kind == "screen") {
                pos2 = m_view->WindowToScreen(pos.x, pos.y);
            } else if (cmd->kind == "window") {
                pos2 = pos;
            } else {
                Error("Unknown coordinate system '%s'", cmd->kind.c_str());
            }
            
            cmd->SetReturn(ScmCons(Float2Scm(pos2.x),
                             ScmCons(Float2Scm(pos2.y), Scm_EOL)));
            } break;
        
        default:
            // execute default commands
            Glut2DController::ExecCommand(command);
    }
}


void DrawController::HotspotClick(Vertex2i pos)
{
    assert(m_model);
    
    Vertex2f pt = m_view->WindowToWorld(pos.x, pos.y);
    
    // get all commands triggered by this click
    list<Command*> cmdList = m_model->HotspotClick(pt);
    
    // execute and free all commands triggered
    for (list<Command*>::iterator i=cmdList.begin(); i!=cmdList.end(); i++)
    {
        ExecCommand(*(*i));
        delete (*i);
    }
}


void DrawController::Motion(int x, int y)
{
    // notify view that mouse has moved
    m_view->SetMousePos(x, y);
    
    // pass motion event to base class
    Glut2DController::Motion(x, y);
}


}
