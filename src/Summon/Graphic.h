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
    
    inline void AddPrimitive(Primitive *p)
    { m_primitives.push_back(p); }
    
    typedef list<Primitive*>::iterator PrimitiveIterator;
    
    inline PrimitiveIterator PrimitivesBegin()
    { return m_primitives.begin(); }
    inline PrimitiveIterator PrimitivesEnd()
    { return m_primitives.end(); }
    
protected:
    list<Primitive*> m_primitives;
};




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

}

#endif
