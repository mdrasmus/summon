/***************************************************************************
* Summon
* Matt Rasmussen
* Element.h
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

#ifndef SUMMON_ELEMENT_H
#define SUMMON_ELEMENT_H

#include "Script.h"
#include <list>
#include "elements.h"
#include "Vertex.h"
#include "TransformMatrix.h"


namespace Summon {

using namespace std;


// Model types
typedef enum {
    MODEL_SCREEN,
    MODEL_WORLD
} ModelKind;
// NOTE: this is placed here (instead of in SummonModel.h) so that TextElement 
// has access without needing to include SummonModel.h (a circular include)


class Camera
{
public:
    Camera() {}
    
    Camera(const Vertex2f &trans, const Vertex2f &zoom, const Vertex2f &focus) :
        trans(trans),
        zoom(zoom),
        focus(focus)
    {}
    
        
    Vertex2f trans;
    Vertex2f zoom;
    Vertex2f focus;
};


extern const Camera g_defaultCamera;

// forward declaration
class Transform;

typedef short ElementId;

class Element
{
public:
    Element(ElementId id = -1);
    virtual ~Element();
    
    inline int GetId() { return m_id; }
    virtual int GetSpecificId() { return m_id; }
    inline void SetId(int id) { m_id = id; }
        
    // Returns a new Element (needed by factory)
    virtual Element *Create() {
        return new Element();
    }
    
    
    // Populate element from python code
    virtual bool Build(int header, const Scm &code);
    
    // Return the contents this element in form of python code
    virtual Scm GetContents();
    
    virtual bool SetContents(const Scm &code) { return false; }
    
    //===================================
    // Children functions

    // Iterator for children of element
    typedef list<Element*>::iterator Iterator;

    inline int NumChildren()
    { return m_children.size(); }
    
    inline void AddChild(Element* elm)
    {
        m_children.push_back(elm); 
        elm->SetParent(this);        
        elm->IncRef();
    }
    
    Element *AddChild(Scm code);
    
    inline void RemoveChild(Element *elm)
    {
        elm->DecRef();
        m_children.remove(elm);
        elm->SetParent(NULL);
    }
    
    void ReplaceChild(Element *oldchild, Element *newchild);
    Element *ReplaceChild(Element *oldchild, Scm code);


    inline Iterator Begin() { return m_children.begin(); }
    inline Iterator End() { return m_children.end(); }

        
    
    inline void SetParent(Element *elm) { m_parent = elm; }
    inline Element *GetParent() { return m_parent; }
    virtual bool IsDynamic() { return false; }
    
    // Sets whether this element (and its descendents) are visible on screen
    inline void SetVisible(bool vis) { m_visible = vis; }
    inline bool IsVisible() { return m_visible; }    
    
    // Get and set the model for this element    
    inline void SetModel(void *model) { m_model = model; }
    inline void *GetModel() { return m_model; }

    
    virtual void FindBounding(float *top, float *bottom, float *left, float *right,
                              const TransformMatrix *matrix, 
                              const Camera &camera=g_defaultCamera);

    virtual void Update();
    virtual Transform *GetTransformParent();    
    virtual const TransformMatrix *GetTransform(TransformMatrix *matrix,
                                                const Camera &camera);
    
    
    //==========================================
    // Reference counting
    inline void IncRef() { m_referenced++; }
    inline void DecRef() { m_referenced--; }    
    inline bool IsReferenced() { return m_referenced > 0; }

    
protected:

    ElementId m_id; 
    bool m_visible;
    Element *m_parent;
    int m_referenced;
    void *m_model;
    list<Element*> m_children;
    
    // NOTE: actual type must be Transform*
    Transform *m_transformParent;
};


// Convert an Element id (used in python) to an Element pointer
inline Element *Id2Element(int id)
{
    return (Element*) id;
}


// Convert an Element pointer to an Element id (used in python)
inline int Element2Id(Element *elm)
{
    return (int) elm;
}


// Get an Element pointer from a python object
Element *GetElementFromObject(PyObject *obj);
Element *GetElementFromObject(const Scm code);





}

#endif
