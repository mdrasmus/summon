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

Scm Scm_NULL;
Scm Scm_NONE;
Scm Scm_EOL;
Scm Scm_TRUE;
Scm Scm_FALSE;

PyObject *python_globals;

static char *g_errorMsg = NULL;


void InitPython()
{
    Scm_NULL  = Scm(NULL);
    Scm_NONE  = Py2Scm(Py_None);
    Scm_TRUE  = Py2Scm(Py_True);
    Scm_FALSE = Py2Scm(Py_False);
    Scm_EOL   = Py2ScmTake(PyTuple_New(0));
    
    python_globals = PyModule_GetDict(PyImport_AddModule("__main__"));
    Py_INCREF(python_globals);
}

void DestroyPython()
{
    Py_DECREF(python_globals);
}


void Error(const char *format, ...)
{
    char text[500];
    const int maxtext = 500;

    va_list args;   
    va_start(args, format);
    
    // set error message to global variable
    vsnprintf(text, maxtext, format, args);
    g_errorMsg = strdup(text);
    
    va_end(args);
}


char *GetError()
{
    return g_errorMsg;
}

void ClearError()
{
    free(g_errorMsg);
    g_errorMsg = NULL;
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
