/***************************************************************************
* Summon
* Matt Rasmussen
* ZoomClamp.h
*
***************************************************************************/

#ifndef ZOOM_CLAMP_H
#define ZOOM_CLAMP_H

#include "Script.h"
#include <algorithm>
#include "types.h"
#include "Element.h"


namespace Summon {



class ZoomClamp : public Element
{
public:
    ZoomClamp() : 
        Element(ZOOM_CLAMP_CONSTRUCT)
    {
    }
    
    
    virtual Element *Create()
    { return new ZoomClamp(); }
    
    virtual bool IsDynamic() { return true; }
    
    virtual bool Build(int header, const Scm &code);
    virtual Scm GetContents();
    
    float minx;
    float miny;
    float maxx;
    float maxy;
    bool clip;
    bool linked;
};


} // namespace Summon

#endif
