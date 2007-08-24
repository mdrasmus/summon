/***************************************************************************
* Summon
* Matt Rasmussen
* Script.cpp
*
* Wrappers for Scheme library (guile)
*
***************************************************************************/

#include "Script.h"
#include <stdarg.h>

namespace Summon {

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
}

void DestroyPython()
{
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



} // namespace Summon
