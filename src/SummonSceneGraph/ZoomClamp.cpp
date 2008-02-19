/***************************************************************************
* Summon
* Matt Rasmussen
* ZoomClamp.cpp
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

#include "first.h"
#include <assert.h>
#include "ZoomClamp.h"

namespace Summon {



bool ZoomClamp::Build(int header, const Scm &code)
{
    if (!ParseScm(code, "ffffbbbffffff", &m_minx, &m_miny, &m_maxx, &m_maxy, 
                  &m_clip, &m_link, &m_linkType, 
                  &m_origin.x, &m_origin.y, &m_axis.x, &m_axis.y,
                  &m_prezoom.x, &m_prezoom.y)) 
    {
        Error("Bad format for zoom_clamp construct");
        return false;
    }
    
    m_useAxis = (m_axis != m_origin);
    
    return Element::Build(header, code.Slice(13));
}


Scm ZoomClamp::GetContents()
{
    return BuildScm("ffffbbbffffff", m_minx, m_miny, m_maxx, m_maxy, 
                    m_clip, m_link, m_linkType,
                    m_origin.x, m_origin.y, m_axis.x, m_axis.y,
                    m_prezoom.x, m_prezoom.y);
}


bool ZoomClamp::IsClipped(const Camera &camera) const
{
    if (!m_clip) {
        return false;
    } else {
        TransformMatrix matrix;
        const TransformMatrix *parent;
        if (m_transformParent != NULL)
            parent = m_transformParent->GetTransform(&matrix, camera);
        Vertex2f cameraZoom = ComputeEffectiveZoom(parent, camera.zoom);
        
        // calculate prezoom
        Vertex2f prezoom;
        if (parent) {
            parent->GetScaling(&prezoom.x, &prezoom.y);
            prezoom.x *= m_prezoom.x;
            prezoom.y *= m_prezoom.y;
        } else
            prezoom = m_prezoom;
            
        
        return prezoom.x * cameraZoom.x < m_minx || 
               prezoom.y * cameraZoom.y < m_miny;
    }
}


float ZoomClamp::ComputeWorldAngle(const TransformMatrix *parent,
                                   const Vertex2f &cameraZoom) const
{
    if (m_useAxis) {
        float wox, woy, wax, way;
        if (parent) {        
            parent->VecMult(m_origin.x, m_origin.y, &wox, &woy);
            parent->VecMult(m_axis.x, m_axis.y, &wax, &way);
        } else {
            wox = m_origin.x;
            woy = m_origin.y;
            wax = m_axis.x;
            way = m_axis.y;
        }
        Vertex2f worldVector((wax - wox) * cameraZoom.x, 
                             (way - woy) * cameraZoom.y);
        worldVector.Normalize();
        return Vertex2Angle(worldVector);
    } else
        return 0.0;
}

Vertex2f ZoomClamp::ComputeEffectiveZoom(const TransformMatrix *parent, 
                                   const Vertex2f &cameraZoom) const
{
    TransformMatrix tmp, tmp2;
    Vertex2f cameraZoom2;
    
    static const Vertex2f zoom1(1.0, 1.0);
    float worldAngle;
    if (parent)
        worldAngle = ComputeWorldAngle(parent, zoom1);
    else
        worldAngle = 0.0;
    
    if (m_useAxis) {
        Vertex2f preclampVector((m_axis.x - m_origin.x), (m_axis.y - m_origin.y));
        preclampVector.Normalize();
        float preclampAngle = Vertex2Angle(preclampVector);
        float prerotate = worldAngle - preclampAngle;

        float zoom[3] = { cameraZoom.x, cameraZoom.y, 1.0 };
        MakeScaleMatrix(zoom, tmp.mat);
        MultRotateMatrix(tmp.mat, 180.0 / M_PI * prerotate, tmp2.mat);
        tmp2.GetScaling(&cameraZoom2.x, &cameraZoom2.y);
    } else {
        cameraZoom2 = cameraZoom;
    }
    
    return cameraZoom2;
}


/*

    translate(camera_trans,
        scale(camera_scale,
            
            translate(parent_trans + origin,
                scale(1/camera_scale,
                    rotate(rot,
                        scale(zoom_clamp(prezoom(parent_rot, camera_scale, parent_scale)),
                            translate(-origin, 
                                contents)))))))

    prezoom(parent_rot, camera_scale, parent_scale) =
        parent_scale * scale(camera_scale,
                           rotate(parent_rot)).get_scaling()

*/


const TransformMatrix *ZoomClamp::GetTransform(TransformMatrix *matrix,
                                               const Camera &camera)
{
    const TransformMatrix *parent = NULL;
    TransformMatrix tmp, tmp2;
    
    // determine translation to zoom clamp origin
    float trans[3];
    if (m_transformParent != NULL) {
        parent = m_transformParent->GetTransform(matrix, camera);
        parent->VecMult(m_origin.x, m_origin.y, &trans[0], &trans[1]);
        
    } else {
        trans[0] = m_origin.x;
        trans[1] = m_origin.y;
    }
    trans[2] = 0.0;

    // calculate prezoom
    Vertex2f prezoom;
    if (parent) {
        parent->GetScaling(&prezoom.x, &prezoom.y);
        prezoom.x *= m_prezoom.x;
        prezoom.y *= m_prezoom.y;
    } else
        prezoom = m_prezoom;
    
    // compute camerazoom
    Vertex2f cameraZoom = ComputeEffectiveZoom(parent, camera.zoom);

    // test for clipping
    if (m_clip && (prezoom.x * cameraZoom.x < m_minx || 
                   prezoom.y * cameraZoom.y < m_miny))
    {
        return NULL;
    }

    // determine desired clamped zoom    
    float zoom[3] = { prezoom.x * cameraZoom.x, 
                      prezoom.y * cameraZoom.y, 1.0 };

    if (zoom[0] < m_minx) zoom[0] = m_minx;
    if (zoom[1] < m_miny) zoom[1] = m_miny;
    if (zoom[0] > m_maxx) zoom[0] = m_maxx;
    if (zoom[1] > m_maxy) zoom[1] = m_maxy;
    
    // set zooms equal to each other if axises are linked
    if (m_link) {
        // if link_type == true, use larger of two zooms
        // if link_type == false, use smaller of two zooms
        if (m_linkType == (zoom[0] > zoom[1]))
            zoom[1] = zoom[0];
        else
            zoom[0] = zoom[1];
    }
    
    
    // apply compensating rotation, if axis is specified
    if (m_useAxis) {
        float worldAngle = ComputeWorldAngle(parent, camera.zoom);
        Vertex2f clampVector((m_axis.x - m_origin.x) * zoom[0],
                             (m_axis.y - m_origin.y) * zoom[1]);
        clampVector.Normalize();
        float clampAngle = Vertex2Angle(clampVector);                     
        float diffAngle = worldAngle - clampAngle;
        
        float zoom2[3] = { 1.0 / camera.zoom.x, 1.0 / camera.zoom.y, 1.0 };
        MakeTransMatrix(trans, tmp.mat);        
        MultScaleMatrix(tmp.mat, zoom2, tmp2.mat);
        MultRotateMatrix(tmp2.mat, 180.0 / M_PI * diffAngle, tmp.mat);
        
        // compensate for origin, if needed
        if (m_origin.x != 0.0 || m_origin.y != 0.0) {            
            MultScaleMatrix(tmp.mat, zoom, tmp2.mat);
            trans[0] = -m_origin.x;
            trans[1] = -m_origin.y;
            MultTransMatrix(tmp2.mat, trans, matrix->mat);
        } else {
            MultScaleMatrix(tmp.mat, zoom, matrix->mat);
        }
        
    } else {
        zoom[0] /= cameraZoom.x;
        zoom[1] /= cameraZoom.y;
    
        // apply compensating translate for origin, if specified
        if (m_origin.x != 0.0 || m_origin.y != 0.0) {
            trans[0] -= m_origin.x * zoom[0];
            trans[1] -= m_origin.y * zoom[1];
        }    
    
        // apply translate followed by scaling
        MakeTransScaleMatrix(trans, zoom, matrix->mat);
    }
    
    return matrix;
}

} // namespace Summon
