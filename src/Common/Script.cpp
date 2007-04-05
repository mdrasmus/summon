/***************************************************************************
* Vistools
* Matt Rasmussen
* Script.cpp
*
* Wrappers for Scheme library (guile)
*
***************************************************************************/

#include "Script.h"


namespace Vistools {

using namespace std;

Scm Scm_UNSPECIFIED;
Scm Scm_UNDEFINED;
Scm Scm_EOL;
Scm Scm_TRUE;
Scm Scm_FALSE;

PyObject *python_globals;

void InitPython()
{
    Scm_UNSPECIFIED = Py2Scm(Py_None);
    Scm_UNDEFINED   = Py2Scm(Py_None);
    Scm_TRUE        = Py2Scm(Py_True);
    Scm_FALSE       = Py2Scm(Py_False);
    Scm_EOL         = Py2ScmTake(PyTuple_New(0));
    
    python_globals = PyModule_GetDict(PyImport_AddModule("__main__"));
    Py_INCREF(python_globals);
    
    // create gaurdian object
    PyObject *dict = PyDict_New();
    PyDict_SetItemString(python_globals, GAURDIAN_KEY, dict);
    Py_DECREF(dict);
    
    // take ownership of basic primitives for lifetime of python
    //Py_INCREF(Py_None);
    //Py_INCREF(Py_True);
    //Py_INCREF(Py_False);
    //Py_INCREF(Scm2Py(Scm_EOL));
}

void DestroyPython()
{
    // release ownership of references
    //Py_DECREF(Py_None);
    //Py_DECREF(Py_True);
    //Py_DECREF(Py_False);
    //Py_DECREF(Scm2Py(Scm_EOL));
    
    Py_DECREF(python_globals);
}


string int2string(int num)
{
    enum { STR_SIZE = 10 };
    char *numstr = new char [STR_SIZE + 1];
    snprintf(numstr, STR_SIZE, "%d", num);
    
    string str = string(numstr);
    
    delete [] numstr;
    
    return str;
}

/*
int ScmLength(Scm lst)
{
    int len = 0;
    for (; ScmConsp(lst); len++, lst = ScmCdr(lst));
    return len;
}*/

}
