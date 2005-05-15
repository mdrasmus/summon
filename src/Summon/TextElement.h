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
        Element(TEXT_CONSTRUCT) 
    {
        m_dynamic = true;
    }
    
    enum {
        KIND_BITMAP,
        KIND_SCALE
    };
    
    enum {
        LEFT    = 1,
        CENTER  = 2,
        RIGHT   = 4,
        TOP     = 8,
        MIDDLE  = 16,
        BOTTOM  = 32
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
    int justified;
    int kind;
};


}

#endif
