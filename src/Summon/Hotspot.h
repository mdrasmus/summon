/***************************************************************************
* Summon
* Matt Rasmussen
* Hotspot.h
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

#ifndef SUMMON_HOTSPOT_H
#define SUMMON_HOTSPOT_H

#include "Script.h"
#include "Element.h"

namespace Summon {

using namespace std;

class Hotspot : public Element
{
public:
    Hotspot() : 
        Element(HOTSPOT_CONSTRUCT), 
        m_proc(NULL),
        m_givePos(false),
        m_isDragging(false)
    {}
    
    virtual ~Hotspot()
    {
        if (m_proc)
            delete m_proc;
    }
    
    virtual Element* Create()
    { return new Hotspot(); }
    
    virtual bool Build(int header, const Scm &code);
    virtual Scm GetContents();
    
    inline void SetProc(CallProcCommand *proc)
    { m_proc = proc; }
    
    inline CallProcCommand *GetProc()
    { return m_proc; }
    
    bool IsCollide(const Vertex2f &pt, const Camera &camera, int kind);
    Vertex2f GetLocalPos(const Vertex2f &pos, const Camera &camera);
    bool GivePos() { return m_givePos; }
    bool GiveKind() { return kind == DRAG; }
    
    enum {
        CLICK,
        MOUSE_OVER,
        MOUSE_OUT,
        DRAG,
        DRAG_START,
        DRAG_STOP
    };
    
    int kind;
    Vertex2f pos1;
    Vertex2f pos2;
    
    
protected:
    CallProcCommand *m_proc;
    bool m_givePos;
    bool m_isDragging;
};


}

#endif
