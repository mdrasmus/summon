/***************************************************************************
* Vistools
* Matt Rasmussen
* GroupTable.h
*
***************************************************************************/

#ifndef GROUP_TABLE_H
#define GROUP_TABLE_H

#include "Script.h"
#include "Group.h"
#include "HashTable.h"


namespace Vistools {

using namespace std;

class GroupTable
{
public:
    GroupTable();
    virtual ~GroupTable();
    
    void MakeRoot();
    void AddGroup(int parent, Group *group);
    void RemoveGroup(int group);
    void ReplaceGroup(int id, Group *group);
    void RemoveTree(Element *elm);
    
    inline void AddGroup(Group *group)
    { m_table.Insert(group->GetGroupId(), group); }
    inline Group* GetGroup(int id) { return m_table.Get(id); }
        
    inline void Clear()
    { RemoveGroup(m_root); }

    inline Group *GetRootGroup() { GetGroup(m_root); }
    inline int GetRoot() { return m_root; }
    inline int SetRoot(int id) { m_root = id; }
    
protected:    
    
    enum { TABLE_SIZE = 2003 };
    
    int m_root;
    HashTable<int, Group*, HashInt> m_table;
    
};

}

#endif
