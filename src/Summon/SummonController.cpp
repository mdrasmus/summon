/***************************************************************************
* Summon
* Matt Rasmussen
* SummonController.cpp
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
#include "Color.h"
#include "SummonController.h"
#include "SummonWindow.h"
#include "Element.h"
#include "Hotspot.h"


namespace Summon
{


SummonController::SummonController(CommandExecutor *global, SummonView *view, 
                               SummonModel *world, SummonModel *screen) :
    Glut2DController(view),
    m_global(global),
    m_view(view),
    m_world(world),
    m_screen(screen),
    m_resizeProc(Scm_NONE),
    m_moveProc(Scm_NONE)
{
    m_binding->Clear();
    m_view->AddListener(this);
}


SummonController::~SummonController()
{}





void SummonController::ExecCommand(Command &command)
{
    // route view commands to connected view
    if (g_viewAttr.Has(&command))
    {
        m_view->ExecCommand(command);
        return;
    }

    // send SUMMON commands that are not for controller to global
    // NOTE: why did I have this greater than GLUT_COMMANDS_BEGIN
    //    To prevent an infinite loop (GLUT commands are processed here)    
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
            Input *input = ((ClearBindingCommand*)&command)->input;
            m_binding->ClearBinding(*input);
            delete input;
            } break;
        
        case CLEAR_ALL_BINDINGS_COMMAND: {
            m_binding->Clear();
            } break;
        
        case HOTSPOT_CLICK_COMMAND:
            HotspotClick(((HotspotClickCommand*) &command)->pos, 
                         Hotspot::CLICK);
            break;
            
        case HOTSPOT_DRAG_COMMAND:
            HotspotClick(((HotspotClickCommand*) &command)->pos, 
                         Hotspot::DRAG);
            break;
            
        case HOTSPOT_DRAG_START_COMMAND:
            HotspotClick(((HotspotClickCommand*) &command)->pos,
                         Hotspot::DRAG_START);
            break;
            
        case HOTSPOT_DRAG_STOP_COMMAND:
            HotspotClick(((HotspotClickCommand*) &command)->pos,
                         Hotspot::DRAG_STOP);
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
        
                
        case SET_WINDOW_ON_RESIZE_COMMAND: {
            SetWindowOnResizeCommand *resize = (SetWindowOnResizeCommand*) &command;
            m_resizeProc = resize->proc;
            } break;
        
        case SET_WINDOW_ON_MOVE_COMMAND: {
            SetWindowOnMoveCommand *move = (SetWindowOnMoveCommand*) &command;
            m_moveProc = move->proc;
            } break;
        
        default:
            // execute default commands
            Glut2DController::ExecCommand(command);
    }
}


void SummonController::HotspotClick(const Vertex2i &pos, int kind)
{
    bool clicked = false;

    const Camera screenCamera(Vertex2f(0, 0), Vertex2f(1, 1), Vertex2f(0, 0));
    const Camera worldCamera = m_view->GetCamera();

    // process hotspots in screen
    if (m_screen) {
        // get all commands triggered by this click
        Vertex2i pt = m_view->WindowToScreen(pos.x, pos.y);
        Vertex2f pt2 = Vertex2f(pt.x, pt.y);
        list<Command*> cmdList = m_screen->HotspotClick(pt2, screenCamera, kind);

        // execute and free all commands triggered
        for (list<Command*>::iterator i=cmdList.begin(); i!=cmdList.end(); i++)
        {
            clicked = true;
            ExecCommand(*(*i));
            delete (*i);
        }
    }

    // process hotspots in world (only if no hotspots clicked in screen)
    if (m_world && !clicked) {
        // get all commands triggered by this click    
        Vertex2f pt = m_view->WindowToWorld(pos.x, pos.y);
        list<Command*> cmdList = m_world->HotspotClick(pt, worldCamera, kind);
        
        // execute and free all commands triggered
        for (list<Command*>::iterator i=cmdList.begin(); i!=cmdList.end(); i++)
        {
            ExecCommand(*(*i));
            delete (*i);
        }
    }
    
    
}


void SummonController::PassiveMotion(int x, int y)
{
    // pass motion event to base class
    Glut2DController::PassiveMotion(x, y);
    
    // notify view that mouse has moved
    m_view->SetMousePos(x, y);
}


void SummonController::ViewResize(GlutView *view)
{
    if (m_resizeProc != Scm_NONE) {
        CallProcCommand proc(m_resizeProc);
        ExecCommand(proc);
    }
}

void SummonController::ViewMove(GlutView *view)
{
    if (m_moveProc != Scm_NONE) {
        CallProcCommand proc(m_moveProc);
        ExecCommand(proc);
    }
}

}
