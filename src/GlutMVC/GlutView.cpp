/***************************************************************************
* Summon
* Matt Rasmussen
* GlutView.cpp
*
***************************************************************************/

#include "first.h"
#include <stdio.h>
#include "GlutView.h"
#include "glut2DCommands.h"

namespace Summon
{

// globals
std::vector<GlutView*> g_windows;


GlutView::GlutView(int width, int height, const char *name) :
   m_windowSize(width, height),
   m_windowOffset(0, 0),
   m_opened(true)
{
    // set initial glut settings for window
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); // | GLUT_ALPHA | GLUT_MULTISAMPLE);
    //glutInitDisplayString( "rgba double samples>=4 ");         
    // NOTE: GLUT_ALPHA had problems on OSX

    glutInitWindowSize(width, height);
    
    // use default position
    glutInitWindowPosition(-1, -1);
    
    
    // create glut window and register it
    m_window = glutCreateWindow(name);
    if ((unsigned int) m_window >= g_windows.size()) {
        g_windows.resize(m_window+1);
    }
    g_windows[m_window] = this;

    // register callbacks
    glutDisplayFunc(GlutView::GlutDisplay);
    glutReshapeFunc(GlutView::GlutReshape);
    
#ifdef GLUT_ACTION_ON_WINDOW_CLOSE
    glutCloseFunc(GlutView::GlutClose);
#endif

    // setup opengl
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
}

GlutView::~GlutView()
{
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


void GlutView::GlutClose()
{
    int winid = glutGetWindow();
    PyGILState_STATE gstate = PyGILState_Ensure();
    g_windows[winid]->OnClose();
    PyGILState_Release(gstate);
}


void GlutView::Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
}

void GlutView::Reshape(int width, int height)
{
    // window has resized
    for (ListenerIter iter = m_listeners.begin(); 
         iter != m_listeners.end(); iter++) {
        (*iter)->ViewResize(this);
    }
    MakeCurrentWindow();    
}


void GlutView::OnClose()
{
    m_opened = false;
    
    // window has been closed
    for (ListenerIter iter = m_listeners.begin(); 
         iter != m_listeners.end(); iter++) {
        GlutViewListener *listener = (*iter);
        listener->ViewClose(this);
    }
    MakeCurrentWindow();
}

void GlutView::Close()
{
    m_opened = false;
    glutDestroyWindow(m_window);
}


void GlutView::UpdatePosition()
{
    // do nothing if window is closed
    if (!m_opened)
        return;


    MakeCurrentWindow();
    Vertex2i newpos(glutGet(GLUT_WINDOW_X) - m_windowOffset.x,
                    glutGet(GLUT_WINDOW_Y) - m_windowOffset.y);
    m_windowPos = newpos;

    if (m_windowPosLastKnown.x != m_windowPos.x || 
        m_windowPosLastKnown.y != m_windowPos.y)
    {
        for (ListenerIter iter = m_listeners.begin(); 
             iter != m_listeners.end(); iter++) {
            GlutViewListener *listener = (*iter);
            listener->ViewMove(this);
        }
    }
    
    m_windowPosLastKnown = m_windowPos;
}


}
