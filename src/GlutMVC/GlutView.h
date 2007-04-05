/***************************************************************************
* Vistools
* Matt Rasmussen
* GlutView.h
*
***************************************************************************/

#ifndef GLUT_VIEW_H
#define GLUT_VIEW_H

#include <vector>
#include <string>
#include <GL/glut.h>
#include "types.h"
#include "View.h"
#include "Command.h"


namespace Vistools
{

using namespace std;

class GlutView : public View {
public:
    GlutView(int width = 800, int height = 600, const char *name="");
    virtual ~GlutView();

    virtual void ExecCommand(Command &command);

    virtual void Resize(int width, int height)
    {
        MakeCurrentWindow(); 
        glutReshapeWindow(width, height);
        Reshape(width, height);
        m_windowSize = Vertex2i(width, height);
        glutPostRedisplay();
    }

    // accessors   
    inline int GetGlutWindow() { return m_window; }
    inline Vertex2i GetWindowSize() { return m_windowSize; }

    // glut callbacks
    static void GlutDisplay();
    static void GlutReshape(int width, int height);

    inline void SetName(char *name) {
        m_name = name;
        glutSetWindowTitle(name);
    }
    
    inline string GetName()
    { return m_name; }

protected:   
    inline void MakeCurrentWindow() { glutSetWindow(m_window); }

    // callbacks
    virtual void Display();
    virtual void Reshape(int width, int height);

    int m_window;
    Vertex2i m_windowSize;
    string m_name;
};

}

#endif

