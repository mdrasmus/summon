/***************************************************************************
* Summon
* Matt Rasmussen
* Hotspot.h
*
***************************************************************************/

#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Script.h"
#include "types.h"
#include "Element.h"

namespace Summon {

using namespace std;

class Hotspot : public Element
{
public:
    Hotspot() : 
        Element(HOTSPOT_CONSTRUCT), 
        m_proc(NULL) 
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
    
    bool IsCollide(const Vertex2f &pt, const Camera &camera);
    
    
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
