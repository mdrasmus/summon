/***************************************************************************
* Vistools
* Matt Rasmussen
* Glut2DController.cpp
*
***************************************************************************/

#include "first.h"
#include "Glut2DController.h"
#include "glutInputs.h"

namespace Vistools
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

