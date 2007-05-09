/***************************************************************************
* Vistools
* Matt Rasmussen
* TransformMatrix.cpp
*
***************************************************************************/

#include "first.h"
#include "Element.h"


namespace Vistools {


void Element::FindBounding(float *top, float *bottom, 
                           float *left, float *right,
                           TransformMatrix *matrix)
{
    // loop through children of this element
    for (Element::Iterator i=Begin(); i!=End(); i++) {
        (*i)->FindBounding(top, bottom, left, right, matrix);
    }
}



} // namespace Vistools
