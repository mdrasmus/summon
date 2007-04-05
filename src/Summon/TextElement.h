/***************************************************************************
* Vistools
* Matt Rasmussen
* TextElement.h
*
***************************************************************************/

#ifndef TEXT_ELEMENT_H
#define TEXT_ELEMENT_H

#include "Script.h"
#include <algorithm>
#include <string>
#include "types.h"
#include "Element.h"


namespace Vistools {

class TextElement : public Element
{
public:
    TextElement() : 
        Element(TEXT_CONSTRUCT),
        minHeight(0.0),
        maxHeight(0.0)
    {
        m_dynamic = true;
    }
    
    enum {
        KIND_BITMAP,
        KIND_SCALE,
        KIND_CLIP
    };
    
    enum {
        LEFT    = 1,
        CENTER  = 2,
        RIGHT   = 4,
        TOP     = 8,
        MIDDLE  = 16,
        BOTTOM  = 32,
        VERTICAL = 64
    };
    
    void SetRect(Vertex2f &p1, Vertex2f &p2) {
        pos1 = p1;
        pos2 = p2;
        if (pos1.x > pos2.x) swap(pos1.x, pos2.x);
        if (pos1.y > pos2.y) swap(pos1.y, pos2.y);
    }
    
    string text;
    Vertex2f pos1;
    Vertex2f pos2;
    Vertex2f envpos1;
    Vertex2f envpos2;
    Vertex2f scale;
    int justified;
    int kind;
    float minHeight;
    float maxHeight;
};


}

#endif
