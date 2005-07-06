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
    // send visdraw commands that are not for controller to global
    if (command.GetId() > COMMON_COMMANDS_BEGIN &&
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


}
