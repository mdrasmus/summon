/***************************************************************************
* Vistools
* Matt Rasmussen
* Element.h
*
***************************************************************************/

#ifndef ELEMENT_H
#define ELEMENT_H

#include "Script.h"
#include <list>
#include "drawCommands.h"

namespace Vistools {

using namespace std;



class Element
{
public:
    Element(int id = -1) : 
        m_id(id), 
        m_parent(NULL),
        m_visible(true),
        m_dynamic(false)                
    {}
    
    virtual ~Element()
    {
        // delete all child elements
        for (Iterator i=Begin(); i!=End(); i++)
            delete (*i);
    }
    
    typedef list<Element*>::iterator Iterator;
    
    inline void AddChild(Element* elm)
    {
        m_children.push_back(elm); 
        elm->SetParent(this);
    }
    
    inline void RemoveChild(Element *elm)
    {
        m_children.remove(elm);
        elm->SetParent(NULL);
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
    
    inline int GetId() { return m_id; }
    
    inline Iterator Begin() { return m_children.begin(); }
    inline Iterator End() { return m_children.end(); }
    inline void SetParent(Element *elm) { m_parent = elm; }
    inline Element *GetParent() { return m_parent; }
    inline bool IsDynamic() { return m_dynamic; }
    inline bool IsVisible() { return m_visible; }
    inline void SetVisible(bool vis) { m_visible = vis; }    
    
protected:
    int m_id;
    Element *m_parent;
    list<Element*> m_children;
    bool m_visible;    
    bool m_dynamic;
};


}

#endif
