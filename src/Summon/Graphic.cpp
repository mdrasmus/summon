/***************************************************************************
* Vistools
* Matt Rasmussen
* Graphic.cpp
*
***************************************************************************/

#include "first.h"
#include "Graphic.h"

namespace Vistools {

using namespace std;


Graphic::Graphic(int id) :
    Element(id)
{
    
}

Graphic::~Graphic()
{
    for (PrimitiveIterator i=PrimitivesBegin(); i!=PrimitivesEnd(); i++) {
        delete (*i);
    }
}

}

