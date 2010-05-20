/***************************************************************************
* SUMMON
* Matt Rasmussen
* summon_scene_graph.cpp
*
* This file creates main python interface to the SUMMON module.  The SUMMON 
* module itself, it represented by a singleton class SummonModule.
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

// summon headers
#include "elements.h"
#include "Element.h"

using namespace Summon;

#define MODULE_NAME "summon_scene_graph"


//#include "HashTable.h"
//HashTable<Element*, int, HashPointer> pointers;


//=============================================================================
// python visible prototypes
extern "C" {

// NOTE: this will some day become its own python module for just importing
// the scene graph without OpenGL.


/*
// functions directly visible in python
static PyObject *MakeElement(PyObject *self, PyObject *args);
static PyObject *IncRefElement(PyObject *self, PyObject *args);
static PyObject *DeleteElement(PyObject *self, PyObject *args);
static PyObject *GetElementChildren(PyObject *self, PyObject *args);
static PyObject *GetElementContents(PyObject *self, PyObject *args);
static PyObject *GetElementParent(PyObject *self, PyObject *args);

// python module method table
// NOTE: the (char*) casts are needed to avoid compiler warnings.
static PyMethodDef g_summonSceneGraphMethods[] = {
    // make element
    {(char*) "make_element", MakeElement, METH_VARARGS, (char*) ""},

    // delete an element
    {(char*) "delete_element", DeleteElement, METH_VARARGS, (char*) ""},

    // reference an element
    {(char*) "incref_element", IncRefElement, METH_VARARGS, (char*) ""},

    // get the children of an element
    {(char*) "get_element_children", GetElementChildren, METH_VARARGS, (char*) ""},

    // get the contents of an element
    {(char*) "get_element_contents", GetElementContents, METH_VARARGS, (char*) ""},

    // get the parent of an element
    {(char*) "get_element_parent", GetElementParent, METH_VARARGS, (char*) ""},
    
    // cap the methods table with NULL method
    {NULL, NULL, 0, NULL}
};
*/


// Create a new Element
PyObject *
MakeElement(PyObject *self, PyObject *args)
{
    long elmid;
    PyObject *lst;
    // (char*) needed to avoid compiler warning (python's fault)    
    int ok = PyArg_ParseTuple(args, (char*) "lO", &elmid, &lst); 

        
    if (!ok)
        return NULL;
    Scm code(lst);
    
    // add factory call
    Element *elm = g_elementFactory.Create(elmid);
    
    if (elm == NULL || !elm->Build(elmid, code)) {
        Error("error constructing element");
        SetException();
        return NULL;
    }
    elm->IncRef();
    
    //printf("new: %p\n", elm);

    //pointers[elm] = 1;
    
    // return element address
    PyObject *addr = PyInt_FromLong(Element2Id(elm));
    return addr;
}


// Delete an Element (or just decrement its reference count)
PyObject *
DeleteElement(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = Id2Element(addr);
    
    elm->DecRef();
    
    // if there are no more references then delete element
    if (!elm->IsReferenced()) { // && elm->GetParent() == NULL) {
        //printf("delete: %p %d\n", elm, pointers.HasKey(elm));
        delete elm;
    }
    
    Py_RETURN_NONE;
}


// Increase the reference count for an Element
PyObject *
IncRefElement(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = Id2Element(addr);
    
    //printf("incref %p\n", elm);
    elm->IncRef();
    
    Py_RETURN_NONE;
}


// Returns the children of an Element
PyObject *
GetElementChildren(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = Id2Element(addr);
    
    int len = elm->NumChildren() * 2;
    
    PyObject *children = PyTuple_New(len);
    int i = 0;
    for (Element *child=elm->GetChild(); child; child=child->GetNext()) {
        PyTuple_SET_ITEM(children, i, PyInt_FromLong(child->GetSpecificId()));
        PyTuple_SET_ITEM(children, i+1, PyInt_FromLong(Element2Id(child)));
        i+=2;
    }
    
    return children;
}


// Returns the contents (Element-specific information) of an Element
PyObject *
GetElementContents(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = Id2Element(addr);
    
    Scm contents = elm->GetContents();
    PyObject *pycontents = Scm2Py(contents);
    
    if (pycontents != NULL)
        Py_INCREF(pycontents);
    
    return pycontents;
}


// Returns the parent of an Element
PyObject *
GetElementParent(PyObject *self, PyObject *args)
{
    long addr = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));
    Element *elm = Id2Element(addr);
    
    PyObject *parent = PyTuple_New(2);
    PyTuple_SET_ITEM(parent, 0, PyInt_FromLong(elm->GetSpecificId()));
    PyTuple_SET_ITEM(parent, 1, PyInt_FromLong(Element2Id(elm->GetParent())));
    
    return parent;
}



/*
// Module initialization for python
PyMODINIT_FUNC
initsummon_core()
{
    // prepare the python environment for summon
    InitPython();
    // register all direction functions with python
    Py_InitModule((char*) MODULE_NAME, g_summonSceneGraphMethods);
}
*/

 
} // extern "C"
