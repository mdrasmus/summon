/***************************************************************************
* Summon
* Matt Rasmussen
* Factory.h
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
***************************************************************************/

#ifndef SUMMON_FACTORY_H
#define SUMMON_FACTORY_H

#include <assert.h>
#include <vector>
#include <map>
#include <iostream>


namespace Summon
{

using namespace std;

template <class KeyType, class ProductType>
class FactoryArray
{
public:
    FactoryArray(int maxid=3000) :
        m_maxid(maxid)
    {
        m_products = new ProductType* [3000];
        
        for (int i=0; i<maxid; i++)
            m_products[i] = NULL;
    }
    virtual ~FactoryArray() 
    {
        delete [] m_products;
    }
    
    void Register(ProductType *product, const KeyType &id) {
        assert(id < m_maxid);
        m_products[id] = product;
    }
    
    inline ProductType *Create(KeyType id) {
        if (id >= m_maxid)
            return NULL;
        ProductType *ptr = m_products[id];
        if (ptr)
            return ptr->Create();
        else
            return NULL;
    }
    
    
    class Registrator {
    public:
        Registrator(FactoryArray &factory, ProductType *product, KeyType id) {
            factory.Register(product, id);
        }
    };
    
protected:
    int m_maxid;
    ProductType **m_products;
};



template <class KeyType, class ProductType>
class Factory
{
public:
    Factory() {}
    virtual ~Factory() {}
    
    void Register(ProductType *product, const KeyType &id) {
        m_products[id] = product;
    }
    
    inline ProductType *Create(KeyType id) {
        ProductType *ptr = m_products[id];
        if (ptr)
            return ptr->Create();
        else
            return NULL;
    }
    
    
    inline map<KeyType, ProductType*> GetProducts() { 
        return m_products;
    }
    
    
    class Registrator {
    public:
        Registrator(Factory &factory, ProductType *product, KeyType id) {
            factory.Register(product, id);
        }
    };
    
protected:
    map<KeyType, ProductType*> m_products;
};



}

#endif
