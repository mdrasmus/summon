/***************************************************************************
* Summon
* Matt Rasmussen
* Hotspot.h
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
    Vertex2f GetLocalPos(const Vertex2f &pos, const Camera &camera);
    bool GivePos() { return m_givePos; }
    
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
    bool m_givePos;
};


}

#endif
