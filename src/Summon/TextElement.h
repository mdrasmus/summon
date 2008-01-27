/***************************************************************************
* Summon
* Matt Rasmussen
* TextElement.h
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

#ifndef SUMMON_TEXT_ELEMENT_H
#define SUMMON_TEXT_ELEMENT_H

#include "Script.h"
#include <algorithm>
#include <string>
#include "Element.h"


namespace Summon {



class TextElement : public Element
{
public:
    TextElement() : 
        Element(TEXT_CONSTRUCT),
        minHeight(0.0),
        maxHeight(0.0),
        modelKind(MODEL_WORLD)
    {
    }
    
    virtual int GetSpecificId()
    {
        switch (kind) {
            case KIND_BITMAP: return TEXT_CONSTRUCT;
            case KIND_SCALE: return TEXT_SCALE_CONSTRUCT;
            case KIND_CLIP: return TEXT_CLIP_CONSTRUCT;
            default: assert(0);
        }
    }
    
    virtual Element *Create()
    { return new TextElement(); }
    
    virtual bool IsDynamic() { return kind != KIND_SCALE; }
    
    virtual bool Build(int header, const Scm &code);
    virtual Scm GetContents();
    
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
    unsigned char justified;
    unsigned char kind;
    float minHeight;
    float maxHeight;
    int modelKind;
};


} // namespace Summon

#endif
