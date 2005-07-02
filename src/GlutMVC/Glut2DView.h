/***************************************************************************
* Vistools
* Matt Rasmussen
* Glut2DView.h
*
***************************************************************************/

#ifndef GLUT_2D_VIEW_H
#define GLUT_2D_VIEW_H

#include "GlutView.h"
#include "types.h"

namespace Vistools
{

class Glut2DView : public GlutView {
public:
    Glut2DView(int width = 800, int height = 600);
    virtual ~Glut2DView();

    virtual void ExecCommand(Command &command);

    // manipulation
    void SetVisible(float x, float y, float x2, float y2);   
    void TranslateBy(float x, float y);
    void TranslateTo(float x, float y);
    void ZoomBy(float x, float y);
    void ZoomTo(float x, float y);
    void SetFocus(float x, float y);

    // accessors
    Vertex2i WindowToScreen(int x, int y);
    Vertex2f ScreenToWorld(int x, int y);
    Vertex2i WorldToScreen(float x, float y);   
    inline Vertex2f WindowToWorld(int x, int y) {
        Vertex2i pt = WindowToScreen(x, y);
        return ScreenToWorld(pt.x, pt.y);
    }
    inline Vertex2f GetTranslate() { return m_trans; }
    inline Vertex2f GetZoom() { return m_zoom; }

    static const float MIN_ZOOM = .01;

protected:
    // callbacks
    virtual void Display();
    virtual void Reshape(int width, int height);

    // drawing
    virtual void DrawWorld();
    virtual void DrawScreen();
    virtual void TransformWorld();

    Vertex2f m_trans;
    Vertex2f m_zoom;
    Vertex2f m_focus;
};

}

#endif

