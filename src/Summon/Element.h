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


enum {
    MODEL_SCREEN,
    MODEL_WORLD
};


class Element
{
public:
    Element(int id = -1);
    virtual ~Element();
    
    
    virtual Element *Create() {
        return new Element();
    }
    
    virtual bool Build(int header, const Scm &code);
    virtual Scm GetContents();    
    
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
    
    inline int GetId() { return m_id; }
    virtual int GetSpecificId() { return m_id; }
    inline void SetId(int id) { m_id = id; }
    
    inline Iterator Begin() { return m_children.begin(); }
    inline Iterator End() { return m_children.end(); }
    inline void SetParent(Element *elm) { m_parent = elm; }
    inline Element *GetParent() { return m_parent; }
    virtual bool IsDynamic() { return false; }
    inline bool IsVisible() { return m_visible; }
    inline void SetVisible(bool vis) { m_visible = vis; }    
    inline void SetModel(void *model) { m_model = model; }
    inline void *GetModel() { return m_model; }

    
    virtual void FindBounding(float *top, float *bottom, float *left, float *right,
                      TransformMatrix *matrix);
    
    
    inline void IncRef() { m_referenced++; }
    inline void DecRef() { m_referenced--; }
    
    inline bool IsReferenced() { return m_referenced > 0; }
    
protected:
    int m_id;
    Element *m_parent;
    void *m_model;
    list<Element*> m_children;
    bool m_visible;    
    int m_referenced;
};


inline Element *Id2Element(int id)
{
    return (Element*) id;
}

inline int Element2Id(Element *elm)
{
    return (int) elm;
}


Element *GetElementFromObject(PyObject *obj);
Element *GetElementFromObject(const Scm code);

// element factory and registration
class Element;
typedef Factory<int, Element> ElementFactory;

extern ElementFactory g_elementFactory;



}

#endif
