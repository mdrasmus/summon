/***************************************************************************
* Summon
* Matt Rasmussen
* ZoomClamp.h
*
***************************************************************************/

#ifndef ZOOM_CLAMP_H
#define ZOOM_CLAMP_H

#include "Script.h"
#include "Transform.h"


namespace Summon {



class ZoomClamp : public Transform
{
public:
    ZoomClamp() : 
        Transform(ZOOM_CLAMP_CONSTRUCT, false)
    {
    }
    
    virtual Element *Create()
    { return new ZoomClamp(); }
    
    virtual bool IsDynamic() { return true; }
    
    virtual bool Build(int header, const Scm &code);
    virtual Scm GetContents();

    virtual const TransformMatrix *GetTransform(TransformMatrix *matrix,
                                                const Camera &camera);

    float minx;
    float miny;
    float maxx;
    float maxy;
    bool clip;
    bool link;
    
protected:
    virtual Transform *GetDynamicTransformParent()
    {
        return this;
    }
};


} // namespace Summon

#endif

