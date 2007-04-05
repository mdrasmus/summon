/***************************************************************************
* Vistools
* Matt Rasmussen
* GlutView.cpp
*
***************************************************************************/

#include "first.h"
#include <stdio.h>
#include "GlutView.h"
#include "glut2DCommands.h"

namespace Vistools
{

// globals
std::vector<GlutView*> g_windows;


GlutView::GlutView(int width, int height, const char *name) :
   m_windowSize(width, height)
{
    // set initial glut settings for window
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_MULTISAMPLE);
    glutInitWindowSize(width, height);

    // create glut window and register it
    m_window = glutCreateWindow(name);
    if ((unsigned int) m_window >= g_windows.size()) {
        g_windows.resize(m_window+1);
    }
    g_windows[m_window] = this;

    // register callbacks
    glutDisplayFunc(GlutView::GlutDisplay);
    glutReshapeFunc(GlutView::GlutReshape);

    // setup opengl
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
}

GlutView::~GlutView()
{
    glutDestroyWindow(m_window);
}
   
void GlutView::ExecCommand(Command &command)
{
    switch (command.GetId()) {
        case REDISPLAY_COMMAND:
            MakeCurrentWindow();
            glutPostRedisplay();
    }
}

void GlutView::GlutDisplay()
{
    PyGILState_STATE gstate = PyGILState_Ensure();
    g_windows[glutGetWindow()]->Display();
    PyGILState_Release(gstate);
}

void GlutView::GlutReshape(int width, int height)
{
    PyGILState_STATE gstate = PyGILState_Ensure();
    g_windows[glutGetWindow()]->Reshape(width, height);
    PyGILState_Release(gstate);
}

void GlutView::Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
}

void GlutView::Reshape(int width, int height)
{
   
}




}
