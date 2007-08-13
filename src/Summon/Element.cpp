/***************************************************************************
* Summon
* Matt Rasmussen
* TransformMatrix.cpp
*
***************************************************************************/

#include "first.h"
#include "Element.h"


namespace Summon {


// recursively build child elements and if sucessful, add them as children
bool Element::Build(const Scm &code)
{
    printf("element build\n");

    // process children
    for (Scm children = code; 
         ScmConsp(children); 
         children = ScmCdr(children))
    {
        printf("build child\n");
        
        Scm child = ScmCar(children);
        Element *elm;
        
        PyObject *obj = child.GetPy();
        if (PyObject_HasAttrString(obj, "ptr")) {
            // preconstructed object
            PyObject* ptr = PyObject_GetAttrString(obj, "ptr");
            long addr = PyLong_AsLong(ptr);
            elm = (Element*) addr;
        } else {
            // python code
            
            // do nothing if not a list
            // and check that header is int
            if (!ScmConsp(child) || !ScmIntp(ScmCar(child)))
                return false;
        
            // build element based on header
            int elmid = Scm2Int(ScmCar(code));
            printf("elmid: %d\n", elmid);
        
            elm = g_elementFactory.Create(elmid);
            if (!elm && elm->Build(code)) {
                delete elm;
                return false;
            }
        }
        
        AddChild(elm);
        printf("added %p to %p\n", elm, this);
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
