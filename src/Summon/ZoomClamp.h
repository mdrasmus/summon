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
        Transform(ZOOM_CLAMP_CONSTRUCT, false),
        m_useAxis(false)
    {
        MakeIdentityMatrix(m_matrix.mat);
    }
    
    virtual Element *Create()
    { return new ZoomClamp(); }
    
    virtual bool IsDynamic() { return true; }
    virtual int GetSpecificId()
    { return m_id; }
        
    
    virtual bool Build(int header, const Scm &code);
    virtual Scm GetContents();

    virtual const TransformMatrix *GetTransform(TransformMatrix *matrix,
                                                const Camera &camera);

    inline bool IsClipped(const Camera &camera) const
    {
        return m_clip && (camera.zoom.x < m_minx || camera.zoom.y < m_miny);
    }

protected:
    float m_minx;
    float m_miny;
    float m_maxx;
    float m_maxy;
    bool m_clip;
    bool m_link;
    bool m_linkType;
    bool m_useAxis;
    Vertex2f m_origin;
    Vertex2f m_axis;
    
    virtual Transform *GetDynamicTransformParent()
    {
        return this;
    }
};


} // namespace Summon

#endif

