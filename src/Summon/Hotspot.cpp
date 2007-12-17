/***************************************************************************
* Summon
* Matt Rasmussen
* Hotspot.cpp
*
***************************************************************************/

#include "first.h"
#include <assert.h>
#include "Hotspot.h"

namespace Summon {



bool Hotspot::Build(int header, const Scm &code)
{
    //int header;
    int kindid;
    string kindstr;
    Scm procCode;
    
    // parse the scm code for a hotspot
    if (!ParseScm(code,
                  "sffffp", &kindstr, 
                   &pos1.x, &pos1.y, &pos2.x, &pos2.y, &procCode))
    {
        Error("Bad format for hotspot\n");
        return false;
    }

    // parse hotspot kind
    if (kindstr == "click") {
        kindid = Hotspot::CLICK;
    } else {
        Error("Unknown hotspot type '%s'", kindstr.c_str());
        return false;
    }
    
    // parse procedure
    CallProcCommand *proc = new CallProcCommand(procCode);
    if (!proc->defined) {
        delete proc;
        return false;
    }    
    
    // construct Hotspot
    SetProc(proc);
    kind = kindid;
    
    // NOTE: swapping may not be necessary (save it for model book-keeping).
    if (pos1.x > pos2.x)
        swap(pos1.x, pos2.x);
    if (pos1.y > pos2.y)
        swap(pos1.y, pos2.y);
    
    return true;
}


Scm Hotspot::GetContents()
{
    char *skind;

    if (kind == CLICK) {
        skind = (char*) "click";
    } else {
        assert(0);
    }
    
    return Py2ScmTake(Py_BuildValue("sffffO", (char*) skind, 
                                    pos1.x, pos1.y, pos2.x, pos2.y,
                                    Scm2Py(GetProc()->GetScmProc())));
}


} // namespace Summon
