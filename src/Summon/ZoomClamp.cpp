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
    if (!ParseScm(code, "ffffbbbff", &minx, &miny, &maxx, &maxy, &clip, &link,
                         &linkType, &origin.x, &origin.y)) {
        Error("Bad format for zoom_clamp construct");
        return false;
    }
    
    return Element::Build(header, code.Slice(9));
}


Scm ZoomClamp::GetContents()
{
    return BuildScm("ffffbbbff", minx, miny, maxx, maxy, clip, link, linkType,
                                 origin.x, origin.y);
}


const TransformMatrix *ZoomClamp::GetTransform(TransformMatrix *matrix,
                                               const Camera &camera)
{
    float zoom[3] = { camera.zoom.x, camera.zoom.y, 1.0 };

    // determine desired clamped zoom
    if (zoom[0] < minx) zoom[0] = minx;
    if (zoom[1] < miny) zoom[1] = miny;
    if (zoom[0] > maxx) zoom[0] = maxx;
    if (zoom[1] > maxy) zoom[1] = maxy;
    
    // set zooms equal to each other if axises are linked
    if (link) {
        // if link_type == true, use larger of two zooms
        // if link_type == false, use smaller of two zooms
        if (linkType == (zoom[0] > zoom[1]))
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
        const TransformMatrix *parent = m_transformParent->GetTransform(matrix, camera);
        parent->VecMult(origin.x, origin.y, &trans[0], &trans[1]);
    } else {
        trans[0] = origin.x;
        trans[1] = origin.y;
    }
    trans[2] = 0.0;

    
    if (origin.x != 0.0 || origin.y != 0.0) {
        // apply compensating translate for origin        
        trans[0] -= origin.x * zoom[0];
        trans[1] -= origin.y * zoom[1];
    }

    // apply translate followed by scaling
    MakeTransScaleMatrix(trans, zoom, matrix->mat);
    
    return matrix;
}

} // namespace Summon
