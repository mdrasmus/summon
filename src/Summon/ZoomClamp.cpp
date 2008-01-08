/***************************************************************************
* Summon
* Matt Rasmussen
* ZoomClamp.cpp
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


const TransformMatrix *ZoomClamp::GetTransform(TransformMatrix *matrix,
                                               const Camera &camera)
{
    const TransformMatrix *parent = NULL;
    float zoom[3] = { camera.zoom.x, camera.zoom.y, 1.0 };


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
