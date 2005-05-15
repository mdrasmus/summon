/***************************************************************************
* Vistools
* Matt Rasmussen
* Script.h
*
* Wrappers for Scheme library (guile)
*
***************************************************************************/


#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <vector>
#include <libguile.h>
#include <guile/gh.h>

namespace Vistools {

using namespace std;

typedef SCM Scm;

#define Scm_UNSPECIFIED SCM_UNSPECIFIED
#define Scm_UNDEFINED SCM_UNDEFINED
#define Scm_EOL SCM_EOL
#define Scm_TRUE SCM_BOOL_T
#define Scm_FALSE SCM_BOOL_F

#define SCM2Scm(var) ((Scm)(var))
#define Scm2SCM(var) ((SCM)(var))

vector<string> Scheme2Strings(Scm lst);

inline Scm ScmEvalStr(const char *str)
{ return SCM2Scm(gh_eval_str(str)); }

inline bool ScmEvalFile(const char *file) 
{   
    SCM2Scm(gh_eval_file(file)); 
    return true;
}

inline Scm ScmApply(Scm proc, Scm args)
{ return SCM2Scm(gh_apply(Scm2SCM(proc), Scm2SCM(args))); }


inline bool ScmConsp(Scm scm)
{ return SCM_CONSP(Scm2SCM(scm)); }

inline Scm ScmCons(Scm a, Scm b)
{ return SCM2Scm(scm_cons(Scm2SCM(a), Scm2SCM(b))); }

inline Scm ScmCar(Scm scm)
{ return SCM2Scm(SCM_CAR(Scm2SCM(scm))); }

inline Scm ScmCdr(Scm scm)
{ return SCM2Scm(SCM_CDR(Scm2SCM(scm))); }

inline Scm ScmCadr(Scm scm) { return ScmCar(ScmCdr(scm)); }
inline Scm ScmCddr(Scm scm) { return ScmCdr(ScmCdr(scm)); }
inline Scm ScmCaddr(Scm scm) { return ScmCar(ScmCddr(scm)); }
inline Scm ScmCdddr(Scm scm) { return ScmCdr(ScmCddr(scm)); }
inline Scm ScmCadddr(Scm scm) { return ScmCar(ScmCdddr(scm)); }
inline Scm ScmCddddr(Scm scm) { return ScmCdr(ScmCdddr(scm)); }



inline bool ScmIntp(Scm scm)
{ return SCM_INUMP(Scm2SCM(scm)); }

inline bool ScmFloatp(Scm scm)
{ return scm_real_p(Scm2SCM(scm)); }

inline bool ScmProcedurep(Scm scm)
{ return gh_procedure_p(Scm2SCM(scm)); }

inline bool ScmStringp(Scm scm)
{ return (bool) SCM_STRINGP(Scm2SCM(scm)); }

inline int Scm2Int(Scm num)
{ return SCM_INUM(Scm2SCM(num)); }

inline float Scm2Float(Scm scm)
{ return (float) gh_scm2double(Scm2SCM(scm)); }

inline string Scm2String(Scm scm)
{ return string(SCM_STRING_CHARS(Scm2SCM(scm))); }

inline Scm String2Scm(const char *str)
{ return SCM2Scm(gh_str02scm(str)); }

inline Scm Int2Scm(int num)
{ return SCM2Scm(SCM_MAKINUM(num)); }

inline Scm Float2Scm(float val)
{ return SCM2Scm(gh_double2scm(val)); }


int ScmLength(Scm lst);

inline Scm ScmAppend(Scm l1, Scm l2)
{ return SCM2Scm(scm_append(scm_cons(Scm2SCM(l1), 
                              scm_cons(Scm2SCM(l2), SCM_EOL)))); }

string int2string(int num);

inline Scm ScmDefine(const char *name, Scm val)
{ return SCM2Scm(scm_c_define(name, Scm2SCM(val))); }

inline Scm ScmLookup(const char *name)
{ return SCM2Scm(scm_c_lookup(name)); }

inline Scm ScmVariableRef(Scm var)
{ return SCM2Scm(scm_variable_ref(Scm2SCM(var))); }

}

#endif

