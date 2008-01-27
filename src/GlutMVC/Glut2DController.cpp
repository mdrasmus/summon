/***************************************************************************
* Summon
* Matt Rasmussen
* Glut2DController.cpp
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
#include "Glut2DController.h"
#include "glutInputs.h"

namespace Summon
{


Glut2DController::Glut2DController(Glut2DView *view) :
   GlutController(view->GetGlutWindow()),
   m_view(view)
{
    // default bindings

    // translation
    MouseMotionInput *transInput = new MouseMotionInput();
    transInput->button = GLUT_LEFT_BUTTON;
    transInput->state  = GLUT_DOWN;
    
    m_defaultBinding.AddBinding(transInput, TRANSLATE_COMMAND);
    
    // zoom
    MouseMotionInput *zoomInput = new MouseMotionInput();
    zoomInput->button = GLUT_RIGHT_BUTTON;
    zoomInput->state  = GLUT_DOWN;
    
    m_defaultBinding.AddBinding(zoomInput, ZOOM_COMMAND);
    
    // focus
    MouseClickInput *focusInput = new MouseClickInput();
    focusInput->button = GLUT_RIGHT_BUTTON;
    focusInput->state  = GLUT_DOWN;
    
    m_defaultBinding.AddBinding(focusInput, FOCUS_COMMAND);
}

Glut2DController::~Glut2DController()
{}


void Glut2DController::ExecCommand(Command &command)
{
    m_view->ExecCommand(command);
}


}

