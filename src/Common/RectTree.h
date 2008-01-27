/*******************************************************************************
* Summon
* Matt Rasmussen
* RectTree.h
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
*******************************************************************************/

#ifndef SUMMON_RECT_TREE_H
#define SUMMON_RECT_TREE_H

#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include "Vertex.h"


namespace Summon {

using namespace std;


template <class T>
class RectLeaf
{
public:
    RectLeaf(T o, Vertex2f p1, Vertex2f p2) : object(o), pos1(p1), pos2(p2)
    {}

    T object;
    Vertex2f pos1;
    Vertex2f pos2;
};


template <class T>
class RectTree
{
public:
    RectTree()
    {}
    
    virtual ~RectTree()
    {}
    
    typedef T Object;
    typedef list<Object> ObjectList;
    typedef RectLeaf<Object> Leaf;
    typedef list<Leaf> LeafList;
    
    
    inline void Insert(Object object, Vertex2f pos1, Vertex2f pos2)
    {
        // make pos1 less than pos2
        if (pos1.x > pos2.x) swap(pos1.x, pos2.x);
        if (pos1.y > pos2.y) swap(pos1.y, pos2.y);
        
        Insert(Leaf(object, pos1, pos2));
    }
    
    void Insert(Leaf leaf)
    {    
        m_leaves.push_back(leaf);
    }
    
    void Remove(Object object)
    {
        for (typename LeafList::iterator i=m_leaves.begin(); 
             i!=m_leaves.end(); i++)
        {
            if ((*i).object == object) {
                m_leaves.erase(i);
                return;
            }
        }
    }
    
    ObjectList Find(Vertex2f pos) {
        ObjectList objs;
        for (typename LeafList::iterator i=m_leaves.begin(); 
             i!=m_leaves.end(); i++)
        {
            if (pos.x >= (*i).pos1.x && pos.x <= (*i).pos2.x &&
                pos.y >= (*i).pos1.y && pos.y <= (*i).pos2.y)
            {
                objs.push_back((*i).object);
            }
        }
        
        return objs;
    }
    
    
    void Clear()
    {
        for (int i=0; i<m_leaves.size(); i++)
            delete m_leaves[i];
        m_leaves.clear();
    }
        
private:
    
    LeafList m_leaves;
};

}

#endif
