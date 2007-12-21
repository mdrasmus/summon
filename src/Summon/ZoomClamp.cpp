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
    if (!ParseScm(code, "ffffbb", &minx, &miny, &maxx, &maxy, &clip, &link)) {
        Error("Bad format for zoom_clamp construct");
        return false;
    }
    
    return Element::Build(header, code.Slice(6));
}


Scm ZoomClamp::GetContents()
{
    return BuildScm("ffffbb", minx, miny, maxx, maxy, clip, link);
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
    
    // if axises are linked, use larger one
    if (link) {
        if (zoom[0] > zoom[1])
            zoom[1] = zoom[0];
        else
            zoom[0] = zoom[1];
    }
    
    
    // calculate zoom adjustment needed
    zoom[0] /= camera.zoom.x;
    zoom[1] /= camera.zoom.y;

    // determine translation to zoom clamp origin
    float trans[3] = { 0.0, 0.0, 0.0 };    
    if (m_transformParent != NULL) {
        const TransformMatrix *parent = m_transformParent->GetTransform(matrix, camera);
        parent->VecMult(0, 0, &trans[0], &trans[1]);
    }
    
    // translate to zoom_clamp origin
    float tmp[16];
    MakeTransMatrix(trans, tmp);

    // perform zoom adjustment according to clamping
    float tmp2[16];
    MakeScaleMatrix(zoom, tmp2);
    MultMatrix(tmp, tmp2, matrix->mat);
        
    return matrix;
}

} // namespace Summon
