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
    Group(int groupid) : 
        Element(GROUP_CONSTRUCT),
        m_groupid(groupid)
    {}
    virtual ~Group() {}
    
    inline int GetGroupId() { return m_groupid; }
    
protected:    
    int m_groupid;
};


class DynamicGroup : public Element
{
public:
    DynamicGroup(Scm proc) : 
        Element(DYNAMIC_GROUP_CONSTRUCT),
        m_proc(proc)
    {
        m_dynamic = true;
    }
    
    inline Scm GetProc() { return m_proc; }
    
protected:
    Scm m_proc;
};

}

#endif
