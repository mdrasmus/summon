/***************************************************************************
* Vistools
* Matt Rasmussen
* Hotspot.h
*
***************************************************************************/

#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Script.h"
#include "types.h"
#include "Element.h"

namespace Vistools {

using namespace std;

class Hotspot : public Element
{
public:
    Hotspot() : Element(HOTSPOT_CONSTRUCT), m_proc(NULL) {}
    virtual ~Hotspot()
    {
        if (m_proc)
            delete m_proc;
    }
    
    inline void SetProc(CallProcCommand *proc)
    { m_proc = proc; }
    
    inline CallProcCommand *GetProc()
    { return m_proc; }

    enum {
        CLICK,
        OVER,
        OUT
    };
    
    int kind;
    Vertex2f pos1;
    Vertex2f pos2;
    
protected:
    CallProcCommand *m_proc;
};


}

#endif
