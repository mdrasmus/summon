/***************************************************************************
* Vistools
* Matt Rasmussen
* Graphic.h
*
***************************************************************************/

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "Script.h"
#include "Element.h"

namespace Vistools {

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
    Graphic(int id);
    virtual ~Graphic();

    /*    
    inline void AddPrimitive(Primitive *p)
    { m_primitives.push_back(p); }
    
    typedef list<Primitive*>::iterator PrimitiveIterator;
    
    inline PrimitiveIterator PrimitivesBegin()
    { return m_primitives.begin(); }
    inline PrimitiveIterator PrimitivesEnd()
    { return m_primitives.end(); }
    
    */
    
    // tags for primitives
    const static char PRIM_VERTICES = 0x01;
    const static char PRIM_COLOR    = 0x02;
    
    
    bool Build(Scm code);
    bool BuildVertices(char *data, int &ptr, Scm &code);
    int GetDataSize(Scm code);
    
    inline int NextPrimitive(int ptr)
    {
        if (m_data[ptr] == PRIM_VERTICES) {
            int len = *((int*) (m_data + ptr + 1));
            ptr += 1 + sizeof(int) + 2 * sizeof(float) * len;
        } else if (m_data[ptr] == PRIM_COLOR) {
            ptr += 5;
        } else {
            assert(0);
        }
        return ptr;
    }
    
    inline char GetTag(int ptr)
    { return m_data[ptr]; }
    
    inline bool IsVertices(int ptr)
    { return m_data[ptr] == PRIM_VERTICES; }
    
    inline int GetVerticesLen(int ptr)
    { return *((int*) (m_data + ptr + 1)); }
    
    inline int VerticesEnd(int ptr)
    { return ptr + 1 + sizeof(int) + 2 * sizeof(float) * GetVerticesLen(ptr); }
    
    inline int VerticesStart(int ptr)
    { return ptr + 1 + sizeof(int); }
    
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
    //list<Primitive*> m_primitives;
};


/*

class VerticesPrimitive : public Primitive
{
public:
    VerticesPrimitive() :
        Primitive(VERTICES_CONSTRUCT), 
        data(NULL),
        len(0),
        dim(2)
    {}
    
    virtual ~VerticesPrimitive() 
    {
        if (data)
            delete [] data;
    }
    
    float *data;
    int len;
    int dim;
};

class ColorPrimitive : public Primitive
{
public:
    ColorPrimitive(float r = 1, float g = 1, float b = 1, float a = 1) :
        Primitive(COLOR_CONSTRUCT)        
    {
        data[0] = r;
        data[1] = g;
        data[2] = b;
        data[3] = a;
    }
    
    float data[4];
};

*/

}

#endif
