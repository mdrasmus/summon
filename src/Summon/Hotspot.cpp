/***************************************************************************
* Summon
* Matt Rasmussen
* Hotspot.cpp
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
        kindid = CLICK;
    } else if (kindstr == "drag") {
        kindid = DRAG;
        m_givePos = true; // drag always gives back mouse position
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
    } else if (kind == DRAG) {
        skind = (char*) "drag";
    } else {
        assert(0);
    }
    
    return BuildScm("sffffpb", skind, pos1.x, pos1.y, pos2.x, pos2.y, 
                    &GetProc()->proc, m_givePos);
}


bool Hotspot::IsCollide(const Vertex2f &pt, const Camera &camera, int _kind)
{
    // cannot collide if click is wrong kind
    if (kind == CLICK && _kind != CLICK)
        return false;
    if (kind == DRAG) {
        // if hotspot is being dragged then accept these events
        if (_kind == DRAG)
            return m_isDragging;
        
        // if this is a drag_stop event, stop dragging
        if (_kind == DRAG_STOP) {
            bool collide = m_isDragging;
            m_isDragging = false;
            return collide;
        }
        
        // don't process anything else otherthan drag_start
        if (_kind != DRAG_START)
            return false;
    }

    TransformMatrix tmp;
    const TransformMatrix *matrix = GetTransform(&tmp, camera);
    const float *mat = matrix->mat;

    // NOTE: the following is equivalent to:
    //      Vertex2f a, b, c, d;
    //      matrix->VecMult(pos1.x, pos1.y, &a.x, &a.y);
    //      matrix->VecMult(pos2.x, pos2.y, &b.x, &b.y);
    //      matrix->VecMult(pos1.x, pos2.y, &c.x, &c.y);
    //      matrix->VecMult(pos2.x, pos1.y, &d.x, &d.y);
    
    const Vertex2f a(pos1.x*mat[0] + pos1.y*mat[1] + mat[3],
                     pos1.x*mat[4] + pos1.y*mat[5] + mat[7]);
    const Vertex2f b(pos2.x*mat[0] + pos2.y*mat[1] + mat[3],
                     pos2.x*mat[4] + pos2.y*mat[5] + mat[7]);
    const Vertex2f c(pos1.x*mat[0] + pos2.y*mat[1] + mat[3],
                     pos1.x*mat[4] + pos2.y*mat[5] + mat[7]);
    const Vertex2f d(pos2.x*mat[0] + pos1.y*mat[1] + mat[3],
                     pos2.x*mat[4] + pos1.y*mat[5] + mat[7]);

    
    if (InQuad(a, c, b, d, pt)) {
        // begin a drag
        if (kind == DRAG && _kind == DRAG_START)
            m_isDragging = true;
        return true;
    } else
        return false;
}


Vertex2f Hotspot::GetLocalPos(const Vertex2f &pos, const Camera &camera)
{
    TransformMatrix tmp;
    const TransformMatrix *matrix = GetTransform(&tmp, camera);  
    Vertex2f lpos;
    matrix->VecInvMult(pos.x, pos.y, &lpos.x, &lpos.y);
    return lpos;
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

} // namespace Summon
