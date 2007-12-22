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
    if (PyObject_HasAttrString(obj, (char*) "ptr")) {
        PyObject* ptr = PyObject_GetAttrString(obj, (char*) "ptr");
        long addr = PyLong_AsLong(ptr);
        Py_DECREF(ptr);
        
        return (Element*) addr;
    } else {
        // python code
        // NOTE: I think the only element that uses this code is a color
        // construct outside any graphic
        
        // do nothing if not a list
        // and check that header is int
        if (!PyTuple_Check(obj) ||
            PyTuple_GET_SIZE(obj) < 1 ||
            !PyInt_Check(PyTuple_GET_ITEM(obj, 0)))
            return NULL;
        
        // build element based on header
        int elmid = (int) PyInt_AsLong(PyTuple_GET_ITEM(obj, 0));

        Element *elm = g_elementFactory.Create(elmid);
        if (elm == NULL) {
            // invalid elmid, return error
            return NULL;
        }
        
        Scm code = Py2ScmTake(PyTuple_GetSlice(obj, 1, PyTuple_GET_SIZE(obj)));
        
        if (!elm->Build(elmid, code)) {
            delete elm;
            return NULL;
        }
        
        // return the built element
        return elm;
    }
}


Element *GetElementFromObject(const Scm code)
{
    return GetElementFromObject(code.GetPy());
}


Element::Element(ElementId id) : 
    m_id(id), 
    m_visible(true),   
    m_parent(NULL),    
    m_referenced(0),
    m_model(NULL),
    m_transformParent(NULL)
{}
    

Element::~Element()
{
    // delete all child elements
    for (Iterator i=Begin(); i!=End(); i++) {
        (*i)->DecRef();
        if (!(*i)->IsReferenced())
            delete (*i);
    }
}

// recursively build child elements and if sucessful, add them as children
bool Element::Build(int header, const Scm &code)
{
    // process children
    for (Scm children = code; 
         ScmConsp(children); 
         children = children.Slice(1))
    {
        Scm child = children.GetScm(0);
        if (!AddChild(child))
            return false;
    }
    return true;
}

Element *Element::AddChild(Scm code)
{
    Element *elm = GetElementFromObject(code);

    // if error with child, report error
    if (!elm)
        return NULL;

    // ensure elements are not added twice
    if (elm->GetParent() != NULL) {
        Error("element already has parent.");
        return NULL;
    }

    AddChild(elm);

    return elm;
}

Scm Element::GetContents()
{
    return Scm_EOL;
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

const TransformMatrix *Element::GetTransform(TransformMatrix *matrix,
                                             const Camera &camera)
{
    if (m_transformParent == NULL) {
        return &g_transformIndentity;
    } else {
        return m_transformParent->GetTransform(matrix, camera);
    }
}


void Element::Update()
{
    if (m_parent == NULL) 
        m_transformParent = NULL;
    else
        m_transformParent = m_parent->GetTransformParent();
}


Element *Element::GetTransformParent()
{
    return m_transformParent;
}



} // namespace Summon
