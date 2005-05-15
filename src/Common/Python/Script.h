/***************************************************************************
* Vistools
* Matt Rasmussen
* Script.h
*
* Wrappers for Python
*
***************************************************************************/


#ifndef SCRIPT_H
#define SCRIPT_H

#include <Python.h>
#include <string>
#include <vector>


namespace Vistools {

using namespace std;


class Scm {
public:
    Scm(PyObject* o = NULL, bool take = true) :
        py(o),
        start(0)
    {
        if (o != NULL && take)
            Py_INCREF(o);
    }
    
    Scm(const Scm &other)
    {
        py = other.py;
        start = other.start;
        if (py != NULL)
            Py_INCREF(py);
    }
    
    ~Scm()
    {
        if (py != NULL)
            Py_DECREF(py);
    }
        
    inline int Size() const
    {
        return PyTuple_GET_SIZE(py) - start;
    }
    
    inline void Set(PyObject *p, int i)
    {
        PyTuple_SET_ITEM(py, start + i, p);
    }
    
    inline PyObject *Get(int i) const
    {
        return PyTuple_GET_ITEM(py, start + i);
    }
    
    inline PyObject *GetPy() const
    {
        if (start == 0)
            return py;
        else {
            // get slice makes a new reference.  However, it is my convention
            // that GetPy() always returns a borrowed reference
            PyObject *o = PyTuple_GetSlice((PyObject*) py, start, 
                                           PyTuple_GET_SIZE(py));
            Py_DECREF(o);
            return o;
        }
    }
    
    inline bool IsTuple()  const { return PyTuple_Check(py); }
    inline bool IsInt()    const { return PyInt_Check(py); }
    inline bool IsLong()   const { return PyLong_Check(py); }
    inline bool IsFloat()  const { return PyFloat_Check(py); }
    inline bool IsString() const { return PyString_Check(py); }

    
    inline Scm operator= (const Scm &other)
    {
        // decref old python object
        if (py != NULL)
            Py_DECREF(py);
        
        py = other.py;
        start = other.start;
        if (py != NULL)
            Py_INCREF(py);
        return *this;
    }
    
    inline bool operator== (const Scm &s) const
    {
        return py == s.py && 
               start == s.start;
    }
    
    inline bool operator!= (const Scm &s) const
    {
        return !(*this == s);
    }
        
    PyObject* py;
    int start;
};

extern Scm Scm_UNSPECIFIED;
extern Scm Scm_UNDEFINED;
extern Scm Scm_EOL;
extern Scm Scm_TRUE;
extern Scm Scm_FALSE;
extern PyObject *python_globals;

#define Py2Scm(var) (Scm(var))
#define Scm2Py(var) (var.GetPy())
#define Py2ScmTake(var) (Scm(var, false))

#define GAURDIAN_KEY "__gaurdian"

void InitPython();
void DestroyPython();


inline PyObject *GetGaurdian()
{
    PyObject *globals = PyModule_GetDict(PyImport_AddModule("__main__"));
    return PyDict_GetItemString(globals, GAURDIAN_KEY);
}

// gaurd a scm object from garbage collection
inline void ScmGaurd(Scm scm)
{
    return;
    PyObject *gaurdian = GetGaurdian();
    PyObject *p = Scm2Py(scm);
    
    int count = 1;
    PyObject *pcount = PyDict_GetItem(gaurdian, p);
    
    if (pcount) {
        // add one to count
        count = PyInt_AS_LONG(pcount) + 1;
    }
    
    //printf("gaurd %p %d %d\n", p, count, p->ob_refcnt);
 
    PyObject *pcount2 = PyInt_FromLong((long) count);
    PyDict_SetItem(gaurdian, p, pcount2);
    //Py_INCREF(p);
    Py_DECREF(pcount2);
}


inline void ScmUngaurd(Scm scm)
{
    return;
    PyObject *gaurdian = GetGaurdian();
    PyObject *p = Scm2Py(scm);
    
    PyObject *pcount = PyDict_GetItem(gaurdian, p);
    assert(pcount);
    
    int count = PyInt_AS_LONG(pcount) - 1;
    
    //printf("ungaurd %p %d %d\n", p, count, p->ob_refcnt);
    
    if (count == 0) {
        PyDict_DelItem(gaurdian, p);
    } else {
        PyObject *pcount2 = PyInt_FromLong((long) count);
        PyDict_SetItem(gaurdian, p, pcount2);
        Py_DECREF(pcount2);
    }
}


inline Scm ScmEvalStr(const char *str)
{
    return Py2ScmTake(PyRun_String(str, Py_file_input, 
                                   python_globals, python_globals));
}

inline bool ScmEvalFile(const char *file) 
{
    FILE *stream;
    if ((stream = fopen(file, "r")) == NULL) {
        return false;
    } else {
        PyRun_SimpleFile(stream, file);
        fclose(stream);
        return true;
    }
}

inline Scm ScmApply(const Scm &proc, const Scm &args)
{
    PyObject *p = Scm2Py(proc);
    return Py2ScmTake(PyObject_CallObject(p, Scm2Py(args)));
}


inline bool ScmConsp(const Scm &scm)
{
    return scm.IsTuple() && scm.Size() > 0;
}

inline Scm ScmCons(const Scm &a, const Scm &b)
{ 
    int len = b.Size();
    PyObject *tup = PyTuple_New(len + 1);
    PyObject *x = Scm2Py(a);
    Py_INCREF(x);
    PyTuple_SET_ITEM(tup, 0, x);
    for (int i=0; i <len; i++) {
        x = b.Get(i);
        Py_INCREF(x);
        PyTuple_SET_ITEM(tup, i+1, x);
    }
    return Py2ScmTake(tup);
}

inline Scm ScmCar(const Scm &scm)
{ return Py2Scm(scm.Get(0)); }

inline Scm ScmCdr(const Scm &scm)
{
    Scm scm2 = scm;
    scm2.start += 1;
    return scm2;
}

inline Scm ScmCadr(const Scm &scm)
{ return scm.Get(1); }

inline Scm ScmCddr(const Scm &scm) 
{
    Scm scm2 = scm;
    scm2.start += 2;
    return scm2;
}
inline Scm ScmCaddr(const Scm &scm)
{ return scm.Get(2); }
inline Scm ScmCdddr(const Scm &scm) {
    Scm scm2 = scm;
    scm2.start += 3;
    return scm2;
}
inline Scm ScmCadddr(const Scm &scm)
{ return scm.Get(3); }
inline Scm ScmCddddr(const Scm &scm) {
    Scm scm2 = scm;
    scm2.start += 4;
    return scm2;
}



inline bool ScmIntp(const Scm &scm)
{ return (bool) scm.IsInt() ||
         (bool) scm.IsLong(); }

inline int Scm2Int(const Scm &num)
{
    if (num.IsInt())
        return (int) PyInt_AS_LONG(Scm2Py(num));
    else
        return (int) PyLong_AsLong(Scm2Py(num));
}

inline Scm Int2Scm(int num)
{ return Py2ScmTake(PyInt_FromLong((long) num)); }


inline bool ScmFloatp(const Scm &scm)
{ return scm.IsFloat() || scm.IsInt(); }

inline float Scm2Float(const Scm &scm)
{
    if (scm.IsInt())
        return (float) PyInt_AS_LONG(Scm2Py(scm));
    else if (scm.IsLong())
        return (float) PyLong_AsLongLong(Scm2Py(scm));
    else        
        return (float) PyFloat_AS_DOUBLE(Scm2Py(scm));
}

inline Scm Float2Scm(float val)
{ return Py2ScmTake(PyFloat_FromDouble(val)); }


inline bool ScmStringp(const Scm &scm)
{ return (bool) scm.IsString(); }

inline string Scm2String(const Scm &scm)
{ return string(PyString_AS_STRING(Scm2Py(scm))); }

inline Scm String2Scm(const char *str)
{ return Py2ScmTake(PyString_FromString(str)); }


inline bool ScmProcedurep(const Scm &scm)
{ return (bool) PyCallable_Check(Scm2Py(scm)); }


inline int ScmLength(const Scm &lst)
{ return lst.Size(); }

inline Scm ScmAppend(const Scm &l1, const Scm &l2)
{
    int size1 = l1.Size();
    int size2 = l2.Size();
    PyObject *newTup = PyTuple_New(size1 + size2);
    PyObject *x;
    
    for (int i=0; i<size1; i++) {
        x = l1.Get(i);
        Py_INCREF(x);
        PyTuple_SET_ITEM(newTup, i, x);
    }
    for (int i=0; i<size2; i++) {
        x = l2.Get(i);
        Py_INCREF(x);
        PyTuple_SET_ITEM(newTup, i+size1, x);
    }
    
    return Py2ScmTake(newTup); 
}

string int2string(int num);

/*
Not needed for Python code

inline Scm ScmDefine(const char *name, Scm val)
{ return Scm_UNDEFINED; }

inline Scm ScmLookup(const char *name)
{ return Scm_UNDEFINED; }

inline Scm ScmVariableRef(Scm var)
{ return Scm_UNDEFINED; }
*/

}

#endif

