/***************************************************************************
* Summon
* Matt Rasmussen
* GlutView.cpp
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
#include <stdio.h>
#include "GlutView.h"
#include "glut2DCommands.h"

namespace Summon
{

// globals
std::vector<GlutView*> g_windows;


GlutView::GlutView(int width, int height, const char *name, 
                   int left, int top) :
   m_windowSize(width, height),
   m_windowPos(left, top),   
   m_windowOffset(0, 0),
   m_opened(true)
{
    // set initial glut settings for window

#   ifdef GLUT_MULTISAMPLE
        // try to use multisample, freeglut usually doesn't support it
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE |
        GLUT_DEPTH);
#   else
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); // | GLUT_ALPHA | GLUT_MULTISAMPLE);
#   endif
    //glutInitDisplayString( "rgba double samples>=4 ");         
    // NOTE: GLUT_ALPHA had problems on OSX

    // set initial window size and position
    glutInitWindowSize(width, height);
    glutInitWindowPosition(left, top);
    
    
    // create glut window and register it
    m_window = glutCreateWindow(name);
    if ((unsigned int) m_window >= g_windows.size()) {
        g_windows.resize(m_window+1);
    }
    g_windows[m_window] = this;

    // register callbacks
    glutDisplayFunc(GlutView::GlutDisplay);
    glutReshapeFunc(GlutView::GlutReshape);
    
#   ifdef GLUT_ACTION_ON_WINDOW_CLOSE
        glutCloseFunc(GlutView::GlutClose);
#   endif

    // setup opengl
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
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


//=============================================================================
// Object-oriented callbacks

void GlutView::Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
}

void GlutView::Reshape(int width, int height)
{
    m_windowSize.x = width;
    m_windowSize.y = height;

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
    m_windowPos.x = glutGet(GLUT_WINDOW_X) - m_windowOffset.x;
    m_windowPos.y = glutGet(GLUT_WINDOW_Y) - m_windowOffset.y;

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


//=============================================================================
// GLUT static callbacks

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
    int winid = glutGetWindow();  // TODO: I wonder why I placed this here
    PyGILState_STATE gstate = PyGILState_Ensure();
    g_windows[winid]->OnClose();
    PyGILState_Release(gstate);
}


}
