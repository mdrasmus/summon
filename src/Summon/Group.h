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
        Element(GROUP_CONSTRUCT),
        m_groupid(groupid)
    {}
    virtual ~Group() {}
    
    virtual Element *Create() {
        return new Group();
    };
    
    virtual bool Build(const Scm &code) {
        SetGroupId((int) this);
        return Element::Build(ScmCdr(code));
    }
    
    inline int GetGroupId() { return m_groupid; }
    inline void SetGroupId(int groupid) { m_groupid = groupid; }
    
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
