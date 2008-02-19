/***************************************************************************
* Summon
* Matt Rasmussen
* Script.cpp
*
* Wrappers for scripting (python)
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

static string g_errorMsg;
static int g_nerrors = 0;


void InitPython()
{
    Scm_NULL  = Scm(NULL);
    Scm_NONE  = Py2Scm(Py_None);
    Scm_TRUE  = Py2Scm(Py_True);
    Scm_FALSE = Py2Scm(Py_False);
    Scm_EOL   = Py2ScmTake(PyTuple_New(0));
    
    python_globals = PyModule_GetDict(PyImport_AddModule((char*) "__main__"));
    Py_INCREF(python_globals);
}

void DestroyPython()
{
    Py_DECREF(python_globals);
}


void Error(const char *format, ...)
{
    const int maxtext = 500;
    char text[maxtext];
    char errornum[10];
    
    // process variable number of arguments 
    va_list args;   
    va_start(args, format);
    
    // count the number of errors
    g_nerrors++;
    
    // format error
    vsnprintf(text, maxtext, format, args);
    snprintf(errornum, 10, "[%d] ", g_nerrors);
    
    // append error text to existing errors, if they exist
    if (g_errorMsg.size() > 0)
        g_errorMsg += "\n";
    g_errorMsg += errornum;
    g_errorMsg += text;
    
    // cleanup variable number of arguments
    va_end(args);
}


const char *GetError()
{
    return g_errorMsg.c_str();
}

void ClearError()
{
    g_errorMsg = "";
    g_nerrors = 0;
}


void SetException()
{
    PyErr_Format(PyExc_Exception, GetError());
    ClearError();
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



bool ParseScm(Scm lst, const char *fmt, ...)
{
    va_list ap;   
    
    bool status = true;
    int *d;
    float *f;
    string *str;
    Scm *proc;
    Scm *code;
    bool *b;
    
    va_start(ap, fmt);

       
    // loop through format string
    int i=1;
    for (const char *argtype = fmt; *argtype && status; argtype++, i++) {
        // ensure lst is a list
        if (!ScmConsp(lst)) {
            status = false;
            break;
        }
        
        
        // get next arg in lst
        Scm arg = ScmCar(lst);
        lst = ScmCdr(lst);
        
        switch (*argtype) {
            case 'd':
                if (!ScmIntp(arg)) {
                    Error("expected integer for argument %d", i);
                    status = false;
                    break;
                }
                
                d = va_arg(ap, int *);
                *d = Scm2Int(arg);
                break;
            case 'f':
                if (!ScmFloatp(arg)) {
                    Error("expected float for argument %d", i);
                    status = false;
                    break;
                }
                
                f = va_arg(ap, float *);
                *f = Scm2Float(arg);
                break;
            case 's':
                if (!ScmStringp(arg)) {
                    Error("expected string for argument %d", i);
                    status = false;
                    break;
                }
                
                str = va_arg(ap, string *);
                *str = Scm2String(arg);
                break;
            case 'b':
                b = va_arg(ap, bool *);
                *b = (arg != Scm_FALSE);
                break;
            case 'p':
                if (!ScmProcedurep(arg)) {
                    Error("expected procedure for argument %d", i);
                    status = false;
                    break;
                }
                
                proc = va_arg(ap, Scm *);
                *proc = arg;
                break;
            case 'c':
                code = va_arg(ap, Scm *);
                *code = arg;
                break;
        }
    }
    
    va_end(ap);
    
    return status;
}


// the 's' format is for char* only
// Scm objects must be passed by pointers
Scm BuildScm(const char *fmt, ...)
{
    va_list ap;   
        
    PyObject *tup = PyTuple_New(strlen(fmt));
    
    // argument pointers
    int d;
    float f;
    char *str;
    Scm *proc;
    Scm *code;
    int b;
    
    va_start(ap, fmt);

       
    // loop through format string
    int i=0;
    for (const char *argtype = fmt; *argtype; argtype++, i++) {        
        switch (*argtype) {
            case 'd': {
                d = va_arg(ap, int);
                PyObject *py = PyInt_FromLong((long) d);
                assert(py != NULL);
                PyTuple_SET_ITEM(tup, i, py);
                } break;
            case 'f': {
                f = va_arg(ap, double);
                PyObject *py = PyFloat_FromDouble(f);
                assert(py != NULL);
                PyTuple_SET_ITEM(tup, i, py);
                } break;
            case 's': {
                str = va_arg(ap, char *);
                PyObject *py = PyString_FromString(str);
                assert(py != NULL);
                PyTuple_SET_ITEM(tup, i, py);
                } break;
            case 'b': {
                b = va_arg(ap, int);
                PyObject *py = (b) ? Py_True : Py_False;
                Py_INCREF(py);
                PyTuple_SET_ITEM(tup, i, py);
                } break;
            case 'p': {
                proc = va_arg(ap, Scm *);
                PyObject *py = proc->GetPy();
                Py_INCREF(py);
                PyTuple_SET_ITEM(tup, i, py);
                } break;
            case 'c': {
                code = va_arg(ap, Scm *);
                PyObject *py = code->GetPy();
                Py_INCREF(py);
                PyTuple_SET_ITEM(tup, i, py);
                } break;
            default:
                // unknown format code
                assert(0);
        }
    }
    
    va_end(ap);
    
    return Py2ScmTake(tup);
}


} // namespace Summon
