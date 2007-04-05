/***************************************************************************
* Vistools
* Matt Rasmussen
* Glut2DView.cpp
*
***************************************************************************/

#include "first.h"
#include <algorithm>
#include "Glut2DView.h"
#include "glut2DCommands.h"


namespace Vistools
{

Glut2DView::Glut2DView(int width, int height, const char *name) :
   GlutView(width, height, name),
   m_trans(0, 0),
   m_zoom(1.0, 1.0)
{
   SetVisible(0, 0, 10, 10);
}


Glut2DView::~Glut2DView()
{}


void Glut2DView::ExecCommand(Command &command)
{
    // translate
    switch (command.GetId()) {
        case TRANSLATE_COMMAND: {
            TranslateBy(((TranslateCommand*)(&command))->trans.x, 
                        ((TranslateCommand*)(&command))->trans.y);    
            RedisplayCommand redisplay;
            GlutView::ExecCommand(redisplay);
            } break;
    
        case ZOOM_COMMAND: {
            ZoomBy(((ZoomCommand*) &command)->zoom.x,
                   ((ZoomCommand*) &command)->zoom.y);
            RedisplayCommand redisplay;
            GlutView::ExecCommand(redisplay);
            } break;
        
        case ZOOM_X_COMMAND: {
            ZoomBy(((ZoomCommand*) &command)->zoom.x, 1.0);
            RedisplayCommand redisplay;
            GlutView::ExecCommand(redisplay);
            } break;
        
        case ZOOM_Y_COMMAND: {
            ZoomBy(1.0, ((ZoomCommand*) &command)->zoom.y);
            RedisplayCommand redisplay;
            GlutView::ExecCommand(redisplay);
            } break;
            
        case FOCUS_COMMAND: {
            FocusCommand* cmd = (FocusCommand*) &command;
            Vertex2i pt = WindowToScreen(cmd->focus.x, cmd->focus.y);
            Vertex2f focus = ScreenToWorld(pt.x, pt.y);
            SetFocus(focus.x, focus.y);
            } break;
        default:
            GlutView::ExecCommand(command);
    }
}




// callbacks
void Glut2DView::Display()
{
   // clear screen and initialize coordinate system
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   // draw world in its own coordinate system
   glPushMatrix();
   TransformWorld();
   DrawWorld();
   glPopMatrix();   
   
   // draw graphics with the screen coordinate system
   DrawScreen();
   
   // display new frame
   glutSwapBuffers();
}


void Glut2DView::TransformWorld()
{
   // perform translation
   glTranslatef(m_trans.x , m_trans.y, 0);
   
   // perform a zoom with respect to a focus point
   glTranslatef(m_focus.x, m_focus.y, 0);
   glScalef(m_zoom.x, m_zoom.y, 1.0);
   glTranslatef(-m_focus.x, -m_focus.y, 0);
}

void Glut2DView::Reshape(int w, int h)
{
   // find screen centers before and after resize
   Vertex2f center(m_windowSize.x / 2.0, m_windowSize.y / 2.0);
   Vertex2f center2(w / 2.0, h / 2.0);
   Vertex2f wcenter = ScreenToWorld((int) center.x, (int) center.y);
   
   // move the focus to center of screen
   SetFocus(wcenter.x, wcenter.y);
   
   // find scaling factor for resizing
   float len1 = std::min(center.x, center.y);
   float len2 = std::min(center2.x, center2.y);
   float scale = len2 / len1;
   
   // change zoom and translation in order to keep world centered and in view
   m_trans = m_trans + center2 - center;
   m_zoom = m_zoom * scale;
   
   // set view port and window size
   glViewport(0, 0, w, h);
   m_windowSize = Vertex2i(w, h);
   
   // set projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, w, 0, h, -1.0, 1.0);
}


// drawing
void Glut2DView::DrawWorld()
{
   // default testing drawing
   glColor3f(1, 1, 1);
   glBegin(GL_LINES);
      glVertex3f(0, 0, 0);
      glVertex3f(10, 10, 0);
      glVertex3f(0, 10, 0);
      glVertex3f(10, 0, 0);
   glEnd();

   glTranslatef(m_focus.x, m_focus.y, 0);

   glColor3f(1, 0, 0);
   glBegin(GL_LINES);
      glVertex3f(0, -5, 0);
      glVertex3f(0, 5, 0);
      glVertex3f(5, 0, 0);
      glVertex3f(-5, 0, 0);
   glEnd();   
}

void Glut2DView::DrawScreen()
{
   // default testing drawing
   glColor3f(1, 1, 0);
   glBegin(GL_LINES);
      glVertex3f(0, -10, 0);
      glVertex3f(0, 10, 0);
      glVertex3f(10, 0, 0);
      glVertex3f(-10, 0, 0);
   glEnd();
}




// manipulation
void Glut2DView::SetVisible(float x, float y, float x2, float y2)
{
    if (m_windowSize.x == 0 || m_windowSize.y == 0)
        return;
        
    Vertex2f worldBottom(std::min(x, x2), std::min(y, y2));
    Vertex2f worldTop(std::max(x, x2), std::max(y, y2));
    Vertex2f worldSize = worldTop - worldBottom;
    Vertex2f worldCenter = (worldTop + worldBottom);
    worldCenter.x /= 2.0;
    worldCenter.y /= 2.0;
    
    // can't zoom to fit window on a point
    if (worldSize.x == 0.0 || worldSize.y == 0.0)
        return;

    // determine whether the heights of the world and window are tight
    bool heightTight;
    float winAspect = m_windowSize.x / float(m_windowSize.y);
    
    if (worldSize.y == 0.0) {
        heightTight = false;
    } else {
       // find proper world rect based on window aspect ratio       
       float worldAspect = worldSize.x / worldSize.y;
       
       heightTight = winAspect > worldAspect;
    }
    
    // adjust worldBottom and worldTop to fit dimensions of window
    if (heightTight) {
        // adjust left and right
        worldBottom.x = worldCenter.x - .5 * winAspect * worldSize.y;
        worldTop.x    = worldCenter.x + .5 * winAspect * worldSize.y;
    } else {
        worldBottom.y = worldCenter.y - .5 * worldSize.x / winAspect;
        worldTop.y    = worldCenter.y + .5 * worldSize.x / winAspect;
    }
    
    // set trans, focus, and zoom
    m_trans = Vertex2f(-worldBottom.x, -worldBottom.y);
    m_focus = worldBottom;
    if (heightTight) {
        m_zoom.x  = m_windowSize.y / worldSize.y;
        m_zoom.y  = m_windowSize.y / worldSize.y;
    } else {
        m_zoom.x  = m_windowSize.x / worldSize.x;
        m_zoom.y  = m_windowSize.x / worldSize.x;    
    }
}

void Glut2DView::TranslateBy(float x, float y)
{
   m_trans = m_trans - Vertex2f(x, y);
}


void Glut2DView::TranslateTo(float x, float y)
{
   m_trans = Vertex2f(x, y);
}


void Glut2DView::ZoomBy(float x, float y)
{
   m_zoom.x *= x;
   m_zoom.y *= y;
}


void Glut2DView::ZoomTo(float x, float y)
{
   m_zoom = Vertex2f(x, y);  
}

void Glut2DView::SetFocus(float x, float y) { 
   Vertex2f focus2 = Vertex2f(x, y);
   m_trans.x = m_trans.x + (m_focus.x - focus2.x) * (1 - m_zoom.x);
   m_trans.y = m_trans.y + (m_focus.y - focus2.y) * (1 - m_zoom.y);   
   m_focus = focus2;
}

}
