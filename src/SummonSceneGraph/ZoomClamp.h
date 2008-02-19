/***************************************************************************
* Summon
* Matt Rasmussen
* ZoomClamp.h
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

#ifndef SUMMON_ZOOM_CLAMP_H
#define SUMMON_ZOOM_CLAMP_H

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

    // TODO: needs to include prescale
    bool IsClipped(const Camera &camera) const;

protected:
    inline float ComputeWorldAngle(const TransformMatrix *parent,
                                   const Vertex2f &cameraZoom) const;
    inline Vertex2f ComputeEffectiveZoom(const TransformMatrix *parent,
                                         const Vertex2f &cameraZoom) const;

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
    Vertex2f m_prezoom;
    
    virtual Transform *GetDynamicTransformParent()
    {
        return this;
    }
};


} // namespace Summon

#endif

