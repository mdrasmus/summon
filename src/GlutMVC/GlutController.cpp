/***************************************************************************
* Vistools
* Matt Rasmussen
* GlutController.cpp
*
***************************************************************************/

#include "first.h"
#include <vector>
#include <GL/glut.h>
#include "GlutController.h"
#include "glutInputs.h"
#include "Command.h"

namespace Vistools
{

std::vector<GlutController*> g_controllers;


GlutController::GlutController(int window):
    m_button(0),
    m_state(GLUT_UP),
    m_mod(0)
{
    if ((unsigned int) window >= g_controllers.size()) {
        g_controllers.resize(window+1);
    }
    g_controllers[window] = this;

    // register callbacks
    glutKeyboardFunc(GlutController::GlutKey);
    glutSpecialFunc(GlutController::GlutSpecialKey);
    glutMotionFunc(GlutController::GlutMotion);
    glutPassiveMotionFunc(GlutController::GlutMotion);
    glutMouseFunc(GlutController::GlutMouseClick);
}

GlutController::~GlutController()
{
}

void GlutController::AddKeyBinding(char key, CommandId cmd)
{
    KeyboardInput *input = new KeyboardInput();
    input->key = key;
    m_defaultBinding.AddBinding(input, cmd);
}

Vertex2i GlutController::GetMousePos()
{
    return m_lastMouse;
}
   
void GlutController::GlutKey(unsigned char key, int x, int y)
{
    PyGILState_STATE gstate = PyGILState_Ensure();
    g_controllers[glutGetWindow()]->Key(key, x, y);
    PyGILState_Release(gstate);
}

void GlutController::GlutSpecialKey(int key, int x, int y)
{
    PyGILState_STATE gstate = PyGILState_Ensure();
    g_controllers[glutGetWindow()]->SpecialKey(key, x, y);
    PyGILState_Release(gstate);    
}

void GlutController::GlutMotion(int x, int y)
{
    PyGILState_STATE gstate = PyGILState_Ensure();
    g_controllers[glutGetWindow()]->Motion(x, y);
    PyGILState_Release(gstate);
}

void GlutController::GlutMouseClick(int button, int state, int x, int y)
{
    PyGILState_STATE gstate = PyGILState_Ensure();
    g_controllers[glutGetWindow()]->MouseClick(button, state, x, y);
    PyGILState_Release(gstate);    
}


// callbacks
void GlutController::Key(unsigned char key, int x, int y)
{
    KeyboardInput input;
    input.key = key;
    input.pos = Vertex2i(x, y);
    input.mod = glutGetModifiers();
    
    // adjust key if needed
    if (input.mod & GLUT_ACTIVE_CTRL && input.key <= 26) {
        input.key += 96;
    }

    ProcessInput(input);

    m_lastMouse = input.pos;
}


void GlutController::SpecialKey(int key, int x, int y)
{
    SpecialKeyInput input;
    input.key = key;
    input.pos = Vertex2i(x, y);
    input.mod = glutGetModifiers();

    ProcessInput(input);

    m_lastMouse = input.pos;
}


void GlutController::Motion(int x, int y)
{
    MouseMotionInput input;
    input.pos    = Vertex2i(x, y);
    input.vel    = m_lastMouse - input.pos;
    input.button = m_button;
    input.state  = m_state;
    input.mod    = m_mod;

    ProcessInput(input);

    m_lastMouse = input.pos;
}


void GlutController::MouseClick(int button, int state, int x, int y)
{
    m_button = button;
    m_state  = state;
    m_mod    = glutGetModifiers();

    MouseClickInput input;
    input.pos    = Vertex2i(x, y);
    input.button = m_button;
    input.state  = m_state;
    input.mod    = m_mod; 

    ProcessInput(input);

    m_lastMouse = input.pos;
}



} // namespace Vistools
