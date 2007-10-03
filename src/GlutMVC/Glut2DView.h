/***************************************************************************
* Summon
* Matt Rasmussen
* Glut2DView.h
*
***************************************************************************/

#ifndef GLUT_2D_VIEW_H
#define GLUT_2D_VIEW_H

#include "GlutView.h"
#include "types.h"

namespace Summon
{

class Glut2DView : public GlutView {
public:
    Glut2DView(int width = 800, int height = 600, const char *name="",
               int left=-1, int top=-1);
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
    inline Vertex2f GetTranslate() { return m_trans; }
    inline Vertex2f GetZoom() { return m_zoom; }
    
    // coordinate system conversions
    inline Vertex2i WindowToScreen(int x, int y)
    {
        return Vertex2i(x, m_windowSize.y - y);
    }

    
    inline Vertex2f ScreenToWorld(int x, int y)
    {
        return Vertex2f((x - m_trans.x - m_focus.x) / m_zoom.x + m_focus.x,
                        (y - m_trans.y - m_focus.y) / m_zoom.y + m_focus.y);
    }

    inline Vertex2i WorldToScreen(float x, float y)
    {
        return Vertex2i(int(m_trans.x + m_focus.x + m_zoom.x * (x - m_focus.x)),
                        int(m_trans.y + m_focus.y + m_zoom.y * (y - m_focus.y)));
    }

    inline Vertex2f WindowToWorld(int x, int y) {
        return Vertex2f((x - m_trans.x - m_focus.x) / m_zoom.x + m_focus.x,
                        (m_windowSize.y - y - m_trans.y - m_focus.y) / m_zoom.y + m_focus.y);
    }

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

