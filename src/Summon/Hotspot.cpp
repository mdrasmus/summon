/***************************************************************************
* Summon
* Matt Rasmussen
* Hotspot.cpp
*
***************************************************************************/

#include "first.h"
#include <assert.h>
#include <math.h>
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
                  "sffffpb", &kindstr, 
                   &pos1.x, &pos1.y, &pos2.x, &pos2.y, &procCode, &m_givePos))
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
    
    return BuildScm("sffffpb", skind, pos1.x, pos1.y, pos2.x, pos2.y, 
                    &GetProc()->proc, m_givePos);
}


/*
bool Hotspot::IsCollide(const Vertex2f &pt, const Camera &camera)
{
    TransformMatrix tmp;
    const TransformMatrix *matrix = GetTransform(&tmp, camera);
    
    Vertex2f a, b;
    matrix->VecMult(pos1.x, pos1.y, &a.x, &a.y);
    matrix->VecMult(pos2.x, pos2.y, &b.x, &b.y);
    
    if (a.x > b.x)
        swap(a.x, b.x);
    if (a.y > b.y)
        swap(a.y, b.y);

    return (pt.x >= a.x && pt.x <= b.x &&
            pt.y >= a.y && pt.y <= b.y);
}*/


bool Hotspot::IsCollide(const Vertex2f &pt, const Camera &camera)
{
    TransformMatrix tmp;
    const TransformMatrix *matrix = GetTransform(&tmp, camera);
    
    Vertex2f a, b, c, d;
    matrix->VecMult(pos1.x, pos1.y, &a.x, &a.y);
    matrix->VecMult(pos2.x, pos2.y, &b.x, &b.y);
    matrix->VecMult(pos1.x, pos2.y, &c.x, &c.y);
    matrix->VecMult(pos2.x, pos1.y, &d.x, &d.y);
    
    return InQuad(a, c, b, d, pt);
}


Vertex2f Hotspot::GetLocalPos(const Vertex2f &pos, const Camera &camera)
{
    TransformMatrix tmp;
    const TransformMatrix *matrix = GetTransform(&tmp, camera);  
    Vertex2f lpos;
    matrix->VecInvMult(pos.x, pos.y, &lpos.x, &lpos.y);
    return lpos;
}


} // namespace Summon
