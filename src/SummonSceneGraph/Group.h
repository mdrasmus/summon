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
    Group() : 
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


class CustomGroup : public Element
{
public:
    CustomGroup() : 
        Element(CUSTOM_GROUP_CONSTRUCT)
    {
    }
    virtual ~CustomGroup() {}
    
    virtual Element *Create() {
        return new CustomGroup();
    };
    
    virtual bool Build(int header, const Scm &code)
    {
        if (!ScmConsp(code) || code.Size() != 2)
            return false;
        
        m_element = code.GetScm(0);
        return Element::Build(header, code.GetScm(1));
    }
    
    virtual Scm GetContents()
    {    
        return m_element;
    }

    
protected:
    Scm m_element;
};

}

#endif
