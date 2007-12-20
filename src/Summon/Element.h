/***************************************************************************
* Summon
* Matt Rasmussen
* Element.h
*
***************************************************************************/

#ifndef ELEMENT_H
#define ELEMENT_H

#include "Script.h"
#include <list>
#include "summonCommands.h"
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
    
    //===================================
    // Children functions

    // Iterator for children of element
    typedef list<Element*>::iterator Iterator;
    
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
    
    inline void ReplaceChild(Element *oldchild, Element *newchild) {
        // TODO: fix
        RemoveChild(oldchild);
        AddChild(newchild);
    }
    
    inline Element *ReplaceChild(Element *oldchild, Scm code) {
        // TODO: fix
        RemoveChild(oldchild);
        return AddChild(code);
    }
    
    inline Iterator GetChild(Element *elm)
    {
        for (Iterator i=Begin(); i!=End(); i++) {
            if ((*i) == elm) {
                return i;
            }
        }
        return End();
    }
    
    inline int NumChildren()
    { return m_children.size(); }

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
                      TransformMatrix *matrix);

    virtual void Update();
    virtual Element *GetTransformParent();    
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
    
    // NOTE: must be of actual type Transform
    Element *m_transformParent;
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


// element factory and registration
class Element;
typedef Factory<int, Element> ElementFactory;

extern ElementFactory g_elementFactory;



}

#endif
