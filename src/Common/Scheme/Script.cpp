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

vector<string> Scheme2Strings(Scm lst)
{
    vector<string> args;

    // convert SCM list into a string array
    for (;SCM_CONSP(lst); lst = SCM_CDR(lst)) {
        SCM var = SCM_CAR(lst);
        SCM str;
        
        if (SCM_STRINGP(var))
            str = var;
        else if (SCM_INUMP(var))
            str = scm_number_to_string(var, SCM_MAKINUM(10));
        else if (scm_real_p(var)) {
            float val = (float) gh_scm2double(var);
            enum { STR_SIZE = 10 };
            char *c_str = (char*) malloc(STR_SIZE + 1);
            snprintf(c_str, STR_SIZE, "%f", val);
            args.push_back(string(c_str));
            free(c_str);
            continue;
        } else
            continue;
        
        size_t len;
        char *c_str = gh_scm2newstr(str, &len);
        args.push_back(string(c_str));
        free(c_str);
    }
    
    return args;
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


int ScmLength(Scm lst)
{
    int len = 0;
    for (; ScmConsp(lst); len++, lst = ScmCdr(lst));
    return len;
}

}
