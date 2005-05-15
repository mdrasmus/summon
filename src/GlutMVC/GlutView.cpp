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


GlutView::GlutView(int width, int height) :
   m_windowSize(width, height)
{
    // set initial glut settings for window
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(width, height);

    // create glut window and register it
    m_window = glutCreateWindow("");
    if (m_window >= g_windows.size()) {
        g_windows.resize(m_window+1);
    }
    g_windows[m_window] = this;

    // register callbacks
    glutDisplayFunc(GlutView::GlutDisplay);
    glutReshapeFunc(GlutView::GlutReshape);

    // setup opengl
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    g_windows[glutGetWindow()]->Display();
}

void GlutView::GlutReshape(int width, int height)
{
    g_windows[glutGetWindow()]->Reshape(width, height);
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
