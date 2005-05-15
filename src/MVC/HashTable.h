/***************************************************************************
* Vistools
* Matt Rasmussen
* HashTable.h
*
***************************************************************************/

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <list>
#include <string>
#include <vector>

namespace Vistools
{

using namespace std;


struct HashInt {
    static unsigned int Hash(const int &n) { return (unsigned) n; }
};

struct HashCharStar {
    static unsigned int Hash(const char *s) 
    {
        unsigned int n = 0;
        
        for (; *s; s++)
            n += *s;
        return n; 
    }
};

struct HashString {
    static unsigned int Hash(string s) 
    {
        return HashCharStar::Hash(s.c_str());
    }
};




template <class KeyType, class ValueType, class HashFunc>
class HashTable
{
public:
    HashTable(int size = 20, ValueType null = ValueType()) :
        m_table(NULL),
        m_null(null)
    {
        if (size > 0)
            Create(size);
    }
    
    virtual ~HashTable()
    {
        if (m_table)
            delete [] m_table;
    }
    
    void Create(int size)
    {
        m_size = size;
        m_table = new ListType [size];
    }
    
    void Insert(const KeyType &key, const ValueType &object)
    {
        unsigned int hash = HashFunc::Hash(key);
        m_table[hash % m_size].push_back(NodeType(key, object));
    }
    
    void Remove(const KeyType &key)
    {
        unsigned int hash = HashFunc::Hash(key);
        ListType *lst  = &m_table[hash % m_size];
        
        for (typename ListType::iterator i = lst->begin(); 
             i != lst->end(); i++)
        {
            if ((*i).first == key) {
                lst->erase(i);
                return;
            }
        }
    }
    
    ValueType Get(const KeyType &key) {
        unsigned int hash = HashFunc::Hash(key);
        ListType *lst = &m_table[hash % m_size];
        
        for (typename ListType::iterator i = lst->begin(); i != lst->end(); i++) {
            if ((*i).first == key) {
                return (*i).second;
            }
        }
        
        return m_null;
    }
    
    bool HasKey(const KeyType &key)
    {
        unsigned int hash = HashFunc::Hash(key);
        ListType *lst = &m_table[hash % m_size];
        
        for (typename ListType::iterator i = lst->begin(); i != lst->end(); i++) {
            if ((*i).first == key) {
                return true;
            }
        }
        
        return false;
    }
    
    void Keys(vector<KeyType> *keys)
    {
        keys->clear();
        for (int l=0; l<m_size; l++) {
            ListType *lst = &m_table[l];
        
            for (typename ListType::iterator i = lst->begin(); 
                 i != lst->end(); i++) 
            {
                keys->push_back((*i).first);
            }
        }
    }
    
    
protected:
    typedef pair<KeyType, ValueType> NodeType;
    typedef list< NodeType > ListType;
    ListType *m_table;
    int m_size;
    ValueType m_null;
};

}

#endif
