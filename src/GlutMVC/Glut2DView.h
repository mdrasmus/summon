/***************************************************************************
* Summon
* Matt Rasmussen
* Glut2DView.h
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

#ifndef SUMMON_GLUT_2D_VIEW_H
#define SUMMON_GLUT_2D_VIEW_H

#include "GlutView.h"
#include "Vertex.h"

namespace Summon
{

inline float getTextHeight(const void *font)
{ 
    return 119.05;
}

inline float getTextWidth(const void *font, const unsigned char *text)
{
    float error = 1.0;
    
    // NOTE: some glut implementations have a bug in calculating text width
    // This works around that bug
    if (font == GLUT_STROKE_MONO_ROMAN) {
        float asize = glutStrokeLength((void*) font, (const unsigned char*) "AAAAAAAAAA");
        error = 1048.0 / asize;
    } else {
        error = 1.0;
    }
    
    return glutStrokeLength((void*) font, text) * error;
}


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




} // namespace Summon

#endif

