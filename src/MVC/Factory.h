/***************************************************************************
* Vistools
* Matt Rasmussen
* Factory.h
*
***************************************************************************/

#ifndef FACTORY_H
#define FACTORY_H

#include <assert.h>
#include <vector>
#include <map>
#include <iostream>

namespace Vistools
{

using namespace std;

template <class KeyType, class ProductType>
class Factory
{
public:
    Factory() {}
    virtual ~Factory() {}
    
    void Register(ProductType *product, KeyType &id) {
        m_products[id] = product;
    }
    
    inline ProductType *Create(KeyType id) {
        ProductType *ptr = m_products[id];
        assert(ptr != NULL);
        return ptr->Create();
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
