/***************************************************************************
* Summon
* Matt Rasmussen
* Graphic.h
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

#ifndef SUMMON_GRAPHIC_H
#define SUMMON_GRAPHIC_H

#include "Script.h"
#include "Element.h"

namespace Summon {

using namespace std;

class Primitive
{
public:
    Primitive(int id = -1) : m_id(id) {}
    virtual ~Primitive() {}
    
    inline int GetId() { return m_id; }
    
protected:
    int m_id;
};

inline bool IsGraphic(int header)
{
    switch (header) {
        case POINTS_CONSTRUCT:
        case LINES_CONSTRUCT:
        case LINE_STRIP_CONSTRUCT:        
        case TRIANGLES_CONSTRUCT:
        case TRIANGLE_STRIP_CONSTRUCT:
        case TRIANGLE_FAN_CONSTRUCT:
        case QUADS_CONSTRUCT:
        case QUAD_STRIP_CONSTRUCT:
        case POLYGON_CONSTRUCT:
            return true;
        default:
            return false;
    }
}


class Graphic : public Element
{
public:
    Graphic(int id=-1);
    virtual ~Graphic();

    virtual Element *Create() {
        return new Graphic();
    }

    
    // tags for primitives
    const static char PRIM_VERTICES = 0x01;
    const static char PRIM_COLOR    = 0x02;
    
    
    virtual bool Build(int header, const Scm &code);
    bool BuildVertices(char *data, int &ptr, Scm &code);
    int GetDataSize(Scm code);
    virtual Scm GetContents();
    
    void FindBounding(float *top, float *bottom, float *left, float *right,
                      const TransformMatrix *matrix,
                      const Camera &camera=g_defaultCamera);
    
    //===============================================
    // Byte-code navigation
    
    inline int NextPrimitive(int ptr)
    {
        if (m_data[ptr] == PRIM_VERTICES) {
            return VerticesEnd(ptr);
        } else if (m_data[ptr] == PRIM_COLOR) {
            return ptr + 5;
        } else {
            assert(0);
        }
    }
    
    inline char GetTag(int ptr)
    { return m_data[ptr]; }
    
    inline bool IsVertices(int ptr)
    { return m_data[ptr] == PRIM_VERTICES; }
    
    inline int GetVerticesLen(int ptr)
    { return *((int*) (m_data + ptr + 1)); }

    inline int VerticesStart(int ptr)
    { return ptr + 1 + sizeof(int); }
    
    inline int VerticesEnd(int ptr)
    { return ptr + 1 + sizeof(int) + 2 * sizeof(float) * GetVerticesLen(ptr); }
    
    inline float *GetVertex(int ptr)
    { return (float*) (m_data + ptr); }
    
    inline int NextVertex(int ptr)
    { return ptr + 2 * sizeof(float); }

    inline bool IsColor(int ptr)
    { return m_data[ptr] == PRIM_COLOR; }
    
    inline char *GetColor(int ptr)
    { return m_data + ptr + 1; }
    
    inline bool More(int ptr)
    { return ptr < m_datasize; }
    
protected:
    char *m_data;
    int m_datasize;
};


}

#endif
