/***************************************************************************
* Summon
* Matt Rasmussen
* Group.h
*
***************************************************************************/

#ifndef GROUP_H
#define GROUP_H

#include "Script.h"
#include "Element.h"

namespace Summon {

using namespace std;



class Group : public Element
{
public:
    Group(int groupid=-1) : 
        Element(GROUP_CONSTRUCT)
    {
    }
    virtual ~Group() {}
    
    virtual Element *Create() {
        return new Group();
    };
};


class DynamicGroup : public Element
{
public:
    DynamicGroup(Scm proc) : 
        Element(DYNAMIC_GROUP_CONSTRUCT),
        m_proc(proc)
    {
    }
    
    virtual bool IsDynamic() { return true; }
    
    inline Scm GetProc() { return m_proc; }
    
protected:
    Scm m_proc;
};

}

#endif
