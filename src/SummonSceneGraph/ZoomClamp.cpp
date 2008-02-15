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
    if (!ParseScm(code, "ffffbbbffff", &m_minx, &m_miny, &m_maxx, &m_maxy, 
                  &m_clip, &m_link, &m_linkType, 
                  &m_origin.x, &m_origin.y, &m_axis.x, &m_axis.y)) 
    {
        Error("Bad format for zoom_clamp construct");
        return false;
    }
    
    m_useAxis = (m_axis != m_origin);
    
    return Element::Build(header, code.Slice(11));
}


Scm ZoomClamp::GetContents()
{
    return BuildScm("ffffbbbffff", m_minx, m_miny, m_maxx, m_maxy, 
                    m_clip, m_link, m_linkType,
                    m_origin.x, m_origin.y, m_axis.x, m_axis.y);
}


bool ZoomClamp::IsClipped(const Camera &camera) const
{
    if (!m_clip) {
        return false;
    } else if (m_transformParent != NULL) {
        TransformMatrix matrix;
        const TransformMatrix *parent;
        Vertex2f prezoom(camera.zoom);
        float x, y;
        
        parent = m_transformParent->GetTransform(&matrix, camera);
        parent->GetScaling(&x, &y);
        prezoom.x *= x;
        prezoom.y *= y;
        
        return prezoom.x < m_minx || prezoom.y < m_miny;
    } else {
        return camera.zoom.x < m_minx || camera.zoom.y < m_miny;
    }
}

const TransformMatrix *ZoomClamp::GetTransform(TransformMatrix *matrix,
                                               const Camera &camera)
{
    const TransformMatrix *parent = NULL;    
    Vertex2f prezoom = camera.zoom;
    
    // determine translation to zoom clamp origin
    float trans[3];
    if (m_transformParent != NULL) {
        parent = m_transformParent->GetTransform(matrix, camera);
        parent->VecMult(m_origin.x, m_origin.y, &trans[0], &trans[1]);
        float x, y;
        parent->GetScaling(&x, &y);
        prezoom.x *= x;
        prezoom.y *= y;
    } else {
        trans[0] = m_origin.x;
        trans[1] = m_origin.y;
    }
    trans[2] = 0.0;    
    
    
    float zoom[3] = { prezoom.x, prezoom.y, 1.0 };

    // determine desired clamped zoom
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
    
    
    // calculate zoom adjustment needed
    zoom[0] /= camera.zoom.x;
    zoom[1] /= camera.zoom.y;



    // apply compensating rotation, if axis is specified
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
        Vertex2f worldVector((wax - wox) * camera.zoom.x, 
                             (way - woy) * camera.zoom.y);
        worldVector.Normalize();
        float worldAngle = vertex2angle(worldVector);
        
        Vertex2f clampVector((m_axis.x - m_origin.x) * zoom[0] * camera.zoom.x,
                             (m_axis.y - m_origin.y) * zoom[1] * camera.zoom.y);
        clampVector.Normalize();
        float clampAngle = vertex2angle(clampVector);                     
        float diffAngle = worldAngle - clampAngle;
        
        TransformMatrix tmp, tmp2, tmp3;
        MakeTransScaleMatrix(trans, zoom, tmp.mat);
        
        
        
        // compensate for origin, if needed
        if (m_origin.x != 0.0 || m_origin.y != 0.0) {            
            MakeRotateMatrix(180.0 / M_PI * diffAngle, tmp2.mat);        
            MultMatrix(tmp.mat, tmp2.mat, tmp3.mat);
            
            trans[0] = - m_origin.x;
            trans[1] = - m_origin.y;
            
            MakeTransMatrix(trans, tmp.mat);
            MultMatrix(tmp3.mat, tmp.mat, matrix->mat);                    
        } else {
            MakeRotateMatrix(180.0 / M_PI * diffAngle, tmp2.mat);        
            MultMatrix(tmp.mat, tmp2.mat, matrix->mat);
        }
        
        
        
    } else {
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