/***************************************************************************
* Summon
* Matt Rasmussen
* TransformMatrix.cpp
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


#include "first.h"
#include "Element.h"
#include "Transform.h"



namespace Summon {


const Camera g_defaultCamera(Vertex2f(0, 0), Vertex2f(1, 1), Vertex2f(0, 0));


Element *GetElementFromObject(PyObject *obj)
{
    if (PyObject_HasAttrString(obj, (char*) "ptr")) {
        PyObject* ptr = PyObject_GetAttrString(obj, (char*) "ptr");
        long addr = PyLong_AsLong(ptr);
        Py_DECREF(ptr);
        
        return Id2Element(addr); //(Element*) addr;
    } else {
        // python code
        // NOTE: I think the only element that uses this code is a color
        // construct outside any graphic
        
        // do nothing if not a tuple
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
    m_referenced(0),
    m_model(NULL),
    m_transformParent(NULL),
    m_parent(NULL),
    m_child(NULL),
    m_next(NULL),
    m_prev(NULL)
{}
    

Element::~Element()
{
    // delete all child elements
    for (Element *elm=m_child; elm; elm=elm->m_next) {
        // clear parent pointer
        elm->m_parent = NULL;
        elm->DecRef();
        if (!elm->IsReferenced())
            delete elm;
    }
}

// recursively build child elements and if sucessful, add them as children
bool Element::Build(int header, const Scm &code)
{
    if (!ScmConsp(code))
        return true;

    Scm children = code;
    
    // process children
    for (; children.Size() > 0; children.Pop()) {
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

void Element::ReplaceChild(Element *oldchild, Element *newchild)
{
    newchild->m_parent = this;
    newchild->m_next = oldchild->m_next;
    newchild->m_prev = oldchild->m_prev;
    
    if (newchild->m_next)
        newchild->m_next->m_prev = newchild;
    if (oldchild == m_child) {
        // replace first child
        m_child = newchild;

        // TODO: this might be a bug, I think the following code is needed
        //if (oldchild->prev == oldchild)
        //    // replace single child
        //    newchild->m_prev = newchild;
        newchild->m_prev = newchild;
    } else
        newchild->m_prev->m_next = newchild;
    newchild->IncRef();
    
    oldchild->m_parent = NULL;
    oldchild->m_next = NULL;
    oldchild->m_prev = NULL;
    oldchild->DecRef();
}


Element *Element::ReplaceChild(Element *oldchild, Scm code) {
    Element *elm = GetElementFromObject(code);

    // if error with child, report error
    if (!elm)
        return NULL;

    // ensure elements are not added twice
    if (elm->GetParent() != NULL) {
        Error("element already has parent.");
        return NULL;
    }    
    
    ReplaceChild(oldchild, elm);
    return elm;
}

Scm Element::GetContents()
{
    return Scm_EOL;
}

void Element::FindBounding(float *top, float *bottom, 
                           float *left, float *right,
                           const TransformMatrix *matrix,
                           const Camera &camera)
{
    // loop through children of this element
    for (Element *elm=m_child; elm; elm=elm->m_next) {
        elm->FindBounding(top, bottom, left, right, matrix, camera);
    }
}

const TransformMatrix *Element::GetTransform(TransformMatrix *matrix,
                                             const Camera &camera)
{
    if (m_transformParent == NULL) {
        return &g_transformIdentity;
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


Transform *Element::GetTransformParent()
{
    return m_transformParent;
}



} // namespace Summon
