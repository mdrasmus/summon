/***************************************************************************
* Summon
* Matt Rasmussen
* TransformMatrix.cpp
*
***************************************************************************/

#include "first.h"
#include "Element.h"


namespace Summon {

Element *GetElementFromObject(PyObject *obj)
{
    if (PyObject_HasAttrString(obj, "ptr")) {
        PyObject* ptr = PyObject_GetAttrString(obj, "ptr");
        long addr = PyLong_AsLong(ptr);
        Py_DECREF(ptr);
        
        return (Element*) addr;
    } else {
        return NULL;
    }
}


Element::Element(int id) : 
    m_id(id), 
    m_parent(NULL),
    m_visible(true),
    m_referenced(false),
    m_dynamic(false)
{}
    

Element::~Element()
{
    // delete all child elements
    for (Iterator i=Begin(); i!=End(); i++) {
        if ((*i)->m_referenced) {
            // just detach parent pointer and let python garbage collect child
            (*i)->SetParent(NULL);
        } else {
            delete (*i);
        }
    }
}

// recursively build child elements and if sucessful, add them as children
bool Element::Build(const Scm &code)
{
    // process children
    for (Scm children = code; 
         ScmConsp(children); 
         children = ScmCdr(children))
    {
        //printf("build child\n");
        
        Scm child = ScmCar(children);
        
        PyObject *obj = child.GetPy();
        Element *elm = GetElementFromObject(obj);
        
        if (!elm) {
            // python code
            
            // do nothing if not a list
            // and check that header is int
            Scm header = ScmCar(child);
            if (!ScmConsp(child) || !ScmIntp(header))
                return false;
        
            // build element based on header
            int elmid = Scm2Int(header);
        
            elm = g_elementFactory.Create(elmid);
            if (!elm->Build(child)) {
                delete elm;
                return false;
            }
        }
        
        if (elm->GetParent() != NULL) {
            Error("element already has parent.");
            return false;
        }
        
        AddChild(elm);
    }
    return true;
}

void Element::FindBounding(float *top, float *bottom, 
                           float *left, float *right,
                           TransformMatrix *matrix)
{
    // loop through children of this element
    for (Element::Iterator i=Begin(); i!=End(); i++) {
        (*i)->FindBounding(top, bottom, left, right, matrix);
    }
}



} // namespace Summon
