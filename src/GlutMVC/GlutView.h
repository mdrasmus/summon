/***************************************************************************
* Summon
* Matt Rasmussen
* GlutView.h
*
***************************************************************************/

#ifndef SUMMON_GLUT_VIEW_H
#define SUMMON_GLUT_VIEW_H

#include <vector>
#include <string>
#include <list>
#include <GL/glut.h>
#include "Vertex.h"
#include "View.h"
#include "Command.h"


// Use FREEGLUT extensions
#ifndef NOGLUTEXT
#  include <GL/freeglut_ext.h>
#endif


namespace Summon
{

using namespace std;

// forward declaration
class GlutView;

class GlutViewListener {
public:
    virtual ~GlutViewListener() {}
    virtual void ViewRedraw(GlutView *view) {}
    virtual void ViewResize(GlutView *view) {}
    virtual void ViewClose(GlutView *view) {}
    virtual void ViewMove(GlutView *view) {}
};


class GlutView : public View {
public:
    GlutView(int width = 800, int height = 600, const char *name="",
             int left=-1, int top=-1);
    virtual ~GlutView();

    virtual void ExecCommand(Command &command);

    // TODO: think more about how window resizing is treated with threads
    virtual void Resize(int width, int height)
    {
        MakeCurrentWindow(); 
        glutReshapeWindow(width, height);
        m_windowSize.x = width;
        m_windowSize.y = height;
        glutPostRedisplay();
    }

    // accessors   
    inline int GetGlutWindow() { return m_window; }
    inline Vertex2i GetWindowSize() { return m_windowSize; }

    // glut callbacks
    static void GlutDisplay();
    static void GlutReshape(int width, int height);
    static void GlutClose();
    
    virtual void Close();
    
    inline void SetName(char *name) {
        m_name = name;
        glutSetWindowTitle(name);
    }
    
    inline string GetName()
    { return m_name; }
    
    inline void SetPosition(int x, int y)
    {
        m_windowPos.x = x;
        m_windowPos.y = y;  
        MakeCurrentWindow();        
        glutPositionWindow(x, y);          
    }
    
    inline Vertex2i GetPosition()
    { return m_windowPos; }

    
    void UpdatePosition();
    
    inline void SetOffset(Vertex2i &offset)
    { m_windowOffset = offset; }
    
    inline void AddListener(GlutViewListener *view) {
        m_listeners.push_back(view);
    }
    inline void RemoveListener(GlutViewListener *view) {
        m_listeners.remove(view);
    }
    
    typedef std::list<GlutViewListener*> ListenerList;
    typedef ListenerList::iterator ListenerIter;

protected:   
    inline void MakeCurrentWindow() { glutSetWindow(m_window); }

    // callbacks
    virtual void Display();
    virtual void Reshape(int width, int height);
    virtual void OnClose();

    int m_window;
    Vertex2i m_windowSize;
    Vertex2i m_windowPos;
    Vertex2i m_windowPosLastKnown;
    Vertex2i m_windowOffset;    // GLUT sometimes places the window offset
    string m_name;
    ListenerList m_listeners;
    bool m_opened;
};

}

#endif

