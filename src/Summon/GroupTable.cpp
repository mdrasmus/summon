/***************************************************************************
* Summon
* Matt Rasmussen
* GroupTable.cpp
*
***************************************************************************/

#include "first.h"
#include "GroupTable.h"


namespace Summon {

using namespace std;


GroupTable::GroupTable() :
    m_root(0),
    m_table(TABLE_SIZE, NULL)
{
    // initialize table with root
    MakeRoot();
}

GroupTable::~GroupTable()
{}


void GroupTable::MakeRoot()
{
    m_table.Insert(m_root, new Group(m_root)); 
}

void GroupTable::AddGroup(int parent, Group *group)
{    
    Group* pgroup = GetGroup(parent);
    
    if (pgroup) {
        pgroup->AddChild(group);
        m_table.Insert(group->GetGroupId(), group);
    }
}

void GroupTable::RemoveGroup(int groupid)
{
    Group *group = GetGroup(groupid);
    
    if (group) {
        Element *parent = group->GetParent();
        
        // notify parent
        if (parent) {
            parent->RemoveChild(group);
        }
        
        // remove from table
        RemoveTree(group);
        
        delete group;
        
        // make sure table always has a root
        if (groupid == m_root) {
            MakeRoot();
        }
    }
}


void GroupTable::RemoveTree(Element *elm)
{
    // remove if group
    if (elm->GetId() == GROUP_CONSTRUCT) {
        m_table.Remove(((Group*)elm)->GetGroupId());
        assert(GetGroup(((Group*)elm)->GetGroupId()) == NULL);
    }
    
    // recurse
    for (Element::Iterator i=elm->Begin(); i!=elm->End(); i++)
        RemoveTree(*i);
}



void GroupTable::ReplaceGroup(int id, Group *group)
{
    Group *old = GetGroup(id);
    
    if (old) {
        Element *parent = old->GetParent();
        
        if (parent) {
            Element::Iterator elm = parent->GetChild(old);
            
            // replace old child with new child
            *elm = group;
            group->SetParent(parent);
            m_table.Insert(group->GetGroupId(), group);
            
            // detach old from parent and table
            old->SetParent(NULL);
            m_table.Remove(id);
        }
    }
}


}
