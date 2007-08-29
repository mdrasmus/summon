/***************************************************************************
* Summon
* Matt Rasmussen
* HashTable.h
*
***************************************************************************/

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <list>
#include <string>
#include <vector>

namespace Summon
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


/*

            ILP32   LP64  LLP64  ILP64
char        8       8     8      8
short       16      16    16     16
int         32      32    32     64
long        32      64    32     64
long long 	64      64    64     64
size_t 	    32      64    64     64
pointer 	32      64    64     64
*/

struct HashPointer {
    static unsigned int Hash(const void *p) { 
        if (sizeof(void*) == 8)
            return ((unsigned long long) p) ^ (((unsigned long long) p) >> 32);
        else
            return (unsigned int) p;
    }
};


/*

struct HashPointer {
    static unsigned int Hash(const void *p) { 
        return (unsigned int) p;
    }
};

struct HashPointer {
    static unsigned int Hash(const void *p) { 
        return *((unsigned int*)p) + *((unsigned int*)p + 1); 
    }
};
*/


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
    
    inline ValueType &Insert(const KeyType &key, const ValueType &object)
    {
        unsigned int hash = HashFunc::Hash(key);
        int index = hash % m_size;
        m_table[index].push_back(NodeType(key, object));
        return m_table[index].back().second;
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
    
    ValueType &operator[](const KeyType &key) {
        unsigned int hash = HashFunc::Hash(key);
        ListType *lst = &m_table[hash % m_size];

        for (typename ListType::iterator i = lst->begin(); i != lst->end(); i++) {
            if ((*i).first == key) {
                return (*i).second;
            }
        }

        // existing key not found, insert new value
        return Insert(key, m_null);
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
